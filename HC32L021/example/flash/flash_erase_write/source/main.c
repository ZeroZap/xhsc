/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of FLASH
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-11-14       MADS            First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2024, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "flash.h"
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define TEST_DATA_LEN (0x80u)
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static uint8_t  u8TestData[TEST_DATA_LEN]  = {0};
static uint16_t u16TestData[TEST_DATA_LEN] = {0};
static uint32_t u32TestData[TEST_DATA_LEN] = {0};
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    uint32_t u32Addr = 0xFE00u; /* 扇区127 首地址 */

    /* 写入测试数据 */
    uint8_t u8TestDataIndex = 0u;
    for (u8TestDataIndex = 0; u8TestDataIndex < TEST_DATA_LEN; u8TestDataIndex++)
    {
        u8TestData[u8TestDataIndex]  = u8TestDataIndex;
        u16TestData[u8TestDataIndex] = 0x1010u + u8TestDataIndex;
        u32TestData[u8TestDataIndex] = 0x20000020u + u8TestDataIndex;
    }

    FLASH_ReadOnlyDisable(); /* FLASH可读、可编程、可擦写 */

    /* 对需要擦除的扇区解锁，bit31设1，扇区124~127擦写使能 */
    if (Ok != FLASH_LockUnlockSectors(0x80000000u))
    {
        while (1)
        {
            ; /* 解锁失败 */
        }
    }

    /* 8位写编程样例 */
    if (Ok != FLASH_SectorErase(u32Addr)) /* FLASH目标扇区擦除 */
    {
        while (1)
        {
            ; /* 擦除失败 */
        }
    }
    if (Ok != FLASH_WriteByte(u32Addr, u8TestData, TEST_DATA_LEN)) /* FLASH字节（8位方式）写 */
    {
        while (1)
        {
            ; /* 编程失败 */
        }
    }

    /* 16位写编程样例 */
    if (Ok != FLASH_SectorErase(u32Addr)) /* FLASH目标扇区擦除 */
    {
        while (1)
        {
            ; /* 擦除失败 */
        }
    }
    if (Ok != FLASH_WriteHalfWord(u32Addr, u16TestData, TEST_DATA_LEN)) /* FLASH半字（16位方式）写 */
    {
        while (1)
        {
            ; /* 编程失败 */
        }
    }

    /* 32位写编程样例 */
    if (Ok != FLASH_SectorErase(u32Addr)) /* FLASH目标扇区擦除 */
    {
        while (1)
        {
            ; /* 擦除失败 */
        }
    }
    if (Ok != FLASH_WriteWord(u32Addr, u32TestData, TEST_DATA_LEN)) /* FLASH字（32位方式）写 */
    {
        while (1)
        {
            ; /* 编程失败 */
        }
    }

    /* 对应上述解锁的扇区，重新对该扇区加锁 */
    if (Ok != FLASH_LockAllSector())
    {
        while (1)
        {
            ; /* 加锁失败 */
        }
    };

    /* 设置读模式 */
    if (Ok != FLASH_OperateModeConfig(FLASH_MD_RD))
    {
        while (1)
        {
            ; /* 配置失败 */
        }
    }

    FLASH_ReadOnlyEnable(); /* FLASH只读、不可编程或擦写 */

    while (1)
    {
        ;
    }
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
