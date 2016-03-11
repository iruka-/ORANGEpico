#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

void	IntSetVectorPriority(u8 vector, u8 pri, u8 sub);
unsigned	int IntGetVectorPriority(u8 vector);
unsigned	int IntGetVectorSubPriority(u8 vector);
void	IntClearFlag(u8 numinter);
unsigned	int IntGetFlag(u8 numinter);
void	IntEnable(u8 numinter);
void	IntDisable(u8 numinter);
unsigned	int IntGetInterruptVectorNumber(void);
unsigned	int IntGetInterruptVectorPriority(void);
unsigned	int MIPS32 IntDisableInterrupts();
unsigned	int MIPS32 IntEnableInterrupts();
void	MIPS32 IntRestoreInterrupts(unsigned int intStatus);
void	MIPS32 IntSetEBASE(unsigned int ebase_address);
void	MIPS32 IntConfigureSystem(u8 mode);
int		TMR_X_calc_preset_divider(int timediv,u32 delay,int *tx_con,int *limit);
/*
void	Tmr1Interrupt(void);
u8		OnTimer1(callback func, u8 timediv, u32 delay);
void	Tmr2Interrupt(void);
u8		OnTimer2(callback func, u8 timediv, u32 delay);
void	Tmr3Interrupt(void);
u8		OnTimer3(callback func, u8 timediv, u32 delay);
void	Tmr4Interrupt(void);
u8		OnTimer4(callback func, u8 timediv, u32 delay);
void	Tmr5Interrupt(void);
u8		OnTimer5(callback func, u8 timediv, u32 delay);
*/
#endif
