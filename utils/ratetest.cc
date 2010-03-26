#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>

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
	h->resetDone();
	p->getStatus(addr);
	p->flush();
	while (!h->isDone()) {
		p->waitForMsg(100);
	}
}

void do_write(Proto *p, uint8_t addr)
{
	p->setRelay(addr, 0x0);
}




int main(int argc, char *argv[])
{
	FtdiLink link(0x0403, 0x6010, INTERFACE_A);
	Handler h;
	Proto p(&link, &h, NULL, 0, 0, true);
	uint8_t addr = 0x01;
	int i, tries;
	int n_tries = 100;
	int n_reads = 7;
	int n_writes = 16;
	struct timeval start, end, diff;
	float t;

	gettimeofday(&start, NULL);
	for (tries = 0; tries < n_tries; tries++) {
		for (i = 0; i < n_writes; i++)
			do_write(&p, addr);
		p.flush();
		for (i = 0; i < n_reads; i++)
			do_read(&p, &h, addr);
	}
	gettimeofday(&end, NULL);
	timersub(&end, &start, &diff);

	t = (float)diff.tv_sec + (float)diff.tv_usec / 1000000.0;
	printf("%fs\n", t);
	printf("%f Hz\n", 1.0 / t * n_tries);
	
	return 0;
}
