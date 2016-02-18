/*********************************************************************
 *	USB経由のターゲットメモリーアクセス
 *********************************************************************
 *API:
int		UsbInit(int verbose,int enable_bulk, char *serial);			初期化.
int		UsbExit(void);												終了.
void 	UsbBench(int cnt,int psize);					ベンチマーク

void 	UsbPoke(int adr,int arena,int data,int mask);	書き込み
int 	UsbPeek(int adr,int arena);						1バイト読み出し
int 	UsbRead(int adr,int arena,uchar *buf,int size);	連続読み出し
void	UsbDump(int adr,int arena,int size);			メモリーダンプ

void 	UsbFlash(int adr,int arena,int data,int mask);	連続書き込み
 *
 *内部関数:
void	dumpmem(int adr,int cnt);
void	pokemem(int adr,int data0,int data1);
void	memdump_print(void *ptr,int len,int off);
 */

#ifndef	_LINUX_
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#include "monit.h"
#include "portlist.h"
#include "util.h"
//#include "../../firmware/monitor/hidcmd.h"
#include "hidcmd.h"
#include "packet.h"
#include "hex.h"

#define	VERBOSE	0

#define	if_V	if(VERBOSE)

static	char verbose_mode = 0;
static  int  HidLength0 = PACKET_SIZE;
static  int  HidLength1 = PACKET_SIZE;
#define	ID1			1
#define	ID4			1
#define	LENGTH4		PACKET_SIZE


typedef	struct {
	uint	start,size;
} Region;

#define	AREA_MASK	0xe0000000
#define	ADDR_MASK	0x1FFFFFFF
static	Region region[]={
	{0x00000000,0x2000	},	// RAM
	{0x1D000000,0x8000	},	// FLASH
	{0x1FC00000,0x0C00	},	// BOOTROM
	{0x1F800000,0x10000	},	// PORT
	{0x1F880000,0x10000	},	// PORT
	{		  0,0		},	// END
};
static	int	   area_tab[4]={
	0x00000000,
	0x20000000,
	0x80000000,
	0xa0000000,
};

int	check_region(int addr,int size)
{
	int area = addr & AREA_MASK;
			   addr &= ADDR_MASK;
	int i;
	uint off,sz;
	for(i=0;i<4;i++) {
		if(	area_tab[i] == area) {	// エリアブロックに嵌っている.
			Region *r = region;		// 領域テーブルを舐める.
			while(r->size) {		// テーブル末ではないなら...
				off = addr - r->start;	// 要求アドレスはベースアドレスから何バイト目(off)か?
				if(off < (r->size) ) {	// 領域サイズ内に収まっている.
					//残量を返す.
					sz = (r->size - off);	//その領域内の残量を計算.
					if(size >= sz) {size=sz;}
					return size;
				}
				r++;
			}
		}
	}
	return -1;
}

void set_region_flash_size(int size)
{
	region[1].size = size;
}
// ReportID:4  POLLING PORTが実装済である.
static	int	pollcmd_implemented=0;

#define	READ_BENCH			0

int disasm_print(unsigned char *buf,int size,int adr);

#define	USE_CODE_BUF		1
int code_buf_init();
int code_buf_read(int adr,int size,uchar *buf);

//	exit()
#define	EXIT_SUCCESS		0
#define	EXIT_FAILURE		1

extern	char	reboot_done;

/****************************************************************************
 *	メモリー内容をダンプ.
 ****************************************************************************
 *	void *ptr : ダンプしたいデータ.
 *	int   len : ダンプしたいバイト数.
 *	int   off : ターゲット側の番地表示.
 */
void memdump_print(void *ptr,int len,int off)
{
	unsigned char *p = (unsigned char *)ptr;
	int i;	//,j,c;

	for(i=0;i<len;i++) {
		if( (i & 15) == 0 ) printf("%08lx", ((long)p - (long)ptr + off) );
		printf(" %02x",*p);p++;
		if( (i & 15) == 7 ) printf(" ");

		if( (i & 15) == 15 ) printf("\n");

		if( (i & 15) == (len-1) )
		{
#if	0	// ASCII DUMP
			printf("  ");
			for(j=0;j<len;j++) {
				c=p[j-len];
				if(c<' ') c='.';
				if(c>=0x7f) c='.';
				printf("%c",c);
			}
#endif
		}
	}
//	printf("\n");
}



