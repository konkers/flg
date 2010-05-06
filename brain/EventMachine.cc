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

#include <sstream>
#include <png.h>

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


EventScript::~EventScript()
{
   for (uint y=0; y < info->height; y++) {
      free(data[y]);
   }
   png_read_destroy(png, info, NULL);
}


uint
EventScript::get_frames()
{
   return info->height;
}


bool
EventScript::load(string script)
{
   /* open file and test for it being a png */
   FILE *fp = fopen(script.c_str(), "rb");
   if (!fp) {
      fprintf(stderr, "EventMachine::loadScript: File %s could not be opened for reading",
              script.c_str());
      return false;
   }

   /* initialize stuff */
   png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   info = png_create_info_struct(png);

   png_init_io(png, fp);
   png_read_info(png, info);
   png_read_update_info(png, info);

   data = (png_bytepp) malloc(sizeof(png_bytep) * info->height);
   for (uint y=0; y < info->height; y++) {
      data[y] = (png_byte*) malloc(info->rowbytes);
   }
   png_read_image(png, data);

   fclose(fp);
   return true;
}


bool
EventScript::update(State *state,
                    uint frame,
                    vector<string> lowerLedNames,
                    vector<string> axonLedNames,
                    vector<string> upperLedNames,
                    vector<string> digitalNames)
{
   // XXX: constify offsets
   uint8_t x=0;
   uint8_t y=frame;

   if (y >= info->height) {
      //fprintf(stderr, "EventScript::update frame %d greater than PNG height %lu\n",
      //        frame, info->height);
      return false;
   }
   png_byte* row = data[y];

   vector<string>::iterator i = axonLedNames.begin();
   x = 0; //axon LED's start at pixel 0
   for (i = axonLedNames.begin(); i != axonLedNames.end(); i++) {
      png_byte* ptr = &(row[x*3]); //get pixel rgb
      //fprintf(stderr, "Pixel [x: %d, y: %d] R:%d G:%d B:%d\n",
      //        x, y, ptr[0], ptr[1], ptr[2]);
      state->setLightOut(i->c_str(), ptr[0], ptr[1], ptr[2]);
      //printf("%d %d %d | ", ptr[0], ptr[1], ptr[2]);
      x++;
   }

   i = upperLedNames.begin();
   x = 10; //upper soma LED's start at pixel 10
   for (i = upperLedNames.begin(); i != upperLedNames.end(); i++) {
      png_byte* ptr = &(row[x*3]); //get pixel rgb
      state->setLightOut(i->c_str(), ptr[0], ptr[1], ptr[2]);
      x++;
   }

   i = lowerLedNames.begin();
   x = 40; //lower soma LED's start at pixel 40
   for (i = lowerLedNames.begin(); i != lowerLedNames.end(); i++) {
      png_byte* ptr = &(row[x*3]); //get pixel rgb
      state->setLightOut(i->c_str(), ptr[0], ptr[1], ptr[2]);
      x++;
   }

   i = digitalNames.begin();
   x = 50; //digital poofers start at pixel 50
   for (i = digitalNames.begin(); i != digitalNames.end(); i++) {
      //png_byte* ptr = &(row[x*3]); //get pixel rgb
      //fprintf(stderr, "Setting Poofer [x: %d, y: %d] %s\n",
      //        x, y, ptr[0] + ptr[1] + ptr[2] == 0 ? "ON" : "OFF");
      // XXX: WARNING trying to set non-existant digital out a8b
      //state->setDigitalOut(i->c_str(), ptr[0] + ptr[1] + ptr[2] == 0);
      x++;
   }

   //fprintf(stderr, "EventScript::update return == %s\n", y < info->height ? "TRUE" : "FALSE");
   return y < info->height;
}


bool
EventMachine::addScript(string mask, string script)
{
   if (scriptData[script] != NULL) {
      fprintf(stderr, "EventMachine::addScript: reusing loaded script '%s'\n",
              script.c_str());
      return true;
   }

   // load script file, add with name to scriptData
   EventScript *es = scriptData[script];
   if (!es) {
      es = new EventScript();
      if (!es->load(script)) {
         delete es;
         fprintf(stderr, "EventMachine::addScript: failed to load script '%s'\n",
                 script.c_str());
         return false;
      }
   }

   scriptMasks.push_back(pair<EventMask, string>(EventMask(mask), script));
   scriptData[script] = es;

   fprintf(stderr, "EventMachine::addScript: mask=%s, script=%s, total scripts: %d\n",
           mask.c_str(), script.c_str(), (int)scriptMasks.size());
   return true;
}


void
EventMachine::update(State *state,
                     vector<string> lowerLedNames,
                     vector<string> axonLedNames,
                     vector<string> upperLedNames,
                     vector<string> digitalNames)
{
   for (vector< pair<EventMask, string> >::iterator i = scriptMasks.begin();
        i != scriptMasks.end(); i++) {
      if (i->first.stateMatch(state)) {
         //fprintf(stderr, "EventMachine::update: state matches for script: %s\n",
         //        i->second.c_str());
         EventScript *data = scriptData[i->second];
         bool isRunning = false;
         for (vector< pair<EventScript*, uint> >::iterator i2 = scriptStates.begin();
              i2 != scriptStates.end(); i2++) {
            if (data == i2->first) {
               //fprintf(stderr, "EventMachine::update: script %p already running.\n",
               //        i2->first);
               isRunning = true;
            }
         }
         if (!isRunning) {
            fprintf(stderr, "EventMachine::update: restarting script '%s'.\n",
                    i->second.c_str());
            scriptStates.push_back(pair<EventScript *, uint>(data, 0));
         }
      }
   }

   vector< pair<EventScript*, uint> > nextStates;
   for (vector< pair<EventScript*, uint> >::iterator i2 = scriptStates.begin();
        i2 != scriptStates.end(); i2++) {
      // Call Ben's event updating for the row at index i2->second.
      EventScript *data = i2->first;
      uint frame = i2->second;

      if (data && data->update(state, frame, lowerLedNames, axonLedNames,
                               upperLedNames, digitalNames)) {
         //fprintf(stderr, "EventMachine::update: advancing to frame %d: %p\n", frame, data);
         nextStates.push_back(pair<EventScript*, uint>(data, frame + 1));
      } else {
         fprintf(stderr, "EventMachine::update: script completed: %p\n", data);
      }
   }
   scriptStates = nextStates;
}
