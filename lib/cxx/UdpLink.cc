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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <UdpLink.hh>

UdpLink::UdpLink(int localPort, const char *remoteAddr, int remotePort)
{
	struct sockaddr_in addr;
	int err;
	struct hostent *he;

	he = gethostbyname(remoteAddr);
	if (he == NULL) {
		perror("can't resolve remote address");
		return;
	}

	memset(&this->remoteAddr, 0, sizeof(this->remoteAddr));
	this->remoteAddr.sin_family = AF_INET;
	this->remoteAddr.sin_port = htons(remotePort);
	memcpy(&this->remoteAddr.sin_addr.s_addr, he->h_addr, he->h_length);

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		perror("can't create socket");
		return;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(localPort);

	err = bind(fd, (struct sockaddr *) &addr, sizeof(addr));
	if (err < 0) {
		perror("can't bind to port");
		close(fd);
		fd = -1;
	}
}

UdpLink::~UdpLink()
{
	if (fd >= 0)
		close(fd);
}


bool UdpLink::send(uint8_t *data, int len)
{
	ssize_t retval;

	retval = sendto(fd, data, len, 0, 
			(struct sockaddr *) &remoteAddr,
			sizeof(remoteAddr));
	if (retval < 0) {
		perror("sendto failed");
		return false;
	} else if (retval != len) {
		perror("short write");
		return false;
	}
	return true;
}

int UdpLink::wait(int timeout)
{
	struct pollfd fds;
	int retval;

	fds.fd = fd;
	fds.events = POLLIN | POLLERR | POLLHUP;
	retval = poll( &fds, 1, timeout);

	if (retval < 0) {
		return -1;
	}
	if (fds.revents & (POLLERR | POLLHUP)) {
		return -1;
	}
	if (retval == 0) {
		return 0;
	}

	return timeout;
}

bool UdpLink::recv(uint8_t *data, int *len)
{
	ssize_t retval;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	memcpy(&addr, &remoteAddr, addrlen);

	retval = recvfrom(fd, data, *len, 0,
			  (struct sockaddr *) &remoteAddr,
			  &addrlen);
	if (retval < 0) {
		perror("sendto failed");
		return false;
	}

	*len = retval;

	return true;
}

