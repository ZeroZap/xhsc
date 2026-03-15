/******************************************************************************
*
* Copyright (C) 2024, Xiaohua Semiconductor Co., Ltd. All rights reserved.
*
* This software component is licensed by XHSC under BSD 3-Clause license
* (the "License"); You may not use this file except in compliance with the
* License. You may obtain a copy of the License at:
*                    opensource.org/licenses/BSD-3-Clause
*
******************************************************************************/

/*****************************************************************************/
/*  Startup for GCC                                                          */
/*  Version     V1.0                                                         */
/*  Date        2024-12-01                                                   */
/*  Target-mcu  HC32L021                                                     */
/*****************************************************************************/

.syntax unified
.cpu cortex-m0plus
.thumb

/*******************************************************************************
* Stack Configuration
* Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
*******************************************************************************/
.stack_size = 0x00000400;

/*******************************************************************************
* Heap Configuration
* Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
*******************************************************************************/
.heap_size = 0x00000200;

/*******************************************************************************
* Stack Section
*******************************************************************************/
.section .stack, "w"
.align 3
.globl __Stack_Top
.globl __Stack_Limit
__Stack_Limit:
    .space .stack_size
__Stack_Top:

/*******************************************************************************
* Heap Section
*******************************************************************************/
.section .heap, "w"
.align 3
.globl __HeapBase
.globl __HeapLimit
__HeapBase:
    .space .heap_size
__HeapLimit:

/*******************************************************************************
* Vector Table
*******************************************************************************/
.section .isr_vector, "a"
.globl _isr_vector
_isr_vector:
    .long   __Stack_Top           /* Top of Stack */
    .long   Reset_Handler         /* Reset Handler */
    .long   NMI_Handler           /* NMI Handler */
    .long   HardFault_Handler     /* Hard Fault Handler */
    .long   0                     /* Reserved */
    .long   0                     /* Reserved */
    .long   0                     /* Reserved */
    .long   0                     /* Reserved */
    .long   0                     /* Reserved */
    .long   0                     /* Reserved */
    .long   0                     /* Reserved */
    .long   SVC_Handler           /* SVCall Handler */
    .long   0                     /* Reserved */
    .long   0                     /* Reserved */
    .long   PendSV_Handler        /* PendSV Handler */
    .long   SysTick_Handler       /* SysTick Handler */

    /* External Interrupts */
    .long   PORTA_IRQHandler          /* 0  - PORTA Interrupt */
    .long   PORTB_IRQHandler          /* 1  - PORTB Interrupt */
    .long   0                         /* 2  - Reserved */
    .long   0                         /* 3  - Reserved */
    .long   0                         /* 4  - Reserved */
    .long   ATIM3_IRQHandler          /* 5  - ATIM3 Interrupt */
    .long   0                         /* 6  - Reserved */
    .long   0                         /* 7  - Reserved */
    .long   LPUART0_IRQHandler        /* 8  - LPUART0 Interrupt */
    .long   LPUART1_IRQHandler        /* 9  - LPUART1 Interrupt */
    .long   SPI_IRQHandler            /* 10 - SPI Interrupt */
    .long   0                         /* 11 - Reserved */
    .long   0                         /* 12 - Reserved */
    .long   0                         /* 13 - Reserved */
    .long   CTIM0_IRQHandler          /* 14 - CTIM0 Interrupt */
    .long   CTIM1_IRQHandler          /* 15 - CTIM1 Interrupt */
    .long   0                         /* 16 - Reserved */
    .long   0                         /* 17 - Reserved */
    .long   HSI2C_IRQHandler          /* 18 - HSI2C Interrupt */
    .long   0                         /* 19 - Reserved */
    .long   0                         /* 20 - Reserved */
    .long   0                         /* 21 - Reserved */
    .long   IWDT_IRQHandler           /* 22 - IWDT Interrupt */
    .long   RTC_IRQHandler            /* 23 - RTC Interrupt */
    .long   ADC_IRQHandler            /* 24 - ADC Interrupt */
    .long   0                         /* 25 - Reserved */
    .long   VC0_IRQHandler            /* 26 - VC0 Interrupt */
    .long   VC1_IRQHandler            /* 27 - VC1 Interrupt */
    .long   LVD_IRQHandler            /* 28 - LVD Interrupt */
    .long   0                         /* 29 - Reserved */
    .long   FLASH_IRQHandler          /* 30 - FLASH Interrupt */
    .long   CTRIM_CLKDET_IRQHandler   /* 31 - CTRIM/CLKDET Interrupt */

/*******************************************************************************
* Reset Handler
*******************************************************************************/
.section .text
.thumb_func
.globl Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    /* Reset NVIC vector table if in ROM debug */
    ldr     r0, =0x20000000
    ldr     r2, =0x0
    movs    r1, #0
    add     r1, pc, #0
    cmp     r1, r0
    bls     RAMCODE

    /* RAM code base address */
    add     r2, r0, r2
