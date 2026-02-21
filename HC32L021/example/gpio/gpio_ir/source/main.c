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
static void GpioIrFuncConfig(void);
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
    GpioIrFuncConfig(); /* IR 功能配置 */

    DDL_Delay1ms(3000);

    while (1)
    {
        GPIO_PA08_RESET();
        DDL_Delay1ms(50);

        GPIO_PA08_SET();
        DDL_Delay1ms(50);
    }
}

/**
 * @brief  IR功能配置
 * @retval None
 */
static void GpioIrFuncConfig(void)
{
    /* 使能RCL 38400 */
    SYSCTRL_RCLTrimSet(SYSCTRL_CLK_SRC_RCL_38400); /* 写RCL的TRIM值 */
    SYSCTRL_ClockSrcEnable(SYSCTRL_CLK_SRC_RCL_38400);

    stc_gpio_init_t stcGpioInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /*开启GPIO时钟*/

    /* 端口初始化 */
    GPIO_StcInit(&stcGpioInit);                   /* 结构体变量初始值初始化 */
    stcGpioInit.u32Mode      = GPIO_MD_OUTPUT_PP; /* 端口方向配置 */
    stcGpioInit.u32PullUp    = GPIO_PULL_NONE;    /* 端口上拉配置 */
    stcGpioInit.u32Pin       = GPIO_PIN_08;       /* 端口引脚配置 */
    stcGpioInit.bOutputValue = TRUE;              /* 设置输出端口，端口功能输出为高电平 */
    GPIOA_Init(&stcGpioInit);                     /* GPIO IR端口初始化 */
    GPIO_PA08_AF_CTRL1_IR_N_SET();                /* 设置红外信号输出极性 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
