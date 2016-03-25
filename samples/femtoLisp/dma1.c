/********************************************************************
 *	DMA KICKER
 ********************************************************************
 */
#include <p32xxxx.h>			// always in first place to avoid conflict with const.h ON

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "interrupt.h"
#include "util.h"
#include "dma1.h"
#include "spi2.h"

#define CH1  1
#define PRI2 2

#define DMA_EV_START_IRQ_EN     _DCH0ECON_SIRQEN_MASK
#define DMA_EV_START_IRQ(irq)   (DMA_EV_START_IRQ_EN | ((irq)<<_DCH0ECON_CHSIRQ_POSITION))

/********************************************************************
 *	DMA転送バッファ: Graphic-VRAM
 ********************************************************************
 */
#define	WIDTH_WORD	(WIDTH_DMA/sizeof(int))	// WORD数.

uint	txferTxBuff[HEIGHT+16][WIDTH_WORD];

/********************************************************************
 *	DMA 設定レジスタ CH毎 テーブル
 ********************************************************************
 */
static	volatile uint *DCHxCON_tab[4]={&DCH0CON,&DCH1CON,&DCH2CON,&DCH3CON};

/********************************************************************
 *	DMAイベント制御
 ********************************************************************
 */
void _MIPS32 dmaWriteEventControlFlags(int chn,int dmaEvCtrl)
{
	volatile uint *p=DCHxCON_tab[chn & 3];
	p[4]= dmaEvCtrl & 0xffff38;  //DCHxECON
}
/********************************************************************
 *	DMAオープン
 ********************************************************************
 */
void _MIPS32 dmaOpen(int chn,int chPri, int oFlags)
{
	volatile uint *p=DCHxCON_tab[chn & 3];
	IntEnable(65+chn);
	p[6]=0x40;

	DMACONSET=0x8000;
	while(p[4] & 0x40);

	uint v0=DCRCCON & 7;
	if(v0==chn) {
		DCRCCONCLR=0x8000;
	}
	p[5]=0x38;
	p[9]=-1;
	IntClearFlag(65+chn);
	if(oFlags==0) {
		p[6]=0x20;
	}
	p[0]=oFlags|(chPri & 3);
}

/********************************************************************
 *	DMA転送アドレス、サイズのセット
 ********************************************************************
 */
void _MIPS32 dmaSetTxfer(int chn, const void* vSrcAdd, void* vDstAdd, int srcSize, int dstSize, int cellSize)
{
	volatile uint *p=DCHxCON_tab[chn & 3];
	uint a1 = (uint) vSrcAdd;
	uint a2 = (uint) vDstAdd;
	p[12]=a1 & 0x1fffffff;	//DCH0SSA src
	p[16]=a2 & 0x1fffffff;	//DCH0DSA dst
	p[20]=srcSize;			//DCH0SSIZ ssize
	p[24]=dstSize;			//DCH0DSIZ dsize
	p[36]=cellSize;			//DCH0CSIZ
	p[9] =0xff;				//DCH0INTCLR

//	p[9]=0xff;
	p[2]=0x80;				//DCH0CONSET
	p[6]=0x80;				//DCH0ECONSET
}

/********************************************************************
 *	DMA転送の開始
 ********************************************************************
 */
int _MIPS32 dmaStartTxfer(int chn)
{
	volatile uint *p=DCHxCON_tab[chn & 3];
	p[9]=0xff;				//DCH0INTCLR
	p[2]=0x80;				//DCH0CONSET
	p[6]=0x80;				//DCH0ECONSET
	return 0;
}

/********************************************************************
 *	
 ********************************************************************
 */
void dma_init()
{
	int x,y,c=0;
	uint *vram = &txferTxBuff[0][0];
	for(y=0;y<HEIGHT;y++) {
		for(x=0;x<WIDTH_WORD;x++) {
			*vram++ = c;
//			c++;
		}
	}
	dmaOpen(CH1,PRI2,0);
	dmaWriteEventControlFlags(CH1, DMA_EV_START_IRQ_EN|DMA_EV_START_IRQ(_SPI2_TX_IRQ));
}

/********************************************************************
 *	
 ********************************************************************
 */
void _MIPS32 dma_kick(int cnt)
{
	uint *p = txferTxBuff[cnt];
	dmaSetTxfer(CH1, p, (void*)&SPI2BUF, WIDTH_DMA, 4, 4);
//	dmaStartTxfer(CH1);
}

void _MIPS32 Dma1Interrupt(void)
{
/*
	int	evFlags;
	IntClearFlag(INT_SOURCE_DMA(CH1));
	evFlags=DmaChnGetEvFlags(CH1);
    if(evFlags & BLOCK_DONE) {
        DmaChnClrEvFlags(CH1, BLOCK_DONE);
    }
*/
}


