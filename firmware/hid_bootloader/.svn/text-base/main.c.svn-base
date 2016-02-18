/********************************************************************
 FileName:     main.c
 Dependencies: See INCLUDES section
 Processor:		PIC32MX USB Microcontrollers
 Hardware:		The code is natively intended to be used on the following
 				hardware platforms:
 				Explorer 16 + PIC32MX460F512L PIM.  The firmware may be
 				modified for use on other USB platforms by editing the
 				HardwareProfile.h file.
 Complier:  	Microchip C32 (for PIC32)
 Company:		Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the 'Company') for its PIC(TM) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

********************************************************************
 File Description:

 Change History:

  Rev   Description
  ----- ---------------------------------------------
  v2.2  Adapted from PIC18F87J50 HID Bootloader Firmware
        as basis for BootApplication().  The rest of the
        code was taken from the Simple HID Demo in
        MCHPFSUSB v2.2.

  vx.x  Fixed race condition where an OUT packet could potentially
        clear the prior IN packet depending on the bus communication
        order.


  Rev   Date         Description
  1.0   04/30/2008   Initial Release
  					 Adapted from PIC18F87J50 HID Bootloader Firmware
  					 as basis for BootApplication().  The rest of the
  					 code was taken from the Simple HID Demo in
  					 MCHPFSUSB v2.1.
  1.1				 Modified for UBW32 project with software_key
  					 and different button/LED I/O bits
  1.2				 Fixed bug in boot_software_key_sec so it works
  					 properly all the time.
  1.3				 Updated code to use latest stack USB v2.5a
  1.4				 Updated main code to match HID Bootloader from 2.6A USB stack

		08/28/2010	 Updated to use Microchip folder - from Microchip Applications Library USB
					 stack v2.7 - updated project to use unchanged MAL Microchip folder - no code changes
		10/17/2011	 Switched to XT mode from HS for cyrstal gain - will help with startup of clock (not overdriving)
  1.5   11/09/2011   Updated to use MAL USB stack 2.9a
                     Updated procdefs.ld and SoftwareKey declration to work with C32 v2.00 and above
                     Created MPLAB X project (tested with beta 7.12)

********************************************************************/


/** INCLUDES *******************************************************/
#include "./USB/usb.h"
#include "HardwareProfile.h"
#include "./USB/usb_function_hid.h"

#include "NVMem.h"

/** CONFIGURATION **************************************************/
#if !defined(__PIC32MX__)
#error "This project was designed for PIC32MX family devices.  Please select the appropriate project for your target device family."
#endif

#pragma config FPLLODIV = DIV_2         // PLL Output Divider
#pragma config UPLLEN   = ON       		// USB PLL Enabled
#pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
#pragma config FPLLMUL  = MUL_20        // PLL Multiplier
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider
#pragma config FWDTEN   = OFF           // Watchdog Timer
#pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
#pragma config WDTPS    = PS1           // Watchdog Timer Postscale
#pragma config FCKSM    = CSECME        // Clock Switching & Fail Safe Clock Monitor enabled
#pragma config OSCIOFNC = OFF           // CLKO Enable
#pragma config POSCMOD  = HS            // Primary Oscillator
#pragma config IESO     = OFF          	// Internal/External Switch-over
#pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable (KLO was off)
#pragma config FNOSC    = PRIPLL        // Oscillator Selection
#pragma config CP       = OFF           // Code Protect
#pragma config BWP      = OFF           // Boot Flash Write Protect
#pragma config PWP      = OFF           // Program Flash Write Protect
#pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
#pragma config DEBUG    = OFF            // Background Debugger Enable


/** C O N S T A N T S **********************************************************/


/** STRUCT *********************************************************/
//	for PORT SENSE (64byte)
typedef	struct {
	int ansela;
	int ansela_clr;
	int ansela_set;
	int ansela_inv;
	int trisa;
	int trisa_clr;
	int trisa_set;
	int trisa_inv;
	int porta;
	int porta_clr;
	int porta_set;
	int porta_inv;
	int lata;
	int lata_clr;
	int lata_set;
	int lata_inv;
} pAnselA;

/**** configurations settings *************************************************/
#if 1	//defined(__PIC32MX1XX_2XX__)
// PIC32MX1xx and PIC32MX2xx devices
#define FLASH_PAGE_SIZE		 		1024
#else
// PIC32MX3xx to PIC32MX7xx devices
#define FLASH_PAGE_SIZE 			4096
#endif


// **THIS VALUE MUST BE ALIGNED WITH BLOCK BOUNDRY** Also, in order to work correctly, make sure the StartPageToErase is set to erase this section.

//	ユーザープログラムの開始番地(実際には1000,2000,3000,4000,5000までを検索する)
#ifdef	USE_INTERNAL_FRC_OSC	// RC OSC
#define BootMemStart					0x9D002000 
#else
#define BootMemStart					0x9D001000 
#endif

//Beginning of application program memory (not occupied by bootloader).  **THIS VALUE MUST BE ALIGNED WITH BLOCK BOUNDRY** Also, in order to work correctly, make sure the StartPageToErase is set to erase this section.

//	物理メモリーでFlashの消去先頭と終了:
#define ProgramMemStart	(BootMemStart & 0x7fffffff) //Beginning of application program memory (not occupied by bootloader).
//#define ProgramMemEnd					0x1D008000 //Beginning of application program memory (not occupied by bootloader).


