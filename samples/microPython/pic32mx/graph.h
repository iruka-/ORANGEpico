#ifndef _graph_h_
#define _graph_h_

void	pset(int x,int y);
void	preset(int x,int y);
void	prev(int x,int y);

void	gr_line(int x1,int y1,int x2,int y2,int c);
void	gr_circle( int cx,int cy,int r,int c);

void	gr_scrollup(int y);
void	gr_crlf(void);
void	gr_curadv(void);
void	gr_locate(int cx,int cy);
void	gr_putch_xy(int ch,int cx,int cy);
void	gr_putch(int ch);
void	gr_puts(char *str);

void	gr_test();

#endif

