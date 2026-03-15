/******************************************************************************
 * Copyright (C) 2021, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************/

/******************************************************************************
 * @file   startup_hc32l19x.S
 *
 * @brief  Startup file for HC32L19x series MCU (GCC)
 *
 * @author MADS Team
 *
 ******************************************************************************/

.syntax unified
.arch armv6-m
.thumb

/*******************************************************************************
 * Interrupt Vector Table
 ******************************************************************************/
.section .isr_vector, "a", %progbits
.type g_pfnVectors, %object

g_pfnVectors:
    /* Core Level - CM0P */
    .word   _estack                   /* 0: Top of Stack */
    .word   Reset_Handler             /* 1: Reset Handler */
    .word   NMI_Handler               /* 2: NMI Handler */
    .word   HardFault_Handler         /* 3: Hard Fault Handler */
    .word   0                         /* 4: Reserved */
    .word   0                         /* 5: Reserved */
    .word   0                         /* 6: Reserved */
    .word   0                         /* 7: Reserved */
    .word   0                         /* 8: Reserved */
    .word   0                         /* 9: Reserved */
    .word   0                         /* 10: Reserved */
    .word   SVC_Handler               /* 11: SV Call Handler */
    .word   0                         /* 12: Reserved */
    .word   0                         /* 13: Reserved */
    .word   PendSV_Handler            /* 14: Pend SV Handler */
    .word   SysTick_Handler           /* 15: SysTick Handler */

    /* External Interrupts - HC32L19x */
    .word   PORTA_IRQHandler          /* 0: PORTA */
    .word   PORTB_IRQHandler          /* 1: PORTB */
    .word   PORTC_E_IRQHandler        /* 2: PORTC_E */
    .word   PORTD_F_IRQHandler        /* 3: PORTD_F */
    .word   DMAC_IRQHandler           /* 4: DMAC */
    .word   TIM3_IRQHandler           /* 5: TIM3 */
    .word   UART0_2_IRQHandler        /* 6: UART0_2 */
    .word   UART1_3_IRQHandler        /* 7: UART1_3 */
    .word   LPUART0_IRQHandler        /* 8: LPUART0 */
    .word   LPUART1_IRQHandler        /* 9: LPUART1 */
    .word   SPI0_IRQHandler           /* 10: SPI0 */
    .word   SPI1_IRQHandler           /* 11: SPI1 */
    .word   I2C0_IRQHandler           /* 12: I2C0 */
    .word   I2C1_IRQHandler           /* 13: I2C1 */
    .word   TIM0_IRQHandler           /* 14: TIM0 */
    .word   TIM1_IRQHandler           /* 15: TIM1 */
    .word   TIM2_IRQHandler           /* 16: TIM2 */
    .word   LPTIM_0_1_IRQHandler      /* 17: LPTIM_0_1 */
    .word   ADTIM4_IRQHandler         /* 18: ADTIM4 */
    .word   ADTIM5_IRQHandler         /* 19: ADTIM5 */
    .word   ADTIM6_IRQHandler         /* 20: ADTIM6 */
    .word   PCA_IRQHandler            /* 21: PCA */
    .word   WDT_IRQHandler            /* 22: WDT */
    .word   RTC_IRQHandler            /* 23: RTC */
    .word   ADC_DAC_IRQHandler        /* 24: ADC_DAC */
    .word   PCNT_IRQHandler           /* 25: PCNT */
    .word   VC0_IRQHandler            /* 26: VC0 */
    .word   VC1_2_IRQHandler          /* 27: VC1_2 */
    .word   LVD_IRQHandler            /* 28: LVD */
    .word   LCD_IRQHandler            /* 29: LCD */
    .word   FLASH_RAM_IRQHandler      /* 30: FLASH_RAM */
    .word   CLKTRIM_IRQHandler        /* 31: CLKTRIM */

.size g_pfnVectors, .-g_pfnVectors

/*******************************************************************************
 * Reset Handler
 ******************************************************************************/
.text
.thumb_func
.global Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    /* Set stack pointer */
    ldr     r0, =_estack
    mov     sp, r0

    /* Copy data section from FLASH to SRAM */
    ldr     r0, =_sdata
    ldr     r1, =_edata
    ldr     r2, =_sidata
    b       .L_loop_data
.L_copy_data:
    ldr     r3, [r2], #4
    str     r3, [r0], #4
.L_loop_data:
    cmp     r0, r1
    bcc     .L_copy_data

    /* Clear bss section */
    ldr     r0, =_sbss
    ldr     r1, =_ebss
    mov     r2, #0
    b       .L_loop_bss
.L_zero_bss:
    str     r2, [r0], #4
