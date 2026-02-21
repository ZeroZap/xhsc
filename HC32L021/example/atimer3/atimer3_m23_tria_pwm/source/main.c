/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of ATIM3
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-02       MADS            First version
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

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "atim3.h"
#include "ddl.h"
#include "flash.h"
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
static void GpioConfig(void);
static void Atimer3Config(uint16_t u16Period, uint16_t u16CHxACompare, uint16_t u16CHxBCompare);
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
    STK_LedConfig(); /* 板载LED配置 */

    GpioConfig(); /* 端口配置 */

    /* Atimer3 配置:周期 0x9000 CH0/1/2通道A比较值0x6000; CH0/1/2通道B比较值0x3000 */
    Atimer3Config(0x9000, 0x6000, 0x3000);

    ATIM3_Mode23_Run(); /* 运行 */

    /* 可添加其他其他操作，待准备好后使能PWM输出 */

    ATIM3_Mode23_ManualPWMOutputEnable(); /* 端口输出使能 */

    while (1)
    {
        ;
    }
}

/**
 * @brief  ATIM3 中断函数
 * @retval None
 */
void Atim3_IRQHandler(void)
{
    static boolean_t bFlag = FALSE;

    /* Atimer3 模式23 更新中断 */
    if (FALSE == bFlag)
    {
        STK_LED_ON(); /* LED 引脚输出高电平 */

        ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH0A, 0x3000); /* 设置CH0 通道A比较值 */
        ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH0B, 0x6000); /* 设置CH0 通道B比较值 */

        ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH1A, 0x3000); /* 设置CH1 通道A比较值 */
        ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH1B, 0x6000); /* 设置CH1 通道B比较值 */

        ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH2A, 0x3000); /* 设置CH2 通道A比较值 */
        ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH2B, 0x6000); /* 设置CH2 通道B比较值 */

        bFlag = TRUE;
    }
    else
    {
        STK_LED_OFF(); /* LED 引脚输出低电平 */

        ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH0A, 0x6000); /* 设置CH0 通道A比较值 */
        ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH0B, 0x3000); /* 设置CH0 通道B比较值 */

        ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH1A, 0x6000); /* 设置CH1 通道A比较值 */
        ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH1B, 0x3000); /* 设置CH1 通道B比较值 */

        ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH2A, 0x6000); /* 设置CH2 通道A比较值 */
        ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH2B, 0x3000); /* 设置CH2 通道B比较值 */

        bFlag = FALSE;
    }

    ATIM3_IntFlagClear(ATIM3_FLAG_UND); /* 清中断标志 */
}

/**
 * @brief  ATIM3 CHx 端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* GPIOA外设时钟使能 */

    /* PA11(ATIM3_CH0A), PA01(ATIM3_CH0B),PA02(ATIM3_CH1A),PA03(ATIM3_CH1B), PA04(ATIM3_CH2A)PA05(ATIM3_CH2B)端口初始化 */
    GPIO_StcInit(&stcGpioInit);                                                                                /* 结构体初始化 */
    stcGpioInit.u32Pin    = GPIO_PIN_11 | GPIO_PIN_01 | GPIO_PIN_02 | GPIO_PIN_03 | GPIO_PIN_04 | GPIO_PIN_05; /* 端口引脚 */
    stcGpioInit.u32Mode   = GPIO_MD_OUTPUT_PP;                                                                 /* 端口方向配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE;                                                                    /* 端口上下拉配置 */
    GPIOA_Init(&stcGpioInit);                                                                                  /* 端口初始化 */

    GPIO_PA11_AF_ATIM3_CH0A(); /* 端口复用配置 */
    GPIO_PA01_AF_ATIM3_CH0B(); /* 端口复用配置 */
    GPIO_PA02_AF_ATIM3_CH1A(); /* 端口复用配置 */
    GPIO_PA03_AF_ATIM3_CH1B(); /* 端口复用配置 */
    GPIO_PA04_AF_ATIM3_CH2A(); /* 端口复用配置 */
    GPIO_PA05_AF_ATIM3_CH2B(); /* 端口复用配置 */
}

/**
 * @brief  ATIM3 配置
 * @param  [in] u16Period 周期
 * @param  [in] u16CHxACompare CHA比较值
 * @param  [in] u16CHxBCompare CHB比较值
 * @retval None
 */
