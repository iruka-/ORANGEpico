/********************************************************************
 *	簡易モニタ
 ********************************************************************
 */
#include <string.h>

#include "NVMem.h"
#include "serial1.h"

#include "hidcmd.h"
#include "monit.h"



/********************************************************************
 *	スイッチ
 ********************************************************************
 */

/********************************************************************
 *	定義
 ********************************************************************
 */
#define	DEVID	0x32

#define	VER_H	1
#define	VER_L	1

#if	_ROMADRS || _APPLICATION
#define	DEV_LOADER	0
#else
#define	DEV_LOADER	1
#endif

#define	MONIT_SOF		0x01
#define	REPLY_REPORT	0xaa

//
// ===        STM32以外でのFlash Programming 実装.
//
#if	0	//ndef	SUBMDL_STM32F103

UINT	NVMemOperation(UINT nvmop);
UINT	NVMemErasePage(void* address);
UINT	NVMemWriteWord(void* address, UINT data);
UINT	NVMemWriteRow(void* address, void* data);
UINT	NVMemClearError(void);

#endif

void	reset_device(int adrs);

/********************************************************************
 *	データ
 ********************************************************************
 */

//	AVR書き込み用コンテクスト.
uchar  page_mode;
uchar  page_addr;
uchar  page_addr_h;

//	データ引取りモードの設定.
uchar   poll_mode;	// 00=無設定  0xa0=アナログサンプル  0xc0=デジタルサンプル
// 0xc9=runコマンドターミネート
uchar  *poll_addr;	//

//	コマンドの返信が必要なら1をセットする.
uchar	ToPcRdy;

uchar	puts_mode;

uchar 	puts_buf[64];
uchar 	puts_ptr;

uchar	poll_wptr;
uchar	poll_rptr;
uchar	poll_buf[256];


#define	Firm_top	0x1d000000

// 							   BMXPFMSZ:I/O(R):  FlashROMのサイズ
#define	Firm_end	(Firm_top + BMXPFMSZ)
//#define	Firm_end	0x1d008000

#define	FLASH_END_ADR	BMXPFMSZ		//実際にはサイズ.
//#define	FLASH_END_ADR	0x8000

int		total_errs=0;
static  int test_adr0=0;

static  volatile char s_linecoding = 0;		//	baudrateが変更された.


Packet PacketFromPC;			//入力パケット 64byte
Packet PacketToPC;				//出力パケット 64byte

#define	Cmd0	PacketFromPC.cmd

/********************************************************************
 *
 ********************************************************************
 */
/*
int	align_size(int adr,int align)
{
	return 	(adr + align - 1) & (-align) ;	// align byte境界に合わせる.
}
*/
/********************************************************************
 *
 ********************************************************************
 */
static	inline	int	UARTputpacket(uchar *buf,int len)
{
	UARTwrite(buf,len);
	return len;
}

/********************************************************************
 *
 ********************************************************************
 */
int check_flash_adrs(int adrs)
{
	if( (adrs >= Firm_top)&&(adrs < Firm_end) )	return 1;	// VALID!
	return 0;	// INVALID.
}
/********************************************************************
 *	ISPモジュール インクルード.
 ********************************************************************
 */
//#include "usi_pic18s.h"

static void isp_command(uchar *data)
{
	uchar i;
	for (i=0; i<4; i++) {
		PacketToPC.raw[i]=usi_trans(data[i]);
	}
}

/********************************************************************
 *	FLASH領域(9d00_0000,9d00_0000 + ea)内の最終書き込みポイントを探す.
 ********************************************************************
 */
int	search_flash_end(int ea)
{
	int *p  = (int *) (0x9d000000+ea);
	int cnt = ea >> 2;
	int i;
	for(i=0;i<cnt;i++) {
		p--;
		if(*p != (-1)) break;	// 0xffff_ffff ?
	}
	return (int)p;
}

/********************************************************************
 *	接続テストの返答
 ********************************************************************
 */
void cmd_echo(void)
{
	int *fl_stat = &PacketToPC.rawint[2];
	PacketToPC.raw[1]=DEVID;				// DeviceID
	PacketToPC.raw[2]=VER_L;				// version.L
	PacketToPC.raw[3]=VER_H;				// version.H
	PacketToPC.raw[4]=DEV_LOADER;			// bootloader
	PacketToPC.raw[5]=PacketFromPC.raw[1];	// ECHOBACK TEST

	fl_stat[0] = (FLASH_END_ADR);
	fl_stat[1] = (search_flash_end(FLASH_END_ADR));
	fl_stat[2] = (total_errs);

	total_errs=0;
	ToPcRdy = 1;
}

