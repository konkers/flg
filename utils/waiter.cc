#include <stdint.h>
#include <stdio.h>

#include <UdpLink.hh>
#include <Proto.hh>


int main(int argc, char *argv[])
{
	UdpLink link(6502, "localhost", 8051);
	Proto p(&link, 0);
	struct proto_packet pkt;
	int err;

	while (1) {
		err = p.waitForMsg(&pkt, 1000);
		if (err <= 0) {
			printf("timeout %d\n", err);
			continue;
		}

		printf("msg a:%02x c:%02x v:%02x\n", 
		       pkt.addr, pkt.cmd, pkt.val);
	}

	return 0;
}
