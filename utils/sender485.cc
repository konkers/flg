#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <TtyLink.hh>
#include <Proto.hh>


int main(int argc, char *argv[])
{
  TtyLink link("/dev/tty.usbserial-FTDW2WJ3");
  Proto p(&link, NULL, NULL, 0, 1);  

  p.setRelay(1, 0x5a);
  
  return 0;
}