static void Atimer3Config(uint16_t u16Period, uint16_t u16CHxACompare, uint16_t u16CHxBCompare)
{
    uint16_t                     u16CntValue;
    stc_atim3_mode23_init_t      stcAtim3BaseConfig    = {0};
    stc_atim3_m23_compare_init_t stcAtim3ChxACmpConfig = {0};
    stc_atim3_m23_compare_init_t stcAtim3ChxBCmpConfig = {0};
    stc_atim3_m23_rcr_init_t     stcRcrConfig          = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockAtim3); /* ATIM3 外设时钟使能 */

    ATIM3_Mode23_StcInit(&stcAtim3BaseConfig);                                           /* 结构体初始化清零 */
    stcAtim3BaseConfig.u32WorkMode         = ATIM3_M23_M23CR_WORK_MODE_TRIANGULAR;       /* 三角波模式 */
    stcAtim3BaseConfig.u32CountClockSelect = ATIM3_M23_M23CR_CT_PCLK;                    /* 定时器功能，计数时钟为内部PCLK */
    stcAtim3BaseConfig.u32PRS              = ATIM3_M23_M23CR_ATIM3CLK_PRS1;              /* PCLK */
    stcAtim3BaseConfig.u32PWMTypeSelect    = ATIM3_M23_M23CR_COMP_PWM_INDEPT;            /* 独立输出PWM */
    stcAtim3BaseConfig.u32PWM2sSelect      = ATIM3_M23_M23CR_PWM2S_COMPARE_SINGLE_POINT; /* 单点比较功能 */
    stcAtim3BaseConfig.u32ShotMode         = ATIM3_M23_M23CR_SHOT_CYCLE;                 /* 循环计数 */
    stcAtim3BaseConfig.u32URSSelect        = ATIM3_M23_M23CR_URS_OV_UND;                 /* 上下溢更新 */
    ATIM3_Mode23_Init(&stcAtim3BaseConfig);                                              /* ATIM3 的模式23功能初始化 */

    ATIM3_Mode23_ARRSet(u16Period); /* 设置重载值,并使能缓存 */
    ATIM3_Mode23_ARRBufferEnable();

    ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH0A, u16CHxACompare); /* 设置CH0比较值A */
    ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH0B, u16CHxBCompare); /* 设置CH0比较值B */

    ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH1A, u16CHxACompare); /* 设置CH1比较值A */
    ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH1B, u16CHxBCompare); /* 设置CH1比较值B */

    ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH2A, u16CHxACompare); /* 设置CH2比较值A */
    ATIM3_Mode23_ChannelCompareValueSet(ATIM3_COMPARE_CAPTURE_CH2B, u16CHxBCompare); /* 设置CH2比较值B */

    ATIM3_Mode23_CompareStcInit(&stcAtim3ChxACmpConfig); /* 结构体初始化 */
    stcAtim3ChxACmpConfig.u32CHxCompareCap       = ATIM3_M23_CRCHx_CSA_CSB_COMPARE;
    stcAtim3ChxACmpConfig.u32CHxCompareModeCtrl  = ATIM3_M23_FLTR_OCMxx_PWM_MODE_2;          /* OCREFA输出控制OCMA:PWM模式2 */
    stcAtim3ChxACmpConfig.u32CHxPolarity         = ATIM3_M23_FLTR_CCPxx_NORMAL_IN_OUT;       /* 正常输出 */
    stcAtim3ChxACmpConfig.u32CHxCompareBufEn     = ATIM3_M23_CRCHx_BUFEx_ENABLE;             /* A通道缓存控制 */
    stcAtim3ChxACmpConfig.u32CHxCompareIntSelect = ATIM3_M23_M23CR_CISA_DISABLE_INT;         /* A通道比较中断控制:无 */
    ATIM3_Mode23_PortOutputCHxAConfig(ATIM3_M23_OUTPUT_CHANNEL_CH0, &stcAtim3ChxACmpConfig); /* 比较输出端口配置 */
    ATIM3_Mode23_PortOutputCHxAConfig(ATIM3_M23_OUTPUT_CHANNEL_CH1, &stcAtim3ChxACmpConfig); /* 比较输出端口配置 */
    ATIM3_Mode23_PortOutputCHxAConfig(ATIM3_M23_OUTPUT_CHANNEL_CH2, &stcAtim3ChxACmpConfig); /* 比较输出端口配置 */

    ATIM3_Mode23_CompareStcInit(&stcAtim3ChxBCmpConfig); /* 结构体初始化 */
    stcAtim3ChxBCmpConfig.u32CHxCompareCap = ATIM3_M23_CRCHx_CSA_CSB_COMPARE;
    stcAtim3ChxBCmpConfig.u32CHxCompareModeCtrl = ATIM3_M23_FLTR_OCMxx_PWM_MODE_2; /* OCREFB输出控制OCMB:PWM模式2(PWM互补模式下也要设置，避免强制输出) */
    stcAtim3ChxBCmpConfig.u32CHxPolarity         = ATIM3_M23_FLTR_CCPxx_NORMAL_IN_OUT;       /* 正常输出 */
    stcAtim3ChxBCmpConfig.u32CHxCompareBufEn     = ATIM3_M23_CRCHx_BUFEx_ENABLE;             /* B通道缓存控制使能 */
    stcAtim3ChxBCmpConfig.u32CHxCompareIntSelect = ATIM3_M23_CRCHx_CISB_DISABLE_INT;         /* B通道比较中断控制:无 */
    ATIM3_Mode23_PortOutputCHxBConfig(ATIM3_M23_OUTPUT_CHANNEL_CH0, &stcAtim3ChxBCmpConfig); /* 比较输出端口配置 */
    ATIM3_Mode23_PortOutputCHxBConfig(ATIM3_M23_OUTPUT_CHANNEL_CH1, &stcAtim3ChxBCmpConfig); /* 比较输出端口配置 */
    ATIM3_Mode23_PortOutputCHxBConfig(ATIM3_M23_OUTPUT_CHANNEL_CH2, &stcAtim3ChxBCmpConfig); /* 比较输出端口配置 */

    ATIM3_Mode23_RCRStcInit(&stcRcrConfig);                                 /* 结构体初始化     */
    stcRcrConfig.u32EnOverFLowMask  = ATIM3_M23_RCR_OVF_INT_EVT_CNT_MASK;   /* 屏蔽上溢重复计数 */
    stcRcrConfig.u32EnUnderFlowMask = ATIM3_M23_RCR_UND_INT_EVT_CNT_ENABLE; /* 使能下溢重复计数 */
    stcRcrConfig.u32RepeatCountNum  = 0;
    ATIM3_Mode23_SetRepeatPeriod(&stcRcrConfig); /* 间隔周期设置 */

    u16CntValue = 0;
    ATIM3_Mode23_Cnt16Set(u16CntValue); /* 设置计数初值 */

    ATIM3_IntFlagClearAll();                         /* 清中断标志 */
    ATIM3_Mode23_IntEnable(ATIM3_M23_INT_UND);       /* 使能ATIM3 中断 */
    EnableNvic(ATIM3_IRQn, IrqPriorityLevel3, TRUE); /* ATIM3 开中断 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
