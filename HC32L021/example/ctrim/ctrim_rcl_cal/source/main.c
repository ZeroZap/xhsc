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
    STK_LedConfig(); /* LED端口配置 */

    STK_UserKeyConfig(); /* USER KEY端口配置 */

    SYSCTRL_ClockSrcEnable(SYSCTRL_CLK_SRC_RCL_32768); /* 使能RCL时钟 */

    DDL_Delay1ms(2000);                          /* 必须等待一段时间，再切换PA14从SWD为GPIO模式，否则将无法下载程序 */
    SYSCTRL_FuncEnable(SYSCTRL_FUNC_SWD_USE_IO); /* 切换PA14从SWD为GPIO模式 */

    GpioConfig(); /* CTRIM端口初始化 */

    while (FALSE == STK_USER_KEY_PRESSED()) /* 等待 USER KEY按下 */
    {
        ;
    }

    DDL_Delay1ms(200);

    CtrimCalibConfig(); /* CTRIM 校准功能初始化 */

    CTRIM_Enable(); /* 使能CTRIM模块，开启校验 */

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
    uint32_t u32TrimTVAL = 0;

    if (TRUE == CTRIM_FlagGet(CTRIM_FLAG_END)) /* 判断是否收到自动CTRIM结束标志位 */

    {
        CTRIM_FlagClear(CTRIM_FLAG_END); /* 清除标志位 */

        STK_LED_ON(); /* 点亮LED，校验成功 */

        /* 用户根据需要将自动CTRIM结果CTRIM->TVAL寄存器的值写入SYSCTRL->RCL_CR的TRIM */
        u32TrimTVAL = CTRIM_TrimCodeGet();                                 /* 获取自动CTRIM结果 */
        MODIFY_REG(SYSCTRL->RCL_CR, SYSCTRL_RCL_CR_TRIM_Msk, u32TrimTVAL); /* 自动CTRIM结果写入寄存器 */

        /* 用户根据需要关闭CTRIM使能并复位 */
        CTRIM_Disable();                              /* 关闭CTRIM */
        SYSCTRL_PeriphReset(PeriphResetCtrim);        /* 复位CTRIM */
        SYSCTRL_PeriphClockDisable(PeriphClockCtrim); /* 关闭CTRIM时钟 */
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

    /* RCL输出端口初始化 */
    GPIO_StcInit(&stcGpioInit);                /* 结构体变量初始值初始化 */
    stcGpioInit.u32Mode   = GPIO_MD_OUTPUT_PP; /* 端口方向配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE;    /* 端口上拉配置 */
    stcGpioInit.u32Pin    = GPIO_PIN_14;       /* 端口引脚配置 */
    GPIOA_Init(&stcGpioInit);                  /* GPIO端口初始化 */
    GPIO_PA14_AF_TCLK_OUT_RCL_DIV1();          /* 端口复用功能配置 */

    /* CTRIM_ETRTOG端口初始化 */
    GPIO_StcInit(&stcGpioInit);             /* 结构体变量初始值初始化 */
    stcGpioInit.u32Mode   = GPIO_MD_INPUT;  /* 端口方向配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE; /* 端口上拉配置 */
    stcGpioInit.u32Pin    = GPIO_PIN_11;    /* 端口引脚配置 */
    GPIOA_Init(&stcGpioInit);               /* GPIO端口初始化 */
    GPIO_PA11_AF_CTRIM_ETRTOG();            /* 端口复用功能配置 */
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
    CTRIM_CalibStcInit(&stcCtrimInit);                          /* 结构体变量初始值初始化 */
    stcCtrimInit.u32Mode            = CTRIM_AUTO_CALIB_MD_RCL;  /* 配置为校准RCL模式 */
    stcCtrimInit.u32AccurateClock   = CTRIM_ACCURATE_CLOCK_ETR; /* 配置外部输入精准时钟源 */
    stcCtrimInit.u32RefClockDiv     = CTRIM_REF_CLOCK_DIV1024;  /* GCLK频率为 32768/1024 = 32Hz*/
    stcCtrimInit.u32OneShot         = CTRIM_ONE_SHOT_SINGLE;    /* 单次校准 */
    stcCtrimInit.u32InitStep        = CTRIM_INIT_STEP_16;       /* 初始步进量为16 */
    stcCtrimInit.u16ReloadValue     = 31250u - 1u;              /* 一个GCLK的计数周期目标计数值 = 1000000/32 = 31250 */
    stcCtrimInit.u16CountErrorLimit = 156u;                     /* 校准精度设置为0.5%，FLIM = 31250*0.5% = 156 */
    CTRIM_CalibInit(&stcCtrimInit);

    CTRIM_FlagClearALL(); /* 清除所有标志位 */

    /* 使能中断 */
    CTRIM_IntEnable(CTRIM_INT_END);                         /* 打开CTRIM中断使能  */
    EnableNvic(CTRIM_CLKDET_IRQn, IrqPriorityLevel3, TRUE); /* 使能并配置CTRIM系统中断 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
