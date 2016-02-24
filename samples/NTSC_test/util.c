#include <p32xxxx.h>			// always in first place to avoid conflict with const.h ON
#include <stdlib.h>			// always in first place to avoid conflict with const.h ON
/********************************************************************
 *	ユーティリティ
 ********************************************************************
 */


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

