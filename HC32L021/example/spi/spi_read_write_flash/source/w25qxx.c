/**
 *******************************************************************************
 * @file  w25qxx.c
 * @brief This file contains all the functions prototypes of the w25qxx library
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-06       MADS            First version
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
#include "w25qxx.h"
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Initializes the W25QXX interface
 * @retval W25QXX memory status
 *         W25QXX_OK        成功
 *         W25QXX_ERROR     错误
 *         W25QXX_BUSY      状态忙
 *         W25QXX_TIMEOUT   超时
 */
uint8_t W25QXX_Init(void)
{
    W25QXX_Reset(); /* Reset W25Qxxx */

    return W25QXX_StatusGet();
}

/**
 * @brief  This function reset the W25QXX
 * @retval None
 */
static void W25QXX_Reset(void)
{
    uint16_t u16Cmd[2] = {RESET_ENABLE_CMD, RESET_MEMORY_CMD};

    W25QXX_Enable(); /* enable w25qxx */

    /* Send the reset command */
    SPI_TransmitReceive(SPI, u16Cmd, NULL, 2);  // transmit Cmd 0x66,0x99

    W25QXX_Disable(); /* disable w25qxx */
}

/**
 * @brief  Reads current status of the W25QXX
 * @retval W25QXX memory status
 *         W25QXX_OK        成功
 *         W25QXX_ERROR     错误
 *         W25QXX_BUSY      状态忙
 *         W25QXX_TIMEOUT   超时
 */
static uint8_t W25QXX_StatusGet(void)
{
    uint16_t u16Cmd[2] = {READ_STATUS_REG1_CMD, 0x0000u};  // 0x05
    uint16_t status;

    W25QXX_Enable(); /* enable w25qxx */

    /* Send the read status command */
    SPI_TransmitReceive(SPI, u16Cmd, NULL, 1);  // transmit Cmd 0x05

    /* Reception of the data */
    SPI_FlagClearALL(SPI);
    SPI_TransmitReceive(SPI, &u16Cmd[1], &status, 1);

    W25QXX_Disable(); /* disable w25qxx */

    /* Check the value of the register */
    if ((status & W25QXX_FSR_BUSY) != 0)
    {
        return W25QXX_BUSY;
    }
    else
    {
        return W25QXX_OK;
    }
}

/**
 * @brief  This function send a Write Enable and wait it is effective
 * @retval W25QXX memory status
 *         W25QXX_OK        成功
 *         W25QXX_ERROR     错误
 *         W25QXX_BUSY      状态忙
 *         W25QXX_TIMEOUT   超时
 */
uint8_t W25QXX_WriteEnable(void)
{
    uint16_t u16Cmd[]     = {WRITE_ENABLE_CMD};  // 0x06
    uint32_t u32DelayTick = 0;

    W25QXX_Enable(); /* enable w25qxx */

    /* Send the Write Enable command */
    SPI_TransmitReceive(SPI, u16Cmd, NULL, 1);  // transmit Cmd 0x06

    W25QXX_Disable(); /* disable w25qxx */

    /* Wait the end of Flash writing */
    while (W25QXX_StatusGet() == W25QXX_BUSY)
    {
        /* Check for the Timeout */
        if ((u32DelayTick++) > W25QXX_TIMEOUT_VALUE)
        {
            return W25QXX_TIMEOUT;
        }
    }
    return W25QXX_OK;
}

/**
 * @brief  This function send a Write Enable and wait it is effective
 * @retval W25QXX memory status
 *         W25QXX_OK        成功
 *         W25QXX_ERROR     错误
 *         W25QXX_BUSY      状态忙
 *         W25QXX_TIMEOUT   超时
 */
uint8_t W25QXX_WriteDisable(void)
{
    uint16_t u16Cmd[]     = {WRITE_DISABLE_CMD};  // 0x04
    uint32_t u32DelayTick = 0;

    W25QXX_Enable(); /* enable w25qxx */

    /* Send the Write Disable command */
    SPI_TransmitReceive(SPI, u16Cmd, NULL, 1);  // transmit Cmd 0x04

    W25QXX_Disable(); /* disable w25qxx */

    /* Wait the end of Flash writing */
    while (W25QXX_StatusGet() == W25QXX_BUSY)
    {
        /* Check for the Timeout */
        if ((u32DelayTick++) > W25QXX_TIMEOUT_VALUE)
        {
            return W25QXX_TIMEOUT;
        }
    }
    return W25QXX_OK;
}

/**
 * @brief  Read Manufacture/Device ID
 * @retval value address
 *         0XEF13,表示芯片型号为W25Q80
 *         0XEF14,表示芯片型号为W25Q16
 *         0XEF15,表示芯片型号为W25Q32
 *         0XEF16,表示芯片型号为W25Q64
 */
uint16_t W25QXX_ReadID(void)
{
    uint16_t u16Idt[4]   = {0xaa, 0x55, 0xaa, 0x55};
    uint16_t u16Cmd[4]   = {READ_ID_CMD, 0x00, 0x00, 0x00};
    uint16_t u16Rt_ID[4] = {0};

    W25QXX_Enable(); /* enable w25qxx */

    /* Send the read ID command */
    SPI_TransmitReceive(SPI, u16Cmd, NULL, 4);

    /* Reception of the data */
    SPI_TransmitReceive(SPI, u16Idt, u16Rt_ID, 2);

    W25QXX_Disable(); /* diable w25qxx */

    return (((0x00FFu & u16Rt_ID[0]) << 8) | (u16Rt_ID[1]));
}

