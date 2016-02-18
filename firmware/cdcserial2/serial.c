/*	----------------------------------------------------------------------------
	FILE:			serial.c
	PROJECT:		pinguinoX
	PURPOSE:		
	PROGRAMER:		regis blanchot <rblanchot@gmail.com>
	FIRST RELEASE:	10 nov. 2010
	LAST RELEASE:	18 feb. 2012
	----------------------------------------------------------------------------
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	--------------------------------------------------------------------------*/
	
	// 13 feb.2011 jp mandon added #define for RX/TX pin on 32mx440f256h
	// 21 set.2011 Marcus Fazzi added support for UART3
	// 23 set.2011 Marcus Fazzi added support for UART4,5 AND 6
	// 18 feb.2012 jp mandon added support for PIC32-PIGUINO-220
	// 19 may.2012 jp mandon added support for GENERIC32MX250F128 and GENERIC32MX220F032


//

#include <p32xxxx.h>			// always in first place to avoid conflict with const.h ON
#include "typedef.h"
#include "const.h"

#include "HardwareProfile.h"

#include "serial1.h"
#include "fifo.h"

	// IPCx: INTERRUPT PRIORITY CONTROL REGISTER
	#define INT_UART1_ALL_PRIORITY		0x0000001F	// disable all UART1 interrupts
	#define INT_UART2_ALL_PRIORITY		0x0000001F	// disable all UART2 interrupts
	#define INT_UART3_ALL_PRIORITY		0x0000001F	// disable all UART3 interrupts
	#define INT_PRIORITY_7				0b111
	#define INT_PRIORITY_6				0b110
	#define INT_PRIORITY_5				0b101
	#define INT_PRIORITY_4				0b100
	#define INT_PRIORITY_3				0b011
	#define INT_PRIORITY_2				0b010
	#define INT_PRIORITY_1				0b001
	#define INT_PRIORITY_DISABLED		0b000
	#define INT_SUBPRIORITY_3			0b11
	#define INT_SUBPRIORITY_2			0b10
	#define INT_SUBPRIORITY_1			0b01
	#define INT_SUBPRIORITY_0			0b00

#define	FIFO_SIZE	256

extern	int _ebase_address[];	// From: procdefs.ld


static	FIFO	rx_fifo;
static	char	rx_buff[FIFO_SIZE];

/*	----------------------------------------------------------------------------
	SerialEnable
	----------------------------------------------------------------------------
	ex : SerialEnable(UART1, UART_RX_TX_ENABLED | UART_INTERRUPT_ON_RX_FULL)
	--------------------------------------------------------------------------*/

static	void  SerialEnable(u8 port, u32 config)
{
	U1STASET = config;
}

/*	----------------------------------------------------------------------------
	SerialSetLineControl
	----------------------------------------------------------------------------
	ex : SerialSetLineControl(UART1, UART_ENABLE | UART_ENABLE_PINS_TX_RX_ONLY | UART_8_BITS_NO_PARITY | UART_STOP_BITS_1)
	--------------------------------------------------------------------------*/

void SerialSetLineControl(u8 port, u32 config)
{
	U1MODESET = config;
}

/*	----------------------------------------------------------------------------
	SerialPinConfigure : UART I/O pins control
	--------------------------------------------------------------------------*/

void  SerialPinConfigure(u8 port)
{
	TRISBbits.TRISB4 = OUTPUT;	// RB4 / U1TX output
	TRISAbits.TRISA4 = INPUT;	// RA4 / U1RX input
}

	// INTERRUPT CONFIG MODE
	#define INT_SYSTEM_CONFIG_MULT_VECTOR	1
	#define INT_SYSTEM_CONFIG_SINGLE_VECTOR 2


