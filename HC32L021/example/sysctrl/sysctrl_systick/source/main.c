/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of SYSCTRL
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-11-14       MADS            First version
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
#include "flash.h"
#include "gpio.h"
#include "sysctrl.h"
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
static void SysClockConfig(void);
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
    /* 系统时钟配置：4M */
    SysClockConfig();

    /* LED端口配置 */
    STK_LedConfig();

    /* 内核函数，SysTick配置，定时1s，系统时钟默认RC48M 4MHz */
    SysTick_Config(SystemCoreClock);

    while (1)
    {
        ;
    }
}

/**
 * @brief  系统时钟配置
 * @retval None
 */
static void SysClockConfig(void)
{
    stc_sysctrl_clock_init_t stcSysClockInit = {0};

    /* 结构体初始化 */
    SYSCTRL_ClockStcInit(&stcSysClockInit);

    stcSysClockInit.u32SysClockSrc = SYSCTRL_CLK_SRC_RC48M_4M; /* 选择系统默认RC48M 4MHz作为Hclk时钟源 */
    stcSysClockInit.u32HclkDiv     = SYSCTRL_HCLK_PRS_DIV8;    /* Hclk 8分频 */
    SYSCTRL_ClockInit(&stcSysClockInit);                       /* 系统时钟初始化 */
}

/**
 * @brief systick中断函数
 * @retval None
 */
void SysTick_IRQHandler(void)
{
    if (STK_LED_READ()) /* LED is on */
    {
        STK_LED_OFF();
    }
    else /* LED is off */
    {
        STK_LED_ON();
    }
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
