/*
 ************************************************************************
void	NTSC_init(void);
void	init_timer1(void);
void	init_timer2(void);
void	init_timer3(void);
 ************************************************************************
*/
#include <p32xxxx.h>			// always in first place to avoid conflict with const.h ON
#include "dma1.h"
#include "spi2.h"
#include "ps2keyb.h"

//	HSYNC割り込みタイマー周期.
#define	HSYNC_PR	(1525*2-3)		// (63.557uS * 24MHz) = 1525.35

//	ジッター軽減処理：
//	先行割り込みタイマー起床.
#define	TIMER2_PR		HSYNC_PR-119	//119はTIMER1割り込みからTIMER2 KICKまでの必要クロック.

#define	TIMER3_PR		HSYNC_PR/5
//(528*2)			// (22uS * 24MHz) = 528

#define	ANTI_JITTER		(1)	//ジッター対策あり.



//	デバッグ用のグローバルカウンタ・タップ
volatile int 	g_Timer1=0;
volatile int 	g_Timer2=0;
volatile int 	g_Timer3=0;
//	HSYNC,VSYNCのカウンタ.
volatile short 	g_Hcount=0;
volatile int 	g_Vcount=0;

void wait_125ns(int n);
void clear_dmabuf();
void font_draw(int line);

int	getVcount(int n)
{
	if(n>=0) g_Vcount = n;
	return g_Vcount;
}

/**********************************************************************
 *	TIMER1はHSYNC割り込み.
 **********************************************************************
 */
void init_timer1(void)
{
	int pri=7;
	int sub=0;

	T1CON=0;			// reset timer 1 configuration
	TMR1=0;				// reset timer 1 counter register
	PR1=HSYNC_PR;		// define the preload register
	IFS0bits.T1IF = 0;
	IPC1bits.T1IP = pri;
	IPC1bits.T1IS = sub;
	IEC0SET=(1<<4);		// enable timer 1 interrupt (T1IE)
	T1CON=0x8000;		// start timer 1 and set prescaler(1/1)
}


/**********************************************************************
 *	TIMER2は次回割り込み準備に使用
 **********************************************************************
 */
void init_timer2(void)
{
	int pri=4;
	int sub=0;

	T2CON=0;			// reset timer 2 configuration
	TMR2=0;				// reset timer 2 counter register
	PR2=TIMER2_PR;		// define the preload register
	IFS0bits.T2IF = 0;
	IPC2bits.T2IP = pri;
	IPC2bits.T2IS = sub;
	IEC0SET=(1<<9);		// enable timer 2 interrupt (T2IE)
//	T2CON=0x8000;		// start timer 2 and set prescaler(1/1)
}

/**********************************************************************
 *	TIMER3はPS/2割り込み.
 **********************************************************************
 */
void init_timer3(void)
{
	int pri=4;
	int sub=0;

	T3CON=0;			// reset timer 3 configuration
	TMR3=0;				// reset timer 3 counter register
	PR3=TIMER3_PR;		// define the preload register
	IFS0bits.T3IF = 0;
	IPC3bits.T3IP = pri;
	IPC3bits.T3IS = sub;
	IEC0SET=(1<<14);		// enable timer 3 interrupt (T3IE)
//	T3CON=0x8000;		// start timer 3 and set prescaler(1/1)
}


/**********************************************************************
 *	TIMER2は、TIMER1割り込み直後に起動させる.
 **********************************************************************
 */
#define	KICK_TIMER2()	\
	TMR2=0;				\
	T2CON=0x8000;		\

#define	KICK_TIMER3()	\
	TMR3=0;				\
	T3CON=0x8000;		\

#define	HSYNC		0	// PB0
#define	LUMINANCE	13	// PB13

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
	SPI2_init();
#if	USE_DMA
	dma_init();
#endif
	init_timer1();
	init_timer2();
	init_timer3();
}

// HH1 = 縦の表示dot数
#define	HH1  HEIGHT
// HH2 = 上下の余白dot ＝ (240 - 縦の表示dot数)/2
#define	HH2 (240-HEIGHT)/2
// HH3 = 垂直帰線の開始LINEを遅らせるタイミング(HSYNC数)
#define	HH3  6
//
void _MIPS32 gen_VSYNC()
{
	g_Hcount++;
	if(	g_Hcount < HH1 ) {
#if	USE_DMA
		dma_kick(g_Hcount-1);
#endif
	}else{
		// 垂直帰線を作る.
		if((g_Hcount>=(HH1+HH2+HH3) )&&(g_Hcount< (HH1+HH2+HH3+6))) {
			setLOW(LUMINANCE);
			setLOW(HSYNC);
		}

		if(	g_Hcount>=262) {
			g_Hcount=0;
			g_Vcount++;
		}
	}
}
/**********************************************************************
 *	HSYNC割り込み
 **********************************************************************
 */
void _MIPS32 Timer1Interrupt(void)
{
	if (IFS0bits.T1IF) {	// Timer Interrupt flag
		IFS0CLR=(1<<4);	// IFS04: Clear the timer interrupt flag
#if	ANTI_JITTER		//ジッター対策あり.
		KICK_TIMER2();
		KICK_TIMER3();
#endif
		setLOW(LUMINANCE);
		setLOW(HSYNC);
		wait_125ns(5*8);	// 4.7uS
		setHIGH(HSYNC);
		wait_125ns(5);
		gen_VSYNC();
		g_Timer1++;		// increment the counter
	}
}
/**********************************************************************
 *	HSYNC割り込みに先行して割り込んでNOPを走らせる割り込み.(ジッタ対策)
 **********************************************************************
 */
void _MIPS32 Timer2Interrupt(void)
{
	if (IFS0bits.T2IF) {	// Timer Interrupt flag
		IFS0CLR=(1<<9);		// IFS09: Clear the timer interrupt flag
		T2CON=0;			// Stop TIMER2
		asm("ei $2");
		asm("nop");

		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		kbd_int_handler();

	}
}
/**********************************************************************
 *	HSYNC割り込みとHSYNC割り込みの中間で割り込んで
 *   PS/2キーボードポーリングする割り込み
 **********************************************************************
 */
void _MIPS32 Timer3Interrupt(void)
{
	if (IFS0bits.T3IF) {	// Timer Interrupt flag
		IFS0CLR=(1<<14);	// IFS14: Clear the timer interrupt flag
		kbd_int_handler();
		g_Timer3++;		// increment the counter
	}
}
/**********************************************************************
 *
 **********************************************************************
 */
