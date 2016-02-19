#ifndef _JZ_P32MX_DEVCFG_BUILD_H_
#define _JZ_P32MX_DEVCFG_BUILD_H_
#include <stdint.h>

#ifdef _BUILD_CONFIG

#ifndef CONFIG_USERID
    #define CONFIG_USERID	0xffff
#endif
#ifndef CONFIG_PMDL1WAY
    #define CONFIG_PMDL1WAY	0  /* Multiple Reconfiguration Enabled */
#endif
#ifndef CONFIG_IOL1WAY
    #define CONFIG_IOL1WAY	0  /* Multiple Reconfiguration Enabled */
#endif
#ifndef CONFIG_FUSBIDIO
    #define CONFIG_FUSBIDIO	0  /* USBID is port */
#endif
#ifndef CONFIG_FVBUSONIO
    #define CONFIG_FVBUSONIO	0  /* Vbuson is port */
#endif

#if defined(__32MX1XX__) || defined(__32MX2XX__)
struct devcfg3 _config3 __attribute__((section(".config_BFC00BF0"))) = 
{
    .USERID	= CONFIG_USERID,
    .reserved1	= 0xfff,
    .PMDL1WAY	= CONFIG_PMDL1WAY,
    .IOL1WAY	= CONFIG_IOL1WAY,
    .FUSBIDIO	= CONFIG_FUSBIDIO,
    .FVBUSONIO	= CONFIG_FVBUSONIO,
};
#elif defined(__32MX3XX__) || defined(__32MX4XX__)

struct devcfg3 _config3 __attribute__((section(".config_BFC02FF0"))) = 
{
    .USERID	= CONFIG_USERID,
    .reserved1	= 0xfff,
    .PMDL1WAY	= 1,
    .IOL1WAY	= 1,
    .FUSBIDIO	= 1,
    .FVBUSONIO	= 1,
};
#endif

#ifndef CONFIG_FPLLIDIV
    #define CONFIG_FPLLIDIV	IDIV_2 /* 8 MHz -> 4 MHz */
#endif
#ifndef CONFIG_FPLLMUL
    #define CONFIG_FPLLMUL	MUL_20 /* 4 MHz -> 80 MHz */
#endif
#ifndef CONFIG_FPLLODIV
    #define CONFIG_FPLLODIV	ODIV_2  /* 80 MHz -> 40 MHz */
#endif
#ifndef CONFIG_UPLLIDIV
    #define CONFIG_UPLLIDIV	IDIV_2 /* 8 MHz -> 4 MHz */
#endif

#ifndef CONFIG_UPLLEN
  #if defined(__32MX2XX__) || defined(__32MX4XX__)
    #define CONFIG_UPLLEN	0 /* Enable USB PLL */
  #else
    #define CONFIG_UPLLEN	1 /* Disable USB PLL */
  #endif
#endif

#if defined(__32MX1XX__) || defined(__32MX2XX__)
struct devcfg2 __attribute__((section(".config_BFC00BF4"))) _config2 = 
{
    .FPLLIDIV	= CONFIG_FPLLIDIV,
    .reserved1	= 0x1,
    .FPLLMUL	= CONFIG_FPLLMUL,
    .reserved2	= 0x1,
    .UPLLIDIV	= CONFIG_UPLLIDIV,
    .reserved3	= 0xf,
    .UPLLEN	= CONFIG_UPLLEN,
    .FPLLODIV	= CONFIG_FPLLODIV,
    .reserved4	= 0x1fff,
};
#elif defined(__32MX3XX__) || defined(__32MX4XX__)
struct devcfg2 __attribute__((section(".config_BFC02FF4"))) _config2 = 
{
    .FPLLIDIV	= CONFIG_FPLLIDIV,
    .reserved1	= 0x1,
    .FPLLMUL	= CONFIG_FPLLMUL,
    .reserved2	= 0x1,
    .UPLLIDIV	= CONFIG_UPLLIDIV,
    .reserved3	= 0xf,
    .UPLLEN	= CONFIG_UPLLEN,
    .FPLLODIV	= CONFIG_FPLLODIV,
    .reserved4	= 0x1fff,
};
#endif

#ifndef CONFIG_FWDTWINSZ
    #define CONFIG_FWDTWINSZ	WISZ_25  /* 25 % */
#endif
#ifndef CONFIG_FWDTEN
    #define CONFIG_FWDTEN	0  /* Watchdog Timer is disabled */
#endif
#ifndef CONFIG_WINDIS
    #define CONFIG_WINDIS	1 /* Watchdog Timer is  non-Window mode */
#endif
#ifndef CONFIG_WDTPS
    #define CONFIG_WDTPS	0  /* 1/1 */
#endif
#ifndef CONFIG_FCKSM
    #define CONFIG_FCKSM	CSDCMD  /* Clock Switching Disabled  */
#endif
#ifndef CONFIG_FPBDIV
    #define CONFIG_FPBDIV	0  /* PBCLK = SYSCLK */
    #define CONFIG_FPBDIV	0  /* PBCLK = SYSCLK */
#endif
#ifndef CONFIG_OSCIOFNC
  #if defined(__32MX1XX__) || defined(__32MX2XX__)
    #define CONFIG_OSCIOFNC	1  /* CLKO output Disabled  */
  #elif defined(__32MX3XX__) || defined(__32MX4XX__)
    #define CONFIG_OSCIOFNC	0  /* CLKO output Disabled : reversed */
  #endif
