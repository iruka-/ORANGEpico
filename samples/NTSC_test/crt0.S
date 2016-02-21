##################################################################
#	MIPS32 (pic32mx220) 用 C Runtime Startup Library.
##################################################################
#	- pic32mx220に合わせて改造してあります。

#	- Pinguinoのmips-gccで使えます。
#	- 割り込み処理のjmpテーブルを含みます。
#	- 各関数名はこのソースの最後の章をお読みください。
#

#include "regdef.h"
#include "cp0defs.h"

#ifdef __LIBBUILD__
   .file 1 "crt0.S"
   .loc 1 0
#endif
        ##################################################################
        # Entry point of the entire application
        ##################################################################
        .section .reset,"ax",@progbits
        .set noreorder
        .ent _reset
_reset:
        la      k0, _startup
        jr      k0                      # Jump to startup code
        nop
  
        .end _reset
        .globl _reset

        ##################################################################
        # Startup code
        ##################################################################
        .section .startup,"ax",@progbits
        .set noreorder
        .ent _startup
_startup:
        ##################################################################
        # If entered because of an NMI, jump to the NMI handler.
        ##################################################################
        mfc0    k0,_CP0_STATUS
        ext     k0,k0,19,1              # Extract NMI bit
        beqz    k0,_no_nmi
        nop
        # AF la      k0,_nmi_handler
        # AF jr      k0
        nop
_no_nmi:

        ##################################################################
        # Initialize Stack Pointer
        #   _stack is initialized by the linker script to point to the
        #    starting location of the stack in DRM
        ##################################################################
        la      sp,_stack

        ##################################################################
        # Initialize Global Pointer
        #   _gp is initialized by the linker script to point to "middle"
        #   of the small variables region
        ##################################################################
        la      gp,_gp
        
        ##################################################################
        # Initialize Global Pointer in Shadow Set
        #   The SRSCtl's PSS field must be set to the shadow set in which
        #   to initialize the global pointer.  Since we only have a
        #   single shadow set (besides the normal), we will initialize
        #   SRSCtl<PSS> to SRSCtl<HSS>.  We then write the global pointer
        #   to the previous shadow set to ensure that on interrupt, the
        #   global pointer has been initialized.
        ##################################################################
        mfc0    t1,_CP0_SRSCTL          # Read SRSCtl register
        add     t3,t1,zero              # Save off current SRSCtl
        ext     t2,t1,26,4              # to obtain HSS field
        ins     t1,t2,6,4               # Put HSS field
        mtc0    t1,_CP0_SRSCTL          # into SRSCtl<PSS>
        wrpgpr  gp,gp                   # Set global pointer in PSS
        mtc0    t3,_CP0_SRSCTL          # Restore SRSCtl

        ##################################################################
        # Call the "on reset" procedure
        ##################################################################
        # AF la      t0,_on_reset
        # AF jalr    t0
        nop

        ##################################################################
        # Clear uninitialized data sections
        ##################################################################
        la      t0,_bss_begin
        la      t1,_bss_end
        b       _bss_check
        nop

_bss_init:
        sw      zero,0x0(t0)
        addu    t0,4
_bss_check:
        bltu    t0,t1,_bss_init
        nop
        
        ##################################################################
        # Copy initialized data from program flash to data memory
        #   src=_data_image_begin dst=_data_begin stop=_data_end
        ##################################################################
        la      t0,_data_image_begin
        la      t1,_data_begin
        la      t2,_data_end
        b       _init_check
        nop

_init_data:     
        lw      t3,(t0)
        sw      t3,(t1)
        addu    t0,4
        addu    t1,4
_init_check:
        bltu    t1,t2,_init_data
        nop

#ifdef	ENABLE_RAMFUNC_
        ##################################################################
        # If there are no RAM functions, skip the next two sections --
        # copying RAM functions from program flash to data memory and
        # initializing bus matrix registers.
        ##################################################################
        la      t1,_ramfunc_length
        beqz    t1,_ramfunc_done
        nop

        ##################################################################
        # Copy RAM functions from program flash to data memory
        #   src=_ramfunc_image_begin dst=_ramfunc_begin stop=_ramfunc_end
        ##################################################################
        la      t0,_ramfunc_image_begin
        la      t1,_ramfunc_begin
        la      t2,_ramfunc_end

