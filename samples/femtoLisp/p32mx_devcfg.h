#ifndef _JZ_P32MX_DEVCFG_H_
#define _JZ_P32MX_DEVCFG_H_
#include <stdint.h>

#if defined(__32MX1XX__) || defined(__32MX2XX__)
  #define DEVCFG3          (*(unsigned *)0xBFC00BF0)
  #define DEVCFG2          (*(unsigned *)0xBFC00BF4)
  #define DEVCFG1          (*(unsigned *)0xBFC00BF8)
  #define DEVCFG0          (*(unsigned *)0xBFC00BFC)
#elif defined(__32MX3XX__) || defined(__32MX4XX__)
  #define DEVCFG3          (*(unsigned *)0xBFC02FF0)
  #define DEVCFG2          (*(unsigned *)0xBFC02FF4)
  #define DEVCFG1          (*(unsigned *)0xBFC02FF8)
  #define DEVCFG0          (*(unsigned *)0xBFC02FFC)
#endif

    #define DEVCFG3_USERID                 0x00
    #define DEVCFG3_PMDL1WAY               0x1C
    #define DEVCFG3_IOL1WAY                0x1D
    #define DEVCFG3_FUSBIDIO               0x1E
    #define DEVCFG3_FVBUSONIO              0x1F
    #define DEVCFG2_FPLLIDIV               0x00
    #define DEVCFG2_FPLLMUL                0x04
    #define DEVCFG2_UPLLIDIV               0x08
    #define DEVCFG2_UPLLEN                 0x0F
    #define DEVCFG2_FPLLODIV               0x10
    #define DEVCFG1_FNOSC                  0x00
    #define DEVCFG1_FSOSCEN                0x05
    #define DEVCFG1_IESO                   0x07
    #define DEVCFG1_POSCMOD                0x08
    #define DEVCFG1_OSCIOFNC               0x0A
    #define DEVCFG1_FPBDIV                 0x0C
    #define DEVCFG1_FCKSM                  0x0E
    #define DEVCFG1_WDTPS                  0x10
    #define DEVCFG1_WINDIS                 0x16
    #define DEVCFG1_FWDTEN                 0x17
    #define DEVCFG1_FWDTWINSZ              0x18
    #define DEVCFG0_DEBUG                  0x00
    #define DEVCFG0_JTAGEN                 0x02
    #define DEVCFG0_ICESEL                 0x03
    #define DEVCFG0_PWP                    0x0A
    #define DEVCFG0_BWP                    0x18
    #define DEVCFG0_CP                     0x1C
    #define DEVCFG0_FDEBUG                 0x00

  #define CFGCON           (*(volatile unsigned *)0xBF80F200)
  #define DDPCON           (*(volatile unsigned *)0xBF80F200)
  #define DEVID            (*(volatile unsigned *)0xBF80F220)

    #define CFGCON_TDOEN                   0x00
    #define CFGCON_TROEN                   0x02
    #define CFGCON_JTAGEN                  0x03
    #define CFGCON_PMDLOCK                 0x0C
    #define CFGCON_IOLOCK                  0x0D
    #define DDPCON_TDOEN                   0x00
    #define DDPCON_TROEN                   0x02
    #define DDPCON_JTAGEN                  0x03
    #define DDPCON_PMDLOCK                 0x0C
    #define DDPCON_IOLOCK                  0x0D
    #define DEVID_DEVID                    0x00
    #define DEVID_VER                      0x1C

#define DEVID_IS_1XX(id)	((id) & 0x0FF00FFF == 0x04A00053)
#define DEVID_IS_2XX(id)	((id) & 0x0FF00FFF == 0x04D00053)
#define DEVID_IS_3XX(id)	((id) & 0x0FFC0FFF == 0x09000053)
#define DEVID_IS_4XX(id)	((id) & 0x0FFC0FFF == 0x09400053)

  #define SYSKEY           (*(volatile unsigned *)0xBF80F230)
  #define SYSKEY_CLR       (*(volatile unsigned *)0xBF80F234)
  #define SYSKEY_SET       (*(volatile unsigned *)0xBF80F238)
  #define SYSKEY_INV       (*(volatile unsigned *)0xBF80F23C)

  struct devcfg3 {
    unsigned USERID:16;
    unsigned reserved1:12;
    unsigned PMDL1WAY:1;	// only 1xx/2xx
    unsigned IOL1WAY:1;		// only 1xx/2xx
    unsigned FUSBIDIO:1;	// only 1xx/2xx
    unsigned FVBUSONIO:1;	// only 1xx/2xx
  };
  struct devcfg2 {
    unsigned FPLLIDIV:3;
    unsigned reserved1:1;
    unsigned FPLLMUL:3;
    unsigned reserved2:1;
    unsigned UPLLIDIV:3;
    unsigned reserved3:4;
    unsigned UPLLEN:1;
    unsigned FPLLODIV:3;
    unsigned reserved4:13;
  };
  struct devcfg1 {
    unsigned FNOSC:3;
    unsigned reserved1:2;
    unsigned FSOSCEN:1;
    unsigned reserved2:1;
    unsigned IESO:1;
    unsigned POSCMOD:2;
    unsigned OSCIOFNC:1;
    unsigned reserved3:1;
    unsigned FPBDIV:2;
    unsigned FCKSM:2;
    unsigned WDTPS:5;
    unsigned reserved4:1;
    unsigned WINDIS:1;		// only 1xx/2xx
    unsigned FWDTEN:1;
    unsigned FWDTWINSZ:2;	// only 1xx/2xx
    unsigned reserved5:6;
  };
