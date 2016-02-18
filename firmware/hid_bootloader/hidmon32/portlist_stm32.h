//
PortList portlist_stm32[]={

// -----------------------
// port=InterruptType , type=InterruptType_Type
 {"InterruptType.ICTR"    ,0xe000e004, A__I|A_uint32_t},
 {"InterruptType.ACTLR"   ,0xe000e008,A__IO|A_uint32_t},

// -----------------------
// port=SCB , type=SCB_Type

// -----------------------
// port=SysTick , type=SysTick_Type
 {"SysTick.CTRL"          ,0xe000e010,A__IO|A_uint32_t},
 {"SysTick.LOAD"          ,0xe000e014,A__IO|A_uint32_t},
 {"SysTick.VAL"           ,0xe000e018,A__IO|A_uint32_t},
 {"SysTick.CALIB"         ,0xe000e01c, A__I|A_uint32_t},

// -----------------------
// port=NVIC , type=NVIC_Type

// -----------------------
// port=ITM , type=ITM_Type

// -----------------------
// port=CoreDebug , type=CoreDebug_Type
 {"CoreDebug.DHCSR"       ,0xe000edf0,A__IO|A_uint32_t},
 {"CoreDebug.DCRSR"       ,0xe000edf4, A__O|A_uint32_t},
 {"CoreDebug.DCRDR"       ,0xe000edf8,A__IO|A_uint32_t},
 {"CoreDebug.DEMCR"       ,0xe000edfc,A__IO|A_uint32_t},

// -----------------------
// port=TIM2 , type=TIM_TypeDef
 {"TIM2.CR1"              ,0x40000000,A__IO|A_uint16_t},
 {"TIM2.CR2"              ,0x40000004,A__IO|A_uint16_t},
 {"TIM2.SMCR"             ,0x40000008,A__IO|A_uint16_t},
 {"TIM2.DIER"             ,0x4000000c,A__IO|A_uint16_t},
 {"TIM2.SR"               ,0x40000010,A__IO|A_uint16_t},
 {"TIM2.EGR"              ,0x40000014,A__IO|A_uint16_t},
 {"TIM2.CCMR1"            ,0x40000018,A__IO|A_uint16_t},
 {"TIM2.CCMR2"            ,0x4000001c,A__IO|A_uint16_t},
 {"TIM2.CCER"             ,0x40000020,A__IO|A_uint16_t},
 {"TIM2.CNT"              ,0x40000024,A__IO|A_uint16_t},
 {"TIM2.PSC"              ,0x40000028,A__IO|A_uint16_t},
 {"TIM2.ARR"              ,0x4000002c,A__IO|A_uint16_t},
 {"TIM2.RCR"              ,0x40000030,A__IO|A_uint16_t},
 {"TIM2.CCR1"             ,0x40000034,A__IO|A_uint16_t},
 {"TIM2.CCR2"             ,0x40000038,A__IO|A_uint16_t},
 {"TIM2.CCR3"             ,0x4000003c,A__IO|A_uint16_t},
 {"TIM2.CCR4"             ,0x40000040,A__IO|A_uint16_t},
 {"TIM2.BDTR"             ,0x40000044,A__IO|A_uint16_t},
 {"TIM2.DCR"              ,0x40000048,A__IO|A_uint16_t},
 {"TIM2.DMAR"             ,0x4000004c,A__IO|A_uint16_t},

// -----------------------
// port=TIM3 , type=TIM_TypeDef
 {"TIM3.CR1"              ,0x40000400,A__IO|A_uint16_t},
 {"TIM3.CR2"              ,0x40000404,A__IO|A_uint16_t},
 {"TIM3.SMCR"             ,0x40000408,A__IO|A_uint16_t},
 {"TIM3.DIER"             ,0x4000040c,A__IO|A_uint16_t},
 {"TIM3.SR"               ,0x40000410,A__IO|A_uint16_t},
 {"TIM3.EGR"              ,0x40000414,A__IO|A_uint16_t},
 {"TIM3.CCMR1"            ,0x40000418,A__IO|A_uint16_t},
 {"TIM3.CCMR2"            ,0x4000041c,A__IO|A_uint16_t},
 {"TIM3.CCER"             ,0x40000420,A__IO|A_uint16_t},
 {"TIM3.CNT"              ,0x40000424,A__IO|A_uint16_t},
 {"TIM3.PSC"              ,0x40000428,A__IO|A_uint16_t},
 {"TIM3.ARR"              ,0x4000042c,A__IO|A_uint16_t},
 {"TIM3.RCR"              ,0x40000430,A__IO|A_uint16_t},
 {"TIM3.CCR1"             ,0x40000434,A__IO|A_uint16_t},
 {"TIM3.CCR2"             ,0x40000438,A__IO|A_uint16_t},
 {"TIM3.CCR3"             ,0x4000043c,A__IO|A_uint16_t},
 {"TIM3.CCR4"             ,0x40000440,A__IO|A_uint16_t},
 {"TIM3.BDTR"             ,0x40000444,A__IO|A_uint16_t},
 {"TIM3.DCR"              ,0x40000448,A__IO|A_uint16_t},
 {"TIM3.DMAR"             ,0x4000044c,A__IO|A_uint16_t},

// -----------------------
// port=TIM4 , type=TIM_TypeDef
 {"TIM4.CR1"              ,0x40000800,A__IO|A_uint16_t},
 {"TIM4.CR2"              ,0x40000804,A__IO|A_uint16_t},
 {"TIM4.SMCR"             ,0x40000808,A__IO|A_uint16_t},
 {"TIM4.DIER"             ,0x4000080c,A__IO|A_uint16_t},
 {"TIM4.SR"               ,0x40000810,A__IO|A_uint16_t},
 {"TIM4.EGR"              ,0x40000814,A__IO|A_uint16_t},
 {"TIM4.CCMR1"            ,0x40000818,A__IO|A_uint16_t},
 {"TIM4.CCMR2"            ,0x4000081c,A__IO|A_uint16_t},
 {"TIM4.CCER"             ,0x40000820,A__IO|A_uint16_t},
 {"TIM4.CNT"              ,0x40000824,A__IO|A_uint16_t},
 {"TIM4.PSC"              ,0x40000828,A__IO|A_uint16_t},
 {"TIM4.ARR"              ,0x4000082c,A__IO|A_uint16_t},
 {"TIM4.RCR"              ,0x40000830,A__IO|A_uint16_t},
 {"TIM4.CCR1"             ,0x40000834,A__IO|A_uint16_t},
 {"TIM4.CCR2"             ,0x40000838,A__IO|A_uint16_t},
 {"TIM4.CCR3"             ,0x4000083c,A__IO|A_uint16_t},
 {"TIM4.CCR4"             ,0x40000840,A__IO|A_uint16_t},
 {"TIM4.BDTR"             ,0x40000844,A__IO|A_uint16_t},
 {"TIM4.DCR"              ,0x40000848,A__IO|A_uint16_t},
 {"TIM4.DMAR"             ,0x4000084c,A__IO|A_uint16_t},

// -----------------------
// port=TIM5 , type=TIM_TypeDef
 {"TIM5.CR1"              ,0x40000c00,A__IO|A_uint16_t},
 {"TIM5.CR2"              ,0x40000c04,A__IO|A_uint16_t},
 {"TIM5.SMCR"             ,0x40000c08,A__IO|A_uint16_t},
 {"TIM5.DIER"             ,0x40000c0c,A__IO|A_uint16_t},
 {"TIM5.SR"               ,0x40000c10,A__IO|A_uint16_t},
 {"TIM5.EGR"              ,0x40000c14,A__IO|A_uint16_t},
 {"TIM5.CCMR1"            ,0x40000c18,A__IO|A_uint16_t},
 {"TIM5.CCMR2"            ,0x40000c1c,A__IO|A_uint16_t},
 {"TIM5.CCER"             ,0x40000c20,A__IO|A_uint16_t},
 {"TIM5.CNT"              ,0x40000c24,A__IO|A_uint16_t},
 {"TIM5.PSC"              ,0x40000c28,A__IO|A_uint16_t},
 {"TIM5.ARR"              ,0x40000c2c,A__IO|A_uint16_t},
 {"TIM5.RCR"              ,0x40000c30,A__IO|A_uint16_t},
 {"TIM5.CCR1"             ,0x40000c34,A__IO|A_uint16_t},
 {"TIM5.CCR2"             ,0x40000c38,A__IO|A_uint16_t},
 {"TIM5.CCR3"             ,0x40000c3c,A__IO|A_uint16_t},
 {"TIM5.CCR4"             ,0x40000c40,A__IO|A_uint16_t},
 {"TIM5.BDTR"             ,0x40000c44,A__IO|A_uint16_t},
 {"TIM5.DCR"              ,0x40000c48,A__IO|A_uint16_t},
 {"TIM5.DMAR"             ,0x40000c4c,A__IO|A_uint16_t},

// -----------------------
// port=TIM6 , type=TIM_TypeDef
 {"TIM6.CR1"              ,0x40001000,A__IO|A_uint16_t},
 {"TIM6.CR2"              ,0x40001004,A__IO|A_uint16_t},
 {"TIM6.SMCR"             ,0x40001008,A__IO|A_uint16_t},
 {"TIM6.DIER"             ,0x4000100c,A__IO|A_uint16_t},
 {"TIM6.SR"               ,0x40001010,A__IO|A_uint16_t},
 {"TIM6.EGR"              ,0x40001014,A__IO|A_uint16_t},
 {"TIM6.CCMR1"            ,0x40001018,A__IO|A_uint16_t},
 {"TIM6.CCMR2"            ,0x4000101c,A__IO|A_uint16_t},
 {"TIM6.CCER"             ,0x40001020,A__IO|A_uint16_t},
 {"TIM6.CNT"              ,0x40001024,A__IO|A_uint16_t},
 {"TIM6.PSC"              ,0x40001028,A__IO|A_uint16_t},
 {"TIM6.ARR"              ,0x4000102c,A__IO|A_uint16_t},
 {"TIM6.RCR"              ,0x40001030,A__IO|A_uint16_t},
 {"TIM6.CCR1"             ,0x40001034,A__IO|A_uint16_t},
 {"TIM6.CCR2"             ,0x40001038,A__IO|A_uint16_t},
 {"TIM6.CCR3"             ,0x4000103c,A__IO|A_uint16_t},
 {"TIM6.CCR4"             ,0x40001040,A__IO|A_uint16_t},
 {"TIM6.BDTR"             ,0x40001044,A__IO|A_uint16_t},
 {"TIM6.DCR"              ,0x40001048,A__IO|A_uint16_t},
 {"TIM6.DMAR"             ,0x4000104c,A__IO|A_uint16_t},

// -----------------------
// port=TIM7 , type=TIM_TypeDef
 {"TIM7.CR1"              ,0x40001400,A__IO|A_uint16_t},
 {"TIM7.CR2"              ,0x40001404,A__IO|A_uint16_t},
 {"TIM7.SMCR"             ,0x40001408,A__IO|A_uint16_t},
 {"TIM7.DIER"             ,0x4000140c,A__IO|A_uint16_t},
 {"TIM7.SR"               ,0x40001410,A__IO|A_uint16_t},
 {"TIM7.EGR"              ,0x40001414,A__IO|A_uint16_t},
 {"TIM7.CCMR1"            ,0x40001418,A__IO|A_uint16_t},
 {"TIM7.CCMR2"            ,0x4000141c,A__IO|A_uint16_t},
 {"TIM7.CCER"             ,0x40001420,A__IO|A_uint16_t},
 {"TIM7.CNT"              ,0x40001424,A__IO|A_uint16_t},
 {"TIM7.PSC"              ,0x40001428,A__IO|A_uint16_t},
 {"TIM7.ARR"              ,0x4000142c,A__IO|A_uint16_t},
 {"TIM7.RCR"              ,0x40001430,A__IO|A_uint16_t},
 {"TIM7.CCR1"             ,0x40001434,A__IO|A_uint16_t},
 {"TIM7.CCR2"             ,0x40001438,A__IO|A_uint16_t},
 {"TIM7.CCR3"             ,0x4000143c,A__IO|A_uint16_t},
 {"TIM7.CCR4"             ,0x40001440,A__IO|A_uint16_t},
 {"TIM7.BDTR"             ,0x40001444,A__IO|A_uint16_t},
 {"TIM7.DCR"              ,0x40001448,A__IO|A_uint16_t},
 {"TIM7.DMAR"             ,0x4000144c,A__IO|A_uint16_t},

// -----------------------
// port=TIM12 , type=TIM_TypeDef
 {"TIM12.CR1"             ,0x40001800,A__IO|A_uint16_t},
 {"TIM12.CR2"             ,0x40001804,A__IO|A_uint16_t},
 {"TIM12.SMCR"            ,0x40001808,A__IO|A_uint16_t},
 {"TIM12.DIER"            ,0x4000180c,A__IO|A_uint16_t},
 {"TIM12.SR"              ,0x40001810,A__IO|A_uint16_t},
 {"TIM12.EGR"             ,0x40001814,A__IO|A_uint16_t},
 {"TIM12.CCMR1"           ,0x40001818,A__IO|A_uint16_t},
 {"TIM12.CCMR2"           ,0x4000181c,A__IO|A_uint16_t},
 {"TIM12.CCER"            ,0x40001820,A__IO|A_uint16_t},
 {"TIM12.CNT"             ,0x40001824,A__IO|A_uint16_t},
 {"TIM12.PSC"             ,0x40001828,A__IO|A_uint16_t},
 {"TIM12.ARR"             ,0x4000182c,A__IO|A_uint16_t},
 {"TIM12.RCR"             ,0x40001830,A__IO|A_uint16_t},
 {"TIM12.CCR1"            ,0x40001834,A__IO|A_uint16_t},
 {"TIM12.CCR2"            ,0x40001838,A__IO|A_uint16_t},
 {"TIM12.CCR3"            ,0x4000183c,A__IO|A_uint16_t},
 {"TIM12.CCR4"            ,0x40001840,A__IO|A_uint16_t},
 {"TIM12.BDTR"            ,0x40001844,A__IO|A_uint16_t},
 {"TIM12.DCR"             ,0x40001848,A__IO|A_uint16_t},
 {"TIM12.DMAR"            ,0x4000184c,A__IO|A_uint16_t},

// -----------------------
// port=TIM13 , type=TIM_TypeDef
 {"TIM13.CR1"             ,0x40001c00,A__IO|A_uint16_t},
 {"TIM13.CR2"             ,0x40001c04,A__IO|A_uint16_t},
 {"TIM13.SMCR"            ,0x40001c08,A__IO|A_uint16_t},
 {"TIM13.DIER"            ,0x40001c0c,A__IO|A_uint16_t},
 {"TIM13.SR"              ,0x40001c10,A__IO|A_uint16_t},
 {"TIM13.EGR"             ,0x40001c14,A__IO|A_uint16_t},
 {"TIM13.CCMR1"           ,0x40001c18,A__IO|A_uint16_t},
 {"TIM13.CCMR2"           ,0x40001c1c,A__IO|A_uint16_t},
 {"TIM13.CCER"            ,0x40001c20,A__IO|A_uint16_t},
 {"TIM13.CNT"             ,0x40001c24,A__IO|A_uint16_t},
 {"TIM13.PSC"             ,0x40001c28,A__IO|A_uint16_t},
 {"TIM13.ARR"             ,0x40001c2c,A__IO|A_uint16_t},
 {"TIM13.RCR"             ,0x40001c30,A__IO|A_uint16_t},
 {"TIM13.CCR1"            ,0x40001c34,A__IO|A_uint16_t},
 {"TIM13.CCR2"            ,0x40001c38,A__IO|A_uint16_t},
 {"TIM13.CCR3"            ,0x40001c3c,A__IO|A_uint16_t},
 {"TIM13.CCR4"            ,0x40001c40,A__IO|A_uint16_t},
 {"TIM13.BDTR"            ,0x40001c44,A__IO|A_uint16_t},
 {"TIM13.DCR"             ,0x40001c48,A__IO|A_uint16_t},
 {"TIM13.DMAR"            ,0x40001c4c,A__IO|A_uint16_t},

// -----------------------
// port=TIM14 , type=TIM_TypeDef
 {"TIM14.CR1"             ,0x40002000,A__IO|A_uint16_t},
 {"TIM14.CR2"             ,0x40002004,A__IO|A_uint16_t},
 {"TIM14.SMCR"            ,0x40002008,A__IO|A_uint16_t},
 {"TIM14.DIER"            ,0x4000200c,A__IO|A_uint16_t},
 {"TIM14.SR"              ,0x40002010,A__IO|A_uint16_t},
 {"TIM14.EGR"             ,0x40002014,A__IO|A_uint16_t},
 {"TIM14.CCMR1"           ,0x40002018,A__IO|A_uint16_t},
 {"TIM14.CCMR2"           ,0x4000201c,A__IO|A_uint16_t},
 {"TIM14.CCER"            ,0x40002020,A__IO|A_uint16_t},
 {"TIM14.CNT"             ,0x40002024,A__IO|A_uint16_t},
 {"TIM14.PSC"             ,0x40002028,A__IO|A_uint16_t},
 {"TIM14.ARR"             ,0x4000202c,A__IO|A_uint16_t},
 {"TIM14.RCR"             ,0x40002030,A__IO|A_uint16_t},
 {"TIM14.CCR1"            ,0x40002034,A__IO|A_uint16_t},
 {"TIM14.CCR2"            ,0x40002038,A__IO|A_uint16_t},
 {"TIM14.CCR3"            ,0x4000203c,A__IO|A_uint16_t},
 {"TIM14.CCR4"            ,0x40002040,A__IO|A_uint16_t},
 {"TIM14.BDTR"            ,0x40002044,A__IO|A_uint16_t},
 {"TIM14.DCR"             ,0x40002048,A__IO|A_uint16_t},
 {"TIM14.DMAR"            ,0x4000204c,A__IO|A_uint16_t},

// -----------------------
// port=RTC , type=RTC_TypeDef
 {"RTC.CRH"               ,0x40002800,A__IO|A_uint16_t},
 {"RTC.CRL"               ,0x40002804,A__IO|A_uint16_t},
 {"RTC.PRLH"              ,0x40002808,A__IO|A_uint16_t},
 {"RTC.PRLL"              ,0x4000280c,A__IO|A_uint16_t},
 {"RTC.DIVH"              ,0x40002810,A__IO|A_uint16_t},
 {"RTC.DIVL"              ,0x40002814,A__IO|A_uint16_t},
 {"RTC.CNTH"              ,0x40002818,A__IO|A_uint16_t},
 {"RTC.CNTL"              ,0x4000281c,A__IO|A_uint16_t},
 {"RTC.ALRH"              ,0x40002820,A__IO|A_uint16_t},
 {"RTC.ALRL"              ,0x40002824,A__IO|A_uint16_t},

// -----------------------
// port=WWDG , type=WWDG_TypeDef
 {"WWDG.CR"               ,0x40002c00,A__IO|A_uint32_t},
 {"WWDG.CFR"              ,0x40002c04,A__IO|A_uint32_t},
 {"WWDG.SR"               ,0x40002c08,A__IO|A_uint32_t},

// -----------------------
// port=IWDG , type=IWDG_TypeDef
 {"IWDG.KR"               ,0x40003000,A__IO|A_uint32_t},
 {"IWDG.PR"               ,0x40003004,A__IO|A_uint32_t},
 {"IWDG.RLR"              ,0x40003008,A__IO|A_uint32_t},
 {"IWDG.SR"               ,0x4000300c,A__IO|A_uint32_t},

// -----------------------
// port=SPI2 , type=SPI_TypeDef
 {"SPI2.CR1"              ,0x40003800,A__IO|A_uint16_t},
 {"SPI2.CR2"              ,0x40003804,A__IO|A_uint16_t},
 {"SPI2.SR"               ,0x40003808,A__IO|A_uint16_t},
 {"SPI2.DR"               ,0x4000380c,A__IO|A_uint16_t},
 {"SPI2.CRCPR"            ,0x40003810,A__IO|A_uint16_t},
 {"SPI2.RXCRCR"           ,0x40003814,A__IO|A_uint16_t},
 {"SPI2.TXCRCR"           ,0x40003818,A__IO|A_uint16_t},
 {"SPI2.I2SCFGR"          ,0x4000381c,A__IO|A_uint16_t},
 {"SPI2.I2SPR"            ,0x40003820,A__IO|A_uint16_t},

// -----------------------
// port=SPI3 , type=SPI_TypeDef
 {"SPI3.CR1"              ,0x40003c00,A__IO|A_uint16_t},
 {"SPI3.CR2"              ,0x40003c04,A__IO|A_uint16_t},
 {"SPI3.SR"               ,0x40003c08,A__IO|A_uint16_t},
 {"SPI3.DR"               ,0x40003c0c,A__IO|A_uint16_t},
 {"SPI3.CRCPR"            ,0x40003c10,A__IO|A_uint16_t},
 {"SPI3.RXCRCR"           ,0x40003c14,A__IO|A_uint16_t},
 {"SPI3.TXCRCR"           ,0x40003c18,A__IO|A_uint16_t},
 {"SPI3.I2SCFGR"          ,0x40003c1c,A__IO|A_uint16_t},
 {"SPI3.I2SPR"            ,0x40003c20,A__IO|A_uint16_t},

// -----------------------
// port=USART2 , type=USART_TypeDef
 {"USART2.SR"             ,0x40004400,A__IO|A_uint16_t},
 {"USART2.DR"             ,0x40004404,A__IO|A_uint16_t},
 {"USART2.BRR"            ,0x40004408,A__IO|A_uint16_t},
 {"USART2.CR1"            ,0x4000440c,A__IO|A_uint16_t},
 {"USART2.CR2"            ,0x40004410,A__IO|A_uint16_t},
 {"USART2.CR3"            ,0x40004414,A__IO|A_uint16_t},
 {"USART2.GTPR"           ,0x40004418,A__IO|A_uint16_t},

// -----------------------
// port=USART3 , type=USART_TypeDef
 {"USART3.SR"             ,0x40004800,A__IO|A_uint16_t},
 {"USART3.DR"             ,0x40004804,A__IO|A_uint16_t},
 {"USART3.BRR"            ,0x40004808,A__IO|A_uint16_t},
 {"USART3.CR1"            ,0x4000480c,A__IO|A_uint16_t},
 {"USART3.CR2"            ,0x40004810,A__IO|A_uint16_t},
 {"USART3.CR3"            ,0x40004814,A__IO|A_uint16_t},
 {"USART3.GTPR"           ,0x40004818,A__IO|A_uint16_t},

// -----------------------
// port=UART4 , type=USART_TypeDef
 {"UART4.SR"              ,0x40004c00,A__IO|A_uint16_t},
 {"UART4.DR"              ,0x40004c04,A__IO|A_uint16_t},
 {"UART4.BRR"             ,0x40004c08,A__IO|A_uint16_t},
 {"UART4.CR1"             ,0x40004c0c,A__IO|A_uint16_t},
 {"UART4.CR2"             ,0x40004c10,A__IO|A_uint16_t},
 {"UART4.CR3"             ,0x40004c14,A__IO|A_uint16_t},
 {"UART4.GTPR"            ,0x40004c18,A__IO|A_uint16_t},

// -----------------------
// port=UART5 , type=USART_TypeDef
 {"UART5.SR"              ,0x40005000,A__IO|A_uint16_t},
 {"UART5.DR"              ,0x40005004,A__IO|A_uint16_t},
 {"UART5.BRR"             ,0x40005008,A__IO|A_uint16_t},
 {"UART5.CR1"             ,0x4000500c,A__IO|A_uint16_t},
 {"UART5.CR2"             ,0x40005010,A__IO|A_uint16_t},
 {"UART5.CR3"             ,0x40005014,A__IO|A_uint16_t},
 {"UART5.GTPR"            ,0x40005018,A__IO|A_uint16_t},

// -----------------------
// port=I2C1 , type=I2C_TypeDef
 {"I2C1.CR1"              ,0x40005400,A__IO|A_uint16_t},
 {"I2C1.CR2"              ,0x40005404,A__IO|A_uint16_t},
 {"I2C1.OAR1"             ,0x40005408,A__IO|A_uint16_t},
 {"I2C1.OAR2"             ,0x4000540c,A__IO|A_uint16_t},
 {"I2C1.DR"               ,0x40005410,A__IO|A_uint16_t},
 {"I2C1.SR1"              ,0x40005414,A__IO|A_uint16_t},
 {"I2C1.SR2"              ,0x40005418,A__IO|A_uint16_t},
 {"I2C1.CCR"              ,0x4000541c,A__IO|A_uint16_t},
 {"I2C1.TRISE"            ,0x40005420,A__IO|A_uint16_t},

// -----------------------
// port=I2C2 , type=I2C_TypeDef
 {"I2C2.CR1"              ,0x40005800,A__IO|A_uint16_t},
 {"I2C2.CR2"              ,0x40005804,A__IO|A_uint16_t},
 {"I2C2.OAR1"             ,0x40005808,A__IO|A_uint16_t},
 {"I2C2.OAR2"             ,0x4000580c,A__IO|A_uint16_t},
 {"I2C2.DR"               ,0x40005810,A__IO|A_uint16_t},
 {"I2C2.SR1"              ,0x40005814,A__IO|A_uint16_t},
 {"I2C2.SR2"              ,0x40005818,A__IO|A_uint16_t},
 {"I2C2.CCR"              ,0x4000581c,A__IO|A_uint16_t},
 {"I2C2.TRISE"            ,0x40005820,A__IO|A_uint16_t},

// -----------------------
// port=CAN1 , type=CAN_TypeDef
 {"CAN1.MCR"              ,0x40006400,A__IO|A_uint32_t},
 {"CAN1.MSR"              ,0x40006404,A__IO|A_uint32_t},
 {"CAN1.TSR"              ,0x40006408,A__IO|A_uint32_t},
 {"CAN1.RF0R"             ,0x4000640c,A__IO|A_uint32_t},
 {"CAN1.RF1R"             ,0x40006410,A__IO|A_uint32_t},
 {"CAN1.IER"              ,0x40006414,A__IO|A_uint32_t},
 {"CAN1.ESR"              ,0x40006418,A__IO|A_uint32_t},
 {"CAN1.BTR"              ,0x4000641c,A__IO|A_uint32_t},
 {"CAN1.sTxMailBox[3]"    ,0x40006580,A_CAN_TxMailBox_TypeDef},
 {"CAN1.sFIFOMailBox[2]"  ,0x400065b0,A_CAN_FIFOMailBox_TypeDef},
 {"CAN1.FMR"              ,0x40006600,A__IO|A_uint32_t},
 {"CAN1.FM1R"             ,0x40006604,A__IO|A_uint32_t},
 {"CAN1.FS1R"             ,0x4000660c,A__IO|A_uint32_t},
 {"CAN1.FFA1R"            ,0x40006614,A__IO|A_uint32_t},
 {"CAN1.FA1R"             ,0x4000661c,A__IO|A_uint32_t},
 {"CAN1.sFilterRegister[14]",0x40006640,A_CAN_FilterRegister_TypeDef},
 {"CAN1.sFilterRegister[28]",0x40006640,A_CAN_FilterRegister_TypeDef},

// -----------------------
// port=CAN2 , type=CAN_TypeDef
 {"CAN2.MCR"              ,0x40006800,A__IO|A_uint32_t},
 {"CAN2.MSR"              ,0x40006804,A__IO|A_uint32_t},
 {"CAN2.TSR"              ,0x40006808,A__IO|A_uint32_t},
 {"CAN2.RF0R"             ,0x4000680c,A__IO|A_uint32_t},
 {"CAN2.RF1R"             ,0x40006810,A__IO|A_uint32_t},
 {"CAN2.IER"              ,0x40006814,A__IO|A_uint32_t},
 {"CAN2.ESR"              ,0x40006818,A__IO|A_uint32_t},
 {"CAN2.BTR"              ,0x4000681c,A__IO|A_uint32_t},
 {"CAN2.sTxMailBox[3]"    ,0x40006980,A_CAN_TxMailBox_TypeDef},
 {"CAN2.sFIFOMailBox[2]"  ,0x400069b0,A_CAN_FIFOMailBox_TypeDef},
 {"CAN2.FMR"              ,0x40006a00,A__IO|A_uint32_t},
 {"CAN2.FM1R"             ,0x40006a04,A__IO|A_uint32_t},
 {"CAN2.FS1R"             ,0x40006a0c,A__IO|A_uint32_t},
 {"CAN2.FFA1R"            ,0x40006a14,A__IO|A_uint32_t},
 {"CAN2.FA1R"             ,0x40006a1c,A__IO|A_uint32_t},
 {"CAN2.sFilterRegister[14]",0x40006a40,A_CAN_FilterRegister_TypeDef},
 {"CAN2.sFilterRegister[28]",0x40006a40,A_CAN_FilterRegister_TypeDef},

// -----------------------
// port=BKP , type=BKP_TypeDef
 {"BKP.DR1"               ,0x40006c04,A__IO|A_uint16_t},
 {"BKP.DR2"               ,0x40006c08,A__IO|A_uint16_t},
 {"BKP.DR3"               ,0x40006c0c,A__IO|A_uint16_t},
 {"BKP.DR4"               ,0x40006c10,A__IO|A_uint16_t},
 {"BKP.DR5"               ,0x40006c14,A__IO|A_uint16_t},
 {"BKP.DR6"               ,0x40006c18,A__IO|A_uint16_t},
 {"BKP.DR7"               ,0x40006c1c,A__IO|A_uint16_t},
 {"BKP.DR8"               ,0x40006c20,A__IO|A_uint16_t},
 {"BKP.DR9"               ,0x40006c24,A__IO|A_uint16_t},
 {"BKP.DR10"              ,0x40006c28,A__IO|A_uint16_t},
 {"BKP.RTCCR"             ,0x40006c2c,A__IO|A_uint16_t},
 {"BKP.CR"                ,0x40006c30,A__IO|A_uint16_t},
 {"BKP.CSR"               ,0x40006c34,A__IO|A_uint16_t},
 {"BKP.DR11"              ,0x40006c40,A__IO|A_uint16_t},
 {"BKP.DR12"              ,0x40006c44,A__IO|A_uint16_t},
 {"BKP.DR13"              ,0x40006c48,A__IO|A_uint16_t},
 {"BKP.DR14"              ,0x40006c4c,A__IO|A_uint16_t},
 {"BKP.DR15"              ,0x40006c50,A__IO|A_uint16_t},
 {"BKP.DR16"              ,0x40006c54,A__IO|A_uint16_t},
 {"BKP.DR17"              ,0x40006c58,A__IO|A_uint16_t},
 {"BKP.DR18"              ,0x40006c5c,A__IO|A_uint16_t},
 {"BKP.DR19"              ,0x40006c60,A__IO|A_uint16_t},
 {"BKP.DR20"              ,0x40006c64,A__IO|A_uint16_t},
 {"BKP.DR21"              ,0x40006c68,A__IO|A_uint16_t},
 {"BKP.DR22"              ,0x40006c6c,A__IO|A_uint16_t},
 {"BKP.DR23"              ,0x40006c70,A__IO|A_uint16_t},
 {"BKP.DR24"              ,0x40006c74,A__IO|A_uint16_t},
 {"BKP.DR25"              ,0x40006c78,A__IO|A_uint16_t},
 {"BKP.DR26"              ,0x40006c7c,A__IO|A_uint16_t},
 {"BKP.DR27"              ,0x40006c80,A__IO|A_uint16_t},
 {"BKP.DR28"              ,0x40006c84,A__IO|A_uint16_t},
 {"BKP.DR29"              ,0x40006c88,A__IO|A_uint16_t},
 {"BKP.DR30"              ,0x40006c8c,A__IO|A_uint16_t},
 {"BKP.DR31"              ,0x40006c90,A__IO|A_uint16_t},
 {"BKP.DR32"              ,0x40006c94,A__IO|A_uint16_t},
 {"BKP.DR33"              ,0x40006c98,A__IO|A_uint16_t},
 {"BKP.DR34"              ,0x40006c9c,A__IO|A_uint16_t},
 {"BKP.DR35"              ,0x40006ca0,A__IO|A_uint16_t},
 {"BKP.DR36"              ,0x40006ca4,A__IO|A_uint16_t},
 {"BKP.DR37"              ,0x40006ca8,A__IO|A_uint16_t},
 {"BKP.DR38"              ,0x40006cac,A__IO|A_uint16_t},
 {"BKP.DR39"              ,0x40006cb0,A__IO|A_uint16_t},
 {"BKP.DR40"              ,0x40006cb4,A__IO|A_uint16_t},
 {"BKP.DR41"              ,0x40006cb8,A__IO|A_uint16_t},
 {"BKP.DR42"              ,0x40006cbc,A__IO|A_uint16_t},

// -----------------------
// port=PWR , type=PWR_TypeDef
 {"PWR.CR"                ,0x40007000,A__IO|A_uint32_t},
 {"PWR.CSR"               ,0x40007004,A__IO|A_uint32_t},

// -----------------------
// port=DAC , type=DAC_TypeDef
 {"DAC.CR"                ,0x40007400,A__IO|A_uint32_t},
 {"DAC.SWTRIGR"           ,0x40007404,A__IO|A_uint32_t},
 {"DAC.DHR12R1"           ,0x40007408,A__IO|A_uint32_t},
 {"DAC.DHR12L1"           ,0x4000740c,A__IO|A_uint32_t},
 {"DAC.DHR8R1"            ,0x40007410,A__IO|A_uint32_t},
 {"DAC.DHR12R2"           ,0x40007414,A__IO|A_uint32_t},
 {"DAC.DHR12L2"           ,0x40007418,A__IO|A_uint32_t},
 {"DAC.DHR8R2"            ,0x4000741c,A__IO|A_uint32_t},
 {"DAC.DHR12RD"           ,0x40007420,A__IO|A_uint32_t},
 {"DAC.DHR12LD"           ,0x40007424,A__IO|A_uint32_t},
 {"DAC.DHR8RD"            ,0x40007428,A__IO|A_uint32_t},
 {"DAC.DOR1"              ,0x4000742c,A__IO|A_uint32_t},
 {"DAC.DOR2"              ,0x40007430,A__IO|A_uint32_t},
 {"DAC.SR"                ,0x40007434,A__IO|A_uint32_t},

// -----------------------
// port=CEC , type=CEC_TypeDef
 {"CEC.CFGR"              ,0x40007800,A__IO|A_uint32_t},
 {"CEC.OAR"               ,0x40007804,A__IO|A_uint32_t},
 {"CEC.PRES"              ,0x40007808,A__IO|A_uint32_t},
 {"CEC.ESR"               ,0x4000780c,A__IO|A_uint32_t},
 {"CEC.CSR"               ,0x40007810,A__IO|A_uint32_t},
 {"CEC.TXD"               ,0x40007814,A__IO|A_uint32_t},
 {"CEC.RXD"               ,0x40007818,A__IO|A_uint32_t},

// -----------------------
// port=AFIO , type=AFIO_TypeDef
 {"AFIO.EVCR"             ,0x40010000,A__IO|A_uint32_t},
 {"AFIO.MAPR"             ,0x40010004,A__IO|A_uint32_t},
 {"AFIO.EXTICR[4]"        ,0x40010008,A__IO|A_uint32_t},
 {"AFIO.MAPR2"            ,0x4001001c,A__IO|A_uint32_t},

// -----------------------
// port=EXTI , type=EXTI_TypeDef
 {"EXTI.IMR"              ,0x40010400,A__IO|A_uint32_t},
 {"EXTI.EMR"              ,0x40010404,A__IO|A_uint32_t},
 {"EXTI.RTSR"             ,0x40010408,A__IO|A_uint32_t},
 {"EXTI.FTSR"             ,0x4001040c,A__IO|A_uint32_t},
 {"EXTI.SWIER"            ,0x40010410,A__IO|A_uint32_t},
 {"EXTI.PR"               ,0x40010414,A__IO|A_uint32_t},

// -----------------------
// port=GPIOA , type=GPIO_TypeDef
 {"GPIOA.CRL"             ,0x40010800,A__IO|A_uint32_t|QQ},
 {"GPIOA.CRH"             ,0x40010804,A__IO|A_uint32_t|QQ},
 {"GPIOA.IDR"             ,0x40010808,A__IO|A_uint32_t|QQ},
 {"GPIOA.ODR"             ,0x4001080c,A__IO|A_uint32_t|QQ},
 {"GPIOA.BSRR"            ,0x40010810,A__IO|A_uint32_t},
 {"GPIOA.BRR"             ,0x40010814,A__IO|A_uint32_t},
 {"GPIOA.LCKR"            ,0x40010818,A__IO|A_uint32_t},

// -----------------------
// port=GPIOB , type=GPIO_TypeDef
 {"GPIOB.CRL"             ,0x40010c00,A__IO|A_uint32_t|QQ},
 {"GPIOB.CRH"             ,0x40010c04,A__IO|A_uint32_t|QQ},
 {"GPIOB.IDR"             ,0x40010c08,A__IO|A_uint32_t|QQ},
 {"GPIOB.ODR"             ,0x40010c0c,A__IO|A_uint32_t|QQ},
 {"GPIOB.BSRR"            ,0x40010c10,A__IO|A_uint32_t},
 {"GPIOB.BRR"             ,0x40010c14,A__IO|A_uint32_t},
 {"GPIOB.LCKR"            ,0x40010c18,A__IO|A_uint32_t},

// -----------------------
// port=GPIOC , type=GPIO_TypeDef
 {"GPIOC.CRL"             ,0x40011000,A__IO|A_uint32_t|QQ},
 {"GPIOC.CRH"             ,0x40011004,A__IO|A_uint32_t|QQ},
 {"GPIOC.IDR"             ,0x40011008,A__IO|A_uint32_t|QQ},
 {"GPIOC.ODR"             ,0x4001100c,A__IO|A_uint32_t|QQ},
 {"GPIOC.BSRR"            ,0x40011010,A__IO|A_uint32_t},
 {"GPIOC.BRR"             ,0x40011014,A__IO|A_uint32_t},
 {"GPIOC.LCKR"            ,0x40011018,A__IO|A_uint32_t},

// -----------------------
// port=GPIOD , type=GPIO_TypeDef
 {"GPIOD.CRL"             ,0x40011400,A__IO|A_uint32_t|QQ},
 {"GPIOD.CRH"             ,0x40011404,A__IO|A_uint32_t|QQ},
 {"GPIOD.IDR"             ,0x40011408,A__IO|A_uint32_t|QQ},
 {"GPIOD.ODR"             ,0x4001140c,A__IO|A_uint32_t|QQ},
 {"GPIOD.BSRR"            ,0x40011410,A__IO|A_uint32_t},
 {"GPIOD.BRR"             ,0x40011414,A__IO|A_uint32_t},
 {"GPIOD.LCKR"            ,0x40011418,A__IO|A_uint32_t},

// -----------------------
// port=GPIOE , type=GPIO_TypeDef
 {"GPIOE.CRL"             ,0x40011800,A__IO|A_uint32_t|QQ},
 {"GPIOE.CRH"             ,0x40011804,A__IO|A_uint32_t|QQ},
 {"GPIOE.IDR"             ,0x40011808,A__IO|A_uint32_t|QQ},
 {"GPIOE.ODR"             ,0x4001180c,A__IO|A_uint32_t|QQ},
 {"GPIOE.BSRR"            ,0x40011810,A__IO|A_uint32_t},
 {"GPIOE.BRR"             ,0x40011814,A__IO|A_uint32_t},
 {"GPIOE.LCKR"            ,0x40011818,A__IO|A_uint32_t},

// -----------------------
// port=GPIOF , type=GPIO_TypeDef
 {"GPIOF.CRL"             ,0x40011c00,A__IO|A_uint32_t},
 {"GPIOF.CRH"             ,0x40011c04,A__IO|A_uint32_t},
 {"GPIOF.IDR"             ,0x40011c08,A__IO|A_uint32_t},
 {"GPIOF.ODR"             ,0x40011c0c,A__IO|A_uint32_t},
 {"GPIOF.BSRR"            ,0x40011c10,A__IO|A_uint32_t},
 {"GPIOF.BRR"             ,0x40011c14,A__IO|A_uint32_t},
 {"GPIOF.LCKR"            ,0x40011c18,A__IO|A_uint32_t},

// -----------------------
// port=GPIOG , type=GPIO_TypeDef
 {"GPIOG.CRL"             ,0x40012000,A__IO|A_uint32_t},
 {"GPIOG.CRH"             ,0x40012004,A__IO|A_uint32_t},
 {"GPIOG.IDR"             ,0x40012008,A__IO|A_uint32_t},
 {"GPIOG.ODR"             ,0x4001200c,A__IO|A_uint32_t},
 {"GPIOG.BSRR"            ,0x40012010,A__IO|A_uint32_t},
 {"GPIOG.BRR"             ,0x40012014,A__IO|A_uint32_t},
 {"GPIOG.LCKR"            ,0x40012018,A__IO|A_uint32_t},

// -----------------------
// port=ADC1 , type=ADC_TypeDef
 {"ADC1.SR"               ,0x40012400,A__IO|A_uint32_t},
 {"ADC1.CR1"              ,0x40012404,A__IO|A_uint32_t},
 {"ADC1.CR2"              ,0x40012408,A__IO|A_uint32_t},
 {"ADC1.SMPR1"            ,0x4001240c,A__IO|A_uint32_t},
 {"ADC1.SMPR2"            ,0x40012410,A__IO|A_uint32_t},
 {"ADC1.JOFR1"            ,0x40012414,A__IO|A_uint32_t},
 {"ADC1.JOFR2"            ,0x40012418,A__IO|A_uint32_t},
 {"ADC1.JOFR3"            ,0x4001241c,A__IO|A_uint32_t},
 {"ADC1.JOFR4"            ,0x40012420,A__IO|A_uint32_t},
 {"ADC1.HTR"              ,0x40012424,A__IO|A_uint32_t},
 {"ADC1.LTR"              ,0x40012428,A__IO|A_uint32_t},
 {"ADC1.SQR1"             ,0x4001242c,A__IO|A_uint32_t},
 {"ADC1.SQR2"             ,0x40012430,A__IO|A_uint32_t},
 {"ADC1.SQR3"             ,0x40012434,A__IO|A_uint32_t},
 {"ADC1.JSQR"             ,0x40012438,A__IO|A_uint32_t},
 {"ADC1.JDR1"             ,0x4001243c,A__IO|A_uint32_t},
 {"ADC1.JDR2"             ,0x40012440,A__IO|A_uint32_t},
 {"ADC1.JDR3"             ,0x40012444,A__IO|A_uint32_t},
 {"ADC1.JDR4"             ,0x40012448,A__IO|A_uint32_t},
 {"ADC1.DR"               ,0x4001244c,A__IO|A_uint32_t},

// -----------------------
// port=ADC2 , type=ADC_TypeDef
 {"ADC2.SR"               ,0x40012800,A__IO|A_uint32_t},
 {"ADC2.CR1"              ,0x40012804,A__IO|A_uint32_t},
 {"ADC2.CR2"              ,0x40012808,A__IO|A_uint32_t},
 {"ADC2.SMPR1"            ,0x4001280c,A__IO|A_uint32_t},
 {"ADC2.SMPR2"            ,0x40012810,A__IO|A_uint32_t},
 {"ADC2.JOFR1"            ,0x40012814,A__IO|A_uint32_t},
 {"ADC2.JOFR2"            ,0x40012818,A__IO|A_uint32_t},
 {"ADC2.JOFR3"            ,0x4001281c,A__IO|A_uint32_t},
 {"ADC2.JOFR4"            ,0x40012820,A__IO|A_uint32_t},
 {"ADC2.HTR"              ,0x40012824,A__IO|A_uint32_t},
 {"ADC2.LTR"              ,0x40012828,A__IO|A_uint32_t},
 {"ADC2.SQR1"             ,0x4001282c,A__IO|A_uint32_t},
 {"ADC2.SQR2"             ,0x40012830,A__IO|A_uint32_t},
 {"ADC2.SQR3"             ,0x40012834,A__IO|A_uint32_t},
 {"ADC2.JSQR"             ,0x40012838,A__IO|A_uint32_t},
 {"ADC2.JDR1"             ,0x4001283c,A__IO|A_uint32_t},
 {"ADC2.JDR2"             ,0x40012840,A__IO|A_uint32_t},
 {"ADC2.JDR3"             ,0x40012844,A__IO|A_uint32_t},
 {"ADC2.JDR4"             ,0x40012848,A__IO|A_uint32_t},
 {"ADC2.DR"               ,0x4001284c,A__IO|A_uint32_t},

// -----------------------
// port=TIM1 , type=TIM_TypeDef
 {"TIM1.CR1"              ,0x40012c00,A__IO|A_uint16_t},
 {"TIM1.CR2"              ,0x40012c04,A__IO|A_uint16_t},
 {"TIM1.SMCR"             ,0x40012c08,A__IO|A_uint16_t},
 {"TIM1.DIER"             ,0x40012c0c,A__IO|A_uint16_t},
 {"TIM1.SR"               ,0x40012c10,A__IO|A_uint16_t},
 {"TIM1.EGR"              ,0x40012c14,A__IO|A_uint16_t},
 {"TIM1.CCMR1"            ,0x40012c18,A__IO|A_uint16_t},
 {"TIM1.CCMR2"            ,0x40012c1c,A__IO|A_uint16_t},
 {"TIM1.CCER"             ,0x40012c20,A__IO|A_uint16_t},
 {"TIM1.CNT"              ,0x40012c24,A__IO|A_uint16_t},
 {"TIM1.PSC"              ,0x40012c28,A__IO|A_uint16_t},
 {"TIM1.ARR"              ,0x40012c2c,A__IO|A_uint16_t},
 {"TIM1.RCR"              ,0x40012c30,A__IO|A_uint16_t},
 {"TIM1.CCR1"             ,0x40012c34,A__IO|A_uint16_t},
 {"TIM1.CCR2"             ,0x40012c38,A__IO|A_uint16_t},
 {"TIM1.CCR3"             ,0x40012c3c,A__IO|A_uint16_t},
 {"TIM1.CCR4"             ,0x40012c40,A__IO|A_uint16_t},
 {"TIM1.BDTR"             ,0x40012c44,A__IO|A_uint16_t},
 {"TIM1.DCR"              ,0x40012c48,A__IO|A_uint16_t},
 {"TIM1.DMAR"             ,0x40012c4c,A__IO|A_uint16_t},

// -----------------------
// port=SPI1 , type=SPI_TypeDef
 {"SPI1.CR1"              ,0x40013000,A__IO|A_uint16_t},
 {"SPI1.CR2"              ,0x40013004,A__IO|A_uint16_t},
 {"SPI1.SR"               ,0x40013008,A__IO|A_uint16_t},
 {"SPI1.DR"               ,0x4001300c,A__IO|A_uint16_t},
 {"SPI1.CRCPR"            ,0x40013010,A__IO|A_uint16_t},
 {"SPI1.RXCRCR"           ,0x40013014,A__IO|A_uint16_t},
 {"SPI1.TXCRCR"           ,0x40013018,A__IO|A_uint16_t},
 {"SPI1.I2SCFGR"          ,0x4001301c,A__IO|A_uint16_t},
 {"SPI1.I2SPR"            ,0x40013020,A__IO|A_uint16_t},

// -----------------------
// port=TIM8 , type=TIM_TypeDef
 {"TIM8.CR1"              ,0x40013400,A__IO|A_uint16_t},
 {"TIM8.CR2"              ,0x40013404,A__IO|A_uint16_t},
 {"TIM8.SMCR"             ,0x40013408,A__IO|A_uint16_t},
 {"TIM8.DIER"             ,0x4001340c,A__IO|A_uint16_t},
 {"TIM8.SR"               ,0x40013410,A__IO|A_uint16_t},
 {"TIM8.EGR"              ,0x40013414,A__IO|A_uint16_t},
 {"TIM8.CCMR1"            ,0x40013418,A__IO|A_uint16_t},
 {"TIM8.CCMR2"            ,0x4001341c,A__IO|A_uint16_t},
 {"TIM8.CCER"             ,0x40013420,A__IO|A_uint16_t},
 {"TIM8.CNT"              ,0x40013424,A__IO|A_uint16_t},
 {"TIM8.PSC"              ,0x40013428,A__IO|A_uint16_t},
 {"TIM8.ARR"              ,0x4001342c,A__IO|A_uint16_t},
 {"TIM8.RCR"              ,0x40013430,A__IO|A_uint16_t},
 {"TIM8.CCR1"             ,0x40013434,A__IO|A_uint16_t},
 {"TIM8.CCR2"             ,0x40013438,A__IO|A_uint16_t},
 {"TIM8.CCR3"             ,0x4001343c,A__IO|A_uint16_t},
 {"TIM8.CCR4"             ,0x40013440,A__IO|A_uint16_t},
 {"TIM8.BDTR"             ,0x40013444,A__IO|A_uint16_t},
 {"TIM8.DCR"              ,0x40013448,A__IO|A_uint16_t},
 {"TIM8.DMAR"             ,0x4001344c,A__IO|A_uint16_t},

// -----------------------
// port=USART1 , type=USART_TypeDef
 {"USART1.SR"             ,0x40013800,A__IO|A_uint16_t},
 {"USART1.DR"             ,0x40013804,A__IO|A_uint16_t},
 {"USART1.BRR"            ,0x40013808,A__IO|A_uint16_t},
 {"USART1.CR1"            ,0x4001380c,A__IO|A_uint16_t},
 {"USART1.CR2"            ,0x40013810,A__IO|A_uint16_t},
 {"USART1.CR3"            ,0x40013814,A__IO|A_uint16_t},
 {"USART1.GTPR"           ,0x40013818,A__IO|A_uint16_t},

// -----------------------
// port=ADC3 , type=ADC_TypeDef
 {"ADC3.SR"               ,0x40013c00,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.CR1"              ,0x40013c04,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.CR2"              ,0x40013c08,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.SMPR1"            ,0x40013c0c,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.SMPR2"            ,0x40013c10,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.JOFR1"            ,0x40013c14,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.JOFR2"            ,0x40013c18,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.JOFR3"            ,0x40013c1c,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.JOFR4"            ,0x40013c20,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.HTR"              ,0x40013c24,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.LTR"              ,0x40013c28,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.SQR1"             ,0x40013c2c,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.SQR2"             ,0x40013c30,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.SQR3"             ,0x40013c34,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.JSQR"             ,0x40013c38,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.JDR1"             ,0x40013c3c,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.JDR2"             ,0x40013c40,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.JDR3"             ,0x40013c44,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.JDR4"             ,0x40013c48,A__IO|A_uint32_t|EX_PORT},
 {"ADC3.DR"               ,0x40013c4c,A__IO|A_uint32_t|EX_PORT},

// -----------------------
// port=TIM15 , type=TIM_TypeDef
 {"TIM15.CR1"             ,0x40014000,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.CR2"             ,0x40014004,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.SMCR"            ,0x40014008,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.DIER"            ,0x4001400c,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.SR"              ,0x40014010,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.EGR"             ,0x40014014,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.CCMR1"           ,0x40014018,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.CCMR2"           ,0x4001401c,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.CCER"            ,0x40014020,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.CNT"             ,0x40014024,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.PSC"             ,0x40014028,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.ARR"             ,0x4001402c,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.RCR"             ,0x40014030,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.CCR1"            ,0x40014034,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.CCR2"            ,0x40014038,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.CCR3"            ,0x4001403c,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.CCR4"            ,0x40014040,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.BDTR"            ,0x40014044,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.DCR"             ,0x40014048,A__IO|A_uint16_t|EX_PORT},
 {"TIM15.DMAR"            ,0x4001404c,A__IO|A_uint16_t|EX_PORT},

// -----------------------
// port=TIM16 , type=TIM_TypeDef
 {"TIM16.CR1"             ,0x40014400,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.CR2"             ,0x40014404,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.SMCR"            ,0x40014408,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.DIER"            ,0x4001440c,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.SR"              ,0x40014410,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.EGR"             ,0x40014414,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.CCMR1"           ,0x40014418,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.CCMR2"           ,0x4001441c,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.CCER"            ,0x40014420,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.CNT"             ,0x40014424,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.PSC"             ,0x40014428,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.ARR"             ,0x4001442c,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.RCR"             ,0x40014430,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.CCR1"            ,0x40014434,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.CCR2"            ,0x40014438,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.CCR3"            ,0x4001443c,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.CCR4"            ,0x40014440,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.BDTR"            ,0x40014444,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.DCR"             ,0x40014448,A__IO|A_uint16_t|EX_PORT},
 {"TIM16.DMAR"            ,0x4001444c,A__IO|A_uint16_t|EX_PORT},

// -----------------------
// port=TIM17 , type=TIM_TypeDef
 {"TIM17.CR1"             ,0x40014800,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.CR2"             ,0x40014804,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.SMCR"            ,0x40014808,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.DIER"            ,0x4001480c,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.SR"              ,0x40014810,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.EGR"             ,0x40014814,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.CCMR1"           ,0x40014818,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.CCMR2"           ,0x4001481c,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.CCER"            ,0x40014820,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.CNT"             ,0x40014824,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.PSC"             ,0x40014828,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.ARR"             ,0x4001482c,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.RCR"             ,0x40014830,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.CCR1"            ,0x40014834,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.CCR2"            ,0x40014838,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.CCR3"            ,0x4001483c,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.CCR4"            ,0x40014840,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.BDTR"            ,0x40014844,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.DCR"             ,0x40014848,A__IO|A_uint16_t|EX_PORT},
 {"TIM17.DMAR"            ,0x4001484c,A__IO|A_uint16_t|EX_PORT},

// -----------------------
// port=TIM9 , type=TIM_TypeDef
 {"TIM9.CR1"              ,0x40014c00,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.CR2"              ,0x40014c04,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.SMCR"             ,0x40014c08,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.DIER"             ,0x40014c0c,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.SR"               ,0x40014c10,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.EGR"              ,0x40014c14,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.CCMR1"            ,0x40014c18,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.CCMR2"            ,0x40014c1c,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.CCER"             ,0x40014c20,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.CNT"              ,0x40014c24,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.PSC"              ,0x40014c28,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.ARR"              ,0x40014c2c,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.RCR"              ,0x40014c30,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.CCR1"             ,0x40014c34,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.CCR2"             ,0x40014c38,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.CCR3"             ,0x40014c3c,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.CCR4"             ,0x40014c40,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.BDTR"             ,0x40014c44,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.DCR"              ,0x40014c48,A__IO|A_uint16_t|EX_PORT},
 {"TIM9.DMAR"             ,0x40014c4c,A__IO|A_uint16_t|EX_PORT},

// -----------------------
// port=TIM10 , type=TIM_TypeDef
 {"TIM10.CR1"             ,0x40015000,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.CR2"             ,0x40015004,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.SMCR"            ,0x40015008,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.DIER"            ,0x4001500c,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.SR"              ,0x40015010,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.EGR"             ,0x40015014,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.CCMR1"           ,0x40015018,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.CCMR2"           ,0x4001501c,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.CCER"            ,0x40015020,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.CNT"             ,0x40015024,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.PSC"             ,0x40015028,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.ARR"             ,0x4001502c,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.RCR"             ,0x40015030,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.CCR1"            ,0x40015034,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.CCR2"            ,0x40015038,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.CCR3"            ,0x4001503c,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.CCR4"            ,0x40015040,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.BDTR"            ,0x40015044,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.DCR"             ,0x40015048,A__IO|A_uint16_t|EX_PORT},
 {"TIM10.DMAR"            ,0x4001504c,A__IO|A_uint16_t|EX_PORT},

// -----------------------
// port=TIM11 , type=TIM_TypeDef
 {"TIM11.CR1"             ,0x40015400,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.CR2"             ,0x40015404,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.SMCR"            ,0x40015408,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.DIER"            ,0x4001540c,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.SR"              ,0x40015410,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.EGR"             ,0x40015414,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.CCMR1"           ,0x40015418,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.CCMR2"           ,0x4001541c,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.CCER"            ,0x40015420,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.CNT"             ,0x40015424,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.PSC"             ,0x40015428,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.ARR"             ,0x4001542c,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.RCR"             ,0x40015430,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.CCR1"            ,0x40015434,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.CCR2"            ,0x40015438,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.CCR3"            ,0x4001543c,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.CCR4"            ,0x40015440,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.BDTR"            ,0x40015444,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.DCR"             ,0x40015448,A__IO|A_uint16_t|EX_PORT},
 {"TIM11.DMAR"            ,0x4001544c,A__IO|A_uint16_t|EX_PORT},
// -----------------------
// port=SDIO , type=SDIO_TypeDef
 {"SDIO.POWER"            ,0x40018000,A__IO|A_uint32_t|EX_PORT},
 {"SDIO.CLKCR"            ,0x40018004,A__IO|A_uint32_t|EX_PORT},
 {"SDIO.ARG"              ,0x40018008,A__IO|A_uint32_t|EX_PORT},
 {"SDIO.CMD"              ,0x4001800c,A__IO|A_uint32_t|EX_PORT},
 {"SDIO.RESPCMD"          ,0x40018010, A__I|A_uint32_t|EX_PORT},
 {"SDIO.RESP1"            ,0x40018014, A__I|A_uint32_t|EX_PORT},
 {"SDIO.RESP2"            ,0x40018018, A__I|A_uint32_t|EX_PORT},
 {"SDIO.RESP3"            ,0x4001801c, A__I|A_uint32_t|EX_PORT},
 {"SDIO.RESP4"            ,0x40018020, A__I|A_uint32_t|EX_PORT},
 {"SDIO.DTIMER"           ,0x40018024,A__IO|A_uint32_t|EX_PORT},
 {"SDIO.DLEN"             ,0x40018028,A__IO|A_uint32_t|EX_PORT},
 {"SDIO.DCTRL"            ,0x4001802c,A__IO|A_uint32_t|EX_PORT},
 {"SDIO.DCOUNT"           ,0x40018030, A__I|A_uint32_t|EX_PORT},
 {"SDIO.STA"              ,0x40018034, A__I|A_uint32_t|EX_PORT},
 {"SDIO.ICR"              ,0x40018038,A__IO|A_uint32_t|EX_PORT},
 {"SDIO.MASK"             ,0x4001803c,A__IO|A_uint32_t|EX_PORT},
 {"SDIO.FIFOCNT"          ,0x40018048, A__I|A_uint32_t|EX_PORT},
 {"SDIO.FIFO"             ,0x40018080,A__IO|A_uint32_t|EX_PORT},

// -----------------------
// port=DMA1 , type=DMA_TypeDef
 {"DMA1.ISR"              ,0x40020000,A__IO|A_uint32_t},
 {"DMA1.IFCR"             ,0x40020004,A__IO|A_uint32_t},

// -----------------------
// port=DMA2 , type=DMA_TypeDef
 {"DMA2.ISR"              ,0x40020400,A__IO|A_uint32_t|EX_PORT},
 {"DMA2.IFCR"             ,0x40020404,A__IO|A_uint32_t|EX_PORT},

// -----------------------
// port=DMA1_Channel1 , type=DMA_Channel_TypeDef
 {"DMA1_Channel1.CCR"     ,0x40020008,A__IO|A_uint32_t},
 {"DMA1_Channel1.CNDTR"   ,0x4002000c,A__IO|A_uint32_t},
 {"DMA1_Channel1.CPAR"    ,0x40020010,A__IO|A_uint32_t},
 {"DMA1_Channel1.CMAR"    ,0x40020014,A__IO|A_uint32_t},

// -----------------------
// port=DMA1_Channel2 , type=DMA_Channel_TypeDef
 {"DMA1_Channel2.CCR"     ,0x4002001c,A__IO|A_uint32_t},
 {"DMA1_Channel2.CNDTR"   ,0x40020020,A__IO|A_uint32_t},
 {"DMA1_Channel2.CPAR"    ,0x40020024,A__IO|A_uint32_t},
 {"DMA1_Channel2.CMAR"    ,0x40020028,A__IO|A_uint32_t},

// -----------------------
// port=DMA1_Channel3 , type=DMA_Channel_TypeDef
 {"DMA1_Channel3.CCR"     ,0x40020030,A__IO|A_uint32_t},
 {"DMA1_Channel3.CNDTR"   ,0x40020034,A__IO|A_uint32_t},
 {"DMA1_Channel3.CPAR"    ,0x40020038,A__IO|A_uint32_t},
 {"DMA1_Channel3.CMAR"    ,0x4002003c,A__IO|A_uint32_t},

// -----------------------
// port=DMA1_Channel4 , type=DMA_Channel_TypeDef
 {"DMA1_Channel4.CCR"     ,0x40020044,A__IO|A_uint32_t},
 {"DMA1_Channel4.CNDTR"   ,0x40020048,A__IO|A_uint32_t},
 {"DMA1_Channel4.CPAR"    ,0x4002004c,A__IO|A_uint32_t},
 {"DMA1_Channel4.CMAR"    ,0x40020050,A__IO|A_uint32_t},

// -----------------------
// port=DMA1_Channel5 , type=DMA_Channel_TypeDef
 {"DMA1_Channel5.CCR"     ,0x40020058,A__IO|A_uint32_t},
 {"DMA1_Channel5.CNDTR"   ,0x4002005c,A__IO|A_uint32_t},
 {"DMA1_Channel5.CPAR"    ,0x40020060,A__IO|A_uint32_t},
 {"DMA1_Channel5.CMAR"    ,0x40020064,A__IO|A_uint32_t},

// -----------------------
// port=DMA1_Channel6 , type=DMA_Channel_TypeDef
 {"DMA1_Channel6.CCR"     ,0x4002006c,A__IO|A_uint32_t},
 {"DMA1_Channel6.CNDTR"   ,0x40020070,A__IO|A_uint32_t},
 {"DMA1_Channel6.CPAR"    ,0x40020074,A__IO|A_uint32_t},
 {"DMA1_Channel6.CMAR"    ,0x40020078,A__IO|A_uint32_t},

// -----------------------
// port=DMA1_Channel7 , type=DMA_Channel_TypeDef
 {"DMA1_Channel7.CCR"     ,0x40020080,A__IO|A_uint32_t},
 {"DMA1_Channel7.CNDTR"   ,0x40020084,A__IO|A_uint32_t},
 {"DMA1_Channel7.CPAR"    ,0x40020088,A__IO|A_uint32_t},
 {"DMA1_Channel7.CMAR"    ,0x4002008c,A__IO|A_uint32_t},

// -----------------------
// port=DMA2_Channel1 , type=DMA_Channel_TypeDef
 {"DMA2_Channel1.CCR"     ,0x40020408,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel1.CNDTR"   ,0x4002040c,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel1.CPAR"    ,0x40020410,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel1.CMAR"    ,0x40020414,A__IO|A_uint32_t|EX_PORT},

// -----------------------
// port=DMA2_Channel2 , type=DMA_Channel_TypeDef
 {"DMA2_Channel2.CCR"     ,0x4002041c,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel2.CNDTR"   ,0x40020420,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel2.CPAR"    ,0x40020424,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel2.CMAR"    ,0x40020428,A__IO|A_uint32_t|EX_PORT},

// -----------------------
// port=DMA2_Channel3 , type=DMA_Channel_TypeDef
 {"DMA2_Channel3.CCR"     ,0x40020430,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel3.CNDTR"   ,0x40020434,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel3.CPAR"    ,0x40020438,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel3.CMAR"    ,0x4002043c,A__IO|A_uint32_t|EX_PORT},

// -----------------------
// port=DMA2_Channel4 , type=DMA_Channel_TypeDef
 {"DMA2_Channel4.CCR"     ,0x40020444,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel4.CNDTR"   ,0x40020448,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel4.CPAR"    ,0x4002044c,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel4.CMAR"    ,0x40020450,A__IO|A_uint32_t|EX_PORT},

// -----------------------
// port=DMA2_Channel5 , type=DMA_Channel_TypeDef
 {"DMA2_Channel5.CCR"     ,0x40020458,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel5.CNDTR"   ,0x4002045c,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel5.CPAR"    ,0x40020460,A__IO|A_uint32_t|EX_PORT},
 {"DMA2_Channel5.CMAR"    ,0x40020464,A__IO|A_uint32_t|EX_PORT},

// -----------------------
// port=RCC , type=RCC_TypeDef
 {"RCC.CR"                ,0x40021000,A__IO|A_uint32_t},
 {"RCC.CFGR"              ,0x40021004,A__IO|A_uint32_t},
 {"RCC.CIR"               ,0x40021008,A__IO|A_uint32_t},
 {"RCC.APB2RSTR"          ,0x4002100c,A__IO|A_uint32_t},
 {"RCC.APB1RSTR"          ,0x40021010,A__IO|A_uint32_t},
 {"RCC.AHBENR"            ,0x40021014,A__IO|A_uint32_t},
 {"RCC.APB2ENR"           ,0x40021018,A__IO|A_uint32_t},
 {"RCC.APB1ENR"           ,0x4002101c,A__IO|A_uint32_t},
 {"RCC.BDCR"              ,0x40021020,A__IO|A_uint32_t},
 {"RCC.CSR"               ,0x40021024,A__IO|A_uint32_t},
 {"RCC.CFGR2"             ,0x4002102c,A__IO|A_uint32_t},

// -----------------------
// port=CRC , type=CRC_TypeDef
 {"CRC.DR"                ,0x40023000,A__IO|A_uint32_t},
 {"CRC.IDR"               ,0x40023004, A__IO|A_uint8_t},
 {"CRC.CR"                ,0x40023008,A__IO|A_uint32_t},

// -----------------------
// port=FLASH , type=FLASH_TypeDef
 {"FLASH.ACR"             ,0x40022000,A__IO|A_uint32_t},
 {"FLASH.KEYR"            ,0x40022004,A__IO|A_uint32_t},
 {"FLASH.OPTKEYR"         ,0x40022008,A__IO|A_uint32_t},
 {"FLASH.SR"              ,0x4002200c,A__IO|A_uint32_t},
 {"FLASH.CR"              ,0x40022010,A__IO|A_uint32_t},
 {"FLASH.AR"              ,0x40022014,A__IO|A_uint32_t},
 {"FLASH.OBR"             ,0x4002201c,A__IO|A_uint32_t},
 {"FLASH.WRPR"            ,0x40022020,A__IO|A_uint32_t},

// -----------------------
// port=OB , type=OB_TypeDef
 {"OB.RDP"                ,0x1ffff800,A__IO|A_uint16_t},
 {"OB.USER"               ,0x1ffff802,A__IO|A_uint16_t},
 {"OB.Data0"              ,0x1ffff804,A__IO|A_uint16_t},
 {"OB.Data1"              ,0x1ffff806,A__IO|A_uint16_t},
 {"OB.WRP0"               ,0x1ffff808,A__IO|A_uint16_t},
 {"OB.WRP1"               ,0x1ffff80a,A__IO|A_uint16_t},
 {"OB.WRP2"               ,0x1ffff80c,A__IO|A_uint16_t},
 {"OB.WRP3"               ,0x1ffff80e,A__IO|A_uint16_t},

#if	0
// -----------------------
// port=ETH , type=ETH_TypeDef
 {"ETH.MACCR"             ,0x40028000,A__IO|A_uint32_t},
 {"ETH.MACFFR"            ,0x40028004,A__IO|A_uint32_t},
 {"ETH.MACHTHR"           ,0x40028008,A__IO|A_uint32_t},
 {"ETH.MACHTLR"           ,0x4002800c,A__IO|A_uint32_t},
 {"ETH.MACMIIAR"          ,0x40028010,A__IO|A_uint32_t},
 {"ETH.MACMIIDR"          ,0x40028014,A__IO|A_uint32_t},
 {"ETH.MACFCR"            ,0x40028018,A__IO|A_uint32_t},
 {"ETH.MACVLANTR"         ,0x4002801c,A__IO|A_uint32_t},
 {"ETH.MACRWUFFR"         ,0x40028028,A__IO|A_uint32_t},
 {"ETH.MACPMTCSR"         ,0x4002802c,A__IO|A_uint32_t},
 {"ETH.MACSR"             ,0x40028038,A__IO|A_uint32_t},
 {"ETH.MACIMR"            ,0x4002803c,A__IO|A_uint32_t},
 {"ETH.MACA0HR"           ,0x40028040,A__IO|A_uint32_t},
 {"ETH.MACA0LR"           ,0x40028044,A__IO|A_uint32_t},
 {"ETH.MACA1HR"           ,0x40028048,A__IO|A_uint32_t},
 {"ETH.MACA1LR"           ,0x4002804c,A__IO|A_uint32_t},
 {"ETH.MACA2HR"           ,0x40028050,A__IO|A_uint32_t},
 {"ETH.MACA2LR"           ,0x40028054,A__IO|A_uint32_t},
 {"ETH.MACA3HR"           ,0x40028058,A__IO|A_uint32_t},
 {"ETH.MACA3LR"           ,0x4002805c,A__IO|A_uint32_t},
 {"ETH.MMCCR"             ,0x40028100,A__IO|A_uint32_t},
 {"ETH.MMCRIR"            ,0x40028104,A__IO|A_uint32_t},
 {"ETH.MMCTIR"            ,0x40028108,A__IO|A_uint32_t},
 {"ETH.MMCRIMR"           ,0x4002810c,A__IO|A_uint32_t},
 {"ETH.MMCTIMR"           ,0x40028110,A__IO|A_uint32_t},
 {"ETH.MMCTGFSCCR"        ,0x4002814c,A__IO|A_uint32_t},
 {"ETH.MMCTGFMSCCR"       ,0x40028150,A__IO|A_uint32_t},
 {"ETH.MMCTGFCR"          ,0x40028168,A__IO|A_uint32_t},
 {"ETH.MMCRFCECR"         ,0x40028194,A__IO|A_uint32_t},
 {"ETH.MMCRFAECR"         ,0x40028198,A__IO|A_uint32_t},
 {"ETH.MMCRGUFCR"         ,0x400281c4,A__IO|A_uint32_t},
 {"ETH.PTPTSCR"           ,0x40028700,A__IO|A_uint32_t},
 {"ETH.PTPSSIR"           ,0x40028704,A__IO|A_uint32_t},
 {"ETH.PTPTSHR"           ,0x40028708,A__IO|A_uint32_t},
 {"ETH.PTPTSLR"           ,0x4002870c,A__IO|A_uint32_t},
 {"ETH.PTPTSHUR"          ,0x40028710,A__IO|A_uint32_t},
 {"ETH.PTPTSLUR"          ,0x40028714,A__IO|A_uint32_t},
 {"ETH.PTPTSAR"           ,0x40028718,A__IO|A_uint32_t},
 {"ETH.PTPTTHR"           ,0x4002871c,A__IO|A_uint32_t},
 {"ETH.PTPTTLR"           ,0x40028720,A__IO|A_uint32_t},
 {"ETH.DMABMR"            ,0x40029000,A__IO|A_uint32_t},
 {"ETH.DMATPDR"           ,0x40029004,A__IO|A_uint32_t},
 {"ETH.DMARPDR"           ,0x40029008,A__IO|A_uint32_t},
 {"ETH.DMARDLAR"          ,0x4002900c,A__IO|A_uint32_t},
 {"ETH.DMATDLAR"          ,0x40029010,A__IO|A_uint32_t},
 {"ETH.DMASR"             ,0x40029014,A__IO|A_uint32_t},
 {"ETH.DMAOMR"            ,0x40029018,A__IO|A_uint32_t},
 {"ETH.DMAIER"            ,0x4002901c,A__IO|A_uint32_t},
 {"ETH.DMAMFBOCR"         ,0x40029020,A__IO|A_uint32_t},
 {"ETH.DMACHTDR"          ,0x40029048,A__IO|A_uint32_t},
 {"ETH.DMACHRDR"          ,0x4002904c,A__IO|A_uint32_t},
 {"ETH.DMACHTBAR"         ,0x40029050,A__IO|A_uint32_t},
 {"ETH.DMACHRBAR"         ,0x40029054,A__IO|A_uint32_t},
#endif
// -----------------------
// port=FSMC_Bank1 , type=FSMC_Bank1_TypeDef

// -----------------------
// port=FSMC_Bank1E , type=FSMC_Bank1E_TypeDef
 {"FSMC_Bank1E.BWTR[7]"   ,0xa0000104,A__IO|A_uint32_t|EX_PORT},

// -----------------------
// port=FSMC_Bank2 , type=FSMC_Bank2_TypeDef

// -----------------------
// port=FSMC_Bank3 , type=FSMC_Bank3_TypeDef

// -----------------------
// port=FSMC_Bank4 , type=FSMC_Bank4_TypeDef

// -----------------------
// port=DBGMCU , type=DBGMCU_TypeDef
 {"DBGMCU.IDCODE"         ,0xe0042000,A__IO|A_uint32_t},
 {"DBGMCU.CR"             ,0xe0042004,A__IO|A_uint32_t},
 {NULL                    ,0       ,  0       }
};

