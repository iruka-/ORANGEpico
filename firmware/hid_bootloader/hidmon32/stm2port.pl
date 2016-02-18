#!/usr/bin/perl

#
# Convert: stm32f10x.h  to  portadrs.h
#
use strict;

my ($arg1,$arg2,$arg3);

#my $core_hdr  = '../../HW/Libraries/CMSIS/CM3/CoreSupport/core_cm3.h';
#my $stm32_hdr = '../../HW/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/stm32f10x.h';
my $core_hdr  = 'core_cm3.h';
my $stm32_hdr = 'stm32f10x.h';
my $lpc13xx_hdr='LPC13xx.h';

# ../../HW/Libraries/CMSIS/CM3/CoreSupport/core_cm3.h
# ../../HW/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/stm32f10x.h
# define TIM2                ((TIM_TypeDef *) TIM2_BASE)

my @buf;
my @hdr;
my @adrlist;
my %adrhash;       # アドレスハッシュ.
my %attrhash;       # アドレスハッシュ.

my %hash;          # 型名ハッシュ.
my @labels;

# =====================================================
#      
# =====================================================
sub loadfile()
{
	my ($file)=@_;
	my @buf;
	open(FILE,$file) || die("Can't open file $file");
	@buf = <FILE>;
	close(FILE);
	return @buf;
}

# =====================================================
#      
# =====================================================
sub push_hdr()
{
	my $line;
	foreach $line (@buf) {
		$line =~ s/\n//;
		$line =~ s/\r//;
		push @hdr,$line;
	}
}

# =====================================================
#      
# =====================================================
sub sub1()
{
	my $line;
	my $value;
	my $label;

	foreach $line(keys %hash) {
		$value = $line;
		$label = '"' . $hash{$value} . '"';
#		printf("	{%-16s,%s,0	},\n",$label , $value );
		push @labels,"$value,$label";
	}

	my @label = sort(@labels);
	foreach $line(@label) {
		($value,$label)=split(/,/,$line);
		printf("	{%-16s,%s,0	},\n",$label , $value );
	}

}
# =====================================================
#   型を登録する.
# =====================================================
sub regist_type()
{
	my ($typ,$str)=@_;
# 型名を$typに入れる.
	$typ =~ s/}//;
	$typ =~ s/;//;
	$typ =~ s/[ \t]+//;
	
	$hash{$typ} = $str;
}


