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

#include <string.h>

#include <StepSequencer.hh>

StepSequencer::StepSequencer(unsigned numChannels, vector<string> *pixelMap)
{
	unsigned i;

	this->numChannels = numChannels;
	this->pixelMap = pixelMap;

	frame = new uint32_t[pixelMap->size()];
	channels = new ChannelState[numChannels];

	for (i = 0; i < numChannels; i++) {
		channels[i].state = STATE_OFF;
		channels[i].seq = NULL;
	}

	lastStep.tv_sec = 0;
	lastStep.tv_usec = 0;
}

StepSequencer::~StepSequencer()
{
	delete channels;
}

void StepSequencer::setChannelSequence(unsigned channel, Sequence *seq)
{
	if (channel >= numChannels)
		return;

	channels[channel].state = STATE_OFF;
	channels[channel].seq = seq;
}

void StepSequencer::setChannelState(unsigned channel, int state)
{
	if (channel >= numChannels)
		return;

	channels[channel].state = state;
	channels[channel].timeCode = 0.0;
}

void StepSequencer::step(State *state)
{
	struct timeval thisStep, stepDelta;
	float step;
	unsigned c;

	gettimeofday(&thisStep, NULL);

	if (lastStep.tv_sec == 0 && lastStep.tv_usec == 0)
		lastStep = thisStep;

	timersub(&thisStep, &lastStep, &stepDelta);

	step = stepDelta.tv_sec + stepDelta.tv_usec / 1000000.0;

	memset(frame, 0x0, sizeof(uint32_t) * pixelMap->size());
	for (c = 0; c < numChannels; c++) {
		bool looped;

		if (channels[c].state == STATE_OFF ||channels[c].seq == NULL)
			continue;

		channels[c].timeCode += step;

		looped = channels[c].seq->handleFrame(frame, pixelMap->size(),
						      channels[c].timeCode);

		if (looped && channels[c].state == STATE_SINGLE) {
			channels[c].state = STATE_OFF;
			channels[c].timeCode = 0.0;
		}
	}

	unsigned i;

	for (i = 0; i < pixelMap->size(); i++) {
		const char *name = (*pixelMap)[i].c_str();
		uint8_t r, g, b;

		r = frame[i] & 0xff;
		g = (frame[i] >> 8) & 0xff;
		b = (frame[i] >> 16) & 0xff;

		if (state->hasDigitalOut(name)) {
			state->setDigitalOut(name, r > 5 || g > 5 || b > 5);
		} else if (state->hasLightOut(name)) {
			state->setLightOut(name, r, g, b);
		} else {
			fprintf(stderr, "unknown pixel output %s\n", name);
		}
	}

	lastStep = thisStep;
}