#define MaxPageToEraseNoConfigs		    BMXPFMSZ/FLASH_PAGE_SIZE		 //Last full page of flash on the PIC24FJ256GB110, which does not contain the flash configuration words.
#define MaxPageToEraseWithConfigs	    BMXPFMSZ/FLASH_PAGE_SIZE		 //Page 170 contains the flash configurations words on the PIC24FJ256GB110.  Page 170 is also smaller than the rest of the (1536 byte) pages.

#define ProgramMemStopNoConfigs		    0x1D000000 + BMXPFMSZ
#define ProgramMemStopWithConfigs	    0x1D000000 + BMXPFMSZ

/**** definitions *************************************************************/
//Switch State Variable Choices
#define	QUERY_DEVICE				    0x02	//Command that the host uses to learn about the device (what regions can be programmed, and what type of memory is the region)
#define	UNLOCK_CONFIG				    0x03	//Note, this command is used for both locking and unlocking the config bits (see the "//Unlock Configs Command Definitions" below)
#define ERASE_DEVICE				    0x04	//Host sends this command to start an erase operation.  Firmware controls which pages should be erased.
#define PROGRAM_DEVICE				    0x05	//If host is going to send a full RequestDataBlockSize to be programmed, it uses this command.
#define	PROGRAM_COMPLETE			    0x06	//If host send less than a RequestDataBlockSize to be programmed, or if it wished to program whatever was left in the buffer, it uses this command.
#define GET_DATA					    0x07	//The host sends this command in order to read out memory from the device.  Used during verify (and read/export hex operations)
#define	RESET_DEVICE				    0x08	//Resets the microcontroller, so it can update the config bits (if they were programmed, and so as to leave the bootloader (and potentially go back into the main application)

#define PUT_DATA					    0x09
#define EXEC_CODE					    0x0a
#define PORT_SENSE					    0x0b

//Unlock Configs Command Definitions
#define UNLOCKCONFIG				    0x00	//Sub-command for the ERASE_DEVICE command
#define LOCKCONFIG					    0x01	//Sub-command for the ERASE_DEVICE command

//Query Device Response "Types"
#define	TypeProgramMemory			    0x01    //When the host sends a QUERY_DEVICE command, need to respond by populating a list of valid memory regions that exist in the device (and should be programmed)
#define TypeEEPROM					    0x02
#define TypeConfigWords				    0x03
#define	TypeEndOfTypeList			    0xFF    //Sort of serves as a "null terminator" like number, which denotes the end of the memory region list has been reached.


//BootState Variable States
#define	IdleState					    0x00
#define NotIdleState				    0x01

//OtherConstants
#define InvalidAddress				    0xFFFFFFFF

//Application and Microcontroller constants
#define DEVICE_FAMILY		            0x03    //0x01 for PIC18, 0x02 for PIC24, 0x03 for PIC23

#define	TotalPacketSize				    0x40
#define WORDSIZE				    	0x04    //PIC18 uses 2 byte instruction words, PIC24 uses 3 byte "instruction words" (which take 2 addresses, since each address is for a 16 bit word; the upper word contains a "phantom" byte which is unimplemented.).
#define RequestDataBlockSize 	        56  	//Number of bytes in the "Data" field of a standard request to/from the PC.  Must be an even number from 2 to 56. 
#define BufferSize 				        0x20    //32 16-bit words of buffer

/** PRIVATE PROTOTYPES *********************************************/
//void BlinkUSBStatus(void);
BOOL Switch2IsPressed(void);
BOOL Switch3IsPressed(void);
void Emulate_Mouse(void);
static void InitializeSystem(void);
void ProcessIO(void);
void YourHighPriorityISRCode();
void YourLowPriorityISRCode();

void EraseFlash(void);
void WriteFlashSubBlock(void);
void BootApplication(void);
DWORD ReadProgramMemory(DWORD);

/** T Y P E  D E F I N I T I O N S ************************************/

typedef union __attribute__ ((packed)) _USB_HID_BOOTLOADER_COMMAND {
	unsigned char Contents[64];

	struct __attribute__ ((packed)) {
		unsigned char Command;
		WORD AddressHigh;
		WORD AddressLow;
		unsigned char Size;
		unsigned char PadBytes[(TotalPacketSize - 6) - (RequestDataBlockSize)];
		unsigned int Data[RequestDataBlockSize/WORDSIZE];
	};

	struct __attribute__ ((packed)) {
		unsigned char Command;
		DWORD Address;
		unsigned char Size;
		unsigned char PadBytes[(TotalPacketSize - 6) - (RequestDataBlockSize)];
		unsigned int Data[RequestDataBlockSize/WORDSIZE];
	};

	struct __attribute__ ((packed)) {
		unsigned char Command;
		unsigned char PacketDataFieldSize;
		unsigned char DeviceFamily;
		unsigned char Type1;
		unsigned long Address1;
		unsigned long Length1;
		unsigned char Type2;
		unsigned long Address2;
		unsigned long Length2;
		unsigned char Type3;		//End of sections list indicator goes here, when not programming the vectors, in that case fill with 0xFF.
		unsigned long Address3;
		unsigned long Length3;
		unsigned char Type4;		//End of sections list indicator goes here, fill with 0xFF.
		unsigned char ExtraPadBytes[33];
	};

	struct __attribute__ ((packed)) {						//For lock/unlock config command
		unsigned char Command;
		unsigned char LockValue;
	};
} PacketToFromPC;