/*********************************************************************
 *	AVRデバイスに処理コマンドを送って、必要なら結果を受け取る.
 *********************************************************************
 *	cmdBuf   *cmd : 処理コマンド
 *	uchar    *buf : 結果を受け取る領域.
 *  int reply_len : 結果の必要サイズ. (0の場合は結果を受け取らない)
 *戻り値
 *		0	: 失敗
 *	   !0   : 成功
 */
static int QueryAVR(cmdBuf *cmd)
{
	int rc = 0;
	rc = hidWriteBuffer(&cmd->report_id , HidLength1 );
	if(rc == 0) {
		printf("\nhidWrite error\n");
		exit(EXIT_FAILURE);
	}
	return rc;
}
/*********************************************************************
 *	ターゲット側のメモリー内容を取得する
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *	int           size :読み取りサイズ.
 *	unsigned char *buf :受け取りバッファ.
 *	注意: HID Report Length - 1 より長いサイズは要求してはならない.
 */
int dumpmem(int adr,int width,int size,unsigned char *buf)
{
	uchar reply[256];
	int rc,rsize;

	memset(buf, 0xff , size);

	rsize = check_region(adr,size);
	if( rsize > 0 ) {
		//printf("getData(adr %x,rsize %x)\n",adr,rsize);
		rc = getData(adr,rsize,reply);
		if(rc!=0) return -1;
		//読み取ったメモリーブロック部分のみをコピーで返す.
		memcpy(buf, reply, rsize);
	}

	return size;
}

/*********************************************************************
 *
 *********************************************************************
 */
int UsbBootTarget(int resetpc,int bootflag,int vector)
{
	execCode(resetpc);
	return 1;

#if	0
	cmdBuf cmd;
//	int   *cmdvect= (int*) &cmd.data;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_JMP;
	cmd.adr   = resetpc;		// 実行開始番地.
	cmd.size  = bootflag;		// 1ならUSBバスをリセットする.
//	*cmdvect  = vector;			// FM3のみで必要: 仮想vector tableのアドレス.

	if(	bootflag ) {			// 別のファームを起動させた場合は、戻ってこない.
		reboot_done = 1;
	}

	if( QueryAVR(&cmd) == 0) return 0;	//失敗.

	return 1;
#endif
}

/*********************************************************************
 *	hostPC-->device への下りストリームテスト.
 *********************************************************************
 */
#define TESTSIZE 60

int UsbTestTarget(int size)
{
#if	0
	cmdBuf cmd;
	static	int adr0=0;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_BOOT_RWW;	//一応ダミーコマンドになっている.
	cmd.adr   = adr0++;
	cmd.size  = size;		// 1ならUSBバスをリセットする.

	printf("test size=%d\n",size);

	if( QueryAVR(&cmd) == 0) return 0;	//失敗.

	if(size >= 64) {
		uchar buf[64];
		memset(buf,0,64);
		while(size>0) {
			int len = size;
			if( len >= TESTSIZE) { len = TESTSIZE; }
			hidWriteBuffer2(buf , len );
			size -= len;
		}
	}

	UsbPeek(0,MEM_BYTE);
#endif
	return 1;
}

/*********************************************************************
 *	device-->hostPC への上りストリームテスト.
 *********************************************************************
 */
int UsbTestUpload(int size)
{
	uchar buf[4096];	// 1080.
	cmdBuf cmd;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_SAMPLING;
	cmd.subcmd= 1;	// analog.
	cmd.size  = size;
	cmd.adr   = 7;	// sample rate (0=1X 7=256X)

//	printf("test size=%d\n",size);

	if( QueryAVR(&cmd) == 0) return 0;	//失敗.

	hidReadBuffer(buf,size);
	memdump_print(buf,16,0);		//結果印字.
	return 1;
}

int	hidGetChars();
/*********************************************************************
 *
 *********************************************************************
 */
int UsbExecUser(int arg)
{
	cmdBuf cmd;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_USER_CMD;
	cmd.adr   = arg;

	if( QueryAVR(&cmd) == 0) return 0;	//失敗.

	hidGetChars();
	printf("\n* Terminate.\n");

	return 1;
}

/*********************************************************************
 *
 *********************************************************************
 */
int UsbReadString(uchar *buf)
{
	cmdBuf cmd;
	int rc;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_GET_STRING;

	buf[0]=0;
	if( QueryAVR(&cmd) == 0) return 0;	//失敗.


	rc = hidReadBuffer(buf+1 , HidLength0);
	if(rc == 0) {
		printf("\nhidRead error \n");
		exit(EXIT_FAILURE);
	}

	return 1;
}

