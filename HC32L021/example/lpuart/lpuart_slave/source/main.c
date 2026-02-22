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
#include "hc32l021_gpio.h"
#include "hc32l021_lpuart.h"
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define BAUD_RATE       (9600u) /* 波特率 */
#define SLAVE_ADDR      (0xC1u) /* 从机地址 */
#define RX_TX_FRAME_LEN (10u)   /* 通信帧长度 */
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
static uint8_t au8SlaveRxData[RX_TX_FRAME_LEN] = { 0u };
static uint16_t u16AddrData                    = 0u;
static uint8_t u8RxCount                       = 0u;
static uint8_t u8TxCount                       = 0u;
static uint8_t u8AddrFlag                      = 0u;
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    GpioConfig(); /* 端口初始化 */

    LpuartConfig(); /* 串口模块功能配置 */

    while (1) {
        ;
    }
}

/**
 * @brief  UART1中断处理函数
 * @retval None
 */
void LpUart1_IRQHandler(void)
{
    /* 接收出错处理（PE/FE） */
    if (LPUART_IntFlagGet(LPUART1, LPUART_FLAG_FE)) {
        LPUART_IntFlagClear(LPUART1, LPUART_FLAG_FE); /* 清帧错误请求 */
        /* 添加出错处理 */
    }
    if (LPUART_IntFlagGet(LPUART1, LPUART_FLAG_PE)) {
        LPUART_IntFlagClear(LPUART1, LPUART_FLAG_PE); /* 清奇偶检验错误请求 */
        /* 添加出错处理 */
    }

    /*从机数据接收*/
    if (LPUART_IntFlagGet(LPUART1, LPUART_FLAG_RC)) /* 数据接收 */
    {
        if (0 == u8AddrFlag) /* 如果接收到地址帧 */
        {
            u8AddrFlag  = 1;
            u16AddrData = LPUART1->SBUF; /* 首字节为地址字节 */
            if (u16AddrData & 0x100u)    /* 再次确认是否地址帧 */
            {
                LPUART_FuncDisable(LPUART1, LPUART_FUN_ADRDET); /* 多机地址识别位清零，进入数据帧接收状态
                                                                 */
            }
        } else /* 如果接收到数据帧 */
        {
            au8SlaveRxData[u8RxCount++] =
                LPUART_DataReceive(LPUART1); /* 接收数据字节 */
        }
        if (u8RxCount >= RX_TX_FRAME_LEN) {
            LPUART_IntDisable(LPUART1, LPUART_INT_RC); /* 禁止接收中断 */
            LPUART_IntEnable(LPUART1, LPUART_INT_TC);  /* 使能发送中断 */

            while (FALSE == LPUART_IntFlagGet(LPUART1, LPUART_FLAG_TXE)) {
                ;
            }
            LPUART_DataTransmit(
                LPUART1, au8SlaveRxData[u8TxCount++]); /* 发送数据 */
            u8RxCount = 0u;
        }
        LPUART_IntFlagClear(LPUART1, LPUART_FLAG_RC); /* 清中断状态位 */
    }

    /*从机数据发送*/
    if (LPUART_IntFlagGet(LPUART1, LPUART_FLAG_TC)) {
        if (u8TxCount >= RX_TX_FRAME_LEN) {
            LPUART_IntDisable(LPUART1, LPUART_INT_TC); /* 禁用UART1发送中断 */
            LPUART_IntEnable(LPUART1, LPUART_INT_RC); /* 使能UART1接收中断 */
            u8TxCount = 0u;
        } else {
            while (FALSE == LPUART_IntFlagGet(LPUART1, LPUART_FLAG_TXE)) {
                ;
            }
            LPUART_DataTransmit(
                LPUART1, au8SlaveRxData[u8TxCount++]); /* 发送数据 */
        }
        LPUART_IntFlagClear(LPUART1, LPUART_FLAG_TC); /* 清中断状态位 */
    }
}

/**
 * @brief  端口初始化
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

    /* 配置PA02为LPUART1_RX */
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Mode   = GPIO_MD_INPUT;
    stcGpioInit.u32PullUp = GPIO_PULL_UP;
    stcGpioInit.u32Pin    = GPIO_PIN_02;
    GPIO_Init(GPIOA, &stcGpioInit);
    GPIO_PA02_AF_LPUART1_RXD();
}

/**
 * @brief  串口模块功能配置
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
    stcLpuartInit.u32Parity   = LPUART_B8_MULTI_DATA_OR_ADDR; /* 多机模式 */
    stcLpuartInit.u32StopBits = LPUART_STOPBITS_1;            /* 1停止位 */
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

    LPUART_MultiModeConfig(LPUART1, SLAVE_ADDR, 0xFFu); /* 从机地址配置 */

    LPUART_IntFlagClearAll(LPUART1);          /* 清除所有状态标志 */
    LPUART_IntEnable(LPUART1, LPUART_INT_RC); /* 使能串口接收中断 */
    EnableNvic(LPUART1_IRQn, IrqPriorityLevel3, TRUE); /* 系统中断使能 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