_init_ramfunc:  
        lw      t3,(t0)
        sw      t3,(t1)
        addu    t0,4
        addu    t1,4
_ramfunc_check:
        bltu    t1,t2,_init_ramfunc
        nop

        ##################################################################
        # Initialize bus matrix registers if RAM functions exist in the
        # application
        ##################################################################
        la      t1,_bmxdkpba_address
        la      t2,BMXDKPBA
        sw      t1,0(t2)
        la      t1,_bmxdudba_address
        la      t2,BMXDUDBA
        sw      t1,0(t2)
        la      t1,_bmxdupba_address
        la      t2,BMXDUPBA
        sw      t1,0(t2)

_ramfunc_done:
#endif

        ##################################################################
        # Initialize CP0 registers
        ##################################################################
        # Initialize Count register
        ##################################################################
        mtc0    zero,_CP0_COUNT

        ##################################################################
        # Initialize Compare register
        ##################################################################
        li      t2,-1
        mtc0    t2,_CP0_COMPARE

        ##################################################################
        # Initialize EBase register
        ##################################################################
        la      t1,_ebase_address
        mtc0    t1,_CP0_EBASE

        ##################################################################
        # Initialize IntCtl register
        ##################################################################
        la      t1,_vector_spacing
        li      t2,0                    # Clear t2 and
        ins     t2,t1,5,5               # shift value to VS field
        mtc0    t2,_CP0_INTCTL

        ##################################################################
        # Initialize CAUSE registers
        # - Enable counting of Count register <DC = 0>
        # - Use special exception vector <IV = 1>
        # - Clear pending software interrupts <IP1:IP0 = 0>
        ##################################################################
        li      t1,0x00800000
        mtc0    t1,_CP0_CAUSE

        ##################################################################
        # Initialize STATUS register
        # - Access to Coprocessor 0 not allowed in user mode <CU0 = 0>
        # - User mode uses configured endianness <RE = 0>
        # - Preserve Bootstrap Exception vectors <BEV>
        # - Preserve soft reset <SR> and non-maskable interrupt <NMI>
        # - CorExtend enabled based on whether CorExtend User Defined
        #   Instructions have been implemented <CEE = Config<UDI>>
        # - Disable any pending interrups <IM7..IM2 = 0, IM1..IM0 = 0>
        # - Disable hardware interrupts <IPL7:IPL2 = 0>
        # - Base mode is Kernel mode <UM = 0>
        # - Error level is normal <ERL = 0>
        # - Exception level is normal <EXL = 0>
        # - Interrupts are disabled <IE = 0>
        ##################################################################
        mfc0    t0,_CP0_CONFIG
        ext     t1,t0,22,1              # Extract UDI from Config register
        sll     t1,t1,17                # Move UDI to Status.CEE location
        mfc0    t0,_CP0_STATUS
        and     t0,t0,0x00580000        # Preserve SR, NMI, and BEV
        or      t0,t1,t0                # Include Status.CEE (from UDI)
        mtc0    t0,_CP0_STATUS

        ##################################################################
        # Call the "on bootstrap" procedure
        ##################################################################
        # AF la      t0,_on_bootstrap
        # AF jalr    t0
        nop

        ##################################################################
        # Initialize Status<BEV> for normal exception vectors
        ##################################################################
        mfc0    t0,_CP0_STATUS
        and     t0,t0,0xffbfffff        # Clear BEV
        mtc0    t0,_CP0_STATUS

        ##################################################################
        # Call main. We do this via a thunk in the text section so that
        # a normal jump and link can be used, enabling the startup code
        # to work properly whether main is written in MIPS16 or MIPS32
        # code. I.e., the linker will correctly adjust the JAL to JALX if
        # necessary
        ##################################################################
        and     a0,a0,0
        and     a1,a1,0
        la      t0,_main_entry
        jr      t0
        nop

        
        .end _startup

        ##################################################################
        # Boot Exception Vector Handler
        # Jumps to _bootstrap_exception_handler
        ##################################################################
        .section .bev_handler,"ax",@progbits
        .set noreorder
        .ent _bev_exception
