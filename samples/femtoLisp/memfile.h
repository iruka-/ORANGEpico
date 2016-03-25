#ifndef	_memfile_h_
#define	_memfile_h_

typedef unsigned int uint32_t;

FILE *mem_fopen(const char *file,const char *mode);
int   mem_fread(FILE *fp,);
int   mem_fclose(FILE *fp);

#define fopen(file_,mode_) mem_fopen(file_,mode_)


//for DEBUG
void       zz(const char *file,int line);
#define ZZ zz(__FILE__,__LINE__);

#endif

