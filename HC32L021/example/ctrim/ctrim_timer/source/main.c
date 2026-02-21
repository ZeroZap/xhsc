/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of  CTRIM
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
#include "ctrim.h"
#include "ddl.h"
#include "flash.h"
#include "gpio.h"
#include "lpm.h"
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
static void CtrimTimerConfig(void);
static void CtrimClockConfig(void);
static void GpioLowPowerConfig(void);
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
    STK_LedConfig(); /* LED端口配置 */

    STK_UserKeyConfig(); /* USER KEY端口配置 */

    CtrimClockConfig(); /* 时钟初始化 */

    CtrimTimerConfig(); /* CTRIM 定时初始化 */

    /* =============================================== */
    /* ==============       WARNING       ============ */
    /* =============================================== */
    /* 本样例程序会进入深度休眠模式，因此以下两行代码起防护作用*/
    /* （防止进入深度休眠后芯片调试功能不能再次使用） */
    /* 在使用本样例时，禁止在没有唤醒机制的情况下删除以下两行代码 */
    DDL_Delay1ms(2000);

    while (FALSE == STK_USER_KEY_PRESSED()) /* 等待按键按下后进入休眠模式 */
    {
        ;
    }
    DDL_Delay1ms(200);

    CTRIM_Enable(); /* 使能CTRIM模块，开启校验 */

    FLASH_LowPowerEnable(); /* 配置FLASH为低功耗模式 */

    GpioLowPowerConfig(); /* 配置Demo板上所有不使用的IO为输入下拉,避免端口漏电 */

    LPM_GotoDeepSleep(TRUE); /* 进入低功耗模式——深度休眠（使能唤醒后退出中断自动休眠特性） */

    while (1)
    {
        ;
    }
}

/**
 * @brief  CTRIM 中断服务程序
 * @retval None
 */
void Ctrim_Clkdet_IRQHandler(void)
{
    static boolean_t bFlag = FALSE;

    if (TRUE == CTRIM_FlagGet(CTRIM_FLAG_UD))
    {
        CTRIM_FlagClear(CTRIM_FLAG_UD); /* 清除标志位 */

        /* LED灯按定时时间切换输出ON/OFF */
        if (TRUE == bFlag)
        {
            STK_LED_OFF(); /* 关闭LED */
            bFlag = FALSE;
        }
        else
        {
            STK_LED_ON(); /* 开启LED */
            bFlag = TRUE;
        }
    }
}

/**
 * @brief  时钟配置
 * @retval None
 */
static void CtrimClockConfig(void)
{
    SYSCTRL_XTLDrvConfig(SYSCTRL_XTL_AMP3, SYSCTRL_XTL_DRV3); /* 设置XTL晶振参数，使能目标时钟，SYSTEM_XTL = 32768Hz */
    SYSCTRL_XTLStableTimeSet(SYSCTRL_XTL_STB_CYCLE16384);     /* 置XTL稳定等待时间 */
    SYSCTRL_ClockSrcEnable(SYSCTRL_CLK_SRC_XTL);              /* 使能XTL时钟 */
}

/**
 * @brief  CTRIM定时初始化
 * @retval None
 */
static void CtrimTimerConfig(void)
{
    stc_ctrim_timer_init_t stcCtrimInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockCtrim); /* 开启CTRIM 外设时钟 */
    SYSCTRL_PeriphReset(PeriphResetCtrim);       /* 复位CTRIM模块 */

    /* CTRIM 初始化配置 */
    CTRIM_TimerStcInit(&stcCtrimInit);                      /* 结构体变量初始值初始化 */
    stcCtrimInit.u32Clock       = CTRIM_ACCURATE_CLOCK_XTL; /* 时钟源选择XTL 32768Hz */
    stcCtrimInit.u32ClockDiv    = CTRIM_REF_CLOCK_DIV2;     /* 分频2 */
    stcCtrimInit.u16ReloadValue = 16384 - 1;                /* 定时约1s，32768/2 = 16384 */
    CTRIM_TimerInit(&stcCtrimInit);                         /* CTRIM Timer初始化 */

    CTRIM_FlagClearALL(); /* 清除所有标志位 */

    CTRIM_IntEnable(CTRIM_INT_UD);                          /* 计数器下溢中断 使能 */
    EnableNvic(CTRIM_CLKDET_IRQn, IrqPriorityLevel3, TRUE); /* 使能并配置CTRIM 系统中断 */
}

/**
 * @brief  GPIO低功耗配置
 * @retval None
 */
static void GpioLowPowerConfig(void)
{
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);  /* 打开GPIO外设时钟门控 */
    SYSCTRL_FuncEnable(SYSCTRL_FUNC_SWD_USE_IO); /* SWD切换为GPIO */

    GPIOA->ADS = 0xEFFEu; /* 配置为模拟端口(除按键、LED端口外)*/
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
