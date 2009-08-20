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

#ifndef __Soma_hh__
#define __Soma_hh__

#include <Proto.hh>

class Soma
{
public:
	static const int nLights = 60;
	static const int nRelays = 33;
	static const int nDpots = 4;
	static const int nButtons = 11;
	static const int nKnobs = 4;

private:
	class State {
	public:
		uint32_t lights[Soma::nLights];
		bool relays[Soma::nRelays];
		uint8_t dpots[Soma::nDpots];
		bool buttons[Soma::nButton];
		uint16_t knobs[Soma::nKnobs];
	};

	State state[2];

	int ledIdx;
	int flameIdx;

	Thread::Mutex ledLocks[2];
	Thread::Mutex flameLocks[2];

	LedProtoThread *ledProto;
	LedProtoThread *flameProto;

	bool button(int i) {
		return state[flameIdx].button[i];
	}

	uint16_t knob(int i) {
		return state[flameIdx].knob[i];
	}

	uint32_t light(int i) {
		return state[ledIdx].light[i];
	}

	void setLight(int i, int r, int g, int b) {
		uint32_t val = (r & 0xff) |
			((g & 0xff) << 8) |
			((b & 0xff) << 16);

		state[ledIdx].light[i] = val;
	}

	bool relay(int i) {
		return state[flameIdx].relay[i];
	}

	void setRelay(int i, bool val) {
		state[flameIdx].relay[i] = val;
	}

public:
	Soma();
	~Soma();

	void attachLedLink(Link *l);
	void attachFlameProto(Link *l);

	void run(void);

	bool getRelay(int i) {
		return state[!flameIdx].relay[i];
	}

	uint32_t getLight(int i) {
		return state[!lightIdx].lights[i];
	}

	uint32_t *getLights(void) {
		return state[!lightIdx].lights;
	}


	void setKnob(int i, uint16_t val) {
		state[!flameIdx].knobs[i] = val;
	}

	void setButton(int i, bool val) {
		state[!flameIdx].buttons[i] = val;
	}

	void flameSync(void);
};

#endif /* __Soma_hh__ */

