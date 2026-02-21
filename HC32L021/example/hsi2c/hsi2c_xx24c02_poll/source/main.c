/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of HSI2C
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-02       MADS            First version
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
#include "flash.h"
#include "gpio.h"
#include "hsi2c.h"
#include "sysctrl.h"
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define BAUD_RATE        (400000)           /* 波特率 */
#define DEVICE_ADDR      (0x50u)            /* 从机设备地址 */
#define DEVICE_PAGE_SIZE (8u)               /* 设备页字节长度 */
#define TRANS_SIZE       (DEVICE_PAGE_SIZE) /* 传输字节数 */

#define TRANS_TIMEOUT    (0xFFFFFFFFu) /* 传输超时保护计数值 */
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void SysClockConfig(void);
static void GpioConfig(void);
static void Hsi2cMasterReadConfig(void);
static void Hsi2cMasterWriteConfig(void);
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static uint8_t u8WriteData[TRANS_SIZE] = {0};
static uint8_t u8ReadData[TRANS_SIZE]  = {0};
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    uint32_t u32Index = 0;

    /* 系统时钟配置 */
    SysClockConfig();

    /* LED端口配置 */
    STK_LedConfig();

    /* 测试数据初始化 */
    for (u32Index = 0; u32Index < TRANS_SIZE; u32Index++)
    {
        u8WriteData[u32Index] = u32Index + 0x10u;
        u8ReadData[u32Index]  = u32Index + 0x20u;
    }

    /* 端口配置 */
    GpioConfig();

    /* HSI2C配置：主机写 */
    Hsi2cMasterWriteConfig();

    /* 主机轮询传输 */
    HSI2C_MasterTransferPoll(HSI2C, u8WriteData, sizeof(u8WriteData), TRANS_TIMEOUT);

    while (stcHsi2cCom.enComStatus == Hsi2cComBusy)
    {
        ;
    }

    /* 根据实际使用场景设置合适的延时时间 */
    DDL_Delay1ms(10);

    /* HSI2C配置：主机读 */
    Hsi2cMasterReadConfig();

    /* 主机轮询传输 */
    HSI2C_MasterTransferPoll(HSI2C, u8ReadData, sizeof(u8ReadData), TRANS_TIMEOUT);

    while (stcHsi2cCom.enComStatus == Hsi2cComBusy)
    {
        ;
    }

    STK_LED_ON();

    while (1)
    {
        ;
    }
}

/**
 * @brief  HSI2C主机写配置
 * @retval None
 * @note   时序：
 *         u8SubAddrSize = 0：
 *             Start + 7-bit Slave Address(W) + u8WriteData[TRANS_SIZE] + Stop
 *         u8SubAddrSize = 2：
 *             Start + 7-bit Slave Address(W) + Sub Address(2bytes) + u8WriteData[TRANS_SIZE] + Stop
 */
static void Hsi2cMasterWriteConfig(void)
{
    /* 结构体初始化 */
    HSI2C_MasterStcInit(&stcHsi2cMasterInit);

    /* 开启HSI2C外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockHsi2c);

    stcHsi2cMasterInit.bResetBeforeInit = TRUE; /* 初始化前复位 */
    stcHsi2cMasterInit.u32EnableDebug   = HSI2C_MASTER_DEBUG_OFF;
    stcHsi2cMasterInit.u32BaudRateHz    = BAUD_RATE;   /* 波特率 */
    stcHsi2cMasterInit.u8SlaveAddr      = DEVICE_ADDR; /* 从机地址 */
    stcHsi2cMasterInit.u32SubAddr       = 0x0000u;     /* Sub address */
    stcHsi2cMasterInit.u8SubAddrSize    = 2;           /* Sub address size */
    stcHsi2cMasterInit.enDir            = Hsi2cMasterWriteSlaveRead;

    stcHsi2cMasterInit.stcMasterConfig2.u32SdaFilterEnable = HSI2C_MASTER_FILTBPSDA_ENABLE;
    stcHsi2cMasterInit.stcMasterConfig2.u32SclFilterEnable = HSI2C_MASTER_FILTBPSCL_ENABLE;

    if (Ok != HSI2C_MasterInit(HSI2C, &stcHsi2cMasterInit, SystemCoreClock))
    {
        while (1)
        {
            ;
        }
    }
}

/**
 * @brief  HSI2C主机读配置
 * @retval None
 * @note   时序：
 *         u8SubAddrSize = 0：
 *             Start + 7-bit Slave Address(R) + u8ReadData[TRANS_SIZE] + Stop
 *         u8SubAddrSize = 2：
 *             Start + 7-bit Slave Address(W) + Sub Address(2bytes) + Restart + 7-bit Slave Address(R) + u8ReadData[TRANS_SIZE] + Stop
 */
static void Hsi2cMasterReadConfig(void)
{
    /* 结构体初始化 */
    HSI2C_MasterStcInit(&stcHsi2cMasterInit);

    /* 开启HSI2C外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockHsi2c);

    stcHsi2cMasterInit.bResetBeforeInit = TRUE; /* 初始化前复位 */
    stcHsi2cMasterInit.u32EnableDebug   = HSI2C_MASTER_DEBUG_OFF;
    stcHsi2cMasterInit.u32BaudRateHz    = BAUD_RATE;   /* 波特率 */
    stcHsi2cMasterInit.u8SlaveAddr      = DEVICE_ADDR; /* 从机地址 */
    stcHsi2cMasterInit.u32SubAddr       = 0x0000u;     /* Sub address */
    stcHsi2cMasterInit.u8SubAddrSize    = 2;           /* Sub address size */
    stcHsi2cMasterInit.enDir            = Hsi2cMasterReadSlaveWrite;

    stcHsi2cMasterInit.stcMasterConfig2.u32SdaFilterEnable = HSI2C_MASTER_FILTBPSDA_ENABLE;
    stcHsi2cMasterInit.stcMasterConfig2.u32SclFilterEnable = HSI2C_MASTER_FILTBPSCL_ENABLE;

    if (Ok != HSI2C_MasterInit(HSI2C, &stcHsi2cMasterInit, SystemCoreClock))
    {
        while (1)
        {
            ;
        }
    }
}

/**
 * @brief  系统时钟配置
 * @retval None
 */
static void SysClockConfig(void)
{
    stc_sysctrl_clock_init_t stcSysClockInit = {0};

    /* 结构体初始化 */
    SYSCTRL_ClockStcInit(&stcSysClockInit);

    stcSysClockInit.u32SysClockSrc = SYSCTRL_CLK_SRC_RC48M_48M; /* 选择系统默认RC48M 48MHz作为Hclk时钟源 */
    stcSysClockInit.u32HclkDiv     = SYSCTRL_HCLK_PRS_DIV1;     /* Hclk 1分频 */
    SYSCTRL_ClockInit(&stcSysClockInit);                        /* 系统时钟初始化 */
}

/**
 * @brief  端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    /* 结构体初始化 */
    GPIO_StcInit(&stcGpioInit);

    /* 开启GPIO外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    /* 配置PA10(SDA)、PA11(SCL)端口 */
    stcGpioInit.bOutputValue = TRUE;
    stcGpioInit.u32Pin       = GPIO_PIN_10 | GPIO_PIN_11;
    stcGpioInit.u32Mode      = GPIO_MD_OUTPUT_OD;
    GPIOA_Init(&stcGpioInit);
    /* 设置PA10(SDA)、PA11(SCL)功能 */
    GPIO_PA10_AF_HSI2C_SDA();
    GPIO_PA11_AF_HSI2C_SCL();
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
