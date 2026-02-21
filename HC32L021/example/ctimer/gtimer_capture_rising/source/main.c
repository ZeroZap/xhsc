/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of GTIM
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-03       MADS            First version
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
volatile static uint32_t u32CaptureValue_0;
volatile static uint32_t u32CaptureValue_1;
volatile static uint32_t u32CaptureValue_Result;
volatile static uint32_t u32CaptureValue_OldResult;
static uint32_t          u32OverCntValue;
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void GpioConfig(void);
static void Gtim0Config(void);
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    STK_LedConfig(); /* LED 初始化 */

    STK_UserKeyConfig(); /* KEY 初始化 */

    GpioConfig(); /* GPIO初始化 */

    Gtim0Config(); /* GTIM0初始化 */

    while (FALSE == STK_USER_KEY_PRESSED())
    {
        ; /* 等待USER KEY按下 */
    }
    DDL_Delay1ms(200);

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
    static uint32_t u32index = 0;

    uint32_t u32Temp = 0;

    if (TRUE == GTIM_IntFlagGet(GTIM0, GTIM_FLAG_UI))
    {
        GTIM_IntFlagClear(GTIM0, GTIM_INT_UI); /* 清除GTIM0的溢出中断标志位 */

        u32OverCntValue += 0x10000;
    }

    if (TRUE == GTIM_IntFlagGet(GTIM0, GTIM_FLAG_CC0))
    {
        GTIM_IntFlagClear(GTIM0, GTIM_INT_CC0); /* 清除GTIM0的捕获比较0中断标志位 */

        if (u32index == 0) /* 第一次检测到上升沿, 起始计数 */
        {
            u32index = 1;

            u32CaptureValue_0 = GTIM_CompareCaptureRegGet(GTIM0, GTIM_COMPARE_CAPTURE_CH0);
            u32OverCntValue   = 0;

            STK_LED_ON(); /* 点亮LED */
        }
        else /* 第二次检测到上升沿, 结束计数,计算两次上升沿之间的值 */
        {
            u32CaptureValue_1 = GTIM_CompareCaptureRegGet(GTIM0, GTIM_COMPARE_CAPTURE_CH0);

            u32Temp = u32CaptureValue_1 + u32OverCntValue;

            u32CaptureValue_Result = u32Temp - u32CaptureValue_0;

            if (u32CaptureValue_Result > 0x80000)
            {
                u32CaptureValue_Result = 0;
            }

            u32CaptureValue_OldResult = u32CaptureValue_Result;

            u32OverCntValue = 0;

            STK_LED_OFF(); /* 关闭LED */

            u32index = 0;
        }
    }
}

/**
 * @brief  端口配置初始化
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcChxGpioInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* 开启GPIO时钟 */

    /* CTIM0_CH0(PA02)端口初始化 */
    GPIO_StcInit(&stcChxGpioInit);             /* 结构体变量初始值初始化 */
    stcChxGpioInit.u32Mode   = GPIO_MD_INPUT;  /* 端口方向配置 */
    stcChxGpioInit.u32PullUp = GPIO_PULL_NONE; /* 端口上拉配置 */
    stcChxGpioInit.u32Pin    = GPIO_PIN_02;    /* 端口引脚配置 */
    GPIOA_Init(&stcChxGpioInit);               /* GPIO IO初始化 */
    GPIO_PA02_AF_CTIM0_CH0();                  /* CTIM0_CH0端口复用功能 */
}

/**
 * @brief  初始化GTIMER
 * @retval None
 */
static void Gtim0Config(void)
{
    stc_gtim_init_t stcGtimInit = {0};

    SYSCTRL_FuncDisable(SYSCTRL_FUNC_CTIMER0_USE_BTIM); /* 配置GTIM0有效，BTIM0/1/2无效 */
    SYSCTRL_PeriphClockEnable(PeriphClockCtim0);        /* 使能CTIM0 外设时钟 */

    GTIM_StcInit(&stcGtimInit);                               /* 结构体变量初始值初始化 */
    stcGtimInit.u32Mode            = GTIM_MD_PCLK;            /* 工作模式: 定时器模式，计数时钟源来自PCLK */
    stcGtimInit.u32OneShotEn       = GTIM_CONTINUOUS_COUNTER; /* 计数模式：连续计数 */
    stcGtimInit.u32Prescaler       = GTIM_COUNTER_CLK_DIV1;   /* 对计数时钟进行预除频 */
    stcGtimInit.u32ToggleEn        = GTIM_TOGGLE_DISABLE;     /* TOG输出禁止 */
    stcGtimInit.u32AutoReloadValue = 0xFFFF;                  /* 自动重载寄存ARR赋值,计数周期为PRS*(ARR+1)*TPCLK */
    GTIM_Init(GTIM0, &stcGtimInit);                           /* GTIM0初始化 */

    GTIM_CompareCaptureModeSet(GTIM0, GTIM_COMPARE_CAPTURE_CH0, GTIM_COMPARE_CAPTURE_RISING);

    GTIM_IntFlagClear(GTIM0, GTIM_INT_UI | GTIM_INT_CC0); /* 清除溢出中断和捕获比较0中断标志位 */
    GTIM_IntEnable(GTIM0, GTIM_INT_UI | GTIM_INT_CC0);    /* 允许GTIM0溢出中断 */
    EnableNvic(CTIM0_IRQn, IrqPriorityLevel3, TRUE);      /* 开启中断 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
