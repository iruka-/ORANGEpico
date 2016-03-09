#ifndef _DMA1_H_
#define _DMA1_H_

#include "util.h"

void	dma_init();
void 	_MIPS32 dma_kick(int cnt);
void	_user_putc(int c);
void	_user_puts(char *s);

#endif