.L_loop_bss:
    cmp     r0, r1
    bcc     .L_zero_bss

    /* Initialize system */
    bl      SystemInit

    /* Call static constructors */
    bl      __libc_init_array

    /* Call main function */
    bl      main

    /* If main returns, enter infinite loop */
.L_infinite_loop:
    b       .L_infinite_loop
.size Reset_Handler, .-Reset_Handler

/*******************************************************************************
 * Default Interrupt Handlers
 ******************************************************************************/
.weak   NMI_Handler
.thumb_set NMI_Handler, Default_Handler

.weak   HardFault_Handler
.thumb_set HardFault_Handler, Default_Handler

.weak   SVC_Handler
.thumb_set SVC_Handler, Default_Handler

.weak   PendSV_Handler
.thumb_set PendSV_Handler, Default_Handler

.weak   SysTick_Handler
.thumb_set SysTick_Handler, Default_Handler

.weak   PORTA_IRQHandler
.thumb_set PORTA_IRQHandler, Default_Handler

.weak   PORTB_IRQHandler
.thumb_set PORTB_IRQHandler, Default_Handler

.weak   PORTC_E_IRQHandler
.thumb_set PORTC_E_IRQHandler, Default_Handler

.weak   PORTD_F_IRQHandler
.thumb_set PORTD_F_IRQHandler, Default_Handler

.weak   DMAC_IRQHandler
.thumb_set DMAC_IRQHandler, Default_Handler

.weak   TIM3_IRQHandler
.thumb_set TIM3_IRQHandler, Default_Handler

.weak   UART0_2_IRQHandler
.thumb_set UART0_2_IRQHandler, Default_Handler

.weak   UART1_3_IRQHandler
.thumb_set UART1_3_IRQHandler, Default_Handler

.weak   LPUART0_IRQHandler
.thumb_set LPUART0_IRQHandler, Default_Handler

.weak   LPUART1_IRQHandler
.thumb_set LPUART1_IRQHandler, Default_Handler

.weak   SPI0_IRQHandler
.thumb_set SPI0_IRQHandler, Default_Handler

.weak   SPI1_IRQHandler
.thumb_set SPI1_IRQHandler, Default_Handler

.weak   I2C0_IRQHandler
.thumb_set I2C0_IRQHandler, Default_Handler

.weak   I2C1_IRQHandler
.thumb_set I2C1_IRQHandler, Default_Handler

.weak   TIM0_IRQHandler
.thumb_set TIM0_IRQHandler, Default_Handler

.weak   TIM1_IRQHandler
.thumb_set TIM1_IRQHandler, Default_Handler

.weak   TIM2_IRQHandler
.thumb_set TIM2_IRQHandler, Default_Handler

.weak   LPTIM_0_1_IRQHandler
.thumb_set LPTIM_0_1_IRQHandler, Default_Handler

.weak   ADTIM4_IRQHandler
.thumb_set ADTIM4_IRQHandler, Default_Handler

.weak   ADTIM5_IRQHandler
.thumb_set ADTIM5_IRQHandler, Default_Handler

.weak   ADTIM6_IRQHandler
.thumb_set ADTIM6_IRQHandler, Default_Handler

.weak   PCA_IRQHandler
.thumb_set PCA_IRQHandler, Default_Handler

.weak   WDT_IRQHandler
.thumb_set WDT_IRQHandler, Default_Handler

.weak   RTC_IRQHandler
.thumb_set RTC_IRQHandler, Default_Handler

.weak   ADC_DAC_IRQHandler
.thumb_set ADC_DAC_IRQHandler, Default_Handler

.weak   PCNT_IRQHandler
.thumb_set PCNT_IRQHandler, Default_Handler

.weak   VC0_IRQHandler
.thumb_set VC0_IRQHandler, Default_Handler

.weak   VC1_2_IRQHandler
.thumb_set VC1_2_IRQHandler, Default_Handler

.weak   LVD_IRQHandler
.thumb_set LVD_IRQHandler, Default_Handler

.weak   LCD_IRQHandler
.thumb_set LCD_IRQHandler, Default_Handler

.weak   FLASH_RAM_IRQHandler
.thumb_set FLASH_RAM_IRQHandler, Default_Handler

.weak   CLKTRIM_IRQHandler
.thumb_set CLKTRIM_IRQHandler, Default_Handler

/*******************************************************************************
 * Default Handler Implementation
 ******************************************************************************/
.weak   Default_Handler
.type Default_Handler, %function
Default_Handler:
    /* Infinite loop */
.L_default_infinite_loop:
    b       .L_default_infinite_loop
.size Default_Handler, .-Default_Handler

/*******************************************************************************
 * SystemInit Declaration
 ******************************************************************************/
.weak   SystemInit
.type SystemInit, %function
SystemInit:
    bx      lr
.size SystemInit, .-SystemInit
