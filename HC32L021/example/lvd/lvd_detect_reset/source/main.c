/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of LVD
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-04       MADS            First version
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
#include "lvd.h"
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
static void LvdConfig(void);
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
    /* 首次上电FLAG清零 */
    if (TRUE == SYSCTRL_ResetFlagGet(SYSCTRL_RST_POR5V))
    {
        SYSCTRL_ResetFlagClearAll();
    }

    GpioConfig(); /* GPIO端口配置 */

    STK_LedConfig(); /* LED配置 */

    /* 低电压复位检测 */
    if (TRUE == SYSCTRL_ResetFlagGet(SYSCTRL_RST_LVD))
    {
        LVD_IntFlagClear();

        SYSCTRL_ResetFlagClear(SYSCTRL_RST_LVD);

        STK_LED_ON();
        DDL_Delay1ms(1000);
        STK_LED_OFF();
        DDL_Delay1ms(1000);
    }

    /* LVD 配置 */
    LvdConfig();

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

    /* 开启GPIO外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    /* 配置LVD_OUT PA06 中断受触发设置限制，LVD输出不受限制 */
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Pin  = GPIO_PIN_06;
    stcGpioInit.u32Mode = GPIO_MD_OUTPUT_PP;
    GPIOA_Init(&stcGpioInit);
    GPIO_PA06_AF_LVD_OUT();

    /* 配置LVD输入源 PA03 */
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Pin  = GPIO_PIN_03;
    stcGpioInit.u32Mode = GPIO_MD_ANALOG;
    GPIOA_Init(&stcGpioInit);
}

/**
 * @brief  LVD配置
 * @retval None
 */
static void LvdConfig(void)
{
    stc_lvd_init_t stcLvdInit = {0};

    LVD_StcInit(&stcLvdInit); /* 结构体初始化 */

    SYSCTRL_PeriphClockEnable(PeriphClockVc); /* 开LVD时钟 */

    stcLvdInit.u32TriggerAction = LVD_TRIG_ACT_RESET;     /* 配置触发产生复位 */
    stcLvdInit.u32TriggerMode   = LVD_TRIG_MD_HIGH;       /* 中断触发类型 */
    stcLvdInit.u32Src           = LVD_SRC_PA03;           /* 配置LVD输入源 */
    stcLvdInit.u32ThresholdVolt = LVD_THRESHOLD_VOLT1P8V; /* 配置LVD基准电压 */
    stcLvdInit.u32FilterTime    = LVD_FILTER_TIME_2CYCLE; /* 滤波时间设置 */
    stcLvdInit.u32FilterMode    = LVD_FILTER_MD_NFM;      /* 滤波模式 */
    LVD_Init(&stcLvdInit);

    /* LVD 模块使能 */
    LVD_Enable();
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
