#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <string>
#include <iostream>

#include <TtyLink.hh>
#include <Proto.hh>

class Handler : public ProtoHandler {
private:
	uint8_t status;
	bool done;
public:
	virtual ~Handler() {}

	void resetDone(void) {
		done = false;
	}

	bool isDone(void) {
		return done;
	}

	uint8_t getStatus(void) {
		return status;
	}

	virtual void relay(uint8_t idx, uint8_t state) {
	}

	virtual void light(uint8_t idx, uint8_t val) {
	}

	virtual void dpot(uint8_t idx, uint8_t val) {
	}

	virtual void resp(struct proto_packet *pkt) {
		switch (pkt->cmd) {
		case PROTO_CMD_GET_STATUS:
		case PROTO_CMD_SWITCH_QUERY:
		case PROTO_CMD_ADC_QUERY_LO:
		case PROTO_CMD_ADC_QUERY_HI:
			status = pkt->val;
			done = true;
			break;
		}
	}
};

void usage(void)
{
	fprintf(stderr, "usage: ledsend tty-device\n");
}

bool get_uint8(char *s, uint8_t *val)
{
	char *endp;
	long v;

	v = strtol(s, &endp, 0);
	if (endp == NULL || endp == s)
		return false;
	if (v < 0 || v > 0xff)
		return false;

	*val = v & 0xff;
	 return true;
 }

int main(int argc, char *argv[])
{
	if (argc != 2) { usage(); exit(1) ;}
	TtyLink link(argv[1]);
	Handler h;
	Proto p(&link, &h, NULL, 0, 1);
	std::string line;

	argv++;
	argc--;

	for (;;) {
		getline(std::cin, line);
		if ( ! std::cin)
			break;
		int r1, r2, g1, g2, g3, b1, b2, b3;
		if (sscanf(line.c_str(), "%d %d %d %d %d %d %d %d", 
			&r1, &r2, &g1, &g2, &g3, &b1, &b2, &b3) != 8) {
			printf("bad line: %s\n", line.c_str());
			continue;
		}
		printf( "setLight(0x%02x, 0x%02x, 0x%02x)\n", 0, 0, r1);
		p.setLight(0, 0, r1);
		printf( "setLight(0x%02x, 0x%02x, 0x%02x)\n", 0, 3, r2);
		p.setLight(0, 3, r2);
		printf( "setLight(0x%02x, 0x%02x, 0x%02x)\n", 0, 1, g1);
		p.setLight(0, 1, g1);
		printf( "setLight(0x%02x, 0x%02x, 0x%02x)\n", 0, 4, g2);
		p.setLight(0, 4, g2);
		printf( "setLight(0x%02x, 0x%02x, 0x%02x)\n", 0, 6, g3);
		p.setLight(0, 6, g3);
		printf( "setLight(0x%02x, 0x%02x, 0x%02x)\n", 0, 2, b1);
		p.setLight(0, 2, b1);
		printf( "setLight(0x%02x, 0x%02x, 0x%02x)\n", 0, 5, b2);
		p.setLight(0, 5, b2);
		printf( "setLight(0x%02x, 0x%02x, 0x%02x)\n", 0, 7, b3);
		p.setLight(0, 7, b3);
	}

	return 0;
}
