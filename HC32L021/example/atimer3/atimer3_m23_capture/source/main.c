/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of ATIM3
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
#include "hc32l021_atim3.h"
#include "hc32l021_ddl.h"
#include "hc32l021_flash.h"
#include "hc32l021_gpio.h"
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
static void GpiotConfig(void);
static void Atimer3Config(void);
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
volatile uint32_t u32Atim3CaptureValue1;
volatile uint32_t u32Atim3CaptureValue;
volatile uint16_t u32Atim3UevCnt;
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    GpiotConfig(); /* 端口配置 */

    Atimer3Config(); /* Atimer3 配置 */

    ATIM3_Mode23_Run(); /* 运行 */

    while (1) {
        ;
    }
}

/**
 * @brief  ATimer3 中断函数
 * @retval None
 */
void Atim3_IRQHandler(void)
{
    if (ATIM3_IntFlagGet(ATIM3_FLAG_UI)) {
        u32Atim3UevCnt++;
        ATIM3_IntFlagClear(ATIM3_FLAG_UI);
    }

    static boolean_t bFlag = FALSE;
    static uint32_t u32Temp;

    if (ATIM3_IntFlagGet(ATIM3_FLAG_PWC_CA0)) {
        if (FALSE == bFlag) {
            u32Atim3CaptureValue1 = ATIM3_Mode23_ChannelCaptureValueGet(
                ATIM3_COMPARE_CAPTURE_CH0A); /* 第一次读取捕获值 */
            u32Atim3UevCnt = 0;
            bFlag          = TRUE;
        } else {
            u32Temp =
                ATIM3_Mode23_ChannelCaptureValueGet(ATIM3_COMPARE_CAPTURE_CH0A)
                - u32Atim3CaptureValue1;
            u32Atim3CaptureValue =
                u32Temp + u32Atim3UevCnt * 0xFFFF; /* 两次捕获之间的差值 */
            u32Atim3UevCnt = 0;
            bFlag          = FALSE;
        }

        ATIM3_IntFlagClear(ATIM3_FLAG_PWC_CA0); /* 清中断标志 */
    }
}

/**
 * @brief  ATIM3 CHx 端口配置
 * @retval None
 */
static void GpiotConfig(void)
{
    stc_gpio_init_t stcGpioInit = { 0 };

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* GPIOA外设时钟使能 */

    /* PA11(ATIM3_CH0A)端口初始化 */
    GPIO_StcInit(&stcGpioInit);             /* 结构体初始化 */
    stcGpioInit.u32Pin    = GPIO_PIN_11;    /* 端口引脚 */
    stcGpioInit.u32Mode   = GPIO_MD_INPUT;  /* 端口方向配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE; /* 端口上下拉配置 */
    GPIOA_Init(&stcGpioInit);               /* 端口初始化 */
    GPIO_PA11_AF_ATIM3_CH0A();              /* 端口复用配置 */
}

/**
 * @brief  ATIM3 配置
 * @retval None
 */
static void Atimer3Config(void)
{
    uint16_t u16ArrValue;
    uint16_t u16CntValue;
    stc_atim3_mode23_init_t stcAtim3BaseCfg       = { 0 };
    stc_atim3_m23_input_init_t stcAtim3ChxaCapCfg = { 0 };

    SYSCTRL_PeriphClockEnable(PeriphClockAtim3); /* ATIM3 外设时钟使能 */

    ATIM3_Mode23_StcInit(&stcAtim3BaseCfg); /* 结构体初始化清零 */
    stcAtim3BaseCfg.u32WorkMode =
        ATIM3_M23_M23CR_WORK_MODE_SAWTOOTH; /* 锯齿波模式 */
    stcAtim3BaseCfg.u32CountClockSelect =
        ATIM3_M23_M23CR_CT_PCLK; /* 定时器功能，计数时钟为内部PCLK */
    stcAtim3BaseCfg.u32PRS = ATIM3_M23_M23CR_ATIM3CLK_PRS64; /* PCLK/64 */
    stcAtim3BaseCfg.u32CountDir =
        ATIM3_M23_M23CR_DIR_UP_CNT; /* 向上计数，在三角波模式时只读 */
    ATIM3_Mode23_Init(&stcAtim3BaseCfg); /* ATIM3 的模式23功能初始化 */

    ATIM3_Mode23_InputStcInit(&stcAtim3ChxaCapCfg); /* 结构体初始化清零 */
    stcAtim3ChxaCapCfg.u32CHxCompareCap =
        ATIM3_M23_CRCHx_CSA_CSB_CAPTURE; /* CH0A通道设置为捕获模式 */
    stcAtim3ChxaCapCfg.u32CHxCaptureSelect =
        ATIM3_M23_CRCHx_CRxCFx_CAPTURE_EDGE_RISE_FALL; /* CH0A通道上升沿下降沿捕获都使能
                                                        */
    stcAtim3ChxaCapCfg.u32CHxInFlt =
        ATIM3_M23_FLTR_FLTxx_THREE_PCLK_DIV4_4; /* PCLK/4 4个连续有效 */
    stcAtim3ChxaCapCfg.u32CHxPolarity =
        ATIM3_M23_FLTR_CCPxx_NORMAL_IN_OUT; /* 正常输入输出 */

    ATIM3_Mode23_PortInputCHxAConfig(
        ATIM3_M23_OUTPUT_CHANNEL_CH0,
        &stcAtim3ChxaCapCfg); /* 端口输入初始化配置 */

    u16ArrValue = 0xFFFF;
    ATIM3_Mode23_ARRSet(u16ArrValue); /* 设置重载值,并使能缓存 */
    ATIM3_Mode23_ARRBufferEnable();

    u16CntValue = 0;
    ATIM3_Mode1_Cnt16Set(u16CntValue); /* 设置计数初值 */

    ATIM3_IntFlagClearAll();                         /* 清中断标志 */
    ATIM3_Mode23_IntEnable(ATIM3_M23_INT_CA0);       /* 使能ATIM3 中断 */
    ATIM3_Mode23_IntEnable(ATIM3_M23_INT_UI);        /* 使能ATIM3 中断 */
    EnableNvic(ATIM3_IRQn, IrqPriorityLevel3, TRUE); /* ATIM3 开中断 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
