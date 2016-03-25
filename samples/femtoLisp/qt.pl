#!/usr/bin/perl

use strict;

sub print_map()
{
	my ($line)=@_;
	print '"', $line . '\n"' . "\n";
}

sub main()
{
	my $line;
	my $id;
	my @idbuf;
	my $mapfile = $ARGV[0];
	open(FILE,$mapfile) || Die("Can't open file $mapfile\n");
	while(<FILE>) {
		$line = $_;
		$line =~ s/\n//;
		$line =~ s/\r//;
		&print_map($line);
	}
	close(FILE);
}
&main();

