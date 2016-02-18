/********************************************************************
 *	PIC32MX220F032B 用 UARTブートローダー Main:
 ********************************************************************
 *

- Pinguinoコンパイル環境のみを使用.(MicroChip gccヘッダー非依存)
- バイナリーサイズ3kB以内. ( BOOT FLASH ROM のみ使用 )

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

#define	_BOOTROM_  __attribute__((section(".bootrom")))

#ifndef	BAUDRATE
#define	BAUDRATE	500000
#endif

#define mInitBootSwitch()   TRISBbits.TRISB7=1;
#define mGetSWRFlag()       RCONbits.SWR
#define Boot_SW             PORTBbits.RB7

#if	1
#define BootMemStart					0x9D000000
#else
#define BootMemStart					0x9D001000
#endif

unsigned int *SoftwareKey = NULL;

#if	0
#define	Boot_SW_POLARITY	1		// If not PRESSED , Then Run Sketch Program (UBW32 Compat)
#else
#define	Boot_SW_POLARITY	0		// If PRESSED , Then Run Sketch Program (Pinguino Useful)
#endif

//
//	start_addr に分岐する.
//
static inline
void call_ptr(int start_addr)
{
	void (*fptr)(void);
	fptr = (void (*)(void)) start_addr;
	fptr();
}


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

/*	----------------------------------------------------------------------------
	call from cmd_boot()	adrsは無視.
	--------------------------------------------------------------------------*/
void reset_device(int adrs)
{
	volatile int* p = &RSWRST;

	int j;

	IEC1bits.U1RXIE=0;			// U1RX disable.
	INTDisableInterrupts();		// DI

	*SoftwareKey = 0x12345678;	// Software Reset Signal:

//	U1CON = 0x0000;				//Disable USB module
//	for(j = 0; j < 0xFFFF; j++) {
//		Nop();
//	}

	system_unlock();

	RSWRSTSET=_RSWRST_SWRST_MASK;

	*p;	// RSWRST;

	while(1);

}

void io_setSpecial()
{
	#if defined(PIC32_PINGUINO) || defined(PIC32_PINGUINO_OTG)
	TRISDbits.TRISD9  = INPUT;		// because PORTB is shared with SDA on Olimex board
	TRISDbits.TRISD10 = INPUT;		// because PORTB is shared with SCL on Olimex board
	#endif	
	
	#if defined(PIC32_PINGUINO_220)
	//the followings are already declared in serial.c
	//TRISCbits.TRISC9 = OUTPUT;	// RC9 / U2TX output
	//TRISCbits.TRISC8 = INPUT;		// RC8 / U2RX input			
	#endif	
}

// All Analog Pins as Digital IOs
void io_setDigital()
{
	#if 1 //defined(__32MX220F032D__)||defined(__32MX250F128B__)||defined(__32MX220F032B__)
			DDPCONbits.JTAGEN=0;		// check : already in system.c
			ANSELA = 0;
			ANSELB = 0;
			#if defined(__32MX220F032D__)
				ANSELC = 0;
			#endif
	#else
	AD1PCFG = 0xFFFF;
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
//	SystemLock();
}
/********************************************************************
 *		Arduino風:	初期化処理
 ********************************************************************
 */
static	inline void setup()
{
	TRISBCLR=0x8000;	//	pinmode(13, OUTPUT);
	SerialConfigure(UART1, UART_ENABLE,	UART_RX_TX_ENABLED,	BAUDRATE);
}

/********************************************************************
 *		Arduino風:	繰り返し処理
 ********************************************************************

 Packet構造:

 [0xa5] ==> [0x55]   :合言葉エコー.

 [0xaa] [0x55]       :コマンドフェーズに移行する.

 [0x01] [コマンドパケット(64byte)] : USB HIDと同じパケットを受け取る.

 */
static	inline	void loop(void)
{
	int ch = Serial1GetKey();
	if( ch == 0xaa) {
		ch = Serial1GetKey();
		if(	ch == 0x55) {
			UARTmonit();
		}
	} else if(ch == 0xa5) {
		Serial1WriteChar(0x55);
	}
}

/********************************************************************
 *		メイン
 ********************************************************************
 */

int _BOOTROM_ main()
{
//	AN1388と同等の初期化:
//	int	pbClk = SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

	mInitBootSwitch();
	SoftwareKey = (unsigned int *)0xA0000000;

#ifdef	USE_INTERNAL_FRC_OSC	// RC OSC
	InitializeFRC();
#endif

#if	1
	//  起動直後の処理:
	//  -	BootSWが押されていたら、ユーザープログラムへ分岐する.
	//  -	BootSWが押されていなくても、
	//	-		ソフトウェアリセット起動、かつ、
	//	-		スケッチ実行キーが書き込まれていたらユーザープログラムへ分岐する.

	if(	(Boot_SW == Boot_SW_POLARITY) ||
	        (mGetSWRFlag() && (*SoftwareKey == 0x12345678) ) ) {
		*SoftwareKey = 0;
		//
		//ユーザープログラムを 9D000000,9D001000,9D002000,9D003000,9D004000,9D005000 の順に検索する.
		//
		int start_addr=BootMemStart;	// 9d001000
		for(; start_addr<=0x9d005000; start_addr+=0x1000) {
			if (*(int *)start_addr != 0xFFFFFFFF) {
				call_ptr(start_addr);
				while(1);
			}
		}
	}
#endif

	DDPCONbits.JTAGEN=0;		// PORTA is used as digital instead of JTAG
	io_setSpecial();
	io_setDigital();
	io_setRemap();
	//
	setup();
	while (1) {
		loop();
	}

	return(0);
}

/********************************************************************
 *
 ********************************************************************
 */
