/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of CTRIM
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
#include "sysctrl.h"
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
static void CtrimClockConfig(void);
static void CtrimCalibConfig(void);
static void GpioConfig(void);
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
    uint32_t u32TrimTVAL = 0u;

    CtrimClockConfig(); /* 时钟初始化 */

    DDL_Delay1ms(2000);                          /* 必须等待一段时间，再切换PA14从SWD为GPIO模式，否则将无法下载程序 */
    SYSCTRL_FuncEnable(SYSCTRL_FUNC_SWD_USE_IO); /* 切换PA14从SWD为GPIO模式 */

    STK_LedConfig(); /* LED初始化 */

    STK_UserKeyConfig(); /* KEY初始化 */

    GpioConfig(); /* CTRIM 端口初始化 */

    while (FALSE == STK_USER_KEY_PRESSED()) /* 等待 USER KEY 按下 */
    {
        ;
    }

    DDL_Delay1ms(200);

    CtrimCalibConfig(); /* CTRIM 校准功能初始化 */

    CTRIM_Enable(); /* 使能CTRIM模块，开启校验 */

    while (1)
    {
        if (TRUE == CTRIM_FlagGet(CTRIM_FLAG_END)) /* 判断是否收到自动CTRIM结束标志位 */

        {
            CTRIM_FlagClear(CTRIM_FLAG_END); /* 清除标志位 */

            STK_LED_ON(); /* 点亮LED，校验成功 */

            /* 用户根据需要将自动CTRIM结果CTRIM->TVAL寄存器的值写入SYSCTRL->RC48M_CR的TRIMF与TRIMC寄存器 */
            u32TrimTVAL = CTRIM_TrimCodeGet();
            MODIFY_REG(SYSCTRL->RC48M_CR, SYSCTRL_RC48M_CR_TRIMC_Msk | SYSCTRL_RC48M_CR_TRIMF_Msk, u32TrimTVAL << SYSCTRL_RC48M_CR_TRIMF_Pos);

            /* 用户根据需要关闭CTRIM使能并复位 */
            CTRIM_Disable();                              /* 关闭CTRIM */
            SYSCTRL_PeriphReset(PeriphResetCtrim);        /* CTRIM复位 */
            SYSCTRL_PeriphClockDisable(PeriphClockCtrim); /* 关闭CTRIM时钟 */
        }
    }
}

/**
 * @brief  端口相关配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* 打开GPIO外设时钟门控 */

    /* LED端口、RC48M输出端口初始化 */
    GPIO_StcInit(&stcGpioInit);                 /* 结构体变量初始值初始化 */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* 打开GPIO外设时钟门控 */
    stcGpioInit.u32Mode   = GPIO_MD_OUTPUT_PP;  /* 端口方向配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE;     /* 端口上拉配置 */
    stcGpioInit.u32Pin    = GPIO_PIN_14;        /* 端口引脚配置 */
    GPIOA_Init(&stcGpioInit);                   /* GPIO端口初始化 */
    GPIO_PA14_AF_TCLK_OUT_RC48M_DIV1();         /* 端口复用功能配置 */
}

/**
 * @brief  时钟配置
 * @retval None
 */
static void CtrimClockConfig(void)
{
    /* 设置XTL晶振参数，使能目标时钟，SYSTEM_XTL = 32768Hz */
    SYSCTRL_XTLDrvConfig(SYSCTRL_XTL_AMP2, SYSCTRL_XTL_DRV2); /* XTL驱动能力配置 */
    SYSCTRL_XTLStableTimeSet(SYSCTRL_XTL_STB_CYCLE32768);     /* 设置等待XTL时钟稳定时间 */
    SYSCTRL_ClockSrcEnable(SYSCTRL_CLK_SRC_XTL);              /* 使能XTL时钟 */
    SYSCTRL->RC48M_CR_f.TRIMC = 1; /* 待校准的RC48M,注：任意设置值，可以更明显表现校准过程，仅作本样例使用 */
    SYSCTRL->RC48M_CR_f.TRIMF = 1; /* 待校准的RC48M,注：任意设置值，可以更明显表现校准过程，仅作本样例使用 */
}

/**
 * @brief  CTRIM校准初始化
 * @retval None
 */
static void CtrimCalibConfig(void)
{
    stc_ctrim_calib_init_t stcCtrimInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockCtrim); /* 开启CTRIM 外设时钟 */
    SYSCTRL_PeriphReset(PeriphResetCtrim);       /* 复位CTRIM模块 */

    /* CTRIM 初始化配置 */
    CTRIM_CalibStcInit(&stcCtrimInit);                           /* 结构体变量初始值初始化 */
    stcCtrimInit.u32Mode            = CTRIM_AUTO_CALIB_MD_RC48M; /* 配置为校准RC48M模式 */
    stcCtrimInit.u32AccurateClock   = CTRIM_ACCURATE_CLOCK_XTL;  /* 配置XTL精准时钟源 */
    stcCtrimInit.u32RefClockDiv     = CTRIM_REF_CLOCK_DIV128;    /* 128分频 */
    stcCtrimInit.u32OneShot         = CTRIM_ONE_SHOT_SINGLE;     /* 单次校准 */
    stcCtrimInit.u32InitStep        = CTRIM_INIT_STEP_32;        /* 初始步进量为32 */
    stcCtrimInit.u16ReloadValue     = 15625u - 1u;               /* 一个GCLK的计数周期目标计数值 = 4000000/256 = 15625 */
    stcCtrimInit.u16CountErrorLimit = 78u;                       /* 校准精度设置为0.5%，FLIM = 15625*0.5% = 78 */
    CTRIM_CalibInit(&stcCtrimInit);                              /* CTRIM校准初始化 */

    CTRIM_FlagClearALL(); /* 清除所有标志位 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
