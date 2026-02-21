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
static void Btim3Config(uint16_t u16Period);
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

    STK_UserKeyConfig(); /* KEY初始化 */

    GpioConfig(); /* GPIO初始化 */

    Btim3Config(4000u); /* BTIM3初始化*/

    BTIM_Enable(BTIM3); /* 启动BTIM3运行 */

    while (FALSE == STK_USER_KEY_PRESSED()) /* 等待USER KEY按下 */
    {
        ;
    }
    DDL_Delay1ms(200);

    BTIM_ToggleEnable(BTIM3); /* TOG输出使能 */

    while (1)
    {
        ;
    }
}

/**
 * @brief  BTIM3中断服务函数
 * @retval None
 */
void Ctim1_IRQHandler(void)
{
    static boolean_t bFlag = TRUE;

    if (TRUE == BTIM_IntFlagGet(BTIM3, BTIM_FLAG_UI)) /* 获取BTIM3的溢出中断标志位 */
    {
        if (TRUE == bFlag)
        {
            STK_LED_ON(); /* LED 开启 */

            bFlag = FALSE;
        }
        else
        {
            STK_LED_OFF(); /* LED 关闭*/

            bFlag = TRUE;
        }

        BTIM_IntFlagClear(BTIM3, BTIM_FLAG_UI); /* 清除BTIM3的溢出中断标志位 */
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

    /* CTIM1_TOG(PA03)和CTIM1_TOGN(PA04)端口初始化 */
    GPIO_StcInit(&stcGpioInit);                        /* 结构体变量初始值初始化 */
    stcGpioInit.u32Mode   = GPIO_MD_OUTPUT_PP;         /* 端口方向配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE;            /* 端口上拉配置 */
    stcGpioInit.u32Pin    = GPIO_PIN_03 | GPIO_PIN_04; /* 端口引脚配置 */
    GPIOA_Init(&stcGpioInit);                          /* GPIO 端口初始化 */
    GPIO_PA03_AF_CTIM1_TOG();                          /* CTIM1_TOG端口复用功能 */
    GPIO_PA04_AF_CTIM1_TOGN();                         /* CTIM1_TOGN端口复用功能 */
}

/**
 * @brief  初始化BTIM3
 * @retval None
 */
static void Btim3Config(uint16_t u16Period)
{
    stc_btim_init_t stcBtimInit = {0};

    SYSCTRL_FuncEnable(SYSCTRL_FUNC_CTIMER1_USE_BTIM); /* 配置BTIM3/4/5有效，GTIM1无效 */
    SYSCTRL_PeriphClockEnable(PeriphClockCtim1);       /* 使能BTIM3/4/5 外设时钟 */

    BTIM_StcInit(&stcBtimInit);                               /* 结构体变量初始值初始化 */
    stcBtimInit.u32Mode            = BTIM_MD_PCLK;            /* 工作模式: 计数器模式，计数时钟源来自PCLK */
    stcBtimInit.u32OneShotEn       = BTIM_CONTINUOUS_COUNTER; /* 连续计数模式 */
    stcBtimInit.u32Prescaler       = BTIM_COUNTER_CLK_DIV256; /* 对计数时钟进行预除频 */
    stcBtimInit.u32ToggleEn        = BTIM_TOGGLE_DISABLE;     /* TOG输出禁止 */
    stcBtimInit.u32AutoReloadValue = u16Period - 1;           /* 自动重载寄存ARR赋值,计数周期为PRS*(ARR+1)*TPCLK */
    BTIM_Init(BTIM3, &stcBtimInit);                           /* BTIM3初始化*/

    BTIM_IntFlagClear(BTIM3, BTIM_FLAG_UI);          /* 清除溢出中断标志位 */
    BTIM_IntEnable(BTIM3, BTIM_INT_UI);              /* 允许BTIM3溢出中断  */
    EnableNvic(CTIM1_IRQn, IrqPriorityLevel3, TRUE); /* 开启中断 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
