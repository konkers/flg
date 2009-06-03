#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <UdpLink.hh>
#include <Proto.hh>


int main(int argc, char *argv[])
{
	UdpLink link(8051, "localhost", 6502);
	Proto p(&link, 1);

	p.sendMsg(0, 0xca, 0xfe);

	return 0;
}
