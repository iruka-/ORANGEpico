/********************************************************************
 *	GRAPHIC SUBROUTINES
 ********************************************************************
 */
#include <stdlib.h>
#include <string.h>
#include <p32xxxx.h>			// always in first place to avoid conflict with const.h ON
//#include <plib.h>

#include "utype.h"
#include "spi2.h"
#include "font8.h"

#define	M_PI	3.14159
#define SWAP(x,y)  {int t; t=(x); (x)=(y); (y)=t; }

/********************************************************************
 *	宣言
 ********************************************************************
 */
#define	GR_LSPACE	(LSPACE*8)				// 左余白dot 5*8=40
#define	GR_RSPACE	(RSPACE*8)				// 左余白dot 5*8=40
#define	GR_WIDTH	(WIDTH - GR_LSPACE-GR_RSPACE)	// 画面幅dot 320-5*8*2=240
#define	GR_HEIGHT	(HEIGHT)				// 画面高さdot 208

//	word size
#define	WSTRIDE		(WIDTH_DMA / 4)			// DMA 1ライン分の32bit WORD数.

//	bit address
#define	BITMASK(x)	( ((uint)0x80000000) >> (x & 31) )

#define	CWIDTH		(GR_WIDTH/8) //30
#define	CHEIGHT		(HEIGHT/8)	 //26
#define	CSTRIDE		(WIDTH_DMA)	 //40


extern uint	txferTxBuff[];
// スクリーンカーソル座標
static int sx=0;
static int sy=0;

/********************************************************************
 *	
 ********************************************************************
 */
//	====================================================
//	(x,y)座標からピクセルバッファのポインタ*pを得る.
//	====================================================
uint *p_adr(int x,int y)
{
	if( ((uint)x < GR_WIDTH) && ((uint)y < GR_HEIGHT) ) {
		x+=GR_LSPACE;
		return &txferTxBuff[ y * WSTRIDE + (x>>5) ];	// (x/32)
	}
	return 0;
}
//	====================================================
//	(x,y)座標にドットを打つ.
//	====================================================
void pset(int x,int y)
{
	uint *p=p_adr(x,y);
	if(p) *p |= BITMASK(x);
}
//	====================================================
//	(x,y)座標のドットを消す.
//	====================================================
void preset(int x,int y)
{
	uint *p=p_adr(x,y);
	if(p) *p &= ~BITMASK(x);
}
//	====================================================
//	(x,y)座標のドットを反転.
//	====================================================
void prev(int x,int y)
{
	uint *p=p_adr(x,y);
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
//	(cx,cy)文字座標からピクセルバッファのbyteポインタ*pを得る.
//	====================================================
uchar *ch_adr(int cx,int cy)
{
	uchar *t = (uchar*)txferTxBuff;
	if( ((uint)cx < CWIDTH) && ((uint)cy < CHEIGHT) ) {
		cx+=LSPACE;
		t += (cy*(8*CSTRIDE)+(cx & 0xfffc)); // 4文字単位で 1234 --> 4321 になる (DMA Endian問題)
		t += (3 -            (cx & 0x0003));
		return t;
	}
	return 0;
}

//	====================================================
//	ASCII文字(ch) の8x8フォントデータ(8byte)を得る.
//	====================================================
const uchar *get_font_adr(int ch)
{
	return &font8[ (ch & 0xff) * 8 ];
}

void gr_locate(int cx,int cy)
{
	sx=cx;
	sy=cy;
}

void gr_scrollup(int y)
{
	y *= 8;

	uint *t = txferTxBuff;
	uint *s = t + (y * WSTRIDE);
	int i;
	int m = (HEIGHT - y) * WSTRIDE;
	int n = (HEIGHT    ) * WSTRIDE;

	// 1行スクロール.
	for(i=0;i<m;i++) *t++ = *s++;
	// スクロール最後の行をゼロクリア.
	for(i=m;i<n;i++) *t++ = 0;
}

void gr_cls()
{
	// スクロール行=画面全体の行数にして、画面クリアの代用品にする.
	gr_scrollup(CHEIGHT);
	// カーソルを左上に.
	gr_locate(0,0);
}

void gr_crlf(void)
{
	sx=0;
	sy++;
	if(sy>=CHEIGHT) {
		sy=CHEIGHT-1;
		gr_scrollup(1);
	}
}

void gr_curadv(void)
{
	sx++;
	if(sx>=CWIDTH) {
		gr_crlf();
	}
}

void gr_bs(void)
{
	if(	sx == 0) {
		if( sy == 0) {
			return;
		}else{
			sy--;
		}
		sx = CWIDTH-1;
	}
}

void gr_del(void)
{
	gr_bs();
}

//	====================================================
//	文字座標(cx,cy)にASCII文字(ch)をグラフィック描画
//	====================================================
void gr_putch_xy(int ch,int cx,int cy)
{
	uchar *t = ch_adr(cx,cy);
	const uchar *font = get_font_adr(ch);

	if(t) {
		int i;
		for(i=0;i<8;i++) {
			*t = *font++;
			t += CSTRIDE;
		}
	}
}

void gr_tab(void)
{
	while(1) {
		int ch = ' ';
		gr_putch_xy(ch , sx,sy);
		gr_curadv();
		if((sx & 7)==0) break;
	}
}

//	====================================================
//	ASCII文字(ch)をカーソル位置に書き、カーソルを進める
//	====================================================
void gr_putch(int ch)
{
	if( ch>=' ' ) {
		gr_putch_xy(ch , sx,sy);
		gr_curadv();
	}else{
		switch(ch){
		 case '\n':	gr_crlf();break;
		 case 0x0c:	gr_cls() ;break;
		 case 0x08:	gr_bs()  ;break;
		 case 0x7f:	gr_del() ;break;
		 case 0x09:	gr_tab() ;break;
		 default:
			break;
		}
	}
}

//	====================================================
//	文字座標(cx,cy)にASCII文字(ch)をグラフィック描画
//	====================================================
void gr_puts(char *str)
{
	while(1) {
		int ch = *str++;if( ch==0 ) break;
		gr_putch(ch);
	}
}

//	====================================================
//	グラフィックテスト.
//	====================================================
void gr_test()
{
	int x;
	for(x=0;x<HEIGHT;x+=16) {
		gr_line(x,0,GR_WIDTH,x,1);
	}
	gr_circle(GR_WIDTH/2,HEIGHT/2,HEIGHT/2,1);
//	gr_circle_arc(GR_WIDTH/2,HEIGHT/2,HEIGHT/2,HEIGHT/2,60,140,1);

	gr_locate(0,0);
	int i;
	for(i=0;i<(CHEIGHT);i+=2) {
		gr_puts("Hello,World.\n");
		gr_puts("0123456789012345678901234567890123");
	}
}

