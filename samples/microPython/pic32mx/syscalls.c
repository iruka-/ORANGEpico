/********************************************************************
 *	printfやputchar関数の出力先を ユーザー実装の_user_putc() にする。
 ********************************************************************
 */

#include <stdio.h>
#include "util.h"

#include "serial1.h"
#include "ps2keyb.h"
#include "graph.h"

#define	EXTERN	extern char

static int user_stdout_mode_f=0;

/********************************************************************
 *
 ********************************************************************
 */
void user_putc(char c)
{
	if(	user_stdout_mode_f ){	// UARTとNTSCの両方に出力する.
	    Serial1WriteChar(c);
	}
	gr_putch(c);				// NTSCのみ.
}
/********************************************************************
 *
 ********************************************************************
 */
void user_puts(const char *s)
{
	while(*s) {
	    user_putc(*s++);
	}
}
/********************************************************************
 *
 ********************************************************************
 */
void user_write_console(const char *s,int len)
{
	int i;
	for(i=0;i<len;i++) {
	    user_putc(*s++);
	}
}
/********************************************************************
 *
 ********************************************************************
 */
int user_stdout_mode(int f)
{
	int ret = user_stdout_mode_f;
	user_stdout_mode_f = f;
	return ret;
}

/********************************************************************
 *
 ********************************************************************
 */
int user_getc(void)
{
	int c;
	// from UART1
	if(	Serial1Available()) {
		c = Serial1GetKey();
//		printf("[%02x]",c);
		return c;
	}
	
	// from PS/2 Key
	c = kbd_getchar();		// : ASCII コードを取得.
	if(c != 0) {
		return c;
	}
	return -1;
}

//	open()したファイルハンドルは、コンソール(tty)であるかどうか知る.
//	ここでは fd <= 2 のときは tty であることを通知する.
int isatty (int fd)
{
	return (fd <= 2) ? 1 : 0;  /* one of stdin, stdout, stderr */
}

//	open()は、常に 2 (stdout) を返す. (printf専用)
int open (const char * path,int  flags,...)
{
	return 2;	//未実装.
}

//	close()は、常に成功する.
int close (int file)
{
	return 0;
}

//	lseek()は何もしない.
int lseek (int file,int ptr,int dir)
{
	return 0;
}

//	write()は、printfの出力を受け取り、ユーザー実装の１文字出力を呼ぶ.
int write (int file,char * ptr,int    len)
{
	//仮実装.
	if(file<=3) {
		while(len) {
			user_putc(*ptr++);
			len--;
		}
	}
	return len;	
}

//	read()はUART1からlenバイト(実際は1文字だけ)読み取る.
int read (int file,char * ptr,int    len)
{
#if	0
	//仮実装.
	if(file<3) {
//		printf("read(%d,%x,%d):\n",file,(int)ptr,len);
		while(1) {
			int c = user_getc();
			if( c!=(-1)) {
				*ptr++ = c;
				//printf("<%02x>",c);
				return 1;	// 1文字だけ入力されたら帰る.
			}
		}
	}
//	printf("read(%d,%x,%d)=%d\n",file,(int)ptr,len,len0);
	return len;	
#else
	return len;
#endif
}

//	fstatは何もしない.
int fstat (int file,char * ptr,int    len)
{
	return 0;
}


EXTERN	_heap	;/* end address for the .bss section.     */
EXTERN	_splim	;/* end address for the .bss section.     */

//char buf[8192];

static char *sbrk_ptr = (char *) &_heap;
//static char *sbrk_ptr = buf;

//	malloc()から呼び出される可能性がある.
char *sbrk(int size)
{
    char *ret;

    if(0){//sbrk_ptr + size > &_splim ) {
		return (char *) (-1);	// メモリーがない.
    }else {
        ret = sbrk_ptr;
        sbrk_ptr += size;		// sbrk_ptrを進める.
    }
    return ret;
}

/********************************************************************
 *
 ********************************************************************
 */
