#include <p32xxxx.h>			// always in first place to avoid conflict with const.h ON
#include <stdlib.h>			// always in first place to avoid conflict with const.h ON
#include <stdio.h>
#include <sys/time.h>

#include "config.h"
#include "util.h"

// gettimeofday, settimeofday - get / set time
//int gettimeofday(struct timeval *tv, struct timezone *tz)
int gettimeofday(struct timeval *tv, void *tz)
{
	return 0;
}
//  access - check real user's permissions for a file
int access(const char *pathname, int mode)
{
	return 0;
}

int chdir(const char *path)
{
	return 0;
}
void exit(int rc)
{
	while(1) {
	}
	(void) rc;
}

int scanf(const char *fmt, ...)
{
	return 0;
}
int sscanf(const char *buf,const char *fmt, ...)
{
	return 0;
}
char *getenv(const char *name)
{
	return NULL;
}
int system(const char *cmd)
{
	printf("!system(%s)\n",cmd);
	return 0;
}
/********************************************************************
 *	
 ********************************************************************
 */
