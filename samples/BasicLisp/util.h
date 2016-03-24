#ifndef _UTIL_H_
#define _UTIL_H_

#define	_MIPS32 __attribute__((nomips16))

void    led_on();
void    led_off();
void	led_flip();

void	led_test();
void	led_blink();

//	コンソール(UART/NTSC)共通 文字出力.
void user_putc(char c);
void user_puts(const char *s);
void user_write_console(const char *s,int len);
//	出力モード: UARTにも同時出力する(1)かしない(0)かを指定する.
int  user_stdout_mode(int f);

//  デバッグ.
void zz(const char *file,int line);

#endif

