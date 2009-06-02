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

#ifndef __proto_h__
#define __proto_h__

#define PROTO_SOF	0x53
#define PROTO_EOF	0x45

struct proto_packet {
	uint8_t	addr;
	uint8_t	cmd;
	uint8_t	val;
	uint8_t	crc;
};

enum proto_state {
	PROTO_STATE_IDLE,
	PROTO_STATE_ADDR,
	PROTO_STATE_CMD,
	PROTO_STATE_VAL,
	PROTO_STATE_CRC,
	PROTO_STATE_EOF,
};

struct proto_state {
	struct short_packet	packet;
	enum proto_state	state;
};

void proto_init(struct proto_state *p);
struct proto_packet *proto_recv(struct proto_state *p, uint8_t byte);


#endif /* __proto_h__ */


