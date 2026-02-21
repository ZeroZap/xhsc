/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of BTIM
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
#include "ddl.h"
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
static void GpioConfig(void);
static void Btim0Config(void);
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

    Btim0Config(); /* BTIM0初始化 */

    /* ETR 输入一次下降沿，启动btim0运行一次，溢出后停止 */

    while (1)
    {
        ;
    }
}

/**
 * @brief  BTIM0中断服务函数
 * @retval None
 */
void Ctim0_IRQHandler(void)
{
    if (TRUE == BTIM_IntFlagGet(BTIM0, BTIM_FLAG_TI)) /* 获取BTIM0的触发中断标志位 */
    {
        STK_LED_ON(); /* LED 开启 */

        BTIM_IntFlagClear(BTIM0, BTIM_FLAG_TI); /* 清除BTIM0的触发中断标志位 */
    }

    if (TRUE == BTIM_IntFlagGet(BTIM0, BTIM_FLAG_UI)) /* 获取BTIM0的溢出中断标志位 */
    {
        STK_LED_OFF(); /* LED 关闭 */

        BTIM_IntFlagClear(BTIM0, BTIM_FLAG_UI); /* 清除BTIM0的溢出中断标志位 */
    }
}

/**
 * @brief  端口配置初始化
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /*开启GPIO时钟*/

    /* PA05复用为 CIIM0_ETR配置 */
    GPIO_StcInit(&stcGpioInit);          /* 结构体变量初始值初始化 */
    stcGpioInit.u32Mode = GPIO_MD_INPUT; /* 端口方向配置 */
    stcGpioInit.u32Pin  = GPIO_PIN_05;   /* 端口引脚配置 */
    GPIOA_Init(&stcGpioInit);            /* GPIOA初始化 */
    GPIO_PA05_AF_CTIM0_ETR();            /* 复用 CIIM0_ETR 配置 */
}

/**
 * @brief  初始化BTIM0
 * @retval None
 */
static void Btim0Config(void)
{
    stc_btim_init_t stcBtimInit = {0};

    SYSCTRL_FuncEnable(SYSCTRL_FUNC_CTIMER0_USE_BTIM); /* 配置BTIM0/1/2有效，GTIM0无效*/
    SYSCTRL_PeriphClockEnable(PeriphClockCtim0);       /* 使能BTIM0/1/2 外设时钟 */

    BTIM_StcInit(&stcBtimInit);                                      /* 结构体变量初始值初始化 */
    stcBtimInit.u32Mode                = BTIM_MD_TRIG;               /* 工作模式: 触发模式 */
    stcBtimInit.u32OneShotEn           = BTIM_ONESHOT_COUNTER;       /* 单次计数模式 */
    stcBtimInit.u32Prescaler           = BTIM_COUNTER_CLK_DIV512;    /* 对计数时钟进行预除频 */
    stcBtimInit.u32ToggleEn            = BTIM_TOGGLE_DISABLE;        /* TOG输出禁止 */
    stcBtimInit.u32TriggerSrc          = BTIM_TRIG_SRC_ETR;          /* 触发源选择，使用外部ETR输入作为触发信号 */
    stcBtimInit.u32ExternInputPolarity = BTIM_ETR_POLARITY_INVERTED; /* 外部输入极性选择，极性不翻转 */
    stcBtimInit.u32AutoReloadValue     = 10000 - 1;                  /* 自动重载寄存ARR赋值,计数周期为PRS*(ARR+1)*TPCLK */
    BTIM_Init(BTIM0, &stcBtimInit);                                  /* BITM0初始化 */

    BTIM_IntFlagClear(BTIM0, BTIM_FLAG_UI | BTIM_FLAG_TI); /* 清除溢出中断标志位、触发中断标志位 */
    BTIM_IntEnable(BTIM0, BTIM_INT_UI | BTIM_INT_TI);      /* 允许BTIM0溢出中断、触发中断 */
    EnableNvic(CTIM0_IRQn, IrqPriorityLevel3, TRUE);       /* 开启中断 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
