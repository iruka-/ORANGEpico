/*********************************************************************
 *	P I C b o o t
 *********************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifndef	_LINUX_
#include <windows.h>
#endif

#include "monit.h"
#include "util.h"
#include "codebuf.h"
#include "usb-uart.h"


#define CMD_NAME "uartflash32.exe"
#define DEFAULT_SERIAL	"*"
#define	END_MASK	0xfffff


char HELP_USAGE[]= {
	"* UARTflash32 Ver 0.1 (" __DATE__ ")\n"
	"Usage is\n"
	"  " CMD_NAME " [Options] <hexfile.hex>\n"
	"Options\n"
	"  -p[:XXXX]   ...  Select serial number (default=" DEFAULT_SERIAL ").\n"
	"  -r          ...  Run after write.\n"
	"  -v          ...  Verify.\n"
	"  -E          ...  Erase.\n"
	"  -rp         ...  Read program area.\n"
//	"  -nv         ...  No-Verify.\n"
	"  -B          ...  Allow Bootloader Area to Write / Read !.\n"
	"  -sXXXX      ...  Set program start address. (default=2000)\n"
};

#define	MAX_CMDBUF	4096

static	char lnbuf[1024];
static	char usb_serial[128]=DEFAULT_SERIAL;	/* 使用するHIDaspxのシリアル番号を格納 */
//static	char verbose_mode = 0;	//1:デバイス情報をダンプ.
		int  hidmon_mode = 1;	/* picmon を実行中 */
static	uchar 	databuf[256];
static	int		dataidx;
static	int		adr_u = 0;		// hexrec mode 4

#define	HIGH_MASK	0xffff0000

#define	READ_SIZE	64
//	#define	WRITE_SIZE	(64-8)
#define	WRITE_SIZE	(56-8)
//	#define	WRITE_SIZE	1024


//==PIC32MX===================
#define	FLASH_BASE	0x1d000000
#define	FLASH_START	0x00000000
#define	FLASH_SIZE	0x00020000

#define	FLASH_STEP	512
#define	ERASE_STEP	1024	// MX1xx/2xx only 



static	uchar flash_buf[FLASH_SIZE];
static	int	  flash_start = FLASH_START;
static	int	  flash_end   = FLASH_SIZE;
static	int	  flash_end_for_read = FLASH_SIZE;


static	char  opt_r  = 0;		//	'-r '
static	char  opt_rp = 0;		//	'-rp'
static	char  opt_re = 0;		//	'-re'
static	char  opt_rf = 0;		//	'-rf'

static	char  opt_E  = 0;		//	erase
static	char  opt_v  = 0;		//	'-v'
static	char  opt_nv = 0;		//	'-nv'

//	ユーザーアプリケーション開始番地.
static	int   opt_s  = 0x2000;	//	'-s1000 など'

#define	CHECKSUM_CALC	(1)


#define	DEVCAPS_BOOTLOADER	0x01

void hidasp_delay_ms(int dly);
/*********************************************************************
 *	使用法を表示
 *********************************************************************
 */
void usage(void)
{
	fputs( HELP_USAGE, stdout );
}
/*********************************************************************
 *	一行入力ユーティリティ
 *********************************************************************
 */
static	int getln(char *buf,FILE *fp)
{
	int c;
	int l;
	l=0;
	while(1) {
		c=getc(fp);
		if(c==EOF)  {
			*buf = 0;
			return(EOF);/* EOF */
		}
		if(c==0x0d) continue;
		if(c==0x0a) {
			*buf = 0;	/* EOL */
			return(0);	/* OK  */
		}
		*buf++ = c;
		l++;
		if(l>=255) {
			*buf = 0;
			return(1);	/* Too long line */
		}
	}
}
/**********************************************************************
 *	INTEL HEXレコード 形式01 を出力する.
 **********************************************************************
 */