/**
 * @brief  Reads an amount of data from the QSPI memory
 * @param  pu16Data Pointer to data to be read
 * @param  u32ReadAddr Read start address
 * @param  u32Size u32Size of data to read
 * @retval QSPI memory status
 */
uint8_t W25QXX_Read(uint16_t *pu16Data, uint32_t u32ReadAddr, uint32_t u32Size)
{
    uint16_t u16Cmd[4];

    /* Configure the command */
    u16Cmd[0] = READ_CMD;
    u16Cmd[1] = (uint16_t)(u32ReadAddr >> 16);
    u16Cmd[2] = (uint16_t)(u32ReadAddr >> 8);
    u16Cmd[3] = (uint16_t)(u32ReadAddr);

    W25QXX_Enable(); /* enable w25qxx */

    /* Send the read command */
    SPI_TransmitReceive(SPI, u16Cmd, NULL, 4);

    /* Reception of the data */
    SPI_FlagClearALL(SPI);

    if (Ok != SPI_TransmitReceive(SPI, NULL, pu16Data, u32Size))
    {
        return W25QXX_ERROR;
    }

    W25QXX_Disable(); /* disable w25qxx */

    return W25QXX_OK;
}

/**
 * @brief  Writes an amount of data to the QSPI memory
 * @param  pu16Data Pointer to data to be written
 * @param  u32WriteAddr Write start address
 * @param  u32Size u32Size of data to write,No more than 256byte.
 * @retval QSPI memory status
 */
uint8_t W25QXX_Write(uint16_t *pu16Data, uint32_t u32WriteAddr, uint32_t u32Size)
{
    uint16_t u16Cmd[4];
    uint32_t u32EndAddr, u32CurrentSize, u32CurrentAddr;
    uint32_t u32DelayTick = 0;

    /* Calculation of the size between the write address and the end of the page */
    u32CurrentAddr = 0;

    while (u32CurrentAddr <= u32WriteAddr)
    {
        u32CurrentAddr += W25QXX_PAGE_SIZE;
    }
    u32CurrentSize = u32CurrentAddr - u32WriteAddr;

    /* Check if the size of the data is less than the remaining place in the page */
    if (u32CurrentSize > u32Size)
    {
        u32CurrentSize = u32Size;
    }

    /* Initialize the adress variables */
    u32CurrentAddr = u32WriteAddr;
    u32EndAddr     = u32WriteAddr + u32Size;

    /* Perform the write page by page */
    do
    {
        /* Configure the command */
        u16Cmd[0] = PAGE_PROG_CMD;
        u16Cmd[1] = (uint16_t)(u32CurrentAddr >> 16);
        u16Cmd[2] = (uint16_t)(u32CurrentAddr >> 8);
        u16Cmd[3] = (uint16_t)(u32CurrentAddr);

        W25QXX_WriteEnable(); /* Enable write operations */

        W25QXX_Enable();

        /* Send the command */
        SPI_TransmitReceive(SPI, u16Cmd, NULL, 4);

        SPI_TransmitReceive(SPI, pu16Data, NULL, u32CurrentSize);

        W25QXX_Disable();

        /* Wait the end of Flash writing */
        while (W25QXX_StatusGet() == W25QXX_BUSY)
        {
            /* Check for the Timeout */
            if ((u32DelayTick++) > W25QXX_TIMEOUT_VALUE)
            {
                return W25QXX_TIMEOUT;
            }
        }
        /* Update the address and size variables for next page programming */
        u32CurrentAddr += u32CurrentSize;
        pu16Data       += u32CurrentSize;
        u32CurrentSize  = ((u32CurrentAddr + W25QXX_PAGE_SIZE) > u32EndAddr) ? (u32EndAddr - u32CurrentAddr) : W25QXX_PAGE_SIZE;
    } while (u32CurrentAddr < u32EndAddr);

    return W25QXX_OK;
}

/**
 * @brief  Erases the specified block of the QSPI memory
 * @param  u32Addr Block address to erase
 * @retval QSPI memory status
 */
uint8_t W25QXX_BlockErase(uint32_t u32Addr)
{
    uint16_t u16Cmd[4];
    uint32_t u32DelayTick = 0;

    u16Cmd[0] = SECTOR_ERASE_CMD;  // 0X20
    u16Cmd[1] = (uint16_t)(u32Addr >> 16);
    u16Cmd[2] = (uint16_t)(u32Addr >> 8);
    u16Cmd[3] = (uint16_t)(u32Addr);

    W25QXX_WriteEnable(); /* Enable write operations */

    W25QXX_Enable();

    /* Send the Block Erase command */
    SPI_TransmitReceive(SPI, u16Cmd, NULL, 4);

    /*Deselect the FLASH: Chip Select high */
    W25QXX_Disable();
    DDL_Delay1ms(100);
    /* Wait the end of Flash writing */
    while (W25QXX_StatusGet() == W25QXX_BUSY)
    {
        /* Check for the Timeout */
        if ((u32DelayTick++) > W25QXX_SECTOR_ERASE_MAX_TIME)
        {
            return W25QXX_TIMEOUT;
        }
    }
    W25QXX_WriteDisable();
    return W25QXX_OK;
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
