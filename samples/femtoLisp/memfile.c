#include <string.h>
#include "memfile.h"

extern const char system_lsp[];

FILE memp[1];

#if 0
struct __sFILE {
  unsigned char *_p;	/* current position in (some) buffer */
  int	_r;		/* read space left for getc() */
  int	_w;		/* write space left for putc() */
  short	_flags;		/* flags, below; this FILE is free if 0 */
  short	_file;		/* fileno, if Unix descriptor, else -1 */
  struct __sbuf _bf;	/* the buffer (at least 1 byte, if !NULL) */
  int	_lbfsize;	/* 0 or -_bf._size, for inline putc */
}
#endif
FILE *mem_fopen(const char *file,const char *mode)
{
	 FILE *fp = memp;
	 memset(fp,0,sizeof(FILE));
	 fp->_p = (unsigned char*) system_lsp;
	 fp->_r = strlen( (char *)fp->_p);
	 fp->_file = 'M';
	 return fp;
}
int   mem_fread(char *buf,int size,int n,FILE *fp)
{
	 size *= n;

	 if(fp->_file != 'M') return EOF;
	 if(size > fp->_r) return EOF;

	 char *src = (char *)fp->_p;
	 memcpy(buf,src,size);
	 {
		  fp->_p += size;
		  fp->_r -= size;
	 }
	 return size;
}
int   mem_fclose(FILE *fp)
{
	return 0; 
}
int   mem_feof(FILE *fp)
{
	 if(fp->_file != 'M') return EOF;
	 if(fp->_r <= 0) return EOF;
	 return 0;
}
int   mem_fgetc(FILE *fp)
{
	 if(fp->_file != 'M') return EOF;
	 int c = fp->_p[0];
	 int size = 1;
	 {
		  fp->_p += size;
		  fp->_r -= size;
	 }
	 return c;
}
int   mem_ungetc(int c,FILE *fp)
{
	 if(fp->_file != 'M') return EOF;
	 int size = -1;
	 {
		  fp->_p += size;
		  fp->_r -= size;
	 }
	 return c;
}


