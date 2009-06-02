/*
 * Copyright 2009 Flaming Lotus Girls
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

#include <proto.h>
#include <crc8.h>

void proto_init(struct proto_state *p, uint8_t addr)
{
	p->addr = addr;
	p->state = PROTO_STATE_IDLE;
}

struct proto_packet *proto_recv(struct proto_state *p, uint8_t byte)
{
	struct proto_packet *ret = NULL;

	switch (p->state) {
	case PROTO_STATE_IDLE:
		if (byte == PROTO_SOF) {
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
		p->state = PROTO_STATE_CAL;
		break;

	case PROTO_STATE_VAL:
		p->crc = crc8_calc(p->crc, data);
		p->packet.cal = data;
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

