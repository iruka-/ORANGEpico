#ifndef	util_h_
#define	util_h_

#define	RX_DEFFERED		(1)		//遅延読み込みを許可.

//	for DEBUG
#define	USE_LOGFILE		(0)		// LOGFILE_print()をファイルに出力する.

typedef unsigned char   uchar;          // 8-bit
typedef unsigned char   byte;           // 8-bit

typedef unsigned short  ushort;         // 16-bit
typedef unsigned short  word;           // 16-bit

typedef unsigned int    uint;           // 32-bit
typedef unsigned int    dword;          // 32-bit

int		hidCommand(int cmd,int arg1,int arg2,int arg3);
int		hidasp_cmd(const unsigned char cmd[4], unsigned char res[4]);
int	 	UartInit(int com_n);
int 	UartExit(void);

#define	ZZ	printf("%s:%d: ZZ\n",__FILE__,__LINE__);

/* util.c */
void memdump_print (void *ptr, int len, int off);
void memdump(char *msg, uchar *buf, int len);
int dumpmem (int adr, int arena, int size, unsigned char *buf);
int pokemem (int adr, int arena, int data0, int data1);
int hid_ping (int i);
void UsbBench (int cnt, int psize);
void UsbDump (int adr, int arena, int cnt);
int UsbDisasm (int adr, int arena, int cnt);
int UsbRead (int adr, int arena, uchar *buf, int size);
int UsbPeek (int adr, int arena);
int UsbFlash(int adr,int arena,uchar *buf,int size);

int UsbSetPoll_slow (int adr, int arena);
int UsbPoll_slow (void);
int UsbSetPoll (int adr, int mode);
int UsbPoll (uchar *buf);
int UsbAnalogPoll(int mode,unsigned char *abuf);
int UsbPoke (int adr, int arena, int data, int mask);
void UsbPoke_b (int adr, int arena, int bit, int mask);
void UsbCheckPollCmd (void);
int	UsbSyncCmd(int cnt);

int UsbBootTarget(int resetpc,int bootflag,int vector);
int UsbExecUser(int arg);
int UsbReadString(uchar *buf);
int UsbPutKeyInput(int key);
int UsbEraseTargetROM(int adr,int size);

int UsbInit (void);
int UsbExit (void);
int	UsbGetDevID(void);
int	UsbGetDevVersion(void);
int	UsbGetDevCaps(int *flash_size,int *flash_used);

int UsbFlashLock(int lockf);
int UsbTestUpload(int size);

int	check_region(int addr,int size);
int UartInit(int com_n);

#endif
