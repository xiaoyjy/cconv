#!/usr/bin/perl

use cconv;
my $s = cconv::cconv('utf-8', 'utf8-tw', "中华人民共和国\n");

print $s;