/*	----------------------------------------------------------------------------
	IntConfigureSystem
	----------------------------------------------------------------------------
	Configures the system for  multi-vector or single vectored interrupts.
	This routine configures the core to receive interrupt requests and configures the 
	Interrupt module for Multi-vectored or Single Vectored mode.
	Parameters:
		config      - The interrupt configuration to set.
		IntConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
		IntConfigureSystem(INT_SYSTEM_CONFIG_SINGLE_VECTOR);
	NB : Place EBASE at 0xBD000000 (microchip) or 0xBD005000 (jean-pierre mandon)
	--------------------------------------------------------------------------*/

void MIPS32 IntConfigureSystem(u8 mode)
{
	unsigned int temp;

	asm("di"); // Disable all interrupts
	temp = _CP0_GET_STATUS(); // Get Status
	temp |= 0x00400000; // Set BEV bit
	_CP0_SET_STATUS(temp); // Update Status

	_CP0_SET_EBASE(_ebase_address);	// from Linker script

	_CP0_SET_INTCTL(0x00000020); // Set the Vector Spacing to non-zero value

	temp = _CP0_GET_CAUSE(); // Get Cause
	temp |= 0x00800000; // Set IV
	_CP0_SET_CAUSE(temp); // Update Cause

	temp = _CP0_GET_STATUS(); // Get Status
	temp &= 0xFFBFFFFD; // Clear BEV and EXL
	_CP0_SET_STATUS(temp); // Update Status

	switch (mode) {
		case INT_SYSTEM_CONFIG_MULT_VECTOR:
			// Set the CP0 registers for multi-vector interrupt
			INTCONSET = 0x1000; // Set MVEC bit
			break;
		case INT_SYSTEM_CONFIG_SINGLE_VECTOR:
			// Set the CP0 registers for single-vector interrupt
			INTCONCLR = 0x1000; // Clear MVEC bit
			break;	
	}

	asm("ei"); // Enable all interrupts
}



/*	----------------------------------------------------------------------------
	SerialIntConfigure() : Serial Interrupts Configuration
	----------------------------------------------------------------------------
	@param		port		1 (UART1), 2 (UART2) or 3 (UART3)
	@param		priority
	@param		subpriority
	@return		
	--------------------------------------------------------------------------*/

void  SerialIntConfigure(u8 port, u8 priority, u8 subpriority)
{
	IntConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
	if(port==1) {
		IPC8bits.U1IP = priority;
		IPC8bits.U1IS = subpriority;
		IEC1bits.U1RXIE=1;
	}else{
		IPC9bits.U2IP = priority;
		IPC9bits.U2IS = subpriority;
		IEC1bits.U2RXIE=1;
	}
}

//
//	近い値を得る除算.
//
//	pbc  = 5000000  (5MHz)
//	baud =  230400とか.
//
int near_div(int pbc , int baud)
{
//	rc = pbc / baud;
	int rc = (pbc + baud/2) / baud;	//四捨五入的な.
	return rc;
}

void SerialSetDataRate(u8 port,u32 baudrate)
{
	u8 speed;
	u32 max, max1, max2;
	u32 min1, min2;
	u32 pbclock,pbc;

	pbclock = GetPeripheralClock();
	max1 = pbclock / 4;
	min1 = max1 / 65536;
	max2 = pbclock / 16;
	min2 = max2 / 65536;
	if (baudrate > max1) baudrate = max1;
	if (baudrate < min2) baudrate = min2;
	max = (min1 + max2) / 2;
	if (baudrate > max && baudrate < max1) speed = UART_ENABLE_HIGH_SPEED;

	if (speed == UART_ENABLE_HIGH_SPEED) {
//		U1MODEbits.BRGH = UART_ENABLE_HIGH_SPEED;
		pbc = pbclock/4;
	}else{
		pbc = pbclock/16;
	}
	int UxBRG = (near_div(pbc , baudrate)) - 1;

	if(port==1) {
        U1BRG = UxBRG;
        U1MODEbits.BRGH = speed;
	}else{	//	UART2
        U2BRG = UxBRG;
        U2MODEbits.BRGH = speed;
	}
}