#if defined(__32MX1XX__) || defined(__32MX2XX__)
  struct devcfg0 {
    unsigned DEBUG:2;
    unsigned JTAGEN:1;
    unsigned ICESEL:2;
    unsigned reserved1:5;
    unsigned PWP:6;
    unsigned reserved2:8;
    unsigned BWP:1;
    unsigned reserved3:3;
    unsigned CP:1;
    unsigned reserved4:2;
    unsigned reserved5:1;
  };
#elif defined(__32MX3XX__) || defined(__32MX4XX__)
  struct devcfg0 {
    unsigned DEBUG:2;
    unsigned reserved0:1;
    unsigned ICESEL:1;
    unsigned reserved1:8;
    unsigned PWP:8;
    unsigned reserved2:4;
    unsigned BWP:1;
    unsigned reserved3:3;
    unsigned CP:1;
    unsigned reserved4:2;
    unsigned reserved5:1;
  };
#endif

#ifdef _BUILD_CONFIG
/*
#pragma config PMDL1WAY = OFF
#pragma config IOL1WAY = OFF
#pragma config FUSBIDIO = ON
#pragma config FVBUSONIO = ON

#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_16
#pragma config UPLLIDIV = DIV_5
#pragma config UPLLEN = OFF
#pragma config FPLLODIV = DIV_2

#pragma config FNOSC = PRIPLL
#pragma config FSOSCEN = OFF
#pragma config IESO = OFF
#pragma config POSCMOD = HS
#pragma config OSCIOFNC = OFF
#pragma config FPBDIV = DIV_2
#pragma config FCKSM = CSDCMD
#pragma config WDTPS = PS1
#pragma config WINDIS = OFF
#pragma config FWDTEN = OFF
#pragma config FWDTWINSZ = WISZ_25

#pragma config JTAGEN = OFF
#pragma config ICESEL = ICS_PGx1
#pragma config PWP = OFF
#pragma config BWP = OFF
#pragma config CP = OFF
*/

#define IDIV_1	0
#define IDIV_2	1
#define IDIV_3	2
#define IDIV_4	3
#define IDIV_5	4
#define IDIV_6	5
#define IDIV_10	6
#define IDIV_12	7

#define MUL_15  0
#define MUL_16  1
#define MUL_17  2
#define MUL_18  3
#define MUL_19  4
#define MUL_20  5
#define MUL_21  6
#define MUL_24	7

#define ODIV_1    0
#define ODIV_2    1
#define ODIV_4    2
#define ODIV_8    3
#define ODIV_16   4
#define ODIV_32   5
#define ODIV_64   6
#define ODIV_256  7


#define FRCDIV	7	/* Fast RC without PLL /N */
#define FRC16	6	/* Fast RC without PLL /16 */
#define LPRC	5	/* Low-Power RC Osillator */

//ˆÈ‰ºsystem.c‚ÉŠÜ‚Ü‚ê‚é.
#ifndef	SOSC
#define SOSC	4	/* Secondary Osillator */
#define POSCPLL	3	/* Primary Osillator with PLL */
#define POSC	2	/* Primary Osillator without PLL */
#endif

#define FRCPLL	1	/* Fast RC with PLL */
#define FRC	0	/* Fast RC without PLL */

#define CSDCMD	3	/* Clock Switching Disabled / Clock Moniter Disabled */
#define CSECMD	1	/* Clock Switching Enabled  Clock Moniter Disabled */
#define CSECME	0	/* Clock Switching Enabled  Clock Moniter Enabled */

#define WISZ_25	3	/* 25 % */
#define WISZ_37	2	/* 37.5 % */
#define WISZ_50	1	/* 50 % */
#define WISZ_75	0	/* 75 % */

#if defined(__32MX1XX__) || defined(__32MX2XX__)
#define ICS_PGx1	3
#define ICS_PGx2	2
#define ICS_PGx3	1
#define ICS_OFF		0

#define PWP_OFF		0x3f
#define PWP_1K		(0x3f - 1)
#define PWP_4K		(0x3f - 4)
#define PWP_16K		(0x3f - 16)
#define PWP_SIZE(x)	(0x3f - (x))

#elif defined(__32MX3XX__) || defined(__32MX4XX__)

#define ICS_PGx1	0
#define ICS_PGx2	1

#define PWP_OFF		0xff
#define PWP_4K		(0xff - 1)
#define PWP_16K		(0xff - 4)
#define PWP_SIZE(x)	(0xff - (x)/4)

#endif
#endif /* _BUILD_CONFIG */

/* sample */
#if 0
    /* for using 12 MHz Crystal */
    #define CONFIG_FPLLIDIV		IDIV_3 
    #define CONFIG_UPLLIDIV		IDIV_3
#endif
#if 0
/* for using 16 MHz Crystal */
    #define CONFIG_FPLLIDIV		IDIV_4
    #define CONFIG_UPLLIDIV		IDIV_4
#endif
#if 0
/* for using Fast RC osillator */
    #define CONFIG_FNOSC	FRCPLL  /* Fast RC with PLL */
#endif

#if 0
/* for useing Watchdog Timer */
    #define CONFIG_FWDTEN		1	/* Watchdog Timer is enabled */
    #define CONFIG_WDTPS		20  /* 1/1M */
#endif

#if 0
/* for using JTAG */
    #define CONFIG_JTAGEN		0 /* Disabled */
/* for selecting PGEx */
    #define CONFIG_ICSEL    ICS_PGx3  /* PGEx3 */
#endif

#if 0
/* for bootloader */
   #define CONFIG_BWP      0  /* Protected */
   #define CONFIG_CP       0  /* Protected */

/* for second bootloader */
    #define CONFIG_PWP      PWP_SIZE(12) /* lower 12KB is Protected */
#endif

#endif /* _JZ_P32MX_DEVCFG_H_ */
