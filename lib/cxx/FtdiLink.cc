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

#undef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <errno.h>
#include <sys/select.h>

#include <proto.h>

#include <FtdiLink.hh>

FtdiLink::FtdiLink(int vid, int pid, ftdi_interface interface)
{
	int err;

	err = ftdi_init(&ftdi);
	if (err < 0) {
		perror("ftdi_init failed");
		return;
	}

	ftdi_set_interface(&ftdi, interface);

	err = ftdi_usb_open(&ftdi, vid, pid);
	if (err < 0) {
		perror("ftdi_usb_open failed");
		return;
	}

	err = ftdi_usb_reset(&ftdi);
	if (err < 0) {
		perror("ftdi_usb_open failed");
		return;
	}

	err = ftdi_usb_purge_rx_buffer(&ftdi);
	err = ftdi_usb_purge_tx_buffer(&ftdi);

	ftdi_set_baudrate(&ftdi, 115200);
	ftdi_set_line_property(&ftdi, BITS_8, STOP_BIT_1, NONE);
	ftdi_set_event_char(&ftdi, PROTO_EOF, 1);
}

FtdiLink::~FtdiLink()
{
	ftdi_usb_close(&ftdi);
	ftdi_deinit(&ftdi);
}


bool FtdiLink::send(const void *data, int len)
{
	ssize_t retval;
	bool status = true;


#ifdef DEBUG
	printf(">");

	for (int i = 0; i < len; i++ )
		printf(" %02x", ((uint8_t *)data)[i]);

	printf("\n");
#endif

	retval = ftdi_write_data(&ftdi, (unsigned char*) data, len);
	if (retval < 0) {
		perror("write failed");
		return false;
	}

	if (retval != len) {
		perror("short write");
		status = false;
	}

	return true;
}

int FtdiLink::wait(int timeout)
{
	ftdi.usb_read_timeout = timeout;
	return timeout;
}

bool FtdiLink::recv(void *data, int *len)
{
	ssize_t retval;

	retval = ftdi_read_data(&ftdi, (unsigned char*) data, *len);
	if (retval < 0) {
		perror("recv failed");
		return false;
	}

#ifdef DEBUG
	printf("<");

	for (int i = 0; i < retval; i++ )
		printf(" %02x", ((uint8_t *)data)[i]);

	printf("\n");
#endif

	*len = retval;
	return true;
}

