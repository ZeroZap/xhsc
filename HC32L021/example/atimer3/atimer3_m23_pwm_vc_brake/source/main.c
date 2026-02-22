/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of ATIM3
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-02       MADS            First version
   2025-07-08       MADS            Add VC trim function
                                    Modify VC_NEG_SEL_CH5 Comment
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
#include "hc32l021_atim3.h"
#include "hc32l021_ddl.h"
#include "hc32l021_flash.h"
#include "hc32l021_gpio.h"
#include "hc32l021_vc.h"
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
static void ATimer3Config(uint16_t u16Period, uint16_t u16CHxACompare,
                          uint16_t u16CHxBCompare);
static void VcConfig(void);
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
volatile uint16_t u16MainLoopCnt;
volatile boolean_t bMainLoopFlag = FALSE;
volatile uint16_t u16BkDelayCnt;
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

    ATimer3Config(0x12C0, 0x900,
                  0); /* CH0/1/0x900; CH0/1/2通道B比较值互补模式不需要设置 */

    GpioConfig(); /* 端口配置 */

    VcConfig(); /* VC1比较输出刹车到Timer3 */

    ATIM3_Mode23_Run(); /* 运行 */

    /* 可添加其他其他操作，待准备好后使能PWM输出 */

    ATIM3_Mode23_ManualPWMOutputEnable(); /* 端口输出使能 */

    while (1) {
        if (bMainLoopFlag) /* 1ms延时 */
        {
            bMainLoopFlag = FALSE;

            if (TRUE == ATIM3_IntFlagGet(ATIM3_FLAG_BI)) /* 是否发生刹车事件 */
            {
                u16BkDelayCnt++;
                if (u16BkDelayCnt > 400) /* 发生Brake，延时约4S后恢复PWM输出 */
                {
                    u16BkDelayCnt = 0;
                    ATIM3_IntFlagClear(ATIM3_FLAG_BI); /* 清除刹车标志 */
                    ATIM3_Mode23_ManualPWMOutputEnable(); /* 端口输出使能 */
                }
            }
        }
    }
}

/**
 * @brief  ATIM3_中断函数
 * @retval None
 */
void Atim3_IRQHandler(void)
{
    static boolean_t bFlag = FALSE;

    /* Atimer3 模式23 更新中断 */
    if (FALSE == bFlag) {
        STK_LED_ON(); /* LED 引脚输出高电平 */
        ATIM3_Mode23_ChannelCompareValueSet(
            ATIM3_COMPARE_CAPTURE_CH0A, 0xA00); /* 设置CH0 通道A比较值 */
        ATIM3_Mode23_ChannelCompareValueSet(
            ATIM3_COMPARE_CAPTURE_CH1A, 0xA00); /* 设置CH1 通道A比较值 */
        ATIM3_Mode23_ChannelCompareValueSet(
            ATIM3_COMPARE_CAPTURE_CH2A, 0xA00); /* 设置CH2 通道A比较值 */
        bFlag = TRUE;
    } else {
        STK_LED_OFF(); /* LED 引脚输出低电平 */
        ATIM3_Mode23_ChannelCompareValueSet(
            ATIM3_COMPARE_CAPTURE_CH0A, 0x600); /* 设置CH0 通道A比较值 */
        ATIM3_Mode23_ChannelCompareValueSet(
            ATIM3_COMPARE_CAPTURE_CH1A, 0x600); /* 设置CH1 通道A比较值 */
        ATIM3_Mode23_ChannelCompareValueSet(
            ATIM3_COMPARE_CAPTURE_CH2A, 0x600); /* 设置CH2 通道A比较值 */
        bFlag = FALSE;
    }

    u16MainLoopCnt++;
    if (u16MainLoopCnt >= 5) /* 为主循环延时1ms */
    {
        u16MainLoopCnt = 0;
        bMainLoopFlag  = TRUE;
    }

    ATIM3_IntFlagClear(ATIM3_FLAG_UND); /* 清中断标志 */
}

/**
 * @brief  VC1 中断函数
 * @retval None
 */
void Vc1_IRQHandler(void)
{
    if (VC_IntFlagGet(VC_CH1) == TRUE) {
        /* 当VC高电平中断有效时，触发Timer3PWM刹车动作，MOE会自动清零 */
        /* 当MOE自动清零，PWM输出就会关断，若需要恢复PWM输出，则需要再次使能MOE(AOE不使能的情况下)
         */
        VC_IntFlagClear(VC_CH1);
    }
}

