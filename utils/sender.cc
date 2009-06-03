#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <UdpLink.hh>


int main(int argc, char *argv[])
{
	char data[256];
	UdpLink link(8051, "localhost", 6502);

	strcpy(data, "test");
	if (!link.send((uint8_t *)data,strlen(data))) {
		printf("error sending data\n");
	}
}
