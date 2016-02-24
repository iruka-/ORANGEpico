#ifndef _spi2_h_
#define _spi2_h_

#define	_MIPS32 __attribute__((nomips16,noinline))
#define	_ISR    __attribute__((interrupt,nomips16,noinline))

//================================================================
//	SPI Config:
#define	USE_SPI2				(1)	// SPI2 を使用する.
#define	SVGA_MODE				(0)	// 0:VGA 1:SVGA

#define	DDS_TEST				(0)	// DDS
#define	TEST_SPI2_TONE			(0)	// SPI2 初期化直後にテストデータをWriteする.
#define	DMA_INTERRUPT			(0)	// DMA割り込みを使用する.


#if	DDS_TEST
//	SPI2のTX Empty割り込みを使用して連続書き込み(Audio).
#define	USE_SPI2_INTERRUPT		(1)	// SPI2 送信割り込みを使用する.
#define	USE_DMA					(0)	// DMA  を使用する. (0)= SPI_writeで代用.
#else
//	DMAを使用して1LINE分の書き込み.
#define	USE_SPI2_INTERRUPT		(0)	// SPI2 送信割り込みを使用する.
#define	USE_DMA					(1)	// DMA  を使用する. (0)= SPI_writeで代用.
#endif


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
#define	WIDTH32		40		//32	// 4の倍数に限る.
#define	HEIGHT		208

#define	WIDTH_DMA	 WIDTH32	//(32bit) ４の倍数に限る. WIDTHと同値.
#define	WIDTH_WORD	(WIDTH32/sizeof(int))	// WORD数.



#endif

