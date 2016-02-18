#ifndef	packet_h_
#define	packet_h_

#include "hidcmd.h"

#define	RX_DEFFERED		(1)		//遅延読み込みを許可.

//	for DEBUG
#define	USE_LOGFILE		(0)		// LOGFILE_print()をファイルに出力する.

typedef unsigned char   uchar;          // 8-bit
typedef unsigned char   byte;           // 8-bit

typedef unsigned short  ushort;         // 16-bit
typedef unsigned short  word;           // 16-bit

typedef unsigned int    uint;           // 32-bit
typedef unsigned int    dword;          // 32-bit


int		hidReadPoll(uchar *buf, int Length, int id);
int		hidWriteBuffer(uchar *buf, int len);
int		hidWriteBuffer2(uchar *buf, int len);
int		hidReadBuffer(uchar *buf, int len);

int		hidCommand(int cmd,int arg1,int arg2,int arg3);
int		hidPokeMem(int addr,int data0,int mask);
int		hidPeekMem(int addr);
int		hidasp_init(int type, const char *serial, const char *baud);
void	hidasp_close();
int		hidasp_cmd(const unsigned char cmd[4], unsigned char res[4]);
void	memdump(char *msg, char *buf, int len);
int		hidasp_list(char * string);
int 	UsbIsMary(void);

#define	PACKET_SIZE		60	//64

#endif

