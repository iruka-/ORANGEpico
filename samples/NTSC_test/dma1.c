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

#if	SVGA_MODE		// 0:VGA 1:SVGA
#define	WIDTH		48	// 4ÇÃî{êîÇ…å¿ÇÈ.
#define	HEIGHT		300	// 800 x 600 ÇÃ 600 Ç2Ç≈äÑÇ¡ÇƒÇ¢ÇÈ.
#else
#define	WIDTH		32	// 4ÇÃî{êîÇ…å¿ÇÈ.
#define	HEIGHT		240
#endif

#define	WIDTH_DMA	WIDTH	//(32bit) ÇSÇÃî{êîÇ…å¿ÇÈ. WIDTHÇ∆ìØíl.
#define	WIDTH_WORD	(WIDTH/sizeof(int))	// WORDêî.
static ushort g_Line=0;
//static uchar  g_cnt=0;
//static 
uint	txferTxBuff[HEIGHT][WIDTH_WORD];

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
//	if(cnt<HEIGHT) {
//	DmaChnOpen(DMA_CHANNEL1, DMA_CHN_PRI2, DMA_OPEN_DEFAULT);
//	DmaChnSetEventControl(DMA_CHANNEL1, DMA_EV_START_IRQ_EN|DMA_EV_START_IRQ(_SPI2_TX_IRQ));
	int *p = txferTxBuff[g_Line];
	DmaChnSetTxfer(DMA_CHANNEL1, p, (void*)&SPI2BUF, WIDTH_DMA, 4, 4);
	DmaChnStartTxfer(DMA_CHANNEL1, DMA_WAIT_NOT, 0);
	
//	if(cnt==0) {g_Line=0;g_cnt=0;}
		g_Line++;

//	g_cnt++;
//	if(	g_cnt>=2) 
//	{
//		g_cnt=0;
//	}
}

/********************************************************************
 *	
 ********************************************************************
 */
void dma_clear(void)
{
	{g_Line=0;}	//g_cnt=0;}
	
#if	0
	int i,j;
	for(i=0;i<HEIGHT;i++) {
		for(j=0;j<WIDTH_WORD;j++) {
			txferTxBuff[i][j]=0;
		}
	}
#endif
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
