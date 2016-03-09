#ifndef _spi2_h_
#define _spi2_h_

#include "util.h"
//#define	_ISR    __attribute__((interrupt,nomips16,noinline))

//================================================================
//	SPI Config:
#define	DMA_INTERRUPT			(0)	// DMA割り込みを使用する.

//	DMAを使用して1LINE分の書き込み.
#define	USE_SPI2_INTERRUPT		(0)	// SPI2 送信割り込みを使用する.
#define	USE_DMA					(1)	// DMA  を使用する. (0)= SPI_writeで代用.


//================================================================
#include "utype.h"

//================================================================
void	SPI2_mode(uchar mode);
void	SPI2_clock(uint speed);
void	SPI2_close();
void	SPI2_init();

uint 	SPI2_write(uint data_out);
uint 	SPI2_read(void);
//================================================================

void 	NTSC_init(void);
void 	_MIPS32 wait_ms(int ms);
void 	draw_screen(void);
void	putch_cls();

//================================================================
//	画面サイズ
#define	WIDTH_DMA	40				// 4の倍数に限る.
#define	WIDTH		(WIDTH_DMA*8)	// 320dot
#define	HEIGHT		200				// 26行

//	左右余白サイズ
#define LSPACE		5		// 文字数
#define RSPACE		1		// 文字数

#endif