static	int	out_ihex01(unsigned char *data,int adrs,int cnt,FILE *ofp,int rec_id)
{
	int length, i, sum;
	unsigned char buf[1024];

	buf[0] = cnt;
	buf[1] = adrs >> 8;
	buf[2] = adrs & 0xff;
	buf[3] = rec_id;

	length = cnt+5;
	sum=0;

	memcpy(buf+4,data,cnt);
	for(i=0; i<length-1; i++) {
		sum += buf[i];
	}
	sum = 256 - (sum&0xff);
	buf[length-1] = sum;

	fprintf(ofp,":");
	for(i=0; i<length; i++) {
		fprintf(ofp,"%02X",buf[i]);
	}
	fprintf(ofp,"\n");
	return 0;
}
/**********************************************************************
 *	INTEL HEXレコード 形式01 (バイナリー化済み) を解釈する.
 **********************************************************************
 */
static void put_flash_buf(int adrs,int data)
{
	adrs &= 0x1fffffff;	// 論理アドレスを物理アドレスに変換.

	adrs = adrs - FLASH_BASE;
	if( (adrs >= 0) && (adrs < FLASH_SIZE ) ) {
		flash_buf[adrs] = data;
	}
}
/**********************************************************************
 *	INTEL HEXレコード 形式01 (バイナリー化済み) を解釈する.
 **********************************************************************
 */
static	int	parse_ihex01(unsigned char *buf,int length)
{

	int adrs = (buf[1] << 8) | buf[2];
	int cnt  =  buf[0];
	int i;
	int sum=0;

#if	CHECKSUM_CALC
	for(i=0; i<length; i++) {
		sum += buf[i];
	}
//	printf("checksum=%x\n",sum);
	if( (sum & 0xff) != 0 ) {
		fprintf(stderr,"%s: HEX RECORD Checksum Error!\n", CMD_NAME);
		exit(EXIT_FAILURE);
	}
#endif

	buf += 4;

#if	HEX_DUMP_TEST
	printf("%04x",adrs|adr_u);
	for(i=0; i<cnt; i++) {
		printf(" %02x",buf[i]);
	}
	printf("\n");
#endif

	adrs |= adr_u;
	for(i=0; i<cnt; i++) {
		put_flash_buf(adrs+i,buf[i]);
	}
	return 0;
}
/**********************************************************************
 *	INTEL HEXレコード(１行:バイナリー化済み) を解釈する.
 **********************************************************************
 */
int	parse_ihex(int scmd,unsigned char *buf,int length)
{
	switch(scmd) {
	case 0x00:	//データレコード:
		//データフィールドは書き込まれるべきデータである。
		return parse_ihex01(buf,length);

	case 0x01:	//エンドレコード:
		//HEXファイルの終了を示す.付加情報無し.
		return scmd;
		break;

	case 0x02:	//セグメントアドレスレコード:
		//データフィールドにはセグメントアドレスが入る。 たとえば、
		//:02000002E0100C
		//		   ~~~~
		//
		break;

	case 0x03:	//スタートセグメントアドレスレコード.
		//とりあえず無視.
		return scmd;
		break;

	case 0x04:	//拡張リニアアドレスレコード.
		//このレコードでは32ビットアドレスのうち上位16ビット
		//（ビット32～ビット16の）を与える。
		adr_u = (buf[4]<<24)|(buf[5]<<16);
//				printf("adr_u=%x\n",adr_u);
		return scmd;
		break;

	case 0x05:	//スタートリニアアドレス：
		//例えば
		//:04000005FF000123D4
		//         ~~~~~~~~
		//となっていれば、FF000123h番地がスタートアドレスになる。
		//とりあえず無視.
		return scmd;
		break;

	}
	fprintf(stderr,"Unsupported ihex cmd=%x\n",scmd);
	return 0;
}

/**********************************************************************
 *	16進2桁の文字列をバイナリーに変換する.
 **********************************************************************
 */
int read_hex_byte(char *s)
{
	char buf[16];
	int rc = -1;

	buf[0] = s[0];
	buf[1] = s[1];
	buf[2] = 0;
	sscanf(buf,"%x",&rc);
	return rc;
}

/**********************************************************************
 *	16進で書かれた文字列をバイナリーに変換する.
 **********************************************************************
 */
static	int read_hex_string(char *s)
{
	int rc;

	dataidx = 0;
	while(*s) {
		rc = read_hex_byte(s);
		s+=2;
		if(rc<0) return rc;
		databuf[dataidx++]=rc;
	}
	return 0;
}

