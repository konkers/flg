#include <stdint.h>
#include <stdio.h>

#include <UdpLink.hh>


int main(int argc, char *argv[])
{
	uint8_t data[256];
	int len;
	UdpLink link(6502, "localhost", 8051);
	int err;

	while (1) {
		len = 256;
		err = link.wait(1000);
		if (err <= 0) {
			printf("timeout %d\n", err);
			continue;
		}

		if (link.recv(data,&len)) {
			printf("received %d bytes\n", len);
		}
	}
}
