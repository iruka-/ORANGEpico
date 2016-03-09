/********************************************************************
 *	PIC32MX SPI2 (Serial Peripheral Interface) for mx220f032b
 ********************************************************************
 */

/********************************************************************
関数一覧:
	void	SPI2_init();						初期化
	void	SPI2_mode(uchar mode);				モード設定
	void	SPI2_clock(unsigned int speed);		クロック設定
	void	SPI2_close() //(u8 num);				終了

unsigned int SPI2_write(unsigned int data_out);	32bit送信
unsigned int SPI2_read(void);					32bit受信
void  _ISR  _SPI2Interrupt(void);				割り込みハンドラー

割り込み許可など
unsigned int IntGetFlag(u8 numinter);
	void	IntEnable(u8 numinter);
	void	IntDisable(u8 numinter);
	void	IntClearFlag(u8 numinter);
 ********************************************************************
 */
#include <p32xxxx.h>			// always in first place to avoid conflict with const.h ON
//#include "interrupt.h"
	#define INT_SPI2_FAULT					37 
	#define INT_SPI2_TRANSFER_DONE			38 
	#define INT_SPI2_RECEIVE_DONE			39
//    #define GetPeripheralClock()        GetSystemClock()


#include "config.h"
#include "spi2.h"

#define SPIBUFFER	SPI2BUF
#define STATUS		SPI2STATbits.SPIROV	// Receive Overflow Flag bit
#define STATRX  	SPI2STATbits.SPIRBF	// Receive buffer full
#define STATTX		SPI2STATbits.SPITBF	// Transmit buffer full
#define SPICONF		SPI2CON
#define SPICONCLR	SPI2CONCLR
#define SPIENHBUF	SPI2CONbits.ENHBUF
#define CLKSPD		SPI2BRG
#define INTFAULT	INT_SPI2_FAULT
#define INTTXDONE 	52 	//INT_SPI2_TRANSFER_DONE
#define INTRXDONE 	INT_SPI2_RECEIVE_DONE
#define INTVECTOR 	36	//INT_SPI2_VECTOR

// SPIxCON.MSTEN
#define SPI2_MASTER			1
#define SPI2_SLAVE			0
#define SPI2_PBCLOCK_DIV2	2
#define SPI2_PBCLOCK_DIV4	4
#define SPI2_PBCLOCK_DIV8	8
#define SPI2_PBCLOCK_DIV16	16
#define SPI2_PBCLOCK_DIV32	32
#define SPI2_PBCLOCK_DIV64	64

void IntDisable(int x);

/********************************************************************
 *	
 ********************************************************************
 */
void SPI2_mode(uchar mode)
{
	STATUS = 0;					// clear the Overflow
	if (mode == SPI2_MASTER) {
		SPICONF=0x8824;		// 32 bits transfer, Master mode SDI disable
	}
	if (mode == SPI2_SLAVE) {
		SPICONF=0x8000;		// SPI ON, 8 bits transfer, Slave mode
	}
	SPI2CON2=0x300;	//AUDEN=1 (32bit)
}
/********************************************************************
 *	
 ********************************************************************
 */
// Fsck = Fpb / (2 * (SPIxBRG + 1)
// SPIxBRG = (Fpb / (2 * Fsck)) - 1 
// speed must be in bauds
void SPI2_clock(unsigned int speed)
{
	unsigned int Fpb;
	unsigned short clk;

	Fpb = GetPeripheralClock();
	if (speed > (Fpb / 2)){
		CLKSPD = 0;			// use the maximum baud rate possible
		return;
	}else{
		clk = (Fpb / (2 * speed)) - 1;
		if (clk > 511)
			CLKSPD = 511;	// use the minimum baud rate possible
		else					// ** fix for bug identified by dk=KiloOne
			CLKSPD = clk;	// use calculated divider-baud rate
	}
}

/********************************************************************
 *	
 ********************************************************************
 */
void SPI2_close() //(u8 num)
{
	unsigned char rData;
	IntDisable(INTFAULT); 
	IntDisable(INTTXDONE); 
	IntDisable(INTRXDONE);
	SPICONF=0;
	rData=SPIBUFFER;
	(void)rData;
}

/********************************************************************
 *	
 ********************************************************************
 */
void SPI2_init()
{
	unsigned char rData;
	SPICONCLR = 0x8000; // bit 15
	rData = SPIBUFFER;
	(void)rData;
#if	USE_SPI2_INTERRUPT
	IntClearFlag(INTTXDONE);
	IntEnable(INTTXDONE); 

//	IntSetVectorPriority(INTVECTOR, 3, 1);
	int pri=7;	//3;
	int sub=1;
			IPC9bits.SPI2IP = pri;
			IPC9bits.SPI2IS = sub;
#endif

//	SPI2_clock(4000000); //  4  M bps div=1:6
//	SPI2_clock(4800000); //  4.8M bps     1:5
	SPI2_clock(6000000); //  6  M bps     1:4
	SPI2_mode(SPI2_MASTER);

#if	1
	RPB13R=0x04;	// PB13=SDO2
	TRISBCLR=1<<13;	// PB13 output.
#else
	RPB2R=0x04;		// PB2=SDO2
	TRISBCLR=1<<2;	// PB2 output.
#endif

}

/********************************************************************
 *	
 ********************************************************************
 */
unsigned int SPI2_write(unsigned int data_out)
{
	SPIBUFFER = data_out;	// write to buffer for TX
	while (!STATRX);		// wait for the receive flag (transfer complete)
	return SPIBUFFER;
}

/********************************************************************
 *	
 ********************************************************************
 */
unsigned int SPI2_read(void)
{
	SPIBUFFER = 0x00;			// dummy byte to capture the response
	while (!STATRX);		// wait until cycle complete
	return SPIBUFFER;			// return with byte read
}


/*********************************************************************
 *
 *********************************************************************
 */
