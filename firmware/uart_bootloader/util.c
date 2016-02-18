//#include "HardwareProfile.h"
//#include "./USB/usb.h"
#include <p32xxxx.h>			// always in first place to avoid conflict with const.h ON
#include <stdlib.h>			// always in first place to avoid conflict with const.h ON

//#define	_BOOTROM_	/**/
#define	_BOOTROM_  __attribute__((section(".bootrom")))

typedef	unsigned int uint;
void _BOOTROM_ delay_us(int us);
void wait_ms(int ms);

/********************************************************************
 *		
 ********************************************************************
 */
void led_flip()
{
	LATBINV=0x8000;	// LATB.bit15を反転
}

/********************************************************************
 *		
 ********************************************************************
 */
void _BOOTROM_ led_test()
{
	while(1) {
		led_flip();
		wait_ms(500);
	}
}

/********************************************************************
 *		
 ********************************************************************
 */
//#define	CNTMAX		400000
#define	CNTMAX		100000
void _BOOTROM_ led_blink()
{
	static int cnt=0;
	cnt++;
	if(	cnt >= CNTMAX) {
		cnt=0;
		led_flip();
	}
}


#if	1
/********************************************************************
 *	code領域節約 memcpy
 ********************************************************************
 */
void _BOOTROM_ *memcpy(void *dst,const void *src,size_t size)
{
	char *t = (char*)dst;
	char *s = (char*)src;
	while(size--) {*t++=*s++;}
	return t;
}
#endif


/********************************************************************
 *	code領域節約 memset
 ********************************************************************
 */
void _BOOTROM_ *memset(void *dst,int data,size_t size)
{
	char *t = (char*)dst;
	while(size--) {*t++=data;}
	return t;
}


void wait_nloop(int n);
/********************************************************************
 *	( ms ) * 1mS 待つ.
 ********************************************************************
 */
void _BOOTROM_ wait_ms(int ms)
{
	int i;
	for(i=0; i<ms; i++) {
		delay_us(1000);
	}
}

/********************************************************************
 *	( us ) * 1uS 待つ.
 ********************************************************************
 */
void _BOOTROM_ delay_us(int us)
{
	wait_nloop(us*10);
}
/********************************************************************
 *	( nloop * 1/10 ) uS 待つ. clock=40MHzと仮定.
 ********************************************************************

9fc00110 <wait_nloop>:
9fc00110:	6500      	nop
9fc00112:	4cff      	addiu	a0,-1
9fc00114:	2cfd      	bnez	a0,9fc00110 <wait_nloop>
9fc00116:	e8a0      	jrc	ra

 */
//clock=40MHzと仮定.	最適化オプションやコンパイラが変わったら再計測.
void _BOOTROM_ wait_nloop(int n)
{
	do {
		asm("nop");
		n--;
	}while( n != 0);
}


#if	1
/********************************************************************
 *	code領域節約 INT系関数.
 ********************************************************************
 */
unsigned int __attribute__((nomips16,section(".bootrom"))) INTDisableInterrupts(void)
{
	asm("di $2");	// $2 = v0
}
void __attribute__((nomips16,section(".bootrom")))  INTRestoreInterrupts(unsigned int status)
{
	if((status & 1)==0) {
		asm("di");
	}else{
		asm("ei");
	}
}
//void _BOOTROM_ INTClearFlag(INT_SOURCE source)
//{
	// FIXME!
//}
#endif


/********************************************************************
 *	
 ********************************************************************
 */
