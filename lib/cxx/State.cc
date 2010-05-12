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


#include <string.h>

#include <util.hh>
#include <State.hh>



State::State()
{
	aliases["relay"] = "digitalOut";
	aliases["pot"] = "analogOut";
	aliases["button"] = "digitalIn";
	aliases["knob"] = "analogIn";
	aliases["led"] = "lightOut";

	boardConfigs["relay3"] = new BoardConfig(3, 0, 0, 0, 0);
	boardConfigs["input"] = new BoardConfig(0, 0, 8, 2, 0);
	boardConfigs["ledRgb"] = new BoardConfig(0, 0, 0, 0, 1);
	boardConfigs["dpot"] = new BoardConfig(0, 2, 0, 0, 0);

	activePage = 0;
}

State::~State()
{

}

void State::dumpConfig(void)
{
	map<string, DigitalOut *>::iterator doi;
	map<string, AnalogOut *>::iterator aoi;
	map<string, DigitalIn *>::iterator dii;
	map<string, AnalogIn *>::iterator aii;
	map<string, LightOut *>::iterator loi;

	printf("digitalOuts:\n");
	for (doi = digitalOutMap.begin(); doi != digitalOutMap.end(); doi++) {
		printf("\t%s\t%02x\t%d\t%s\n",
		       doi->second->name.c_str(),
		       doi->second->board->addr,
		       doi->second->index,
		       doi->second->state[0] ? "on" : "off");
	}

	printf("analogOuts:\n");
	for (aoi = analogOutMap.begin(); aoi != analogOutMap.end(); aoi++) {
		printf("\t%s\t%02x\t%d\t%d\n",
		       aoi->second->name.c_str(),
		       aoi->second->board->addr,
		       aoi->second->index,
		       aoi->second->val[0]);
	}

	printf("\ndigitalIns:\n");
	for (dii = digitalInMap.begin(); dii != digitalInMap.end(); dii++) {
		printf("\t%s\t%02x\t%d\t%s\n",
		       dii->second->name.c_str(),
		       dii->second->board->addr,
		       dii->second->index,
		       dii->second->state[0] ? "on" : "off");
	}

	printf("\nanalogIns:\n");
	for (aii = analogInMap.begin(); aii != analogInMap.end(); aii++) {
		printf("\t%s\t%02x\t%d\t%d\n",
		       aii->second->name.c_str(),
		       aii->second->board->addr,
		       aii->second->index,
		       aii->second->val[0]);
	}

	printf("lightOuts:\n");
	for (loi = lightOutMap.begin(); loi != lightOutMap.end(); loi++) {
		printf("\t%s\t%02x\t%d\t%02x %02x %02x\n",
		       loi->second->name.c_str(),
		       loi->second->board->addr,
		       loi->second->index,
		       loi->second->red[0],
		       loi->second->green[0],
		       loi->second->blue[0]);
	}

}

void State::setDigitalOut(const char* name, bool state)
{
	map<string, DigitalOut *>::iterator i;

	i = digitalOutMap.find(name);
	if (i != digitalOutMap.end()) {
		i->second->state[activePage] = state;
		return;
	}

	fprintf(stderr, "WARNING trying to set non-existant digital out %s\n", name);
}

void State::setAnalogOut(const char* name, int val)
{
	map<string, AnalogOut *>::iterator i;

	i = analogOutMap.find(name);
	if (i != analogOutMap.end()) {
		i->second->val[activePage] = val;
		return;
	}

	fprintf(stderr, "WARNING trying to set non-existant analog out %s\n", name);
}

bool State::getDigitalIn(const char* name)
{
	map<string, DigitalIn *>::iterator i;

	i = digitalInMap.find(name);
	if (i != digitalInMap.end()) {
		return i->second->state[activePage];
	}

	fprintf(stderr, "WARNING trying to get non-existant digital in %s\n", name);

	return false;
}

int State::getAnalogIn(const char* name)
{
	map<string, AnalogIn *>::iterator i;

	i = analogInMap.find(name);
	if (i != analogInMap.end()) {
		return i->second->val[activePage];
	}

	fprintf(stderr, "WARNING trying to get non-existant analog in %s\n", name);

	return 0;
}

