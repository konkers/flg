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

#ifndef __StepSequencer_hh__
#define __StepSequencer_hh__

#include <sys/time.h>

#include <vector>
using namespace std;

#include <State.hh>
#include <Sequence.hh>

class StepSequencer
{
public:
	enum {
		STATE_OFF = 0,
		STATE_SINGLE = 1,
		STATE_LOOP = 2,
	};

private:
	class ChannelState
	{
	public:
		int state;
		float timeCode;
		Sequence *seq;
	};

	unsigned numChannels;
	ChannelState *channels;

	uint32_t *frame;

	vector<string> *pixelMap;

	struct timeval lastStep;

public:
	StepSequencer(unsigned numChannels, vector<string> *pixelMap);
	~StepSequencer();

	void setChannelSequence(unsigned channel, Sequence *seq);
	Sequence *getChannelSequence(unsigned channel) {
		if (channel >= numChannels)
			return NULL;
		return channels[channel].seq;
	}
	void setChannelState(unsigned channel, int mode);
	int getChannelState(unsigned channel) {
		if (channel >= numChannels)
			return -1;
		return channels[channel].state;
	}

	void step(State *state);

	unsigned getNumChannels(void) {
		return numChannels;
	}
};


#endif /* __StepSequence_hh__ */
