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
static void RtcAlarmConfig(void);
/******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static uint8_t u8ToggleCnt;
/******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    /* LED配置 */
    STK_LedConfig();

    /* RTC配置 */
    RtcConfig();

    /* RTC闹钟配置 */
    RtcAlarmConfig();

    while (1)
    {
        if (u8ToggleCnt > 0) /* ToggleCnt 在中断中设定值 */
        {
            u8ToggleCnt--;

            STK_LED_ON(); /* 点亮LED */
            DDL_Delay1ms(500);

            STK_LED_OFF(); /* 熄灭LED */
            DDL_Delay1ms(500);
        }
    }
}

/**
 * @brief  RTC中断入口函数
 * @retval None
 */
void Rtc_IRQHandler(void)
{
    if (RTC_IntFlagGet(RTC_INT_ALMA) == TRUE) /* 闹铃A中断 */
    {
        u8ToggleCnt = 10;               /* 通知main函数主循环中闪烁LED 10次 */
        RTC_IntFlagClear(RTC_INT_ALMA); /* 清中断标志位 */
    }
}

/**
 * @brief  RTC配置
 * @retval None
 */
static void RtcConfig(void)
{
    SYSCTRL_PeriphClockEnable(PeriphClockRtc);           /* RTC模块时钟打开 */
    SYSCTRL_ClockSrcEnable(SYSCTRL_CLK_SRC_XTL);         /* 打开XTL */
    SYSCTRL_DebugActiveEnable(SYSCTRL_DEBUG_RTC_ACTIVE); /* debug时RTC正常计数 */

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

    RTC_Init(&stcRtcInit);       /* RTC初始化 */
    RTC_IntDisable(RTC_INT_ALL); /* 禁止所有中断 */
    RTC_Enable();                /* 使能RTC开始计数 */

    RTC_Lock(); /* RTC写保护锁定 */
}

/**
 * @brief  闹钟配置
 * @retval None
 */
static void RtcAlarmConfig(void)
{
    stc_rtc_alarmtime_t stcAlarmTime = {0};

    RTC_AlarmStcInit(&stcAlarmTime); /* 结构体初始化 */

    // stcAlarmTime.u8AmPm           = RTC_AMPM_AM; /* 12小时制需要设置AmPm，24小时制不用设置 */
    stcAlarmTime.u8AlarmSecond    = 0x05;
    stcAlarmTime.bAlarmMskSecond  = FALSE; /* FALSE 不屏蔽，秒匹配才产生闹钟中断 */
    stcAlarmTime.u8AlarmMinute    = 0x02;
    stcAlarmTime.bAlarmMskMinute  = FALSE; /* FALSE 不屏蔽，分匹配才产生闹钟中断 */
    stcAlarmTime.u8AlarmHour      = 0x14;
    stcAlarmTime.bAlarmMskHour    = FALSE;           /* FALSE 不屏蔽，时匹配才产生闹钟中断 */
    stcAlarmTime.u8AlarmDayOfWeek = RTC_ALMWEEK_ALL; /* 周内7天全选 */

    RTC_Unlock();                                 /* RTC写保护解锁 */
    RTC_AlarmTimeSet(RTC_ALM_CHA, &stcAlarmTime); /* 设置闹钟时间 */
    RTC_IntEnable(RTC_INT_ALMA);                  /* 使能闹钟中断 */
    RTC_AlarmEnalbe(RTC_ALM_CHA);                 /* 使能闹钟 */
    RTC_Lock();                                   /* RTC写保护锁定 */

    EnableNvic(RTC_IRQn, IrqPriorityLevel0, TRUE); /* 使能RTC中断向量 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