RAMCODE:
    /* Reset Vector table address */
    ldr     r0, =0xE000ED08
    str     r2, [r0]

    /* Call SystemInit */
    ldr     r0, =SystemInit
    blx     r0

    /* Call __main (C runtime initialization) */
    ldr     r0, =__main
    bx      r0
.size Reset_Handler, .-Reset_Handler

/*******************************************************************************
* Default Exception Handlers (weak aliases)
*******************************************************************************/
.thumb_func
.weak NMI_Handler
.type NMI_Handler, %function
NMI_Handler:
    b       .
.size NMI_Handler, .-NMI_Handler

.thumb_func
.weak HardFault_Handler
.type HardFault_Handler, %function
HardFault_Handler:
    b       .
.size HardFault_Handler, .-HardFault_Handler

.thumb_func
.weak SVC_Handler
.type SVC_Handler, %function
SVC_Handler:
    b       .
.size SVC_Handler, .-SVC_Handler

.thumb_func
.weak PendSV_Handler
.type PendSV_Handler, %function
PendSV_Handler:
    b       .
.size PendSV_Handler, .-PendSV_Handler

.thumb_func
.weak SysTick_Handler
.type SysTick_Handler, %function
SysTick_Handler:
    b       .
.size SysTick_Handler, .-SysTick_Handler

/*******************************************************************************
* Default Interrupt Handlers (weak aliases)
*******************************************************************************/
.thumb_func
.weak PORTA_IRQHandler
.type PORTA_IRQHandler, %function
PORTA_IRQHandler:
    b       .
.size PORTA_IRQHandler, .-PORTA_IRQHandler

.thumb_func
.weak PORTB_IRQHandler
.type PORTB_IRQHandler, %function
PORTB_IRQHandler:
    b       .
.size PORTB_IRQHandler, .-PORTB_IRQHandler

.thumb_func
.weak ATIM3_IRQHandler
.type ATIM3_IRQHandler, %function
ATIM3_IRQHandler:
    b       .
.size ATIM3_IRQHandler, .-ATIM3_IRQHandler

.thumb_func
.weak LPUART0_IRQHandler
.type LPUART0_IRQHandler, %function
LPUART0_IRQHandler:
    b       .
.size LPUART0_IRQHandler, .-LPUART0_IRQHandler

.thumb_func
.weak LPUART1_IRQHandler
.type LPUART1_IRQHandler, %function
LPUART1_IRQHandler:
    b       .
.size LPUART1_IRQHandler, .-LPUART1_IRQHandler

.thumb_func
.weak SPI_IRQHandler
.type SPI_IRQHandler, %function
SPI_IRQHandler:
    b       .
.size SPI_IRQHandler, .-SPI_IRQHandler

.thumb_func
.weak CTIM0_IRQHandler
.type CTIM0_IRQHandler, %function
CTIM0_IRQHandler:
    b       .
.size CTIM0_IRQHandler, .-CTIM0_IRQHandler

.thumb_func
.weak CTIM1_IRQHandler
.type CTIM1_IRQHandler, %function
CTIM1_IRQHandler:
    b       .
.size CTIM1_IRQHandler, .-CTIM1_IRQHandler

.thumb_func
.weak HSI2C_IRQHandler
.type HSI2C_IRQHandler, %function
HSI2C_IRQHandler:
    b       .
.size HSI2C_IRQHandler, .-HSI2C_IRQHandler

.thumb_func
.weak IWDT_IRQHandler
.type IWDT_IRQHandler, %function
IWDT_IRQHandler:
    b       .
.size IWDT_IRQHandler, .-IWDT_IRQHandler

.thumb_func
.weak RTC_IRQHandler
.type RTC_IRQHandler, %function
RTC_IRQHandler:
    b       .
.size RTC_IRQHandler, .-RTC_IRQHandler

.thumb_func
.weak ADC_IRQHandler
.type ADC_IRQHandler, %function
ADC_IRQHandler:
    b       .
.size ADC_IRQHandler, .-ADC_IRQHandler

.thumb_func
.weak VC0_IRQHandler
.type VC0_IRQHandler, %function
VC0_IRQHandler:
    b       .
.size VC0_IRQHandler, .-VC0_IRQHandler

.thumb_func
.weak VC1_IRQHandler
.type VC1_IRQHandler, %function
VC1_IRQHandler:
    b       .
.size VC1_IRQHandler, .-VC1_IRQHandler

.thumb_func
.weak LVD_IRQHandler
.type LVD_IRQHandler, %function
LVD_IRQHandler:
    b       .
.size LVD_IRQHandler, .-LVD_IRQHandler

.thumb_func
.weak FLASH_IRQHandler
.type FLASH_IRQHandler, %function
FLASH_IRQHandler:
    b       .
.size FLASH_IRQHandler, .-FLASH_IRQHandler

.thumb_func
.weak CTRIM_CLKDET_IRQHandler
.type CTRIM_CLKDET_IRQHandler, %function
CTRIM_CLKDET_IRQHandler:
    b       .
.size CTRIM_CLKDET_IRQHandler, .-CTRIM_CLKDET_IRQHandler

.end
