/**********************************************************************
 *	R3000 DISASM
 **********************************************************************
 */		char USAGE[]=

"*  R3000 DISASM Ver0.2\n"
"   使い方：\n"
"C:> DISR -Option FILENAME.BIN\n"
"   オプション\n"
"         -s<start address>  先頭番地を１６進数で指定.\n"
"         -m                 ビッグエンディアンコードを解析する.\n"
"         -r                 レジスタ名称をr0～r31に変える.\n"
"         -h                 １６進数の表示を $ でなく 0x にする.\n"



;


#include "std.h"
#include <sys/stat.h>
#include "ansidecl.h"
#include "dis-asm.h"
#include "symbol.h"


long	lpeek(long adr);
int	print_insn_little_mips (long memaddr,struct disassemble_info *info);

struct 	disassemble_info info;
long	filesize;
struct  stat statbuf;
long	startaddr=0;
long	endaddr=0;
int		bigmips=0;

char	 lbuf[128];
char	 remarkbuf[128];
char	*lbufp;

SYMBOL  lsymbol;
char    label[80];

pr_rem(form,val)
	char *form;
	char *val;
{
	vsprintf(remarkbuf,form,&val);
}

static	prfun(stream,form,val)
	FILE *stream;
	char *form;
	char *val;
{
	vsprintf(lbufp,form,&val);lbufp+=strlen(lbufp);
}

static	unsigned long	chkbig(unsigned long data)
{
	if(bigmips) {
		return	(((data <<24L) & 0xff000000L) |
				 ((data << 8L) & 0x00ff0000L) |
				 ((data >> 8L) & 0x0000ff00L) |
				 ((data >>24L) & 0x000000ffL) );
	}
	return data;
}

static	memfun(long adr,long *buf)
{
	long data;

	if( (adr>=startaddr) && (adr<endaddr) ) {
		Read(&data,4);

		*buf = data;
		sprintf(lbufp,"%08lX %08lX ",adr,chkbig(data));lbufp+=strlen(lbufp);
		return 0;
	}else{
		return 1;
	}
}

static	void	errfun()
{
	//printf("errfun()\n");
}


static	int	pr_addr(long addr)
{
	if(find_symbol_by_adrs(&lsymbol,addr)) {
		char far *name=lsymbol.name;
		char *p = label;
		while(*name) *p++ = *name++;
					 *p++ = 0;
		sprintf(lbufp," ;%s\n",label);lbufp+=strlen(lbufp);
	}
}

void set_ea(char *buf,long addr)
{
	if(find_symbol_by_adrs(&lsymbol,addr)) {
		char far *name=lsymbol.name;
		char *p = label;
		while(*name) *p++ = *name++;
					 *p++ = 0;
		sprintf(buf,"%s",label);
	}else{
		sprintf(buf,"0x%08lx",addr);
	}
}


static	void	prafun(long addr)
{
	sprintf(lbufp,"$%08lx",addr);lbufp+=strlen(lbufp);
	pr_addr(addr);
}


static	dislmips_init()
{
	info.stream				= NULL;
	info.fprintf_func       = prfun;
	info.read_memory_func   = memfun;
	info.memory_error_func  = errfun;
	info.print_address_func = prafun;
}

dislmips(long memaddr)
{
	lbufp=lbuf;
	remarkbuf[0]=0;

	if(find_symbol_by_adrs(&lsymbol,memaddr)) {
		char far *name=lsymbol.name;
		char *p = label;
		while(*name) *p++ = *name++;
					 *p++ = 0;
		printf("                  %s:\n",label);
	}

	if(bigmips) {
		print_insn_big_mips (memaddr,&info);
	}else{
		print_insn_little_mips (memaddr,&info);
	}
	if(remarkbuf[0]) {
		printf("%-48s %s\n",lbuf,remarkbuf);
	}else{
		printf("%s\n",lbuf);
	}
	if(is_crlf())
		printf("\n");
}

bfd_vma		bfd_getl32(const unsigned char *buffer)
{
	unsigned long *l=(unsigned long *) buffer;
	return *l;
}

bfd_vma		bfd_getb32(const unsigned char *buffer)
{
	return (((unsigned long )buffer[0]<<24L) |
			((unsigned long )buffer[1]<<16L) |
			((unsigned long )buffer[2]<< 8L) |
			((unsigned long )buffer[3]     ) )
	;
}

void disr_usage()
{
	printf(USAGE);
	exit(1);
}

/*
 *	一行入力ユーティリティ
 */
int getln(char *buf,FILE *fp)
{
	int c;
	int l;
	l=0;
	while(1) {
		c=getc(fp);
		if(c==EOF)  return(EOF);/* EOF */
		if(c==0x0d) continue;
		if(c==0x0a) {
			*buf = 0;	/* EOL */
			return(0);	/* OK  */
		}
		*buf++ = c;l++;
		if(l>=255) {
			*buf = 0;
			return(1);	/* Too long line */
		}
	}
}
/*
 *	拡張子付加ユーティリティ
 *
 *	addext(name,"EXE") のようにして使う
 *	もし拡張子が付いていたらリネームになる
 */
void addext(char *name,char *ext)
{
	char *p,*q;
	p=name;q=NULL;
	while( *p ) {
		if ( *p == '.' ) q=p;
		p++;
	}
	if(q==NULL) q=p;
	/*if( (p-q) >= 4) return;  なんかbugっている*/
	strcpy(q,".");
	strcpy(q+1,ext);
}

/**********************************************************************
 *	シンボルファイルがあればそれを読み込む
 **********************************************************************
 */
void read_symbol(char *name)
{
	char mapname[80];
	char buf[256];
	char sym[256];
	char dum[256];
	SYMBOL symbol;
	long adr=0;

	strcpy(mapname,name);
	addext(mapname,"map");
	ifp=fopen(mapname,"rb");if(ifp!=NULL) {
		while(getln(buf,ifp)!=EOF) {
			if(sscanf(buf,"%lx %s %s",&adr,sym,dum)==2) {
				symbol.adrs = adr;
				symbol.name = sym;
				reg_symbol(&symbol);
			/*	printf("#%s\n%lx <%s>\n",buf,adr,sym);*/
			}
		}
		fclose(ifp);
	}
}



int disr_main(int argc,char **argv)
{
	long off;
	long adr;
	
	Getopt(argc,argv);
	if(argc<2) disr_usage();

	if(IsOpt('m')) bigmips=1;
	if(IsOpt('r')) set_alternate_regname();
	if(IsOpt('h')) set_alternate_hexa();
	if(IsOpt('s')) sscanf(Opt('s'),"%lx",&startaddr);
	adr = startaddr;
	dislmips_init();

	init_symbol();
	read_symbol(argv[1]);

	stat(argv[1],&statbuf);
		filesize= statbuf.st_size;
		endaddr = startaddr + filesize;

	Ropen(argv[1]);
	
	for(off=0;off<filesize;off+=4) {
		dislmips(adr);adr+=4;
	}
	
	Rclose();
}
