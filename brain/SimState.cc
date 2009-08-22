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

#include <util.h>


#include "SimState.hh"
#include "MapFileParser.hh"

#include "mongoose.h"

static struct proto_widget relay3_widgets[] = {
	PROTO_WIDGET_RELAY(0,100),
	PROTO_WIDGET_RELAY(1,100),
	PROTO_WIDGET_RELAY(2,100),
};

static struct proto_widget input_widgets[] = {
        PROTO_WIDGET_SWITCH(0),
        PROTO_WIDGET_ADC(0),
        PROTO_WIDGET_ADC(1),
};


void handle_relay(void *data, uint8_t idx, uint8_t state)
{
	SimState::SimProto *p = (SimState::SimProto *)data;
	p->sim->handleRelay(p, idx, state);
}

void handle_long_data(void *data, uint32_t val)
{
	SimState::SimProto *p = (SimState::SimProto *)data;
	p->sim->handleLongData(p, val);

}

void handle_sync(void *data)
{
//	SimState::SimProto *p = (SimState::SimProto *)data;
}

void handle_flame_send(void *data, uint8_t *pkt_data, int len)
{
	SimState::SimProto *p = (SimState::SimProto *)data;
	int i;

	for (i = 0; i < len; i++)
		p->sim->insertFlameByte(pkt_data[i]);
}

void handle_led_send(void *data, uint8_t *pkt_data, int len)
{
	SimState::SimProto *p = (SimState::SimProto *)data;
	int i;

	for (i = 0; i < len; i++)
		p->sim->insertLedByte(pkt_data[i]);
}


void SimState::handleRelay(SimProto *p, uint8_t idx, uint8_t state)
{
	int relay = (p->p.addr - 0x90) * 3 + idx;

	relayState[relay] = state != 0;
}


void SimState::handleLongData(SimProto *p, uint32_t val)
{
	int i = ledMapping[p->name];

	if (i >= 0 && i < nLights) {
		lightState[i] = val;
	}
}

void SimState::insertFlameByte(uint8_t c)
{
	flameOutputBuff->insert(c);
}

void SimState::insertLedByte(uint8_t c)
{
	ledOutputBuff->insert(c);
}

void state_page(struct mg_connection *conn,
		const struct mg_request_info *ri, void *data)
{
	SimState *s = (SimState *)data;
	s->statePage(conn, ri);
}

void button_page(struct mg_connection *conn,
		const struct mg_request_info *ri, void *data)
{
	SimState *s = (SimState *)data;
	s->buttonPage(conn, ri);
}


SimState::SimState(void)
{
	int i;

	flameHandlers.relay= handle_relay;
	flameHandlers.send= handle_flame_send;

	ledHandlers.long_data = handle_long_data;
	ledHandlers.sync = handle_sync;
	ledHandlers.send= handle_led_send;

	memset(lightState, 0x0, sizeof(lightState));

	for (i = 0; i < nButtons; i++)
		buttonState[i] = false;

	for (i = 0; i < nRelays; i++)
		relayState[i] = false;

	flameOutputBuff = new RingBuff(outputBuffSize);
	// led output buff is not stricly needed as the led boards
	// in incapable of talkng back... obedient little slaves
	ledOutputBuff = new RingBuff(outputBuffSize);
}

bool SimState::loadLightMap(const char * fileName)
{
	MapFileParser p;
	char *key;
	int val;

	if (!p.open(fileName))
		return false;

	while (p.read(&key, &val))
		ledMapping[key] = val;

	return true;
}


void SimState::addLedRgb(string name, uint8_t addr)
{
	SimProto *p = new SimProto;

	p->name = name;
	p->sim = this;

	memset(&p->p, 0x0, sizeof(p->p));
	p->p.addr = addr;
	p->p.handler_data = p;
	p->p.handlers = &ledHandlers;

	proto_init(&p->p, addr);

	ledProtos.insert(ledProtos.end(), p);
	protoMap[addr] = p;
}

void SimState::addRelay3(string name, uint8_t addr)
{
	SimProto *p = new SimProto;

	p->name = name;
	p->sim = this;

	memset(&p->p, 0x0, sizeof(p->p));
	p->p.addr = addr;
	p->p.handler_data = p;
	p->p.handlers = &flameHandlers;
	p->p.widgets = new struct proto_widget[ARRAY_SIZE(relay3_widgets)];
	memcpy(p->p.widgets, relay3_widgets, sizeof(relay3_widgets));
	p->p.n_widgets = ARRAY_SIZE(relay3_widgets);

	proto_init(&p->p, addr);

	flameProtos.insert(flameProtos.end(), p);
	protoMap[addr] = p;
}

