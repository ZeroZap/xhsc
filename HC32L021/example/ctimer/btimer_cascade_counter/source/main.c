/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of the BTIM
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
#include "btim.h"
#include "gpio.h"
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
static void Btim0Config(void);
static void Btim1Config(void);
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    STK_LedConfig(); /* LED初始化 */

    Btim0Config(); /* BTIM0初始化 */

    Btim1Config(); /* BTIM1初始化 */

    BTIM_Enable(BTIM1); /* 启动BTIM1运行 */

    BTIM_Enable(BTIM0); /* 启动BTIM0运行 */

    while (1)
    {
        ;
    }
}

/**
 * @brief  BTIM1中断服务函数
 * @retval None
 */
void Ctim0_IRQHandler(void)
{
    static boolean_t bFlag = FALSE;

    if (TRUE == BTIM_IntFlagGet(BTIM1, BTIM_FLAG_UI)) /* 获取的溢出中断标志位 */
    {
        if (TRUE == bFlag)
        {
            STK_LED_ON(); /* LED 开启*/

            bFlag = FALSE;
        }
        else
        {
            STK_LED_OFF(); /* LED 关闭 */

            bFlag = TRUE;
        }
        BTIM_IntFlagClear(BTIM1, BTIM_FLAG_UI); /*清除BTIM1的溢出中断标志位  */
    }
}

/**
 * @brief  初始化BTIM0
 * @retval None
 */
static void Btim0Config(void)
{
    stc_btim_init_t stcBtimInit = {0};

    SYSCTRL_FuncEnable(SYSCTRL_FUNC_CTIMER0_USE_BTIM); /* 配置BTIM0/1/2有效，GTIM0无效 */
    SYSCTRL_PeriphClockEnable(PeriphClockCtim0);       /* 开启ctim0时钟 */

    BTIM_StcInit(&stcBtimInit);                               /* 结构体变量初始值初始化 */
    stcBtimInit.u32Mode            = BTIM_MD_PCLK;            /* 工作模式:定时器模式，计数时钟源来自PCLK */
    stcBtimInit.u32OneShotEn       = BTIM_CONTINUOUS_COUNTER; /* 连续计数模式 */
    stcBtimInit.u32Prescaler       = BTIM_COUNTER_CLK_DIV1;   /* 对计数时钟进行预除频 */
    stcBtimInit.u32ToggleEn        = BTIM_TOGGLE_DISABLE;     /* TOG输出禁止 */
    stcBtimInit.u32AutoReloadValue = 0xFFFFu;                 /* 自动重载寄存ARR赋值,计数周期为PRS*(ARR+1)*TPCLK */
    BTIM_Init(BTIM0, &stcBtimInit);
}

/**
 * @brief  初始化BTIM1
 * @retval None
 */
static void Btim1Config(void)
{
    stc_btim_init_t stcBtimInit = {0};

    SYSCTRL_FuncEnable(SYSCTRL_FUNC_CTIMER0_USE_BTIM); /* 配置BTIM0/1/2有效，GTIM0无效 */
    SYSCTRL_PeriphClockEnable(PeriphClockCtim0);       /* 开启ctim0时钟 */

    BTIM_StcInit(&stcBtimInit);                                    /* 结构体变量初始值初始化 */
    stcBtimInit.u32Mode                = BTIM_MD_EXTERN;           /* 工作模式: 计数器模式，计数时钟源来自CR.TRS选择 */
    stcBtimInit.u32OneShotEn           = BTIM_CONTINUOUS_COUNTER;  /* 连续计数模式 */
    stcBtimInit.u32Prescaler           = BTIM_COUNTER_CLK_DIV1;    /* 对计数时钟进行预除频 */
    stcBtimInit.u32ToggleEn            = BTIM_TOGGLE_DISABLE;      /* TOG输出禁止 */
    stcBtimInit.u32TriggerSrc          = BTIM_TRIG_SRC_ITR;        /* 触发源选择，上一级BTIM的ITR输出(溢出) */
    stcBtimInit.u32ExternInputPolarity = BTIM_ETR_POLARITY_NORMAL; /* 外部输入极性选择，极性不翻转 */
    stcBtimInit.u32AutoReloadValue     = 20 - 1;                   /* 自动重载寄存ARR赋值,计数周期为PRS*(ARR+1)*TPCLK */
    BTIM_Init(BTIM1, &stcBtimInit);

    BTIM_IntFlagClear(BTIM1, BTIM_FLAG_UI);          /* 清除溢出中断标志位 */
    BTIM_IntEnable(BTIM1, BTIM_INT_UI);              /* 允许BTIM1溢出中断 */
    EnableNvic(CTIM0_IRQn, IrqPriorityLevel3, TRUE); /* 使能中断 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
