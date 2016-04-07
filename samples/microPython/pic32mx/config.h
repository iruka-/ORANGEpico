/*
 *  ボード設定:
 */

#ifndef _CONFIG_H_
#define	_CONFIG_H_

// この #define は、config.cでFuse設定の切り替えに使用されているので注意.
#define	CPU_CLOCK_48MHz 1 		// 48MHz動作.
#define	USE_INTERNAL_FRC_OSC	// RC OSC


#ifdef	CPU_CLOCK_48MHz
// CPUとペリフェラルの動作周波数
#define SYS_FREQ (48000000)
#else
#define SYS_FREQ (40000000)
#endif

    /** LED ************************************************************/
    
    #define mLED_1              LATAbits.LATA1


    #define mGetLED_1()         mLED_1
    #define mLED_1_On()         mLED_1 = 1;
    #define mLED_1_Off()        mLED_1 = 0;
    #define mLED_1_Toggle()     mLED_1 = !mLED_1;
    #define mInitAllLEDs()      TRISAbits.TRISA1=0;mLED_1_Off();
    /** SWITCH *********************************************************/
    #define mInitSwitch2()      TRISBbits.TRISB7=1;
    #define mInitAllSwitches()  mInitSwitch2();

    // Various clock values
    #define GetSystemClock()    SYS_FREQ        // 48000000UL
    #define GetPeripheralClock() SYS_FREQ



//	PS2 KEYBOARD HANDLER SELECT =========
#define	PS2KBD_USE_INTERRUPT	0
//	1: 	hardware interrupt ( PCINT8 )
//	0:	software emulate   ( called from timer-interrupt )

//	PS2 KEYBOARD ENABLE GETCHAR FUNCTION =======
#define	PS2KBD_USE_GETCHAR		1

//	PS2 KEYBOARD ENABLE PRESSTABLE FUNCTION =======
#define	PS2KBD_USE_PRESSTABLE	1


#endif
