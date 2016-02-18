#ifndef	_include_monit_h_
#define	_include_monit_h_

//#include "../../firmware/monitor/hidcmd.h"
#include "hidcmd.h"
#include "packet.h"

#ifdef	SH2A
int	bswap16(int i);
int	bswap32(int	i);

#define	BSWAP16(i)	bswap16(i)
#define	BSWAP32(i)	bswap32(i)
#else
#define	BSWAP16(i)  (i)
#define	BSWAP32(i)  (i)
#endif

//typedef	unsigned char  uchar;

typedef	struct {
	uchar  	 rsv[3];
	uchar  	 report_id;
	
	uchar  	 cmd;			//+00
	uchar  	 subcmd;		//+01
	unsigned short size;	//+02
	unsigned int   adr;		//+04
	uchar    data[64-8];	//+08
} cmdBuf;

//
//	cmd.
//
enum {
	CMD_PING 	= HIDASP_TEST,
	CMD_SETPAGE = HIDASP_SET_MODE,
	CMD_POKE = HIDASP_POKE,
	CMD_PEEK = HIDASP_PEEK,
};

//
//	area.
//
enum {
	AREA_RAM    = 0   ,
	AREA_EEPROM = 0x00,
	AREA_PGMEM  = 0x00,
};

enum {
	MEM_BYTE = 0,
	MEM_HALF = 1,
	MEM_WORD = 2,
};

typedef	struct {
	char *name;
	void (*func)();
} CmdList;


/*
 *	オプション文字列チェック
 */
#ifdef MAIN_DEF
char  *opt[128];	/* オプション文字が指定されていたら、
						その文字に続く文字列を格納、
						指定がなければNULLポインタを格納	*/
#else
extern char  *opt[128];
#endif

/*
 *	オプション文字列チェック
 *		optstring に含まれるオプション文字は、
 *				  後続パラメータ必須とみなす。
 */
#define Getopt(argc,argv,optstring)           		\
 {int i;int c;for(i=0;i<128;i++) opt[i]=NULL; 		\
   while( ( argc>1 )&&( *argv[1]=='-') ) {    		\
	 c = argv[1][1] & 0x7f;   						\
       opt[c] = &argv[1][2] ; 						\
       if(( *opt[c] ==0 )&&(strchr(optstring,c))) {	\
         opt[c] = argv[2] ;argc--;argv++;          	\
       }                        					\
     argc--;argv++;           						\
 } }

#define IsOpt(c) ((opt[ c & 0x7f ])!=NULL)
#define   Opt(c)   opt[ c & 0x7f ]

#define Ropen(name) {ifp=fopen(name,"rb");if(ifp==NULL) \
{ printf("Fatal: can't open file:%s\n",name);exit(1);}  \
}

#define Wopen(name) {if(strcmp(name, "con")==0) ofp=stdout; else {ofp=fopen(name,"wb");if(ofp==NULL) \
{ printf("Fatal: can't create file:%s\n",name);exit(1);}}  \
}

#define Rclose()  fclose(ifp)
#define Wclose()  fclose(ofp)


#define	ZZ	printf("%s:%d: ZZ\n",__FILE__,__LINE__);

/* monit.c */
char *sp_skip (char *buf);
double AnalogCalcVoltage (int Tref, int Tin);
int analog_chk_trig (int val, int oldval, int trig_mode);

int calc_ypos (int i);
int cmdExecTerminal (void);
int getAnalogVolt (double *val, int f);
int getRegistance (double *val, int f);
int get_arg (char *buf);
int is_space (int c);
int main (int argc, char **argv);
int radix2scanf (char *s, int *p);
int str_comp (char *buf, char *cmd);

void analog_conv (uchar *sample, int poll_mode);
void chop_crlf (char *p);
void cmdAin (char *buf);
void cmdAinDebug (char *buf);
void cmdBench (char *buf);
void cmdBoot (char *buf);
void cmdDump (char *buf);
void cmdEdit (char *buf);
void cmdErase (char *buf);
void cmdFill (char *buf);
void cmdFlash (char *buf);
void cmdGraph (char *buf);
void cmdHelp (char *buf);
void cmdList (char *buf);
void cmdMonit (char *buf);
void cmdPoll (char *buf);
void cmdPort (char *buf);
void cmdPortPrintAllCr (int count, unsigned char *pinbuf);
void cmdPortPrintOne (char *name, int adrs, int val);
void cmdPortPrintOneCr (int count, char *name, int adrs, int val);
void cmdPortPrintOne_b (char *name, int adrs, int val, int mask);
void cmdQuestion (char *buf);
void cmdQuit (char *buf);
void cmdReg (char *buf);
void cmdRun (char *buf);
void cmdSleep (char *buf);
void cmdSync (char *buf);
void cmdUser (char *buf);
void draw_PortName (int adr, int poll_mode);
void draw_sample (int x, int y, int val);
void plot_analog (int x, int y, int val, int old_val);
void plot_signal (int x, int y, int v, int diff);
void scan_args (int arg_cnt);
void usage (void);
void vline2 (int x0, int y0, int x1, int y1, int col);

//------------------------------------------------------
#ifdef	_LINUX_
//	MSDOSに存在する関数のダミー版をプロトタイプ宣言する:
void Sleep(int mSec);			// wait mSec.
void strupr(char *s);			// 文字列を全部大文字化.
int	stricmp(char *s1,char *s2);	// 大文字小文字の区別をしない文字列比較.
int getch(void);
int kbhit(void);

#endif


#endif	//_include_monit_h_

