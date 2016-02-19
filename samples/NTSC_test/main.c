/********************************************************************
 *	PIC32MX220F032B 用 UARTサンプル Main:
 ********************************************************************
 *

- Pinguinoコンパイル環境のみを使用.(MicroChip gccヘッダー非依存)

 ********************************************************************
 */
#include <p32xxxx.h>			// always in first place to avoid conflict with const.h ON
#include <typedef.h>			// Pinguino's types definitions
#include <const.h>				// Pinguino's constants definitions
#include <macro.h>				// Pinguino's macros definitions

/********************************************************************
 *
 ********************************************************************
 */
#define	__SERIAL__
#define __SYSTEM_C


#include <system.c>				// PIC32 System Core Functions
//#include <io.c>					// Pinguino Boards Peripheral Remappage and IOs configurations

#include "serial1.h"

#ifndef	BAUDRATE
#define	BAUDRATE	500000
#endif

#define mInitBootSwitch()   TRISBbits.TRISB7=1;
#define mGetSWRFlag()       RCONbits.SWR
#define Boot_SW             PORTBbits.RB7

/*	----------------------------------------------------------------------------
	SystemUnlock() perform a system unlock sequence
	--------------------------------------------------------------------------*/

void system_unlock()
{
	SYSKEY = 0;				// ensure OSCCON is locked
	SYSKEY = 0xAA996655;	// Write Key1 to SYSKEY
	SYSKEY = 0x556699AA;	// Write Key2 to SYSKEY
}

/*	----------------------------------------------------------------------------
	SystemLock() relock OSCCON by relocking the SYSKEY
	--------------------------------------------------------------------------*/

void system_lock()
{
	SYSKEY = 0x12345678;	// Write any value other than Key1 or Key2
}

// All Analog Pins as Digital IOs
void io_setDigital()
{
	DDPCONbits.JTAGEN=0;		// check : already in system.c
	ANSELA = 0;
	ANSELB = 0;
	#if defined(__32MX220F032D__)
		ANSELC = 0;
	#endif
}


/********************************************************************
 *		PIC32 Peripheral Remappage
 ********************************************************************
 */
void io_setRemap()
{
	system_unlock();
	CFGCONbits.IOLOCK=0;			// unlock configuration
	CFGCONbits.PMDLOCK=0;
	{
		U1RXRbits.U1RXR=2;			// Define U1RX as RA4 ( UEXT SERIAL )
		RPB4Rbits.RPB4R=1;			// Define U1TX as RB4 ( UEXT SERIAL )
	}
	CFGCONbits.IOLOCK=1;			// relock configuration
	CFGCONbits.PMDLOCK=1;
	system_lock();
}
/********************************************************************
 *		Arduino風:	初期化処理
 ********************************************************************
 */
static	inline void setup()
{
	TRISBCLR=0x8000;	//	pinmode(13, OUTPUT);
	SerialConfigure(UART1, UART_ENABLE,	UART_RX_TX_ENABLED,	BAUDRATE);

	Serial1WriteString("\r\n\r\nHello,World.\r\n");
}

/********************************************************************
 *		Arduino風:	繰り返し処理
 ********************************************************************
 */
static	inline	void loop(void)
{
	int ch = Serial1GetKey();

	Serial1WriteChar(ch);

	if(ch == '\r') {
		Serial1WriteChar('\n');
	}
}

/********************************************************************
 *		メイン
 ********************************************************************
 */

int main()
{
	mInitBootSwitch();

	DDPCONbits.JTAGEN=0;	// PORTA is used as digital instead of JTAG
	io_setDigital();		// Analog から Digital I/Oに切り替えます.
	io_setRemap();          // RA4/RB4 をUARTに割り当てます.
	// 初期化:ユーザー処理
	setup();
	// ループ:ユーザー処理
	while (1) {
		loop();
	}

	return(0);
}

/********************************************************************
 *
 ********************************************************************
 */