_bev_exception:
		j	_bootstrap_exception_handler
		nop
	.weak	_bootstrap_exception_handler
	.set	_bootstrap_exception_handler,Default_Handler
        # AF la      k0,_bootstrap_exception_handler
        # AF jr      k0
        #nop

        .end _bev_exception
                        
        ##################################################################
        # General Exception Vector Handler
        # Jumps to _general_exception_handler
        ##################################################################
        .section .gen_handler,"ax",@progbits
        .set noreorder
        .ent _gen_exception
_gen_exception:
		j	_general_exception_context
		nop
	.weak	_general_exception_context
	.set	_general_exception_context,Default_Handler
        # AF la      k0,_general_exception_context
        # AF jr      k0
        #nop

        .end _gen_exception


        .text
        .ent _main_entry
_main_entry:
        ##################################################################
        # Call main
        ##################################################################
        jal main
        nop

        ##################################################################
        # Call exit
        ##################################################################
        # jal    exit
        nop

        ##################################################################
        # Just in case, go into infinite loop
        ##################################################################
1:
        b       1b
        nop
        .end _main_entry


##################################################################
#	
#	.weak 擬似命令は、該当ラベルがＣ言語側で未定義だったら、
#		  このダミーハンドラーがリンクされる。弱いシンボル定義.
#
        .ent Default_Handler
Default_Handler:
		eret
        .end Default_Handler


##################################################################
# TMR1 ISR VECTOR 
#
	.section .vector_4,"ax",%progbits
		j	_Tmr1Interrupt
		nop
	.weak	_Tmr1Interrupt
	.set	_Tmr1Interrupt,Default_Handler

##################################################################
# TMR2 ISR VECTOR 
#
	.section .vector_8,"ax",%progbits
		j	_Tmr2Interrupt
		nop
	.weak	_Tmr2Interrupt
	.set	_Tmr2Interrupt,Default_Handler

##################################################################
# TMR3 ISR VECTOR 
#
#	.section .vector_12,"ax",%progbits
#		j	_Tmr3Interrupt
#		nop
#	.weak	_Tmr3Interrupt
#	.set	_Tmr3Interrupt,Default_Handler

##################################################################
# TMR4 ISR VECTOR 
#
	.section .vector_16,"ax",%progbits
		j	_Tmr4Interrupt
		nop
	.weak	_Tmr4Interrupt
	.set	_Tmr4Interrupt,Default_Handler

##################################################################
# ADC ISR VECTOR (IFS0<28>)
#
	.section .vector_23,"ax",%progbits
		j	_ADC1Interrupt
		nop
	.weak	_ADC1Interrupt
	.set	_ADC1Interrupt,Default_Handler

##################################################################
# USB ISR VECTOR 
#
	.section .vector_30,"ax",%progbits
		j	_USB1Interrupt
		nop
	.weak	_USB1Interrupt
	.set	_USB1Interrupt,Default_Handler

##################################################################
# SPI1 ISR VECTOR 
#
	.section .vector_31,"ax",%progbits
		j	_SPI1Interrupt
		nop
	.weak	_SPI1Interrupt
	.set	_SPI1Interrupt,Default_Handler

##################################################################
# UART1 ISR VECTOR 
#
	.section .vector_32,"ax",%progbits
		j	_UART1Interrupt
		nop
	.weak	_UART1Interrupt
	.set	_UART1Interrupt,Default_Handler

##################################################################
# SPI2 ISR VECTOR 
#
	.section .vector_36,"ax",%progbits
		j	_SPI2Interrupt
		nop
	.weak	_SPI2Interrupt
	.set	_SPI2Interrupt,Default_Handler

##################################################################
# UART2 ISR VECTOR 
#
	.section .vector_37,"ax",%progbits
		j	_UART2Interrupt
		nop
	.weak	_UART2Interrupt
	.set	_UART2Interrupt,Default_Handler

##################################################################
# DMA0 ISR VECTOR 
#
	.section .vector_40,"ax",%progbits
		j	_DMA0Interrupt
		nop
	.weak	_DMA0Interrupt
	.set	_DMA0Interrupt,Default_Handler

##################################################################
# DMA1 ISR VECTOR 
#
	.section .vector_41,"ax",%progbits
		j	_DMA1Interrupt
		nop
	.weak	_DMA1Interrupt
	.set	_DMA1Interrupt,Default_Handler

