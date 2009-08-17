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

#include <stdint.h>

#include <RingBuff.hh>

RingBuff::RingBuff(int size)
{
	this->size = size;
	head = 0;
	tail = 0;
	data = new uint8_t[size];
}

RingBuff::~RingBuff()
{
	delete data;
}


void RingBuff::insert(uint8_t d)
{
	if (((head + 1) % size) == tail)
		return;

	data[head] = d;

	head = (head + 1) % size;
}

uint8_t RingBuff::remove(void)
{
	uint8_t d;

	if (head == tail)
		return 0;

	d = data[tail];
	tail = (tail + 1) % size;

	return d;
}

int RingBuff::length(void)
{
	if (head >= tail)
		return head - tail;
	else
		return head + size - tail;
}

