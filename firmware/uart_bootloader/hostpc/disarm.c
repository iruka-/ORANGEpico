/**********************************************************************
 *	ARM DISASM
 **********************************************************************
 */		char USAGE[]=

"*  ARM DISASM Ver0.1\n"
"   使い方：\n"
"C:> DISARM -Option FILENAME.BIN\n"
"   オプション\n"
"         -s<start address>  先頭番地を１６進数で指定.\n"
"         -t                 全てThumb命令と見なして逆アセンブル.\n"
"         -m                 ビッグエンディアンコードを解析する.\n"
;


//#include "std.h"
#include <stdarg.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include "ansidecl.h"
#include "dis-asm.h"
#include "symbol.h"

typedef	unsigned char uchar;

void set_default_mips_dis_options (struct disassemble_info *info);

int		set_force_thumb(int f);
int		is_crlf(void);
void 	reg_symbol(SYMBOL *symbol);
void 	init_symbol(void);

long	lpeek(long adr);
//int	print_insn_little_arm (long memaddr,struct disassemble_info *info);

int
print_insn_mips16 (bfd_vma memaddr, struct disassemble_info *info);

struct 	disassemble_info info;
long	filesize;
struct  stat statbuf;
long	startaddr=0;
long	endaddr=0;
int		bigarm=0;
int		is_thumb =0;


char	 hbuf[64];		// 00000000 00000000
unsigned int		 hbuf_adr;
unsigned int		 hbuf_dat;

char	 lbuf[128];		//					 mov	ax,3
char	 remarkbuf[128];//									;コメント.
char	*lbufp;

SYMBOL  lsymbol;
char    label[80];

#define	INS_BUFSIZE	16
static	uchar	ins_buf[INS_BUFSIZE];
static	long	ins_adr=0;


void pr_rem(char *fmt , ... )
{
    va_list args;
    va_start(args,fmt);

    vsprintf(remarkbuf , fmt, args);

    va_end(args);
}

static void prfun(FILE *stream,char *fmt, ...)
{
    va_list args;
    va_start(args,fmt);

    vsprintf(lbufp,fmt,args);

    lbufp+=strlen(lbufp);

    va_end(args);
}

static	unsigned long	chkbig(unsigned long data)
{
	if(bigarm) {
		return	(((data <<24L) & 0xff000000L) |
				 ((data << 8L) & 0x00ff0000L) |
				 ((data >> 8L) & 0x0000ff00L) |
				 ((data >>24L) & 0x000000ffL) );
	}
	return data;
}



static	int	memfun(long adr,long *buf)
{
	int  off;
	if(	adr >= ins_adr) {
		off = adr - ins_adr;
		if(	off < 16 ) {
			memcpy(buf,ins_buf+off,sizeof(long));
			hbuf_adr =  adr;
			hbuf_dat = *buf;
			return 0;
		}
	}
	return 1;
}

#if	0
static	long	seek_oldpos = (-1);
//
static	void	OptSeek(pos)
{
	if(	seek_oldpos != pos) {
		seek_oldpos = pos;
		Seek(ifp,pos);
	}
}

static	int	memfun(long adr,long *buf)
{
	long data;

	if( (adr>=startaddr) && (adr<endaddr) ) {
		OptSeek(adr - startaddr);
		Read(&data,sizeof(long));
		seek_oldpos += sizeof(long);

		*buf = data;
//		sprintf(lbufp,"%08x %08x\t",adr,chkbig(data));lbufp+=strlen(lbufp);
		hbuf_adr = adr;
		hbuf_dat = data;
		return 0;
	}else{
		return 1;
	}
}
#endif

static	void	errfun()
{
	//printf("errfun()\n");
}

#if	0
static	int	pr_addr(long addr)
{
	if(find_symbol_by_adrs(&lsymbol,addr)) {
		char *name=lsymbol.name;
		char *p = label;
		while(*name) *p++ = *name++;
					 *p++ = 0;
		sprintf(lbufp," ;%s\n",label);lbufp+=strlen(lbufp);
	}
	return 0;
}
#endif