# =====================================================
#      typedef の中身を登録する.
# =====================================================
sub get_struct()
{
	my $line;
	my $s=0;
	my $str='';
	my $typ;
	foreach $line (@hdr) {
		if($line =~ /^typedef.*struct/) {
#			print "\n------------------------------\n";
			$s=1;
			$str='';
		}
		elsif($line =~ /^}/) {
			$typ = $line;
			&regist_type($typ,$str);
			$s=0;
		}
		elsif($s)  {
#			print $line . "\n";
			($line) = split(/;/,$line);
			if($line !~ /^{/) {
				if($line ne '') {
				if($line !~ /RESERVED/) {
				if($line !~ /^#/) {
				if($line !~ /}/) {
				if($line !~ /{/) {
				if(substr($line,0,2) ne '//') {
					$str .= $line . "\n";

#					print ">$line<\n";

				}}}}}}
			}
		}
	}
}
sub get_member()
{
	my ($type)=@_;
	my $str = $hash{$type};
	my @lines = split(/\n/,$str);
	my $line;
	my @row;
	my $m;
	my $var;
	my $attr1;
	my $attr2;
	my @list=();

	undef %attrhash;

	foreach $line(@lines) {
		@row=split(/[ \t]+/,$line);
		$m=scalar(@row);
		$var = $row[$m-1];
		if(($m>=3)&&($row[$m-3] ne '')) {
			$attr1 = 'A' . $row[$m-3] . '|';
		}else{
			$attr1 = '';
		}
		$attr2 = 'A_' . $row[$m-2];

		push @list,$var;


		#==========
		$attrhash{$var}= $attr1 . $attr2;
	}
	return @list;
}
sub portdef()
{
	my ($port,$type,$addr)=@_;
	my ($member,$var,$var2);
	$type =~ s/^\(\(//;
	$addr =~ s/\)//;

#	print "$port $type $addr\n";

	my @memberlist = &get_member($type);

	print "// port=$port , type=$type\n";

	foreach $member (@memberlist) {
		$var = $port . "->" . "$member" ;
		$var2 = $port . "." . "$member" ;

		$var =~ s/\[[1-9]+\]/[0]/;

		print '	printf("%32s = %x\n","' . "$var2\",\t" . "&(" . $var . "));\n";
	}
	print "\n// --------------\n";
}
sub print_portdef_1()
{
	my ($port,$type,$addr)=@_;
	my ($member,$var,$var2,$adr,$attrib);
	$type =~ s/^\(\(//;
	$addr =~ s/\)//;

	my @memberlist = &get_member($type);

#//	print "\n// -----------------------\n";
#//	print "// port=$port , type=$type\n";

#//	foreach $member (@memberlist) {
		$var = $port ;	# . "." . "$member" ;
#		$adr = $adrhash{$var};
		$attrib = "A__IO|A_uint32_t";#$attrhash{$member};
		# {ポート名,アドレス,属性}
		printf(" {%-24s,0x%8s,%16s},\n", '"' . $var . '"' ,$addr,$attrib);
#//	}
}
# =====================================================
#      port の定義を取得.
# =====================================================
# define TIM2                ((TIM_TypeDef *) TIM2_BASE)
#
sub get_portdef()
{
	my $line;
	my $s=0;
	my $str='';
	my $typ;
	my @row;
	foreach $line (@hdr) {
		@row=split(/[ \t]+/,$line);
		if($row[0] eq '#define') {		#  '#define' で始まっている.
			if($row[2] =~ /^\(\(/) {
				if($row[3] eq '*)') {
#					print $line . "\n";
					&portdef($row[1],$row[2],$row[4]);
				}
			}
		}
	}
}
# =====================================================
#      port の定義を取得.
# =====================================================
# define TIM2                ((TIM_TypeDef *) TIM2_BASE)
#
sub print_portdef()
{
	my $line;
	my $s=0;
	my $str='';
	my $typ;
	my @row;
	foreach $line (@buf) {
		$line =~ s/\n//;
		$line =~ s/\r//;
		@row=split(/[ \t]+/,$line);
		&print_portdef_1($row[5],"",$row[0]);
	}
}
sub get_adrlist()
{
	my $line;
	my ($var,$adr);
	foreach $line (@adrlist) {
		$line =~ s/\n//;
		$line =~ s/[ \t]//g;

		($var,$adr) = split(/=/,$line);
		$adr = '0x' . $adr;
		$adrhash{$var}=$adr;
	}
}

# =====================================================
#      typedef の中身をprintする.
# =====================================================
sub pr_struct()
{
	my $str;
	my $typ;
	foreach $typ (keys %hash) {
		$str = $hash{$typ};
		print "type = >" . $typ . "<\n";
		print "def = >" . $str . "<\n";
	}
}

my $H_head= << '_EOM1';
//
PortList portlist_stm32[]={
_EOM1

my $H_foot= << '_EOM2';
 {NULL                    ,0       ,  0       }
};

_EOM2

my $LPC_head = << '_EOM4';

#include <stdio.h>
#include "LPC13xx.h"

//#define	__INLINE	/**/

typedef	unsigned	int		uint32_t;
typedef	unsigned	short	uint16_t;
typedef	unsigned	char	uint8_t;

//typedef	int					IRQn_Type;

int main()
{
_EOM4

my $CPP_head = << '_EOM';

#include <stdio.h>
#include "core_cm3.h"
#include "stm32f10x.h"

//#define	__INLINE	/**/

typedef	unsigned	int		uint32_t;
typedef	unsigned	short	uint16_t;
typedef	unsigned	char	uint8_t;

//typedef	int					IRQn_Type;

int main()
{
_EOM

# =====================================================
#      
# =====================================================
sub main1()
{
	($arg1,$arg2,$arg3) = @ARGV;

	if($arg1 eq '-stm32') {
		@buf = &loadfile($core_hdr);	# core_cm3.hを読む.
		&push_hdr();
		@buf = &loadfile($stm32_hdr);	# stm32f10x.hを読む.
		&push_hdr();
	}else{
		@buf = &loadfile($lpc13xx_hdr);	# stm32f10x.hを読む.
		&push_hdr();
	}

	if($arg2 eq '-a') {
		@adrlist = &loadfile('a.txt');	# アドレス決定.
		&get_adrlist();
	}

	&get_struct(); # 構造体を解析.

	if($arg2 eq '-a') {
		# C言語用ヘッダーを吐く.
		print $H_head;
		&print_portdef();
		print $H_foot;
	}else{
		# C++ のソースを吐く.
		if($arg1 eq '-stm32') {
			print $CPP_head;
		}else{
			print $LPC_head;
		}
		&get_portdef();
		print "\n	return 0;\n}\n"
	}
}

sub main()
{
	@buf = &loadfile("port.txt");
	&print_portdef();
}
# =====================================================
#      
# =====================================================

&main();

