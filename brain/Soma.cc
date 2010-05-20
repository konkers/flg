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

	lowerLedNames.push_back("l11");
	lowerLedNames.push_back("l12");
	lowerLedNames.push_back("l13");
	lowerLedNames.push_back("l14");
	lowerLedNames.push_back("l15");
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

	upperLedNames.push_back("u1");
	upperLedNames.push_back("u2");
	upperLedNames.push_back("u3");
	upperLedNames.push_back("u4");
	upperLedNames.push_back("u5");
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

	allLedNames.insert(allLedNames.end(),
			   lowerLedNames.begin(), lowerLedNames.end());
	allLedNames.insert(allLedNames.end(),
			   axonLedNames.begin(), axonLedNames.end());
	allLedNames.insert(allLedNames.end(),
			   upperLedNames.begin(), upperLedNames.end());

	digitalNames.push_back("a1a");
	digitalNames.push_back("a1b");
	digitalNames.push_back("a1c");
	digitalNames.push_back("a2a");
	digitalNames.push_back("a2b");
	digitalNames.push_back("a2c");
	digitalNames.push_back("a3a");
	digitalNames.push_back("a3b");
	digitalNames.push_back("a3c");
	digitalNames.push_back("a4a");
	digitalNames.push_back("a4b");
	digitalNames.push_back("a4c");
	digitalNames.push_back("a5a");
	digitalNames.push_back("a5b");
	digitalNames.push_back("a5c");
	digitalNames.push_back("a6a");
	digitalNames.push_back("a6b");
	digitalNames.push_back("a6c");
	digitalNames.push_back("a7a");
	digitalNames.push_back("a7b");
	digitalNames.push_back("a7c");
	digitalNames.push_back("a8a");
	digitalNames.push_back("a8b");
	digitalNames.push_back("a8c");
	digitalNames.push_back("s1");
	digitalNames.push_back("s2");
	digitalNames.push_back("s3");
	digitalNames.push_back("s4");
	digitalNames.push_back("s5");

	printf("lower:%d axon:%d upper:%d poofer:%d\n",
	       (int) lowerLedNames.size(),
	       (int) axonLedNames.size(),
	       (int) upperLedNames.size(),
	       (int) digitalNames.size());
}

Soma::~Soma()
{

}

bool Soma::loadConfig(const char *fileName, const char *eventsfileName)
{
	return state.loadConfig(fileName) && em.loadConfig(eventsfileName);
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

	vector<string>::iterator i;

	frametime.tv_sec = 0;
	frametime.tv_usec = 33000;

	state.run();

	printf("run\n");

	// XXX: gettimeofday is not garunteed to be regular as the clock can be
	//      set at any time (by user, ntp, etc.)
	gettimeofday(&last_tv, NULL);
	while (1) {
		state.sync();

		em.update(&state, allLedNames, digitalNames);

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