/** VARIABLES ******************************************************/
#pragma udata

#pragma udata USB_VARS
PacketToFromPC PacketFromPC;		//64 byte buffer for receiving packets on EP1 OUT from the PC
PacketToFromPC PacketToPC;			//64 byte buffer for sending packets on EP1 IN to the PC
PacketToFromPC PacketFromPCBuffer;

#pragma udata
USB_HANDLE USBOutHandle = 0;
USB_HANDLE USBInHandle = 0;
BOOL blinkStatusValid ;
// This was the way that the C32 v1.12 and before compiler needed things
//unsigned int __attribute__((section(".boot_software_key_sec,\"aw\",@nobits#"))) SoftwareKey;
// Updated this line for C32 v2.00 and above
//unsigned int SoftwareKey __attribute__((persistent,section("boot_software_key"),address(0xA0000000)));
//unsigned int SoftwareKey __attribute__((persistent));
// Total, total hack, to get around bug in C32 v2.01
unsigned int * SoftwareKey = NULL;


unsigned int  ProgramMemEnd;

unsigned char MaxPageToErase;
unsigned long ProgramMemStopAddress;
unsigned char BootState;
unsigned char ErasePageTracker;
unsigned int ProgrammingBuffer[BufferSize];
unsigned char BufferedDataIndex;
unsigned long ProgrammedPointer;
unsigned char ConfigsProtected;

/** DECLARATIONS ***************************************************/
#pragma code

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


#ifdef	USE_INTERNAL_FRC_OSC	// RC OSC
/********************************************************************
 * Function:	Clock Select to FRC
 *******************************************************************/
void InitializeFRC()
{
	mSysUnlockOpLock( 
		{PPSOutput(3, RPA4, REFCLKO);}
	);

	OSCREFConfig(OSC_REFOCON_FRC,OSC_REFOCON_OE | OSC_REFOCON_ON, 1);

	mSysUnlockOpLock(
		{OSCCONbits.NOSC = 3; //ECPLL
		 OSCCONbits.OSWEN = 1;}
	);

	while(OSCCONbits.COSC != 3);
	SYSTEMConfigPerformance(40000000);
}
#endif

/********************************************************************
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

#define SYS_FREQ (40000000L)
#define SYS_CFG_WAIT_STATES     0x00000001 //SYSTEMConfig wait states
#define SYS_CFG_PB_BUS          0x00000002 //SYSTEMConfig pb bus
#define SYS_CFG_PCACHE          0x00000004 //SYSTEMConfig cache
#define	_CHECON	0xBF884000


static inline void UserInit(void)
{
	//Initialize all of the LED pins
	mInitAllLEDs();

	//initialize the variable holding the handle for the last
	// transmission
	USBOutHandle = 0;
	USBInHandle = 0;

	blinkStatusValid = TRUE;

	//Initialize bootloader state variables
	MaxPageToErase = MaxPageToEraseNoConfigs;		//Assume we will not allow erase/programming of config words (unless host sends override command)
	ProgramMemStopAddress = ProgramMemStopNoConfigs;
	ConfigsProtected = LOCKCONFIG;					//Assume we will not erase or program the vector table at first.  Must receive unlock config bits/vectors command first.
	BootState = IdleState;
	ProgrammedPointer = InvalidAddress;
	BufferedDataIndex = 0;

	ProgramMemEnd = 0x1D000000 + BMXPFMSZ;	// BMXPFMSZ:I/O(R):  FlashROMのサイズ

}//end UserInit

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
static inline void InitializeSystem(void)
{
//	The USB specifications require that USB peripheral devices must never source
//	current onto the Vbus pin.  Additionally, USB peripherals should not source
//	current on D+ or D- when the host/hub is not actively powering the Vbus line.
//	When designing a self powered (as opposed to bus powered) USB peripheral
//	device, the firmware should make sure not to turn on the USB module and D+
//	or D- pull up resistor unless Vbus is actively powered.  Therefore, the
//	firmware needs some means to detect when Vbus is being powered by the host.
//	A 5V tolerant I/O pin can be connected to Vbus (through a resistor), and
// 	can be used to detect when Vbus is high (host actively powering), or low
//	(host is shut down or otherwise not supplying power).  The USB firmware
// 	can then periodically poll this I/O pin to know when it is okay to turn on
//	the USB module/D+/D- pull up resistor.  When designing a purely bus powered
//	peripheral device, it is not possible to source current on D+ or D- when the
//	host is not actively providing power on Vbus. Therefore, implementing this
//	bus sense feature is optional.  This firmware can be made to use this bus
//	sense feature by making sure "USE_USB_BUS_SENSE_IO" has been defined in the
//	HardwareProfile.h file.
#if defined(USE_USB_BUS_SENSE_IO)
	tris_usb_bus_sense = INPUT_PIN; // See HardwareProfile.h
#endif

//	If the host PC sends a GetStatus (device) request, the firmware must respond
//	and let the host know if the USB peripheral device is currently bus powered
//	or self powered.  See chapter 9 in the official USB specifications for details
//	regarding this request.  If the peripheral device is capable of being both
//	self and bus powered, it should not return a hard coded value for this request.
//	Instead, firmware should check if it is currently self or bus powered, and
//	respond accordingly.  If the hardware has been configured like demonstrated
//	on the PICDEM FS USB Demo Board, an I/O pin can be polled to determine the
//	currently selected power source.  On the PICDEM FS USB Demo Board, "RA2"
//	is used for	this purpose.  If using this feature, make sure "USE_SELF_POWER_SENSE_IO"
//	has been defined in HardwareProfile.h, and that an appropriate I/O pin has been mapped
//	to it in HardwareProfile.h.
#if defined(USE_SELF_POWER_SENSE_IO)
	tris_self_power = INPUT_PIN;	// See HardwareProfile.h
#endif

	USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
	//variables to known states.
	UserInit();

}//end InitializeSystem


int _BOOTROM_ main(void)
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

	// The PRG switch is tied high, so when it is not pressed it will read
	// 1. To call the normal application (i.e. don't go into the bootloader)
	// we need to make sure that the PRG button is not pressed, and that we
	// don't have a software reset
	if(	(Boot_SW == Boot_SW_POLARITY) ||
	    (mGetSWRFlag() && (*SoftwareKey == 0x12345678) ) ){
		// Must clear out software key
		*SoftwareKey = 0;

		//
		//	ユーザープログラムを 9D001000,9D002000,9D003000,9D004000,9D005000 の順に検索する.
		//
		int start_addr=BootMemStart;	// 9d001000
		for(;start_addr<=0x9d005000;start_addr+=0x1000) {
			if (*(int *)start_addr != 0xFFFFFFFF) {
				call_ptr(start_addr);
				while(1);
			}
		}
	}
#endif

//	led_test();

	InitializeSystem();
	mClearSWRFlag();
	*SoftwareKey = 0;		// Must clear out software key

#if defined(USB_INTERRUPT)
	USBDeviceAttach();
#endif

	while(1) {
#if defined(USB_POLLING)
		// Check bus status and service USB interrupts.
		USBDeviceTasks(); // Interrupt or polling method.  If using polling, must call
		// this function periodically.  This function will take care
		// of processing and responding to SETUP transactions
		// (such as during the enumeration process when you first
		// plug in).  USB hosts require that USB devices should accept
		// and process SETUP packets in a timely fashion.  Therefore,
		// when using polling, this function should be called
		// frequently (such as once about every 100 microseconds) at any
		// time that a SETUP packet might reasonably be expected to
		// be sent by the host to your device.  In most cases, the
		// USBDeviceTasks() function does not take very long to
		// execute (~50 instruction cycles) before it returns.
#endif

		// Application-specific tasks.
		// Application related code may be added here, or in the ProcessIO() function.
		ProcessIO();
	}//end while
}//end main


/********************************************************************
 * Function:        void BlinkUSBStatus(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        BlinkUSBStatus turns on and off LEDs
 *                  corresponding to the USB device state.
 *
 * Note:            mLED macros can be found in HardwareProfile.h
 *                  USBDeviceState is declared and updated in
 *                  usb_device.c.
 *******************************************************************/
