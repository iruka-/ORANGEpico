/* portlist.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "monit.h"
#include "packet.h"
#include "portlist.h"
#include "util.h"

/*********************************************************************
 *	ポート名称	,  アドレス , 属性（バイト幅、よく参照されるかどうか）
 *********************************************************************
 */
#define	MAXPORTS	10000
static	char     prbuf[MAXPORTS][256];
/*********************************************************************
 *	ポート名称	,  アドレス , 属性（バイト幅、よく参照されるかどうか）
 *********************************************************************
 */

#include "portlist_mips.h"


#define	COLS 1

static char	mode_LPC = 0;	// LPC1343
static char	mode_FM3 = 0;

/*********************************************************************
 *	大文字小文字を区別しない文字列比較＋bit属性検出
 *********************************************************************
 */
int str_casecmpx(char *s,char *t,int *pmask)
{
	int bit;
	while(*t) {
		if(tolower(*s) != tolower(*t)) return 1;	//不一致.
		s++;
		t++;
	}
	if(*s==0) {
		*pmask = 0; return 0;	//一致.
	}
	if(*s=='.') {
		s++;
		if(s[1]==0) {
			bit = s[0] - '0';
			if((bit >= 0) && (bit < 8)) {
				*pmask = (1 << bit);
				return 0;		// port.bit (bitは0～7) 一致.
			}
		}
	}
	return 1;	//不一致.
}

int stri_chk(char *s,char *t)
{
	while(*t) {
		if(tolower(*s) != tolower(*t)) return 1;	//不一致.
		s++;
		t++;
	}
	return 0;	// 文字列 t の長さだけとりあえず一致した.
}
/*********************************************************************
 *	ポート名称からアドレスを求める.
 *********************************************************************
 */
int	portAddress(char *s)
{
	PortList *p = portList;
	char *q;
	while(p->name) {
		if(stricmp(s,p->name)==0) return p->adrs;
		p++;
	}

	if( mode_FM3 ) {
		while(p->name) {
			q=p->name;
			if(stri_chk(q,"bFM3_")==0) { q+=strlen("bFM3_");} else
			if(stri_chk(q,"FM3_")==0)  { q+=strlen("FM3_");} 
			if(stricmp(s,q)==0) return p->adrs;
			p++;
		}
	}
	return 0;
}

PortList *findPortName(char *s)
{
	char *t = s;
	PortList *p = portList;

#if	1	//def	LPC1343
	// "lpc_" で始まらないポート名に "lpc_" を補完する.

	if( mode_LPC ) {
		char buf[128];
		if(stri_chk(s,"LPC_")!=0) {
			t = buf;
			sprintf(buf,"LPC_%s",s);
		}
	}
#endif

	while(p->name) {
		if(stricmp(t,p->name)==0) return p;
		p++;
	}
	return NULL;
}

char *realPortName(char *s)
{
	PortList *p = findPortName(s);
	if(p) {
		return p->name;
	}
	return "?";
}

int getPortAttrib(char *s)
{
	PortList *p = findPortName(s);
	if(p) {
//		printf("%s=%x\n",s,p->attr);
		return p->attr;
	}
	return 0;
}

char *suggestPortName(char *s)
{
	char *t = s;
	char *rc="?";

	PortList *p = portList;

#if	1	//def	LPC1343
	// "lpc_" で始まらないポート名に "lpc_" を補完する.
	if( mode_LPC ) {
		char buf[128];
		if(stri_chk(s,"LPC_")!=0) {
			t = buf;
			sprintf(buf,"LPC_%s",s);
		}
	}
	if( mode_FM3 ) {
		char buf[128];
		if(stri_chk(s,"bFM3")!=0) {
		if(stri_chk(s,"FM3")!=0) {
			t = buf;
			sprintf(buf,"FM3_%s",s);
		}}
	}
#endif

	while(p->name) {
		if(stricmp(t,p->name)==0) return p->name;
		p++;
	}
	//無いので...
	p = portList;
	while(p->name) {
		if(stri_chk(p->name,t)==0) {
			printf("%16s(0x%x)\n",p->name,p->adrs);
			rc = p->name;
		}
		p++;
	}
	return rc;
}

/*********************************************************************
 *	ポート名称からアドレスとビットマスクを求める.
 *********************************************************************
 */
int	portAddress_b(char *s,int *pmask)
{
	PortList *p = portList;
	while(p->name) {
		if(str_casecmpx(s,p->name,pmask)==0) {
			return p->adrs;
		}
		p++;
	}
	return 0;
}

/*********************************************************************
 *	PortアドレスをPort名に翻訳する.
 *********************************************************************
 *	成功=1
 */
char *GetPortName(int adrs)
{
	PortList *p = portList;
	while(p->name) {
		if(p->adrs == adrs) return p->name;
		p++;
	}
	return NULL;
}

/*********************************************************************
 *	ポート一覧表を表示する.
 *********************************************************************
 */