#if	0
/*	----------------------------------------------------------------------------
	SerialConfigure()
	----------------------------------------------------------------------------
	@param		port		1 (UART1) or 2 (UART2) or 3 (UART3) ...
	@param		baudrate	baud rate
	@return		baudrate
	--------------------------------------------------------------------------*/

void  SerialConfigure(u8 port, u32 config, u32 enable, u32 baudrate)
{
	// single channel only.
	FIFO_init( &rx_fifo , rx_buff , FIFO_SIZE );	//初期化.

	SerialPinConfigure(port);
	SerialSetDataRate(port, baudrate);		// UxBRG
	SerialSetLineControl(port, config);		// UxMODE
	SerialEnable(port, enable);				// UxSTA
	SerialIntConfigure(port, INT_PRIORITY_3, INT_SUBPRIORITY_3);
}
#endif

/*	----------------------------------------------------------------------------
	SerialWriteChar1 : write data bits 0-8 on the UART1
	--------------------------------------------------------------------------*/

void _MIPS32 Serial1WriteChar(char c)
{
	while (!U1STAbits.TRMT);				// wait transmitter is ready

	asm("di"); // Disable all interrupts
	U1TXREG = c;
	asm("ei"); // Enable all interrupts
}

void _MIPS32 Serial2WriteChar(char c)
{
	while (!U2STAbits.TRMT);				// wait transmitter is ready

	asm("di"); // Disable all interrupts
	U2TXREG = c;
	asm("ei"); // Enable all interrupts
}

/*	----------------------------------------------------------------------------
	SerialAvailable
	--------------------------------------------------------------------------*/

int  SerialAvailable()
{
	return FIFO_getsize( &rx_fifo );
}

/*	----------------------------------------------------------------------------
	SerialRead : Get char
	--------------------------------------------------------------------------*/

int  SerialRead()
{
	uchar c = 0;

	if (SerialAvailable()) {
		FIFO_dequeue( &rx_fifo , &c, 1);
		return(c);
	}
	return(-1);
}

/*	----------------------------------------------------------------------------
	SerialGetKey
	--------------------------------------------------------------------------*/

int  SerialGetKey()
{
	int c;
	while (!(SerialAvailable())) {
//		led_blink();
	}
	c = SerialRead();
	return (c);
}


/********************************************************************
 *
 ********************************************************************
 */
void UARTwrite(uchar *buf,int len)
{
	int i;
	for(i=0;i<len;i++) {
		Serial2WriteChar(*buf++);
	}
}
/********************************************************************
 *
 ********************************************************************
 */
int	UARTgetpacket(char *buf,int size)
{
	if(SerialAvailable() >= size) {
		FIFO_dequeue( &rx_fifo , buf, size);
		return size;
	}
	return 0;
}


/*	----------------------------------------------------------------------------
	SerialInterrupt
	TODO: move this to interrupt library and add it to main32.c
	--------------------------------------------------------------------------*/

#if	0
// vector 24 or 32 (PIC32_PINGUINO_220)
void  Serial1Interrupt(void)
{
	// Is this an RX interrupt from UART1 ?
	if (IFS1bits.U1RXIF) {
		char c = U1RXREG;			// read received char
		FIFO_enqueue( &rx_fifo, &c, 1);
		IFS1bits.U1RXIF=0;
	}
	// Is this an TX interrupt from UART1 ?
	if (IFS1bits.U1TXIF) {
		IFS1bits.U1TXIF=0;
	}
	// Is this an ERROR interrupt from UART1 ?
	if (IFS1bits.U1EIF)	{
		IFS1bits.U1EIF=0;
	}
}
//IFS0CLR = UART1_ALL_INTERRUPT;			// clear any existing event
#else
void  Serial2Interrupt(void)
{
	// Is this an RX interrupt from UART1 ?
	if (IFS1bits.U2RXIF) {
		char c = U2RXREG;			// read received char
		FIFO_enqueue( &rx_fifo, &c, 1);
		IFS1bits.U2RXIF=0;
	}
	// Is this an TX interrupt from UART1 ?
	if (IFS1bits.U2TXIF) {
		IFS1bits.U2TXIF=0;
	}
	// Is this an ERROR interrupt from UART1 ?
	if (IFS1bits.U2EIF)	{
		IFS1bits.U2EIF=0;
	}
	led_blink();
}

