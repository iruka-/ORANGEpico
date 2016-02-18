/* util.c */
void memdump_print (void *ptr, int len, int off);
//int QueryAVR (cmdBuf *cmd, uchar *buf, int reply_len);
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
int UsbPoll (uint *buf);
int UsbPollAll (uint *buf);
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

int UsbInit (int verbose, int enable_bulk, char *serial,char *baud);
int UsbExit (void);
int	UsbGetDevID(void);
int	UsbGetDevVersion(void);
int	UsbGetDevCaps(int *flash_size,int *flash_used);

int UsbFlashLock(int lockf);
int UsbTestTarget(int size);
int UsbTestUpload(int size);

int	check_region(int addr,int size);
