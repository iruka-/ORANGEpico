#define MAIN
#include "main.h"
#include "cgrom.h"

/*
	Raster frequency is 15734 hz, so one raster takes 63.56 usecs.
	Raster frequency cange between 15734-15750 hz (15750 hz was used for B/W TV)
	15734 hz corresponds to 3050.72 clocks of 48 Mhz CPU frequency.
	Use 3048 clocks, resulting in 15748 hz
	Note that 508 clocks of 8 MHz (=SPI clock speed) also produces the same frequency.
*/

/*	Minimum time between two interruption events is 2.3 usecs,
	that corresponds to 92 colcks.
*/

#define sync LATBbits.LATB13
#define toggleSync() LATBINV=0x2000


void initNtsc()
{
	int i;
	for (i=0;i<1024;i++) {
		VRAM[i] = i;
	}
	// RB13 and RA4 are output pins
	TRISAbits.TRISA4=0;
	TRISBbits.TRISB13=0;

	// Timer2 is used for sync signal
	// On, Stop in Idle Mode, PBCLK, 1:1 prescale, 16-bit mode
	// Enable interrupt as priority 7
	TMR2=0;
	PR2=0xFFFF;
	T2CON=0xA000;
	T2CONbits.ON=1;
	IPC2bits.T2IP=7;
	IPC2bits.T2IS=0;
	IFS0bits.T2IF=0;
	IEC0bits.T2IE=1;

return;

	// Timer4 is used for video signal
	// Off, Stop in Idle Mode, PBCLK, 1:2 prescale, 16-bit mode
	// Enable interrupt as priority 6
	TMR4=0;
	PR4=0xFFFF;
	T4CON=0x2010;
	IPC4bits.T4IP=6;
	IPC4bits.T4IS=0;
	IFS0bits.T4IF=0;
	IEC0bits.T4IE=1;

	// SPI2 module settings follow
	// PBCLOCK, Enable SPI2, Stop in Idle Mode, Enable SDO, 32 bit mode,
	// SS pin not used, Master mode, Disable SDI
	// SPI clock=REFCLK/(2x(SPI2BRG+1))=16 Mhz. (REFCLK=96 Mhz; SPI2BRG=2)
	// All extended setting are off (SPI2CON2).

	SPI2CON=0x0100A83C;
	SPI2CON2=0x0300;
	SPI2BRG=2;
	// Output SDO2 to RA4
	RPA4R=0x04;
}


/*	ntscSyncInt() is an interrupt handler
	Interrupt occur via Timer2 (vector 8)
*/
//#pragma interrupt ntscSyncInt IPL7SOFT vector 8

/*	ntscVideoInt() is an interrupt handler
	Interrupt occur via Timer4 (vector 16)
*/
//#pragma interrupt ntscVideoInt IPL6SOFT vector 16

