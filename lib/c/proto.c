/*
 * Copyright 2009 Erik Gilling
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stddef.h>
#include <stdint.h>

#include <proto.h>
#include <crc8.h>

void proto_init(struct proto *p, uint8_t addr)
{
	p->addr = addr;
	p->state = PROTO_STATE_IDLE;
}

void proto_packet_seal(struct proto_packet *packet)
{
	packet->sof = PROTO_SOF;
	packet->crc = crc8_start();
	packet->crc = crc8_calc(packet->crc, packet->addr);
	packet->crc = crc8_calc(packet->crc, packet->cmd);
	packet->crc = crc8_calc(packet->crc, packet->val);
	packet->crc = crc8_end(packet->crc);
	packet->eof = PROTO_EOF;
}

static void proto_set_relay(struct proto *p, uint8_t mask)
{
	uint8_t i;

	for (i = 0; i < p->n_widgets; i++) {
		struct proto_widget *w = &p->widgets[i];

		if (w->type == PROTO_WIDGET_RELAY && ((1 << w->idx) & mask)) {
			w->counter = w->timeout;
			if (p->handlers->relay)
				p->handlers->relay(p->handler_data, w->idx,1);
		}
	}
}

static void proto_clear_relay(struct proto *p, uint8_t mask)
{
	uint8_t i;

	for (i = 0; i < p->n_widgets; i++) {
		struct proto_widget *w = &p->widgets[i];

		if (w->type == PROTO_WIDGET_RELAY && ((1 << w->idx) & mask)) {
			w->counter = 0;
			if (p->handlers->relay)
				p->handlers->relay(p->handler_data, w->idx,0);
		}
	}
}

static void proto_set_light(struct proto *p, uint8_t idx, uint8_t val)
{
	uint8_t i;

	for (i = 0; i < p->n_widgets; i++) {
		struct proto_widget *w = &p->widgets[i];

		if (w->type == PROTO_WIDGET_LIGHT && (w->idx == idx)) {
			if( p->handlers->light )
				p->handlers->light(p->handler_data, idx, val);
		}
	}
}

static void proto_handle_packet(struct proto *p)
{
	struct proto_packet *pkt = &p->packet;

	if (pkt->cmd == PROTO_CMD_RELAY_SET)
		proto_set_relay(p,pkt->val);
	else if (pkt->cmd == PROTO_CMD_RELAY_CLEAR)
		proto_clear_relay(p,pkt->val);
	else if (pkt->cmd >= PROTO_CMD_LIGHT0_SET &&
		 pkt->cmd <= PROTO_CMD_LIGHTF_SET)
		proto_set_light(p, pkt->cmd - PROTO_CMD_LIGHT0_SET, pkt->val);
}


void proto_recv(struct proto *p, uint8_t data)
{
	switch (p->state) {
	case PROTO_STATE_IDLE:
		if (data == PROTO_SOF) {
			p->crc = crc8_start();
			p->state = PROTO_STATE_ADDR;
		}
		break;

	case PROTO_STATE_ADDR:
		p->crc = crc8_calc(p->crc, data);
		p->packet.addr = data;
		p->state = PROTO_STATE_CMD;
		break;

	case PROTO_STATE_CMD:
		p->crc = crc8_calc(p->crc, data);
		p->packet.cmd = data;
		p->state = PROTO_STATE_VAL;
		break;

	case PROTO_STATE_VAL:
		p->crc = crc8_calc(p->crc, data);
		p->packet.val = data;
		p->state = PROTO_STATE_CRC;
		break;

	case PROTO_STATE_CRC:
		p->crc = crc8_end(p->crc);
		p->packet.crc = data;
		p->state = PROTO_STATE_EOF;
		break;

	case PROTO_STATE_EOF:
		if (data == PROTO_EOF && p->crc == p->packet.crc)
			proto_handle_packet(p);

		p->state = PROTO_STATE_IDLE;
		break;
	}
}

void proto_ping(struct proto *p)
{
	uint8_t i;

	for (i = 0; i < p->n_widgets; i++) {
		struct proto_widget *w = &p->widgets[i];

		if (w->type == PROTO_WIDGET_RELAY && w->counter > 0) {
			w->counter--;
			if (w->counter == 0 && p->handlers->relay) {
				p->handlers->relay(p->handler_data, w->idx, 0);
			}
		}
	}
}

