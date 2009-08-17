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

#ifndef __RingBuff_hh__
#define __RingBuff_hh__

class RingBuff
{
private:
	uint8_t *data;
	int size;

	int head;
	int tail;

public:
	RingBuff(int size);
	~RingBuff();

	void insert(uint8_t d);
	uint8_t remove(void);
	int length(void);
};

#endif /* __RingBuff_hh__ */
