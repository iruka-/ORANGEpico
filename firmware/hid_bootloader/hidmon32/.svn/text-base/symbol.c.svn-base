/*
 *	シンボルテーブルの管理
 *
 *	struct SYMBOL {
 *		long      adrs;	//番地
 *		char far *name;	//名前
 *	} SYMBOL;
 *
 *	init_symbol();							初期化
 *	reg_symbol(*symbol);					登録
 *	find_symbol_by_adrs(*symbol,address);	番地による検索
 *	find_symbol_by_name(*symbol,*name  );	名前による検索
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "symbol.h"


/***  プロトタイプ宣言　***/
char  *segalloc(long size);
void init_symbol(void);
int adrs_hash(long adrs);
int name_hash(char  *name);
void entsym_by_adrs(SYMBOL  *symbol);
void entsym_by_name(SYMBOL  *symbol);
void reg_symbol(SYMBOL *symbol);
int find_symbol_by_adrs(SYMBOL *symbol,long address);
int find_symbol_by_name(SYMBOL *symbol,char *name );


SYMBOL  *symbuf;
char    *namebuf;
SYMBOL  *  *namehash;
SYMBOL  *  *adrshash;

char    *nameptr;
SYMBOL  *symptr;

#define	HASHMASK	0x3fff

char  *segalloc(long size)
{
//	char  *p;
//	char  *r;
	return malloc(size+16L);
}

void init_symbol()
{
	int i;
	symbuf  =(SYMBOL  *) segalloc(0x10000L);
	namebuf =(char    *) segalloc(0x10000L);
	namehash=(SYMBOL  *  *) segalloc(0x10000L);
	adrshash=(SYMBOL  *  *) segalloc(0x10000L);

	nameptr =namebuf;
	symptr  =symbuf ;
	for(i=0;i<0x4000;i++) namehash[i]=NULL;
	for(i=0;i<0x4000;i++) adrshash[i]=NULL;
}

int	adrs_hash(long adrs)
{
	int h,m,l;
	h = adrs >> 28L;
	m = adrs >> 14L;
	l = adrs ;
	return (h+m+l) & HASHMASK;
}

int	name_hash(char  *name)
{
	int i;
	i=0;
	while(*name) {
		i *= 5;i += (*name++);
	}
	return (i & HASHMASK);
}

void entsym_by_adrs(SYMBOL  *symbol)
{
	int h;
	h = adrs_hash(symbol->adrs);
	while(adrshash[h]!=NULL) {h++;h&=HASHMASK;}
	adrshash[h] = symbol;
}

void entsym_by_name(SYMBOL  *symbol)
{
	int h;
	h = name_hash(symbol->name);
	while(namehash[h]!=NULL) {h++;h&=HASHMASK;}
	namehash[h] = symbol;
}

void reg_symbol(SYMBOL *symbol)
{
	symptr->adrs = symbol->adrs;
	symptr->name = nameptr;
		strcpy(nameptr,symbol->name);
	nameptr += strlen(symbol->name) + 1;
	entsym_by_adrs(symptr);
	entsym_by_name(symptr);
	symptr++;
}



/*
;	番地による検索
 */
int find_symbol_by_adrs(SYMBOL *symbol,long address)
{
	int h;
	h = adrs_hash(address);
	while( adrshash[h]!=NULL) {
		if(adrshash[h]->adrs == address) {
			*symbol = *adrshash[h];
			return 1;
		}
		h++;h&=HASHMASK;
	}
	return 0;
}
/*
;		名前による検索
 */
int find_symbol_by_name(SYMBOL *symbol,char *name )
{
	int h;
	h = name_hash(name);
	while( namehash[h]!=NULL) {
		if(strcmp(namehash[h]->name,name) == 0) {
			*symbol = *namehash[h];
			return 1;
		}
		h++;h&=HASHMASK;
	}
	return 0;
}