static inline	void BlinkUSBStatus(void)
{
	static WORD led_count=0;

	if(led_count == 0)led_count = 10000U;
	led_count--;

#ifdef mLED_2
#define mLED_Both_Off()         {mLED_1_Off();mLED_2_Off();}
#define mLED_Both_On()          {mLED_1_On();mLED_2_On();}
#define mLED_Only_1_On()        {mLED_1_On();mLED_2_Off();}
#define mLED_Only_2_On()        {mLED_1_Off();mLED_2_On();}
#else
#define mLED_Both_Off()         {mLED_1_Off();}
#define mLED_Both_On()          {mLED_1_On();}
#define mLED_Only_1_On()        {mLED_1_On();}
#define mLED_Only_2_On()        {mLED_1_Off();}
#endif


	if(USBSuspendControl == 1) {
		if(led_count==0) {
			mLED_1_Toggle();
#ifdef mLED_2
			mLED_2 = mLED_1;        // Both blink at the same time
#endif
		}//end if
	} else {
		if(USBDeviceState == DETACHED_STATE) {
			mLED_Both_Off();
		} else if(USBDeviceState == ATTACHED_STATE) {
			mLED_Both_On();
		} else if(USBDeviceState == POWERED_STATE) {
			mLED_Only_1_On();
		} else if(USBDeviceState == DEFAULT_STATE) {
#ifdef mLED_2
			mLED_Only_2_On();
#endif
		} else if(USBDeviceState == ADDRESS_STATE) {
			if(led_count == 0) {
				mLED_1_Toggle();
#ifdef mLED_2
				mLED_2_Off();
#endif
			}//end if
		} else if(USBDeviceState == CONFIGURED_STATE) {
			if(led_count==0) {
				mLED_1_Toggle();
#ifdef mLED_2
				mLED_2 = !mLED_1;       // Alternate blink
#endif
			}//end if
		}//end if(...)
	}//end if(UCONbits.SUSPND...)

}//end BlinkUSBStatus




/******************************************************************************
 * Function:        void BootApplication(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a place holder for other user routines.
 *                  It is a mixture of both USB and non-USB tasks.
 *
 * Note:            None
 *****************************************************************************/
