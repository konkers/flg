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

#include "Soma.hh"

Soma::Soma()
{
	axonLedNames.push_back("a1");
	axonLedNames.push_back("a2");
	axonLedNames.push_back("a3");
	axonLedNames.push_back("a4");
	axonLedNames.push_back("a5");
	axonLedNames.push_back("a6");
	axonLedNames.push_back("a7");
	axonLedNames.push_back("a8");
	axonLedNames.push_back("a9");
	axonLedNames.push_back("a10");

	lowerLedNames.push_back("l16a");
	lowerLedNames.push_back("l16c");
	lowerLedNames.push_back("l17a");
	lowerLedNames.push_back("l17c");
	lowerLedNames.push_back("l18a");
	lowerLedNames.push_back("l18c");
	lowerLedNames.push_back("l19a");
	lowerLedNames.push_back("l19c");
	lowerLedNames.push_back("l20a");
	lowerLedNames.push_back("l20c");

	upperLedNames.push_back("u6a");
	upperLedNames.push_back("u6c");
	upperLedNames.push_back("u7a");
	upperLedNames.push_back("u7c");
	upperLedNames.push_back("u8a");
	upperLedNames.push_back("u8c");
	upperLedNames.push_back("u9a");
	upperLedNames.push_back("u9c");
	upperLedNames.push_back("u10a");
	upperLedNames.push_back("u10c");
	upperLedNames.push_back("u11a");
	upperLedNames.push_back("u11c");
	upperLedNames.push_back("u12a");
	upperLedNames.push_back("u12c");
	upperLedNames.push_back("u13a");
	upperLedNames.push_back("u13c");
	upperLedNames.push_back("u14a");
	upperLedNames.push_back("u14c");
	upperLedNames.push_back("u15a");
	upperLedNames.push_back("u15c");
	upperLedNames.push_back("u16a");
	upperLedNames.push_back("u16c");
	upperLedNames.push_back("u17a");
	upperLedNames.push_back("u17c");
	upperLedNames.push_back("u18a");
	upperLedNames.push_back("u18c");
	upperLedNames.push_back("u19a");
	upperLedNames.push_back("u19c");
	upperLedNames.push_back("u20a");
	upperLedNames.push_back("u20c");
}

Soma::~Soma()
{

}

bool Soma::loadConfig(const char *fileName)
{
	return state.loadConfig(fileName);
}

bool Soma::attachLink(const char *busName, Link *link)
{
	return state.attachLink(busName, link);
}

void Soma::run(void)
{
	struct timeval tv;
	struct timeval last_tv;
	struct timeval tmp_tv;
	struct timeval frametime;

	uint8_t val = 0;
	vector<string>::iterator i;

	frametime.tv_sec = 0;
	frametime.tv_usec = 33000;

	state.run();

	printf("run\n");

	// XXX: gettimeofday is not garunteet to be regular as the clock can be
	//      set at any time (by user, ntp, etc.)
	gettimeofday(&last_tv, NULL);
	while (1) {
		state.sync();
		state.setDigitalOut("a1b", !state.getDigitalIn("l6"));
		state.setDigitalOut("a2b", !state.getDigitalIn("l7"));
		state.setDigitalOut("a3b", !state.getDigitalIn("l8"));
		state.setDigitalOut("a4b", !state.getDigitalIn("l9"));
		state.setDigitalOut("a5b", !state.getDigitalIn("l10"));
		state.setDigitalOut("a6b", !state.getDigitalIn("l11"));
		state.setDigitalOut("a7b", !state.getDigitalIn("l12"));
		state.setDigitalOut("a8b", !state.getDigitalIn("l13"));

		for (i = axonLedNames.begin(); i != axonLedNames.end(); i++ )
			state.setLightOut(i->c_str(), val, 0x0, 0x0);

		for (i = lowerLedNames.begin(); i != lowerLedNames.end(); i++ )
			state.setLightOut(i->c_str(), val, 0x0, 0x0);

		for (i = upperLedNames.begin(); i != upperLedNames.end(); i++ )
			state.setLightOut(i->c_str(), val, 0x0, 0x0);

		val++;

		gettimeofday(&tv, NULL);
		timersub(&tv, &last_tv, &tmp_tv);
		if (timercmp(&tmp_tv, &frametime, <)) {
			timersub(&frametime, &tmp_tv, &tv);
			usleep(tv.tv_usec);
		} else {
			fprintf(stderr, "frame overrun: %d %d",
				(int)tmp_tv.tv_sec, (int)tmp_tv.tv_usec);
			fprintf(stderr, "\n");
		}
		gettimeofday(&last_tv, NULL);

	}
}