#endif
#ifndef CONFIG_POSCMOD
    #define CONFIG_POSCMOD	2  /* Primary Osillator is HS mode */
#endif
#ifndef CONFIG_IESO
    #define CONFIG_IESO		0  /* Switchover mode is Disbled */
#endif
#ifndef CONFIG_FSOSCEN
    #define CONFIG_FSOSCEN	0  /* Secondary Osillator is Disabled */
#endif
#ifndef CONFIG_FNOSC
    #define CONFIG_FNOSC	POSCPLL  /* Pri osc + PLL */
#endif

#if defined(__32MX1XX__) || defined(__32MX2XX__)
struct devcfg1 __attribute__((section(".config_BFC00BF8"))) _config1 = 
{
    .FNOSC	= CONFIG_FNOSC,
    .reserved1	= 0x3,
    .FSOSCEN	= CONFIG_FSOSCEN,
    .reserved2	= 0x1,
    .IESO	= CONFIG_IESO,
    .POSCMOD	= CONFIG_POSCMOD,
    .OSCIOFNC	= CONFIG_OSCIOFNC,
    .reserved3	= 0x1,
    .FPBDIV	= CONFIG_FPBDIV,
    .FCKSM	= CONFIG_FCKSM,
    .WDTPS	= CONFIG_WDTPS,
    .reserved4	= 0x1,
    .WINDIS	= CONFIG_WINDIS,
    .FWDTEN	= CONFIG_FWDTEN,
    .FWDTWINSZ	= CONFIG_FWDTWINSZ,
    .reserved5	= 0x3f,
};
#elif defined(__32MX3XX__) || defined(__32MX4XX__)
struct devcfg1 __attribute__((section(".config_BFC02FF8"))) _config1 = 
{
    .FNOSC	= CONFIG_FNOSC,
    .reserved1	= 0x3,
    .FSOSCEN	= CONFIG_FSOSCEN,
    .reserved2	= 0x1,
    .IESO	= CONFIG_IESO,
    .POSCMOD	= CONFIG_POSCMOD,
    .OSCIOFNC	= CONFIG_OSCIOFNC,
    .reserved3	= 0x1,
    .FPBDIV	= CONFIG_FPBDIV,
    .FCKSM	= CONFIG_FCKSM,
    .WDTPS	= CONFIG_WDTPS,
    .reserved4	= 0x1,
    .WINDIS	= 1,
    .FWDTEN	= CONFIG_FWDTEN,
    .FWDTWINSZ	= 3,
    .reserved5	= 0x3f,
};
#endif

#if defined(__32MX1XX__) || defined(__32MX2XX__)

#ifndef CONFIG_DEBUG
    #define CONFIG_DEBUG	2  /* DEBUG OFF */
#endif
#ifndef CONFIG_JTAGEN
    #define CONFIG_JTAGEN	1  /* Enabled */
#endif
#ifndef CONFIG_ICSEL
    #define CONFIG_ICSEL	ICS_PGx3  /* PGEx3 */
#endif
#ifndef CONFIG_PWP
    #define CONFIG_PWP		PWP_SIZE(0) /* All Writable */
#endif
#ifndef CONFIG_BWP
    #define CONFIG_BWP		1  /* Writable */
#endif
#ifndef CONFIG_CP
    #define CONFIG_CP		1  /* No protection */
#endif

struct devcfg0 __attribute__((section(".config_BFC00BFC"))) _config0 = 
{
    .DEBUG	= CONFIG_DEBUG,
    .JTAGEN	= CONFIG_JTAGEN,
    .ICESEL	= CONFIG_ICSEL,
    .reserved1	= 0x1f,
    .PWP	= CONFIG_PWP,
    .reserved2	= 0xff,
    .BWP      	= CONFIG_BWP,
    .reserved3	= 0x7,
    .CP		= CONFIG_CP,
    .reserved4	= 0x3,
    .reserved5	= 0,
};
#elif defined(__32MX3XX__) || defined(__32MX4XX__)

#ifndef CONFIG_DEBUG
    #define CONFIG_DEBUG	2  /* DEBUG OFF */
#endif
#ifndef CONFIG_ICSEL
    #define CONFIG_ICSEL	ICS_PGx2  /* PGEx2 */
#endif
#ifndef CONFIG_PWP
    #define CONFIG_PWP		PWP_SIZE(0) /* All Writable */
#endif
#ifndef CONFIG_BWP
    #define CONFIG_BWP		1  /* Writable */
#endif
#ifndef CONFIG_CP
    #define CONFIG_CP		1  /* No protection */
#endif

struct devcfg0 __attribute__((section(".config_BFC02FFC"))) _config0 = 
{
    .DEBUG	= CONFIG_DEBUG,
    .reserved0	= 0x1,
    .ICESEL	= CONFIG_ICSEL,
    .reserved1	= 0xff,
    .PWP	= CONFIG_PWP,
    .reserved2	= 0xf,
    .BWP      	= CONFIG_BWP,
    .reserved3	= 0x7,
    .CP		= CONFIG_CP,
    .reserved4	= 0x3,
    .reserved5	= 0,
};
#endif

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

#endif /* _BUILD_CONFIG */
#endif /* _JZ_P32MX_DEVCFG_BUILD_H_ */