/*********************************************************************
 *
 *********************************************************************
 */
int UsbPutKeyInput(int key)
{
	cmdBuf cmd;

	memset(&cmd,0,sizeof(cmdBuf));
		cmd.cmd   = HIDASP_KEYINPUT;
		cmd.size  = key;

	if( QueryAVR(&cmd) == 0) return 0;	//失敗.
	return 1;
}

/*********************************************************************
 *	ターゲット側のメモリー内容(1バイト)を書き換える.
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *	int          data0 :書き込みデータ.      (OR)
 *	int          data1 :書き込みビットマスク.(AND)
 *注意
 *	pic32mxのbootloaderはメモリー節約のため DWORDアクセスのみ実装されている.
 *	よって、maskは実装されない.
 *	char,shortのアクセスを行う場合はread modifiedにする必要がある.
 */
int	pokemem(int adr,int arena,int data0,int data1)
{
	int buf[64/4];
	char *byte=(char *)buf;
	int off = adr & 3;	//端数.
	int size = 4;
	if(arena == MEM_BYTE) {
		int rc = UsbRead(adr & (-4),MEM_WORD,byte,size);
		byte[off]=data0;
	}else if(arena == MEM_HALF) {
		int rc = UsbRead(adr & (-4),MEM_WORD,byte,size);
		byte[off]=data0;		// FIXME! SHORT WORD!
	}else{
		buf[0]=data0;
	}
	return putData(adr & (-4),size,(char*) buf);

#if	0
	int 					size = 1;
	if(arena==MEM_HALF) 	size = 2;
	if(arena==MEM_WORD) 	size = 4;

	buf[0]=data0;

	printf("putData(%x %d)\n",adr,size);

	return putData(adr,size,(char*) buf);
#endif
}

void hid_read_mode(int mode,int adr)
{
	cmdBuf cmd;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_SET_MODE;
	cmd.size  = mode;
	cmd.adr   = adr;
	QueryAVR(&cmd);

}

int hid_read(int cnt)
{
	uchar buf[256];	// dummy
	int rc;
	rc = hidReadBuffer(buf , HidLength0 );
	if(rc == 0) {
		printf("\nhidRead error \n");
		exit(EXIT_FAILURE);
	}
	return rc;
}

/*********************************************************************
 *	ターゲットとの接続チェック(ベンチマークを兼ねる)
 *********************************************************************
 *	int i : 0～255 の数値.
 *戻り値
 *		エコーバックされた i の値.
 */
int hid_ping(int i)
{
 	cmdBuf cmd;
	uchar buf[256];	// dummy
	int rc;

	memset(&cmd,i,sizeof(cmdBuf));
	cmd.cmd   = CMD_PING ;
	QueryAVR(&cmd);

	rc = hidReadBuffer(buf+1 , HidLength0 );
	if(rc == 0) {
		printf("\nhidRead error\n");
		exit(EXIT_FAILURE);
	}
	// +00        +01      +02     +03     +04     +05        +06
	// [ReportID] [CMD]  [DEV_ID] [VER_L] [VER_H] [BOOTFLAG] [Echoback]
	//		DEV_ID = 0xa3 (ARMCortexM3)
	//		VER_L  = 0x01
	//		VER_H  = 0x01
	//		BOOTFLAG  0x01=bootloader 0x00=application

	return buf[6] & 0xff;
}
/*********************************************************************
 *	転送速度ベンチマーク
 *********************************************************************
 *	int cnt  : PINGを打つ回数.
 *	int psize: PINGパケットの大きさ(現在はサポートされない)
 */
