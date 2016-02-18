#ifndef	rs232c_h_
#define	rs232c_h_

int		RS_putdata(	unsigned char *buf , int cnt );
int		RS_putc(int c);
int		RS_keyInput(void);
int		RS_getdata(unsigned char *buf,int cnt);
int		RS_checkdata();
int		RS_init(char *portno,int baudrate);
void	RS_exit(void);

void	RS_terminal(char *port_no,int baudrate);
void 	RS_echohost(char *port_no,int baudrate);
void	RS_usbhost(char *port_no,int baudrate);
void 	RS_benchmark(char *port_no,int baudrate);
int 	RS_printinfo(int baudrate);


int		RS_getc();

#endif