/********************************************************************
 *	メモリー読み出し（32byte以上一括）
 ********************************************************************
 */
static	inline	void cmd_peek_large_block(int adrs,int size)
{
	uchar *p = (uchar *)adrs;
	UARTputpacket(p,size);
}

/********************************************************************
 *	メモリー読み出し
 ********************************************************************
 */
void cmd_peek(void)
{
	Packet *t = &PacketFromPC;
	int  size = (t->size);
	int  adrs = (t->adrs);
	int    i,subcmd;
	uchar  *pb;
	ushort *ph;
	uint   *pw;

	uchar  *tb;
	ushort *th;
	uint   *tw;

	subcmd = t->subcmd;

	if(size > PACKET_SIZE) {// 60byte以上の連続読み出しは別処理.
		cmd_peek_large_block(adrs,size);
		return;
	}

	switch(subcmd) {         // 読み出し幅指定に応じた処理に
	default:
	case MEM_BYTE:
		pb = (uchar *)adrs;
		tb = (uchar *)PacketToPC.raw;
		for(i=0; i<size; i++) {
			*tb++ = *pb++;
		}
		break;
	case MEM_HALF:
		ph = (ushort *)adrs;
		th = (ushort *)PacketToPC.rawint;
		for(i=0; i<size; i+=2) {
			*th++ = *ph++;
		}
		break;
	case MEM_WORD:
		pw = (uint *)adrs;
		tw = (uint *)PacketToPC.rawint;
		for(i=0; i<size; i+=4) {
			*tw++ = *pw++;
		}
		break;
	}

	UARTputpacket(PacketToPC.raw,PACKET_SIZE);

//	ToPcRdy = 1;
}
/********************************************************************
 *	メモリー書き込み
 ********************************************************************
 */
void cmd_poke(void)
{
	Packet *t = &PacketFromPC;
	int  size = (t->size);
	int  adrs = (t->adrs);
	int  i,subcmd;

	uchar  *pb;
	ushort *ph;
	uint   *pw;

	uchar  *sb;
	ushort *sh;
	uint   *sw;

	subcmd = t->subcmd;

	switch(subcmd) {	// 書き込み幅指定に応じた処理に
	default:
	case MEM_BYTE:
		pb = (uchar *)adrs;
		sb = (uchar *)t->data;
		for(i=0; i<size; i++) {
			*pb++ = *sb++;
		}
		break;
	case MEM_HALF:
		ph = (ushort *)adrs;
		sh = (ushort *)t->data;
		for(i=0; i<size; i+=2) {
			*ph++ = *sh++;
		}
		break;
	case MEM_WORD:
		pw = (uint *)adrs;
		sw = (uint *)t->data;
		for(i=0; i<size; i+=4) {
			*pw++ = *sw++;
		}
		break;
	}
}

/********************************************************************
 *	FLASHのページ単位消去.	(引数はPageの先頭adrs) 消去サイズは暗黙.
 ********************************************************************
 */
void cmd_page_erase()
{
	int adrs = PacketFromPC.adrs  & 0x1fffffff;
	if( check_flash_adrs(adrs)) {
		NVMemErasePage((void*) adrs);
	}
	ToPcRdy = 1;		//同期のため、ダミーデータを返す.
}

static	inline	void FLASH_Lock()
{
	NVMCONbits.WREN = 0;		//clear WREN bit 
}
static	inline	void FLASH_Unlock()
{
	NVMemClearError();
}
/********************************************************************
 *	FLASHの書き込みをロック/アンロックする.(引数sizeがゼロならアンロック)
 ********************************************************************
 */
static	inline	void cmd_flash_lock()
{
	if(PacketFromPC.size) {
		FLASH_Lock();
	} else {
		/* Unlock the internal flash */
		FLASH_Unlock();
	}
}

/********************************************************************
 *	FLASHのページ単位書き込み.	(引数は書き込みadrsとsize)
 ********************************************************************
  STM32では、word単位で書き込みが可能. (flush動作不要)

  他のマイコン(LPCなど)では、
  	実際にはRAM上のページバッファにword単位でためておいて、
  	最後にsize=0を指定してページバッファを一発書き込みする実装になることもある.
 

 */