void State::setLightOut(const char *name, uint8_t red, uint8_t green, uint8_t blue)
{
	map<string, LightOut *>::iterator i;

	i = lightOutMap.find(name);
	if (i != lightOutMap.end()) {
		i->second->red[activePage] = red;
		i->second->green[activePage] = green;
		i->second->blue[activePage] = blue;
		return;
	}

	fprintf(stderr, "WARNING trying to set non-existant light out %s\n", name);
}


bool State::attachLink(const char *busName, Link *link)
{
	map<string, Bus *>::iterator i;

	i = busMap.find(busName);
	if (i == busMap.end())
		return false;

	i->second->thread = new ProtoThread(i->second, link);
	return true;
}

void State::run(void)
{
	vector<Bus *>::iterator i;

	for (i = busses.begin(); i != busses.end(); i++) {
		printf("starting bus %s\n", (*i)->name.c_str());
		if ((*i)->thread) {
			printf("starting thread\n");
			(*i)->thread->start();
		}
	}
}

void State::sync(void)
{
	vector<Bus *>::iterator i;

	for (i = busses.begin(); i != busses.end(); i++)
		(*i)->thread->waitDone();

	activePage = !activePage;

	for (i = busses.begin(); i != busses.end(); i++)
		(*i)->thread->clearDone();
}

/*
 * XML parsing
 */

bool State::loadConfig(const char *fileName)
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

	for( child = cur->xmlChildrenNode;
	     child != NULL;
	     child = child->next) {
		if (!xmlStrcmp(child->name, (const xmlChar*)"bus")) {
			ret = parseBus(child);
			if (!ret)
				goto err0;
		}
	}

err0:
	xmlFreeDoc(doc);

	dumpConfig();

	return ret;

}


bool State::parseBus(xmlNodePtr node)
{
	xmlNodePtr child;
	xmlChar *name;
	Bus *bus;
	Board *board;
	map<uint8_t, Board *>::iterator i;
	map<string, BoardConfig *>::iterator bi;

	name = xmlGetProp(node, (const xmlChar *)"name");

	if (name == NULL) {
		fprintf(stderr, "bus has no name\n");
		return false;
	}

	bus = new Bus;
	bus->name = (char *)name;
	bus->thread = NULL;
	busMap[bus->name] = bus;
	busses.push_back(bus);

	for( child = node->xmlChildrenNode;
	     child != NULL;
	     child = child->next) {
		bi = boardConfigs.find((char *) child->name);

		if (child->type != XML_ELEMENT_NODE)
			continue;

		if (bi == boardConfigs.end()) {
			fprintf(stderr, "unkown board type %s\n", child->name);
			continue;
		}
		board = parseBoard(child, bi->second);
		if (board == NULL) {
			goto err0;
		}

		i = bus->addrMap.find(board->addr);
		if (i != bus->addrMap.end())
			fprintf(stderr, "WARNING: addr 0x%02x duplicated on bus %s\n",
				board->addr, bus->name.c_str());
		board->bus = bus;

		bus->boards.push_back(board);
		bus->addrMap[board->addr] = board;
	}


	xmlFree(name);

	return true;

err0:
	delete bus;
	xmlFree(name);

	return false;
}

