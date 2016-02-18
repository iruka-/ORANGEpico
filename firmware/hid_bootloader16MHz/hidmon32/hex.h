#ifndef _hex_H_
#define _hex_H_
#include "mphidflash.h"
ErrorCode	hexOpen(char * const filename);
ErrorCode	putData(unsigned int addr,int len,char *data);
ErrorCode	execCode(unsigned int addr);
ErrorCode	getData(unsigned int addr,int len,char *result);
ErrorCode	hexWrite(const char verify);
void		hexClose(void);

ErrorCode resetDevice(void);
#endif