void UsbBench(int cnt,int psize)
{
	int i,n,rc;
	int time1, rate;
#ifdef __GNUC__
	long long total=0;
#else
	long total=0;
#endif
	int nBytes;
	int time0;

#if		READ_BENCH
	nBytes  = HidLength1-1 ;				//登りパケットのみ計測.
#else
	nBytes  = HidLength1-1 + HidLength1-1 ;	//現在固定.
#endif
	// 1回のPingは 63byteのQueryと63バイトのリターン.

   	printf("hid write start\n");
	time0 = clock();
#if		READ_BENCH
	hid_read_mode(1,0);
#endif

	for(i=0;i<cnt;i++) {
		n = i & 0xff;
#if		READ_BENCH
		rc = hid_read(i);
#else
		rc = hid_ping(n);
		if(rc != n) {
			printf("hid ping mismatch error (%x != %x)\n",rc,n);
			exit(EXIT_FAILURE);
		}
#endif
		total += nBytes;
	}

#if		READ_BENCH
	hid_read_mode(0,0);
#endif

	time1 = clock() - time0;
	if (time1 == 0) {
		time1 = 2;
	}
	rate = (int)((total * 1000) / time1);

	if (total > 1024*1024) {
	   	printf("hid write end, %8.3lf MB/%8.2lf s,  %6.3lf kB/s\n",
	   		(double)total/(1024*1024), (double)time1/1000, (double)rate/1024);
	} else 	if (total > 1024) {
	   	printf("hid write end, %8.3lf kB/%8.2lf s,  %6.3lf kB/s\n",
	   		 (double)total/1024, (double)time1/1000, (double)rate/1024);
	} else {
	   	printf("hid write end, %8d B/%8d ms,  %6.3lf kB/s\n",
	   		(int)total, time1, (double)rate/1024);
   	}
}
/*********************************************************************
 *	ターゲット側メモリーをダンプする.
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *	int            cnt :読み取りサイズ.
 */
void UsbDump(int adr,int arena,int cnt)
{
	unsigned char buf[16];
	int size;
	int rc,rsize;
	while(cnt) {
		size = cnt;
		if(size>=16) size = 16;

		rsize = check_region(adr,size);
		if(	rsize <= 0) break;

		rc = dumpmem(adr,arena,rsize,buf);	//メモリー内容の読み出し.
		memdump_print(buf,rsize,adr);		//結果印字.

		adr += size;
		cnt -= size;
	}
}

/*********************************************************************
 *	ターゲット側メモリーを逆アセンブルする.
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *	int            cnt :読み取りサイズ.
 */
int UsbDisasm (int adr, int arena, int cnt)
{
	unsigned char buf[16];
	int size = 8;
	int done = 0;
	int endadr = adr + cnt;
	int rc,disbytes;

	code_buf_init();

	while(adr < endadr) {
		int rsize = check_region(adr,4);
		if(	rsize < 0) break;

#if	USE_CODE_BUF		
		rc = code_buf_read(adr,size,buf);	//メモリー内容の読み出し.
#else
		rc = dumpmem(adr,AREA_PGMEM,size,buf);	//メモリー内容の読み出し.
#endif
		if(rc != size) return done;
		disbytes = disasm_print(buf,size,adr);	//結果印字.
		adr += disbytes;
		done+= disbytes;
	}
	return done;
}

/*********************************************************************
 *	ターゲット側メモリー連続読み出し
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *	uchar         *buf :読み取りバッファ.
 *	int           size :読み取りサイズ.
 *戻り値
 *	-1    : 失敗
 *	正の値: 成功.
 */
int UsbRead(int adr,int arena,uchar *buf,int size)
{
	int rc = size;
	int len;
	while(size) {
		int rc1;
		len = size;
		if(len >= 32) len = 32;
		rc1 = dumpmem(adr,arena,len,buf);
		if( rc1!= len) {
			return -1;
		}
		adr  += len;	// ターゲットアドレスを進める.
		buf  += len;	// 読み込み先バッファを進める.
		size -= len; 	// 残りサイズを減らす.
	}
	return rc;
}
/*********************************************************************
 *	ターゲット側メモリー1バイト読み出し
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *戻り値
 *	メモリー内容の値.
 *注意
 *	pic32mxのbootloaderはメモリー節約のため DWORDアクセスのみ実装されている.
 *	なので、char,shortのアクセスを行う場合はDWORD境界から4byte読んで、必要箇所
 *	を取り出す必要がある.
 */
int UsbPeek(int adr,int width)
{
	unsigned char buf[16];
	int off= (adr & 3);		//端数.
	int size=4;
	int rc = UsbRead(adr & (-4),width,buf,size);
	if( rc != size) {
		return -1;
	}
	switch(width) {
	  default:
	  case MEM_BYTE:
		return buf[off+0];
	  case MEM_HALF:
		return buf[off+0] | (buf[off+1]<<8);
	  case MEM_WORD:
		return buf[0] | (buf[1]<<8)| (buf[2]<<16)| (buf[3]<<24);
	}
}

static	int	poll_port = 0;

/*********************************************************************
 *	ターゲット側メモリー1バイト連続読み出しのセットアップ
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *戻り値
 *	メモリー内容の値.
 */