Board *State::parseBoard(xmlNodePtr node, BoardConfig *config)
{
	xmlNodePtr child;
	Board *b;
	xmlChar *addr;

	addr = xmlGetProp(node, (const xmlChar *)"addr");

	if (addr == NULL) {
		fprintf(stderr, "%s has no addr\n", node->name);
		return NULL;
	}

	b = new Board(config->maxDigitalOuts, config->maxAnalogOuts,
		      config->maxDigitalIns, config->maxAnalogIns,
		      config->maxLightOuts);

	if (!get_uint8((const char *)addr, &b->addr)) {
		fprintf(stderr, "%s is not a valid addr\n", addr);
		goto err0;
	}

	for( child = node->xmlChildrenNode;
	     child != NULL;
	     child = child->next) {
		map<string, string>::iterator i;
		const char *name;

		if (child->type != XML_ELEMENT_NODE)
			continue;

		i = aliases.find((char *)child->name);
		if (i != aliases.end())
			name = i->second.c_str();
		else
			name = (char *)child->name;

		if (!strcmp(name, "digitalOut")) {
			DigitalOut *out = parseDigitalOut(child);

			if (out == NULL)
				goto err0;

			if (out->index > config->maxDigitalOuts) {
				fprintf(stderr, "%s %d index is to high in %s %02x\n",
					child->name, out->index,
					node->name, b->addr);
				delete out;
				goto err0;
			}

			out->board = b;

			b->digitalOuts[out->index] = out;
		} else if (!strcmp(name, "analogOut")) {
			AnalogOut *out = parseAnalogOut(child);

			if (out == NULL)
				goto err0;

			if (out->index > config->maxAnalogOuts) {
				fprintf(stderr, "%s %d index is to high in %s %02x\n",
					child->name, out->index,
					node->name, b->addr);
				delete out;
				goto err0;
			}

			out->board = b;

			b->analogOuts[out->index] = out;
		} else if (!strcmp(name, "digitalIn")) {
			DigitalIn *in = parseDigitalIn(child);

			if (in == NULL)
				goto err0;

			if (in->index > config->maxDigitalIns) {
				fprintf(stderr, "%s %d index is to high in %s %02x\n",
					child->name, in->index,
					node->name, b->addr);
				delete in;
				goto err0;
			}

			in->board = b;

			b->digitalIns[in->index] = in;
		} else if (!strcmp(name, "analogIn")) {
			AnalogIn *in = parseAnalogIn(child);

			if (in == NULL)
				goto err0;

			if (in->index > config->maxAnalogIns) {
				fprintf(stderr, "%s %d index is to high in %s %02x\n",
					child->name, in->index,
					node->name, b->addr);
				delete in;
				goto err0;
			}

			in->board = b;

			b->analogIns[in->index] = in;
		} else if (!strcmp(name, "lightOut")) {
			LightOut *light = parseLightOut(child);

			if (light == NULL)
				goto err0;

			if (light->index > config->maxLightOuts) {
				fprintf(stderr, "%s %d index is to high in %s %02x\n",
					child->name, light->index,
					node->name, b->addr);
				delete light;
				goto err0;
			}

			light->board = b;

			b->lightOuts[light->index] = light;
		} else {
			fprintf(stderr, "unknown port type %s in %s %02x\n",
				child->name, node->name, b->addr);
		}
	}


	xmlFree(addr);
	return b;

err0:
	// XXX: need to clean up ports

	delete b;
	xmlFree(addr);

	return NULL;
}

DigitalOut *State::parseDigitalOut(xmlNodePtr node)
{
	DigitalOut *out;
	xmlChar *indexStr;
	xmlChar *name;
	uint8_t index;
	map<string, DigitalOut *>::iterator i;


	indexStr = xmlGetProp(node, (const xmlChar *)"index");

	if (indexStr == NULL) {
		fprintf(stderr, "%s has no index\n", node->name);
		return NULL;
	}

	if (!get_uint8((const char *)indexStr, &index)) {
		fprintf(stderr, "%s is not a valid index\n", indexStr);
		goto err0;
	}

	name = xmlGetProp(node, (const xmlChar *)"name");

	if (name == NULL) {
		fprintf(stderr, "%s has no name\n", node->name);
		goto err0;
	}

	out = new DigitalOut;
	out->name = (char *)name;
	out->index = index;
	out->state[0] = false;
	out->state[1] = false;

	i = digitalOutMap.find(out->name);
	if (i != digitalOutMap.end())
		fprintf(stderr, "WARNING: digitalOut %s redefined\n",
			out->name.c_str());

	digitalOutMap[out->name] = out;

	xmlFree(indexStr);
	xmlFree(name);

	return out;
err0:
	xmlFree(indexStr);

	return NULL;
}

AnalogOut *State::parseAnalogOut(xmlNodePtr node)
{
	AnalogOut *out;
	xmlChar *indexStr;
	xmlChar *name;
	uint8_t index;
	map<string, AnalogOut *>::iterator i;


	indexStr = xmlGetProp(node, (const xmlChar *)"index");

	if (indexStr == NULL) {
		fprintf(stderr, "%s has no index\n", node->name);
		return NULL;
	}

	if (!get_uint8((const char *)indexStr, &index)) {
		fprintf(stderr, "%s is not a valid index\n", indexStr);
		goto err0;
	}

	name = xmlGetProp(node, (const xmlChar *)"name");

	if (name == NULL) {
		fprintf(stderr, "%s has no name\n", node->name);
		goto err0;
	}

	out = new AnalogOut;
	out->name = (char *)name;
	out->index = index;
	out->val[0] = 0;
	out->val[1] = 0;

	i = analogOutMap.find(out->name);
	if (i != analogOutMap.end())
		fprintf(stderr, "WARNING: digitalOut %s redefined\n",
			out->name.c_str());

	analogOutMap[out->name] = out;

	xmlFree(indexStr);
	xmlFree(name);

	return out;
err0:
	xmlFree(indexStr);

	return NULL;
}

