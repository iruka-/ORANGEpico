#include <p32xxxx.h>
#include <plib.h>

/* Global variables follow */

#ifdef MAIN
//	unsigned char RAM[0x7000]; // 28KB RAM
	volatile unsigned char VRAM[0x0400];
	volatile unsigned char read825x[0x10];
	unsigned long g_ntscVisible;
	unsigned long g_Vsync;
	volatile unsigned long g_timer1;
	volatile unsigned long g_TMR1h;
	volatile unsigned int prompt_8255=0;
	volatile unsigned int tempo_8255=0;
	volatile unsigned char keymap[0x10];
	volatile unsigned long ps2data=0;
	volatile unsigned long ps2status=1;
#else
	extern unsigned char RAM[0x7000];
	extern volatile unsigned char VRAM[0x0400];
	extern volatile unsigned char read825x[0x10];
	extern unsigned long g_ntscVisible;
	extern unsigned long g_Vsync;
	extern volatile unsigned long g_timer1;
	extern volatile unsigned long g_TMR1h;
	extern unsigned int prompt_8255;
	extern unsigned int tempo_8255;
	extern unsigned char keymap[0x10];
	extern unsigned long ps2data;
	extern unsigned long ps2status;
#endif

/* Prototyping */

void main();

void execZ80infinite();
void execZ80code();
void resetZ80();
void intZ80(unsigned char i8);
void nmrZ80();

void initNtsc();
void ntscVisible(unsigned long visible);
unsigned long ntscVsync();

void init8253();
void write8253(unsigned int addr, unsigned char data);
unsigned char read8253(unsigned int addr);
void inc8253timer2();

void init8255();
void write8255(unsigned int addr, unsigned char data);

void ps2_init();
void ps2_received(unsigned char data);

void loadTape();

// Debug macro
#define showHex(pos,val)\
	VRAM[pos]=(((val)>>4)<10)?(0x20+((val)>>4)):(0x01-10+((val)>>4));\
	VRAM[(pos)+1]=(((val)&15)<10)?(0x20+((val)&15)):(0x01-10+((val)&15))

