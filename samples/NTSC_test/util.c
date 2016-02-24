/********************************************************************
 *	ユーティリティ
 ********************************************************************
 */

#include <plib.h>

#include "config.h"
#include "spi2.h"
#include "serial1.h"

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

