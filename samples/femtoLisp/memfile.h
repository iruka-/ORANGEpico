#ifndef	_memfile_h_
#define	_memfile_h_

#include <stdio.h>

// file system replacement
FILE *mem_fopen(const char *file,const char *mode);
int   mem_fread(char *buf,int size,int n,FILE *fp);
int   mem_fclose(FILE *fp);
int   mem_feof(FILE *fp);
int   mem_fgetc(FILE *fp);
int   mem_ungetc(int c,FILE *fp);

#undef feof
#define fopen(file_,mode_)         mem_fopen(file_,mode_)
#define fread(buf_,size_,n_,file_) mem_fread(buf_,size_,n_,file_)
#define fclose(file_)              mem_fclose(file_)
#define feof(file_)                mem_feof(file_)
#define ungetc(c_, file_)          mem_ungetc(c_, file_)

//
typedef unsigned int uint32_t;

//for DEBUG
void       zz(const char *file,int line);
#define ZZ zz(__FILE__,__LINE__);

#endif

