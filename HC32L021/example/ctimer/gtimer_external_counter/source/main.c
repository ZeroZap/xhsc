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
    STK_LedConfig(); /* LED 初始化 */

    GpioConfig(); /* GPIO初始化 */

    Gtim0Config(); /* GTIM0初始化 */

    GTIM_Enable(GTIM0); /* 启动GTIM0运行 */

    while (1)
    {
        ;
    }
}

/**
 * @brief  GTIM0中断服务函数
 * @retval None
 */
void Ctim0_IRQHandler(void)
{
    static boolean_t bFlag = FALSE;

    if (TRUE == GTIM_IntFlagGet(GTIM0, GTIM_FLAG_UI))
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

        GTIM_IntFlagClear(GTIM0, GTIM_FLAG_UI); /* 清除GTIM0的溢出中断标志位    */
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

    /* CIIM1_ETR(PA05)端口初始化 */
    GPIO_StcInit(&stcGpioInit);             /* 结构体变量初始值初始化 */
    stcGpioInit.u32Mode   = GPIO_MD_INPUT;  /* 端口方向配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE; /* 端口上拉配置 */
    stcGpioInit.u32Pin    = GPIO_PIN_05;    /* 端口引脚配置 */
    GPIOA_Init(&stcGpioInit);               /* GPIO端口初始化 */
    GPIO_PA05_AF_CTIM0_ETR();               /* CIIM0_ETR端口复用功能 */
}

/**
 * @brief  初始化GTIMER
 * @retval None
 */
static void Gtim0Config(void)
{
    stc_gtim_init_t stcGtimInit = {0};

    SYSCTRL_FuncDisable(SYSCTRL_FUNC_CTIMER0_USE_BTIM); /* 配置GTIM0有效，BTIMER0/1/2无效 */
    SYSCTRL_PeriphClockEnable(PeriphClockCtim0);        /* 使能CTIM0外设时钟 */

    GTIM_StcInit(&stcGtimInit);                                    /* 结构体变量初始值初始化 */
    stcGtimInit.u32Mode                = GTIM_MD_EXTERN;           /* 工作模式: 计数器模式，计数时钟源来自CR.TRS选择 */
    stcGtimInit.u32OneShotEn           = GTIM_CONTINUOUS_COUNTER;  /* 连续计数模式 */
    stcGtimInit.u32Prescaler           = GTIM_COUNTER_CLK_DIV1;    /* 对计数时钟进行预除频 */
    stcGtimInit.u32ToggleEn            = GTIM_TOGGLE_DISABLE;      /* TOG输出禁止 */
    stcGtimInit.u32TriggerSrc          = GTIM_TRIG_SRC_ETR;        /* 触发源选择，使用外部ETR输入信号 */
    stcGtimInit.u32ExternInputPolarity = GTIM_ETR_POLARITY_NORMAL; /* 外部输入极性选择，极性不翻转 */
    stcGtimInit.u32AutoReloadValue     = 10 - 1;                   /* 自动重载寄存ARR赋值,计数周期为PRS*(ARR+1)*TPCLK */
    GTIM_Init(GTIM0, &stcGtimInit);                                /* GTIM0初始化 */

    GTIM_ExternInputFilterSet(GTIM0, GTIM_ETR_FLT_PCLK_DIV4_CYCLE4); /* 外部输入滤波配置 */

    GTIM_CompareCaptureAllDisable(GTIM0); /* 禁止所有通道比较捕获功能 */

    GTIM_IntFlagClear(GTIM0, GTIM_FLAG_UI);          /* 清除溢出中断标志位 */
    GTIM_IntEnable(GTIM0, GTIM_INT_UI);              /* 允许GTIM0溢出中断 */
    EnableNvic(CTIM0_IRQn, IrqPriorityLevel3, TRUE); /* 中断关闭 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