void _BOOTROM_ BootApplication(void)
{
	unsigned char i;
	unsigned int j;

	if(BootState == IdleState) {
		//Are we done sending the last response.  We need to be before we
		//  receive the next command because we clear the PacketToPC buffer
		//  once we receive a command
		if(!USBHandleBusy(USBInHandle)) {
			if(!USBHandleBusy(USBOutHandle)) {	//Did we receive a command?
				for(i = 0; i < TotalPacketSize; i++) {
					PacketFromPC.Contents[i] = PacketFromPCBuffer.Contents[i];
				}

				USBOutHandle = USBRxOnePacket(HID_EP,(BYTE*)&PacketFromPCBuffer,64);
				BootState = NotIdleState;

				//Prepare the next packet we will send to the host, by initializing the entire packet to 0x00.
				for(i = 0; i < TotalPacketSize; i++) {
					//This saves code space, since we don't have to do it independently in the QUERY_DEVICE and GET_DATA cases.
					PacketToPC.Contents[i] = 0;
				}
			}
		}
	} else { //(BootState must be in NotIdleState)
		switch(PacketFromPC.Command) {
		case QUERY_DEVICE: {
			//Prepare a response packet, which lets the PC software know about the memory ranges of this device.

			PacketToPC.Command = (unsigned char)QUERY_DEVICE;
			PacketToPC.PacketDataFieldSize = (unsigned char)RequestDataBlockSize;
			PacketToPC.DeviceFamily = (unsigned char)DEVICE_FAMILY;

			PacketToPC.Type1 = (unsigned char)TypeProgramMemory;
			PacketToPC.Address1 = (unsigned long)ProgramMemStart;
			PacketToPC.Length1 = (unsigned long)(ProgramMemStopAddress - ProgramMemStart);	//Size of program memory area
			PacketToPC.Type2 = (unsigned char)TypeEndOfTypeList;

//				if(ConfigsProtected == UNLOCKCONFIG)
//				{
//    				PacketToPC.Type2 = (unsigned char)TypeProgramMemory;				//Overwrite the 0xFF end of list indicator if we wish to program the Vectors.
//                    PacketToPC.Address2 = (unsigned long)VectorsStart;
//                    PacketToPC.Length2 = (unsigned long)(VectorsEnd - VectorsStart);	//Size of program memory area
//
//                    PacketToPC.Type3 = (unsigned char)TypeConfigWords;
//                    PacketToPC.Address3 = (unsigned long)ConfigWordsStartAddress;
//                    PacketToPC.Length3 = (unsigned long)(ConfigWordsStopAddress - ConfigWordsStartAddress);
//                    PacketToPC.Type4 = (unsigned char)TypeEndOfTypeList;
//				}

			//Init pad bytes to 0x00...  Already done after we received the QUERY_DEVICE command (just after calling HIDRxPacket()).

			if(!USBHandleBusy(USBInHandle)) {
				USBInHandle = USBTxOnePacket(HID_EP,(BYTE*)&PacketToPC,64);
				BootState = IdleState;
			}
		}
		break;
		case UNLOCK_CONFIG: {
			if(PacketFromPC.LockValue == UNLOCKCONFIG) {
				MaxPageToErase = MaxPageToEraseWithConfigs;		//Assume we will not allow erase/programming of config words (unless host sends override command)
				ProgramMemStopAddress = ProgramMemStopWithConfigs;
				ConfigsProtected = UNLOCKCONFIG;
			} else {	//LockValue must be == LOCKCONFIG
				MaxPageToErase = MaxPageToEraseNoConfigs;
				ProgramMemStopAddress = ProgramMemStopNoConfigs;
				ConfigsProtected = LOCKCONFIG;
			}
			BootState = IdleState;
		}
		break;
		case ERASE_DEVICE: {
			char* pFlash = (char*)ProgramMemStart;
			int temp;

			for( temp = 0; temp < (MaxPageToErase); temp++ ) {
//        			NVMErasePage( pFlash + (temp*FLASH_PAGE_SIZE) );
				NVMemErasePage( pFlash );
				pFlash += FLASH_PAGE_SIZE;
				USBDeviceTasks(); 	//Call USBDriverService() periodically to prevent falling off the bus if any SETUP packets should happen to arrive.
				if((int)pFlash >= 0x1D008000) break; 	// MX220とりあえずの対処.
			}
			NVMCONbits.WREN = 0;		//Good practice to clear WREN bit anytime we are not expecting to do erase/write operations, further reducing probability of accidental activation.
			BootState = IdleState;
		}
		break;
		case PROGRAM_DEVICE: {
			if( ProgrammedPointer == (unsigned long)InvalidAddress) {
				ProgrammedPointer = PacketFromPC.Address;
			}
			if( ProgrammedPointer == (unsigned long)PacketFromPC.Address) {
				for(i = 0; i < (PacketFromPC.Size/WORDSIZE); i++) {
					unsigned int index = (RequestDataBlockSize-PacketFromPC.Size)/WORDSIZE+i;
					ProgrammingBuffer[BufferedDataIndex] = PacketFromPC.Data[(RequestDataBlockSize-PacketFromPC.Size)/WORDSIZE+i];	//Data field is right justified.  Need to put it in the buffer left justified.
					BufferedDataIndex++;
					ProgrammedPointer+=WORDSIZE;
					if(BufferedDataIndex == (RequestDataBlockSize/WORDSIZE)) {	//Need to make sure it doesn't call WriteFlashSubBlock() unless BufferedDataIndex/2 is an integer
						WriteFlashSubBlock();
					}
				}
			}
			//else host sent us a non-contiguous packet address...  to make this firmware simpler, host should not do this without sending a PROGRAM_COMPLETE command in between program sections.
			BootState = IdleState;
		}
		break;
		case PROGRAM_COMPLETE: {
			WriteFlashSubBlock();
			ProgrammedPointer = InvalidAddress;		//Reinitialize pointer to an invalid range, so we know the next PROGRAM_DEVICE will be the start address of a contiguous section.
			BootState = IdleState;
		}
		break;
#if	1
		case PORT_SENSE: 
			if(!USBHandleBusy(USBInHandle)) {
				DWORD *p = PacketToPC.Data;
				pAnselA *port = (pAnselA *) &ANSELA;

				PacketToPC.Command = PORT_SENSE;
#if	0
				p[0]=TRISA;
				p[1]=PORTA;
				p[2]=LATA;
				p[3]=ANSELA;
				
				p[4]=TRISB;
				p[5]=PORTB;
				p[6]=LATB;
				p[7]=ANSELB;
#else
				p[0]=port->trisa;
				p[1]=port->porta;
				p[2]=port->lata;
				p[3]=port->ansela;
//				port += 4;	// +256bytes
				port = (pAnselA *) &ANSELB;
				p[4]=port->trisa;
				p[5]=port->porta;
				p[6]=port->lata;
				p[7]=port->ansela;
#endif
				goto L_getdata;
//				USBInHandle = USBTxOnePacket(HID_EP,(BYTE*)&PacketToPC.Contents[0],64);
//				BootState = IdleState;
			}
			break;

		case GET_DATA: {
			if(!USBHandleBusy(USBInHandle)) {
				DWORD* p;
				int cnt = ( (PacketFromPC.Size+WORDSIZE-1) /WORDSIZE);
				//Init pad bytes to 0x00...  Already done after we received the QUERY_DEVICE command (just after calling HIDRxReport()).
				PacketToPC.Command = GET_DATA;
				PacketToPC.Address = PacketFromPC.Address;
				PacketToPC.Size    = PacketFromPC.Size;
				p = (DWORD*)((PacketFromPC.Address|0x80000000));
//				if(PacketFromPC.Address & 3) {
//					memcpy(PacketToPC.Data,p,PacketFromPC.Size);	// byte by byte
//				}else
				{
					for(i = 0; i < cnt; i++) {						// (size+3) / 4
						PacketToPC.Data[i]=*p++;					// DWORD.
					}
				}
L_getdata:		USBInHandle = USBTxOnePacket(HID_EP,(BYTE*)&PacketToPC.Contents[0],64);
				BootState = IdleState;
			}
		}
		break;

		case PUT_DATA: 
		{
			DWORD* p = (DWORD*)((PacketFromPC.Address|0x80000000));
//			if((PacketFromPC.Address & 3)||(PacketFromPC.Size & 3)) {
//				memcpy(p,PacketFromPC.Data,PacketFromPC.Size);
//			}else
			{
				for(i = 0; i < (PacketFromPC.Size/WORDSIZE); i++) {
					*p++ = PacketFromPC.Data[i];
				}
			}
			BootState = IdleState;
		}
			break;

		case EXEC_CODE: 
		{
			call_ptr(PacketFromPC.Address);
			BootState = IdleState;
		}
		break;
#endif
		case RESET_DEVICE: {
	        *SoftwareKey = 0x12345678;	// Software Reset Signal:
	        
			U1CON = 0x0000;				//Disable USB module
			//And wait awhile for the USB cable capacitance to discharge down to disconnected (SE0) state.
			//Otherwise host might not realize we disconnected/reconnected when we do the reset.
			//A basic for() loop decrementing a 16 bit number would be simpler, but seems to take more code space for
			//a given delay.  So do this instead:
			for(j = 0; j < 0xFFFF; j++) {
				Nop();
			}
			Reset();
		}
		break;
		}//End switch
	}//End if/else

}


