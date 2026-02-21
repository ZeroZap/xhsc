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
static void GpioConfig(void);
static void RtcConfig(void);
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
    /* 端口配置 */
    GpioConfig();

    /* RTC配置 */
    RtcConfig();

    while (1)
    {
        ;
    }
}

/**
 * @brief  端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    GPIO_StcInit(&stcGpioInit); /* 初始值清零 */

    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    /* PA05 RTC1Hz输出 */
    stcGpioInit.u32Pin    = GPIO_PIN_05;
    stcGpioInit.u32Mode   = GPIO_MD_OUTPUT_PP;
    stcGpioInit.u32PullUp = GPIO_PULL_NONE;
    GPIOA_Init(&stcGpioInit);
    GPIO_PA05_AF_RTC_1HZ();
}

/**
 * @brief  RTC配置
 * @retval None
 */
static void RtcConfig(void)
{
    uint16_t  u16RtcCompValue = 0;
    float32_t f32RtcCompUint  = 0;

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

    RTC_Init(&stcRtcInit); /* RTC初始化 */

    f32RtcCompUint = 10; /* 补偿目标值 单位:ppm*/

    if (Ok == RTC_CompValueCalculate(f32RtcCompUint, RTC_COMPEN_PRICISION_0P96PPM, &u16RtcCompValue)) /* 计算补偿寄存器值 */
    {
        RTC_CompConfig(u16RtcCompValue, RTC_COMPEN_PRICISION_0P96PPM); /* 补偿配置 */
    }
    RTC_CompEnable(); /* 补偿使能 */

    RTC_IntDisable(RTC_INT_ALL);   /* 禁止所有中断 */
    RTC_IntFlagClear(RTC_INT_ALL); /* 清所有中断标志位 */

    RTC_1HzConfig(RTC_1HZ_NORMAL_PRICISION); /* 配置普通精度1Hz输出 */
    RTC_1HzEnable();                         /* 1Hz输出使能 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
