#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <FtdiLink.hh>
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
   fprintf(stderr,
	   "usage: sender (<command> <args>*)*\n"
	   "\n"
	   "commands:\n"
	   "    get_status <addr>\n"
	   "    get_switch <addr>\n"
	   "    get_adc <addr> <idx>\n"
	   "    set_relay <addr> <val>\n"
	   "    clear_relay <addr> <val>\n"
	   "    set_dpot <addr> <idx> <val>\n"
	   "    set_addr <addr> <new_addr>\n");
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
	FtdiLink link(0x0403, 0x6010, INTERFACE_A);
	Handler h;
	Proto p(&link, &h, NULL, 0, 0);
	uint8_t addr;
	uint8_t idx;
	uint8_t val;

	argv++;
	argc--;

	while (argc < 1) {
		usage();
		return 1;
	}

	while (argc > 0) {
		if (!strcmp("set_relay", argv[0])) {
			if (argc < 3) {
				usage();
				return 1;
			}

			if (!get_uint8(argv[1], &addr) || !get_uint8(argv[2], &val)) {
				usage();
				return 1;
			}

			printf( "setRelay(0x%02x, 0x%02x)\n", addr, val);
			p.setRelay(addr, val);
			argc -= 3;
		} else if (!strcmp("clear_relay", argv[0])) {
			if (argc < 3) {
				usage();
				return 1;
			}

			if (!get_uint8(argv[1], &addr) || !get_uint8(argv[2], &val)) {
				usage();
				return 1;
			}

			printf( "clearRelay(0x%02x, 0x%02x)\n", addr, val);
			p.clearRelay(addr, val);
			argc -= 3;
		} else if (!strcmp("update_relay", argv[0])) {
			if (argc < 3) {
				usage();
				return 1;
			}

			if (!get_uint8(argv[1], &addr) || !get_uint8(argv[2], &val)) {
				usage();
				return 1;
			}

			printf( "updateRelay(0x%02x, 0x%02x)\n", addr, val);
			p.updateRelay(addr, val);
			argc -= 3;
		} else if (!strcmp("set_dpot", argv[0])) {
			if (argc < 4) {
				usage();
				return 1;
			}

			if (!get_uint8(argv[1], &addr) ||
			    !get_uint8(argv[2], &idx) ||
			    !get_uint8(argv[3], &val)) {
				usage();
				return 1;
			}

			printf( "setDpot(0x%02x, 0x%02x, 0x%02x)\n",
				addr, idx, val);
			p.setDpot(addr, idx, val);
			argc -= 4;
		} else if (!strcmp("set_addr", argv[0])) {
			if (argc < 3) {
				usage();
				return 1;
			}

			if (!get_uint8(argv[1], &addr) || !get_uint8(argv[2], &val)) {
				usage();
				return 1;
			}

			printf( "setAddr(0x%02x, 0x%02x)\n", addr, val);
			p.setAddr(addr, val);
			argc -= 3;
		} else if (!strcmp("get_status", argv[0])) {
			if (argc < 2) {
				usage();
				return 1;
			}

			if (!get_uint8(argv[1], &addr)) {
				usage();
				return 1;
			}

			printf( "getStatus(0x%02x)", addr);
			p.getStatus(addr);
			h.resetDone();
			while (!h.isDone()) {
				p.waitForMsg(100);
			}
			printf(" = %02x\n", h.getStatus());
			argc -= 2;
		} else if (!strcmp("get_switch", argv[0])) {
			if (argc < 2) {
				usage();
				return 1;
			}

			if (!get_uint8(argv[1], &addr)) {
				usage();
				return 1;
			}

			printf( "getSwitch(0x%02x)", addr);
			p.getSwitch(addr);
			h.resetDone();
			while (!h.isDone()) {
				p.waitForMsg(100);
			}
			printf(" = %02x\n", h.getStatus());
			argc -= 2;
		} else if (!strcmp("get_adc", argv[0])) {
			unsigned resp;
			if (argc < 3) {
				usage();
				return 1;
			}

			if (!get_uint8(argv[1], &addr)) {
				usage();
				return 1;
			}

			if (!get_uint8(argv[2], &val)) {
				usage();
				return 1;
			}

			printf( "getAdc(0x%02x, 0x%02x)", addr, val);
			h.resetDone();
			p.getAdcLo(addr, val);
			while (!h.isDone()) {
				p.waitForMsg(100);
			}
			resp = h.getStatus();
			h.resetDone();
			p.getAdcHi(addr, val);
			while (!h.isDone()) {
				p.waitForMsg(100);
			}
			resp |= h.getStatus() << 8;
			printf(" %04x\n", resp);
			argc -= 3;
		} else {
			usage();
			return 1;
		}
	}

	return 0;
}
