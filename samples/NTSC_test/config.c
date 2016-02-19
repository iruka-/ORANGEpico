//#ifdef	USE_SELFBOOT
#define	__32MX2XX__

#define	_BUILD_CONFIG

#include "config.h"

#include "p32mx_devcfg.h"


#define	CONFIG_USERID			0xffff	  
#define	CONFIG_PMDL1WAY			0		/* Multiple	Reconfiguration	is Allowed */	 
#define	CONFIG_IOL1WAY			0		/* Multiple	Reconfiguration	is Allowed */	 
#define	CONFIG_FUSBIDIO			1		/* USBID is	port */	   
#define	CONFIG_FVBUSONIO		1		/* Vbuson is port */	

#if CPU_CLOCK_48MHz
#define	CONFIG_FPLLMUL			MUL_24	/* 4 MHz ->	96 MHz */	 
#define	CONFIG_FPLLODIV			ODIV_2	/* 96 MHz -> 48	MHz	*/	  
#else // 40MHz
#define	CONFIG_FPLLMUL			MUL_20	/* 4 MHz ->	80 MHz */	 
#define	CONFIG_FPLLODIV			ODIV_2	/* 80 MHz -> 40	MHz	*/	  
#endif


#ifdef	USE_INTERNAL_FRC_OSC	// RC OSC
#define	CONFIG_FPLLIDIV			IDIV_1	/* 4 MHz ->	4 MHz */	
#define	CONFIG_UPLLIDIV			IDIV_1	/* 4 MHz ->	4 MHz */	
#define	CONFIG_FNOSC			FRC		/* RC  */	   
#define	CONFIG_POSCMOD			0		/* Primary Osillator is	EC (External)	*/	  
#define	CONFIG_FSOSCEN			0		/* Secondary Osillator is Disabled */	 
#define	CONFIG_OSCIOFNC			0		/* CLKO	output Enabled	 */	   
#define	CONFIG_FCKSM			CSECMD	/* Clock Switching Enabled	 */	   
#else


#ifdef	USE_12MHz_XTAL	// 12MHz
#define	CONFIG_FPLLIDIV			IDIV_3	/* 8 MHz ->	4 MHz */	
#define	CONFIG_UPLLIDIV			IDIV_3	/* 8 MHz ->	4 MHz */	
#else
#define	CONFIG_FPLLIDIV			IDIV_2	/* 8 MHz ->	4 MHz */	
#define	CONFIG_UPLLIDIV			IDIV_2	/* 8 MHz ->	4 MHz */	
#endif

#define	CONFIG_FNOSC			POSCPLL	/* Pri osc + PLL */	   
#define	CONFIG_POSCMOD			2		/* Primary Osillator is	HS mode	*/	  
#define	CONFIG_FSOSCEN			0		/* Secondary Osillator is Disabled */	 
#define	CONFIG_OSCIOFNC			1		/* CLKO	output Disabled	 */	   
#define	CONFIG_FCKSM			CSDCMD	/* Clock Switching Disabled	 */	   
#endif

#define	CONFIG_UPLLEN			0		/* Eable USB PLL */	   
#define	CONFIG_FWDTWINSZ		WISZ_25	/* 25 %	*/	  
#define	CONFIG_FWDTEN			0		/* Watchdog	Timer is disabled */	
#define	CONFIG_WINDIS			1		/* Watchdog	Timer is  non-Window mode */	
#define	CONFIG_WDTPS			0		/* 1/1 */	 
#define	CONFIG_FPBDIV			0		/* PBCLK = SYSCLK */	
#define	CONFIG_IESO				0		/* Switchover mode is Disbled */	
#define	CONFIG_DEBUG			3		/* DEBUG OFF */	   
#define	CONFIG_JTAGEN			1		/* Enabled */	 
#define	CONFIG_ICSEL			ICS_PGx3  /* PGEx3 */	 
#define	CONFIG_PWP				PWP_SIZE(0)	/* All Writable	*/	  
#define	CONFIG_BWP				1		/* Writable	*/	  
#define	CONFIG_CP				1		/* No protection */	   
#define	CONFIG_JTAGEN			1		/* Enabled */	 
#define	CONFIG_BWP				1		/* OFF */	 
#define	CONFIG_CP				1		/* OFF */

#include "p32mx_devcfg_build.h"

//#endif
