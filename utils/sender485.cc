#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <TtyLink.hh>
#include <Proto.hh>

void usage(void)
{
  fprintf(stderr,
	  "usage: sender (<command> <args>*)*\n"
	  "\n"
	  "commands:\n"
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
  Proto p(&link, NULL, NULL, 0, 1);  
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
    } else {
      usage();
      return 1;
    }
  }

  return 0;
}
