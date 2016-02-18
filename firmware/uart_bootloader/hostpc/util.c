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
#include "hidcmd.h"
#include "rs232c.h"
#include "util.h"

#include "usb-uart.h"

#define	VERBOSE	0

typedef	struct {
	uint	start,size;
} Region;

#define	AREA_MASK	0xe0000000
#define	ADDR_MASK	0x1FFFFFFF
static	Region region[]={
//	{0x00000000,0x2000	},	// RAM
	{0x00000000,0x8000	},	// RAM
//	{0x1D000000,0x8000	},	// FLASH
	{0x1D000000,0x20000	},	// FLASH
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

//static	
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
 *	PIC32MX デバイスに処理コマンドを送って、必要なら結果を受け取る.
 *********************************************************************
 *	cmdBuf   *cmd : 処理コマンド
 *	uchar    *buf : 結果を受け取る領域.
 *  int reply_len : 結果の必要サイズ. (0の場合は結果を受け取らない)
 *戻り値
 *		0	: 失敗
 *	   !0   : 成功
 */
int QueryPIC(cmdBuf *cmd)
{
	int rc = usbWrite((uchar*)cmd,PACKET_SIZE,0,0);
	if(	rc == 0) return 1;
	return 0;
}

int QueryPIC2(cmdBuf *cmd,uchar *result)
{
	int rc = usbWrite((uchar*)cmd,PACKET_SIZE,result,PACKET_SIZE);
	if(	rc == 0) return 1;
	return 0;
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
	cmdBuf cmd;

	int rsize = check_region(adr,size);
	if( rsize <= 0 ) {return 0;}

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = CMD_PEEK;
	cmd.subcmd= width;
	cmd.size  = rsize;
	cmd.adr   = adr|0x80000000;		// MSBを立てる.
	if( QueryPIC2(&cmd,reply) == 0) {
		return 0;	//失敗.
	}
	memcpy(buf, reply, rsize);
	return size;
}

/*********************************************************************
 *
 *********************************************************************
 */
int UsbBootTarget(int resetpc,int bootflag,int vector)
{
	cmdBuf cmd;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_JMP;
	cmd.adr   = resetpc;		// 実行開始番地.
	cmd.size  = bootflag;		// 1ならUSBバスをリセットする.

	if( QueryPIC(&cmd) == 0) return 0;	//失敗.

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


	if( QueryPIC2(&cmd,buf) == 0) return 0;	//失敗.

	memdump_print(buf,16,0);		//結果印字.
	return 1;
}

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

	if( QueryPIC(&cmd) == 0) return 0;	//失敗.

//	hidGetChars();
//	printf("\n* Terminate.\n");

	return 1;
}

/*********************************************************************
 *
 *********************************************************************
 */
int UsbReadString(uchar *result)
{
	cmdBuf cmd;
	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd = HIDASP_GET_STRING;

	result[0]=0;
	if( QueryPIC2(&cmd,result) == 0) return 0;	//失敗.
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

	if( QueryPIC(&cmd) == 0) return 0;	//失敗.
	return 1;
}

/*********************************************************************
 *	ターゲット側のメモリー内容(1バイト)を書き換える.
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
 *	int          data0 :書き込みデータ.      (OR)
 *	int          data1 :書き込みビットマスク.(AND)
 *注意:
 *	ファーム側の書き込みアルゴリズムは以下のようになっているので注意.

	if(data1) {	//マスク付の書き込み.
		*adr = (*adr & data1) | data0;
	}else{			//べた書き込み.
		*adr = data0;
	}

 */
int	pokemem(int adr,int arena,int data0,int data1)
{
//	int buf[64/4];
	int size=1;
	if(arena==MEM_HALF) 	size = 2;
	if(arena==MEM_WORD) 	size = 4;

    cmdBuf cmd;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = CMD_POKE ;	//| arena;
	cmd.subcmd= arena;
	cmd.size  = size;	//1;
	cmd.adr   = adr | 0x80000000;		// MSB On.

	if(arena==MEM_HALF) 	cmd.size = 2;
	if(arena==MEM_WORD) 	cmd.size = 4;

	cmd.data[0] = data0;
	cmd.data[1] = data0>>8;
	cmd.data[2] = data0>>16;
	cmd.data[3] = data0>>24;
	return QueryPIC(&cmd);
}

