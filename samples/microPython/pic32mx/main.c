/********************************************************************
 *	PIC32MX220F032B 用 UARTサンプル Main:
 ********************************************************************
 *

- Pinguinoコンパイル環境のみを使用.(MicroChip gccヘッダー非依存)

 ********************************************************************
 */
#include <p32xxxx.h>			// always in first place to avoid conflict with const.h ON
#include <stdio.h>
#include <malloc.h>

#define	__SERIAL__
#define __SYSTEM_C
#include <system.c>				// PIC32 System Core Functions

#include "utype.h"
#include "util.h"
#include "config.h"
#include "spi2.h"
#include "serial1.h"
#include "graph.h"
#include "ps2keyb.h"

unsigned int MIPS32 IntEnableInterrupts();

/********************************************************************
 *
 ********************************************************************
 */

#ifndef	BAUDRATE
#define	BAUDRATE	500000
#endif

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
	RPB13R=0x04;	// PB13=SDO2
	TRISBCLR=1<<13;	// PB13 output.
	TRISBSET=1<<8;	// PB8 input.
	TRISBSET=1<<9;	// PB9 input.

	CFGCONbits.IOLOCK=1;			// relock configuration
	CFGCONbits.PMDLOCK=1;
	system_lock();
}

_MIPS32 void Eint()
{
//	crt0.S の初期化にて、すでにMultiVectordにはなっている。
	IntEnableInterrupts();	// ei命令.
}

/********************************************************************
 *		Arduino風:	初期化処理
 ********************************************************************
 */
static	inline void setup()
{
	DDPCONbits.JTAGEN=0;	// PORTA is used as digital instead of JTAG
	io_setDigital();		// Analog から Digital I/Oに切り替えます.
	io_setRemap();          // RA4/RB4 をUARTに割り当てます.
    mInitAllLEDs();
	kbd_init();				// : 初期化
	NTSC_init();			// NTSC 表示初期化.
	// UART1 初期化 (NTSC_initのあと)
	SerialConfigure(UART1, UART_ENABLE,	UART_RX_TX_ENABLED,	BAUDRATE);
	Eint();					// 割り込み許可.
	gr_test();				// グラフィックテスト.
}



#if 1
void  test_func()
{
   char *p = malloc(100);
   printf("%x\n",(int)p);
}

#define	EXTERN	extern char
EXTERN	_heap	;/* end address for the .bss section.     */
EXTERN	_splim	;/* end address for the .bss section.     */
   
char *sbrk(int size);


void  test_func1()
{
   char buf[256];
   sprintf(buf,"%x\n",(int) &_heap);
   Serial1WriteString(buf);
   sprintf(buf,"%x\n",(int) &_splim );
   Serial1WriteString(buf);
}

extern int 	g_Vcount;
extern int 	g_Timer3;
void  test_func2()
{
   char buf[256];
   sprintf(buf,"v=%d %d\n",g_Vcount,g_Timer3);
   Serial1WriteString(buf);
}
#endif
int main_python(int argc, char **argv);


int  PYTHON_main(int argc, char **argv);
void BASIC_main( int argc,	char *argv[] );

/********************************************************************
 *		Arduino風:	繰り返し処理
 ********************************************************************
 */
static	inline	void loop(void)
{
 	if(Serial1Available()) {
		int ch = Serial1GetKey();

		Serial1WriteChar(ch);
		if(ch == '!') {
		    test_func1();
		}
		if(ch == '@') {
		    test_func();
		}
		if(ch == '#') {
	    	test_func2();
		}
		if(ch == '$') {
			PYTHON_main( 0,NULL );
   		}
    	if(ch == '\r') {
			Serial1WriteChar('\n');
		}
	}
	int key = kbd_getchar();
	if( key ) {
#if	1
		PYTHON_main( 0,NULL );
#else
		Serial1WriteChar(key);
		gr_putch(key);
//		printf("%02x\n",key);
    	if(key == '\r') {
			Serial1WriteChar('\n');
			gr_putch('\n');
		}
#endif
	}
}

/********************************************************************
 *		メイン
 ********************************************************************
 */

int main()
{
	// 初期化:ユーザー処理
	setup();
	// ループ:ユーザー処理
	while (1) {
		loop();
	}

	return(0);
}

/********************************************************************
 *  Dummy Interrupt Handler
 ********************************************************************
 */

void I2C1Interrupt(void)	{  }
void I2C2Interrupt(void)	{  }
void RTCCInterrupt(void)	{  }
void SPI1Interrupt(void)	{  }
void SPI2Interrupt(void)	{  }
void Serial2Interrupt(void)	{  }
//void Timer1Interrupt(void)	{  }
//void Timer2Interrupt(void)	{  }
//void Timer3Interrupt(void)	{  }
void Timer4Interrupt(void)	{  }
void Timer5Interrupt(void)	{  }
void USBInterrupt(void)		{  }