void cmd_page_write()
{
	int  *p = PacketFromPC.data;
	int   i;
	int   size = (PacketFromPC.size);
	int	   adr = (PacketFromPC.adrs) & 0x1fffffff;

	// ここでbyte数をword数に変換している.
	size = (size+3) >> 2;		// size は4byte単位に限る.

//	if(size==0) {				// size が 0 のときは書き込みflush動作をする.
//		FLASH_ProgramPage(adr);	// これはSTM32に限り不要(ダミー関数)
//		return ;
//	}

	for(i=0; i<size; i++) {
		if( check_flash_adrs(adr) ) {
			NVMemWriteWord((void*) adr, *p);
//			FLASH_ProgramWord(adr, *p);	// STM32では直接FLUSHに書く.
		}								// 他のマイコンでは、ページバッファに記録.
		p++;
		adr+=4;
	}
}

/********************************************************************
 *	番地指定の実行
 ********************************************************************
 */
static	inline	void call_func(int adrs)
{
	void (*func)(void);
	func = (void (*)()) adrs;

	func();
}



/********************************************************************
 *	番地指定の実行
 ********************************************************************
 */
static	inline	void cmd_exec()
{
	int bootflag = (PacketFromPC.size);
	int	    adrs = (PacketFromPC.adrs);
	if(	bootflag ) {
		reset_device(adrs);
	}
	call_func(adrs);
}

#if	APPLICATION_MODE
/********************************************************************
 *	puts 文字列をホストに返送する.
 ********************************************************************
 */
void cmd_get_string(void)
{
	PacketToPC.raw[0] =  puts_mode;	//'p';		/* コマンド実行完了をHOSTに知らせる. */
	PacketToPC.raw[1] =  puts_ptr;	// 文字数.
	memcpy( (void*)&PacketToPC.raw[2] , (void*)puts_buf , puts_ptr & 0x3f);	//文字列.
	puts_ptr = 0;
	ToPcRdy = 1;
}
/********************************************************************
 *	ユーザー定義関数の実行.
 ********************************************************************
 */
void cmd_user_cmd(void)
{
	puts_ptr = 0;
	puts_mode = 'p';
	user_cmd(BSWAP32(PacketFromPC.adrs));
	puts_mode = 'q';
}
#endif

/********************************************************************
 *	1mS単位の遅延を行う.
 ********************************************************************
 */
void cmd_wait_msec(void)
{
	uchar ms = PacketFromPC.raw[1];	// '-dN'
	if(ms) {
		wait_ms(ms);
	}
}

/********************************************************************
 *	モニタコマンド受信と実行.
 ********************************************************************
 */
static	inline void ProcessIO(void)
{
	// 返答パケットが空であること、かつ、
	// 処理対象の受信データがある.
	
	//受信データがあれば、受信データを受け取る.
	PacketToPC.raw[0]=Cmd0;		// CMD ECHOBACK
	switch(Cmd0) {
	 case HIDASP_PEEK: 		{cmd_peek();break;}			// メモリー読み出し.
	 case HIDASP_POKE: 		{cmd_poke();break;}			// メモリー書き込み.
	 case HIDASP_JMP: 		{cmd_exec();break;}			// 再起動.
	 case HIDASP_PAGE_ERASE:{cmd_page_erase();break;}	//Flash消去.
	 case HIDASP_PAGE_WRITE:{cmd_page_write();break;}	//Flash書込.
	 case HIDASP_FLASH_LOCK:{cmd_flash_lock();break;}	//FlashLock.
	 case HIDASP_TEST: 		{cmd_echo();break;}			// 接続テスト.

#if	APPLICATION_MODE
	 case HIDASP_GET_STRING:{cmd_get_string();break;}
	 case HIDASP_USER_CMD:  {cmd_user_cmd();break;}
#endif
	 default: break;
	}
	// 必要なら、返答パケットをインタラプト転送(EP1)でホストPCに返却する.
	if( ToPcRdy ) {
		UARTputpacket(PacketToPC.raw,PACKET_SIZE);
		ToPcRdy = 0;
	}
}
/********************************************************************
 *	メイン処理
 ********************************************************************
 */
void UARTmonit(void)
{
	int rc=0;
	while(1){
		// 先頭byteが0x01でなければ、エラー.
		int ch = Serial1GetKey();
		if(	ch != 0x01 ) return;		// Errorしたら終了.

		do {	//	PACKET受信完了を待つ
			rc = UARTgetpacket(PacketFromPC.raw,PACKET_SIZE);
		}while(rc==0);

		// PACKETを処理.
		ProcessIO();
	}
}

/********************************************************************
 *
 ********************************************************************
 */
