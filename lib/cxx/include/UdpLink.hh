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

#ifndef __UdpLink_hh__
#define __UdpLink_hh__

#include <netinet/in.h>

#include <stdint.h>

#include <Link.hh>

class UdpLink : public Link {
private:
	int fd;
	struct sockaddr_in remoteAddr;

public:
	UdpLink(int localPort, const char *remoteAddr, int remotePort);
	virtual ~UdpLink();

	virtual bool send(uint8_t *data, int len);
	virtual int wait(int timeout);
	virtual bool recv(uint8_t *data, int *len);
};

#endif /* __UdpLink_hh__ */
