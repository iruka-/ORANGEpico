#!/usr/bin/perl

# ==================================================
#	mapファイルのダイジェストを表示.
# ==================================================

# 入力は main.map
# ==================================================
# 表示内容は、リンカの各セクションのバイトサイズ.
# 表示オプション
#   '-t'  テキストセクションのサイズを表示.
#   '-d'  データセクションのサイズを表示.
#   '-r'  roデータセクションのサイズを表示.
#   '-b'  bssセクションのサイズを表示.
#	'-xアドレス'	停止箇所から該当モジュールを突き止める.
# ==================================================


use strict;

#my $mapfile = 'main.map';
my $mapfile = 'build/firmware.elf.map';
my $total = 0;
my $opt_t = 0;
my $opt_d = 0;
my $opt_r = 0;
my $opt_b = 0;
my $opt_x = 0;

# ==================================================
#	パス名の".." 以降の文字列を返す.
# ==================================================
sub	shortcut()
{
	my ($path) = @_;
	my $i = rindex($path,'..');
	if($i<0) {return $path;}
	return substr($path,$i);
}
# ==================================================
#	" .text        0x0000000000000000    0x93000  foo.c"
#     .text          0x0000000000100000      0x1d8 crt0.o
# 	このような行をダイジェスト表示する.
# ==================================================
sub print_section()
{
	my ($line)=@_;
	my @row=split(/[ ]+/ , $line);
	my $offset = hex($row[2]);  # rva
	my $size   = hex($row[3]);  # ここで0x93000 などのヘキサを数値にする.
	printf("%-8s %08x %10s %10d %s\n", 
		$row[1] , $offset , $row[3] ,  $size , &shortcut($row[4]) );	
}
# ==================================================
#	".text        0x0000000000000000    0x93000"
# 	このような行を表示して、サイズを積算する.
# ==================================================
sub print_map()
{
	my ($line)=@_;
	if($line =~ /^[.][a-zA-Z0-9]+ / ) {
		my @row=split(/[ ]+/ , $line);
		my $size = hex($row[2]);  # ここで0x93000 などのヘキサを数値にする.

		printf("%s %8d\n", $line , $size);
		$total += $size;
		return ;
	}

	if($line =~ /^ [.]text / ) {
		if($opt_t) {&print_section($line);}
	}
	if($line =~ /^ [.]data / ) {
		if($opt_d) {&print_section($line);}
	}
	if($line =~ /^ [.]rodata / ) {
		if($opt_r) {&print_section($line);}
	}
	if($line =~ /^ [.]bss / ) {
		if($opt_b) {&print_section($line);}
	}
}

sub read_map()
{
	my $line;
	my $id;
	my @idbuf;
	
	open(FILE,$mapfile) || Die("Can't open file $mapfile\n");
	while(<FILE>) {
		$line = $_;
		$line =~ s/\n//;
		$line =~ s/\r//;
		&print_map($line);
	}
	print "----------------------------------------\n";
	printf("       total = %d (0x%x) bytes\n",$total,$total);
	close(FILE);
}


sub	main()
{
	# オプション解析.
	my $option;
	foreach $option (@ARGV) {
		if($option eq '-t') {$opt_t = 1;}
		if($option eq '-d') {$opt_d = 1;}
		if($option eq '-r') {$opt_r = 1;}
		if($option eq '-b') {$opt_b = 1;}
		if($option eq '-x') {$opt_x = 1;}
	}
	# map 読みこみ.
	&read_map();
}

&main();

