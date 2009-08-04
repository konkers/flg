#!/usr/bin/perl

my $DPOT_ADDR = "0x03";
my $INPUT_ADDR = "0x02";

sub set_dpot {
  my $addr = shift;
  my $dpot = shift;
  my $val = shift;

  system("sender485 set_dpot $addr $dpot $val > /dev/null");
}

sub get_adc {
  my $addr = shift;
  my $adc = shift;

  $val = `sender485 get_adc $addr $adc`;
  if ($val =~ /getAdc\(0x[[:xdigit:]]{2}, 0x[[:xdigit:]]{2}\) ([[:xdigit:]]{4})/) {
    return hex($1);
  } else {
    print "bad return '$val'\n";
    return -1;
  }
}

sub test_adc {
  my $dpot_addr = shift;
  my $input_addr = shift;
  my $val0 = shift;
  my $val1 = shift;
  my $fuzz = shift;

  printf("test_adc(0x%02x, 0x%02x, 0x%02x, 0x%02x, %d) = ",
	 $dpot_addr, $input_addr, $val0, $val1, $fuzz);

  set_dpot($dpot_addr, 0x00, $val0);
  set_dpot($dpot_addr, 0x01, $val1);

  $a0 = 0x3ff - get_adc($input_addr, 0x00);
  $a1 = 0x3ff - get_adc($input_addr, 0x01);

  if (abs(($val0<<2) - $a1) > $fuzz) {
    die "dpot0 out of range";
  }

  if (abs(($val1<<2) - $a0) > $fuzz) {
    die "dpot1 out of range";
  }

  printf("(0x%03x, 0x%03x)\n", $a1, $a0);
}

test_adc($DPOT_ADDR, $INPUT_ADDR, 0x0, 0x0, 0x10);
test_adc($DPOT_ADDR, $INPUT_ADDR, 0x80, 0x0, 0x10);
test_adc($DPOT_ADDR, $INPUT_ADDR, 0xff, 0x0, 0x10);
test_adc($DPOT_ADDR, $INPUT_ADDR, 0x00, 0xff, 0x10);
test_adc($DPOT_ADDR, $INPUT_ADDR, 0x00, 0x80, 0x10);
test_adc($DPOT_ADDR, $INPUT_ADDR, 0xff, 0xff, 0x10);

print "\n--------------------------\n";
print "      PASSED\n";
print "--------------------------\n";