/********************************************************************
 * Function:        void ProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a place holder for other user
 *                  routines. It is a mixture of both USB and
 *                  non-USB tasks.
 *
 * Note:            None
 *******************************************************************/
void _BOOTROM_ ProcessIO(void)
{
	//Blink the LEDs according to the USB device status
	if(blinkStatusValid) {
		BlinkUSBStatus();
	}

	// User Application USB tasks
	if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) return;

	BootApplication();

}//end ProcessIO



void _BOOTROM_ WriteFlashSubBlock(void)	//Use word writes to write code chunks less than a full 64 byte block size
{
	unsigned int i = 0;
	int Address,phy,rc=0;

	while(BufferedDataIndex > 0) {		//While data is still in the buffer.
		Address = (ProgrammedPointer - (BufferedDataIndex * WORDSIZE));
		phy = Address & 0x1fffffff;		//物理メモリーアドレスに変換.
		if( (phy>=ProgramMemStart)&&(phy<ProgramMemEnd) ) {	//書き換え許可範囲内か?
			rc=NVMemWriteWord((void*)Address, (unsigned int)ProgrammingBuffer[i]);	// Yes.
		}
		i++;
		BufferedDataIndex --;		//Used up 2 (16-bit) words from the buffer.
#if	0
		if(rc!=0) {		//書き込みエラーを検知してハングアップさせる.
			while(1);
		}
#endif
	}
	Nop();
}


