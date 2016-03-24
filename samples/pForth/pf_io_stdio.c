//#include "../pf_all.h"

int  user_getc(void);
void user_putc(char c);
//void user_write_console(const char *s,int len);

/* Default portable terminal I/O. */
int  sdTerminalOut( char c )
{
	user_putc(c);
	return 1;
//	return putchar(c);
}
/* We don't need to echo because getchar() echos. */
int  sdTerminalEcho( char c )
{
	return 0;
}
int  sdTerminalIn( void )
{
	int c;
//	return getchar();
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
int  sdQueryTerminal( void )
{
	return 0;
}

int  sdTerminalFlush( void )
{
//#ifdef PF_NO_FILEIO
	return -1;
//#else
//	return fflush(PF_STDOUT);
//#endif
}

void sdTerminalInit( void )
{
}
void sdTerminalTerm( void )
{
}

