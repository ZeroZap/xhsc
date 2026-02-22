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
#define TX_FRAME_LEN (2u) /* 发送帧长度 */
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void LpuartLowPowerConfig(void);
static void GpioConfig(void);
static void LpuartConfig(void);
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static uint8_t u8RxData = 0u;
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    /* LED 端口初始化 */
    STK_LedConfig();

    /* STK 按键配置 */
    STK_UserKeyConfig();

    /* LPUART IO口相关配置 */
    GpioConfig();

    /* LPUART 功能配置 */
    LpuartConfig();

    /* =============================================== */
    /* ==============       WARNING       ============ */
    /* =============================================== */
    /* 本样例程序会进入深度休眠模式，因此以下代码起防护作用*/
    /* （防止进入深度休眠后芯片调试功能不能再次使用） */
    /* 在使用本样例时，禁止在没有唤醒机制的情况下删除以下代码 */
    while (!STK_USER_KEY_PRESSED()) {
        ;
    }

    /* 深度休眠端口配置 */
    LpuartLowPowerConfig();

    /* 进入深度休眠模式 */
    LPM_GotoDeepSleep(TRUE);

    while (1) {
        ;
    }
}

/**
 * @brief  LPUART1 中断服务函数
 * @retval None
 */
void LpUart1_IRQHandler(void)
{
    if (LPUART_IntFlagGet(LPUART1, LPUART_FLAG_FE)) {
        LPUART_IntFlagClear(LPUART1, LPUART_FLAG_FE); /* 清帧错误请求 */
    }
    if (LPUART_IntFlagGet(LPUART1, LPUART_FLAG_PE)) {
        LPUART_IntFlagClear(LPUART1, LPUART_FLAG_PE); /* 清奇偶检验错误请求 */
    }

    if (LPUART_IntFlagGet(LPUART1, LPUART_FLAG_TC)) {
        LPUART_IntFlagClear(LPUART1, LPUART_FLAG_TC); /* 清发送中断请求 */

        LPUART_IntDisable(LPUART1, LPUART_INT_TC); /* 关闭发送中断 */
        LPUART_IntEnable(LPUART1, LPUART_INT_RC);  /* 使能接收中断 */
    }

    if (LPUART_IntFlagGet(LPUART1, LPUART_FLAG_RC)) /* 接收数据 */
    {
        LPUART_IntFlagClear(LPUART1, LPUART_FLAG_RC); /* 清接收中断请求 */
        u8RxData = LPUART_DataReceive(LPUART1);       /* 接收数据 */

        LPUART_IntDisable(LPUART1, LPUART_INT_RC); /* 关闭接收中断 */
        LPUART_IntEnable(LPUART1, LPUART_INT_TC);  /* 使能发送中断 */
        while (FALSE == LPUART_IntFlagGet(LPUART1, LPUART_FLAG_TXE)) {
            ;
        }
        LPUART_DataTransmit(LPUART1, u8RxData); /* 将接收的数据发出*/
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

    /* 打开XTL */
    SYSCTRL_ClockSrcEnable(SYSCTRL_CLK_SRC_XTL);

    /* LPUART 初始化 */
    LPUART_StcInit(&stcLpuartInit);                 /* 结构体初始化 */
    stcLpuartInit.u32TransMode = LPUART_MODE_TX_RX; /* 收发模式 */
    stcLpuartInit.u32FrameLength =
        LPUART_FRAME_LEN_8B_PAR; /* 数据8位，奇偶校验1位 */
    stcLpuartInit.u32Parity   = LPUART_B8_PARITY_EVEN; /* 偶校验 */
    stcLpuartInit.u32StopBits = LPUART_STOPBITS_1;     /* 1停止位 */
    stcLpuartInit.u32BaudRateGenSelect =
        LPUART_XTL_BAUD_9600; /* 波特率生成选择：XTL生成9600bps */
    u32CalBaudRate = LPUART_Init(LPUART1, &stcLpuartInit);

    if (0u != u32CalBaudRate) {
        /* 有效设置，可通过查看u32CalBaudRate的值确认当前计算的波特率 */
    }

    LPUART_IntFlagClearAll(LPUART1);          /* 清除所有状态标志 */
    LPUART_IntEnable(LPUART1, LPUART_INT_RC); /* 使能接收中断 */
    EnableNvic(LPUART1_IRQn, IrqPriorityLevel3, TRUE); /* 系统中断使能 */
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

    /* 配置PA08为LPUART1_TX */
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Mode      = GPIO_MD_OUTPUT_PP;
    stcGpioInit.bOutputValue = TRUE;
    stcGpioInit.u32Pin       = GPIO_PIN_08;
    GPIO_Init(GPIOA, &stcGpioInit);
    GPIO_PA08_AF_LPUART1_TXD();

    /* 配置PA09为LPUART1_RX */
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Mode   = GPIO_MD_INPUT;
    stcGpioInit.u32PullUp = GPIO_PULL_UP;
    stcGpioInit.u32Pin    = GPIO_PIN_09;
    GPIO_Init(GPIOA, &stcGpioInit);
    GPIO_PA09_AF_LPUART1_RXD();
}

/**
 * @brief  深度休眠模式外部端口配置
 * @retval None
 */
static void LpuartLowPowerConfig(void)
{
    SYSCTRL_FuncEnable(SYSCTRL_FUNC_SWD_USE_IO); /* SWD设置为GPIO */

    GPIOA->ADS = 0xECFE; /* 配置成模拟(除TX,RX,USER KEY,LED)*/
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
