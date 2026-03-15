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
 * @file   hc32l19x.ld
 *
 * @brief  Linker script for HC32L19x series MCU
 *
 * @author MADS Team
 *
 ******************************************************************************/

/*
 * HC32L19x Memory Configuration
 * -----------------------------
 * Flash: 256KB (0x00000000 - 0x0003FFFF)
 * SRAM:  16KB  (0x20000000 - 0x20003FFF)
 *
 * Note: Vector table is relocated to SRAM for faster interrupt response
 */

/* Entry Point */
ENTRY(Reset_Handler)

/* Memory Definitions */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 256K
    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 16K
}

/* Stack and Heap Sizes */
_Min_Stack_Size = 0x200;      /* 512 bytes */
_Min_Heap_Size  = 0x200;      /* 512 bytes */

/* Top of RAM (for stack initialization) */
_estack = ORIGIN(SRAM) + LENGTH(SRAM);

/* Section Definitions */
SECTIONS
{
    /* Vector table (relocated to SRAM at runtime) */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))     /* Startup code */
        . = ALIGN(4);
    } >SRAM AT> FLASH

    /* Program code and other data into FLASH */
    .text :
    {
        . = ALIGN(4);
        *(.text)                 /* .text sections (code) */
        *(.text*)                /* .text* sections (code) */
        *(.glue_7)               /* glue arm to thumb code */
        *(.glue_7t)              /* glue thumb to arm code */
        *(.eh_frame)
        *(.rodata)               /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)              /* .rodata* sections */

        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
        _etext = .;              /* Global variable at end of code */
        _sidata = _etext;        /* Used by startup to initialize data */
    } >FLASH

    /* ARM exception handling sections */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } >FLASH

    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >FLASH

    .preinit_array :
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } >FLASH

    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
    } >FLASH

    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } >FLASH

    /* Used by startup to initialize data */
    _sdata = .;

    /* Initialized data sections */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;              /* Create a global symbol at data start */
        *(.data)                 /* .data sections */
        *(.data*)                /* .data* sections */

        . = ALIGN(4);
        _edata = .;              /* Define a global symbol at data end */
    } >SRAM AT> FLASH

    /* Uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;               /* Define a global symbol at bss start */
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;               /* Define a global symbol at bss end */
        __bss_end__ = _ebss;
    } >SRAM

    /* User heap and stack */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE(end = .);
        PROVIDE(_end = .);
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } >SRAM

    /* Remove information from standard libraries */
    /DISCARD/ :
    {
        libc.a(*)
        libm.a(*)
        libgcc.a(*)
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
