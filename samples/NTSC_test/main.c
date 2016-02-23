/********************************************************************
 FileName:      main.c
********************************************************************/

/** INCLUDES *******************************************************/
//#include "USB/usb.h"
//#include "USB/usb_function_generic.h"
//#include "GenericTypeDefs.h"
#include <plib.h>

#include "HardwareProfile.h"
#include "spi2.h"

#include "serial1.h"

#ifndef	BAUDRATE
#define	BAUDRATE	500000
#endif

/** VARIABLES ******************************************************/
//unsigned char INPacket[USBGEN_EP_SIZE] ;	//User application buffer for sending IN packets to the host
//unsigned char OUTPacket[USBGEN_EP_SIZE];	//User application buffer for receiving and holding OUT packets sent from the host

BOOL blinkStatusValid;

//USB_HANDLE USBGenericOutHandle; //USB handle.  Must be initialized to 0 at startup.
//USB_HANDLE USBGenericInHandle;  //USB handle.  Must be initialized to 0 at startup.


/** PRIVATE PROTOTYPES *********************************************/
static void InitializeSystem(void);
void USBDeviceTasks(void);
void YourHighPriorityISRCode(void);
void YourLowPriorityISRCode(void);
void USBCBSendResume(void);
void UserInit(void);
void ProcessIO(void);
void BlinkUSBStatus(void);

void init_vga(void);



/** VECTOR REMAPPING ***********************************************/

/** DECLARATIONS ***************************************************/

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

//void wait_ms(int ms);

void led_test()
{
	mInitAllLEDs();
	while(1) {
		mLED_1_Toggle();
		wait_ms(500);
	}
}


void led_toggle()
{
	mLED_1_Toggle();
}

void led_on()
{
	mLED_1_On();
}

void led_off()
{
	mLED_1_Off();
}

#define	_MIPS32 __attribute__((nomips16,noinline))

_MIPS32 void ei()
{
//	crt0.S ÇÃèâä˙âªÇ…ÇƒÅAÇ∑Ç≈Ç…MultiVectordÇ…ÇÕÇ»Ç¡ÇƒÇ¢ÇÈÅB
	INTCONSET=0x1000;		// EI PORT
//	INTEnableSystemMultiVectoredInt();	Ç±ÇÃä÷êîì‡Ç… INTCONSETê›íËÇ™ä‹Ç‹ÇÍÇÈ.

	INTEnableInterrupts();	// eiñΩóﬂ.
}
/******************************************************************************
 * Function:        void main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Main program entry point.
 *
 * Note:            None
 *******************************************************************/
int _MIPS32 main(void)
{
#ifdef	USE_INTERNAL_FRC_OSC	// RC OSC
	InitializeFRC();
#endif
	InitializeSystem();
	init_vga();
	ei();
	gr_test();

	mInitAllLEDs();
//	USBmonit();
	led_test();
}


/********************************************************************
 * Function:        static void InitializeSystem(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        InitializeSystem is a centralize initialization
 *                  routine. All required USB initialization routines
 *                  are called from here.
 *
 *                  User application initialization routine should
 *                  also be called from here.
 *
 * Note:            None
 *******************************************************************/
static void InitializeSystem(void)
{

	UserInit();			//Application related initialization.  See user.c

}//end InitializeSystem


// All Analog Pins as Digital IOs
static	void IOsetDigital()
{
//#ifdef __32MX220F032D__
	DDPCONbits.JTAGEN=0;		// check : already in system.c
	ANSELA = 0;
	ANSELB = 0;
//	ANSELC = 0;
//#else
//	AD1PCFG = 0xFFFF;
//#endif
}

void UserInit(void)
{
	IOsetDigital();
	mInitAllLEDs();
	mInitAllSwitches();

	blinkStatusValid = TRUE;	//Blink the normal USB state on the LEDs.
//	SerialConfigure(UART1, UART_ENABLE,	UART_RX_TX_ENABLED,	BAUDRATE);

}//end UserInit


