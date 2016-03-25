#include <p32xxxx.h>			// always in first place to avoid conflict with const.h ON
#include <stdlib.h>			// always in first place to avoid conflict with const.h ON


#include <stdio.h>			// always in first place to avoid conflict with const.h ON
#include <stdarg.h>
#include "config.h"
#include "util.h"


/********************************************************************
 *	整数べき (?)
 ********************************************************************
 */
int pow(int x,int y)
{
	int i;
	int ret=1;
	for(i=0;i<y;i++) ret=ret*x;
	return ret;
}

void decprint(int d)
{
   	char buf[80];
   	sprintf(buf,"%d\n",d);
	user_puts(buf);
}


int FGETC(FILE *fp)
{
	int c;
	while(1) {
		c = user_getc();
		if(c!=(-1)) {
			user_putc(c);
			if(c==0x0d) 
				user_putc(0x0a);
			return c;
		}
	}
}

//	ダミーscanf
int fscanf(FILE *fp,const char *format,...)
{
        va_list args;
        
        va_start( args, format );
//        return print( 0, format, args );
	return 0;
}


/********************************************************************
 *	
 ********************************************************************
 */
void zz(const char *file,int line)
{
	user_puts("ZZ:");
	user_puts(file);
	decprint(line);
}
