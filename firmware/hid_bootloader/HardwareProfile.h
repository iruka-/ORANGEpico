#ifndef HARDWARE_PROFILE__H
#define HARDWARE_PROFILE__H

//#define USE_SELF_POWER_SENSE_IO
#define tris_self_power     TRISAbits.TRISA2    // Input
#define self_power          1

//#define USE_USB_BUS_SENSE_IO
#define tris_usb_bus_sense  TRISBbits.TRISB7    // Input
#define USB_BUS_SENSE       1

/** LED ************************************************************/

#define mLED_1              LATBbits.LATB15
//    #define mLED_2              LATBbits.LATB14

#define mGetLED_1()         mLED_1
#define mGetLED_2()         mLED_2

#define mLED_1_On()         mLED_1 = 1;
#define mLED_2_On()         mLED_2 = 1;

#define mLED_1_Off()        mLED_1 = 0;
#define mLED_2_Off()        mLED_2 = 0;

#define mLED_1_Toggle()     mLED_1 = !mLED_1;
#define mLED_2_Toggle()     mLED_2 = !mLED_2;

/** LED *********************************************************/
#define mInitAllLEDs()      TRISBbits.TRISB15=0;mLED_1_Off();

/** BOOT SWITCH *********************************************************/
#define mInitBootSwitch()   TRISBbits.TRISB7=1;
#define Boot_SW             PORTBbits.RB7

#if	0
#define	Boot_SW_POLARITY	1		// If not PRESSED , Then Run Sketch Program (UBW32 Compat)
#else
#define	Boot_SW_POLARITY	0		// If PRESSED , Then Run Sketch Program (Pinguino Useful)
#endif

/** SECTIONS *********************************************************/
#define	_BOOTROM_  __attribute__((section(".bootrom")))


#endif  //HARDWARE_PROFILE__H

