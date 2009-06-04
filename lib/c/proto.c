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


struct proto_packet *proto_recv(struct proto *p, uint8_t data)
{
	struct proto_packet *ret = NULL;

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
		if (data == PROTO_EOF && p->crc == p->packet.crc) {
			ret = &p->packet;
		}
		p->state = PROTO_STATE_IDLE;
		break;
	}

	return ret;
}

