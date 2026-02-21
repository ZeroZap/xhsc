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
#include "ddl.h"
#include "gpio.h"
#include "rtc.h"
#include "sysctrl.h"
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
static void RtcTsConfig(void);
/******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static stc_rtc_ts_t stcTsTime;
volatile uint8_t    u8Flag;
volatile uint8_t    u8Second, u8Minute, u8Hour, u8Day, u8Week, u8Month;
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    /* RTC配置 */
    RtcConfig();

    /* RTC时间戳触发配置 */
    RtcTsConfig();

    while (1)
    {
        if (u8Flag == 1)
        {
            u8Flag = 0;
            RTC_TimestampGet(&stcTsTime);
            u8Second = stcTsTime.u8Second;
            u8Minute = stcTsTime.u8Minute;
            u8Hour   = stcTsTime.u8Hour;
            u8Day    = stcTsTime.u8Day;
            u8Week   = stcTsTime.u8DayOfWeek;
            u8Month  = stcTsTime.u8Month;
        }
    }
}

/**
 * @brief  RTC中断入口函数
 * @retval None
 */
void Rtc_IRQHandler(void)
{
    if (RTC_IntFlagGet(RTC_INT_TS) == TRUE) /* 时间戳中断标志 */
    {
        u8Flag = 1;
        RTC_IntFlagClear(RTC_INT_TS); /* 清除中断标志位 */
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

    RTC_Init(&stcRtcInit);         /* RTC初始化 */
    RTC_IntDisable(RTC_INT_ALL);   /* 禁止所有中断 */
    RTC_IntFlagClear(RTC_INT_ALL); /* 清除中断标志位 */
    RTC_Enable();                  /* 使能RTC开始计数 */

    RTC_Lock(); /* RTC写保护锁定 */
}

/**
 * @brief  RTC时间戳触发配置
 * @retval None
 */
static void RtcTsConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* GPIO模块时钟打开 */

    GPIO_StcInit(&stcGpioInit); /* 结构体初始化 */

    stcGpioInit.u32Pin    = GPIO_PIN_01;
    stcGpioInit.u32Mode   = GPIO_MD_INPUT;
    stcGpioInit.u32PullUp = GPIO_PULL_UP;
    GPIOA_Init(&stcGpioInit);
    GPIO_PA01_AF_RTC_TAMP(); /* 设置RTC TS触发脚  */

    RTC_Unlock(); /* RTC写保护解锁 */

    RTC_TimestampEdgeSelect(RTC_TS_EDGE_FALLING); /* RTC时间戳选择下降沿触发 */
    RTC_TimestampEnable();                        /* RTC时间戳触发使能 */
    RTC_IntEnable(RTC_INT_TS);                    /* RTC时间戳中断使能 */

    RTC_Lock(); /* RTC写保护锁定 */

    EnableNvic(RTC_IRQn, IrqPriorityLevel0, TRUE); /*使能RTC中断向量 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
