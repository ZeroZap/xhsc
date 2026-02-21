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
static void GpioConfig(void);
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
    /* 系统时钟配置：48M */
    SysClockConfig();

    /* USER KEY端口配置 */
    STK_UserKeyConfig();

    /* =============================================== */
    /* ==============       WARNING       ============ */
    /* =============================================== */
    /* 因本样例使用PA14（SWCLK）输出时钟，为了避免程序再次下载出现问题，请勿删除延时DDL_Delay1ms函数调用 */
    DDL_Delay1ms(2000u);

    while (!STK_USER_KEY_PRESSED()) /* 等待按键按下 */
    {
        ;
    }

    /* 配置Hclk分频时钟信号从IO口输出 */
    GpioConfig();

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

    stcSysClockInit.u32SysClockSrc = SYSCTRL_CLK_SRC_RC48M_48M; /* 选择系统默认RC48M 48MHz作为Hclk时钟源 */
    stcSysClockInit.u32HclkDiv     = SYSCTRL_HCLK_PRS_DIV1;     /* Hclk 1分频 */
    SYSCTRL_ClockInit(&stcSysClockInit);                        /* 系统时钟初始化 */
}

/**
 * @brief  端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    /* 结构体初始化 */
    GPIO_StcInit(&stcGpioInit);

    /* 开启GPIO外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    /* 配置PA14端口为输出 */
    stcGpioInit.u32Pin  = GPIO_PIN_14;
    stcGpioInit.u32Mode = GPIO_MD_OUTPUT_PP;
    GPIOA_Init(&stcGpioInit);
    /* 设置从PA14输出Hclk的8分频时钟 */
    GPIO_PA14_AF_TCLK_OUT_HCLK_DIV8();

    /* 配置PA14作为端口使用 */
    SYSCTRL_FuncEnable(SYSCTRL_FUNC_SWD_USE_IO);
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
