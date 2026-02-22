/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of ADC
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-02       MADS            First version
   2025-07-08       MADS            Modify ADC IBAS selection
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

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l021_adc.h"
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
static void AdcConfig(void);
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
volatile uint32_t u32AdcResult2;
volatile uint32_t u32AdcResult3;
volatile uint32_t u32AdcResult5;
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    /* ADC配置 */
    AdcConfig();

    while (1) {
        DDL_Delay1ms(2000);
        ADC_SqrStart();

        /* 等待转换完成*/
        if (ADC_IntFlagGet(ADC_FLAG_SQR)) {
            /* 清除中断标志 */
            ADC_IntFlagClear(ADC_FLAG_SQR);

            /* 获取采样值 */
            u32AdcResult2 = ADC_SqrResultGet(ADC_SQR_CH0_MUX);
            u32AdcResult3 = ADC_SqrResultGet(ADC_SQR_CH1_MUX);
            u32AdcResult5 = ADC_SqrResultGet(ADC_SQR_CH2_MUX);
        }
    }
}

/**
 * @brief  ADC配置
 * @retval None
 */
static void AdcConfig(void)
{
    stc_adc_sqr_init_t stcAdcSqrConfig = { 0 };

    /* 开启 ADC 外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockAdc);

    /* ADC 使能 */
    ADC_Enable();

    /* ADC 初始化配置 */
    ADC_SqrStcInit(&stcAdcSqrConfig);
    stcAdcSqrConfig.u32SampCycle = ADC_SAMPLE_CYCLE_12; /* ADC采样周期选择 */
    stcAdcSqrConfig.u32RefVoltage = ADC_REF_VOL_AVCC; /* ADC参考电压选择 */
    stcAdcSqrConfig.u32ClockDiv = ADC_CLK_DIV8; /* ADC时钟分频选择 */
    stcAdcSqrConfig.u32CurrentSelect =
        ADC_IBAS_LOWEST_POWER;       /* ADC IBAS电流选择 */
    stcAdcSqrConfig.u32SqrCount = 3; /* ADC转换次数配置 */
    ADC_SqrInit(&stcAdcSqrConfig);   /* 初始化配置 */

    /* 配置通道和通道输入源 */
    GPIO_PA02_ANALOG_SET();
    GPIO_PA03_ANALOG_SET();
    GPIO_PA05_ANALOG_SET();
    ADC_ConfigSqrCh(
        ADC_SQR_CH0_MUX, ADC_INPUT_CH1); /* 配置通道0的输入源来自PA02 */
    ADC_ConfigSqrCh(
        ADC_SQR_CH1_MUX, ADC_INPUT_CH2); /* 配置通道1的输入源来自PA03 */
    ADC_ConfigSqrCh(
        ADC_SQR_CH2_MUX, ADC_INPUT_CH3); /* 配置通道3的输入源来自PA05 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
