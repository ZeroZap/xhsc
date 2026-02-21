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
static void Atimer3Config(void);
static void EncoderSimulation(void);
static void LedDisplay(void);
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
volatile uint16_t u16ATIMCnt;
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    stc_atim3_m23_master_slave_init_t stcTim3MSCfg;

    STK_LedConfig(); /*板载LED配置 */

    GpioConfig(); /* 端口配置 */

    Atimer3Config(); /* Atimer3 配置 */

    /* 正交编码计数模式1 */
    ATIM3_Mode23_MasterSlaveStcInit(&stcTim3MSCfg);
    stcTim3MSCfg.u32SlaveModeSelect = ATIM3_M23_MSCR_SMS_QUADRATURE_M1;
    ATIM3_Mode23_MasterSlaveTrigSet(&stcTim3MSCfg);

    ATIM3_Mode23_Cnt16Set(0u); /* Cnt清零 */
    ATIM3_Mode23_Run();        /* 运行 */
    EncoderSimulation();       /* 信号输入并计数 */
    ATIM3_Mode23_Stop();       /* 停止 */
    GPIO_PA03_RESET();         /* PA3清零 */
    GPIO_PA04_RESET();         /* PA4清零 */
    DDL_Delay1ms(1000);

    /* 正交编码计数模式2 */
    ATIM3_Mode23_MasterSlaveStcInit(&stcTim3MSCfg);
    stcTim3MSCfg.u32SlaveModeSelect = ATIM3_M23_MSCR_SMS_QUADRATURE_M2;
    ATIM3_Mode23_MasterSlaveTrigSet(&stcTim3MSCfg);

    ATIM3_Mode23_Cnt16Set(0u); /* Cnt清零 */
    ATIM3_Mode23_Run();        /* 运行 */
    EncoderSimulation();       /* 信号输入并计数 */
    ATIM3_Mode23_Stop();       /* 停止 */
    GPIO_PA03_RESET();         /* PA3清零 */
    GPIO_PA04_RESET();         /* PA4清零 */
    DDL_Delay1ms(1000);

    /* 正交编码计数模式3 */
    ATIM3_Mode23_MasterSlaveStcInit(&stcTim3MSCfg);
    stcTim3MSCfg.u32SlaveModeSelect = ATIM3_M23_MSCR_SMS_QUADRATURE_M3;
    ATIM3_Mode23_MasterSlaveTrigSet(&stcTim3MSCfg);

    ATIM3_Mode23_Cnt16Set(0u); /* Cnt清零 */
    ATIM3_Mode23_Run();        /* 运行 */
    EncoderSimulation();       /* 信号输入并计数 */
    ATIM3_Mode23_Stop();       /* 停止 */
    GPIO_PA03_RESET();         /* PA3清零 */
    GPIO_PA04_RESET();         /* PA4清零 */
    DDL_Delay1ms(1000);

    while (1)
    {
        ;
    }
}

/**
 * @brief  ATIM3 CHx 端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* GPIOA外设时钟使能 */

    /* PA03(模拟正交信号A)和PA04(模拟正交信号B)端口初始化 */
    GPIO_StcInit(&stcGpioInit);                        /* 结构体初始化 */
    stcGpioInit.u32Pin    = GPIO_PIN_03 | GPIO_PIN_04; /* 端口引脚 */
    stcGpioInit.u32Mode   = GPIO_MD_OUTPUT_PP;         /* 端口方向配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE;            /* 端口上下拉配置 */
    GPIOA_Init(&stcGpioInit);                          /* 端口初始化 */

    /* PA11(ATIM3_CH0A)和PA01(ATIM3_CH0B)端口初始化 */
    GPIO_StcInit(&stcGpioInit);                        /* 结构体初始化 */
    stcGpioInit.u32Pin    = GPIO_PIN_01 | GPIO_PIN_11; /* 端口引脚 */
    stcGpioInit.u32Mode   = GPIO_MD_INPUT;             /* 端口方向配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE;            /* 端口上下拉配置 */
    GPIOA_Init(&stcGpioInit);                          /* 端口初始化 */
    GPIO_PA11_AF_ATIM3_CH0A();                         /* 端口复用配置 */
    GPIO_PA01_AF_ATIM3_CH0B();                         /* 端口复用配置 */
}

/**
 * @brief  ATIM3 配置
 * @retval None
 */
static void Atimer3Config(void)
{
    uint16_t                u16ArrValue;
    stc_atim3_mode23_init_t stcAtim3BaseCfg = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockAtim3); /* ATIM3 外设时钟使能 */

    ATIM3_Mode23_StcInit(&stcAtim3BaseCfg);                                   /* 结构体初始化清零 */
    stcAtim3BaseCfg.u32WorkMode         = ATIM3_M23_M23CR_WORK_MODE_SAWTOOTH; /* 锯齿波模式 */
    stcAtim3BaseCfg.u32CountClockSelect = ATIM3_M23_M23CR_CT_PCLK;            /* 定时器功能，计数时钟为内部PCLK */
    stcAtim3BaseCfg.u32PRS              = ATIM3_M23_M23CR_ATIM3CLK_PRS64;     /* PCLK/64 */
    stcAtim3BaseCfg.u32CountDir         = ATIM3_M23_M23CR_DIR_UP_CNT;         /* 向上计数，在三角波模式时只读 */
    ATIM3_Mode23_Init(&stcAtim3BaseCfg);                                      /* ATIM3 的模式23功能初始化 */

    u16ArrValue = 0xFFFFU;
    ATIM3_Mode23_ARRSet(u16ArrValue); /* 设置重载值,并使能缓存 */
    ATIM3_Mode23_ARRBufferEnable();
}

/**
 * @brief  模拟产生编码器信号，仅供本样例调试用
 * @retval None
 */
static void EncoderSimulation(void)
{
    uint32_t u32Cnt;

    uint8_t u8Ain[17] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1};
    uint8_t u8Bin[17] = {0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0};

    for (u32Cnt = 0; u32Cnt < 17; u32Cnt++)
    {
        if (1U == u8Ain[u32Cnt])
        {
            GPIO_PA03_SET(); /* PA3置位 */
        }
        else
        {
            GPIO_PA03_RESET(); /* PA3清零 */
        }

        if (1U == u8Bin[u32Cnt])
        {
            GPIO_PA04_SET(); /* PA4置位 */
        }
        else
        {
            GPIO_PA04_RESET(); /* PA4清零 */
        }

        LedDisplay();
    }
}

/**
 * @brief  LED亮灭，闪烁次数代表计数值
 *         仅供调试使用,在正交信号AB模拟输出刷新后均会闪烁以显示Cnt的值，具体闪烁次数依据设置的正交编码计数模式和Cnt的值决定
 * @retval 需设置局部变量cnt的最大值 > u16ATIMCnt的最大值，否则LED显示不对标Cnt的值.
 */
static void LedDisplay(void)
{
    uint32_t u32Cnt;
    u16ATIMCnt = ATIM3_MODE23->CNT_f.CNT; /* 获取正交编码计数值 */
    for (u32Cnt = 0; u32Cnt < 50; u32Cnt++)
    {
        if (u32Cnt < u16ATIMCnt)
        {
            STK_LED_ON();
        }
        else
        {
            STK_LED_OFF();
        }
        DDL_Delay1ms(2);
        STK_LED_OFF();
        DDL_Delay1ms(2);
    }
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