/**********************************************************************
 *
 **********************************************************************
 0 1 2 3 4
:1000100084C083C082C081C080C07FC07EC07DC0DC
 ~~ データ長
   ~~~~ アドレス
       ~~ レコードタイプ
         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~データ
                                         ~~チェックサム.
 */
int	read_ihex(char *s)
{
	int c;
	if(s[0] == ':') {
		read_hex_string(s+1);
		c = databuf[3];
		parse_ihex(c,databuf,dataidx);
	}
	return 0;
}
/*********************************************************************
 *	HEX file を読み込んで flash_buf[] に格納する.
 *********************************************************************
 */
int	read_hexfile(char *fname)
{
	FILE *ifp;
	int rc;
	Ropen(fname);
	while(1) {
		rc = getln(lnbuf,ifp);
		read_ihex(lnbuf);
		if(rc == EOF) break;
	}
	Rclose();
	return 0;
}

/*********************************************************************
 *	flash_buf[]の指定エリアが 0xffで埋まっていることを確認する.
 *********************************************************************
 *	埋まっていれば 0 を返す.
 */
#if	0
static	int	check_prog_ff(int start,int size)
{
	int i;
	for(i=0; i<size; i++) {
		if(flash_buf[start+i]!=0xff) return 1;
	}
	return 0;
}
#endif
/*********************************************************************
 *
 *********************************************************************
 *
 */
static	void modify_vector( int addr , int dst )
{
	int i;
	for(i=0; i<0x180; i++) {
		flash_buf[addr+i] = flash_buf[dst+i];
	}
}
/*********************************************************************
 *	必要なら、flash_buf[] の 0x800,0x808,0x818番地を書き換える.
 *********************************************************************
 */
void modify_start_addr(int start)
{
	if(start == 0x2000) return;	//デフォルト値なので何もしない.

	if(start <  0x2000) {		// ユーザープログラム開始番地が0x800以前にあるのはおかしい.
		fprintf(stderr,"WARNING: start address %x < 0x2000 ?\n",start);
		return;
	}
	// ユーザーファームの例外ベクターエリア(startがoffsetになっている)
	// を0x2000番地にコピーする.

	// こうすることで、任意番地(ただし256byte倍数番地)開始のファームを
	// jumper判定によりそのまま起動させたりできる.
	modify_vector( 0x2000 , start );
}
int	read_block(int addr,uchar *read_buf,int size);

/*********************************************************************
 *	ターゲットの再起動.
 *********************************************************************
 */
int reboot_target(int vect)
{
	UsbBootTarget(0,1,0);
	return 0;
#if	0
	int	buf[32];
	read_block(FLASH_BASE+vect,(uchar*)buf,32);
	int boot_pc = buf[1];	// PC

	if(( boot_pc >= FLASH_BASE)&&
	        ( boot_pc < (FLASH_BASE+FLASH_SIZE)) ) {
		printf("reset vector: %08x\n",boot_pc);
		UsbBootTarget(boot_pc,1,0);
	} else {
		printf("Illegal reset vector: %08x\n",boot_pc);
	}
	return 0;
#endif
}
void Flash_Unlock(void)
{
	UsbFlashLock(0);
}
void Flash_Lock(void)
{
	UsbFlashLock(1);
}

/*********************************************************************
 *	1024 byteを消去.
 *********************************************************************
 */
int	erase_block(int addr,int size)
{
	int i,f=0;
	int pages = size / 1024;

	for(i=0; i<size; i++) {
		if(flash_buf[addr+i]!=0xff) f=1;
	}

	if((f) || (opt_E)) {
#if	0	// TEST
		printf("UsbEraseTargetROM(%x,%x)\n",FLASH_BASE+addr , pages);
#else
		UsbEraseTargetROM(FLASH_BASE+addr , pages);
#endif
		Sleep(8);
		Flash_Lock();
	}
	return 0;
}

/*********************************************************************
 *	flash_buf[] の内容を ターゲット PIC に書き込む.
 *********************************************************************
 */
int	write_block_usb_packet(int addr,int size)
{
	int i,f=0;
	for(i=0; i<size; i++) {
		if(flash_buf[addr+i]!=0xff) f=1;
	}
	if(f) {
		UsbFlash(FLASH_BASE+addr ,AREA_PGMEM,flash_buf+addr,size);
	}
	return 0;
}