void set_ea(char *buf,long addr)
{
#if	0
	if(find_symbol_by_adrs(&lsymbol,addr)) {
		char *name=lsymbol.name;
		char *p = label;
		while(*name) *p++ = *name++;
					 *p++ = 0;
		sprintf(buf,"%s",label);
	}else
#endif
	{
		sprintf(buf,"0x%08lx",addr);
	}
}


static	void	prafun(long addr)
{
	sprintf(lbufp,"$%08lx",addr);lbufp+=strlen(lbufp);
//	pr_addr(addr);
}


static	void	dislarm_init()
{
	set_default_mips_dis_options (&info);

	info.stream				= NULL;
	info.fprintf_func       = (fprintf_ftype) prfun;
	info.read_memory_func   = (int (*)()) memfun;
	info.memory_error_func  = errfun;
	info.print_address_func = (void (*)())prafun;

	info.endian = BFD_ENDIAN_LITTLE;
}

int	dislarm(long memaddr)
{
	int oplen;
	lbufp=lbuf;
	remarkbuf[0]=0;
#if	0
	if(find_symbol_by_adrs(&lsymbol,memaddr)) {
		char *name=lsymbol.name;
		char *p = label;
		while(*name) *p++ = *name++;
					 *p++ = 0;
		printf("                  %s:\n",label);
	}
#endif

	if(is_thumb) {
		oplen = print_insn_mips16 (memaddr,&info);
		if(oplen==2) {
			sprintf(hbuf ,"%08x %04x     "	,hbuf_adr,hbuf_dat & 0xffff);
		}else{
			sprintf(hbuf ,"%08x %04x %04x"	,hbuf_adr,hbuf_dat & 0xffff,hbuf_dat>>16);
		}
	}else{
		oplen = print_insn_little_mips (memaddr,&info);
			sprintf(hbuf ,"%08x %08lx "		,hbuf_adr,chkbig(hbuf_dat));
	}
	
	if(remarkbuf[0]) {
		printf("%s\t%-48s %s\n"	,hbuf,lbuf,remarkbuf);
	}else{
		printf("%s\t%s\n"		,hbuf,lbuf);
	}
	if(is_crlf()) {
		printf("\n");
	}
//	oplen = info.bytes_per_line;
	return oplen;
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

bfd_vma		bfd_getl16(const unsigned char *buffer)
{
	unsigned short *l=(unsigned short *) buffer;
	return *l;
}

bfd_vma		bfd_getb16(const unsigned char *buffer)
{
	return (
			((unsigned long )buffer[2]<< 8L) |
			((unsigned long )buffer[3]     ) )
	;
}

void	disr_usage(void)
{
	printf("%s",USAGE);
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
#if	0
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
			/*	printf("#%s\n%x <%s>\n",buf,adr,sym);*/
			}
		}
		fclose(ifp);
	}
}


int	disr_main(int argc,char **argv)
{
	long off;
	long adr;
	long oplen;
	
	Getopt(argc,argv);
	if(argc<2) usage();

	if(IsOpt('m')) bigarm=1;
	if(IsOpt('t')) is_thumb=1;
	if(IsOpt('s')) sscanf(Opt('s'),"%lx",&startaddr);
	adr = startaddr;
	dislarm_init();
	set_force_thumb(is_thumb);

	init_symbol();
	read_symbol(argv[1]);

	stat(argv[1],&statbuf);
		filesize= statbuf.st_size;
		endaddr = startaddr + filesize;

	Ropen(argv[1]);
	
	off = 0;
	while(off<filesize) {
		oplen = dislarm(adr);
		adr += oplen;
		off += oplen;
	}
	
	Rclose();
	return 0;
}
#endif

int	arm_init(int isthumb)
{
	is_thumb=isthumb;
	dislarm_init();
	set_force_thumb(is_thumb);
	return 0;
}

int disasm_print(uchar *buf,int size,int adr)
{
	if( size <= 0) return 0;
	if(	size >=INS_BUFSIZE) {
		size = INS_BUFSIZE;
	}
	memcpy(ins_buf,buf,size);
	ins_adr = adr;
	return dislarm(adr);
}