/**
 * @brief  ATIM3 CHx 端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = { 0 };

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* GPIOA外设时钟使能 */

    /*PA01设置为VC1正端输入 */
    GPIO_PA01_ANALOG_SET(); /* PA01复用为VC1正端 */

    /* PA02(ATIM3_CH1A),PA03(ATIM3_CH1B),
     * PA04(ATIM3_CH2A)PA05(ATIM3_CH2B)端口初始化 */
    GPIO_StcInit(&stcGpioInit); /* 结构体初始化 */
    stcGpioInit.u32Pin =
        GPIO_PIN_02 | GPIO_PIN_03 | GPIO_PIN_04 | GPIO_PIN_05; /* 端口引脚 */
    stcGpioInit.u32Mode   = GPIO_MD_OUTPUT_PP; /* 端口方向配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE;    /* 端口上下拉配置 */
    GPIOA_Init(&stcGpioInit);                  /* 端口初始化 */

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
static void ATimer3Config(uint16_t u16Period, uint16_t u16CHxACompare,
                          uint16_t u16CHxBCompare)
{
    uint16_t u16CntValue;
    stc_atim3_mode23_init_t stcAtim3BaseCfg         = { 0 };
    stc_atim3_m23_compare_init_t stcAtim3ChxACmpCfg = { 0 };
    stc_atim3_m23_compare_init_t stcAtim3ChxBCmpCfg = { 0 };
    stc_atim3_m23_dt_init_t stcAtim3DeadTimeCfg     = { 0 };
    stc_atim3_m23_bk_input_init_t stcBkCfg          = { 0 };
    stc_atim3_m23_rcr_init_t stcRcrCfg              = { 0 };

    SYSCTRL_PeriphClockEnable(PeriphClockAtim3); /* ATIM3 外设时钟使能 */

    ATIM3_Mode23_StcInit(&stcAtim3BaseCfg); /* 结构体初始化清零 */
    stcAtim3BaseCfg.u32WorkMode =
        ATIM3_M23_M23CR_WORK_MODE_TRIANGULAR; /* 三角波模式 */
    stcAtim3BaseCfg.u32CountClockSelect =
        ATIM3_M23_M23CR_CT_PCLK; /* 定时器功能，计数时钟为内部PCLK */
    stcAtim3BaseCfg.u32PRS = ATIM3_M23_M23CR_ATIM3CLK_PRS1; /*  PCLK */
    stcAtim3BaseCfg.u32PWMTypeSelect =
        ATIM3_M23_M23CR_COMP_PWM_COMP; /* 互补输出PWM */
    stcAtim3BaseCfg.u32PWM2sSelect =
        ATIM3_M23_M23CR_PWM2S_COMPARE_SINGLE_POINT; /* 单点比较功能 */
    stcAtim3BaseCfg.u32ShotMode = ATIM3_M23_M23CR_SHOT_CYCLE; /* 循环计数 */
    stcAtim3BaseCfg.u32URSSelect = ATIM3_M23_M23CR_URS_OV_UND; /* 上下溢更新 */
    ATIM3_Mode23_Init(&stcAtim3BaseCfg); /*  ATIM3 的模式23功能初始化 */

    /*  48M, f = 10K */
    ATIM3_Mode23_ARRSet(u16Period); /* 设置重载值,并使能缓存 */
    ATIM3_Mode23_ARRBufferEnable();

    ATIM3_Mode23_ChannelCompareValueSet(
        ATIM3_COMPARE_CAPTURE_CH0A, u16CHxACompare); /* 设置CH0比较值A */
    ATIM3_Mode23_ChannelCompareValueSet(
        ATIM3_COMPARE_CAPTURE_CH1A, u16CHxACompare); /* 设置CH1比较值A */
    ATIM3_Mode23_ChannelCompareValueSet(
        ATIM3_COMPARE_CAPTURE_CH2A, u16CHxACompare); /* 设置CH2比较值A */

    ATIM3_Mode23_CompareStcInit(&stcAtim3ChxACmpCfg); /* 结构体初始化 */
    stcAtim3ChxACmpCfg.u32CHxCompareCap = ATIM3_M23_CRCHx_CSA_CSB_COMPARE;
    stcAtim3ChxACmpCfg.u32CHxCompareModeCtrl =
        ATIM3_M23_FLTR_OCMxx_PWM_MODE_2; /* OCREFA输出控制OCMA:PWM模式2 */
    stcAtim3ChxACmpCfg.u32CHxPolarity =
        ATIM3_M23_FLTR_CCPxx_NORMAL_IN_OUT; /* 正常输出 */
    stcAtim3ChxACmpCfg.u32CHxCompareBufEn =
        ATIM3_M23_CRCHx_BUFEx_ENABLE; /* A通道缓存控制 */
    stcAtim3ChxACmpCfg.u32CHxCompareIntSelect =
        ATIM3_M23_M23CR_CISA_DISABLE_INT; /* A通道比较控制:无 */
    ATIM3_Mode23_PortOutputCHxAConfig(
        ATIM3_M23_OUTPUT_CHANNEL_CH0,
        &stcAtim3ChxACmpCfg); /* 比较输出端口配置 */
    ATIM3_Mode23_PortOutputCHxAConfig(
        ATIM3_M23_OUTPUT_CHANNEL_CH1,
        &stcAtim3ChxACmpCfg); /* 比较输出端口配置 */
    ATIM3_Mode23_PortOutputCHxAConfig(
        ATIM3_M23_OUTPUT_CHANNEL_CH2,
        &stcAtim3ChxACmpCfg); /* 比较输出端口配置 */

    ATIM3_Mode23_CompareStcInit(&stcAtim3ChxBCmpCfg); /* 结构体初始化 */
    stcAtim3ChxBCmpCfg.u32CHxCompareCap = ATIM3_M23_CRCHx_CSA_CSB_COMPARE;
    stcAtim3ChxBCmpCfg.u32CHxCompareModeCtrl =
        ATIM3_M23_FLTR_OCMxx_PWM_MODE_2; /* OCREFB输出控制OCMB:PWM模式2(PWM互补模式下也要设置，避免强制输出)
                                          */
    stcAtim3ChxBCmpCfg.u32CHxPolarity =
        ATIM3_M23_FLTR_CCPxx_NORMAL_IN_OUT; /* 正常输出 */
    stcAtim3ChxBCmpCfg.u32CHxCompareIntSelect =
        ATIM3_M23_CRCHx_CISB_DISABLE_INT; /* B通道比较控制:无 */
    ATIM3_Mode23_PortOutputCHxBConfig(
        ATIM3_M23_OUTPUT_CHANNEL_CH0,
        &stcAtim3ChxBCmpCfg); /* 比较输出端口配置 */
    ATIM3_Mode23_PortOutputCHxBConfig(
        ATIM3_M23_OUTPUT_CHANNEL_CH1,
        &stcAtim3ChxBCmpCfg); /* 比较输出端口配置 */
    ATIM3_Mode23_PortOutputCHxBConfig(
        ATIM3_M23_OUTPUT_CHANNEL_CH2,
        &stcAtim3ChxBCmpCfg); /* 比较输出端口配置 */

    ATIM3_Mode23_DTStcInit(&stcAtim3DeadTimeCfg); /* 结构体初始化 */
    stcAtim3DeadTimeCfg.bEnDeadTime      = TRUE;
    stcAtim3DeadTimeCfg.u32DeadTimeValue = 0xFF;
    ATIM3_Mode23_DTConfig(&stcAtim3DeadTimeCfg); /* 死区设置 */

    ATIM3_Mode23_RCRStcInit(&stcRcrCfg); /* 结构体初始化 */
    stcRcrCfg.u32EnOverFLowMask =
        ATIM3_M23_RCR_OVF_INT_EVT_CNT_MASK; /* 屏蔽上溢重复计数 */
    stcRcrCfg.u32EnUnderFlowMask =
        ATIM3_M23_RCR_UND_INT_EVT_CNT_ENABLE; /* 使能下溢重复计数 */
    stcRcrCfg.u32RepeatCountNum = 0;
    ATIM3_Mode23_SetRepeatPeriod(&stcRcrCfg); /* 间隔周期设置 */

    ATIM3_Mode23_BrakePortStatusConfig(
        ATIM3_M23_OUTPUT_CHANNEL_CH0, ATIM3_M23_CRCHx_BKSA_BKSB_PORT_OUTPUT_LOW,
        ATIM3_M23_CRCHx_BKSA_BKSB_PORT_OUTPUT_LOW); /* 刹车使能时，CH0端口输出低电平
                                                     */
    ATIM3_Mode23_BrakePortStatusConfig(
        ATIM3_M23_OUTPUT_CHANNEL_CH1, ATIM3_M23_CRCHx_BKSA_BKSB_PORT_OUTPUT_LOW,
        ATIM3_M23_CRCHx_BKSA_BKSB_PORT_OUTPUT_LOW); /* 刹车使能时，CH1端口输出低电平
                                                     */
    ATIM3_Mode23_BrakePortStatusConfig(
        ATIM3_M23_OUTPUT_CHANNEL_CH2, ATIM3_M23_CRCHx_BKSA_BKSB_PORT_OUTPUT_LOW,
        ATIM3_M23_CRCHx_BKSA_BKSB_PORT_OUTPUT_LOW); /* 刹车使能时，CH2端口输出低电平
                                                     */

    ATIM3_Mode23_BKInputStcInit(&stcBkCfg); /* 结构体初始化 */
    stcBkCfg.u32EnVCBrake = ATIM3_M23_DTR_VCE_BRAKE_ENABLE; /* 使能VC刹车 */
    stcBkCfg.u32EnBrake = ATIM3_M23_DTR_BKE_BRAKE_DISABLE; /* PORT 刹车禁止 */
    ATIM3_Mode23_BrakeInputConfig(&stcBkCfg); /* 刹车功能配置 */

    u16CntValue = 0;
    ATIM3_Mode23_Cnt16Set(u16CntValue); /* 设置计数初值 */

    ATIM3_IntFlagClearAll();                   /* 清中断标志 */
    ATIM3_Mode23_IntEnable(ATIM3_M23_INT_UND); /* 使能ATIM3 UEV更新中断 */
    EnableNvic(ATIM3_IRQn, IrqPriorityLevel3, TRUE); /* ATIM3 开中断 */
}