/*********************************************************************
 *	flash_buf[] の内容を 書き込む( 1PAGE = 1kBytes )
 *********************************************************************
 */
int	write_block_page(int addr,int size)
{
	int wsize;
	while(size) {
		wsize = size;
		if(	wsize>= WRITE_SIZE) {
			wsize = WRITE_SIZE;		//１回の書き込み可能サイズ
		}
		write_block_usb_packet(addr,wsize);
		addr += wsize;
		size -= wsize;
	}

	return 0;
}
/*********************************************************************
 *	flash_buf[] の内容を ターゲット PIC に書き込む.
 *********************************************************************
 *	処理単位は FLASHROM のページサイズ.(STM32=1K LPC1343=4K)
 */
int	write_block(int addr,int size)
{
	int i,f=0;
	for(i=0; i<size; i++) {
		if(flash_buf[addr+i]!=0xff) f=1;
	}

	if(f) {
		write_block_page(addr,size);		// 1ページ分書き込む.
		fprintf(stderr,"Writing ... %04x\r",addr);
	}
	return 0;
}
/*********************************************************************
 *	 ターゲット PIC の内容を flash_buf[] に読み込む.
 *********************************************************************
 */
int	read_block(int addr,uchar *read_buf,int size)
{
	int errcnt=0;
	int rsize;

	while(size) {
		rsize = size;
		if(	rsize>= READ_SIZE) {
			rsize = READ_SIZE;		//１回の読み出し可能サイズ=64byte
		}
//		UsbRead(addr ,AREA_PGMEM,read_buf,rsize);
		code_buf_read(addr ,rsize ,read_buf);
		addr     += rsize;
		read_buf += rsize;
		size -= rsize;
	}
	return errcnt;
}


/*********************************************************************
 *	flash_buf[] の内容を ターゲット PIC に書き込む.
 *********************************************************************
 */
int	verify_block(int addr,int size)
{
	uchar verify_buf[FLASH_STEP];
	int i,f=0;
	int errcnt=0;

	for(i=0; i<size; i++) {
		if(flash_buf[addr+i]!=0xff) f=1;
	}

	if(f) {
		read_block(addr,verify_buf,size);
		for(i=0; i<size; i++) {
			if(flash_buf[addr+i] != verify_buf[i]) {
				errcnt++;
				fprintf(stderr,"Verify Error in %x : write %02x != read %02x\n"
				        ,addr+i,flash_buf[addr+i],verify_buf[i]);
			}
		}
		fprintf(stderr,"Verifying ... %04x\r",addr);
	}
	return errcnt;
}
/*********************************************************************
 *	FLashの消去.
 *********************************************************************
 */
int	erase_flash(void)
{
	int i;
	fprintf(stderr,"Erase ... %x-%x\n",flash_start,flash_end);
	for(i=flash_start; i<flash_end; i+= ERASE_STEP) {
		erase_block(i,ERASE_STEP);
	}
	return 0;
}
/*********************************************************************
 *	ダミー.
 *********************************************************************
 */
int disasm_print(unsigned char *buf,int size,int adr)
{
	unsigned short *inst = (unsigned short *)buf;
	printf("%04x %04x\n",adr,inst[0]);
	return 2;
}
/*********************************************************************
 *	flash_buf[] の内容を ターゲット PIC に書き込む.
 *********************************************************************
 */
int	write_hexdata(void)
{
	int i;
//	printf(": flash_start %x ,flash_end %x\n",flash_start,flash_end);
	for(i=flash_start; i<flash_end; i+= FLASH_STEP) {
		write_block(i,FLASH_STEP);
	}
	return 0;
}
/*********************************************************************
 *	flash_buf[] の内容を ターゲット PIC に書き込む.
 *********************************************************************
 */
int	verify_hexdata(void)
{
	int i;
	int errcnt = 0;

	fprintf(stderr,"\n");
	for(i=flash_start; i<flash_end; i+= FLASH_STEP) {
		errcnt += verify_block(i,FLASH_STEP);
	}
	return errcnt;
}
/*********************************************************************
 *	ポート名称からアドレスを求める.（ダミー処理）
 *********************************************************************
 */
