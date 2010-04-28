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

#include <sstream>

#include "EventMachine.hh"

EventMask::EventMask(string mask)
{
   istringstream iss(mask, istringstream::in);
   string word;
   while(iss >> word) {
      fprintf(stderr, "EventMask: mask=%s\n", word.c_str());
      names.push_back(word);
   }
}

bool
EventMask::stateMatch(State *state)
{
   vector<string>::iterator i;
   for (i = names.begin(); i != names.end(); i++) {
      if (!state->getDigitalIn((*i).c_str())) {
         return false;
      }
   }
   return true;
}


EventMachine::EventMachine()
{
}


EventMachine::~EventMachine()
{
}


void
EventMachine::addScript(string mask, string script)
{
   // load script file, add with name to scriptData
   png_structp data = NULL;

   scriptMasks.push_back(pair<EventMask, string>(EventMask(mask), script));
   scriptData[script] = data;

   fprintf(stderr, "EventMachine::addScript: mask=%s, script=%s, total scripts: %d\n",
           mask.c_str(), script.c_str(), (int)scriptMasks.size());
}


void
EventMachine::update(State *state)
{
   for (vector< pair<EventMask, string> >::iterator i = scriptMasks.begin();
        i != scriptMasks.end(); i++) {
      if (i->first.stateMatch(state)) {
         fprintf(stderr, "EventMachine::update: state matches for script: %s\n",
                 i->second.c_str());
         png_structp data = scriptData[i->second];
         scriptStates.push_back(pair<png_structp, uint>(data, 0));
      }
   }

   vector< pair<png_structp, uint> > nextStates;
   for (vector< pair<png_structp, uint> >::iterator i2 = scriptStates.begin();
        i2 != scriptStates.end(); i2++) {
      // Call Ben's event updating for the row at index i2->second.
      png_structp data = i2->first;
      uint frame = i2->second;

      if (data && data->num_rows <= frame) {
         fprintf(stderr, "EventMachine::update: advancing to frame %d: %p\n", frame, data);
         nextStates.push_back(pair<png_structp, uint>(data, frame + 1));
      } else {
         fprintf(stderr, "EventMachine::update: script completed: %p\n", data);
      }
   }
   scriptStates = nextStates;
}
