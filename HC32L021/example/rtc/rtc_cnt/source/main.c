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
#include "rtc.h"
#include "sysctrl.h"
/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void RtcConfig(void);
/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
static stc_rtc_time_t stcReadTime;
volatile uint8_t      u8Flag;
volatile uint8_t      u8Second, u8Minute, u8Hour, u8Day, u8Week, u8Month, u8Year;
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
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
    /* 配置RTC */
    RtcConfig();

    while (1)
    {
        if (u8Flag == 1)
        {
            u8Flag = 0;
            RTC_DateTimeRead(&stcReadTime);
            u8Second = stcReadTime.u8Second;
            u8Minute = stcReadTime.u8Minute;
            u8Hour   = stcReadTime.u8Hour;
            u8Day    = stcReadTime.u8Day;
            u8Week   = stcReadTime.u8DayOfWeek;
            u8Month  = stcReadTime.u8Month;
            u8Year   = stcReadTime.u8Year;
        }
    }
}

/**
 * @brief  RTC中断入口函数
 * @retval None
 */
void Rtc_IRQHandler(void)
{
    if (RTC_IntFlagGet(RTC_INT_PRD) == TRUE)
    {
        u8Flag = 1;
        RTC_IntFlagClear(RTC_INT_PRD); /* 清除中断标志位 */
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
    SYSCTRL_DebugActiveEnable(SYSCTRL_DEBUG_RTC_ACTIVE); /* debug时RTC正常计数 */

    stc_rtc_init_t stcRtcInit = {0};
    stc_rtc_prd_t  stcPrdInit = {0};

    /* 结构体初始化 */
    RTC_RtcStcInit(&stcRtcInit);
    RTC_PrdStcInit(&stcPrdInit);

    stcRtcInit.u32ClockSelect         = RTC_CLK_XTL; /* 外部低速时钟 */
    stcRtcInit.u8Format               = RTC_FMT_24H; /* 24小时制 */
    stcRtcInit.stcRtcTime.u8Second    = 0x55u;
    stcRtcInit.stcRtcTime.u8Minute    = 0x01u;
    stcRtcInit.stcRtcTime.u8Hour      = 0x14u;
    stcRtcInit.stcRtcTime.u8Day       = 0x18u;
    stcRtcInit.stcRtcTime.u8DayOfWeek = 0x04u;
    stcRtcInit.stcRtcTime.u8Month     = 0x08u;
    stcRtcInit.stcRtcTime.u8Year      = 0x22u;

    stcPrdInit.u32PrdSelect = RTC_PRDSEL_PRDX; /* 选择设置产生时间间隔方式:PRDX/PRDS*/
    stcPrdInit.u8Prdx       = 5u;              /* 0000101:3秒，周期中断时间间隔。选择PRDX时有效，选择PRDS时无效 */
    // stcPrdInit.u32Prds      = RTC_PRDS_1SEC;   /* 设置产生中断的时间间隔。选择PRDS时有效，选择PRDX时无效 */

    RTC_Unlock(); /* RTC写保护解锁 */

    RTC_Init(&stcRtcInit);       /* RTC初始化 */
    RTC_PrdConfig(&stcPrdInit);  /* 周期中断配置 */
    RTC_IntDisable(RTC_INT_ALL); /* 禁止所有中断 */
    RTC_IntEnable(RTC_INT_PRD);  /* 使能周期中断 */
    RTC_Enable();                /* 使能RTC开始计数 */

    EnableNvic(RTC_IRQn, IrqPriorityLevel0, TRUE); /*使能RTC中断向量 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
