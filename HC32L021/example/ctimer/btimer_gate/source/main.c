/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of  the BTIM
 *        library.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-03       MADS            First version
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

    GpioConfig(); /* GPIO初始化 */

    Btim1Config(); /* BTIM1初始化 */

    BTIM_Enable(BTIM1); /* 启动BTIM1运行 */

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

    if (TRUE == BTIM_IntFlagGet(BTIM1, BTIM_FLAG_UI)) /* 获取BTIM1的溢出中断标志位 */
    {
        if (TRUE == bFlag)
        {
            STK_LED_ON(); /* LED 开启 */
            bFlag = FALSE;
        }
        else
        {
            STK_LED_OFF(); /* LED 关闭 */
            bFlag = TRUE;
        }

        BTIM_IntFlagClear(BTIM1, BTIM_FLAG_UI); /* 清除BTIM1的溢出中断标志位 */
    }
}

/**
 * @brief  端口配置初始化
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcETRGpioInit = {0};
    stc_gpio_init_t stcTogGpioInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* 开启GPIO时钟 */

    /* CIIM1_ETR端口初始化 */
    GPIO_StcInit(&stcETRGpioInit);             /* 结构体变量初始值初始化 */
    stcETRGpioInit.u32Mode   = GPIO_MD_INPUT;  /* 端口方向配置 */
    stcETRGpioInit.u32PullUp = GPIO_PULL_NONE; /* 端口上拉配置 */
    stcETRGpioInit.u32Pin    = GPIO_PIN_05;    /* 端口引脚配置 */
    GPIOA_Init(&stcETRGpioInit);               /* GPIO初始化 */
    GPIO_PA05_AF_CTIM0_ETR();                  /* CIIM0_ETR端口复用功能 */

    /* CTIM0_CH0和CTIM0_CH1端口初始化 */
    GPIO_StcInit(&stcTogGpioInit);                        /* 结构体变量初始值初始化 */
    stcTogGpioInit.u32Mode   = GPIO_MD_OUTPUT_PP;         /* 端口方向配置 */
    stcTogGpioInit.u32PullUp = GPIO_PULL_NONE;            /* 端口上拉配置 */
    stcTogGpioInit.u32Pin    = GPIO_PIN_08 | GPIO_PIN_09; /* 端口引脚配置 */
    GPIOA_Init(&stcTogGpioInit);                          /* GPIO 初始化 */
    GPIO_PA08_AF_CTIM0_CH0();                             /* CTIM0_CH0端口复用功能 */
    GPIO_PA09_AF_CTIM0_CH1();                             /* CTIM0_CH1端口复用功能 */
}

/**
 * @brief  初始化BTIM1
 * @retval None
 */
static void Btim1Config(void)
{
    stc_btim_init_t stcBtimInit = {0};

    SYSCTRL_FuncEnable(SYSCTRL_FUNC_CTIMER0_USE_BTIM); /* 配置BTIM0/1/2有效，GTIM0无效 */
    SYSCTRL_PeriphClockEnable(PeriphClockCtim0);       /* 使能BTIM0/1/2 外设时钟 */

    BTIM_StcInit(&stcBtimInit);                                      /* 结构体变量初始值初始化 */
    stcBtimInit.u32Mode                = BTIM_MD_GATE;               /*工作模式:门控模式，计数时钟为PCLK，ETR管脚输入信号作为门控 */
    stcBtimInit.u32OneShotEn           = BTIM_CONTINUOUS_COUNTER;    /* 连续计数模式 */
    stcBtimInit.u32Prescaler           = BTIM_COUNTER_CLK_DIV256;    /* 对计数时钟进行预除频 */
    stcBtimInit.u32ToggleEn            = BTIM_TOGGLE_ENABLE;         /* TOG输出使能，TOG和TOGN输出相位相反的信号 */
    stcBtimInit.u32TriggerSrc          = BTIM_TRIG_SRC_ETR;          /* 触发源选择，使用外部ETR输入作为GATE门控 */
    stcBtimInit.u32ExternInputPolarity = BTIM_ETR_POLARITY_INVERTED; /* 外部输入极性选择，极性翻转，低电平有效 */
    stcBtimInit.u32AutoReloadValue     = 4000 - 1;                   /* 自动重载寄存ARR赋值,计数周期为PRS*(ARR+1)*TPCLK */
    BTIM_Init(BTIM1, &stcBtimInit);

    BTIM_IntFlagClear(BTIM1, BTIM_FLAG_UI);          /* 清除溢出中断标志位 */
    BTIM_IntEnable(BTIM1, BTIM_INT_UI);              /* 允许BTIM1溢出中断   */
    EnableNvic(CTIM0_IRQn, IrqPriorityLevel3, TRUE); /* 开启中断 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
