#ifndef _UTIL_H_
#define _UTIL_H_


#define	_MIPS32_ __attribute__((nomips16,noinline))
#define	_MIPS32 __attribute__((nomips16))

void    led_on();
void    led_off();
void	led_flip();

void	led_test();
void	led_blink();

/*
void	wait_ms(int ms);
void	delay_us(int us);
void	wait_nloop(int n);
*/

#endif