int	portAddress(char *s)
{
	return 0;
}

/*********************************************************************
 *	ROM読み出し結果をHEXで出力する.
 *********************************************************************
 */
#define	HEX_DUMP_STEP	16

void print_high_adrs(FILE *ofp,int addr)
{
	//":020000040000F2";
	static  uchar hi_record[64]= {0x02,0x00,0x00,0x04,0x00,0x00,0xf2};
	int hi = (FLASH_BASE+addr) & HIGH_MASK;
	if(	adr_u != hi) {
		adr_u  = hi;
		hi_record[0]=adr_u >> 24;
		hi_record[1]=adr_u >> 16;
		out_ihex01(hi_record,0,2,ofp,0x04);
	}
}

void print_hex_block(FILE *ofp,int addr,int size)
{
	int i,j,f;

	print_high_adrs(ofp,addr);

	for(i=0; i<size; i+=HEX_DUMP_STEP,addr+=HEX_DUMP_STEP) {
		f = 0;
		for(j=0; j<HEX_DUMP_STEP; j++) {
			if( flash_buf[addr+j] != 0xff ) {
				f = 1;
			}
		}
		if(f) {

#if	0
			printf(":%04x",addr);
			for(j=0; j<HEX_DUMP_STEP; j++) {
				printf(" %02x",flash_buf[addr+j]);
			}
			printf("\n");
#endif
			out_ihex01(&flash_buf[addr],addr,HEX_DUMP_STEP,ofp,0x00);


		}
	}


}
/*********************************************************************
 *	ROM読み出し.
 *********************************************************************
 */
void read_from_pic(char *file)
{
	int i, progress_on;
	FILE *ofp;
	int adr=0;
	uchar *read_buf;

//	fprintf(stderr, "Reading... (%x-%x)\n",flash_start,flash_end_for_read);
	fprintf(stderr, "Reading... \n");
	progress_on = 0;
	Wopen(file);

//	fprintf(ofp,":020000040000FA\n");

	adr_u=-1;

	for(i=flash_start; i<flash_end_for_read; i+= FLASH_STEP) {
		adr = 0x80000000 + FLASH_BASE + i;
		if (progress_on) {
			fprintf(stderr,"Reading ... %08x\r",adr);
		}
		read_buf = &flash_buf[i] ;
		read_block(adr,read_buf,FLASH_STEP);
		print_hex_block(ofp,i,FLASH_STEP);
		fflush(ofp);
	}
	fprintf(ofp,":00000001FF\n");
	if (progress_on) {
		fprintf(stderr,"\nRead end address = %08x\n", adr-1);
	}
	Wclose();
}
/*********************************************************************
 *	メイン
 *********************************************************************
 */
void getopt_p(char *s)
{
	if (s) {
		if (*s == ':') s++;
		if (*s == '?') {
			//hidasp_list("picmon");
			exit(EXIT_SUCCESS);
		} else if (isdigit(*s)) {
			if (s) {
				int n, l;
				l = strlen(s);
				if (l < 4 && isdigit(*s)) {
					n = atoi(s);
					if ((0 <= n) && (n <= 999)) {
						sprintf(usb_serial, "%04d", n);
					} else {
						if (l == 1) {
							usb_serial[3] = s[0];
						} else if  (l == 2) {
							usb_serial[2] = s[0];
							usb_serial[3] = s[1];
						} else if  (l == 3) {
							usb_serial[1] = s[0];
							usb_serial[2] = s[1];
							usb_serial[3] = s[2];
						}
					}
				} else {
					strncpy(usb_serial, s, 4);
				}
			}
		} else if (*s == '\0') {
			strcpy(usb_serial, DEFAULT_SERIAL);		// -pのみの時
		} else {
			strncpy(usb_serial, s, 4);
		}
		strupr(usb_serial);
	}
}
int flash_get_status();
/*********************************************************************
 *	メイン
 *********************************************************************
 */
