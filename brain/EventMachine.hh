/*
 * Copyright 2010 Alex Graveley
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
private:
        vector<string> names;

public:
	EventMask(string mask);
        bool stateMatch(State *state);
};


class EventScript
{
private:
        png_structp png;
        png_infop info;
        png_bytepp data;

public:
        EventScript() {};
	~EventScript();
        bool load(string script);
        uint get_frames();
        bool update(State *state, uint frame,
                    vector<string> lowerLedNames,
                    vector<string> axonLedNames,
                    vector<string> upperLedNames,
                    vector<string> digitalNames);
};


class EventMachine
{
private:
        vector< pair<EventMask, string> > scriptMasks;
        map<string, EventScript*> scriptData;
        vector< pair<EventScript*, uint> > scriptStates;

	bool parseEvent(xmlNodePtr cur);

public:
        EventMachine() {};
        ~EventMachine() {};

        bool addScript(string mask, string script);
        void update(State *state,
                    vector<string> lowerLedNames,
                    vector<string> axonLedNames,
                    vector<string> upperLedNames,
                    vector<string> digitalNames);

        bool loadConfig(const char *fileName);
};

#endif /* __Soma_hh__ */
