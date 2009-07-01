#include <stdint.h>
#include <stdio.h>

#include <util.h>

#include <UdpLink.hh>
#include <Proto.hh>

struct proto_widget widgets[] = {
	PROTO_WIDGET_RELAY(0,10),
	PROTO_WIDGET_RELAY(1,10),
	PROTO_WIDGET_RELAY(2,10),
	PROTO_WIDGET_RELAY(3,10),
	PROTO_WIDGET_RELAY(4,10),
	PROTO_WIDGET_RELAY(5,10),
	PROTO_WIDGET_RELAY(6,10),
	PROTO_WIDGET_RELAY(7,10),
	PROTO_WIDGET_LIGHT(0),
	PROTO_WIDGET_LIGHT(1),
	PROTO_WIDGET_LIGHT(2),
};

class WaiterHandler : public ProtoHandler {
private:
	int tmp;
public:
	virtual ~WaiterHandler() {}

	virtual void relay(uint8_t idx, uint8_t state) {
		printf("relay %d %s\n", idx, state?"set":"cleared");
	}

	virtual void light(uint8_t idx, uint8_t val) {
		printf("light %d set to %02x\n", idx, val);
	}

	virtual void resp(struct proto_packet *pkt) {
	}
};

int main(int argc, char *argv[])
{
	UdpLink link(6502, "localhost", 8051);
	WaiterHandler h;
	int err;

	Proto p(&link, &h, widgets, ARRAY_SIZE(widgets), 0);

	while (1) {
		err = p.waitForMsg(100);
		if (err <= 0) {
			p.ping();
			continue;
		}
	}

	return 0;
}
