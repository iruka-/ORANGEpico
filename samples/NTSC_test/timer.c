/*
 ************************************************************************
void	NTSC_init(void);
void	init_timer1(void);
void	init_timer2(void);
 ************************************************************************
*/
#include <p32xxxx.h>			// always in first place to avoid conflict with const.h ON
#include "spi2.h"

#if	SVGA_MODE
#define	HSYNC_PR	(528*2-1)		// (26.4uS * 20MHz) = 528
//	注意：奇数にしないと、SPIクロックとの干渉(1dotずれ)によりギザギザが出ます。
//	kats_me様、ご指摘ありがとうございました。
//	（ここを奇数にすると、実際の周期は偶数clockになります）

#else
#define	HSYNC_PR	(1525*2-3)		// (63.557uS * 24MHz) = 1525.35
//262.5本 - 21 = 241.5
// 240
// 6
// 6
// 6
#endif

//	デバッグ用のグローバルカウンタ・タップ
volatile int 	g_Timer1=0;
volatile int 	g_Timer2=0;
volatile short 	g_Hcount=0;

//	ジッター軽減処理：
//	先行割り込みタイマー起床.
#define	TIMER2_PR		HSYNC_PR-119	//119はTIMER1割り込みからTIMER2 KICKまでの必要クロック.


#define	ANTI_JITTER		(1)	//ジッター対策.

void wait_125ns(int n);
void clear_dmabuf();
void dma_kick(int cnt);
void dma_clearbuf(void);
void font_draw(int line);



/**********************************************************************
 *	TIMER1はHSYNC割り込み.
 **********************************************************************
 */
void init_timer1(void)
{
	T1CON=0;			// reset timer 1 configuration
	TMR1=0;				// reset timer 1 counter register
	PR1=HSYNC_PR;		// define the preload register
	IPC1SET=(7<<2);		// select interrupt priority and sub-priority
	IFS0CLR=(1<<4);		// clear interrupt flag
	IEC0SET=(1<<4);		// enable timer 1 interrupt
	T1CON=0x8000;		// start timer 1 and set prescaler(1/1)
}


/**********************************************************************
 *	TIMER2は次回割り込み準備に使用
 **********************************************************************
 */
void init_timer2(void)
{
	T2CON=0;			// reset timer 2 configuration
	TMR2=0;				// reset timer 2 counter register
	PR2=TIMER2_PR;		// define the preload register
	IPC2SET=(5<<2);		// select interrupt priority and sub-priority
	IFS0CLR=(1<<9);		// clear interrupt flag
	IEC0SET=(1<<9);		// enable timer 2 interrupt
//	T2CON=0x8000;		// start timer 2 and set prescaler(1/1)
}

/**********************************************************************
 *	TIMER2は、TIMER1割り込み直後に起動させる.
 **********************************************************************
 */
#define	KICK_TIMER2()	\
	TMR2=0;				\
	T2CON=0x8000;		\


/*
VGA
  13---HSYNC--RB0
  14---VSYNC--RB1
  GREEN-------RB2
*/

//#define	HSYNC	0	// PB0
//#define	VSYNC	1	// PB1
//#define	GREEN	2	// PB2

#define	HSYNC	0	// PB0
//#define	VSYNC	1	// PB1
#define	GREEN	13	// PB13


#define	setHIGH(n)	LATBSET=(1<<(n))
#define	setLOW(n)	LATBCLR=(1<<(n))

/**********************************************************************
 *	RB0,RB13を出力ポートにする.
 **********************************************************************
 */
void init_vga_port()
{
	TRISBCLR=0x2001;
	LATBSET =0x2001;
}

/**********************************************************************
 *
 **********************************************************************
 */
void NTSC_init(void)
{
	init_vga_port();
#if	USE_SPI2
	SPI2_init();
	dma_init();
#endif

#if	DDS_TEST
	//
	//
#else
	init_timer1();
	init_timer2();
#endif
}

/*
One field
  2 lines front porch
  2 lines vertical sync
 25 lines back porch
  8 lines top border
480 lines video
  8 lines bottom border
---
525 lines total per field              
 */
#if	SVGA_MODE

/*
Visible area 600 15.84 
Front porch 1 0.0264 
Sync pulse 4 0.1056 
Back porch 23 0.6072 
Whole frame 628 16.5792 
*/

inline	void gen_VSYNC()
{
	g_Hcount++;

	if(	g_Hcount < (600-8) ) {		// 37 * 16 = 592
#if	USE_SPI2
#if	USE_DMA
			dma_kick(g_Hcount);
#endif
#else
		if((g_Hcount & 0x0f) >=8 ) {
			setHIGH(GREEN);
		}else{
			setLOW(GREEN);
		}
#endif
	}else{
		if(	g_Hcount==(600)) {			//600+1
			setHIGH(VSYNC);
		}

		if(	g_Hcount==(600+4) ) {		//600+1+4
			setLOW(VSYNC);
		}

		if(	g_Hcount>=(628) ) {
			g_Hcount=0;
			dma_clear();
		}
	}
}
#else

// HH1 = 縦の表示dot数
#define	HH1  HEIGHT
// HH2 = 上下の余白dot ＝ (240 - 縦の表示dot数)/2
#define	HH2 (240-HEIGHT)/2
//
//	VGA @60Hz mode
//
inline void gen_VSYNC()
{
	g_Hcount++;
	if(	g_Hcount < HH1 ) {
#if	USE_SPI2
#if	USE_DMA
			dma_kick(g_Hcount);
#endif
#else
		if((g_Hcount & 0x0f) >=8 ) {
			setHIGH(GREEN);
		}else{
			setLOW(GREEN);
		}
#endif
	}else{
		// 垂直帰線を作る.
		if((g_Hcount>=(HH1+HH2+6) )&&(g_Hcount< (HH1+HH2+12))) {
			setLOW(GREEN);
			setLOW(HSYNC);
		}

		if(	g_Hcount>=262) {
			g_Hcount=0;
		}
	}
}
#endif
/**********************************************************************
 *
 **********************************************************************
 */
void __attribute__((interrupt,nomips16,noinline)) _Tmr1Interrupt(void)
{
	if (IFS0bits.T1IF) {	// Timer Interrupt flag
		IFS0CLR=(1<<4);	// IFS04: Clear the timer interrupt flag
#if	ANTI_JITTER		//ジッター対策あり.
		KICK_TIMER2();
#endif

#if	SVGA_MODE
		setHIGH(HSYNC);
		wait_125ns(3*8);	// 3.2uS
		setLOW(HSYNC);
//		wait_125ns(1);		// porch
#else
		setLOW(GREEN);
		setLOW(HSYNC);
		wait_125ns(5*8);	// 4.7uS
		setHIGH(HSYNC);
		wait_125ns(4);		// porch
#endif
		gen_VSYNC();
		g_Timer1++;		// increment the counter
	}
}
/**********************************************************************
 *
 **********************************************************************
 */
void __attribute__((interrupt,nomips16,noinline)) _Tmr2Interrupt(void)
{
	int i;
	if (IFS0bits.T2IF) {	// Timer Interrupt flag
		IFS0CLR=(1<<9);		// IFS09: Clear the timer interrupt flag
		T2CON=0;			// Stop TIMER2
//		g_Timer2++;			// increment the counter
#if	1
		asm("ei $2");
#if	1
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
//		for(i=0;i<8;i++) {
			asm("nop");
//		}
#endif
#endif
	}
}
/**********************************************************************
 *
 **********************************************************************
 */
