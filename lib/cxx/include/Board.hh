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

#ifndef __Board_hh__
#define __Board_hh__

#include <string>
#include <vector>
using namespace std;

#include <DigitalOut.hh>
#include <AnalogOut.hh>
#include <DigitalIn.hh>
#include <AnalogIn.hh>
#include <LightOut.hh>

class Bus;

class Board {


public:
	uint8_t addr;
	Bus *bus;

	vector<DigitalOut *> digitalOuts;
	vector<AnalogOut *> analogOuts;
	vector<DigitalIn *> digitalIns;
	vector<AnalogIn *> analogIns;
	vector<LightOut *> lightOuts;

	Board(int maxDigitalOuts, int maxAnalogOuts,
	      int maxDigitalIns, int maxAnalogIns,
	      int maxLightOuts) {
		digitalOuts.resize(maxDigitalOuts);
		analogOuts.resize(maxAnalogOuts);
		digitalIns.resize(maxDigitalIns);
		analogIns.resize(maxAnalogIns);
		lightOuts.resize(maxLightOuts);
	}
};


#endif /* __Board_hh__ */
