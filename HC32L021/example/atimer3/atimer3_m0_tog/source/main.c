/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of ATIM3
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

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l021_atim3.h"
#include "hc32l021_ddl.h"
#include "hc32l021_flash.h"
#include "hc32l021_gpio.h"
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
static void GpioConfig(void);
static void Atimer3Config(uint16_t u16Period);
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
    GpioConfig(); /* 端口配置 */

    Atimer3Config(25000); /* ATIM3 配置; 16分频,周期25000-->25000*(1/4M) * 16 =
                             100000us = 100ms */

    ATIM3_Mode0_Run(); /* Atimer3运行 */

    /* 用户可添加其他代码 */
    while (1) {
        ;
    }
}

/**
 * @brief  ATIM3 CHx 端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = { 0 };

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* GPIOA外设时钟使能 */

    /* PA11(ATIM3_CH0A) PA01(ATIM3_CH0B)端口初始化 */
    GPIO_StcInit(&stcGpioInit);                        /* 结构体初始化 */
    stcGpioInit.u32Pin    = GPIO_PIN_11 | GPIO_PIN_01; /* 端口引脚 */
    stcGpioInit.u32Mode   = GPIO_MD_OUTPUT_PP;         /* 端口方向配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE; /* 端口上下拉配置 */
    GPIOA_Init(&stcGpioInit);               /* 端口初始化 */
    GPIO_PA11_AF_ATIM3_CH0A();              /* 端口复用功能配置 */
    GPIO_PA01_AF_ATIM3_CH0B();              /* 端口复用功能配置 */
}

/**
 * @brief  ATIM3 配置
 * @param  [in] u16Period:周期
 * @retval None
 */
static void Atimer3Config(uint16_t u16Period)
{
    uint16_t u16ArrValue;
    uint16_t u16CntValue;
    stc_atim3_mode0_init_t stcAtim3Mode0Config = { 0 };

    SYSCTRL_PeriphClockEnable(PeriphClockAtim3); /* ATIM3 外设时钟使能 */

    ATIM3_Mode0_StcInit(&stcAtim3Mode0Config); /* 结构体初始化清零 */
    stcAtim3Mode0Config.u32WorkMode =
        ATIM3_M0_M0CR_WORK_MODE_TIMER; /* 定时器模式 */
    stcAtim3Mode0Config.u32CountClockSelect =
        ATIM3_M0_M0CR_CT_PCLK; /* 定时器功能，计数时钟为内部PCLK */
    stcAtim3Mode0Config.u32PRS = ATIM3_M0_M0CR_ATIM3CLK_PRS16; /* PCLK/16 */
    stcAtim3Mode0Config.u32CountMode =
        ATIM3_M0_M0CR_MD_16BIT_ARR; /* 自动重载16位计数器/定时器 */
    stcAtim3Mode0Config.u32EnTog = ATIM3_M0_M0CR_TOG_ON; /* TOG使能 */
    stcAtim3Mode0Config.u32EnGate = ATIM3_M0_M0CR_GATE_OFF; /* 门控功能关闭 */
    stcAtim3Mode0Config.u32GatePolar = ATIM3_M0_M0CR_GATE_POLAR_HIGH;

    ATIM3_Mode0_Init(&stcAtim3Mode0Config); /* ATIM3 的模式0功能初始化 */

    u16ArrValue = 0x10000 - u16Period;
    ATIM3_Mode0_ARRSet(u16ArrValue); /* 设置重载值 */

    u16CntValue = 0x10000 - u16Period;
    ATIM3_Mode0_Cnt16Set(u16CntValue); /* 设置计数初值 */

    ATIM3_Mode0_OutputEnable(); /* ATIM3 端口输出使能 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