##################################################################
# DMA2 ISR VECTOR 
#
	.section .vector_42,"ax",%progbits
		j	_DMA2Interrupt
		nop
	.weak	_DMA2Interrupt
	.set	_DMA2Interrupt,Default_Handler

##################################################################
# DMA3 ISR VECTOR 
#
	.section .vector_43,"ax",%progbits
		j	_DMA3Interrupt
		nop
	.weak	_DMA3Interrupt
	.set	_DMA3Interrupt,Default_Handler

##################################################################
# SELFBOOT RESET VECTOR  (IGNORE THIS INSTRUCTION FOR BOOTLOADER MODE)
#
	.section .bootrom,"ax",%progbits
        .set noreorder
        .ent _bootrom_reset
_bootrom_reset:
        la      k0, _startup
        jr      k0                      # Jump to startup code
        nop
  
        .end _bootrom_reset
        .globl _bootrom_reset



##################################################################

#	現在、以下の関数名の割り込みハンドラーをＣ言語から定義できます。
#	.weak	_bootstrap_exception_handler
#	.weak	_general_exception_context
#	.weak	_Tmr1Interrupt
#	.weak	_Tmr2Interrupt
#	.weak	_Tmr3Interrupt
#	.weak	_Tmr4Interrupt
#	.weak	_ADC1Interrupt
#	.weak	_USB1Interrupt
#	.weak	_SPI1Interrupt
#	.weak	_SPI2Interrupt
#	.weak	_UART1Interrupt
#	.weak	_UART2Interrupt
#	.weak	_DMA0Interrupt
#	.weak	_DMA1Interrupt
#	.weak	_DMA2Interrupt
#	.weak	_DMA3Interrupt


#	さらに追加したい場合は、下記の割り込み番号を参考にweakエントリー
#	を増やしてください.

# /* Vector Numbers */
#define _CORE_TIMER_VECTOR                       0
#define _CORE_SOFTWARE_0_VECTOR                  1
#define _CORE_SOFTWARE_1_VECTOR                  2
#define _EXTERNAL_0_VECTOR                       3
#define _TIMER_1_VECTOR                          4
#define _INPUT_CAPTURE_1_VECTOR                  5
#define _OUTPUT_COMPARE_1_VECTOR                 6
#define _EXTERNAL_1_VECTOR                       7
#define _TIMER_2_VECTOR                          8	// IRQ=9
#define _INPUT_CAPTURE_2_VECTOR                  9
#define _OUTPUT_COMPARE_2_VECTOR                 10
#define _EXTERNAL_2_VECTOR                       11
#define _TIMER_3_VECTOR                          12	// IRQ=14
#define _INPUT_CAPTURE_3_VECTOR                  13
#define _OUTPUT_COMPARE_3_VECTOR                 14
#define _EXTERNAL_3_VECTOR                       15
#define _TIMER_4_VECTOR                          16	// IRQ=19
#define _INPUT_CAPTURE_4_VECTOR                  17
#define _OUTPUT_COMPARE_4_VECTOR                 18
#define _EXTERNAL_4_VECTOR                       19
#define _TIMER_5_VECTOR                          20
#define _INPUT_CAPTURE_5_VECTOR                  21
#define _OUTPUT_COMPARE_5_VECTOR                 22
#define _ADC_VECTOR                              23
#define _FAIL_SAFE_MONITOR_VECTOR                24
#define _RTCC_VECTOR                             25
#define _FCE_VECTOR                              26
#define _COMPARATOR_1_VECTOR                     27
#define _COMPARATOR_2_VECTOR                     28
#define _COMPARATOR_3_VECTOR                     29
#define _USB_1_VECTOR                            30
#define _SPI_1_VECTOR                            31
#define _UART_1_VECTOR                           32
#define _I2C_1_VECTOR                            33
#define _CHANGE_NOTICE_VECTOR                    34
#define _PMP_VECTOR                              35
#define _SPI_2_VECTOR                            36
#define _UART_2_VECTOR                           37
#define _I2C_2_VECTOR                            38
#define _CTMU_VECTOR                             39
#define _DMA_0_VECTOR                            40
#define _DMA_1_VECTOR                            41
#define _DMA_2_VECTOR                            42
#define _DMA_3_VECTOR                            43

