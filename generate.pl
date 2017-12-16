#!/usr/bin/perl -w
use warnings;
use strict;

my $size = shift || die("Provide Trace Length");

my $clk = 0;

for(my $i = 0; $i < $size; $i++)
{
    my $time = int(rand(100));
    $clk +=  $time;
    my $addr = sprintf("%08x", 4*int(rand(255)));
    my $data = sprintf("%08x", int(rand(0xFFFFFFFF)));

    print "$clk : write 0x$addr 0x$data\n";
}
