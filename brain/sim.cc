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

#include <MapFileParser.hh>

#include "Soma.hh"
#include "SimState.hh"
#include "SimLink.hh"
#include "Thread.hh"

SimState *state;
SimLink *flameLink;
SimLink *ledLink;
Soma *soma;

int main(int argc, char *argv)
{
	state = new SimState();

	if (!state->loadLightMap("simLed.map"))
		return 1;

	MapFileParser p;
	char *key;
	int val;

	if (!p.open("brainLed.map"))
		return 1;

	while (p.read(&key, &val)) {
		state->addLedRgb(key, val);
	}

	state->addInput("li1", 0x80);
	state->addInput("li2", 0x81);
	state->addInput("ui1", 0x82);

	flameLink = new SimLink(state, SimLink::FLAME);
	ledLink = new SimLink(state, SimLink::LED);

	soma = new Soma();

	if (!soma->loadLightMap("brainLed.map"))
		return 1;

	if (!soma->loadRelayMap("brainRelay.map"))
		return 1;

	if (!soma->loadDpotMap("brainDpot.map"))
		return 1;

	if (!soma->loadButtonMap("brainButton.map"))
		return 1;

	if (!soma->loadKnobMap("brainKnob.map"))
		return 1;

	soma->attachFlameLink(flameLink);
	soma->attachLedLink(ledLink);

	state->startWebServer(8080);

	soma->run();
}

