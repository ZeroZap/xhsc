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
static void Gtim1Config(void);
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    STK_LedConfig(); /* LED 初始化 */

    GpioConfig(); /* GPIO初始化 */

    Gtim1Config(); /* GTIM1初始化 */

    GTIM_Enable(GTIM1); /* 启动GTIM1运行 */

    while (1)
    {
        ;
    }
}

/**
 * @brief  GTIM1中断服务函数
 * @retval None
 */
void Ctim1_IRQHandler(void)
{
    static boolean_t bFlag = FALSE;

    if (TRUE == GTIM_IntFlagGet(GTIM1, GTIM_FLAG_UI))
    {
        if (FALSE == bFlag)
        {
            STK_LED_ON(); /* 开启LED */
            bFlag = TRUE;
        }
        else
        {
            STK_LED_OFF(); /* 关闭LED */
            bFlag = FALSE;
        }

        GTIM_IntFlagClear(GTIM1, GTIM_FLAG_UI); /* 清除GTIM1的溢出中断标志位 */
    }
}

/**
 * @brief  端口配置初始化
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcTogGpioInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* 开启GPIO时钟 */

    /*  CTIM1_TOG(PA03)和CTIM1_TOGN(PA04)端口初始化 */
    GPIO_StcInit(&stcTogGpioInit);                        /* 结构体变量初始值初始化 */
    stcTogGpioInit.u32Mode   = GPIO_MD_OUTPUT_PP;         /* 端口方向配置 */
    stcTogGpioInit.u32PullUp = GPIO_PULL_NONE;            /* 端口上拉配置 */
    stcTogGpioInit.u32Pin    = GPIO_PIN_03 | GPIO_PIN_04; /* 端口引脚配置 */
    GPIOA_Init(&stcTogGpioInit);                          /* GPIO初始化 */
    GPIO_PA03_AF_CTIM1_TOG();                             /* CTIM1_TOG(PA03)端口复用功能 */
    GPIO_PA04_AF_CTIM1_TOGN();                            /* CTIM1_TOGN(PA04)端口复用功能 */
}

/**
 * @brief  初始化GTIM1
 * @retval None
 */
static void Gtim1Config(void)
{
    stc_gtim_init_t stcGtimInit = {0};

    SYSCTRL_FuncDisable(SYSCTRL_FUNC_CTIMER1_USE_BTIM); /* 配置GTIM1有效，BTIMER3/4/5无效 */
    SYSCTRL_PeriphClockEnable(PeriphClockCtim1);        /* 使能CTIM1外设时钟 */

    GTIM_StcInit(&stcGtimInit);                               /* 结构体变量初始值初始化 */
    stcGtimInit.u32Mode            = GTIM_MD_PCLK;            /* 工作模式: 定时器模式，计数时钟源来自PCLK */
    stcGtimInit.u32OneShotEn       = GTIM_CONTINUOUS_COUNTER; /* 连续计数模式 */
    stcGtimInit.u32Prescaler       = GTIM_COUNTER_CLK_DIV4;   /* 对计数时钟进行预除频 */
    stcGtimInit.u32ToggleEn        = GTIM_TOGGLE_ENABLE;      /* TOG输出使能，TOG和TOGN输出相位相反的信号 */
    stcGtimInit.u32AutoReloadValue = 4000 - 1;                /* 自动重载寄存ARR赋值,计数周期为PRS*(ARR+1)*TPCLK */
    GTIM_Init(GTIM1, &stcGtimInit);                           /* GTIM1初始化 */

    GTIM_CompareCaptureAllDisable(GTIM1); /* 禁止所有通道比较捕获功能 */

    GTIM_IntFlagClear(GTIM1, GTIM_FLAG_UI);          /* 清除溢出中断标志位 */
    GTIM_IntEnable(GTIM1, GTIM_INT_UI);              /* 允许GTIM1溢出中断   */
    EnableNvic(CTIM1_IRQn, IrqPriorityLevel3, TRUE); /* NVIC 中断使能 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