void SimState::addInput(string name, uint8_t addr)
{
	SimProto *p = new SimProto;

	p->name = name;
	p->sim = this;

	memset(&p->p, 0x0, sizeof(p->p));
	p->p.addr = addr;
	p->p.handler_data = p;
	p->p.handlers = &flameHandlers;
	p->p.widgets = new struct proto_widget[ARRAY_SIZE(input_widgets)];
	memcpy(p->p.widgets, input_widgets, sizeof(input_widgets));
	p->p.n_widgets = ARRAY_SIZE(input_widgets);

	proto_init(&p->p, addr);

	flameProtos.insert(flameProtos.end(), p);
	protoMap[addr] = p;
	buttonWidgetMap[addr] = &p->p.widgets[0];
}

void SimState::startWebServer(int port)
{
	char buff[16];
	snprintf(buff, 16, "%d", port);
	ctx = mg_start();
	mg_set_option(ctx, "ports", buff);
	mg_set_uri_callback(ctx, "/soma/state", &state_page, this);
	mg_set_uri_callback(ctx, "/soma/button/*", &button_page, this);
//	mg_set_uri_callback(ctx, "/soma/light/*", &light_page, NULL);
}

void SimState::statePage(struct mg_connection *conn,
			 const struct mg_request_info *ri)
{
	int i;

	mg_printf(conn, "HTTP/1.1 200 OK\r\n"
		  "content-Type: text/plain\r\n\r\n");

	for (i = 0; i < nLights; i++) {
		mg_printf(conn, "%02x: %08x\r\n", i, lightState[i]);
	}

	for (i = 0; i < nRelays; i++) {
		mg_printf(conn, "%02x: %08x\r\n", 0x80 + i, relayState[i]);
	}

}

void SimState::buttonPage(struct mg_connection *conn,
			  const struct mg_request_info *ri)
{
	char *num;
	char *dir;

	char *p;
	int button;
	int s;
	int sw;
	struct proto_widget *proto;

	/* XXX: probably buffer overflow action here */
	dir = strrchr(ri->uri, '/');
	*dir = '\0';
	dir++;

	num = strrchr(ri->uri, '/');
	num++;

	button = strtoul(num, &p, 0);
	if (p == NULL || p == num ) {
		fprintf(stderr, "buttonPage: bad integer %s\n", num);
		return;
	}

	if (button < 0 || button >= nButtons) {
		fprintf(stderr, "buttonPage: button %d out of range\n", button);
		return;
	}

	if (!strcmp(dir, "down")) {
		s = 1;
	} else if (!strcmp(dir, "up")) {
		s = 0;
	} else {
		fprintf(stderr, "buttonPage: unkown button action %s\n", dir);
		return;
	}

	if (button >= 0 && button < nButtons ){
		if (button < 8) {
			proto = buttonWidgetMap[0x80];
			sw = button;
		} else {
			proto = buttonWidgetMap[0x81];
			sw = button - 8;
		}
		if (s)
			proto_switch_set(proto, sw);
		else
			proto_switch_clear(proto, sw);
	} else {
		fprintf(stderr, "buttonPage: unknown button: %d %s\n", button, dir);
	}


	mg_printf(conn, "HTTP/1.1 200 OK\r\n"
		  "content-Type: text/plain\r\n\r\n");
}

void SimState::ping(void)
{
	vector<SimProto *>::iterator i;

	for (i = flameProtos.begin(); i != flameProtos.end(); i++)
		proto_ping(&(*i)->p);

	for (i = ledProtos.begin(); i != ledProtos.end(); i++)
		proto_ping(&(*i)->p);
}

void SimState::flameSend(uint8_t c)
{
	vector<SimProto *>::iterator i;

	for (i = flameProtos.begin(); i != flameProtos.end(); i++)
		proto_recv(&(*i)->p, c);
}

bool SimState::flameHasData(void)
{
	return flameOutputBuff->length() > 0;
}

uint8_t SimState::flameRecv(void)
{
	return flameOutputBuff->remove();
}

void SimState::ledSend(uint8_t c)
{
	vector<SimProto *>::iterator i;

	for (i = ledProtos.begin(); i != ledProtos.end(); i++)
		proto_recv(&(*i)->p, c);
}

bool SimState::ledHasData(void)
{
	return ledOutputBuff->length() > 0;
}

uint8_t SimState::ledRecv(void)
{
	return ledOutputBuff->remove();
}




