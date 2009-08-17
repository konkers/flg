
#include "SimState.hh"
#include "MapFileParser.hh"

static struct proto_widget relay3_widgets[] = {
	PROTO_WIDGET_RELAY(0,100),
	PROTO_WIDGET_RELAY(1,100),
	PROTO_WIDGET_RELAY(2,100),
};

extern "C" void handle_relay(void *data, uint8_t idx, uint8_t state)
{
	printf("relay %d = %d\n", idx, state);
}

extern "C" void handle_long_data(void *data, uint32_t val)
{
	printf("long_data %08x\n", val);
}

extern "C" void handle_sync(void *data)
{
	printf("sync\n");

}


SimState::SimState(void)
{
	handlers.relay= handle_relay;
	handlers.long_data = handle_long_data;
	handlers.sync = handle_sync;
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


void SimState::send(uint8_t c)
{
	vector<SimProto *>::iterator i;

	for (i = protos.begin(); i != protos.end(); i++)
		proto_recv(&(*i)->p, c);
}

void SimState::ping(void)
{
	
}

bool SimState::hasData(void)
{
	return false;
}