int main(int argc,char **argv)
{
	int errcnt, ret_val;	//,retry;
	int dev_flash_size=0;
	int dev_flash_used=0;

	//オプション解析.
	Getopt(argc,argv,"i");
	if(IsOpt('h') || IsOpt('?') || IsOpt('/')) {
		usage();
		exit(EXIT_SUCCESS);
	}
	if((argc<2)&& (!IsOpt('r')) && (!IsOpt('E')) && (!IsOpt('p')) ) {
		usage();
		exit(EXIT_SUCCESS);
	}

#if	0
	if(IsOpt('B')) {					// Bootエリアの強制読み書き.
		flash_start = 0;
		flash_end   = FLASH_START;
	}
#endif
	if(IsOpt('E')) {
		opt_E = 1;				//強制消去あり.
	}
	if(IsOpt('p')) {
		getopt_p(Opt('p'));		//シリアル番号指定.
	}
	if(IsOpt('s')) {
		sscanf(Opt('s'),"%x",&opt_s);	//アプリケーションの開始番地指定.
	}
	if(IsOpt('r')) {
		char *r = Opt('r');
		if(r[0]==0  ) opt_r = 1;		//書き込み後リセット動作あり.
		if(r[0]=='p') opt_rp = 1;		//'-rp' flash領域の読み込み.
		if(r[0]=='e') opt_re = 1;		//'-re'
		if(r[0]=='f') opt_rf = 1;		//'-rf'
	}
	if(IsOpt('n')) {
		char *n = Opt('n');
		if(n[0]=='v') opt_nv = 1;		//'-nv'
	}
	if(IsOpt('v')) {
		opt_v = 1;						//'-v'
		opt_nv= 0;
	}

	//初期化.
	if( UartInit(0) < 0) {
		fprintf(stderr, "%s: UsbInit failure.\n", CMD_NAME);
		exit(EXIT_FAILURE);
	} else {
		fprintf(stderr, "UsbInit(\"%s\") Ok.\n", usb_serial);
	}
	UsbGetDevCaps(&dev_flash_size,&dev_flash_used);
//	=================================
	if(!IsOpt('B')) {
		flash_end = dev_flash_size & END_MASK;				// FLASHの実容量.
		flash_end_for_read = dev_flash_used & END_MASK;		// FLASHの使用済み容量.
		
		if(	flash_end >= FLASH_SIZE){
			flash_end  = FLASH_SIZE;
		}
		if(	flash_end_for_read >= FLASH_SIZE){
			flash_end_for_read  = FLASH_SIZE;
		}

#if	1
		printf("flash_end=%x\n",flash_end);
		printf("flash_end_for_read=%x\n",flash_end_for_read);
#endif
	}

	Flash_Unlock();


	if((opt_E) && (argc < 2)) {					// 強制消去の実行.
		erase_flash();					//  Flash消去.
		Flash_Unlock();
	}

	code_buf_init();
	memset(flash_buf,0xff,FLASH_SIZE);
	ret_val = EXIT_SUCCESS;

	if(argc>=2) {
		if(opt_rp||opt_re||opt_rf) {	// ROM読み出し.
			read_from_pic(argv[1]);
		} else {
			read_hexfile(argv[1]);		//	HEXファイルの読み込み.
			modify_start_addr( opt_s & 0x7ffff );

			if(IsOpt('v')==0) {			// ベリファイのときは書き込みをしない.
				erase_flash();
				Flash_Unlock();
				write_hexdata();			//  書き込み.

				// 合否判定.
				int rc = flash_get_status();			// 書き込みエラーカウント.
				if(rc==0) {
					fprintf(stderr,"\nWrite Ok.\n");
					opt_nv=1;
				} else {
					fprintf(stderr,"\nWrite Error (%d).\n",rc);
				}
			}


			if(opt_nv==0) {
				errcnt = verify_hexdata();	//  ベリファイ.
				if(errcnt==0) {
					fprintf(stderr,"\nVerify Ok.\n");
				} else {
					fprintf(stderr,"\nVerify Error. errors=%d\n", errcnt);
					ret_val = EXIT_FAILURE;
				}
			}
		}
	}
	else if (argc==1) {
		if(opt_rp||opt_re||opt_rf) {	// ROM読み出し.
			read_from_pic(NULL);
		}
	}

	Flash_Lock();

	if(opt_r) {
		reboot_target(FLASH_START);		//  ターゲット再起動.
	}
	UartExit();
	return ret_val;
}
/*********************************************************************
 *
 *********************************************************************
 */