#endif


/*	----------------------------------------------------------------------------
	以下、uart2.c からインポート
	--------------------------------------------------------------------------*/

    #define BAUDRATE2       57600UL
    #define BRG_DIV2        4 
    #define BRGH2           1 
    #define BAUDRATEREG2        ((GetPeripheralClock()+(BRG_DIV2/2*BAUDRATE2))/BRG_DIV2/BAUDRATE2-1)

//	全てデジタルI/Oにする.
static	void IOsetDigital()
{
	DDPCONbits.JTAGEN=0;		// check : already in system.c
	ANSELA = 0;
	ANSELB = 0;
}

//	PinguinoX.3はMX220のTX/RXのPORT DIRECTIONを設定していない.
//	X.4では修正されている.
static	void PORT_DIR_init2()
{
	TRISBbits.TRISB9 = 0;	//OUTPUT;	// RB4 / U2TX output
	TRISBbits.TRISB8 = 1;	//INPUT;	// RA4 / U2RX input
}

//	Remapレジスタ操作を許可.
static	void SystemUnlock()
{
	SYSKEY = 0;				// ensure OSCCON is locked
	SYSKEY = 0xAA996655;	// Write Key1 to SYSKEY
	SYSKEY = 0x556699AA;	// Write Key2 to SYSKEY
}

//	Remapレジスタ操作を禁止.
static	void SystemLock()
{
	SYSKEY = 0x12345678;	// Write any value other than Key1 or Key2
}

//	Remapレジスタ操作の実行.
static  void IO_Remap2()
{
	SystemUnlock();
	CFGCONbits.IOLOCK=0;			// unlock configuration
	CFGCONbits.PMDLOCK=0;
		U2RXRbits.U2RXR=4;			// Define U2RX as RB8 ( UEXT SERIAL )
		RPB9Rbits.RPB9R=2;			// Define U2TX as RB9 ( UEXT SERIAL )
	CFGCONbits.IOLOCK=1;			// relock configuration
	CFGCONbits.PMDLOCK=1;	
	SystemLock();
}

//	UART2 の初期化.
void UART2Init()
{
	IOsetDigital();
	IO_Remap2();
	PORT_DIR_init2();

    U2BRG = BAUDRATEREG2;
    U2MODE = 0;
    U2MODEbits.BRGH = BRGH2;
    U2STA = 0;
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;
    IFS1bits.U2RXIF = 0;
    
    U2STAbits.URXEN = 1;

	// FIFO初期化
	FIFO_init( &rx_fifo , rx_buff , FIFO_SIZE );	//初期化.
	// 割り込み初期化
	SerialIntConfigure(2, INT_PRIORITY_3, INT_SUBPRIORITY_3);
}

char UART2IsPressed()
{
#if	1
	return SerialAvailable();
#else
    if(IFS1bits.U2RXIF == 1)
        return 1;
    return 0;
#endif
}

void UART2PutChar( char ch )
{
#if	1
	Serial2WriteChar(ch);
#else
    U2TXREG = ch;
    while(U2STAbits.TRMT == 0);
#endif
}

char UART2GetChar()
{
#if	1
	return SerialGetKey();
#else
    char Temp;

    while(IFS1bits.U2RXIF == 0);

    Temp = U2RXREG;
    IFS1bits.U2RXIF = 0;
    return Temp;
#endif
}

