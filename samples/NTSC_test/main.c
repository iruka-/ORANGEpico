/********************************************************************
    NTSC信号出力 / UART1 test
********************************************************************/

#include <plib.h>

#include "config.h"
#include "spi2.h"
#include "serial1.h"
#include "graph.h"

#ifndef	BAUDRATE
#define	BAUDRATE	500000
#endif

/** PRIVATE PROTOTYPES *********************************************/
static void InitializeSystem(void);

void NTSC_init(void);

#define	_MIPS32 __attribute__((nomips16,noinline))

_MIPS32 void ei()
{
//	crt0.S の初期化にて、すでにMultiVectordにはなっている。
	INTCONSET=0x1000;		// EI PORT
//	INTEnableSystemMultiVectoredInt();	この関数内に INTCONSET設定が含まれる.
	INTEnableInterrupts();	// ei命令.
}
// All Analog Pins as Digital IOs
static	void IOsetDigital()
{
	DDPCONbits.JTAGEN=0;		// check : already in system.c
	ANSELA = 0;
	ANSELB = 0;
//	ANSELC = 0;
//	TRISBCLR=0x8000;	//	pinmode(13, OUTPUT);
}

/********************************************************************
 *		Arduino風:	初期化処理
 ********************************************************************
 */
static	inline void setup()
{
#ifdef	USE_INTERNAL_FRC_OSC	// RC 発振の場合は余分に初期化が必要.
	InitializeFRC();
#endif

	IOsetDigital();				// 全てデジタル ピンとして初期化.

	mInitAllLEDs();				// LED初期化.
	mInitAllSwitches();			// Switch読み取り初期化.

	NTSC_init();				// NTSC 表示初期化.

	// UART1 初期化 (NTSC_initのあと)
	SerialConfigure(UART1, UART_ENABLE,	UART_RX_TX_ENABLED,	BAUDRATE);
	// 割り込み許可.
	ei();
	// グラフィックテスト.
	gr_test();
}


/********************************************************************
 *		システム: 文字出力
 ********************************************************************
 */
static void Putch(int ch)
{
	// 文字: UART1 に出力
	Serial1WriteChar(ch);
	if(ch == '\r') {	// CR なら LF も追加
		Serial1WriteChar('\n');
	}

	// 文字: TV画面 に出力
	if(ch == '\r') {	// CR なら LF に変更
		ch = '\n';
	}
	gr_putch(ch);
}
/********************************************************************
 *		Arduino風:	繰り返し処理
 ********************************************************************
 */
static	inline	void loop(void)
{
	// UART1: から１文字入力  (完了待ちあり)
	int ch = Serial1GetKey();
	// UART1: にエコーバックする.
	Putch(ch);
}

/********************************************************************
 *		main()
 ********************************************************************
 */
int main(void)
{
	setup();		// 初期化:ユーザー処理
	while (1) {
		loop();		// ループ:ユーザー処理
	}
}


