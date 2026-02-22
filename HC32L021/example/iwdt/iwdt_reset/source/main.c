/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of IWDT
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
#include "hc32l021_gpio.h"
#include "hc32l021_iwdt.h"
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define LED_BLINK()        \
    do {                   \
        STK_LED_OFF();     \
        DDL_Delay1ms(100); \
        STK_LED_ON();      \
        DDL_Delay1ms(100); \
        STK_LED_OFF();     \
        DDL_Delay1ms(100); \
        STK_LED_ON();      \
        DDL_Delay1ms(100); \
        STK_LED_OFF();     \
        DDL_Delay1ms(100); \
        STK_LED_ON();      \
        DDL_Delay1ms(100); \
        STK_LED_OFF();     \
    } while (0);
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void IwdtConfig(void);
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
    /* 首次上电先清除RESET标志 */
    if (TRUE == SYSCTRL_ResetFlagGet(SYSCTRL_RST_POR5V)) {
        SYSCTRL_ResetFlagClearAll();
    }

    /* LED配置 */
    STK_LedConfig();

    /* USER KEY配置 */
    STK_UserKeyConfig();

    /* IWDT 复位现象观测 */
    if (TRUE == SYSCTRL_ResetFlagGet(SYSCTRL_RST_IWDT)) {
        SYSCTRL_ResetFlagClear(SYSCTRL_RST_IWDT);
        LED_BLINK();
    }

    /* IWDT配置 */
    IwdtConfig();

    while (1) {
        /* 溢出前按键按下后执行喂狗程序 */
        if (!STK_USER_KEY_PRESSED()) {
            ;
        } else {
            IWDT_Feed();
            DDL_Delay1ms(200);
            STK_LED_OFF();
        }
    }
}

/**
 * @brief  IWDT配置
 * @retval None
 */
static void IwdtConfig(void)
{
    stc_iwdt_init_t stcIwdtInit = { 0 };

    /* 结构体初始化 */
    IWDT_StcInit(&stcIwdtInit);

    /* 开启WDT外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockIwdt);

    /* IWDT 初始化 */
    /* IWDT 溢出时间 = (计数值(u32ArrCounter)/10000) * 分频系数(u32Prescaler) */
    /* 本例为：(1000/10000)*32 = 3.2s */
    stcIwdtInit.u32Action     = IWDT_OVER_RST_SLEEPMODE_STOP;
    stcIwdtInit.u32ArrCounter = 1000u;
    stcIwdtInit.u32Window     = 0xFFFu;
    stcIwdtInit.u32Prescaler  = IWDT_RC10K_DIV_32;
    IWDT_Init(&stcIwdtInit);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