// ******************************************************************************************************
// ************** USB Callback Functions ****************************************************************
// ******************************************************************************************************
// The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
// events.  For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
// packets to your device.  In response to this, all USB devices are supposed to decrease their power
// consumption from the USB Vbus to <2.5mA each.  The USB module detects this condition (which according
// to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
// function.  You should modify these callback functions to take appropriate actions for each of these
// conditions.  For example, in the USBCBSuspend(), you may wish to add code that will decrease power
// consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
// microcontroller to sleep, etc.).  Then, in the USBCBWakeFromSuspend() function, you may then wish to
// add code that undoes the power saving things done in the USBCBSuspend() function.

// The USBCBSendResume() function is special, in that the USB stack will not automatically call this
// function.  This function is meant to be called from the application firmware instead.  See the
// additional comments near the function.

/******************************************************************************
 * Function:        void USBCBSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Call back that is invoked when a USB suspend is detected
 *
 * Note:            None
 *****************************************************************************/
static	inline void USBCBSuspend(void)
{
	//Example power saving code.  Insert appropriate code here for the desired
	//application behavior.  If the microcontroller will be put to sleep, a
	//process similar to that shown below may be used:

	//ConfigureIOPinsForLowPower();
	//SaveStateOfAllInterruptEnableBits();
	//DisableAllInterruptEnableBits();
	//EnableOnlyTheInterruptsWhichWillBeUsedToWakeTheMicro();	//should enable at least USBActivityIF as a wake source
	//Sleep();
	//RestoreStateOfAllPreviouslySavedInterruptEnableBits();	//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.
	//RestoreIOPinsToNormal();									//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.

	//IMPORTANT NOTE: Do not clear the USBActivityIF (ACTVIF) bit here.  This bit is
	//cleared inside the usb_device.c file.  Clearing USBActivityIF here will cause
	//things to not work as intended.


#if defined(__C30__)
#if 0
	U1EIR = 0xFFFF;
	U1IR = 0xFFFF;
	U1OTGIR = 0xFFFF;
	IFS5bits.USB1IF = 0;
	IEC5bits.USB1IE = 1;
	U1OTGIEbits.ACTVIE = 1;
	U1OTGIRbits.ACTVIF = 1;
	Sleep();
#endif
#endif
}


/******************************************************************************
 * Function:        void _USB1Interrupt(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the USB interrupt bit is set
 *					In this example the interrupt is only used when the device
 *					goes to sleep when it receives a USB suspend command
 *
 * Note:            None
 *****************************************************************************/
#if 0
void __attribute__ ((interrupt)) _USB1Interrupt(void)
{
#if !defined(self_powered)
	if(U1OTGIRbits.ACTVIF) {
		IEC5bits.USB1IE = 0;
		U1OTGIEbits.ACTVIE = 0;
		IFS5bits.USB1IF = 0;

		//USBClearInterruptFlag(USBActivityIFReg,USBActivityIFBitNum);
		USBClearInterruptFlag(USBIdleIFReg,USBIdleIFBitNum);
		//USBSuspendControl = 0;
	}
#endif
}
#endif

/******************************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The host may put USB peripheral devices in low power
 *					suspend mode (by "sending" 3+ms of idle).  Once in suspend
 *					mode, the host may wake the device back up by sending non-
 *					idle state signalling.
 *
 *					This call back is invoked when a wakeup from USB suspend
 *					is detected.
 *
 * Note:            None
 *****************************************************************************/
static	inline void USBCBWakeFromSuspend(void)
{
	// If clock switching or other power savings measures were taken when
	// executing the USBCBSuspend() function, now would be a good time to
	// switch back to normal full power run mode conditions.  The host allows
	// a few milliseconds of wakeup time, after which the device must be
	// fully back to normal, and capable of receiving and processing USB
	// packets.  In order to do this, the USB module must receive proper
	// clocking (IE: 48MHz clock must be available to SIE for full speed USB
	// operation).
}

/********************************************************************
 * Function:        void USBCB_SOF_Handler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB host sends out a SOF packet to full-speed
 *                  devices every 1 ms. This interrupt may be useful
 *                  for isochronous pipes. End designers should
 *                  implement callback routine as necessary.
 *
 * Note:            None
 *******************************************************************/
static	inline void USBCB_SOF_Handler(void)
{
	// No need to clear UIRbits.SOFIF to 0 here.
	// Callback caller is already doing that.
}

/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The purpose of this callback is mainly for
 *                  debugging during development. Check UEIR to see
 *                  which error causes the interrupt.
 *
 * Note:            None
 *******************************************************************/
static	inline void USBCBErrorHandler(void)
{
	// No need to clear UEIR to 0 here.
	// Callback caller is already doing that.

	// Typically, user firmware does not need to do anything special
	// if a USB error occurs.  For example, if the host sends an OUT
	// packet to your device, but the packet gets corrupted (ex:
	// because of a bad connection, or the user unplugs the
	// USB cable during the transmission) this will typically set
	// one or more USB error interrupt flags.  Nothing specific
	// needs to be done however, since the SIE will automatically
	// send a "NAK" packet to the host.  In response to this, the
	// host will normally retry to send the packet again, and no
	// data loss occurs.  The system will typically recover
	// automatically, without the need for application firmware
	// intervention.

	// Nevertheless, this callback function is provided, such as
	// for debugging purposes.
}