/*	Sync signals:
	raster 1-3: L for 2.3 usecs, H for 29.5 usec, L for 2.3 usecs, H for 29.5 usec
	(110)         (110)
	2.3u__________2.3u__________                 110+1414+110+1414=3048
	|__|          |__|          
	       29.5u         29.5u
          (1414)        (1414)

	raster 4-6: L for 27.1 usecs, H for 4.7 usec, L for 27.1 usecs, H for 4.7 usec
      (1299)        (1299)
	   27.1u    __   27.1u    __                 1299+225+1299+225=3048
	|__________|  |__________|  
	           4.7u          4.7u
              (225)         (225)

	raster 7-9: L for 2.3 usecs, H for 29.5 usec, L for 2.3 usecs, H for 29.5 usec
	(110)         (110)
	2.3u__________2.3u__________                 110+1414+110+1414=3048
	|__|          |__|          
	       29.5u         29.5u
          (1414)        (1414)

	raster 10-262: H for 1.5 usecs, L for 4.7 usecs, H for 57.4 usecs
      (225)
	___4.7u_____________________                 72+225+2751=3048
	   |__|          
	1.5u          57.4u
    (72)         (2751)

	This must produces 16275 Hz sync signal.
	Following intterupt function takes about 1 micro second.
	This will be 0.5 micro second if IPL7SRS will be used.
*/
void ntscSyncInt(){
	static unsigned long status=0;
	unsigned int i16;
	IFS0CLR=_IFS0_T2IF_MASK; //IFS0bits.T2IF=0;
	toggleSync();
	PR2=i16=ntscSyncTiming[status++];
	if (i16==(2751-1)) {
		// The end of the sequence of statuses.
		// Reset parameter.
		sync=1;
		status=0;
		// Trigger Timer4 for video signal
		// Note that video signal starts from raster 42.
		// Also note that timer for video works at 20 Mhz.
		TMR4=TMR2>>1;
		PR4=(2751+3048*41+72+225)/2+40-1; // Needs to be tuned to define horizontal positoin.
		T4CONSET=_T4CON_ON_MASK; //T4CONbits.ON=1;
		g_Vsync=0; // Pseudo vertical sync signal starts here.
	}
}
/*	Video signal construction
	1 raster: 1525 cycles of timer working at 24 Mhz (24 clocks/usec; 24 clocks/8 dotes)
	status:  1    2    3    4    5    6    7 
	         |    |    |    |    |    |    |
              (113)
	       ___4.7u___________________________
	          |__|          
	       1.5u            57.4u
           (30)           (1381)
	// This routine takes about 2 micro seconds.
	// This will take about 1 micro second if IPL7SRS will be used.
*/
void ntscVideoInt(){
	static unsigned long status=1;
	static unsigned long addr=0;
	static unsigned long fontLine=0;
	unsigned long i1,i2,i3,i4;
	static unsigned char* cgrom_line;
	IFS0CLR=_IFS0_T4IF_MASK; //IFS0bits.T4IF=0;
	switch(status){
		case 1:
			status=2;
			g_Vsync=1; // Pseudo vertical sync signal ends here.
			if (!g_ntscVisible) {
				T4CONCLR=_T4CON_ON_MASK; //T4CONbits.ON=0;
			} else {
				PR4=190; // Needs to be tuned (must be less than 192, more than 170.6).
				cgrom_line=&cgrom[fontLine];
			}
			break;
		case 7:
			status=1;
			if (fontLine<7*256) {
				fontLine+=256;
				addr-=40;
			} else {
				fontLine=0;
				if (999<addr) {
					// Video signal was already end. Let's stop timer
					T4CONCLR=_T4CON_ON_MASK; //T4CONbits.ON=0;
					// Reset parameters
					addr=0;
					fontLine=0;
					break;
				}
			}
			PR4=1524-(190+1)*6-1;
			break;
		default:
			status++;
			i1=(cgrom_line[VRAM[addr+0]])<<24;
			i2=(cgrom_line[VRAM[addr+1]])<<16;
			i3=(cgrom_line[VRAM[addr+2]])<<8;
			i4=(cgrom_line[VRAM[addr+3]]);
			SPI2BUF=i1|i2|i3|i4;
			addr+=4;
			i1=(cgrom_line[VRAM[addr+0]])<<24;
			i2=(cgrom_line[VRAM[addr+1]])<<16;
			i3=(cgrom_line[VRAM[addr+2]])<<8;
			i4=(cgrom_line[VRAM[addr+3]]);
			SPI2BUF=i1|i2|i3|i4;
			addr+=4;
			break;
	}
}
#endif

#define _TIMER_1_VECTOR                          4
#define _TIMER_2_VECTOR                          8
#define _TIMER_3_VECTOR                          12
#define _TIMER_4_VECTOR                          16

void __attribute__((interrupt,nomips16,noinline)) _Tmr1Interrupt(void)
{
	if (IFS0bits.T1IF) {	// Timer Interrupt flag
		TMR1=0;				// reset the timer register
		IFS0CLR= (1<<4);	// IFS04: Clear the timer interrupt flag
	}
}

void __attribute__((interrupt,nomips16,noinline)) _Tmr2Interrupt(void)
{
	if (IFS0bits.T2IF) {	// Timer Interrupt flag
		TMR2=0;
		IFS0CLR= (1<<8);	// IFS04: Clear the timer interrupt flag
	}
}

void __attribute__((interrupt,nomips16,noinline)) _Tmr4Interrupt(void)
{
	if (IFS0bits.T4IF) {	// Timer Interrupt flag
		IFS0CLR= (1<<16);	// IFS04: Clear the timer interrupt flag
	}
}

#if	0
void timer1Int(){
	// This occurs every 1.3653 msec (732 Hz).
	IFS0CLR=_IFS0_T1IF_MASK; //IFS0bits.T1IF=0;
	g_TMR1h+=0x10000;
	if (g_TMR1h==0) g_timer1&=0xffff;

	// Environmental supports follow.

	// Two 555 timers for tempo and prompt
	if (!((++tempo_8255)&0x7)) {
		read825x[8]^=1; // tempo
	}
	if (!((++prompt_8255)&0xff)) {
		read825x[2]^=0x40; // prompt
	}

	// Emulate 8253 timer 2
	if (IFS0bits.T5IF) inc8253timer2();

	// Vgate support
	// Video off when VGATE=0 for more than 20 msec.
	if (read825x[2]&0x01) {
		g_Vgate=15;
	} else if (g_Vgate) {
		g_Vgate--;
	}

	// PS2 key input
	if (0x200000<ps2status) {
		if ((ps2data&0x200800)==0x200000) {
			// Parity check is not done.
			ps2_received((ps2data>>12)&0xff);			
		}
		ps2status=0x800;
		ps2data&=0x7ff;
	}
	if (ps2status==0x800) {
		if ((ps2data&0x401)==0x400) {
			// Parity check is not done.
			ps2_received((ps2data>>1)&0xff);
		}
		ps2status=1;
		ps2data=0;
	}
}
#endif
