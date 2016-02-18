#include <p32mx/p32mx_core.h>
#include <p32mx/p32mx_port.h>

#if	1
void connect_led_on()	{
		LAT_SET(1)  = 1 << 15;
}
void connect_led_off() {
		LAT_CLR(1)  = 1 << 15;
}
void busy_led_on() {
}
void busy_led_off() {
}
#endif

#if	0
// Olimex PIC32-PINGUINO-MX220
//    LED1 (Green) D13 = RB15 Active H (1: Light on)
//    LED2 (Red  ) D9  = RA10 Active H (1: Light on)
//    BUT          D8  = RB7  Active L (0 : Pressed)
int ledmain(int cnt,int spd)
{
	int i;
	TRIS_CLR(1) = 1 << 15;
	TRIS_CLR(0) = 1 << 10;
	LAT_CLR(1)  = 1 << 15;
	LAT_SET(0)  = 1 << 10;
	spd *= 100;
	for (i=0;i<cnt;i++) {
		delay_us(spd);
		LAT_INV(1)  = 1 << 15;
	}
}

#else
void led_blink();

int ledmain(int cnt,int spd)
{
	int i;
	spd *= 100;
	for (i=0;i<cnt;i++) {
		delay_us(spd);
		led_blink();
	}
}
#endif

#if	0
void	bzero (void	*dst, unsigned nbytes)
{
	char *t = dst;
	while(nbytes>0) {
		*t++ = 0;
		nbytes--;
	}
}
#endif
