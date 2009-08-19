#!/usr/bin/perl

$filter = $ARGV[0];

while (<STDIN>) {
  s/\s*$//;
  @vals = split(/\s*,\s*/);
  if ($vals[0] =~ /^$filter/) {
    $val = hex($vals[2]);
    
    if ($vals[3] ne '') {
      $val = ($val << 8) | hex($vals[3]);
      printf("%s: 0x%04x\n", $vals[1], $val);
    } else {
      printf("%s: 0x%02x\n", $vals[1], $val);
    }

  }
}
