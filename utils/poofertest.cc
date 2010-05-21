#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>

#include <util.h>

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

void do_read(Proto *p, Handler *h, uint8_t addr)
{
	printf("do_read(0x%02x) =", addr);
	h->resetDone();
	p->getStatus(addr);
	p->flush();
	while (!h->isDone()) {
		p->waitForMsg(100);
	}
	printf("0x%02x\n", h->getStatus());
}

void do_write(Proto *p, uint8_t addr)
{
	printf("do_write(0x%02x)\n", addr);
	p->setRelay(addr, 0x0);
}


static uint8_t addrs[] = {
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,// 0x98, 0x99,
};

#define SLEEP 100000
int main(int argc, char *argv[])
{
	FtdiLink link(0x0403, 0x6010, INTERFACE_A);
	Handler h;
	Proto p(&link, &h, NULL, 0, 0, true);
	unsigned int i;

	while (1) {
		for (i = 0; i < ARRAY_SIZE(addrs); i++) {
			p.updateRelay(addrs[i], 0x1);
			p.flush();
			usleep(SLEEP);
			p.updateRelay(addrs[i], 0x2);
			p.flush();
			usleep(SLEEP);
			if (addrs[i] == 0x99)
				continue;
			p.updateRelay(addrs[i], 0x4);
			p.flush();
			usleep(SLEEP);
		}
	}

	return 0;
}
