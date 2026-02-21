/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of SYSCTRL
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-11-14       MADS            First version
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
static void SysClockConfig(void);
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
    /* 系统时钟配置: RC48M 6M输出 */
    SysClockConfig();

    /* USER KEY配置 */
    STK_UserKeyConfig();

    /* =============================================== */
    /* ==============       WARNING       ============ */
    /* =============================================== */
    /* 因本样例使用PA14（SWCLK）输出时钟，为了避免程序再次下载出现问题，请勿删除延时DDL_Delay1ms函数调用 */
    DDL_Delay1ms(2000u);

    /* 配置Hclk分频时钟信号从IO口输出 */
    GpioConfig();

    while (!STK_USER_KEY_PRESSED()) /* 等待按键按下 */
    {
        ;
    }
    DDL_Delay1ms(200u);
    /* RC48M 32MHz作为Hclk时钟源 */
    FLASH_WaitCycle(FLASH_WAIT_CYCLE1);                /* Flash wait cycle = 1 */
    SYSCTRL_SysClockSwitch(SYSCTRL_CLK_SRC_RC48M_32M); /* 切换RC48M 32M输出 */
    SystemCoreClockUpdate();                           /* 更新Core时钟（HCLK）*/

    while (!STK_USER_KEY_PRESSED()) /* 等待按键按下 */
    {
        ;
    }
    DDL_Delay1ms(200u);
    /* RC48M 48MHz作为Hclk时钟源 */
    FLASH_WaitCycle(FLASH_WAIT_CYCLE1);                /* Flash wait cycle = 1 */
    SYSCTRL_SysClockSwitch(SYSCTRL_CLK_SRC_RC48M_48M); /* 切换RC48M 48M输出 */
    SystemCoreClockUpdate();                           /* 更新Core时钟（HCLK）*/

    while (!STK_USER_KEY_PRESSED()) /* 等待按键按下 */
    {
        ;
    }
    DDL_Delay1ms(200u);
    /* RC48M 4MHz作为Hclk时钟源 */
    SYSCTRL_SysClockSwitch(SYSCTRL_CLK_SRC_RC48M_4M); /* 切换RC48M 4M输出 */
    FLASH_WaitCycle(FLASH_WAIT_CYCLE0);               /* Flash wait cycle = 0 */
    SystemCoreClockUpdate();                          /* 更新Core时钟（HCLK）*/

    while (!STK_USER_KEY_PRESSED()) /* 等待按键按下 */
    {
        ;
    }
    DDL_Delay1ms(200u);
    /* 使用从PA11输入的EXTH时钟作为Hclk时钟源 */
    SYSCTRL_ClockSrcEnable(SYSCTRL_CLK_SRC_EXTH); /* 使能EXTH输出 */
    if (SYSTEM_EXTH <= 24000000)
    {
        FLASH_WaitCycle(FLASH_WAIT_CYCLE0); /* Flash wait cycle = 0 */
    }
    else
    {
        FLASH_WaitCycle(FLASH_WAIT_CYCLE1); /* Flash wait cycle = 1 */
    }
    SYSCTRL_SysClockSwitch(SYSCTRL_CLK_SRC_EXTH);      /* 切换EXTH输出 */
    SystemCoreClockUpdate();                           /* 更新Core时钟（HCLK）*/
    SYSCTRL_ClockSrcDisable(SYSCTRL_CLK_SRC_RC48M_4M); /* 关闭RC48M 4M输出 */

    while (!STK_USER_KEY_PRESSED()) /* 等待按键按下 */
    {
        ;
    }
    DDL_Delay1ms(200u);
    /* RCL 32768作为Hclk时钟源  */
    SYSCTRL_ClockSrcEnable(SYSCTRL_CLK_SRC_RCL_32768); /* 使能RCL输出 */
    SYSCTRL_SysClockSwitch(SYSCTRL_CLK_SRC_RCL_32768); /* 切换RCL 32768输出 */
    FLASH_WaitCycle(FLASH_WAIT_CYCLE0);                /* Flash wait cycle = 0 */
    SystemCoreClockUpdate();                           /* 更新Core时钟（HCLK）*/
    SYSCTRL_ClockSrcDisable(SYSCTRL_CLK_SRC_EXTH);     /* 关闭EXTH输出 */

    while (!STK_USER_KEY_PRESSED()) /* 等待按键按下 */
    {
        ;
    }
    DDL_Delay1ms(200u);
    /* RCL 38400作为Hclk时钟源  */
    SYSCTRL_SysClockSwitch(SYSCTRL_CLK_SRC_RCL_38400); /* 切换RCL 38400输出 */
    SystemCoreClockUpdate();                           /* 更新Core时钟（HCLK）*/

    while (!STK_USER_KEY_PRESSED()) /* 等待按键按下 */
    {
        ;
    }
    DDL_Delay1ms(200u);
    /* XTL作为Hclk时钟源  */
    SYSCTRL_ClockSrcEnable(SYSCTRL_CLK_SRC_XTL);        /* 使能XTL输出 */
    SYSCTRL_SysClockSwitch(SYSCTRL_CLK_SRC_XTL);        /* 切换XTL输出 */
    SystemCoreClockUpdate();                            /* 更新Core时钟（HCLK）*/
    SYSCTRL_ClockSrcDisable(SYSCTRL_CLK_SRC_RCL_38400); /* 关闭RCL输出 */

    while (!STK_USER_KEY_PRESSED()) /* 等待按键按下 */
    {
        ;
    }
    DDL_Delay1ms(200u);
    /* RC48M 4M作为Hclk时钟源  */
    SYSCTRL_ClockSrcEnable(SYSCTRL_CLK_SRC_RC48M_4M); /* 使能RC48M 4M输出 */
    SYSCTRL_SysClockSwitch(SYSCTRL_CLK_SRC_RC48M_4M); /* 切换RC48M 4M输出 */
    SystemCoreClockUpdate();                          /* 更新Core时钟（HCLK）*/
    SYSCTRL_ClockSrcDisable(SYSCTRL_CLK_SRC_XTL);     /* 关闭XTL输出 */

    while (1)
    {
        ;
    }
}

/**
 * @brief  系统时钟配置
 * @retval None
 */
static void SysClockConfig(void)
{
    stc_sysctrl_clock_init_t stcSysClockInit = {0};

    /* 结构体初始化 */
    SYSCTRL_ClockStcInit(&stcSysClockInit);

    stcSysClockInit.u32SysClockSrc = SYSCTRL_CLK_SRC_RC48M_6M; /* 选择系统默认RC48M 6MHz作为Hclk时钟源 */
    stcSysClockInit.u32HclkDiv     = SYSCTRL_HCLK_PRS_DIV1;    /* Hclk 1分频 */
    SYSCTRL_ClockInit(&stcSysClockInit);                       /* 系统时钟初始化 */
}

/**
 * @brief  端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    /* 结构体初始化 */
    GPIO_StcInit(&stcGpioInit);

    /* 开启GPIO外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    /* 配置PA14端口为输出 */
    stcGpioInit.u32Pin  = GPIO_PIN_14;
    stcGpioInit.u32Mode = GPIO_MD_OUTPUT_PP;
    GPIOA_Init(&stcGpioInit);
    /* 设置从PA14输出Hclk的8分频时钟 */
    GPIO_PA14_AF_TCLK_OUT_HCLK_DIV8();

    /* 配置PA14作为端口使用 */
    SYSCTRL_FuncEnable(SYSCTRL_FUNC_SWD_USE_IO);
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
