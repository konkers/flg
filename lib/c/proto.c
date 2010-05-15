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

static void proto_packet_send(struct proto *p)
{
	proto_packet_seal(&p->packet);
	if (p->handlers->send)
		p->handlers->send(p->handler_data, (uint8_t *)&p->packet,
				  sizeof(p->packet));
}


static void proto_set_relay(struct proto *p, uint8_t mask)
{
	uint8_t i;

	for (i = 0; i < p->n_widgets; i++) {
		struct proto_widget *w = &p->widgets[i];

		if (w->type == PROTO_WIDGET_TYPE_RELAY && ((1 << w->idx) & mask)) {
			w->relay.counter = w->relay.timeout;
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

		if (w->type == PROTO_WIDGET_TYPE_RELAY && ((1 << w->idx) & mask)) {
			w->relay.counter = 0;
			if (p->handlers->relay)
				p->handlers->relay(p->handler_data, w->idx,0);
		}
	}
}

static void proto_update_relay(struct proto *p, uint8_t mask)
{
	uint8_t i;

	for (i = 0; i < p->n_widgets; i++) {
		struct proto_widget *w = &p->widgets[i];

		if (w->type == PROTO_WIDGET_TYPE_RELAY) {
			if (p->handlers->relay) {
				if((1 << w->idx) & mask) {
					w->relay.counter = w->relay.timeout;
					p->handlers->relay(p->handler_data, w->idx,1);
				} else {
					w->relay.counter = 0;
					p->handlers->relay(p->handler_data, w->idx,0);
				}
			}
		}
	}
}

static void proto_set_light(struct proto *p, uint8_t idx, uint8_t val)
{
	uint8_t i;

	for (i = 0; i < p->n_widgets; i++) {
		struct proto_widget *w = &p->widgets[i];

		if (w->type == PROTO_WIDGET_TYPE_LIGHT && (w->idx == idx)) {
			if( p->handlers->light )
				p->handlers->light(p->handler_data, idx, val);
		}
	}
}

static void proto_query_switch(struct proto *p, uint8_t idx)
{
	uint8_t i;

	for (i = 0; i < p->n_widgets; i++) {
		struct proto_widget *w = &p->widgets[i];
		if (w->type == PROTO_WIDGET_TYPE_SWITCH && (w->idx == idx)) {
			struct proto_packet *pkt = &p->packet;
			pkt->addr = 0;
			pkt->cmd = PROTO_CMD_SWITCH_QUERY;
			pkt->val = w->sw.state;
			proto_packet_send(p);
		}
	}
}

static void proto_query_adc(struct proto *p, uint8_t idx, uint8_t byte)
{
	uint8_t i;

	for (i = 0; i < p->n_widgets; i++) {
		struct proto_widget *w = &p->widgets[i];
		if (w->type == PROTO_WIDGET_TYPE_ADC && (w->idx == idx)) {
			struct proto_packet *pkt = &p->packet;
			if (byte) {
				pkt->addr = 0;
				pkt->cmd = PROTO_CMD_ADC_QUERY_HI;
				pkt->val = w->adc.val >> 8;
				proto_packet_send(p);
			} else {
				pkt->addr = 0;
				pkt->cmd = PROTO_CMD_ADC_QUERY_LO;
				pkt->val = w->adc.val & 0xff;
				proto_packet_send(p);
			}
		}
	}
}

static void proto_set_addr(struct proto *p, uint8_t addr)
{
	if (p->handlers->set_addr)
		p->handlers->set_addr(p->handler_data, addr);
}

static void proto_get_status(struct proto *p)
{
	struct proto_packet *pkt = &p->packet;
	pkt->addr = 0;
	pkt->cmd = PROTO_CMD_GET_STATUS;
	pkt->val = p->status;
	proto_packet_send(p);
}

static void proto_set_dpot(struct proto *p, uint8_t idx, uint8_t val)
{
	uint8_t i;

	for (i = 0; i < p->n_widgets; i++) {
		struct proto_widget *w = &p->widgets[i];

		if (w->type == PROTO_WIDGET_TYPE_DPOT && (w->idx == idx)) {
			if( p->handlers->dpot )
				p->handlers->dpot(p->handler_data, idx, val);
		}
	}
}

static void proto_handle_packet(struct proto *p)
{
	struct proto_packet *pkt = &p->packet;

	if (pkt->addr != PROTO_ADDR_BCAST && pkt->addr != p->addr)
		return;

	if (pkt->addr == PROTO_ADDR_MASTER &&
	    p->handlers->resp &&
	    (pkt->cmd == PROTO_CMD_SWITCH_QUERY ||
	     pkt->cmd == PROTO_CMD_ADC_QUERY_LO ||
	     pkt->cmd == PROTO_CMD_ADC_QUERY_HI ||
	     pkt->cmd == PROTO_CMD_GET_STATUS))
		p->handlers->resp(p->handler_data, pkt);
	else if (pkt->cmd == PROTO_CMD_RELAY_SET)
		proto_set_relay(p,pkt->val);
	else if (pkt->cmd == PROTO_CMD_RELAY_CLEAR)
		proto_clear_relay(p,pkt->val);
	else if (pkt->cmd == PROTO_CMD_RELAY_UPDATE)
		proto_update_relay(p,pkt->val);
	else if (pkt->cmd >= PROTO_CMD_LIGHT0_SET &&
		 pkt->cmd <= PROTO_CMD_LIGHTF_SET)
		proto_set_light(p, pkt->cmd - PROTO_CMD_LIGHT0_SET, pkt->val);
	else if (pkt->cmd == PROTO_CMD_SWITCH_QUERY)
		proto_query_switch(p, pkt->val);
	else if (pkt->cmd == PROTO_CMD_ADC_QUERY_LO)
		proto_query_adc(p, pkt->val, 0);
	else if (pkt->cmd == PROTO_CMD_ADC_QUERY_HI)
		proto_query_adc(p, pkt->val, 1);
	else if (pkt->cmd == PROTO_CMD_SET_ADDR)
		proto_set_addr(p, pkt->val);
	else if (pkt->cmd == PROTO_CMD_GET_STATUS)
		proto_get_status(p);
	else if (pkt->cmd >= PROTO_CMD_DPOT0_SET &&
		 pkt->cmd <= PROTO_CMD_DPOT1_SET)
		proto_set_dpot(p, pkt->cmd - PROTO_CMD_DPOT0_SET, pkt->val);
	else if (pkt->cmd == PROTO_CMD_SYNC && p->handlers->sync)
		p->handlers->sync(p->handler_data);

}


void proto_recv(struct proto *p, uint8_t data)
{
	switch (p->state) {
	case PROTO_STATE_IDLE:
		if (data == PROTO_SOF) {
			p->crc = crc8_start();
			p->state = PROTO_STATE_ADDR;
			p->packet.sof = data;
		} else if (data == PROTO_SOF_LONG) {
			p->state = PROTO_STATE_ADDR;
			p->packet.sof = data;
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
		if (p->packet.sof == PROTO_SOF) {
			p->state = PROTO_STATE_VAL;
		} else if (p->packet.sof == PROTO_SOF_LONG) {
			p->state = PROTO_STATE_LEN;
		}
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

	case PROTO_STATE_LEN:
		p->packet.val = data;
		p->state = PROTO_STATE_LONG_DATA0;
		break;

	case PROTO_STATE_LONG_DATA0:
		p->long_val =(uint32_t) data;
		p->state = PROTO_STATE_LONG_DATA1;
		break;

	case PROTO_STATE_LONG_DATA1:
		p->long_val |= (uint32_t)data << 8;
		p->state = PROTO_STATE_LONG_DATA2;
		break;

	case PROTO_STATE_LONG_DATA2:
		p->long_val |= (uint32_t)data << 16;
		p->state = PROTO_STATE_LONG_DATA3;
		break;

	case PROTO_STATE_LONG_DATA3:
		p->long_val |= (uint32_t)data << 24;

		if (p->packet.addr == p->addr && p->handlers->long_data)
			p->handlers->long_data(p->handler_data, p->long_val);

		p->packet.val--;
		p->packet.addr++;
		if (p->packet.val == 0)
			p->state = PROTO_STATE_EOF;
		else
			p->state = PROTO_STATE_LONG_DATA0;
		break;
	}
}

void proto_ping(struct proto *p)
{
	uint8_t i;

	for (i = 0; i < p->n_widgets; i++) {
		struct proto_widget *w = &p->widgets[i];

		if (w->type == PROTO_WIDGET_TYPE_RELAY && w->relay.counter > 0) {
			w->relay.counter--;
			if (w->relay.counter == 0 && p->handlers->relay) {
				p->handlers->relay(p->handler_data, w->idx, 0);
			}
		}
	}
}

