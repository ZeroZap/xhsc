/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of SPI
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
/* 主机相关数据 */
static uint16_t                     u16MasterTxBuf[LEN] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
static uint16_t                     u16TempBuff[LEN]    = {0};
static __attribute((used)) uint16_t u16MasterRxBuf[LEN] = {0};
/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void GpioConfig(void);
static void SpiConfig(void);
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    uint16_t u16Tmp = 0x0u;

    STK_LedConfig(); /* LED初始化 */

    STK_UserKeyConfig(); /* USER KEY初始化 */

    GpioConfig(); /* SPI端口配置 */

    SpiConfig(); /* SPI_MASTER 配置 */

    SPI_Enable(SPI); /* 使能主机SPI*/

    while (FALSE == STK_USER_KEY_PRESSED()) /* 等待USER KEY按下 */
    {
        ;
    }
    DDL_Delay1ms(200);

    SPI_MasterNSSOutput(SPI, SPI_NSS_CONFIG_ENABLE); /* 主机 NSS片选后开始通讯 */

    SPI_TransmitReceive(SPI, u16MasterTxBuf, u16TempBuff, LEN); /* 主机向从机发送数据：u8MasterTxBuf[],接收到的数据存放在u16TempBuff[]，不做任何处理 */

    SPI_MasterNSSOutput(SPI, SPI_NSS_CONFIG_DISABLE); /* 主机 拉高NSS片选 */

    DDL_Delay1ms(1);

    SPI_MasterNSSOutput(SPI, SPI_NSS_CONFIG_ENABLE); /* 主机 NSS片选后开始通讯 */

    /* 主机向从机发送数据u16TempBuff[](该数据可以是随机值），主要是为了接收返回数据,存于u8MasterRxBuf[] */
    SPI_TransmitReceive(SPI, u16TempBuff, u16MasterRxBuf, LEN);

    SPI_MasterNSSOutput(SPI, SPI_NSS_CONFIG_DISABLE); /* 主机 拉高NSS片选 */

    /* 判断发送的数据与接收的数据是否相等 */
    for (u16Tmp = 0; u16Tmp < LEN; u16Tmp++)
    {
        if (u16MasterRxBuf[u16Tmp] == u16MasterTxBuf[u16Tmp])
        {
            continue;
        }
        else
        {
            break;
        }
    }

    while (u16Tmp != LEN)
    {
        ;
    }

    while (1)
    {
        STK_LED_ON(); /* LED开启 */
        DDL_Delay1ms(200);
        STK_LED_OFF(); /* LED关闭 */
        DDL_Delay1ms(200);
    }
}

/**
 * @brief  SPI端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit;

    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    /* 结构体变量初始值初始化 */
    GPIO_StcInit(&stcGpioInit);

    /* 配置主机SPI端口 */
    /* 配置引脚PA2(SPI_MOSI)、PA3(SPI_SCK)、PA4(SPI_CS) */
    stcGpioInit.u32Mode   = GPIO_MD_OUTPUT_PP;                       /* 端口方向配置 */
    stcGpioInit.u32Pin    = GPIO_PIN_02 | GPIO_PIN_03 | GPIO_PIN_04; /* 端口引脚配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE;                          /* 端口上拉配置 */
    GPIOA_Init(&stcGpioInit);                                        /* 初始化GPIO */

    /* 配置引脚PA1(SPI_MISO) */
    GPIO_StcInit(&stcGpioInit);             /* 结构体变量初始值初始化 */
    stcGpioInit.u32Mode   = GPIO_MD_INPUT;  /* 端口方向配置 */
    stcGpioInit.u32Pin    = GPIO_PIN_01;    /* 端口引脚配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE; /* 端口上拉配置 */
    GPIOA_Init(&stcGpioInit);               /* 初始化GPIO */

    /* 设置PA1、PA2、PA3、PA4的复用功能 */
    GPIO_PA02_AF_SPI_MOSI(); /* 配置引脚PA2作为SPI_MOSI */
    GPIO_PA03_AF_SPI_SCK();  /* 配置引脚PA3作为SPI_SCK */
    GPIO_PA04_AF_SPI_CS();   /* 配置引脚PA4作为SPI_CS */
    GPIO_PA01_AF_SPI_MISO(); /* 配置引脚PA1作为SPI_MISO */
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

    /* SPI模块配置：主机 */
    SPI_StcInit(&stcSpiInit);                           /* 结构体变量初始值初始化 */
    stcSpiInit.u32BaudRate    = SPI_BAUDRATE_PCLK_DIV8; /* PCLK/8 */
    stcSpiInit.u32CPHA        = SPI_CLK_PHASE_1EDGE;    /* 第一个边沿采样(第二个边沿移位) */
    stcSpiInit.u32CPOL        = SPI_CLK_POLARITY_LOW;   /* 待机时低电平 */
    stcSpiInit.u32Mode        = SPI_MD_MASTER;          /* 主机模式 */
    stcSpiInit.u32BitOrder    = SPI_MSB_FIRST;          /* 最高有效位MSB收发在前 */
    stcSpiInit.u32DataWidth   = SPI_DATA_WIDTH_8BIT;    /* 8BIT数据宽度 */
    stcSpiInit.u32NSS         = SPI_NSS_HARD_OUTPUT;    /* NSS信号由IO管脚输出 */
    stcSpiInit.u32TransDir    = SPI_FULL_DUPLEX;        /* 全双工双向 */
    stcSpiInit.u32SampleDelay = SPI_SAMPLE_NORMAL;      /* 正常采样 */

    SPI_Init(SPI, &stcSpiInit); /* SPI初始化 */
    SPI_FlagClearALL(SPI);      /* 清除所有中断标志位 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
