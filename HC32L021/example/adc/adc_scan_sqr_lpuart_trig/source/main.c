/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of ADC
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-02       MADS            First version
   2025-07-08       MADS            Modify example
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
#include "hc32l021_lpuart.h"
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void AdcConfig(void);
static void GpioConfig(void);
static void LpuartConfig(void);
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
    /* 端口配置 */
    GpioConfig();

    /* LPUART配置 */
    LpuartConfig();

    /* ADC 配置 */
    AdcConfig();

    while (1) {
        ;
    }
}

/**
 * @brief  ADC中断服务函数
 * @retval None
 */
void Adc_IRQHandler(void)
{
    float32_t f32temp1, f32temp2, f32temp3;

    /* 等待转换完成 */
    if (ADC_IntFlagGet(ADC_FLAG_SQR)) {
        /* 清除中断标志 */
        ADC_IntFlagClear(ADC_FLAG_SQR);

        /* 获取采样值 */
        u32AdcResult2 = ADC_SqrResultGet(ADC_SQR_CH0_MUX);
        u32AdcResult3 = ADC_SqrResultGet(ADC_SQR_CH1_MUX);
        u32AdcResult5 = ADC_SqrResultGet(ADC_SQR_CH2_MUX);

        /* 打印ADC采样值 */
        f32temp1 = (float32_t)u32AdcResult2 / 4095.0f * 3.3f;
        f32temp2 = (float32_t)u32AdcResult3 / 4095.0f * 3.3f;
        f32temp3 = (float32_t)u32AdcResult5 / 4095.0f * 3.3f;
        printf(" %.2f,  %.2f, %.2f\r\n", f32temp1, f32temp2, f32temp3);
    }
}

/**
 * @brief  LPUART0 中断服务函数
 * @retval None
 */
void LpUart0_IRQHandler(void)
{
    if (LPUART_IntFlagGet(LPUART0, LPUART_FLAG_FE)) {
        LPUART_IntFlagClear(LPUART0, LPUART_FLAG_FE); /* 清帧错误请求 */
    }
    if (LPUART_IntFlagGet(LPUART0, LPUART_FLAG_PE)) {
        LPUART_IntFlagClear(LPUART0, LPUART_FLAG_PE); /* 清奇偶检验错误请求 */
    }

    if (LPUART_IntFlagGet(LPUART0, LPUART_FLAG_RC)) /* 接收数据 */
    {
        LPUART_IntFlagClear(LPUART0, LPUART_FLAG_RC); /* 清接收中断请求 */
    }
}

/**
 * @brief  LPUART配置
 * @retval None
 */
static void LpuartConfig(void)
{
    stc_lpuart_init_t stcLpuartInit;

    /* 外设模块时钟使能 */
    SYSCTRL_PeriphClockEnable(PeriphClockLpuart0);

    /* LPUART 初始化 */
    LPUART_StcInit(&stcLpuartInit);                 /* 结构体初始化 */
    stcLpuartInit.u32TransMode = LPUART_MODE_TX_RX; /* 收发模式 */
    stcLpuartInit.u32FrameLength =
        LPUART_FRAME_LEN_8B_PAR; /* 数据8位，奇偶校验1位 */
    stcLpuartInit.u32Parity   = LPUART_B8_PARITY_EVEN; /* 偶校验 */
    stcLpuartInit.u32StopBits = LPUART_STOPBITS_1;     /* 1停止位 */
    stcLpuartInit.u32BaudRateGenSelect =
        LPUART_BAUD_NORMAL; /* 波特率生成选择：用OVER和SCNT产生波特率 */
    stcLpuartInit.stcBaudRate.u32SclkSelect =
        LPUART_SCLK_SEL_PCLK; /* 传输时钟源 */
    stcLpuartInit.stcBaudRate.u32Sclk = SYSCTRL_HclkFreqGet(); /* HCLK获取 */
    stcLpuartInit.stcBaudRate.u32Baud = 9600;                  /* 波特率 */
    LPUART_Init(LPUART0, &stcLpuartInit);

    LPUART_IntFlagClearAll(LPUART0);           /* 清除所有状态标志 */
    LPUART_IntEnable(LPUART0, LPUART_INT_RC);  /* 使能接收中断 */
    LPUART_IntDisable(LPUART0, LPUART_INT_TC); /* 关闭发送中断 */
    EnableNvic(LPUART0_IRQn, IrqPriorityLevel3, TRUE); /* 系统中断使能 */
}

/**
 * @brief  端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    /* 打开GPIO外设时钟门控 */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    stc_gpio_init_t stcGpioInit = { 0 };

    /* 配置PA11为LPUART0_TX */
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Mode      = GPIO_MD_OUTPUT_PP;
    stcGpioInit.bOutputValue = TRUE;
    stcGpioInit.u32Pin       = GPIO_PIN_11;
    GPIO_Init(GPIOA, &stcGpioInit);
    GPIO_PA11_AF_LPUART0_TXD();

    /* 配置PA10为LPUART0_RX */
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Mode   = GPIO_MD_INPUT;
    stcGpioInit.u32PullUp = GPIO_PULL_UP;
    stcGpioInit.u32Pin    = GPIO_PIN_10;
    GPIO_Init(GPIOA, &stcGpioInit);
    GPIO_PA10_AF_LPUART0_RXD();
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

    /* 配置外部触发源 */
    ADC_ExternTrigEnable(ADC_TRIG_LPUART0);

    /* 第四步：配置中断 */
    ADC_IntEnable();
    EnableNvic(ADC_IRQn, IrqPriorityLevel3, TRUE);
}

/**
 * @brief  串口打印重定向
 */
PUTCHAR_PROTOTYPE
{
    LPUART_TransmitPoll(LPUART0, (uint8_t *)(&ch), 1);
    return ch;
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
