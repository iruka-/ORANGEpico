/*********************************************************************
 *	HID_Monit
 *********************************************************************
 *
 * コマンドヘルプは 'h' もしくは '?' です.

アドレス範囲指定にて、
 ADDRESS < ADDRESS2 を満たさないときは ADDRESS2 をバイト数とみなします.
コマンドやアドレスの区切りは空白もしくは ',' のどちらでも可.

 ADDRESS は16進数、もしくはポート名も可.

 *********************************************************************
 */

#define VERSION	"0.5"


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifndef	_LINUX_
#include <stdlib.h>
#include <conio.h>		// getch() kbhit()
#endif

#include "monit.h"
#include "util.h"
#include "gr.h"
#include "analog.h"

#include "usb-uart.h"


#define	VCC_IS_5VOLT		1	// 0:Vcc=3.3V   1:Vcc = 5V
#define	ANALOG_CALC_DEBUG	0	// A/D 変換の計算をデバッグ.

#if	VCC_IS_5VOLT

#define	VOLT_VCC	5.00	// 4.76

#else
//#define	VOLT_VCC	5.00
#define	VOLT_VCC	(3.30-0.02)

#endif


//#define	VOLT_REF	1.23
#define	VOLT_REF	0.970	//1.02	チップごとにばらつきがあります.
							//AVR>ain 100000 を実行して,Tin=Trefになった状態で
							//DMMにてAIN0の電位を測定して決めてください.
							//公称値は1.23Vのはずですが、２回計測の後の値が
							//安定しているようです.


#define	REFERENCE_K_OHM	2.200	// 参照抵抗Ｒ の抵抗値KΩ.
#define	INTERNAL_K_OHM	0.000	//       AVRの内部抵抗KΩ.

void cmdPortPrintOne(char *name,int adrs,int val);

#define	KEY_ESC		0x1b
#define	KEY_CTRL_C	0x03

#include "portlist.h"

extern PortList *portList;
int	arm_init(int isthumb);
int UsbGetDevId(void);

#define DEFAULT_SERIAL	"*"
#define	DEFAULT_PORT	"gpioa.idr"

char HELP_USAGE[]={
	"* uartmon32 Ver " VERSION " (" __DATE__ ")\n"
	"Usage is\n"
	"  uartmon32 [Options]\n"
	"Options\n"
	"  -p[:XXXX]   ...  select Serial number (default=" DEFAULT_SERIAL ").\n"
	"  -l          ...  command echo to Log file.\n"
	"  -i<file>    ...  input script file to execute.\n"
	"  -v          ...  print verbose.\n"
};

char HELP_MESSAGES[]={
	"* uartmon32 Ver " VERSION "\n"
	"Command List\n"
	" d  <ADDRESS1> <ADDRESS2>    Dump Memory(RAM)\n"
#if 0
	" dr <ADDRESS1> <ADDRESS2>    Dump Memory(EEPROM)\n"
	" dp <ADDRESS1> <ADDRESS2>    Dump Memory(PGMEM)\n"
	" ain   <CNT>                 Analog input\n"
	" aingraph                    Analog input (Graphical)\n"
	" aindebug <mode>             Analog input debug\n"
	" reg   <CNT>                 Registance Meter\n"
	" reggraph                    Registance Meter (Graphical)\n"
	" p <PortName>.<bit> <DATA>   Write Data to PortName.bit\n"
	" poll  *  <CNT>              continuous polling port A,B,D\n"
	" poll <portName>             Continuous polling port\n"
#endif
	" e  <ADDRESS1> <DATA>        Edit Memory\n"
	" f  <ADDRESS1> <ADDRESS2> <DATA> Fill Memory\n"
	" l  <ADDRESS1> <ADDRESS2>    List (Disassemble) PGMEM\n"
	" p ?                         Print PortName-List\n"
	" p *                         Print All Port (dump format)\n"
	" p <PortName>                Print PortAddress and data\n"
	" p <PortName> <DATA>         Write Data to PortName\n"
	" sleep <n>                   sleep <n> mSec\n"
	" label <LABEL>               set loop label.\n"
	" :<LABEL>                    set loop label.\n"
	" loop  <LABEL> <CNT>         loop execution <CNT> times.\n"
	" bench <CNT>                 HID Write Speed Test\n"
	" boot [<address>]            Start user program\n"
	" run  <address>              Run user program at <address>\n"
	" user <arg>                  Run user defined function (usercmd.c)\n"
	" poll <portName> [<CNT>]     continuous polling port\n"
	" graph <portName>            Graphic display\n"
	" q                           Quit to DOS\n"
};

//	" p .                         Print All Port (column format)\n"
//
//	コマンド解析用ワーク.
//
#define	MAX_CMDBUF	4096
static	char cmdbuf[MAX_CMDBUF];
static  char *arg_ptr[128];
static  int  arg_val[128];
static  int  arg_hex[128];
static  int  arg_cnt;
static	char isExit = 0;		//1:コマンド入力ループを抜ける.

static	int	 adrs = 0;
static	int	 adrs2= 0;
static	int	 memcnt=0;
static	int	 loopcnt=0;
static	int  arena= 0;
static	int  poll_mode=POLL_PORT;	// 0xc0
static	int  trig_mode=0;			// ロジアナのトリガー条件になる入力bitマスク
									// フリーランの場合は0 全bit監視なら0xff


//	赤外線専用:
static	uchar infra_sample[1280];

//
//	オプション処理ワーク.
//
static	char verbose_mode = 0;	//1:デバイス情報をダンプ.
static	FILE *script_fp = NULL;	//外部ファイルからコマンドを読み込む.
static  int log_mode = 0;		//端末入力をエコーする.
static  int echo_mode = 1;


//
//	未実装.
//
int UsbGetDevId(void)					{return 0;}
void analize_infra(uchar *buf,int cnt)  {}
int bfd_micromips_num_opcodes(void)		{return 0;}
int init_symbol(void)					{return 0;}
int is_crlf(void)						{return 0;}
int micromips_opcodes(void)				{return 0;}
int reg_symbol(void)					{return 0;}
int set_force_thumb(void)				{return 0;}