int UsbSetPoll_slow(int adr,int arena)
{
	poll_port = adr;
	return 1;
}
/*********************************************************************
 *	ターゲット側メモリー1バイト連続読み出し
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *戻り値
 *	メモリー内容の値.
 */
int UsbPoll_slow()
{
	return UsbPeek(poll_port,MEM_WORD);
}

static	int	chkSyncCmd(uchar *buf,int i)
{
	if(buf[1]==CMD_PING) {
		if((buf[2]==0xa3)) {		// ARM-M3
		// +00        +01      +02     +03     +04     +05       +06
		// [ReportID] [CMD]  [DEV_ID] [VER_L] [VER_H] [DEVCAPS] [Echoback]
			if(buf[6]==i) return 1;
		}
	}
	return 0;	// error
}

int	UsbSyncCmd(int cnt)
{
 	cmdBuf cmd;
	uchar buf[128];	// dummy
	int rc;
	int i,c,fail,retry;

	for(i=0x80;i<0x80+cnt;i++) {
		c = i & 0xff;
		memset(&cmd,c,sizeof(cmdBuf));
		cmd.cmd   = CMD_PING ;
		QueryAVR(&cmd);

		fail=0;
		for(retry=0;retry<16;retry++) {
			rc = hidReadBuffer( buf+1 , HidLength0 );
			if(rc == 0) {
				printf("\nhidRead error\n");
				exit(EXIT_FAILURE);
				return -1;
			}
			if( chkSyncCmd(buf,c) ) {
				fail=0;				// OK.
				break;
			}else{
				fail=1;				// Retry!
			}
		}
		if(fail) {
			printf("\nhidRead Sync Error\n");
			exit(EXIT_FAILURE);
			return -1;
		}
	}
if_V	printf("hidRead Sync OK.\n");
	return 0;
}
/*********************************************************************
 *	ターゲット側メモリー1バイト連続読み出しのセットアップ
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *戻り値
 *	メモリー内容の値.
 */
int UsbSetPoll(int adr,int mode)
{
//	char buf[128];
//	char rd_buf[128];

	if( pollcmd_implemented == 0 ) {
		return UsbSetPoll_slow(adr,0);
	}

	hid_read_mode(mode,adr);

//	SYNC処理.
	if(mode==0) {
		UsbSyncCmd(1);
	}
	return 1;	// OK.
}
/*********************************************************************
 *	ターゲット側メモリー1バイト連続読み出し
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *戻り値
 *	メモリー内容の値.
 */
int UsbPoll(uint *buf)
{
	int rc = UsbPoll_slow();
	return rc;
}
int UsbPollAll(uint *buf)
{
	cmdBuf cmd;
	int rc;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_PORT_SENSE;

	if( QueryAVR(&cmd) == 0) return 0;	//失敗.

	rc = hidReadBuffer(buf , HidLength0);
	if(rc == 0) {
		printf("\nhidRead error \n");
		exit(EXIT_FAILURE);
	}
	return rc;
}


int UsbAnalogPoll(int mode,uchar *abuf)
{
	int rc;
	hidCommand(HIDASP_SET_PAGE,mode,0,0);	// Set Page mode
	if(mode == 0xa2) {
		Sleep(3);
	}
	rc = hidReadPoll(abuf , LENGTH4 ,ID4);
//	Sleep(100);
	return rc;
}


/*********************************************************************
 *
 *********************************************************************
 */
int 	UsbEraseTargetROM(int adr,int size)
{
	cmdBuf cmd;
//	printf("adrs = %x size=%x\n",adr,size);

	cmd.cmd   = HIDASP_PAGE_ERASE;
	cmd.size  = size;
	cmd.adr   = adr;

	if( QueryAVR(&cmd) == 0) return 0;	//失敗.
	return size;
}

/*********************************************************************
 *	ターゲット側のFlash内容(32バイトまで)を書き換える.
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *	int           data :書き込みデータ.
 *	int           mask :書き込み有効ビットマスク.
 *注意
 *  mask  = 0 の場合は全ビット有効 (直書きする)
 *	mask != 0 の場合は、maskビット1に対応したdataのみ更新し、他は変更しない.
 */
