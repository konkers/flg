#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include <sys/time.h>

#include <string>
#include <iostream>
#include <sstream>

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

double now()
{
	struct timeval n;
	gettimeofday(&n, 0);
	return n.tv_sec + n.tv_usec * 1e-6;
}

int main(int argc, char *argv[])
{
	FtdiLink link(0x0403, 0x6010, INTERFACE_B, 115200 * 2);
	Handler h;
	Proto p(&link, &h, NULL, 0, 1);
	std::string line;
	double start_time = now();
	double last_time = start_time;
	int n = 0;

//	std::vector<uint8_t> vals;
//	vals.resize(led_cnt*4);
	uint8_t vals[4 * 80];
	for (;;) {
		getline(std::cin, line);
		if ( ! std::cin)
			break;
		std::istringstream linestream(line);

		unsigned led_cnt;
		int addr;

		linestream >> addr >> led_cnt;
		if (led_cnt == 0 || led_cnt > 100) {
			std::cerr << "bad led count " << led_cnt << std::endl;
			continue;
		}
		for (unsigned i = 0; i < led_cnt; ++i) {
			unsigned v;
			linestream >> v;
			vals[i*4] = v;
			linestream >> v;
			vals[i*4+1] = v;
			linestream >> v;
			vals[i*4+2] = v;
			linestream >> v;
			vals[i*4+3] = v;
		}
		if ( ! linestream) {
			std::cerr << "bad line " << line << std::endl;
		
		}
		double curr_time = now();
		printf( "%d t=%f avgHz=%f instHz=%f setLights(0x%02x, 0x%02x, "
			"[[0x%02x, 0x%02x, 0x%02x, 0x%02x, ...]])\n",
			n, curr_time-start_time, n/(curr_time-start_time),
			1/(curr_time-last_time), addr, led_cnt, vals[0],
			vals[1], vals[2], vals[3]);
		last_time = curr_time;
		++n;
		p.setLights(addr, (uint32_t *) &vals[0], led_cnt);
		p.sendSync(0xff);
		p.flush();
//		usleep(30000);
	}

	return 0;
}
