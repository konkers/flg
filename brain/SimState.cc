
#include "SimState.hh"
#include "MapFileParser.hh"

#include "mongoose.h"

static struct proto_widget relay3_widgets[] = {
	PROTO_WIDGET_RELAY(0,100),
	PROTO_WIDGET_RELAY(1,100),
	PROTO_WIDGET_RELAY(2,100),
};

void handle_relay(void *data, uint8_t idx, uint8_t state)
{
	SimState::SimProto *p = (SimState::SimProto *)data;
	printf("%s: relay %d = %d\n", p->name.c_str(), idx, state);
}

void handle_long_data(void *data, uint32_t val)
{
	SimState::SimProto *p = (SimState::SimProto *)data;
	p->sim->handleLongData(p, val);

}

void handle_sync(void *data)
{
	SimState::SimProto *p = (SimState::SimProto *)data;
	printf("%s: sync\n", p->name.c_str());
}

void handle_send(void *data, uint8_t *pkt_data, int len)
{
	SimState::SimProto *p = (SimState::SimProto *)data;
	int i;

	for (i = 0; i < len; i++)
		p->sim->insertByte(pkt_data[i]);
}


void SimState::handleLongData(SimProto *p, uint32_t val)
{
	int i = ledMapping[p->name];
	if (i >= 0 && i < nLights) {
		lightState[i] = val;
	}
}

void SimState::insertByte(uint8_t c)
{
	outputBuff->insert(c);
}

void state_page(struct mg_connection *conn,
		const struct mg_request_info *ri, void *data)
{
	SimState *s = (SimState *)data;
	s->statePage(conn, ri);
}



SimState::SimState(void)
{
	handlers.relay= handle_relay;
	handlers.long_data = handle_long_data;
	handlers.sync = handle_sync;

	memset(lightState, 0x0, sizeof(lightState));

	outputBuff = new RingBuff(outputBuffSize);
}

bool SimState::loadLightMapping(const char * fileName)
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
	p->p.handlers = &handlers;

	proto_init(&p->p, addr);

	protos.insert(protos.end(), p);
}

void SimState::addRelay3(string name, uint8_t addr)
{
	SimProto *p = new SimProto;

	p->name = name;
	p->sim = this;

	memset(&p->p, 0x0, sizeof(p->p));
	p->p.addr = addr;
	p->p.handler_data = p;
	p->p.handlers = &handlers;
	p->p.widgets = relay3_widgets;

	proto_init(&p->p, addr);

	protos.insert(protos.end(), p);
}

void SimState::startWebServer(int port)
{
	char buff[16];
	snprintf(buff, 16, "%d", port);
	ctx = mg_start();
	mg_set_option(ctx, "ports", buff);
	mg_set_uri_callback(ctx, "/soma/state", &state_page, this);
//	mg_set_uri_callback(ctx, "/soma/button/*", &button_page, NULL);
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
}



void SimState::send(uint8_t c)
{
	vector<SimProto *>::iterator i;

	for (i = protos.begin(); i != protos.end(); i++)
		proto_recv(&(*i)->p, c);
}

void SimState::ping(void)
{
	vector<SimProto *>::iterator i;

	for (i = protos.begin(); i != protos.end(); i++)
		proto_ping(&(*i)->p);
}

bool SimState::hasData(void)
{
	return outputBuff->length() > 0;
}

uint8_t SimState::recv(void)
{
	return outputBuff->remove();
}

