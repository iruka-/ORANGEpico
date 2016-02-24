/********************************************************************
 FileName:      main.c
********************************************************************/

#include <plib.h>

#include "HardwareProfile.h"
#include "spi2.h"
#include "serial1.h"

#ifndef	BAUDRATE
#define	BAUDRATE	500000
#endif

/** PRIVATE PROTOTYPES *********************************************/
static void InitializeSystem(void);

void UserInit(void);
void init_vga(void);

#ifdef	USE_INTERNAL_FRC_OSC	// RC OSC
/********************************************************************
 * Function:	Clock Select to FRC
 *******************************************************************/
void InitializeFRC()
{
	mSysUnlockOpLock( 
		{PPSOutput(3, RPA4, REFCLKO);}
	);

	OSCREFConfig(OSC_REFOCON_FRC,
	OSC_REFOCON_OE | OSC_REFOCON_ON, 1);

	mSysUnlockOpLock(
		{OSCCONbits.NOSC = 3; //ECPLL
		 OSCCONbits.OSWEN = 1;}
	);

	while(OSCCONbits.COSC != 3);

	SYSTEMConfigPerformance(48000000);

}
#endif

/********************************************************************
 *		
 ********************************************************************
 */
void led_on()
{
	mLED_1_On();
}

void led_off()
{
	mLED_1_Off();
}

void led_flip()
{
	mLED_1_Toggle();
}

/********************************************************************
 *		
 ********************************************************************
 */
void led_test()
{
	mInitAllLEDs();
	while(1) {
		led_flip();
		wait_ms(500);
	}
}

/********************************************************************
 *		
 ********************************************************************
 */
#define	CNTMAX		400000

void led_blink()
{
	static int cnt=0;
	cnt++;
	if(	cnt >= CNTMAX) {
		cnt=0;
		led_flip();
	}
}


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

void UserInit(void)
{
	IOsetDigital();
	mInitAllLEDs();
	mInitAllSwitches();
	SerialConfigure(UART1, UART_ENABLE,	UART_RX_TX_ENABLED,	BAUDRATE);
}

/********************************************************************
 *		Arduino風:	初期化処理
 ********************************************************************
 */
static	inline void setup()
{
#ifdef	USE_INTERNAL_FRC_OSC	// RC OSC
	InitializeFRC();
#endif

	UserInit();			//Application related initialization.  See user.c
	init_vga();
	ei();
	gr_test();
}

/********************************************************************
 *		Arduino風:	繰り返し処理
 ********************************************************************
 */
static	inline	void loop(void)
{
//	int ch = Serial1GetKey();
	int ch = '.';

	Serial1WriteChar(ch);

	if(ch == '\r') {
		Serial1WriteChar('\n');
	}
	wait_ms(100);
	led_flip();
}

/********************************************************************
 *		main()
 ********************************************************************
 */
int _MIPS32 main(void)
{
	// 初期化:ユーザー処理
	setup();
	// ループ:ユーザー処理
	while (1) {
		loop();
	}
	led_test();

}


