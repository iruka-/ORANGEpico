/********************************************************************
 *	DMA KICKER
 ********************************************************************
 */
#include <stdlib.h>
#include <string.h>
#include <plib.h>

#include "spi2.h"

/********************************************************************
 *	êÈåæ
 ********************************************************************
 */

uint	txferTxBuff[HEIGHT+16][WIDTH_WORD];

/********************************************************************
 *	
 ********************************************************************
 */
void dma_init()
{
#if	1
	int x,y,c=0;
	int *vram = &txferTxBuff[0][0];
	for(y=0;y<HEIGHT;y++) {
		for(x=0;x<WIDTH_WORD;x++) {
			*vram++ = c;
			c++;
		}
	}
#endif
	DmaChnOpen(DMA_CHANNEL1, DMA_CHN_PRI2, DMA_OPEN_DEFAULT);
	DmaChnSetEventControl(DMA_CHANNEL1, DMA_EV_START_IRQ_EN|DMA_EV_START_IRQ(_SPI2_TX_IRQ));
}

/********************************************************************
 *	
 ********************************************************************
 */
void _MIPS32 dma_kick(int cnt)
{
	int *p = txferTxBuff[cnt];
	DmaChnSetTxfer(DMA_CHANNEL1, p, (void*)&SPI2BUF, WIDTH_DMA, 4, 4);
	DmaChnStartTxfer(DMA_CHANNEL1, DMA_WAIT_NOT, 0);
}

/********************************************************************
 *	
 ********************************************************************
 */
void _ISR DmaHandler1(void)
{
	int	evFlags;
	INTClearFlag(INT_SOURCE_DMA(DMA_CHANNEL1));
	evFlags=DmaChnGetEvFlags(DMA_CHANNEL1);
    if(evFlags&DMA_EV_BLOCK_DONE) {
        DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
    }
}


/********************************************************************
 *	
 ********************************************************************
 */
void _user_putc(int c)
{
}
/********************************************************************
 *	
 ********************************************************************
 */
void _user_puts(char *s)
{
}
