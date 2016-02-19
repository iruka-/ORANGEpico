/*
 *  ボード設定:
 */

#ifndef _CONFIG_H_
#define	_CONFIG_H_

// この #define は、config.cでFuse設定の切り替えに使用されているので注意.
#define	CPU_CLOCK_48MHz 1 // 48MHz動作.


#ifdef	CPU_CLOCK_48MHz
// CPUとペリフェラルの動作周波数
#define SYS_FREQ (48000000)
#else
#define SYS_FREQ (40000000)
#endif


#endif
