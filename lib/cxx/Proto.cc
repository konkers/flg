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

#include <stdio.h>
#include <string.h>

#include <Proto.hh>

Proto::Proto(Link *link, uint8_t addr)
{
	this->link = link;
	proto_init(&p, addr);
}

bool Proto::sendMsg(uint8_t addr, uint8_t cmd, uint8_t val)
{
	proto_packet pkt;
	pkt.addr = addr;
	pkt.cmd = cmd;
	pkt.val = val;
	proto_packet_seal(&pkt);

	return link->send(&pkt, sizeof(pkt));
}

int Proto::waitForMsg(struct proto_packet *pkt, int timeout)
{
	int len;
	int err;
	uint8_t data[32];
	uint8_t *dp = data;
	struct proto_packet *ret_pkt;

	err = link->wait(1000);
	if (err <= 0) {
		return err;
	}

	len = 32;
	if (link->recv(data,&len)) {
		while (len--) {
			ret_pkt = proto_recv(&p, *dp++);
			if (ret_pkt) {
				memcpy(pkt, ret_pkt, sizeof(*pkt));
				if (len)
					printf("warning: dropping %d bytes of data\n",
					       len);
				return err;
			}
		}
		return -1;
	} else {
		return -1;
	}
}





