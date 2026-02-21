/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of GTIM
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

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "gpio.h"
#include "gtim.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void GpioConfig(void);
static void Gtim0Config(void);
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    GpioConfig(); /* GPIO初始化 */

    Gtim0Config(); /* GTIM0初始化 */

    GTIM_Enable(GTIM0); /* 启动GTIM0运行 */

    while (1)
    {
        ;
    }
}

/**
 * @brief  端口配置初始化
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* 开启GPIO时钟 */

    /* CTIM0_CH0(PA02)端口初始化 */
    GPIO_StcInit(&stcGpioInit);                /* 结构体变量初始值初始化 */
    stcGpioInit.u32Mode   = GPIO_MD_OUTPUT_PP; /* 端口方向配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE;    /* 端口上拉配置 */
    stcGpioInit.u32Pin    = GPIO_PIN_02;       /* 端口引脚配置 */
    GPIOA_Init(&stcGpioInit);                  /* GPIO初始化 */
    GPIO_PA02_AF_CTIM0_CH0();                  /* CTIM0_CH0(PA02)端口复用功能 */
}

/**
 * @brief  初始化GTIMER
 * @retval None
 */
static void Gtim0Config(void)
{
    stc_gtim_init_t stcGtimInit = {0};

    SYSCTRL_FuncDisable(SYSCTRL_FUNC_CTIMER0_USE_BTIM); /* 配置GTIM0有效，BTIM0/1/2无效 */
    SYSCTRL_PeriphClockEnable(PeriphClockCtim0);        /* 使能CTIM0外设时钟 */

    GTIM_StcInit(&stcGtimInit);                               /* 结构体变量初始值初始化 */
    stcGtimInit.u32Mode            = GTIM_MD_PCLK;            /* 工作模式: 定时模式，计数时钟源来自PCLK */
    stcGtimInit.u32OneShotEn       = GTIM_CONTINUOUS_COUNTER; /* 连续计数模式 */
    stcGtimInit.u32Prescaler       = GTIM_COUNTER_CLK_DIV1;   /* 对计数时钟进行预除频 */
    stcGtimInit.u32ToggleEn        = GTIM_TOGGLE_DISABLE;     /* TOG输出禁止 */
    stcGtimInit.u32AutoReloadValue = 40000 - 1;               /* 自动重载寄存ARR赋值,计数周期为PRS*(ARR+1)*TPCLK */
    GTIM_Init(GTIM0, &stcGtimInit);                           /* GTIM0初始化 */

    GTIM_CompareCaptureModeSet(GTIM0, GTIM_COMPARE_CAPTURE_CH0, GTIM_COMPARE_CAPTURE_PWM_NORMAL); /* CHx PWM输出，CNT＞＝CCR输出高电平 */
    GTIM_CompareCaptureRegSet(GTIM0, GTIM_COMPARE_CAPTURE_CH0, 20000);                            /* 比较值 20000 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
