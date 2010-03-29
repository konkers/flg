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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <errno.h>
#include <sys/select.h>

#include <TtyLink.hh>

TtyLink::TtyLink(const char *devName, bool echo)
{
	struct termios tio;
	int fdflags;

	linkEcho = echo;

	fd = open(devName, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) {
		perror("can't open dev");
		return;
	}

	memset(&tio, 0x0, sizeof(tio));
#ifdef __APPLE__
	tio.c_cflag =  CS8 | CLOCAL | CREAD;
	tio.c_cc[VTIME] = 1;
	tio.c_cc[VMIN] = 1;
#else
	tio.c_cflag =  B115200 | CS8 | CLOCAL | CREAD;
#endif
	tio.c_iflag = IGNPAR;
	tio.c_ispeed = B115200;
	tio.c_ospeed = B115200;

	if (tcsetattr(fd, TCSANOW, &tio) < 0) {
		perror("can't set tio");
	}

	fdflags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, fdflags & ~O_NDELAY);
}

TtyLink::~TtyLink()
{
	if (fd >= 0)
		close(fd);
}


bool TtyLink::send(const void *data, int len)
{
	ssize_t retval;
	bool status = true;

	retval = write(fd, data, len);
	if (retval < 0) {
		perror("write failed");
		return false;
	}

	if (retval != len) {
		perror("short write");
		status = false;
	}

	if (linkEcho) {
		while (retval--) {
			uint8_t c;
			read(fd, &c, 1);
		}
	}

	return true;
}

int TtyLink::wait(int timeout)
{
#ifdef USEPOLL
	struct pollfd fds;
	int retval;

	fds.fd = fd;
	fds.events = POLLIN | POLLERR | POLLHUP;
	retval = poll(&fds, 1, timeout);

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
#else
	fd_set rfds;
	int retval;
	struct timeval tv;

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);

	tv.tv_sec = timeout / 100;
	tv.tv_usec = (timeout % 100) * 10000;

	retval = select(fd+1, &rfds, NULL, NULL, &tv);

	if (errno == EAGAIN) {
		return 0;
	}

	if (retval < 0) {
		perror("select");
		return -1;
	}

	return timeout;
#endif
}

bool TtyLink::recv(void *data, int *len)
{
	ssize_t retval;

	retval = read(fd, data, *len);
	if (retval < 0) {
		perror("recv failed");
		return false;
	}

	*len = retval;
	return true;
}