/*********************************************************************
 *	アナログ入力グラフ表示.(仮)
 *********************************************************************
 */
void cmdAinGraph(char *buf) {}
void cmdRegGraph(char *buf) {}

/**********************************************************************
 *		文字列 p を 文字 c で分割し、args[] 配列に入れる。
 *		文字列は分断されることに注意。
 *		空白もスプリッターになるようにしているので注意
 **********************************************************************
 */
static	int	splitchr=' ';
static	int is_spc(int c)
{
	if(c==splitchr) return 1;
	if(c==' ') 		return 1;
	if(c=='\t') 	return 1;
	return(0);
}

/**********************************************************************
 *		文字列 *p をデリミタ文字 c あるいは空白で分割し
 *             *args[] ポインタに入れる
 *		分割数 n を返す.
 **********************************************************************
 */
static	int split_str(char *p,int c,char **args)
{
	int argc=0;
	int qq=0;
	splitchr=c;

	while(1) {
		while( is_spc(*p) ) p++;
		if(*p==0) break;

		if(*p != 0x22) {
			args[argc++]=p;		/* 項目の先頭文字 */

			while( *p ) {		/* 区切り文字まで読み進める */
				if(is_spc(*p))break;
				p++;
			}

			if(is_spc(*p)) {	/* 区切り文字であれば */
				*p=0;p++;		/* NULLで切る */
			}
		}else{
			qq=*p++;
			args[argc++]=p;		/* 項目の先頭文字 */

			while( *p ) {		/* 区切り文字まで読み進める */
				if(*p==qq)break;
				p++;
			}
			if(*p==qq) {	/* 区切り文字であれば */
				*p=0;p++;		/* NULLで切る */
			}
		}
	}
	return argc;
}


/*********************************************************************
 *	文字 c が空白文字なら1を返す.
 *********************************************************************
 */
int is_space(int c)
{
	if((c==' ') || (c=='\t')) return 1;
	return 0;
}
/*********************************************************************
 *	文字列 *buf の先頭余白を読み飛ばす.
 *********************************************************************
 */
char *sp_skip(char *buf)
{
	while(*buf) {
		if(is_space(*buf)==0) return buf;
		buf++;
	}
	return buf;
}
/*********************************************************************
 *	文字列 *buf を2進値 '0101_1111' と見なせるかどうかチェックする.
 *********************************************************************
 */
int radix2scanf(char *s,int *p)
{
	int rc = 0;
	int c;

	// 2進値を書く場合の条件 = 10文字以下、かつ、 _ を含んでいること.
	// 0b ではじめる方法もあるが、16進値と区別がつかないので,出来れば_を書く.

	if( (strlen(s)<=10) && (strchr(s,'_')) ) {
		while(1) {
			c = *s++;
			if( c==0 ) break;		//文字列終端.
			if((c=='0')||(c=='1')) {
				rc = rc << 1;		// 2倍.
				rc |= (c-'0');		// '1' ならLSBをOn.
			}else
			if(c != '_') {
				return (-1);	//許容できない文字が来たので失敗.
			}
		}
		*p = rc;	//2進値を返す.
		return 0;	// OK.
	}
	return (-1);
}

/*********************************************************************
 *	arg_ptr[] の文字列をHEX数値とみなして値に変換し arg_hex[]に入れる
 *********************************************************************
 */
void scan_args(int arg_cnt)
{
	int i,v;
	for(i=0;i<arg_cnt;i++) {
		v = portAddress(arg_ptr[i]);	//シンボル名による解決.
		if(v) {
			arg_hex[i] = v;
			arg_val[i] = v;
		}else{
			v=(-1);
			sscanf(arg_ptr[i],"%x",&v);
			arg_hex[i] = v;

			radix2scanf(arg_ptr[i],&arg_hex[i]);

			v=(-1);
			sscanf(arg_ptr[i],"%d",&v);
			arg_val[i] = v;
		}
	}
}
/*********************************************************************
 *	文字列 *buf から、パラメータを読み取り、( adrs , memcnt ) を決める
 *********************************************************************
 */
int	get_arg(char *buf)
{
	arena=MEM_BYTE;
	if(*buf == 'b') {
		buf++; 
	}else
	if(*buf == 'h') {
		buf++; arena = MEM_HALF;
	}else
	if(*buf == 'w') {
		buf++; arena = MEM_WORD;
	}
	

	memcnt = 256;
	adrs2 = (-1);

	buf = sp_skip(buf);
	if(*buf==0) return 0;

	arg_cnt = split_str(buf,',',arg_ptr);
	scan_args(arg_cnt);

	if(arg_cnt>=1) {
		adrs = arg_hex[0];
	}
	if(arg_cnt>=2) {
		adrs2 = arg_hex[1];
		if(adrs2 != (-1) ) {
			memcnt = adrs2 - adrs + 1;
		}
		if(	memcnt < 0) {
			memcnt = adrs2;
		}
	}
	return arg_cnt;
}
/*********************************************************************
 *	文字列 *buf がコマンド名 *cmd を含んでいるかどうか調べる.
 *********************************************************************
 */
int	str_comp(char *buf,char *cmd)
{
	while(*cmd) {
		if(*buf != *cmd) {return 1;}	// 不一致.
		cmd++;
		buf++;
	}
	return 0;
}
/*********************************************************************
 *
 *********************************************************************
 */
