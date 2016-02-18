/*********************************************************************
 *	USB経由のターゲットメモリーアクセス : 逆アセンブル専用
 *********************************************************************

int code_buf_init();
int code_buf_read(int adr,int size ,uchar *buf);
int code_buf_peek(int adr,int width);

 */

#ifndef	_LINUX_
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#include "monit.h"
#include "portlist.h"
#include "util.h"
//#include "../../firmware/monitor/hidcmd.h"
#include "hidcmd.h"
#include "packet.h"
#include "codebuf.h"

#define	CACHE_SIZE 		32	//128	//0x100
#define	CACHE_PAGE 		16


uchar cache_buf[CACHE_SIZE*CACHE_PAGE];
int   cache_adr[CACHE_PAGE];
int   cache_idx;


#define	USE_CODE_BUF	1

#define PAGEADR(n)    ((n) & (-CACHE_SIZE))
#define PAGEOFFSET(n) ((n) & ( CACHE_SIZE-1))
/*********************************************************************
 *
 *********************************************************************
 */
int code_buf_init()
{
	int i;
	for(i=0;i<CACHE_PAGE;i++) {
		cache_adr[i]=(-1);
	}
	cache_idx=0;
	return 0;
}

/*********************************************************************
 *
 *********************************************************************
 */
static int code_hit_check(int adr)
{
	int i;
	for(i=0;i<CACHE_PAGE;i++) {
		int page = PAGEADR(adr);
		if(	cache_adr[i] == page) {
			return i;
		}
	}
	return -1;
}

//メモリー内容の読み出し.
static void readmem(int page,int size,uchar *buf)
{
	if(page != 0) {
		dumpmem(page,AREA_PGMEM,size,buf);
	}else{
		int n;
		// ０番地からの読み出しを実行すると、firmがハングする場合がある.
		//   (adr引数がNULLとみなされ、関数が動作しないからだ。)
		while(size) {
			n=size;
			if(n>32) n=32;
			dumpmem(page,AREA_PGMEM,n,buf);
			page +=n;
			buf  +=n;
			size -=n;
		}
		// なので、32byte以下の要求に分割して、NULLポインタを作らせない.
	}
}
/*********************************************************************
 *
 *********************************************************************
 */
static int code_update(int adr)
{
	int page = PAGEADR(adr);
	int off = CACHE_SIZE * cache_idx;
	int rc = cache_idx;
	cache_adr[cache_idx] = page;

	//メモリー内容の読み出し.
	readmem(page,CACHE_SIZE,cache_buf+off);

	// ラウンドロビンで再利用.
	cache_idx++;
	if( cache_idx >= CACHE_PAGE) {
		cache_idx = 0;
	}

	return rc;
}

/*********************************************************************
 *
 *********************************************************************
 */
int code_buf_read_sub(int adr,int size,uchar *buf)
{
#if	USE_CODE_BUF
	int hit;

	uchar *p;
	// キャッシュより大きい読み出しや、ページをまたぐ読み出しには対応しない.
	if( ( size > CACHE_SIZE ) || ( PAGEADR(adr) != PAGEADR(adr+size-1) ) ) {
		return dumpmem(adr,AREA_PGMEM,size,buf);	//メモリー内容の読み出し.
	}
	// キャッシュにあるかどうか調べる.
	hit = code_hit_check(adr);
	if( hit < 0 ) {
		hit = code_update(adr);		//キャッシュを更新.
	}
	p = cache_buf + CACHE_SIZE*hit + PAGEOFFSET(adr);	//キャッシュの該当箇所.
	memcpy(buf,p,size);
	return size;
#else	//	USE_CODE_BUF
	return dumpmem(adr,AREA_PGMEM,size,buf);	//メモリー内容の読み出し.
#endif	//	USE_CODE_BUF
}


int code_buf_read(int adr,int size,uchar *buf)
{
	// キャッシュより大きい読み出しには対応しない.
	if( size > CACHE_SIZE ) {
		return dumpmem(adr,AREA_PGMEM,size,buf);	//メモリー内容の読み出し.
	}
	
	// ページを跨ぐ読み出しは２回に分ける.
	if ( PAGEADR(adr) != PAGEADR(adr+size-1) ) {
		int off1  = PAGEOFFSET(adr);	//要求ページ内オフセット.
		
		int size1 = CACHE_SIZE - off1;  //前半パートのサイズ.
		int size2 = size - size1 ;  	//後半パートのサイズ.
		
		code_buf_read_sub(adr      ,size1,buf);
		code_buf_read_sub(adr+size1,size2,buf+size1);
		return size;
	}
	
	// ページを跨がない.
	return code_buf_read_sub(adr,size,buf);
}

int code_buf_peek(int adr,int width)
{
	unsigned char buf[16];
	int size=4;
//	int rc = UsbRead(adr,width,buf,size);
	int rc = code_buf_read(adr,size,buf);
	if( rc != size) {
		return -1;
	}
	switch(width) {
	  default:
	  case MEM_BYTE:
		return buf[0];
	  case MEM_HALF:
		return buf[0] | (buf[1]<<8);
	  case MEM_WORD:
		return buf[0] | (buf[1]<<8)| (buf[2]<<16)| (buf[3]<<24);
	}
}