/**
 * @brief  VC1 初始化
 * @retval None
 */
static void VcConfig(void)
{
    stc_vc_init_t stcVcInit = { 0 };

    SYSCTRL_PeriphClockEnable(PeriphClockVc); /* 开启VC时钟 */

    VC_StcInit(&stcVcInit);                         /* 结构体初始化 */
    stcVcInit.u32Ch      = VC_CH1;                  /* 使用VC1 */
    stcVcInit.u32HysVolt = VC_HYSTERESIS_VOLT_10MV; /* 迟滞电压约为10mV */
    stcVcInit.u32BiasCurrent = VC_BIAS_CURR_MEDIUM; /* 中功耗 */
    stcVcInit.u32FilterTime =
        VC_FILTER_TIME_16CYCLE; /* 滤波时间设置16个滤波时钟周期 */
    stcVcInit.u32InputPos = VC_POS_SEL_CH0; /* 电压比较器的P端连接PA01 */
    stcVcInit.u32InputNeg = VC_NEG_SEL_CH5; /* 电压比较器的N端连接内核VCAP */
    stcVcInit.u32FilterClk  = VC_FILTER_CLK_RC256K; /* 滤波时钟RC256K */
    stcVcInit.u32FilterMode = VC_FILTER_MD_NFM;     /* 普通滤波模式 */
    stcVcInit.u32OutputPol  = VC_OUTPUT_POL_POS;    /* 输出极性正 */
    stcVcInit.u32IntMode    = VC_INT_MD_RISING;     /* 上升沿中断 */
    stcVcInit.u8TrimSetSelect =
        VC_TRIM_LOWER_0P5AVCC; /* 正端或者负端的参考电压小于等于1/2 AVCC */
    VC_Init(&stcVcInit);

    VC_IntFlagClear(VC_CH1); /* 清除VC1中断标志 */
    VC_IntEnable(VC_CH1);    /* VC1中断使能 */
    EnableNvic(VC1_IRQn, IrqPriorityLevel1, TRUE);

    VC_OutputAtimConfig(
        VC_CH1, VC_OUTPUT_ATM_BK); /* VC1滤波后信号输出到ATIM0/3 刹车使能控制 */

    /* VC使能 */
    VC_Enable(VC_CH1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
