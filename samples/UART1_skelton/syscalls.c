/********************************************************************
 *	printfやputchar関数の出力先を ユーザー実装の_user_putc() にする。
 ********************************************************************
 */

#include <stdio.h>
#include "serial1.h"

#define	EXTERN	extern char

void user_putc(char c)
{
    Serial1WriteChar(c);
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
	if(file<3) {
		while(len) {
			user_putc(*ptr++);
			len--;
		}
	}
	return len;	
}

//	read()は何もしない.
int read (int file,char * ptr,int    len)
{
	return 0;
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
