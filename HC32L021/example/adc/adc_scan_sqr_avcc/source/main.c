/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of ADC
 @verbatim
   Change Logs:
   Date             Author          Notes
   2025-07-07       MADS            First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2025, Xiaohua Semiconductor Co., Ltd. All rights reserved.
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
#include "adc.h"
#include "gpio.h"
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
uint32_t  u32AdcCH0Result;  /* VCAP Adc值 */
uint32_t  u32AdcCH1Result;  /* AVCC/3 Adc值 */
float32_t f32AdcAvccResult; /* AVCC值 */
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

    while (1)
    {
        DDL_Delay1ms(2000);
        ADC_SqrStart();

        /* 等待转换完成*/
        if (ADC_IntFlagGet(ADC_FLAG_SQR))
        {
            /* 清除中断标志 */
            ADC_IntFlagClear(ADC_FLAG_SQR);

            /* 获取采样值 */
            u32AdcCH0Result  = ADC_SqrResultGet(ADC_SQR_CH0_MUX);
            u32AdcCH1Result  = ADC_SqrResultGet(ADC_SQR_CH1_MUX);
            f32AdcAvccResult = 1.6f * u32AdcCH1Result / u32AdcCH0Result * 3.0f;
        }
    }
}

/**
 * @brief  ADC配置
 * @retval None
 */
static void AdcConfig(void)
{
    stc_adc_sqr_init_t stcAdcSqrConfig = {0};

    /* 开启 ADC 外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockAdc);

    /* ADC 使能 */
    ADC_Enable();

    /* ADC 初始化配置 */
    ADC_SqrStcInit(&stcAdcSqrConfig);
    stcAdcSqrConfig.u32SampCycle     = ADC_SAMPLE_CYCLE_12;   /* ADC采样周期选择 */
    stcAdcSqrConfig.u32RefVoltage    = ADC_REF_VOL_AVCC;      /* ADC参考电压选择 */
    stcAdcSqrConfig.u32ClockDiv      = ADC_CLK_DIV8;          /* ADC时钟分频选择 */
    stcAdcSqrConfig.u32CurrentSelect = ADC_IBAS_LOWEST_POWER; /* ADC IBAS电流选择 */
    stcAdcSqrConfig.u32SqrCount      = 2;                     /* ADC转换次数配置 */
    ADC_SqrInit(&stcAdcSqrConfig);                            /* 初始化配置 */

    /* 配置通道和通道输入源 */
    ADC_ConfigSqrCh(ADC_SQR_CH0_MUX, ADC_INPUT_CH8); /* 配置通道0的输入源来自VCAP */
    ADC_ConfigSqrCh(ADC_SQR_CH1_MUX, ADC_INPUT_CH9); /* 配置通道1的输入源来自AVCC/3 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
