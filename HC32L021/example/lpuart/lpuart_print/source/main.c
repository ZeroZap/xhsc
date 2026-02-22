/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of LPUART
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-02       MADS            First version
   2025-07-08       MADS            Modification
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2024~2025, Xiaohua Semiconductor Co., Ltd. All rights reserved.
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
#include "hc32l021_ddl.h"
#include "hc32l021_gpio.h"
#include "hc32l021_lpm.h"
#include "hc32l021_lpuart.h"
#include "hc32l021_sysctrl.h"
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define BAUD_RATE (9600u) /* 波特率 */

#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void GpioConfig(void);
static void LpuartConfig(void);
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    /* 端口配置 */
    GpioConfig();

    /* LPUART配置 */
    LpuartConfig();

    printf("@ LPUART Printf Example:\r\n");
    printf("  ==>> HC32L021 STK \r\n");
    printf("  ==>> www.xhsc.com.cn\r\n\r\n");

    while (1) {
        ;
    }
}

/**
 * @brief  LPUART配置
 * @retval None
 */
static void LpuartConfig(void)
{
    uint32_t u32CalBaudRate         = 0u;
    stc_lpuart_init_t stcLpuartInit = { 0 };

    /* 外设模块时钟使能 */
    SYSCTRL_PeriphClockEnable(PeriphClockLpuart1);

    /* LPUART 初始化 */
    LPUART_StcInit(&stcLpuartInit);                 /* 结构体初始化 */
    stcLpuartInit.u32TransMode = LPUART_MODE_TX_RX; /* 收发模式 */
    stcLpuartInit.u32FrameLength =
        LPUART_FRAME_LEN_8B_PAR; /* 数据8位，奇偶校验1位 */
    stcLpuartInit.u32Parity   = LPUART_B8_PARITY_EVEN; /* 偶校验 */
    stcLpuartInit.u32StopBits = LPUART_STOPBITS_1;     /* 1停止位 */
    stcLpuartInit.u32BaudRateGenSelect =
        LPUART_BAUD_NORMAL; /* 波特率生成选择：用OVER和SCNT产生波特率 */
    stcLpuartInit.stcBaudRate.u32SclkSelect =
        LPUART_SCLK_SEL_PCLK; /* 传输时钟源 */
    stcLpuartInit.stcBaudRate.u32Sclk = SYSCTRL_HclkFreqGet(); /* HCLK获取 */
    stcLpuartInit.stcBaudRate.u32Baud = BAUD_RATE;             /* 波特率 */
    u32CalBaudRate                    = LPUART_Init(LPUART1, &stcLpuartInit);

    if (0u != u32CalBaudRate) {
        /* 有效设置，可通过查看u32CalBaudRate的值确认当前计算的波特率 */
    }

    LPUART_IntFlagClearAll(LPUART1); /* 清除所有状态标志 */
}

/**
 * @brief  端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = { 0 };

    /* 外设模块时钟使能 */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    /* 配置PA01为LPUART1_TX */
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Mode      = GPIO_MD_OUTPUT_PP;
    stcGpioInit.bOutputValue = TRUE;
    stcGpioInit.u32Pin       = GPIO_PIN_01;
    GPIO_Init(GPIOA, &stcGpioInit);
    GPIO_PA01_AF_LPUART1_TXD();
}

PUTCHAR_PROTOTYPE
{
    LPUART_TransmitPoll(LPUART1, (uint8_t *)(&ch), 1);
    return ch;
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
