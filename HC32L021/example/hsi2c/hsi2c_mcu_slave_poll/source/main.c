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
#define TRANS_TIMEOUT (0xFFFFFFFFu) /* 传输超时保护计数值 */
#define TRANS_SIZE    (30)          /* 传输字节数 */
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void SysClockConfig(void);
static void GpioConfig(void);
static void Hsi2cSlaveReadConfig(void);
static void Hsi2cSlaveWriteConfig(void);
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
        u8WriteData[u32Index] = u32Index + 0x80u;
        u8ReadData[u32Index]  = u32Index + 0x90u;
    }

    /* 端口配置 */
    GpioConfig();

    /* HSI2C配置：从机读 */
    Hsi2cSlaveReadConfig();
    /* 从机轮询传输 */
    HSI2C_SlaveTransferPoll(HSI2C, u8ReadData, sizeof(u8ReadData), TRANS_TIMEOUT);

    while (stcHsi2cCom.enComStatus == Hsi2cComBusy)
    {
        ;
    }

    /* HSI2C配置：从机写 */
    Hsi2cSlaveWriteConfig();
    /* 从机轮询传输 */
    HSI2C_SlaveTransferPoll(HSI2C, u8WriteData, sizeof(u8WriteData), TRANS_TIMEOUT);

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
 * @brief  HSI2C从机读配置
 * @retval None
 */
static void Hsi2cSlaveReadConfig(void)
{
    /* 结构体初始化 */
    HSI2C_SlaveStcInit(&stcHsi2cSlaveInit);

    /* 开启HSI2C外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockHsi2c);

    stcHsi2cSlaveInit.bResetBeforeInit = TRUE;  /* 初始化前复位 */
    stcHsi2cSlaveInit.u32SlaveAddr0    = 0x06u; /* 从机地址0 */
    stcHsi2cSlaveInit.u32SlaveAddr1    = 0x0Fu; /* 从机地址1 */
    stcHsi2cSlaveInit.u8SubAddrSize = 2; /* Sub address size：!=0时，接收的Subaddress存放在stcHsi2cCom.u32SubAddr中，否则存放在u8ReadData[]数组中 */
    stcHsi2cSlaveInit.enDir = Hsi2cMasterWriteSlaveRead;

    // stcHsi2cSlaveInit.stcSlaveConfig1.u32FuncSelect = HSI2C_SLAVE_RXSTALL_ENABLE;
    stcHsi2cSlaveInit.stcSlaveConfig1.u32ConfigAddrMatchMode = HSI2C_SLAVE_ADDR0_7B_OR_ADDR1_7B;

    stcHsi2cSlaveInit.stcSlaveConfig2.u32FilterEnable = HSI2C_SLAVE_FILTER_ON;

    HSI2C_SlaveInit(HSI2C, &stcHsi2cSlaveInit, SystemCoreClock);
}

/**
 * @brief  HSI2C从机写配置
 * @retval None
 */
static void Hsi2cSlaveWriteConfig(void)
{
    /* 结构体初始化 */
    HSI2C_SlaveStcInit(&stcHsi2cSlaveInit);

    /* 开启HSI2C外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockHsi2c);

    stcHsi2cSlaveInit.bResetBeforeInit = TRUE;  /* 初始化前复位 */
    stcHsi2cSlaveInit.u32SlaveAddr0    = 0x06u; /* 从机地址0 */
    stcHsi2cSlaveInit.u32SlaveAddr1    = 0x0Fu; /* 从机地址1 */
    stcHsi2cSlaveInit.u8SubAddrSize = 2; /* Sub address size：!=0时，接收的Subaddress存放在stcHsi2cCom.u32SubAddr中，否则存放在u8WriteData[]数组中 */
    stcHsi2cSlaveInit.enDir = Hsi2cMasterReadSlaveWrite;

    stcHsi2cSlaveInit.stcSlaveConfig1.u32FuncSelect          = HSI2C_SLAVE_TXDSTALL_ENABLE | HSI2C_SLAVE_ADRSTALL_ENABLE;
    stcHsi2cSlaveInit.stcSlaveConfig1.u32ConfigAddrMatchMode = HSI2C_SLAVE_ADDR0_7B_OR_ADDR1_7B;

    stcHsi2cSlaveInit.stcSlaveConfig2.u32FilterEnable = HSI2C_SLAVE_FILTER_ON;

    HSI2C_SlaveInit(HSI2C, &stcHsi2cSlaveInit, SystemCoreClock);
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
