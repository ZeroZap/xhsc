/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of GPIO
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
#include "gpio.h"
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
static void GpioUserKeyConfig(void);
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    GpioUserKeyConfig(); /* USER KEY端口初始化 */

    STK_LedConfig(); /* LED端口初始化 */

    while (1)
    {
        ;
    }
}

/**
 * @brief  PortA中断服务函数
 * @retval None
 */
void PortA_IRQHandler(void)
{
    if (TRUE == GPIO_IntFlagGet(STK_USER_PORT, STK_USER_PIN)) /* 获取中断状态 */
    {
        STK_LED_ON(); /* LED点亮 */
        DDL_Delay1ms(2000);

        STK_LED_OFF(); /* LED关闭 */

        GPIO_IntFlagClear(STK_USER_PORT, STK_USER_PIN); /* 清除中断标志位 */
    }
}

/**
 * @brief  GPIO按键配置
 * @retval None
 */
static void GpioUserKeyConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    GPIO_StcInit(&stcGpioInit);                      /* 结构体变量初始值初始化 */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);      /* 打开GPIO外设时钟门控 */
    stcGpioInit.u32Mode      = GPIO_MD_INT_INPUT;    /* 端口方向配置 */
    stcGpioInit.u32PullUp    = GPIO_PULL_NONE;       /* 端口上拉配置 */
    stcGpioInit.u32ExternInt = GPIO_EXTI_LOW;        /* 端口外部中断触发方式配置 */
    stcGpioInit.u32Pin       = STK_USER_PIN;         /* 端口引脚配置 */
    GPIOA_Init(&stcGpioInit);                        /* GPIO USER KEY初始化 */
    EnableNvic(PORTA_IRQn, IrqPriorityLevel3, TRUE); /* 使能端口PORTA系统中断 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