void PrintPortNameList(void)
{
	PortList *p = portList;
//	int      i,k;
	int      m = 0;

	memset(prbuf,0,sizeof(prbuf));

	while(p->name) {
#if	_PIC_COL
		sprintf(prbuf[m],"%16s = 0x%08x ",p->name,p->adrs);
		m++;
		p++;
#else

#ifdef	FM3
		// bFM3で始まるportは表示しない.
		if(stri_chk(p->name,"bFM3_")!=0)
#endif
		{
			printf("%24s = 0x%08x\n",p->name,p->adrs);
			m++;
		}
		p++;
#endif
	}
#if	_PIC_COL
	//段組みしたい.
	k = (m+COLS-1)/COLS;	//COLS段にした場合の行数.
	for(i=0;i<k;i++) {
		printf("%s%s%s\n",prbuf[i],prbuf[i+k],prbuf[i+k*2]);
		printf("%s%s%s%s\n",prbuf[i],prbuf[i+k],prbuf[i+k*2],prbuf[i+k*3]);
	}
#endif
}

/*********************************************************************
 *	数値 d を二進数文字列に変換し buf に格納する.
 *		bufと同じアドレスを返す.
 *********************************************************************
 */
char *radix2str(char *buf,int d)
{
	char *rc = buf;
	int i,c=' ';
	unsigned int m= 0x80000000;
	int n=32;
	
//	if((d & 0xffff0000)==0) {n=16;m=0x8000;}
	
	for(i=0;i<n;i++) {
		if(d & m) c='1';
		else      c='0';
		*buf++ = c;
		m >>= 1;
		if(((i&7)==7)&&(i!=(n-1))) *buf++ = '_';
	}
	*buf = 0;
	return rc;
}

/*********************************************************************
 *	portlist の PORT TYPE(型情報)から アクセス幅を決定.
 *********************************************************************
 */
int	type2size(int type)
{
	type &= A_MASK;
	switch(type) {
	 case A_uint8_t:
		return MEM_BYTE;

	 case A_uint16_t:
		return MEM_HALF;

	 default:
	 case A_uint32_t:
		return MEM_WORD;
	}
}
#if	0
/*********************************************************************
 *	段組表示によるポート値出力.
 *********************************************************************
 */
void PrintPortColumn(void)
{
	PortList *p = portList;
	int      i,k;
	int      m = 0;
//	static   char     prbuf[MAXPORTS][128];	//結果バッファ（段組）

	memset(prbuf,0,sizeof(prbuf));

	while(p->name) {
		if((p->attr & A_MASK)
		 &&((p->attr & EX_PORT)==0)) {
#if	_PIC_COL
			int arena=type2size(p->attr);	//読み出し幅.
			int val = UsbPeek(p->adrs,arena);

	if(val & 0xffff0000) {
		sprintf(prbuf[m],"%14s(0x%08x) 0x%08x ",p->name,p->adrs,val);
	}else{
		sprintf(prbuf[m],"%14s(0x%08x)     0x%04x ",p->name,p->adrs,val);
	}

//			sprintf(prbuf[m],"%16s = %s ",p->name,radix2str(tmp,val));
#else
			sprintf(prbuf[m],"%6s = %s ",p->name,radix2str(tmp,data[p->adrs & 0xff]));
#endif
			m++;
		}
		p++;
	}
	//段組みしたい.
	k = (m+COLS-1)/COLS;	//COLS段にした場合の行数.
	for(i=0;i<k;i++) {
#if	_PIC_COL
		printf("%s%s%s\n",prbuf[i],prbuf[i+k],prbuf[i+k*2]);
#else
		printf("%s%s%s%s\n",prbuf[i],prbuf[i+k],prbuf[i+k*2],prbuf[i+k*3]);
#endif
	}
}
#endif
/*********************************************************************
 *	ポート内容を全部ダンプする.
 *********************************************************************
 */
void PrintPortAll(int mask)
{
	PortList *p = portList;

#ifdef	FM3
	mask = 1;	//全dumpするとハングする.
#endif

	while(p->name) {
		if((p->attr & A_MASK)
		 &&((p->attr & EX_PORT)==0)) {
			if((mask==0)||(p->attr & QQ)) {
				int arena=type2size(p->attr);	//読み出し幅.
				int val = UsbPeek(p->adrs,arena);
				cmdPortPrintOne(p->name,p->adrs,val);
			}
		}
		p++;
	}
}

/*********************************************************************
 *	
 *********************************************************************
 */
void delete_ExPort(void)
{
	PortList *p = portList;
	while(p->name) {
		if(	p->attr & EX_PORT) {
			p->attr &= (~1);
		}
		p++;
	}
}
/*********************************************************************
 *	PORTEの存在チェック.
 *********************************************************************
 */
int	InitPortList(int dev_id)
{
#ifdef	FM3
	if(dev_id==0xf3) {	// FM3
		portList = portlist_fm3;
		mode_FM3 = 1;
	}
#endif
	return 0;
}

/*********************************************************************
 *
 *********************************************************************
 */
