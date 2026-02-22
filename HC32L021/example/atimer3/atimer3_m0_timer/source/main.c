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
    STK_LedConfig(); /* 板载LED配置 */

    Atimer3Config(25000); /* ATIM3 配置; 16分频,周期25000-->25000*(1/4M) * 16 =
                             100000us = 100ms */

    ATIM3_Mode0_Run(); /* Atimer3运行 */

    while (1) {
        ;
    }
}

/**
 * @brief  ATIM3 中断函数
 * @retval None
 */
void Atim3_IRQHandler(void)
{
    ATIM3_IntFlagClear(ATIM3_FLAG_UI); /* ATIM3 模式0 中断标志清除 */
    static boolean_t bFlag = FALSE;

    /* ATIM3 模式0 计时溢出中断 */
    if (FALSE == bFlag) {
        STK_LED_ON(); /* LED引脚输出高电平 */
        bFlag = TRUE;
    } else {
        STK_LED_OFF(); /* LED引脚输出低电平 */
        bFlag = FALSE;
    }
}

/**
 * @brief  ATIM3 配置
 * @param  [in] u16Period 周期值
 * @retval None
 */
static void Atimer3Config(uint16_t u16Period)
{
    uint16_t u16ArrValue;
    uint16_t u16CntValue;
    stc_atim3_mode0_init_t stcAtim3BaseConfig = { 0 };

    SYSCTRL_PeriphClockEnable(PeriphClockAtim3); /* ATIM3 外设时钟使能 */

    ATIM3_Mode0_StcInit(&stcAtim3BaseConfig); /* 结构体初始化 */
    stcAtim3BaseConfig.u32WorkMode =
        ATIM3_M0_M0CR_WORK_MODE_TIMER; /* 定时器模式 */
    stcAtim3BaseConfig.u32CountClockSelect =
        ATIM3_M0_M0CR_CT_PCLK; /* 定时器功能，计数时钟为内部PCLK */
    stcAtim3BaseConfig.u32PRS = ATIM3_M0_M0CR_ATIM3CLK_PRS16; /* PCLK/16 */
    stcAtim3BaseConfig.u32CountMode =
        ATIM3_M0_M0CR_MD_16BIT_ARR; /* 自动重载16位计数器/定时器 */
    stcAtim3BaseConfig.u32EnTog = ATIM3_M0_M0CR_TOG_OFF; /* TOG关闭 */
    stcAtim3BaseConfig.u32EnGate = ATIM3_M0_M0CR_GATE_OFF; /* 门控功能关闭 */
    stcAtim3BaseConfig.u32GatePolar = ATIM3_M0_M0CR_GATE_POLAR_HIGH;

    ATIM3_Mode0_Init(&stcAtim3BaseConfig); /* ATIM3 的模式0功能初始化 */

    u16ArrValue = 0x10000 - u16Period;
    ATIM3_Mode0_ARRSet(u16ArrValue); /* 设置重载值(ARR = 0x10000 - 周期) */

    u16CntValue = 0x10000 - u16Period;
    ATIM3_Mode0_Cnt16Set(u16CntValue); /* 设置计数初值 */

    ATIM3_IntFlagClear(ATIM3_FLAG_UI); /* 清中断标志 */
    ATIM3_Mode0_IntEnable(); /* 使能ATIM3中断(模式0时只有一个中断) */
    EnableNvic(ATIM3_IRQn, IrqPriorityLevel3, TRUE); /* ATIM3 开中断 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
