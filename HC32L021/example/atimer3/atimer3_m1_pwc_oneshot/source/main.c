/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of ATIM3
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-02       MADS            First version
   2025-07-08       MADS            Modify Atim3_IRQHandler()
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
static void Atimer3Config(void);
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
volatile uint32_t u32PwcCapValue;
volatile uint16_t u16ATIM3CntValue;
static uint16_t u16ATIM3OverFlowCnt;
static uint16_t u16ATIM3CapValue;
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    GpioConfig(); /* 端口配置 */

    Atimer3Config(); /* Atimer3 配置 */

    ATIM3_Mode1_Run(); /* Atimer3运行 */

    while (1) {
        DDL_Delay1ms(2000);
        ATIM3_Mode1_Run(); /* Atimer3运行 */
    }
}

/**
 * @brief  ATim3中断函数
 * @retval None
 */
void Atim3_IRQHandler(void)
{
    /* Atimer3 模式1 计数溢出中断 */
    if (ATIM3_IntFlagGet(ATIM3_FLAG_UI)) {
        u16ATIM3OverFlowCnt++; /* 计数脉宽测量开始边沿到结束边沿过程中timer的溢出次数
                                */
        ATIM3_IntFlagClear(ATIM3_FLAG_UI); /* 清除中断标志 */
    }

    /* Atimer3 模式1 PWC脉宽测量中断 */
    if (ATIM3_IntFlagGet(ATIM3_FLAG_PWC_CA0)) {
        u16ATIM3CntValue = ATIM3_Mode1_Cnt16Get(); /* 读取当前计数值 */
        u16ATIM3CapValue = ATIM3_Mode1_PWCCapValueGet(); /* 读取脉宽测量值 */

        u32PwcCapValue = u16ATIM3OverFlowCnt * 0x10000 + u16ATIM3CapValue;

        u16ATIM3OverFlowCnt = 0;

        ATIM3_IntFlagClear(ATIM3_FLAG_PWC_CA0); /* 清除中断标志 */
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

    /* PA11(ATIM3_CH0A) 端口初始化 */
    GPIO_StcInit(&stcGpioInit);          /* 结构体初始化 */
    stcGpioInit.u32Pin  = GPIO_PIN_11;   /* 端口引脚 */
    stcGpioInit.u32Mode = GPIO_MD_INPUT; /* 端口方向配置 */
    GPIOA_Init(&stcGpioInit);            /* 端口初始化 */
    GPIO_PA11_AF_ATIM3_CH0A();           /* 端口复用配置 */
}
/**
 * @brief  ATIM3 配置
 * @retval None
 */
static void Atimer3Config(void)
{
    uint16_t u16CntValue;
    stc_atim3_mode1_init_t stcAtim3BaseCfg      = { 0 };
    stc_atim3_pwc_input_init_t stcAtim3PwcInCfg = { 0 };

    SYSCTRL_PeriphClockEnable(PeriphClockAtim3); /* ATIM3 外设时钟使能 */

    ATIM3_Mode1_StcInit(&stcAtim3BaseCfg); /* 结构体初始化清零 */
    stcAtim3BaseCfg.u32WorkMode = ATIM3_M1_M1CR_WORK_MODE_PWC; /* 定时器模式 */
    stcAtim3BaseCfg.u32CountClockSelect =
        ATIM3_M1_M1CR_CT_PCLK; /* 定时器功能，计数时钟为内部PCLK */
    stcAtim3BaseCfg.u32PRS      = ATIM3_M1_M1CR_ATIM3CLK_PRS4; /* PCLK/4 */
    stcAtim3BaseCfg.u32ShotMode = ATIM3_M1_M1CR_SHOT_ONE; /* PWC单次检测 */
    ATIM3_Mode1_Init(&stcAtim3BaseCfg);

    ATIM3_Mode1_InputStcInit(&stcAtim3PwcInCfg); /* 结构体初始化清零 */
    stcAtim3PwcInCfg.u32TsSelect =
        ATIM3_M1_MSCR_TS_CH0A_FILTER; /* PWC输入选择 CHA */
    stcAtim3PwcInCfg.u32IA0Select = ATIM3_M1_MSCR_IA0S_CH0A; /* CHA选择IA0 */
    stcAtim3PwcInCfg.u32FltIA0 =
        ATIM3_M1_FLTR_FLTA0_B0_ET_3_PCLK_DIV4_4; /* PCLK/4 4个连续有效 */
    ATIM3_Mode1_InputConfig(&stcAtim3PwcInCfg);  /* PWC输入设置 */

    ATIM3_Mode1_PWCEdgeSelect(
        ATIM3_M1_M1CR_DETECT_EDGE_FALL_TO_FALL); /* 下降沿到下降沿脉宽测量 */

    u16CntValue = 0;
    ATIM3_Mode1_Cnt16Set(u16CntValue); /* 设置计数初值 */

    ATIM3_IntFlagClear(ATIM3_FLAG_UI
                       | ATIM3_FLAG_PWC_CA0); /* 清Uev中断、捕捉中断标志 */
    ATIM3_Mode1_IntEnable(ATIM3_M1_INT_UI
                          | ATIM3_M1_INT_CA0);       /* 使能ATIM3 中断 */
    EnableNvic(ATIM3_IRQn, IrqPriorityLevel3, TRUE); /* ATIM3 开中断 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
