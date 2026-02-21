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
#include "flash.h"
#include "gpio.h"
#include "lpm.h"
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
static void GpioLowPowerConfig(void);
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
    STK_LedConfig(); /* LED 端口初始化 */

    GpioUserKeyConfig(); /* USER KEY 端口初始化 */

    /* =============================================== */
    /* ==============       WARNING       ============ */
    /* =============================================== */
    /* 本样例程序会进入深度休眠模式，因此以下代码起防护作用*/
    /* （防止进入深度休眠后芯片调试功能不能再次使用） */
    /* 在使用本样例时，禁止在没有唤醒机制的情况下删除以下代码 */
    DDL_Delay1ms(2000);

    FLASH_LowPowerEnable(); /* 配置FLASH为低功耗模式 */

    GpioLowPowerConfig(); /* 配置Demo板上所有引脚配置为模拟端口(除按键、LED端口外),避免端口漏电 */

    LPM_GotoDeepSleep(TRUE); /* 进入低功耗模式——深度休眠（使能唤醒后退出中断自动休眠特性） */

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

        GPIO_IntFlagClear(STK_USER_PORT, STK_USER_PIN); /* 清除中断 */
    }
}

/**
 * @brief  GPIO低功耗配置
 * @retval None
 */
static void GpioLowPowerConfig(void)
{
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);  /* 打开GPIO外设时钟门控 */
    SYSCTRL_FuncEnable(SYSCTRL_FUNC_SWD_USE_IO); /* SWD设置为GPIO */

    GPIOA->ADS = 0xEFFEu; /* 配置为模拟端口(除按键、LED端口外) */
}

/**
 * @brief  GPIO按键配置
 * @retval None
 */
static void GpioUserKeyConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* 打开GPIO外设时钟门控 */

    GPIO_StcInit(&stcGpioInit);                      /* 结构体变量初始值初始化 */
    stcGpioInit.u32Mode      = GPIO_MD_INT_INPUT;    /* 端口方向配置 */
    stcGpioInit.u32PullUp    = GPIO_PULL_NONE;       /* 端口上下拉配置 */
    stcGpioInit.u32ExternInt = GPIO_EXTI_FALLING;    /* 端口外部中断触发方式配置 */
    stcGpioInit.u32Pin       = STK_USER_PIN;         /* 端口引脚配置 */
    GPIOA_Init(&stcGpioInit);                        /* GPIO USER KEY初始化 */
    EnableNvic(PORTA_IRQn, IrqPriorityLevel3, TRUE); /* 使能端口PORTA系统中断 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
