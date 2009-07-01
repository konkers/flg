#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

	virtual void resp(struct proto_packet *pkt) {
		switch (pkt->cmd) {
		case PROTO_CMD_GET_STATUS:
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
	   "    set_relay <addr> <val>\n"
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
	TtyLink link("/dev/tty.usbserial-FTDW2WJ3");
	Handler h;
	Proto p(&link, &h, NULL, 0, 1);
	uint8_t addr;
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

			printf( "setStatus(0x%02x)", addr);
			p.getStatus(addr);
			h.resetDone();
			while (!h.isDone()) {
				p.waitForMsg(100);
			}
			printf(" = %02x\n", h.getStatus());
			argc -= 2;
		} else {
			usage();
			return 1;
		}
	}

	return 0;
}
