/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of RTC
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-05       MADS            First version
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
#include "rtc.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void RtcConfig(void);
static void RtcWakeupTimerConfig(void);
static void GpioLowPowerConfig(void);
/******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    STK_LedConfig();        /* LED配置 */
    STK_UserKeyConfig();    /* KEY配置 */
    RtcConfig();            /* RTC配置 */
    DDL_Delay1ms(3000);     /* 延时3秒 */
    RtcWakeupTimerConfig(); /* RTC唤醒定时器配置 */

    while (1)
    {
        if (TRUE == STK_USER_KEY_PRESSED()) /* 等待按键按下 */
        {
            FLASH_LowPowerEnable(); /* 配置FLASH为低功耗模式 */

            GpioLowPowerConfig(); /* 配置Demo板上所有引脚配置为模拟端口(除按键、LED端口外),避免端口漏电 */

            LPM_GotoDeepSleep(TRUE); /* 进入低功耗深度睡眠模式，中断唤醒执行完中断自动进入低功耗模式 */
        }
    }
}

/**
 * @brief  RTC中断入口函数
 * @retval None
 */
void Rtc_IRQHandler(void)
{
    if (RTC_IntFlagGet(RTC_INT_WU) == TRUE)
    {
        RTC_IntFlagClear(RTC_INT_WU); /* 清除中断标志位 */
        if (STK_LED_READ())
        {
            STK_LED_OFF(); /* 输出低，熄灭LED */
        }
        else
        {
            STK_LED_ON(); /* 输出高，点亮LED */
        }
    }
}

/**
 * @brief  RTC配置函数
 * @retval None
 */
static void RtcConfig(void)
{
    SYSCTRL_PeriphClockEnable(PeriphClockRtc);           /* RTC模块时钟打开 */
    SYSCTRL_ClockSrcEnable(SYSCTRL_CLK_SRC_XTL);         /* 打开XTL */
    SYSCTRL_DebugActiveEnable(SYSCTRL_DEBUG_RTC_ACTIVE); /* Debug时RTC正常计数 */

    stc_rtc_init_t stcRtcInit = {0};

    RTC_RtcStcInit(&stcRtcInit); /* 结构体初始化 */

    stcRtcInit.u32ClockSelect         = RTC_CLK_XTL; /* 外部低速时钟 */
    stcRtcInit.u8Format               = RTC_FMT_24H; /* 24小时制 */
    stcRtcInit.stcRtcTime.u8Second    = 0x55;
    stcRtcInit.stcRtcTime.u8Minute    = 0x01;
    stcRtcInit.stcRtcTime.u8Hour      = 0x14;
    stcRtcInit.stcRtcTime.u8Day       = 0x18;
    stcRtcInit.stcRtcTime.u8DayOfWeek = 0x04;
    stcRtcInit.stcRtcTime.u8Month     = 0x08;
    stcRtcInit.stcRtcTime.u8Year      = 0x22;

    RTC_Unlock(); /* RTC写保护解锁 */

    RTC_Init(&stcRtcInit);         /* RTC初始化 */
    RTC_IntDisable(RTC_INT_ALL);   /* 禁止所有中断 */
    RTC_IntFlagClear(RTC_INT_ALL); /* 清除所有中断标记 */
    RTC_Enable();                  /* 使能RTC开始计数 */

    RTC_Lock(); /* RTC写保护锁定 */
}

/**
 * @brief  GPIO低功耗配置
 * @retval None
 */
static void GpioLowPowerConfig(void)
{
    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* 打开GPIO外设时钟门控 */

    SYSCTRL_FuncEnable(SYSCTRL_FUNC_SWD_USE_IO); /* SWD设置为GPIO */

    GPIOA->ADS = 0xEFFEu; /* 配置为模拟端口(除按键、LED端口外) */
}

/**
 * @brief  RTC唤醒中断配置函数
 * @retval None
 */
static void RtcWakeupTimerConfig(void)
{
    RTC_Unlock(); /* RTC写保护解锁 */

    RTC_WakeupTimerDisable();                     /* 先关闭RTC wakeup timer */
    RTC_WakeupTimerClockSelect(RTC_WUTR_SECDIV1); /* RTC wakeup timer选择时钟 */
    RTC_WakeupTimerCycleSet(5u);                  /* RTC wakeup timer设置计数值，倒计时 */
    RTC_IntEnable(RTC_INT_WU);                    /* 使能wakeup timer中断 */
    RTC_WakeupTimerEnable();                      /* 使能wakeup timer计数 */

    RTC_StartWait(); /* 进入低功耗模式之前需执行此函数，以确保RTC已启动完成 */

    RTC_Lock(); /* RTC写保护锁定 */

    EnableNvic(RTC_IRQn, IrqPriorityLevel1, TRUE); /*使能RTC中断向量 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
