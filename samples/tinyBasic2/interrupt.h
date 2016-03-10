#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include "utype.h"
#include "util.h"

void IntSetVectorPriority(u8 vector, u8 pri, u8 sub);
uint IntGetVectorPriority(u8 vector);
uint IntGetVectorSubPriority(u8 vector);
void IntClearFlag(u8 numinter);
uint IntGetFlag(u8 numinter);
void IntEnable(u8 numinter);
void IntDisable(u8 numinter);
uint IntGetInterruptVectorNumber(void);
uint IntGetInterruptVectorPriority(void);
uint _MIPS32 IntDisableInterrupts();
uint _MIPS32 IntEnableInterrupts();
void _MIPS32 IntRestoreInterrupts(unsigned int intStatus);
void _MIPS32 IntSetEBASE(unsigned int ebase_address);
void _MIPS32 IntConfigureSystem(u8 mode);
int	 TMR_X_calc_preset_divider(int timediv,u32 delay,int *tx_con,int *limit);

#endif
