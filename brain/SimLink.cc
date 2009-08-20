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

#include "SimLink.hh"

SimLink::SimLink(SimState *state)
{
	s = state;
}

SimLink::~SimLink()
{
}

bool SimLink::send(const void *data, int len)
{
	const uint8_t *d = (uint8_t *)data;

	while (len--)
		s->send(*d++);

	return true;
}

int SimLink::wait(int timeout)
{
	if (s->hasData())
		return timeout;

	usleep(timeout);
	return 0;
}

bool SimLink::recv(void *data, int *len)
{
	int realLen = 0;
	uint8_t *d = (uint8_t *)data;

	while (realLen < *len) {
		if (!s->hasData())
			break;
		d[realLen] = s->recv();
		realLen++;
	}

	*len = realLen;
	return true;
}






