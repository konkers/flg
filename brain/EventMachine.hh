/*
 * Copyright 2009 Alex Graveley
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

#ifndef __EventMachine_hh__
#define __EventMachine_hh__

#include <sys/time.h>

#include <string>
#include <vector>
using namespace std;

#include <png.h>

#include <State.hh>

class EventMask
{
        vector<string> names;

public:
	EventMask(string mask);
        bool stateMatch(State *state);
};

class EventMachine
{
private:
        vector< pair<EventMask, string> > scriptMasks;
        map<string, png_structp> scriptData;
        vector< pair<png_structp, uint> > scriptStates;

public:
	EventMachine();
	~EventMachine();

        void addScript(string mask, string script);
        void update(State *state);
};

#endif /* __Soma_hh__ */
