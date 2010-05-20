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

#include "EventMachine.hh"

#include <sstream>
#include <png.h>


EventMask::EventMask(string mask)
{
	istringstream iss(mask, istringstream::in);
	string word;
	while(iss >> word) {
		names.push_back(word);
	}
}


bool
EventMask::stateMatch(State *state)
{
	vector<string>::iterator i;
	for (i = names.begin(); i != names.end(); i++) {
		if (state->getDigitalIn((*i).c_str())) {
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
#ifndef OSX
	png_read_destroy(png, info, NULL);
#endif
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
		fprintf(stderr, "Script %s could not be opened for reading", script.c_str());
		return false;
	}

	/* initialize stuff */
	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info = png_create_info_struct(png);

	png_init_io(png, fp);
	png_read_info(png, info);
	png_read_update_info(png, info);

        if (info->width != 89) {
		fprintf(stderr, "Script %s is not the correct width (expected 89, got %lu).\n",
                        script.c_str(), info->width);
#ifndef OSX
                png_read_destroy(png, info, NULL);
#endif
                fclose(fp);
		return false;
        }

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
		    vector<string> ledNames,
		    vector<string> digitalNames)
{
	// XXX: constify offsets
	unsigned int x=0;
	unsigned int y=frame;

	if (y >= info->height) {
		fprintf(stderr, "Frame %d greater than PNG height %lu\n",
			frame, info->height);
		return false;
	}
	png_byte* row = data[y];

	vector<string>::iterator i = ledNames.begin();
	for (i = ledNames.begin(), x = 0; i != ledNames.end(); i++, x++) {
		png_byte* ptr = &(row[x*3]); //get pixel rgb

		if (ptr[0] > 5 || ptr[1] > 5 || ptr[2] > 5)
			state->setLightOut(i->c_str(), ptr[0], ptr[1], ptr[2]);
	}

	i = digitalNames.begin();
	for (i = digitalNames.begin(); i != digitalNames.end(); i++, x++) {
		png_byte* ptr = &(row[x*3]); //get pixel rgb

		if (ptr[0] > 5 && ptr[1] > 5 && ptr[2] > 5)
			state->setDigitalOut(i->c_str(), true);
	}

	return y < info->height - 1;
}


bool
EventMachine::addScript(string mask, string script)
{
	EventScript *es = scriptData[script];
	if (!es) {
		// load script file, add with name to scriptData
		es = new EventScript();
		if (!es->load(script)) {
			delete es;
			fprintf(stderr, "Failed to load script '%s'\n", script.c_str());
			return false;
		}
	}

	scriptMasks.push_back(pair<EventMask, string>(EventMask(mask), script));
	scriptData[script] = es;

	fprintf(stderr, "Added script '%s' with event mask '%s'\n",
		script.c_str(), mask.c_str());

	return true;
}


void
EventMachine::update(State *state,
		     vector<string> ledNames,
		     vector<string> digitalNames)
{
	vector<string>::iterator i;

	for (i = digitalNames.begin(); i != digitalNames.end(); i++)
		state->setDigitalOut(i->c_str(), false);

	for (vector< pair<EventMask, string> >::iterator i = scriptMasks.begin();
	     i != scriptMasks.end(); i++) {
		if (i->first.stateMatch(state)) {
			//fprintf(stderr, "Event state matches for script: %s\n",
			//	i->second.c_str());
			EventScript *data = scriptData[i->second];
			bool isRunning = false;
			vector< pair<EventScript*, uint> >::iterator i2;
			for (i2 = scriptStates.begin(); i2 != scriptStates.end(); i2++) {
				if (data == i2->first) {
					//fprintf(stderr, "Script %p already running.\n",
					//	i2->first);
					isRunning = true;
				}
			}

			if (!isRunning) {
				fprintf(stderr, "Restarting script '%s'.\n",
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

		if (!data) {
			fprintf(stderr, "Invalid script data!\n");
			continue;
		}

		if (data->update(state, frame, ledNames, digitalNames)) {
			//fprintf(stderr, "Advancing to frame %d of script %p\n", frame, data);
			nextStates.push_back(pair<EventScript*, uint>(data, frame + 1));
		}
	}
	scriptStates = nextStates;
}


bool
EventMachine::parseEvent(xmlNodePtr node)
{
	bool ret = false;
	xmlChar *maskStr = xmlGetProp(node, (const xmlChar *)"mask");
	xmlChar *scriptStr = xmlGetProp(node, (const xmlChar *)"script");

	if (maskStr == NULL) {
		fprintf(stderr, "%s has no mask\n", node->name);
		goto err;
	}
	if (scriptStr == NULL) {
		fprintf(stderr, "%s has no script\n", node->name);
		goto err;
	}

	ret = addScript((char *)maskStr, (char *)scriptStr);

err:
	xmlFree(maskStr);
	xmlFree(scriptStr);
	return ret;
}


bool
EventMachine::loadConfig(const char *fileName)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	xmlNodePtr child;
	bool ret = true;

	doc = xmlParseFile(fileName);

	if (doc == NULL) {
		fprintf(stderr, "Document %s not parsed successfully\n",
			fileName);
		return false;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		fprintf(stderr, "empty doc\n");
		return false;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *)"config")){
		fprintf(stderr, "config file does not start with config element\n");
		goto err0;
	}

	for (child = cur->xmlChildrenNode;
	     child != NULL;
	     child = child->next) {
		if (!xmlStrcmp(child->name, (const xmlChar*)"event")) {
			ret = parseEvent(child);
			if (!ret)
				goto err0;
		}
	}

err0:
	xmlFreeDoc(doc);
	return ret;
}