/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        When SETUP packets arrive from the host, some
 * 					firmware must process the request and respond
 *					appropriately to fulfill the request.  Some of
 *					the SETUP packets will be for standard
 *					USB "chapter 9" (as in, fulfilling chapter 9 of
 *					the official USB specifications) requests, while
 *					others may be specific to the USB device class
 *					that is being implemented.  For example, a HID
 *					class device needs to be able to respond to
 *					"GET REPORT" type of requests.  This
 *					is not a standard USB chapter 9 request, and
 *					therefore not handled by usb_device.c.  Instead
 *					this request should be handled by class specific
 *					firmware, such as that contained in usb_function_hid.c.
 *
 * Note:            None
 *******************************************************************/
static	inline void USBCBCheckOtherReq(void)
{
	USBCheckHIDRequest();
}//end


/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USBCBStdSetDscHandler() callback function is
 *					called when a SETUP, bRequest: SET_DESCRIPTOR request
 *					arrives.  Typically SET_DESCRIPTOR requests are
 *					not used in most applications, and it is
 *					optional to support this type of request.
 *
 * Note:            None
 *******************************************************************/
static	inline void USBCBStdSetDscHandler(void)
{
	// Must claim session ownership if supporting this request
}//end


/*******************************************************************
 * Function:        void USBCBInitEP(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the device becomes
 *                  initialized, which occurs after the host sends a
 * 					SET_CONFIGURATION (wValue not = 0) request.  This
 *					callback function should initialize the endpoints
 *					for the device's usage according to the current
 *					configuration.
 *
 * Note:            None
 *******************************************************************/
static	inline void USBCBInitEP(void)
{
	//enable the HID endpoint
	USBEnableEndpoint(HID_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
	//Arm the OUT endpoint for the first packet
	USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&PacketFromPCBuffer,64);
}

/********************************************************************
 * Function:        void USBCBSendResume(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB specifications allow some types of USB
 * 					peripheral devices to wake up a host PC (such
 *					as if it is in a low power suspend to RAM state).
 *					This can be a very useful feature in some
 *					USB applications, such as an Infrared remote
 *					control	receiver.  If a user presses the "power"
 *					button on a remote control, it is nice that the
 *					IR receiver can detect this signalling, and then
 *					send a USB "command" to the PC to wake up.
 *
 *					The USBCBSendResume() "callback" function is used
 *					to send this special USB signalling which wakes
 *					up the PC.  This function may be called by
 *					application firmware to wake up the PC.  This
 *					function should only be called when:
 *
 *					1.  The USB driver used on the host PC supports
 *						the remote wakeup capability.
 *					2.  The USB configuration descriptor indicates
 *						the device is remote wakeup capable in the
 *						bmAttributes field.
 *					3.  The USB host PC is currently sleeping,
 *						and has previously sent your device a SET
 *						FEATURE setup packet which "armed" the
 *						remote wakeup capability.
 *
 *					This callback should send a RESUME signal that
 *                  has the period of 1-15ms.
 *
 * Note:            Interrupt vs. Polling
 *                  -Primary clock
 *                  -Secondary clock ***** MAKE NOTES ABOUT THIS *******
 *                   > Can switch to primary first by calling USBCBWakeFromSuspend()

 *                  The modifiable section in this routine should be changed
 *                  to meet the application needs. Current implementation
 *                  temporary blocks other functions from executing for a
 *                  period of 1-13 ms depending on the core frequency.
 *
 *                  According to USB 2.0 specification section 7.1.7.7,
 *                  "The remote wakeup device must hold the resume signaling
 *                  for at lest 1 ms but for no more than 15 ms."
 *                  The idea here is to use a delay counter loop, using a
 *                  common value that would work over a wide range of core
 *                  frequencies.
 *                  That value selected is 1800. See table below:
 *                  ==========================================================
 *                  Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 *                  ==========================================================
 *                      48              12          1.05
 *                       4              1           12.6
 *                  ==========================================================
 *                  * These timing could be incorrect when using code
 *                    optimization or extended instruction mode,
 *                    or when having other interrupts enabled.
 *                    Make sure to verify using the MPLAB SIM's Stopwatch
 *                    and verify the actual signal on an oscilloscope.
 *******************************************************************/
void _BOOTROM_ USBCBSendResume(void)
{
	static WORD delay_count;

	USBResumeControl = 1;                // Start RESUME signaling

	delay_count = 1800U;                // Set RESUME line for 1-13 ms
	do {
		delay_count--;
	} while(delay_count);
	USBResumeControl = 0;
}

/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *                        USB_EVENT event, void *pdata, WORD size)
 *
 * PreCondition:    None
 *
 * Input:           USB_EVENT event - the type of event
 *                  void *pdata - pointer to the event data
 *                  WORD size - size of the event data
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 *
 * Note:            None
 *******************************************************************/
BOOL _BOOTROM_ USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size)
{
	switch(event) {
	case EVENT_CONFIGURED:
		USBCBInitEP();
		break;
	case EVENT_SET_DESCRIPTOR:
		USBCBStdSetDscHandler();
		break;
	case EVENT_EP0_REQUEST:
		USBCBCheckOtherReq();
		break;
	case EVENT_SOF:
		USBCB_SOF_Handler();
		break;
	case EVENT_SUSPEND:
		USBCBSuspend();
		break;
	case EVENT_RESUME:
		USBCBWakeFromSuspend();
		break;
	case EVENT_BUS_ERROR:
		USBCBErrorHandler();
		break;
	case EVENT_TRANSFER:
		Nop();
		break;
	default:
		break;
	}
	return TRUE;
}


/** EOF main.c *************************************************/