DigitalIn *State::parseDigitalIn(xmlNodePtr node)
{
	DigitalIn *in;
	xmlChar *indexStr;
	xmlChar *name;
	uint8_t index;
	map<string, DigitalIn *>::iterator i;


	indexStr = xmlGetProp(node, (const xmlChar *)"index");

	if (indexStr == NULL) {
		fprintf(stderr, "%s has no index\n", node->name);
		return NULL;
	}

	if (!get_uint8((const char *)indexStr, &index)) {
		fprintf(stderr, "%s is not a valid index\n", indexStr);
		goto err0;
	}

	name = xmlGetProp(node, (const xmlChar *)"name");

	if (name == NULL) {
		fprintf(stderr, "%s has no name\n", node->name);
		goto err0;
	}

	in = new DigitalIn;
	in->name = (char *)name;
	in->index = index;
	in->state[0] = false;
	in->state[1] = false;

	i = digitalInMap.find(in->name);
	if (i != digitalInMap.end())
		fprintf(stderr, "WARNING: digitalIn %s redefined\n",
			in->name.c_str());

	digitalInMap[in->name] = in;

	xmlFree(indexStr);
	xmlFree(name);

	return in;
err0:
	xmlFree(indexStr);

	return NULL;
}

AnalogIn *State::parseAnalogIn(xmlNodePtr node)
{
	AnalogIn *in;
	xmlChar *indexStr;
	xmlChar *name;
	uint8_t index;
	map<string, AnalogIn *>::iterator i;


	indexStr = xmlGetProp(node, (const xmlChar *)"index");

	if (indexStr == NULL) {
		fprintf(stderr, "%s has no index\n", node->name);
		return NULL;
	}

	if (!get_uint8((const char *)indexStr, &index)) {
		fprintf(stderr, "%s is not a valid index\n", indexStr);
		goto err0;
	}

	name = xmlGetProp(node, (const xmlChar *)"name");

	if (name == NULL) {
		fprintf(stderr, "%s has no name\n", node->name);
		goto err0;
	}

	in = new AnalogIn;
	in->name = (char *)name;
	in->index = index;
	in->val[0] = 0;
	in->val[1] = 0;

	i = analogInMap.find(in->name);
	if (i != analogInMap.end())
		fprintf(stderr, "WARNING: digitalIn %s redefined\n",
			in->name.c_str());

	analogInMap[in->name] = in;

	xmlFree(indexStr);
	xmlFree(name);

	return in;
err0:
	xmlFree(indexStr);

	return NULL;
}

LightOut *State::parseLightOut(xmlNodePtr node)
{
	LightOut *out;
	xmlChar *indexStr;
	xmlChar *name;
	uint8_t index;
	map<string, LightOut *>::iterator i;


	indexStr = xmlGetProp(node, (const xmlChar *)"index");

	if (indexStr == NULL) {
		fprintf(stderr, "%s has no index\n", node->name);
		return NULL;
	}

	if (!get_uint8((const char *)indexStr, &index)) {
		fprintf(stderr, "%s is not a valid index\n", indexStr);
		goto err0;
	}

	name = xmlGetProp(node, (const xmlChar *)"name");

	if (name == NULL) {
		fprintf(stderr, "%s has no name\n", node->name);
		goto err0;
	}

	out = new LightOut;
	out->name = (char *)name;
	out->index = index;
	out->red[0] = 0x00;
	out->green[0] = 0x00;
	out->blue[0] = 0x00;
	out->red[1] = 0x00;
	out->green[1] = 0x00;
	out->blue[1] = 0x00;

	i = lightOutMap.find(out->name);
	if (i != lightOutMap.end())
		fprintf(stderr, "WARNING: digitalOut %s redefined\n",
			out->name.c_str());

	lightOutMap[out->name] = out;

	xmlFree(indexStr);
	xmlFree(name);

	return out;
err0:
	xmlFree(indexStr);

	return NULL;
}
