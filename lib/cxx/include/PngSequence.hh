/*
 * Copyright 2010 Erik Gilling
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

#ifndef __PngSequence_hh__
#define __PngSequence_hh__

#include <Sequence.hh>

class PngSequence : public Sequence
{
private:
	// data is stored LSB in pre-multiplied alpha RGBA
	uint32_t *data;
	unsigned h, w;

	unsigned lastRow;
public:
	PngSequence();
	virtual ~PngSequence();
	bool load(const char *fileName, unsigned frameLen);

	virtual void start();
	virtual void stop(void);
	virtual bool handleFrame(uint32_t *frame, unsigned frameLen, float timeCode);
};


#endif /* __PngSequence_hh__ */
