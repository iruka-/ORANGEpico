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
//#include <typedef.h>
//#include <const.h>

#include "config.h"
#include "serial1.h"
#include "fifo.h"

#define	_BOOTROM_	/**/
//#define	_BOOTROM_  __attribute__((section(".bootrom")))


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

static	FIFO	rx_fifo;
static	char	rx_buff[FIFO_SIZE];

//
//	コードサイズ削減のために決め打ちにする.
//
int	get_PeripheralClock()
{
	return SYS_FREQ;			// config.h
//	return 40 * 1000 * 1000;	// 40MHz
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
/*	----------------------------------------------------------------------------
	SerialSetDataRate()
	----------------------------------------------------------------------------
	@param		port		1 (UART1) or 2 (UART2)
	@param		baudrate	baud rate
	@return		baudrate
	----------------------------------------------------------------------------
	BRGH: High Baud Rate Enable bit
	if BRGH = 1 = High-Speed mode - 4x baud clock enabled
		then UxBRG = ((FPB/Desired Baud Rate)/ 4) - 1
	if BRGH 0 = Standard Speed mode - 16x baud clock enabled
		then UxBRG = ((FPB/Desired Baud Rate)/16) - 1
	--------------------------------------------------------------------------*/

void _BOOTROM_ SerialSetDataRate(u8 port, u32 baudrate)
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
		U1MODEbits.BRGH = UART_ENABLE_HIGH_SPEED;
		pbc = pbclock/4;
	}else{
		pbc = pbclock/16;
	}
	U1BRG = (near_div(pbc , baudrate)) - 1;
}

/*	----------------------------------------------------------------------------
	SerialEnable
	----------------------------------------------------------------------------
	ex : SerialEnable(UART1, UART_RX_TX_ENABLED | UART_INTERRUPT_ON_RX_FULL)
	--------------------------------------------------------------------------*/

static	void _BOOTROM_ SerialEnable(u8 port, u32 config)
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

void _BOOTROM_ SerialPinConfigure(u8 port)
{
	TRISBbits.TRISB4 = OUTPUT;	// RB4 / U1TX output
	TRISAbits.TRISA4 = INPUT;	// RA4 / U1RX input
}

/*	----------------------------------------------------------------------------
	SerialIntConfigure() : Serial Interrupts Configuration
	----------------------------------------------------------------------------
	@param		port		1 (UART1), 2 (UART2) or 3 (UART3)
	@param		baudrate	baud rate
	@return		baudrate
	--------------------------------------------------------------------------*/
#define INT_SYSTEM_CONFIG_MULT_VECTOR	1

void _BOOTROM_ SerialIntConfigure(u8 port, u8 priority, u8 subpriority)
{
//	IntConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
	
	IPC8bits.U1IP = priority;
	IPC8bits.U1IS = subpriority;
	IEC1bits.U1RXIE=1;
}

/*	----------------------------------------------------------------------------
	SerialConfigure()
	----------------------------------------------------------------------------
	@param		port		1 (UART1) or 2 (UART2) or 3 (UART3) ...
	@param		baudrate	baud rate
	@return		baudrate
	--------------------------------------------------------------------------*/

void _BOOTROM_ SerialConfigure(u8 port, u32 config, u32 enable, u32 baudrate)
{
	// single channel only.
	FIFO_init( &rx_fifo , rx_buff , FIFO_SIZE );	//初期化.

	SerialPinConfigure(port);
	SerialSetDataRate(port, baudrate);		// UxBRG
	SerialSetLineControl(port, config);		// UxMODE
	SerialEnable(port, enable);				// UxSTA
	SerialIntConfigure(port, INT_PRIORITY_3, INT_SUBPRIORITY_3);
}


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

/*	----------------------------------------------------------------------------
	SerialAvailable
	--------------------------------------------------------------------------*/

int _BOOTROM_ Serial1Available()
{
	return FIFO_getsize( &rx_fifo );
}

/*	----------------------------------------------------------------------------
	SerialRead : Get char
	--------------------------------------------------------------------------*/

int _BOOTROM_ Serial1Read()
{
	uchar c = 0;

	if (Serial1Available()) {
		FIFO_dequeue( &rx_fifo , &c, 1);
		return(c);
	}
	return(-1);
}

/*	----------------------------------------------------------------------------
	SerialGetKey
	--------------------------------------------------------------------------*/

int _BOOTROM_ Serial1GetKey()
{
	int c;
	while (!(Serial1Available())) {
		led_blink();
	}
	c = Serial1Read();
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
		Serial1WriteChar(*buf++);
	}
}
/********************************************************************
 *
 ********************************************************************
 */
void Serial1WriteString(uchar *buf)
{
	while(*buf) {
		Serial1WriteChar(*buf++);
	}
}
/********************************************************************
 *
 ********************************************************************
 */
int	UARTgetpacket(char *buf,int size)
{
	if(Serial1Available() >= size) {
		FIFO_dequeue( &rx_fifo , buf, size);
		return size;
	}
	return 0;
}


/*	----------------------------------------------------------------------------
	SerialInterrupt
	TODO: move this to interrupt library and add it to main32.c
	--------------------------------------------------------------------------*/

// vector 24 or 32 (PIC32_PINGUINO_220)
void _BOOTROM_ Serial1Interrupt(void)
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

