#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <UdpLink.hh>
#include <Proto.hh>


int main(int argc, char *argv[])
{
	UdpLink link(8051, "localhost", 6502);
	Proto p(&link, NULL, NULL, 0, 1);

	p.setRelay(0, 0x5a);
	p.setLight(0, 0, 0xde);
	p.setLight(0, 1, 0xad);
	p.setLight(0, 2, 0x5a);

	return 0;
}
