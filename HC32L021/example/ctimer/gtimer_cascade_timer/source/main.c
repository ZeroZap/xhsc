/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of GTIM
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

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "atim3.h"
#include "ddl.h"
#include "gpio.h"
#include "gtim.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void Gtim0Config(void);
static void Atim3Config(void);
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    STK_LedConfig(); /* LED初始化 */

    Gtim0Config(); /* GTIM0初始化 */

    Atim3Config(); /* ATIM3初始化 */

    ATIM3_Mode23_Run(); /* ATIM3运行 */

    GTIM_Enable(GTIM0); /* 启动GTIM0运行 */

    while (1)
    {
        ;
    }
}

/**
 * @brief  GTIM0中断服务函数
 * @retval None
 */
void Ctim0_IRQHandler(void)
{
    static boolean_t bFlag = FALSE;

    if (TRUE == GTIM_IntFlagGet(GTIM0, GTIM_FLAG_UI))
    {
        if (FALSE == bFlag)
        {
            STK_LED_ON(); /* 开启LED */

            bFlag = TRUE;
        }
        else if (TRUE == bFlag)
        {
            STK_LED_OFF(); /* 关闭LED */

            bFlag = FALSE;
        }
        else
        {
            ;
        }

        GTIM_IntFlagClear(GTIM0, GTIM_FLAG_UI); /* 清除GTIM0的溢出中断标志位 */
    }
}

/**
 * @brief  初始化GTIM0
 * @retval None
 */
static void Gtim0Config(void)
{
    stc_gtim_init_t stcGtimInit = {0};

    SYSCTRL_FuncDisable(SYSCTRL_FUNC_CTIMER0_USE_BTIM); /* 配置GTIM0有效，BTIM0/1/2无效 */
    SYSCTRL_PeriphClockEnable(PeriphClockCtim0);        /* 使能CTIM0外设时钟 */

    GTIM_StcInit(&stcGtimInit);                                    /* 结构体变量初始值初始化 */
    stcGtimInit.u32OneShotEn           = GTIM_CONTINUOUS_COUNTER;  /* 连续计数模式 */
    stcGtimInit.u32Mode                = GTIM_MD_EXTERN;           /* 工作模式: 计数器模式，计数时钟源来自TRS */
    stcGtimInit.u32Prescaler           = GTIM_COUNTER_CLK_DIV1;    /* 对计数时钟进行预除频 */
    stcGtimInit.u32ToggleEn            = GTIM_TOGGLE_DISABLE;      /* TOG输出禁止 */
    stcGtimInit.u32TriggerSrc          = GTIM_TRIG_SRC_ITR3;       /* 触发源选择，TRS触发源未来自ITR3:ATIM3_TRGO */
    stcGtimInit.u32ExternInputPolarity = GTIM_ETR_POLARITY_NORMAL; /* 外部ETP输入极性选择，此样例用于内部PCLK计数，不需要配置 */
    stcGtimInit.u32AutoReloadValue     = 20 - 1;                   /* 自动重载寄存ARR赋值,计数周期为PRS*(ARR+1)*TPCLK */
    GTIM_Init(GTIM0, &stcGtimInit);

    GTIM_IntFlagClear(GTIM0, GTIM_FLAG_UI);          /* 清除溢出中断标志位 */
    GTIM_IntEnable(GTIM0, GTIM_INT_UI);              /* 允许GTIM0溢出中断 */
    EnableNvic(CTIM0_IRQn, IrqPriorityLevel3, TRUE); /* 开启中断 */
}

/**
 * @brief  Atim3 配置
 * @retval None
 */
static void Atim3Config(void)
{
    stc_atim3_mode23_init_t           stcAtim3BaseConfig = {0};
    stc_atim3_m23_master_slave_init_t stcMasterConfig    = {0};

    /* ATIM3 外设时钟使能 */
    SYSCTRL_PeriphClockEnable(PeriphClockAtim3); /* 使能ATIM3 外设时钟 */

    ATIM3_Mode23_StcInit(&stcAtim3BaseConfig);                                   /* 结构体变量初始值初始化 */
    ATIM3_Mode23_MasterSlaveStcInit(&stcMasterConfig);                           /* 结构体变量初始值初始化 */
    stcAtim3BaseConfig.u32WorkMode         = ATIM3_M23_M23CR_WORK_MODE_SAWTOOTH; /* 定时器模式 */
    stcAtim3BaseConfig.u32CountClockSelect = ATIM3_M23_M23CR_CT_PCLK;            /* 定时器功能，计数时钟为内部PCLK */
    stcAtim3BaseConfig.u32PRS              = ATIM3_M23_M23CR_ATIM3CLK_PRS1;      /* PCLK/1 */
    stcAtim3BaseConfig.u32ShotMode         = ATIM3_M23_M23CR_SHOT_CYCLE;         /* 循环计数 */
    stcAtim3BaseConfig.u32CountDir         = ATIM3_M23_M23CR_DIR_UP_CNT;         /* 向上计数 */
    stcAtim3BaseConfig.u32URSSelect        = ATIM3_M23_M23CR_URS_OV_UND_UG_RST;  /* 更新源选择0 */
    ATIM3_Mode23_Init(&stcAtim3BaseConfig);                                      /* ATIM3 的模式23功能初始化 */

    ATIM3_Mode23_ARRSet(0xFFFFu); /* 设置重载值 */
    ATIM3_Mode23_Cnt16Set(0);     /* 设置计数初值 */

    stcMasterConfig.u32MasterSrc = ATIM3_M23_MSCR_MMS_TRIG_SOURCE_UEV; /* 定时器事件更新UEV */
    ATIM3_Mode23_MasterSlaveTrigSet(&stcMasterConfig);                 /* ATIM3 模式23 主从触发配置 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