int UsbFlash(int adr,int arena,uchar *buf,int size)
{
	cmdBuf cmd;
	int size0=size;
	
	if(size <= 48) {
		memcpy(cmd.data,buf,size);
	}
	cmd.cmd   = HIDASP_PAGE_WRITE;
	cmd.size  = size;
	cmd.adr   = adr;

	if( QueryAVR(&cmd) == 0) return 0;	//失敗.

	if(size > 48) {
		while(size>0) {
			int len = size;
			if( len >= 60) { len = 60; }
			hidWriteBuffer2(buf , len );
			buf  += len;
			size -= len;
		}
	}
	return size0;
}

/*********************************************************************
 *	ターゲット側のFlashのLock
 *********************************************************************
 */
int UsbFlashLock(int lockf)
{
	cmdBuf cmd;

	memset(&cmd,0,sizeof(cmdBuf));
		cmd.cmd   = HIDASP_FLASH_LOCK;
		cmd.size  = lockf;

	if( QueryAVR(&cmd) == 0) return 0;	//失敗.
	return 1;
}
/*********************************************************************
 *	ターゲット側のメモリー内容(1バイト)を書き換える.
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *	int           data :書き込みデータ.
 *	int           mask :書き込み有効ビットマスク.
 *注意
 *  mask  = 0 の場合は全ビット有効 (直書きする)
 *	mask != 0 の場合は、maskビット1に対応したdataのみ更新し、他は変更しない.
 *注意２
 *	pic32mxのbootloaderはメモリー節約のため DWORDアクセスのみ実装されている.
 *	よって、maskは実装されない.
 *	char,shortのアクセスを行う場合はread modifiedにする必要がある.
 *	
 */
int	usbPoke(int adr,int arena,int data,int mask)
{
	return pokemem(adr,arena,data,0);

#if	0
	int data0 ,data1;
	if(mask == 0) {
		// マスク不要の直書き.
		data0 = data;
		data1 = 0;
	}else{
		// マスク処理を含む書き込み.
		// 書き込みデータの有効成分は mask のビットが 1 になっている部分のみ.
		// mask のビットが 0 である部分は、元の情報を保持する.

		data0 = data & mask;	// OR情報.
		data1 = 0xff &(~mask);	// AND情報.

		// マスク書き込みのロジックは以下.
		// *mem = (*mem & data1) | data0;
	}

	return pokemem(adr,arena,data0,data1);
#endif
}
/*********************************************************************
 *	ターゲット側のメモリー内容(1バイト)を書き換える.
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *	int           data :書き込みデータ.
 *	int           mask :書き込み有効ビットマスク.
 *注意
 *  mask  = 0 の場合は全ビット有効 (直書きする)
 *	mask != 0 の場合は、maskビット1に対応したdataのみ更新し、他は変更しない.
 */
int UsbPoke(int adr,int arena,int data,int mask)
{
	return usbPoke(adr,arena,data,mask);
}
/*********************************************************************
 *	1ビットの書き込み
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *	int            bit :書き込みデータ(1bit) 0 もしくは 1
 *	int           mask :書き込み有効ビットマスク.
 */
void UsbPoke_b(int adr,int arena,int bit,int mask)
{
	int data=0;
	if(mask == 0) {
		//1バイトの書き込み.
		UsbPoke(adr,arena,bit,0);
	}else{
		//1ビットの書き込み.
		if(bit) data = 0xff;
		UsbPoke(adr,arena,data,mask);
	}
}

/*********************************************************************
 *
 *********************************************************************
 */
void local_init(void)
{
}
/*********************************************************************
 *
 *********************************************************************
 */
void UsbCheckPollCmd(void)
{
#if	0
	if((UsbGetDevCaps(NULL,NULL)) == 1) {	// bootloaderである.
		pollcmd_implemented = 0;			// 実装されていない.
	}else{
		pollcmd_implemented = 1;			// 実装されている.
	}
#endif
}

#if	0
/*********************************************************************
 *	初期化
 *********************************************************************
 */
int UsbInit(int verbose,int enable_bulk, char *serial,char *baud)
{
	int type = 0;
	verbose_mode = verbose;
	if(	hidasp_init(type,serial,baud) != 0) {
		if (verbose) {
	    	fprintf(stderr, "error: [%s] device not found!\n", serial);
    	}
//    	Sleep(1000);
    	return 0;
	} else {
		local_init();
		UsbCheckPollCmd();
		return 1;	// OK.
	}
}
/*********************************************************************
 *	終了
 *********************************************************************
 */
int UsbExit(void)
{
	hidasp_close();
	return 0;
}
#endif
