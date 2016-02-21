/********************************************************************
 *	GRAPHIC SUBROUTINES
 ********************************************************************
 */
#include <stdlib.h>
#include <string.h>
#include <plib.h>
#include "spi2.h"

#define	M_PI	3.14159
#define SWAP(x,y)  {int t; t=(x); (x)=(y); (y)=t; }

/********************************************************************
 *	宣言
 ********************************************************************
 */

//	dot size
#if	1
#define	WIDTH		256
#define	HEIGHT		240
#define	STRIDE32	WIDTH
#else
#define	WIDTH		400
#define	HEIGHT		300
#define	STRIDE32	WIDTH
#endif
//	word size
#define	STRIDE		(STRIDE32/32)	// DMA 1ライン分の32bit WORD数.
#define	BITMASK(x)	( ((uint)0x80000000) >> (x & 31) )

extern  uint	txferTxBuff[];


/********************************************************************
 *	
 ********************************************************************
 */
//	====================================================
//	(x,y)座標からピクセルバッファのポインタ*pを得る.
//	====================================================
int *p_adr(int x,int y)
{
	if( ((uint)x < WIDTH) && ((uint)y < HEIGHT) ) {
		return &txferTxBuff[ y * STRIDE + (x>>5) ];	// (x/32)
	}
	return 0;
}
//	====================================================
//	(x,y)座標にドットを打つ.
//	====================================================
void pset(int x,int y)
{
	int *p=p_adr(x,y);
	if(p) *p |= BITMASK(x);
}
//	====================================================
//	(x,y)座標のドットを消す.
//	====================================================
void preset(int x,int y)
{
	int *p=p_adr(x,y);
	if(p) *p &= ~BITMASK(x);
}
//	====================================================
//	(x,y)座標のドットを反転.
//	====================================================
void prev(int x,int y)
{
	int *p=p_adr(x,y);
	if(p) *p ^= BITMASK(x);
}
#define	gr_pset(x,y,c)	pset(x,y)
//	====================================================
//	線分を描画する.
//	====================================================
void gr_line(int x1,int y1,int x2,int y2,int c)
{
	int px,py;		/* 打つべき点 */
	int r;			/* 誤差レジスタ */
	int dx,dy,dir,count;

	if(x1 > x2) {SWAP(x1,x2);SWAP(y1,y2);}

	px=x1;py=y1;	/* 開始点 */
	dx=x2-x1;	/* デルタ */
	dy=y2-y1;dir=1;
	if(dy<0) {dy=-dy;dir=-1;} /* 負の傾き */

	if(dx<dy) {	/* デルタｙの方が大きい場合 */
		count=dy+1;
		r=dy/2;
		do {
			gr_pset(px,py,c);py+=dir;
			r+=dx;if(r>=dy) {r-=dy;px++;}
		} while(--count);
	} else {	/* デルタｘの方が大きい場合 */
		count=dx+1;
		r=dx/2;
		do {
			gr_pset(px,py,c);px++;
			r+=dy;if(r>=dx) {r-=dx;py+=dir;}
		} while(--count);
	}
}

//	====================================================
//	円(cx,cy),半径r,色c を描く
//	====================================================
void gr_circle( int cx,int cy,int r,int c)
{
	int  x,y;
	int  xr,yr;

	if(r==0) return;
	x=r * r;y=0;
	do {
		xr= x / r;
		yr= y / r;
		gr_pset(cx+xr,cy+yr,c);
		gr_pset(cx-xr,cy+yr,c);
		gr_pset(cx-xr,cy-yr,c);
		gr_pset(cx+xr,cy-yr,c);

		gr_pset(cx+yr,cy+xr,c);
		gr_pset(cx-yr,cy+xr,c);
		gr_pset(cx-yr,cy-xr,c);
		gr_pset(cx+yr,cy-xr,c);

		x += yr;
		y -= xr;
	}while( x>= (-y) );
}
#if	0
//	浮動小数点ライブラリがリンクされるので、割愛
//	====================================================
//	円弧(cx,cy),半径r,色c,角度(begin,end) を描く
//	====================================================
void gr_circle_arc( int cx,int cy,int rx,int ry,int c,int begin,int end)
{
	float x,y,rad,t,td;
	int xr,yr;
	if(rx>ry) td = rx;
	else	  td = ry;

	td = (360/6.28) / (td * 1.2);

	for(t=begin;t<end;t=t+td) {
		rad = (t * M_PI *2 ) / 360.0;
		x =  cos(rad);
		y =  sin(rad);
		xr = x * (float)rx;
		yr = y * (float)ry;
		gr_pset(cx-xr,cy-yr,c);
	}
}
#endif

//	====================================================
//	グラフィックテスト.
//	====================================================
void gr_test()
{
	int x,y;
	for(x=0;x<HEIGHT;x+=16) {
		gr_line(x,0,WIDTH,x,1);
	}
	gr_circle(WIDTH/2,HEIGHT/2,HEIGHT/2,1);
//	gr_circle_arc(WIDTH/2,HEIGHT/2,HEIGHT/2,HEIGHT/2,60,140,1);
}