void usage(void)
{
	fputs( HELP_USAGE, stdout );
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdHelp(char *buf)
{
	fputs( HELP_MESSAGES, stdout );
}


/*********************************************************************
 *	1bitだけOnのマスクパタンから、bit番号(0..7)に変換.
 *********************************************************************
 */
static	int get_bpos(int mask)
{
	int i;
	for(i=0;i<8;i++) {
		if(mask & 1) return i;
		mask >>= 1;
	}
	return (-1);
}

/*********************************************************************
 *	電圧測定方法
 *********************************************************************
Vref(1.23V)のときのTrefと、
Vin(xxx V)のときのTin をUSB経由で取得して、
1.23V=5.0V * (1-exp(-Tref/RC)) にて、RCの積の値を得て
Vin  =5.0V * (1-exp(-Tin/RC)) を計算すればＯＫ。
ただし5Vの精度も影響してくると思われる。

RCの逆算式は exp(-Tref/RC))=1-(1.23/5.0)=0.754
RC = Tref/0.28236
 *********************************************************************
 */

double AnalogCalcVoltage(int Tref,int Tin)
{
	double Vin,RC;
	if((Tref == 0xffff)) {
		return 0.0;			//参照側の結果がないので計測出来ず.
	}
	if((Tin == 0xffff)) {
		return VOLT_VCC;	//TIMER1捕獲出来ず. VCCをわずかに超えている可能性あり.
	}
	RC  = (double) Tref / log( 1.0 - (VOLT_REF/VOLT_VCC) ) * -1.0;
	Vin = VOLT_VCC * (1.0 - exp( -1.0 * ((double) Tin / RC )));

#if	ANALOG_CALC_DEBUG
	{	double R,C,RC1;
		RC1 = RC / 12000000.0;	// クロック数.
		R = 2.2 * 1000.0;	// R=2.2kΩとする.
		C = RC1 / R;
		printf("\n   RC=%12g , C=%12g uF\n",RC1,C * 1000000.0);
	}
#endif

	return Vin;
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdQuestion(char *buf)
{
	int v;
	if( get_arg(buf) < 1) {
		cmdHelp("");
	}else{
		v = portAddress(arg_ptr[0]);
		if(v) printf("%s=0x%x\n",arg_ptr[0],v);
	}
}

void cmdPortPrintOne(char *name,int adrs,int val)
{
	char tmp[128];
//	if(val & 0xffff0000) 
	if(1)
	{
		printf("%16s(0x%08x) 0x%08x %s\n",name,adrs,val,radix2str(tmp,val));
	}else{
		printf("%16s(0x%08x)     0x%04x %s\n",name,adrs,val,radix2str(tmp,val));
	}
}

void cmdPortPrintOneCr(int count, char *name,int adrs,int val)
{
	char tmp[128];
	printf("%5d: %8s(%02x) %02x %s\r",count,name,adrs,val,radix2str(tmp,val));
}

void cmdPortPrintAllCr(int count, unsigned char *pinbuf)
{
	char tmp1[128];
	char tmp2[128];
	char tmp3[128];
	int pina = pinbuf[0];
	int pinb = pinbuf[1];
	int pind = pinbuf[2];

	printf("%5d: PORTA %02x %s | PORTB %02x %s | PORTC %02x %s \r"
		, count
		,pina,radix2str(tmp1,pina)
		,pinb,radix2str(tmp2,pinb)
		,pind,radix2str(tmp3,pind)
	);
}

void cmdPortPrintOne_b(char *name,int adrs,int val,int mask)
{
	int bpos = 0;
	char bitlist[16] = "____ ____";
	if( mask==0) {
		cmdPortPrintOne(name,adrs,val);
	}else{
		bpos = get_bpos(mask);
		if(bpos<0) {
			cmdPortPrintOne(name,adrs,val);
		}else{
			bpos = 7 - bpos;
			if(bpos >= 4) bpos ++;

			if( val & mask ) bitlist[bpos]='1';
			else			 bitlist[bpos]='0';
			printf("%8s(%02x) %02x %s\n",name,adrs, val , bitlist);
		}
	}
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdDigitalWrite(char *buf)
{
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdDigitalRead(char *buf)
{
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdPinMode(char *buf)
{
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdPort(char *buf)
{
	int v,n,mask=0;
	int arena=MEM_WORD;	//読み出し幅.
	char *portname="(?)";
	n = get_arg(buf);	//引数の数.
	if(n != 0) {
		v = portAddress_b(arg_ptr[0],&mask);	//ポート名称からアドレスを算出.
		if(v) {		// アドレスが存在する.
			portname=realPortName(arg_ptr[0]);
			arena   =type2size(getPortAttrib(arg_ptr[0]));	//読み出し幅.
			if(n < 2) {					// 1個のポートを読み取る.
				int val = UsbPeek(v,arena);
				cmdPortPrintOne_b(portname,v,val,mask);
			}else{						// p <adrs> <data>
				int val;
				//UsbPoke_b(v,0,arg_hex[1],mask);				//書き込み.
				mask=0;
				UsbPoke(adrs,arena,adrs2,mask);		// adrs2 は書き込みデータ.
				//printf("UsbPoke(%x,%x,%x,%x)\n",adrs,arena,adrs2,mask);
				val = UsbPeek(v,arena);
				cmdPortPrintOne_b(portname,v,val,mask);	//再読み込み.
			}
			return;
		}else if (strcmp(arg_ptr[0],"*")==0) {//ダンプ形式でポート表示する.
			PrintPortAll(0);
			return;
//		}else if (strcmp(arg_ptr[0],".")==0) {//段組形式でポート表示する.
//			PrintPortColumn();
//			return;
		}else if (strcmp(arg_ptr[0],"?")==0) {//ポート名称の一覧を出す.
			PrintPortNameList();
			return;
		}else{
			suggestPortName(arg_ptr[0]);
			return;
		}
	}
	PrintPortAll(1);	//よく参照するポートだけ表示する.
}
/*********************************************************************
 *	ポート内容をポーリングする.
 *********************************************************************
 */
void cmdPoll(char *buf)
{
	uchar sample[256];
	int v,n,count,i;
	n = get_arg(buf);	//引数の数.
	if(n != 0) {
		if(n < 2) {					// 1個のポートを読み取る.
			count = 1000;
		}else{
			count = arg_hex[1];
		}

		v = portAddress(arg_ptr[0]);	//ポート名称からアドレスを算出.
		if(v) {		// アドレスが存在する.
			UsbSetPoll(v,POLL_PORT);
			for(i=0;i<count;i++) {
				int val = UsbPoll(sample);
				cmdPortPrintOneCr(count - i, arg_ptr[0],v,val);
				if(kbhit()) break;
			}
		}
		UsbSetPoll(0,0);
	}
	printf("\n\n");
}

/*********************************************************************
 *
 *********************************************************************
 */
int	cmdExecTerminal(void)
{
	uchar buf[1024];
	int mode = 'p';
	int rc,len,key;

	while(1) {
		rc = UsbReadString(buf+1);
		if(rc == 0) {
			fprintf(stderr, "UsbReadString error\n");
			exit(EXIT_FAILURE);
		}
		if(	buf[1] == mode) {
			len = buf[2] & 0xff;
			if(len) {
				buf[3+len] = 0;
				printf("%s",buf+3);
			}
		}
		if(	buf[1] == 'q') {
			len = buf[2] & 0xff;
			if(len) {
				buf[3+len] = 0;
				printf("%s",buf+3);
			}
			printf("\n* Program Terminated.\n");
			break;
		}
		if(kbhit()) {
			key = getch();
			if((key==KEY_ESC)||(key==KEY_CTRL_C)) {
				printf("\n* User Break. \n");
				break;
			}else{
				UsbPutKeyInput(key);
			}
		}
	}
	return 0;
}
/*********************************************************************
 *	device-->hostPC への上りストリームテスト.
 *********************************************************************
 *	第一引数： １回の転送で送り出すバイト数.
 *	第二引数： 繰り返し回数.
 
 */
void cmdTestul(char *buf)
{
	int n = get_arg(buf);	//引数の数.
	int val = 1040;
	int cnt = 1;
	if(n >= 1) {
		val = arg_val[0];
	}
	if(n >= 2) {
		cnt = arg_val[1];
	}
	printf("cmdTestul(%d) x %d\n",val,cnt);

	int i;
	for(i=0;i<cnt;i++) {
		UsbTestUpload(val);
	}
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdBoot(char *buf)
{
	int n = get_arg(buf);	//引数の数.
	int adr = 0xbfc00000;
	if(n != 0) {
		adr = arg_hex[0];
	}
	int resetpc = adr;
	printf("reset pc=0x%x\n",resetpc);
	UsbBootTarget(resetpc,1,0);

	cmdQuit("");
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdRun(char *buf)
{
	int n = get_arg(buf);	//引数の数.
	int	pc=arg_hex[0];
	int	dev_id = UsbGetDevId();

	if(n != 0) {
		printf("* Start Program at 0x%x\n",pc);
		UsbBootTarget(pc,0,0);
	}else{
		printf("* usage :  run address\n");
	}
	(void)dev_id;
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdUser(char *buf)
{
	int adr=0;
	int n = get_arg(buf);	//引数の数.
	if( n != 0 ) {
		adr = arg_val[0];
	}
	if(UsbGetDevCaps(NULL,NULL) != 0) {	// BOOTLOADER
		printf("Can't Start User Program(BOOTLOADER)\n");
		return;
	}
	printf("* Start Program \n");
	UsbExecUser(adr);
	Sleep(1);
	cmdExecTerminal();
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdSync(char *buf)
{
	int adr=1;
	int n = get_arg(buf);	//引数の数.
	if( n != 0 ) {
		adr = arg_val[0];
	}
	int rc = UsbReadString((uchar*)buf);
	(void)rc;
	(void)adr;
//	UsbSyncCmd(adr);
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdErase(char *buf)
{
	int n = get_arg(buf);	//引数の数.
	if(n != 0) {
		UsbFlashLock(0);
		UsbEraseTargetROM(adrs,memcnt);
	}
}

/*********************************************************************
 *
 *********************************************************************
 */
void cmdFlash(char *buf)
{
	unsigned char buf2[10240];
	int size=128;
	int n = get_arg(buf);	//引数の数.
	if( n != 0 ) {
		size = arg_val[0];
	}

	if(size) {
		dumpmem(0x1000,0,size,buf2);
	}
}

/*********************************************************************
 *	比較器とＲＣ時定数を使用した、アナログ入力(for deBUG)
 *********************************************************************
 */
void cmdAinDebug(char *buf)
{
	int n,mode=0xa0;
	unsigned char abuf[256];
	n = get_arg(buf);	//引数の数.
	if(n != 0) {
		mode = 0xa0 + (arg_hex[0] & 0x0f);
	}

	UsbAnalogPoll(mode,abuf);
	printf("%02x %02x %02x %02x \n",abuf[0],abuf[1],abuf[2],abuf[3]);
}

/*********************************************************************
 *	比較器とＲＣ時定数を使用した、アナログ入力
 *********************************************************************
 */
int	getAnalogVolt(double *val,int f)
{
	int mode,Tref,Tin;
	unsigned char abuf[256];
	double volt;

#if	ANALOG_TREF_CONST		// A/D 変換のVref計測を省略.
	Tref = ANALOG_TREF_CONST;
#else
//for(i=0;i<2;i++)
  {
	mode = AIN_MODE_VREF ;
	UsbAnalogPoll(mode,abuf);
	if( abuf[1] != mode ) {
		printf("analog input error. unimplemented protocol.\n");
		printf("%02x %02x %02x %02x \n",abuf[0],abuf[1],abuf[2],abuf[3]);
		return -1;
	}
	Tref = abuf[2] + (abuf[3]<<8);
  }
#endif

  {
	mode = AIN_MODE_AIN0 ;
	UsbAnalogPoll(mode,abuf);
	if( abuf[1] != mode ) {
		printf("analog input error. unimplemented protocol.\n");
		printf("%02x %02x %02x %02x \n",abuf[0],abuf[1],abuf[2],abuf[3]);
		return -1;
	}
	Tin  = abuf[2] + (abuf[3]<<8);
  }

	volt = AnalogCalcVoltage(Tref,Tin);

	if(f) {
		// 結果表示printf あり.
		printf("ain0 = %12.3f (Tin=%5d/Tref=%5d) \r",volt,Tin,Tref);
//		printf("ain0 = %12.3g\r",volt);
	}
	if(val) {*val = volt;}
	return 0;
}

/*********************************************************************
 *	比較器とＲＣ時定数を使用した、抵抗測定
 *********************************************************************
 */
int	getRegistance(double *val,int f)
{
	int mode,Tref,Tin;
	unsigned char abuf[256];
	double regval;

//  for(i=0;i<2;i++) {
	mode = AIN_MODE_VREF ;
	UsbAnalogPoll(mode,abuf);
	if( abuf[1] != mode ) {
		printf("analog input error. unimplemented protocol.\n");
		printf("%02x %02x %02x %02x \n",abuf[0],abuf[1],abuf[2],abuf[3]);
		return -1;
	}
	Tref = abuf[2] + (abuf[3]<<8);
 // }

	mode = AIN_MODE_REGISTANCE ;
	UsbAnalogPoll(mode,abuf);
	if( abuf[1] != mode ) {
		printf("analog input error. unimplemented protocol.\n");
		printf("%02x %02x %02x %02x \n",abuf[0],abuf[1],abuf[2],abuf[3]);
		return -1;
	}
	Tin  = abuf[2] + (abuf[3]<<8);

	if( (Tref == 0xffff) ) {
		//抵抗 計算出来ない.
		return -1;
	}
	if( (Tin == 0xffff) ) {
		regval = -1;	//計測できず.
	}else{
		regval = (double) REFERENCE_K_OHM * Tin / Tref;
		regval -= INTERNAL_K_OHM;
		if(regval<0) regval = 0;
	}

	if(f) {
		// 結果表示printf あり.
		if( regval >= 0) {
			printf(" R = %10.3f K (Treg=%5d/Tref=%5d) \r",regval,Tin,Tref);
		}else{
			printf(" R = Open         (Treg=%5d/Tref=%5d) \r",Tin,Tref);
		}
	}
	if(val) {*val = regval;}
	return 0;
}


/*********************************************************************
 *	比較器とＲＣ時定数を使用した、アナログ入力
 *********************************************************************
 */
void cmdAin(char *buf)
{
	int n,cnt=1,i,mode;
	unsigned char abuf[256];
	double volt;
	n = get_arg(buf);	//引数の数.
	if(n != 0) { cnt = arg_val[0]; }	// 変換回数.

	for(i=0;i<cnt;i++) {
		getAnalogVolt(&volt,1);
		if(kbhit()) break;
	}
	mode = AIN_MODE_NONE;
	UsbAnalogPoll(mode,abuf);
	printf("\n");
}

/*********************************************************************
 *	比較器とＲＣ時定数を使用した、抵抗測定
 *********************************************************************
 */
void cmdReg(char *buf)
{
	int n,cnt=1,i,mode;
	unsigned char abuf[256];
	double registance;
	n = get_arg(buf);	//引数の数.
	if(n != 0) { cnt = arg_val[0]; }	// 変換回数.

	mode = AIN_MODE_AIN0 ;
	UsbAnalogPoll(mode,abuf);
	if( abuf[1] != mode ) {
		printf("analog input error. unimplemented protocol.\n");
		printf("%02x %02x %02x %02x \n",abuf[0],abuf[1],abuf[2],abuf[3]);
		return;
	}

	for(i=0;i<cnt;i++) {
		getRegistance(&registance,1);
		if(kbhit()) break;
	}
	mode = AIN_MODE_NONE;
	UsbAnalogPoll(mode,abuf);
	printf("\n");
}




/*********************************************************************
 *	ポート内容をポーリングする.
 *********************************************************************
 */
#define	SCREEN_W	768
#define	SCREEN_X0	32
#define	SCREEN_H	480

#define	SCREEN_H2	16		//カーソルラインの長さ.
//#define	H_HEIGHT	20		// LとHの差の高さ.
#define	H_HEIGHT	10		// LとHの差の高さ.

#define	SIG_COLOR	0x00f000
#define	SIG_COLOR1	0x008000
#define	SIG_COLOR2	0x004000
#define	BACK_COLOR	0x000000
#define	FRAME_COLOR	0x504000
#define	FRAME_COLOR2	0x304000

int	calc_ypos(int i)
{
	int off;
	int y1;

#if	0
	if(i>=4) off = 24;
	else off = 0;
	y1 = 60 + i*48 + off;
#else
	if(i>=8) off = 16;
	else off = 0;
	y1 = 60 + i*24 + off;
#endif
	return y1;
}

void vline2(int x0,int y0,int x1,int y1,int col)
{
	int yc;
/*
	int ytmp;
	if(y0>y1) {
		ytmp=y0;y0=y1;y1=ytmp;
	}
 */
	yc = (y0+y1)/2;
	gr_vline(x0,y0	,x0,yc,col);		// (差分)縦の線1.
	gr_vline(x1,yc+1,x1,y1,col);		// (差分)縦の線2.
}
#define	ANALOG_CENTER	(480/2)
#define	ANALOG_BASE		(ANALOG_CENTER-128)
/*********************************************************************
 *	1サンプルの1ビット分を描画
 *********************************************************************
 */
void plot_analog(int x,int y,int val,int old_val)
{
	int diff = old_val - val;
	int yc = 128+ANALOG_BASE;

	int y0 = 256+ANALOG_BASE - old_val;
	int y1 = 256+ANALOG_BASE - val;

	if(x==SCREEN_X0) {	//左端.
		gr_vline(x-1,ANALOG_BASE,x-1,256+ANALOG_BASE,0);
	}

	gr_pset(x,yc 	,FRAME_COLOR);			// ベースライン赤.
	gr_pset(x,yc-129,FRAME_COLOR2);			// Hレベルライン赤.
	gr_pset(x,yc+129,FRAME_COLOR2);			// Lレベルライン赤.

	if(diff) vline2(x-1,y0,x,y1,SIG_COLOR1);	// (差分)縦の線.
	else	       gr_pset(x,y1,SIG_COLOR1);	// 信号の色.

}

/*********************************************************************
 *	1サンプルの1ビット分を描画
 *********************************************************************
 *	v    : 0 か 非0   信号レベル.
 *  diff : 0 か 非0   信号反転が起きた.
 */
void plot_signal(int x,int y,int v,int diff)
{
	int col0,col1,col2;

	if(v) {	col0 = FRAME_COLOR;	col1 = SIG_COLOR ; }
	else  {	col0 = SIG_COLOR;	col1 = BACK_COLOR ; }

	if(diff) {	col2 = SIG_COLOR2;}
	else	 {	col2 = BACK_COLOR;}

	if(diff) gr_vline(x,y-1,x,y- H_HEIGHT,col2);	//縦の線.
	if(v) {
		gr_pset(x,y - H_HEIGHT,col1);				// Hレベルの色.
	}else{
		gr_pset(x,y - 2 ,col0);						// Lの色.
		gr_pset(x,y - 1 ,col0);						// Lの色.
	}
	gr_pset(x,y  ,FRAME_COLOR);						// ベースライン赤.
	gr_pset(x,y- H_HEIGHT-1,FRAME_COLOR2);			// Hレベルライン赤.
}

/*********************************************************************
 *	1サンプルを描画.
 *********************************************************************
 */
void draw_sample(int x,int y,int val)
{
	static int old_val;

	if( poll_mode == POLL_ANALOG) {		// 0xa0
		plot_analog(x,y,val,old_val);
	}else{
		int i,y1;
		int m=1;
		int diff = val ^ old_val;	//前回と値が変わった?
		for(i=0;i<16;i++,m<<=1) {
			y1 = calc_ypos(i);
			plot_signal(x,y+y1,val & m,diff & m);
		}
	}

	old_val = val;
}

/*********************************************************************
 *	グラフ表示内の文字を書く.
 *********************************************************************
 */
void draw_TimeScale()
{
//	int i,y1=20,ms=0;
	int i,ms=0;
	char string[128];
//	int color = 0x80ff00;
	int x1=SCREEN_X0;
	for(i=0;i<8;i++) {
		sprintf(string,"%dmS",ms);
//		gr_puts(x1,y1,string,color,0,0);
		ms+=10;
		x1+=100;
	}
}

/*********************************************************************
 *	グラフ表示内の文字を書く.
 *********************************************************************
 */
void draw_PortName(int adr,int poll_mode)
{
	int i,y1;
	char string[128];
	int color = 0x80ff00;

	if( poll_mode == POLL_ANALOG) {	// 0xa0
		int yc = 128+ANALOG_BASE-8;
		int y0 =     ANALOG_BASE-8;
		int y1 = 256+ANALOG_BASE-8;
		gr_puts(4,y0,"100",color,0,0);
		gr_puts(4,yc,"50" ,color,0,0);
		gr_puts(4,y1,"0"  ,color,0,0);
	}else
	for(i=0;i<16;i++) {
		sprintf(string,"P%d" ,i);
		y1 = calc_ypos(i);
		gr_puts(4,y1-20,string,color,0,0);
	}
}

void analog_conv(uchar *sample,int poll_mode)
{
	// グラフ処理しやすいようにアナログサンプルを10bitから8bitに落とす.
	int i,n,lo,hi,c;
	uchar *p,*t;
	if( poll_mode == POLL_ANALOG) {		// 0xa0
		n = sample[2]/2;
		sample[2]=n;
		p = &sample[3];
		t = &sample[3];
		for(i=0;i<n;i++) {
			lo = *p++;
			hi = *p++;
#if	0
			c = (hi<<8) | lo;
			c >>= 2;			//ADC結果の10bitが右詰の場合.
#else
			c = hi;				//ADC結果の10bitが左詰の場合.
#endif
			*t++ = c;
			(void)lo;
		}
	}
}
#define 	ANALOG_TRIG_THR	16
int	analog_chk_trig(int val,int oldval,int trig_mode)
{
	int diff;
	if(trig_mode) {
		if(	val>oldval) {
			diff = val-oldval;
		}else{
			diff = oldval-val;
		}
		if(diff >= ANALOG_TRIG_THR) return 1;
	}
	return 0;
}

int	GR_break(void)
{
	static	int cnt=0;
	cnt++;
	if(cnt>=32) {
		cnt=0;
		return gr_break();
	}
	return 0;
}
/*********************************************************************
 *	グラフ表示.(仮)
 *********************************************************************
 */
void cmdGraph(char *buf)
{
	uchar sample[256];
	int adr, i, n, oldval=0,infra=0;
	int freerun=1;			// フリーラン、もしくはトリガー発生で1,停止=０.
	int x=SCREEN_X0;
	poll_mode=POLL_PORT;	// 0xc0
	trig_mode = 0;	// FreeRun

	adr = portAddress( DEFAULT_PORT );	// default read PORT

	n = get_arg(buf);	//引数の数.
	if( n ) {
		adr = portAddress(arg_ptr[0]);	//ポート名称からアドレスを算出.
		if(	adr==0) {
			adr = arg_hex[0];
		}
		if( strcmp(arg_ptr[0],"analog")==0) {
			poll_mode = POLL_ANALOG;	// 0xa0
		}
		if(n>=2) {
			if( strcmp(arg_ptr[1],"trig")==0) {
				trig_mode = 0xff;
				freerun=0;
				if(n==3) trig_mode = 1<<(arg_hex[2]);
			}
			if( strcmp(arg_ptr[1],"infra")==0) {
				trig_mode = 0x01;	//
				freerun=0;
				infra=1;
			}
		}
	}


	gr_init(SCREEN_W,SCREEN_H,32,0);
	draw_PortName(adr,poll_mode);
	draw_TimeScale();
	UsbSetPoll(adr,poll_mode);
	do {
		int cnt;
		int vcol, val;

		val = UsbPoll(sample);
		analog_conv(sample,poll_mode);
		cnt = sample[2];

		for(i=0;i<cnt;i++) {
//			val = sample[3+i];
			if( poll_mode == POLL_ANALOG) {	// 0xa0
				if(analog_chk_trig(val,oldval,trig_mode)) freerun=1;
			}else{
			if((val ^ oldval) & trig_mode) freerun=1;
			}
			if(freerun) {
				if((x-SCREEN_X0) % 50) {
					vcol = 0;
				}else{
					vcol = FRAME_COLOR2;
				}
//				gr_vline(x,0,x,SCREEN_H-1,vcol);		//縦の線.
				gr_vline(x,40,x,SCREEN_H-1,vcol);		//縦の線.
				draw_sample(x,0,val);
				infra_sample[x-SCREEN_X0]=val;
				x++;
				if(x>=SCREEN_W) {
					x=SCREEN_X0;
					if(trig_mode) {
						freerun=0;
						if(infra) {
							analize_infra(infra_sample,SCREEN_W-SCREEN_X0);
						}
					}
				}
//				gr_vline(x,0,x,SCREEN_H-1,0xffff00);	//縦の線.
//				gr_vline(x,0,x,SCREEN_H2 ,0xffff00);	//縦の線.
				gr_vline(x,SCREEN_H-1-SCREEN_H2,x,SCREEN_H-1,0xffff00);	//縦の線.
			}
			oldval=val;
		}
	}while(GR_break()==0);
	UsbSetPoll(0,0);
	gr_close();
}
/*********************************************************************
 *	グラフ表示内の文字を書く.
 *********************************************************************
 */
void draw_AnalogFrame(void);
void draw_AnalogInput(void);
/*********************************************************************
 *
 *********************************************************************
 */
void cmdQuit(char *buf)
{
	printf("Bye.\n");
	isExit = 1;
}

void help_region(void)
{
	printf(
		"* PIC32MX220F032B Memory Map\n"
		"-------------:-----------------------\n"
		"BOOT-ROM(3k) : BFC0_0000 ~ BFC0_0BFF \n"
		"FLASH  (32k) : 9D00_0000 ~ 9D00_7FFF \n"
		"S-RAM   (8k) : A000_0000 ~ A000_1FFF \n"
		"PORT         : BF80_0000 ~ BF88_FFFF \n"
		"-------------:-----------------------\n"
		"Need Help ? >  please type help \n"
	);
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdDump(char *buf)
{
	get_arg(buf);

	if( check_region(adrs,1)<0 ) {
		help_region();
		adrs = 0xa0000000;
		return;
	}

	if(adrs==(-1)) {
		adrs = 0;
		return;
	}
	UsbDump(adrs,arena,(memcnt+7)&(-8));
	adrs += memcnt;
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdList(char *buf)
{
	int n,bytes;
	n = get_arg(buf);
	if(n<2) {
		memcnt = 40;	//ステップ数が省略されたら40にする.
	}

	if( check_region(adrs,1)<0 ) {
		help_region();
		adrs = 0x9d000000;
		return;
	}

	int thumb = adrs & 1;
	arm_init(thumb);	//奇数アドレスが与えられた場合、Thumbモード切替.
	if(n<2) {
		if(adrs & 1) {
			memcnt = 40;	//ステップ数が省略されたら40にする.
		}else{
			memcnt = 80;	//ステップ数が省略されたら80にする.
		}
	}
	adrs &= (-2);		//一応開始アドレスは偶数にする.

	bytes = UsbDisasm(adrs,arena,(memcnt+7)&(-8));
	adrs += bytes;

	adrs |= thumb;		//奇数アドレスが与えられた場合、Thumbモードを保持.
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdEdit(char *buf)
{
	int mask = 0;
	int argc = get_arg(buf);

	if( argc >= 2) {
		UsbPoke(adrs,arena,adrs2,mask);		// adrs2 は書き込みデータ.
	}else
	if( argc >= 1) {
		int val = UsbPeek(adrs,arena);
		printf("%06x %02x\n",adrs,val);
	}
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdFill(char *buf)
{
	int i;
	if( get_arg(buf) >= 3) {
		for(i=adrs;i<=adrs2;i++) {
			UsbPoke(i,arena,arg_hex[2],0);
			if(arena) {
				Sleep(25);	// EEPROMに対するFill は待ちを入れないとハングする.
			}
		}
	}
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdBench(char *buf)
{
	int cnt = 1000;
	int packet = 8;
	int argc = get_arg(buf);

	if(argc >= 1) {
		cnt = arg_val[0];
	}
	if(argc >= 2) {
		packet = arg_val[1];
	}
//	printf("%d,%d\n",cnt,packet);
	UsbBench(cnt,packet);
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdSleep(char *buf)
{
	int cnt = 1000;
	int argc = get_arg(buf);
	if(argc >= 1) {
		cnt = arg_val[0];
	}
	Sleep(cnt);
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdEcho(char *buf)
{
	int argc = get_arg(buf);
	if(argc >= 1) {
		if(str_comp(arg_ptr[0],"on")==0)  {echo_mode = 1;}
		if(str_comp(arg_ptr[0],"off")==0) {echo_mode = 0;}
	}
}
/*********************************************************************
 *
 *********************************************************************
 */
void cmdLabel(char *buf)
{
	/* none */
}
static void get_line(char *buf,int echo);

/*********************************************************************
 *	スクリプト実行中のラベルを(先頭から)探す.
 *********************************************************************
 *戻り値:
 *	0	見つかった.
 *	-1	見つからない.
 *	-2	script実行中でないのでLABEL検索できない.
 */
int	find_script_label(char *label)
{
	char *t;
	int argc;
	if( script_fp != NULL) {
		fseek(script_fp,0,SEEK_SET);		//先頭まで巻き戻す.

		while(1) {
			get_line(cmdbuf,0);	// label を探す.

			t = NULL;
			if( str_comp(cmdbuf,"label")==0 ) {t = cmdbuf+strlen("label");}
			if( str_comp(cmdbuf,":")==0 )     {t = cmdbuf+strlen(":");}
			if( t ) {
				argc = get_arg(t);
				if(argc >= 1) {
					if(strcmp(arg_ptr[0],label)==0) {
						return 0;		// OK.
					}
				}
			}
		}
		return -1;
	}
	return -2;
}

/*********************************************************************
 *	loop LABEL [count];   手抜き実装です.
 *********************************************************************
 */
void cmdLoop(char *buf)
{
	int argc = get_arg(buf);
	int cnt = 1000000;
	int rc  = 0;
	char label[256]="";
	
	if(argc >= 1) {
		strcpy(label,arg_ptr[0]);	//コピーしておくこと.
	}
	if(argc >= 2) {
		cnt = arg_val[1];
	}
	if(loopcnt==0) {
//		printf("*loopcnt=%d\n",cnt);
		loopcnt = cnt-1;	//	初回限定:ループカウンタを設定する.
	}else{
		loopcnt--;
//		printf("*--loopcnt=%d\n",loopcnt);
		if(loopcnt <= 0) return;	// loop 終了!!.	
	}

	// goto LABEL の実行.
	rc = find_script_label(label);
	if(rc== (-1)) {
		printf("label '%s' not found.",label);
	}
	if(rc== (-2)) {
		printf("error:Can't exec 'loop' command.\n");
	}
}
/*********************************************************************
 *
 *********************************************************************
 */


CmdList cmdlist[]={
	{	"h"	,	cmdHelp },
	{	"?"	,	cmdQuestion},
	{	"bench",cmdBench},
	{	"sleep",cmdSleep},
	{	"poll", cmdPoll },
	{	"graph",cmdGraph },
	{	"reggraph",cmdRegGraph },
	{	"aingraph",cmdAinGraph },
	{	"aindebug",cmdAinDebug },
	{	"ain",  cmdAin },
	{	"reg",  cmdReg },
	{	"exit",	cmdQuit },
	{	"boot",	cmdBoot },
	{	"run",	cmdRun  },
	{	"user",	cmdUser },
	{	"sync",	cmdSync },
	{	"erase",cmdErase},
	{	"flash",cmdFlash},
	{	":"    ,cmdLabel},
	{	"label",cmdLabel},
	{	"loop" ,cmdLoop},
	{	"echo" ,cmdEcho},
	{	"digitalwrite"  ,cmdDigitalWrite},
	{	"dw"   			,cmdDigitalWrite},
	{	"digitalread"   ,cmdDigitalRead},
	{	"dr"   			,cmdDigitalRead},
	{	"pinmode"   	,cmdPinMode},
	{	"pm"   			,cmdPinMode},

//	{	"s"	,	cmdSample },
	{	"q"	,	cmdQuit },
	{	"d"	,	cmdDump },
	{	"m"	,	cmdEdit },
	{	"e"	,	cmdEdit },
	{	"f"	,	cmdFill },
	{	"l"	,	cmdList },
	{	"p"	,	cmdPort },
	{	NULL,	cmdQuit },
};



/*********************************************************************
 *	モニタコマンド解析と実行.
 *********************************************************************
 */
void cmdMonit(char *buf)
{
	CmdList *t;
	buf = sp_skip(buf);
	if(*buf==0) return;
	for(t = cmdlist; t->name != NULL ; t++) {
		if( str_comp(buf,t->name)==0 ) {
			buf += strlen(t->name);
			t->func(buf);
			return;
		}
	}
	/*error*/
}
/*********************************************************************
 *	行末のCR/LF を削除する.
 *********************************************************************
 */
void chop_crlf(char *p)
{
	while(*p) {
		if((*p==0x0d)||(*p==0x0a)) { *p = 0; return; }
		p++;
	}
}

/*********************************************************************
 *	1行入力. ファイルもしくはコンソールの両方を許容.
 *********************************************************************
 */
static void get_line(char *buf,int echo)
{
	char *rc;
	if( script_fp != NULL) {
		rc = fgets(buf,MAX_CMDBUF,script_fp);		//ファイルから入力.
		if(rc == NULL) {		//ファイル末もしくはエラー.
			buf[0]=0;			//空行を返す.
			fclose(script_fp);
			if(echo) printf("\n");		//コマンドエコーする.
			script_fp = NULL;
			echo_mode = 1;
		}else{
			chop_crlf(buf);		//行末のCR/LF を削除する.
			if(echo) printf("%s\n",buf);	//コマンドエコーする.
		}
	}else{
		rc = fgets(buf,MAX_CMDBUF,stdin);		//コンソールから入力.
		chop_crlf(buf);		//行末のCR/LF を削除する.
		if (log_mode) {
			if(echo) printf("%s\n",buf);	//コマンドエコーする.
		}
	}
}


/*********************************************************************
 *	メイン
 *********************************************************************
 */
int main(int argc,char **argv)
{
	int rc;
	//オプション解析.
	Getopt(argc,argv,"i");
	if(IsOpt('h') || IsOpt('?') || IsOpt('/')) {
		usage();
		exit(EXIT_SUCCESS);
	}
	if(IsOpt('i')) {
		script_fp = fopen( Opt('i') , "rb" );
	}
	if(IsOpt('l')) {
		log_mode = 1;
	}
	if(IsOpt('v')) {	//デバイス情報をダンプする.
		verbose_mode = 1;
	}
	if(IsOpt('p')) {
		char *s;
		s = Opt('p');
		if (s) {
			if (*s == ':') s++;
			if (*s == '?') {
			} else if (isdigit(*s)) {
			} else if (*s == '\0'){
			} else {
			}
		}
	}

    setvbuf( stdout, NULL, _IONBF, 0 );

	arm_init(1);	// 1=Thumb-Mode.

	//初期化.
	if( (rc=UartInit(0)) < 0) {
//		fprintf(stderr, "UsbInit() failure. %d\n",rc);
		fprintf(stderr, "PIC32MX Device Not found.\n");
		exit(EXIT_FAILURE);
	}
	InitPortList(UsbGetDevId());
	
	do {					//モニターモード.
		if(echo_mode) {
			printf("MIPS> ");
		}
		get_line(cmdbuf,echo_mode);
		cmdMonit(cmdbuf);
	} while(isExit == 0);

	UartExit();

	if(script_fp!=NULL) fclose(script_fp);

	return EXIT_SUCCESS;
}
/*********************************************************************
 *
 *********************************************************************
 */

