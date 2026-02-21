/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of SPI
 @verbatim
   Change Logs:
   Date             Author          Notes
   2025-03-05       MADS            First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2025, Xiaohua Semiconductor Co., Ltd. All rights reserved.
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
#include "flash.h"
#include "gpio.h"
#include "spi.h"
#include "sysctrl.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define LEN (10u)
/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
/*从机相关数据 */
static uint16_t                     u16TempBuf[LEN]    = {0};
static __attribute((used)) uint16_t u16SlaveRxBuf[LEN] = {0};
/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void GpioConfig(void);
static void SpiConfig(void);
static void SysClockConfig(void);
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    uint16_t u16Index = 0;

    SysClockConfig(); /* SPI_SLAVE 主频配置 */

    STK_LedConfig(); /* LED初始化 */

    STK_UserKeyConfig(); /* USER KEY初始化 */

    GpioConfig(); /* SPI_SLAVE 端口配置 */

    SpiConfig(); /* SPI_SLAVE 配置 */

    /* 初始化数组 */
    for (u16Index = 0; u16Index < LEN; u16Index++)
    {
        u16TempBuf[u16Index] = u16Index + 1;
    }

    SPI_Enable(SPI); /* SPI_SLAVE 使能 */

    DDL_Delay1ms(100);

    /* 等待主机通信 */
    while (FALSE == STK_USER_KEY_PRESSED()) /* 等待USER KEY按下 */
    {
        ;
    }
    DDL_Delay1ms(200);

    /* 等待片选信号生效 */
    while (GPIO_PA04_READ())
    {
        ;
    }

    /* 接收数据 */
    for (u16Index = 0; u16Index < LEN; u16Index++)
    {
        /* 等待接收数据 */
        while (FALSE == SPI->SR_f.RXNE)
        {
            ;
        }

        /* 读取接收数据 */
        u16SlaveRxBuf[u16Index] = SPI->DR;
    }

    /* 判断接收的数据与传输数据是否相等 */
    for (u16Index = 0; u16Index < LEN; u16Index++)
    {
        if (u16SlaveRxBuf[u16Index] == u16TempBuf[u16Index])
        {
            continue;
        }
        else
        {
            break;
        }
    }

    while (u16Index != LEN)
    {
        ;
    }

    while (1)
    {
        STK_LED_ON();
        DDL_Delay1ms(500);
        STK_LED_OFF();
        DDL_Delay1ms(500);
    }
}

/**
 * @brief  SPI端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    /* 配置从机SPI端口  */
    GPIO_StcInit(&stcGpioInit);                                      /* 结构体变量初始值初始化 */
    stcGpioInit.u32Mode   = GPIO_MD_INPUT;                           /* 端口方向配置 */
    stcGpioInit.u32Pin    = GPIO_PIN_02 | GPIO_PIN_03 | GPIO_PIN_04; /* 端口引脚配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE;                          /* 端口上拉配置 */
    GPIOA_Init(&stcGpioInit);                                        /* 初始化GPIO */

    /* 设置PA2、PA3、PA4的复用功能 */
    GPIO_PA03_AF_SPI_SCK();  /* 配置引脚PA3作为SPI_SCK */
    GPIO_PA04_AF_SPI_CS();   /* 配置引脚PA4作为SPI_CS */
    GPIO_PA02_AF_SPI_MOSI(); /* 配置引脚PA2作为SPI_MOSI */
}

/**
 * @brief  初始化SPI
 * @retval None
 */
static void SpiConfig(void)
{
    stc_spi_init_t stcSpiInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockSpi); /* 开启SPI时钟门控 */
    SYSCTRL_PeriphReset(PeriphResetSpi);       /* 复位SPI模块 */

    /* SPI模块配置 从机 */
    SPI_StcInit(&stcSpiInit);                       /* 结构体变量初始值初始化 */
    stcSpiInit.u32CPHA      = SPI_CLK_PHASE_1EDGE;  /* 第一个边沿采样(第二个边沿移位) */
    stcSpiInit.u32CPOL      = SPI_CLK_POLARITY_LOW; /* 待机时低电平 */
    stcSpiInit.u32Mode      = SPI_MD_SLAVE;         /* 从机模式 */
    stcSpiInit.u32BitOrder  = SPI_MSB_FIRST;        /* 最高有效位MSB收发在前 */
    stcSpiInit.u32DataWidth = SPI_DATA_WIDTH_8BIT;  /* 8BIT数据宽度 */
    stcSpiInit.u32NSS       = SPI_NSS_HARD_INPUT;   /* NSS信号由IO管脚输入 */
    stcSpiInit.u32TransDir  = SPI_SIMPLE_RX;        /* 单工单向接收 */

    SPI_Init(SPI, &stcSpiInit); /* SPI初始化 */

    SPI_FlagClearALL(SPI); /* 清除所有中断标志位 */
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

    stcSysClockInit.u32SysClockSrc = SYSCTRL_CLK_SRC_RC48M_4M; /* 选择系统默认RC48M 4MHz作为Hclk时钟源 */
    stcSysClockInit.u32HclkDiv     = SYSCTRL_HCLK_PRS_DIV1;    /* Hclk 1分频 */
    SYSCTRL_ClockInit(&stcSysClockInit);                       /* 系统时钟初始化 */
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