void hid_read_mode(int mode,int adr)
{
	cmdBuf cmd;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_SET_MODE;
	cmd.size  = mode;
	cmd.adr   = adr;
	QueryPIC(&cmd);

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

	memset(&cmd,i,sizeof(cmdBuf));
	cmd.cmd   = CMD_PING ;
	QueryPIC2(&cmd,buf);

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
	nBytes  = PACKET_SIZE ;				//登りパケットのみ計測.
#else
	nBytes  = PACKET_SIZE + PACKET_SIZE ;	//現在固定.
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
#if	0
/*********************************************************************
 *	転送速度ベンチマーク
 *********************************************************************
 *	int cnt  : PINGを打つ回数.
 *	int psize: PINGパケットの大きさ(現在はサポートされない)
 */
void UsbSampling(int cnt,int psize)
{
	int	result[1024/sizeof(int)];

 	cmdBuf cmd;
	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_SAMPLING ;
	cmd.subcmd= ADC_ANALOG ;
	cmd.size  = 1024;

	int rc = usbWrite( (uchar*)&cmd ,PACKET_SIZE,(uchar*)result,1024);
//	if(	rc == 0) return ;

	memdump_print(result,1024,0);
	(void)rc;
}
#endif
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
	int rsize,rc;
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
	(void)rc;
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
 */
int UsbPeek(int adr,int width)
{
	unsigned char buf[16];
	int size=4;
	int rc = UsbRead(adr,width,buf,size);
	if( rc != size) {
		return -1;
	}
	switch(width) {
	  default:
	  case MEM_BYTE:
		return buf[0];
	  case MEM_HALF:
		return buf[0] | (buf[1]<<8);
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
	if( pollcmd_implemented == 0 ) {
		return UsbSetPoll_slow(adr,0);
	}
	hid_read_mode(mode,adr);
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
int UsbPoll(uchar *buf)
{
	int rc;

	if( pollcmd_implemented == 0 ) {
		rc = UsbPoll_slow();
		buf[0]=0;		// poll_mode
		buf[1]=0xc0;	// poll_mode
		buf[2]=1;		// samples
		buf[3]=rc;
		return rc;
	}
	return 0;
}


int UsbAnalogPoll(int mode,uchar *abuf)
{
	return 0;
}


/*********************************************************************
 *
 *********************************************************************
 */
int 	UsbEraseTargetROM(int adr,int size)
{
	uchar reply[256];
	cmdBuf cmd;

	cmd.cmd   = HIDASP_PAGE_ERASE;
	cmd.size  = size;
	cmd.adr   = adr;

	if( QueryPIC2(&cmd,reply) == 0) return 0;	//失敗.
	return size;
}

/*********************************************************************
 *	ターゲット側のFlash内容(32バイトまで)を書き換える.
 *********************************************************************
 *	int            adr :ターゲット側のメモリーアドレス
 *	int          arena :ターゲット側のメモリー種別(現在未使用)
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

	if( QueryPIC(&cmd) == 0) return 0;	//失敗.

	if(size > 48) {
		while(size>0) {
			int len = size;
			if( len >= 60) { len = 60; }
			RS_putdata(	buf , len );

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
#if	0
	cmdBuf cmd;
	cmd.cmd   = HIDASP_FLASH_LOCK;
	cmd.size  = lockf;
	cmd.adr   = 0;

	if( QueryPIC(&cmd) == 0) return 0;	//失敗.
#endif
	return 0;
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
int	usbPoke(int adr,int arena,int data,int mask)
{
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

#define	BUFF_SIZE		256
#define DEBUG_PKTDUMP  	0		// パケットをダンプする.
//#define	BAUDRATE		115200
#define	BAUDRATE		500000


static	int dev_id        = 0;	// ターゲットID: 0x25 もしくは 0x14 だけを許容.
static	int dev_version   = 0;	// ターゲットバージョン hh.ll
static	int dev_bootloader= 0;	// ターゲットにbootloader機能がある.

//static	int have_isp_cmd = 0;	// ISP制御の有無.

static	int	flash_size     = 0;	// FLASH_ROMの最終アドレス+1
static	int	flash_end_addr = 0;	// FLASH_ROMの使用済み最終アドレス(4の倍数切捨)
static	int	flash_err_count= 0;	// FLASH_ROMの書き込みエラー.

char	reboot_done = 0;

/*
 *	HIDデバイスに HID Report を送信する.
 *	送信バッファの先頭の1バイトにReportID を入れる処理は
 *	この関数内で行うので、先頭1バイトを予約しておくこと.
 *
 *	id と Length の組はデバイス側で定義されたものでなければならない.
 *
 *	戻り値はHidD_SetFeatureの戻り値( 0 = 失敗 )
 *
 */
static int hidWrite(uchar *buf, int cnt)
{
	buf[0]=0x01;

#if	DEBUG_PKTDUMP
	memdump("WR", buf, cnt);
#endif

	RS_putdata(	buf , cnt );
	return 1;
}

static int hidRead(uchar *buf, int cnt)
{
#if	DEBUG_PKTDUMP
	printf("RD: size=%d\n",cnt);
#endif

#if	0
	int n=0;
	while(1) {
		int c = RS_getc();
		if(c!= (-9)) {
			*buf++=c;
			printf("read%d> %02x\n",n,c);n++;
			if(n>=cnt) return 1;
		}
	}
#endif

	int rc;
	while(1) {
		if( RS_checkdata() ) {
			rc = RS_getdata(buf,cnt);
			if(rc>0) break;
		}
		Sleep(1);
	}
#if	DEBUG_PKTDUMP
	memdump("RD", buf, cnt);
#endif

//	return rc;
	return 1;
}

int hidGetChars(void)
{
	int rc,ch;
	uchar buf[4];

	while(1) {
		Sleep(10);
		while( RS_checkdata() ) {
			rc = RS_getdata(buf,1);
			if(rc>0) {
				ch = buf[0];
				if(ch==0) {
					return 1;
				}else {
					putchar(ch);
				}
			}
		}
	}
}

int	hidReadPoll(uchar *buf, int Length, int id)
{
	return hidRead(buf, Length);
}

int	hidReadBuffer(uchar *buf, int len)
{
	return	hidRead(buf,len);
}
int hidCommand(int cmd,int arg1,int arg2,int arg3)
{
	unsigned char buf[BUFF_SIZE];

	memset(buf , 0, sizeof(buf) );

	buf[1] = cmd;
	buf[2] = arg1;
	buf[3] = arg2;
	buf[4] = arg3;

	return hidWrite(buf, PACKET_SIZE+1);
}

int	flash_get_status()
{
	uchar rd_data[BUFF_SIZE];
 	cmdBuf cmd;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_TEST;
	usbWrite( (uchar*)&cmd ,PACKET_SIZE, rd_data ,PACKET_SIZE);
	{
		int *fl_size;
		char *sBoot="(Application)";

		dev_id         = rd_data[1];
		dev_version    = rd_data[2] | (rd_data[3]<<8) ;
		dev_bootloader = rd_data[4];

		fl_size = (int*)&rd_data[8];
		flash_size     = fl_size[0];
		flash_end_addr = fl_size[1];
		flash_err_count= fl_size[2];

		if(dev_bootloader==1) {sBoot="(Bootloader)";}
		fprintf(stderr, "TARGET DEV_ID=%x VER=%d.%d%s FLASH=%x,%x\n",dev_id
				,dev_version>>8,dev_version & 0xff,sBoot
				,flash_end_addr,flash_size
				);
	}
	return flash_err_count;
}

//----------------------------------------------------------------------------
int	UsbGetDevCaps(int *pflash_size,int *pflash_used)
{
	if(pflash_size)	{*pflash_size = flash_size;}
	if(pflash_used)	{*pflash_used = flash_end_addr;}

	return dev_bootloader;
}



void memdump(char *msg, uchar *buf, int len)
{
	int j;
	fprintf(stderr, "%s", msg);
	for (j = 0; j < len; j++) {
		fprintf(stderr, " %02x", buf[j] & 0xff);
		if((j & 0xf)==  7)
			fprintf(stderr, " ");

		if((j & 0xf)== 15)
			fprintf(stderr, "\n +");
	}
	fprintf(stderr, "\n");
}

/*********************************************************************
 *	初期化
 *********************************************************************
 */
int UartInit(int com_n)
{
	if( com_n ==0) {
		com_n = RS_printinfo(BAUDRATE);
	}

	if( com_n==0) return -1;		// NG
	printf("open COM%d:\n",com_n);

#if	0
	int i;
	for(i=0;i<160;i++) {
		RS_putc(0xa5);
	}
#endif

	RS_putc(0xaa);
	RS_putc(0x55);

	return flash_get_status();
}

/*********************************************************************
 *	終了
 *********************************************************************
 */
int UartExit(void)
{
	if( reboot_done == 0 ) {
		unsigned char buf[BUFF_SIZE];
		memset(buf , 0xff, sizeof(buf) );
		RS_putdata(	buf , PACKET_SIZE+1 );
		Sleep(1);
	}
	RS_exit();
	return 0;
}


/*********************************************************************
 *
 *********************************************************************
 */
