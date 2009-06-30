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

#include <TtyLink.hh>

TtyLink::TtyLink(char *devName)
{
  struct termios tio;
  
  fd = open(devName, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
    perror("can't open dev");
    return;
  }

  memset(&tio, 0x0, sizeof(tio));
#ifdef __APPLE__
  tio.c_cflag =  CS8 | CLOCAL | CREAD;
#else
  tio.c_cflag =  B115200 | CS8 | CLOCAL | CREAD;
#endif
  tio.c_iflag = IGNPAR;
  tio.c_ispeed = B115200;
  tio.c_ospeed = B115200;

  if (tcsetattr(fd, TCSANOW, &tio) < 0) {
    perror("can't set tio");
  }

  
}

TtyLink::~TtyLink()
{
	if (fd >= 0)
		close(fd);
}


bool TtyLink::send(const void *data, int len)
{
	ssize_t retval;

	retval = write(fd, data, len);
	if (retval < 0) {
		perror("write failed");
		return false;
	} else if (retval != len) {
		perror("short write");
		return false;
	}
	return true;
}

int TtyLink::wait(int timeout)
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

