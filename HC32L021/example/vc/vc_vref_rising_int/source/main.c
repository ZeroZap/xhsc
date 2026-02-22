/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of VC
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-03       MADS            First version
   2025-07-08       MADS            Add VC trim select
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2024~2025, Xiaohua Semiconductor Co., Ltd. All rights reserved.
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
#include "hc32l021_vc.h"
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
static void Vc1Config(void);
/*******************************************************************************
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
    GpioConfig();      /* GPIO端口配置 */
    STK_LedConfig();   /* LED配置 */
    Vc1Config();       /* VC1配置 */
    VC_Enable(VC_CH1); /* VC1使能 */

    while (1) {
        ;
    }
}

/**
 * @brief  VC1中断服务函数
 * @retval None
 */
void Vc1_IRQHandler(void)
{
    if (VC_IntFlagGet(VC_CH1) == TRUE) {
        VC_IntFlagClear(VC_CH1);

        STK_LED_ON();
        DDL_Delay1ms(200);
        STK_LED_OFF();
        DDL_Delay1ms(200);
    }
}

/**
 * @brief  GPIO端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = { 0 };

    /* GPIOA 外设时钟使能 */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    /* PA15作为VC1_OUT 端口初始化*/
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Pin  = GPIO_PIN_15;
    stcGpioInit.u32Mode = GPIO_MD_OUTPUT_PP;
    GPIOA_Init(&stcGpioInit);
    GPIO_PA15_AF_VC1_OUT();

    /* PA01 VC1 P端输入 */
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Pin  = GPIO_PIN_01;
    stcGpioInit.u32Mode = GPIO_MD_ANALOG;
    GPIOA_Init(&stcGpioInit);
}

/**
 * @brief  Vc1配置
 * @retval None
 */
static void Vc1Config(void)
{
    stc_vc_init_t stcVcInit = { 0 };

    VC_StcInit(&stcVcInit); /* 结构体初始化 */

    SYSCTRL_PeriphClockEnable(PeriphClockVc); /* 开VC时钟 */

    stcVcInit.u32Ch      = VC_CH1;                  /* 使用Vc1 */
    stcVcInit.u32HysVolt = VC_HYSTERESIS_VOLT_10MV; /* 迟滞电压约为10mV */
    stcVcInit.u32BiasCurrent = VC_BIAS_CURR_MEDIUM; /* 中功耗 */
    stcVcInit.u32FilterTime =
        VC_FILTER_TIME_16CYCLE; /* 滤波时间设置16个滤波时钟周期 */
    stcVcInit.u32InputPos = VC_POS_SEL_CH0; /* 电压比较器的P端连接PA01 */
    stcVcInit.u32InputNeg =
        VC_NEG_SEL_CH4; /* 电压比较器的N端连接6BDAC电阻分压 */
    stcVcInit.u32FilterClk  = VC_FILTER_CLK_RC256K; /* 滤波时钟RC256K */
    stcVcInit.u32FilterMode = VC_FILTER_MD_NFM;     /* 普通滤波模式 */
    stcVcInit.u32OutputPol  = VC_OUTPUT_POL_POS;    /* 输出信号极性正 */
    stcVcInit.u32IntMode    = VC_INT_MD_RISING;     /* 上升沿中断 */
    stcVcInit.u8TrimSetSelect =
        VC_TRIM_LOWER_0P5AVCC; /* 正端或负端的参考电压小于等于1/2 AVCC */
    VC_Init(&stcVcInit);

    VC_ResVoltDivConfig(
        VC_CH1, 23); /* 电阻分压配置，ResDivOut = (23/64)*3.3V = 1.2V */
    VC_ResVoltDivEnable(); /* 电阻分压使能 */

    VC_IntFlagClear(VC_CH1); /* 清除VC1中断标志 */
    VC_IntEnable(VC_CH1);    /* VC1中断使能 */
    EnableNvic(VC1_IRQn, IrqPriorityLevel1, TRUE);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
