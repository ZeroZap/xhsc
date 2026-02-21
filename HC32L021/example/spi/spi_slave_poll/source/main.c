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
/*从机相关数据 */
static uint16_t                     u16TempBuff[LEN]   = {0};
static __attribute((used)) uint16_t u16SlaveRxBuf[LEN] = {0};
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
    GpioConfig(); /* SPI_SLAVE 端口配置 */

    SpiConfig(); /* SPI_SLAVE 配置 */

    SPI_Enable(SPI); /* SPI_SLAVE 使能 */

    /* =====从机接收数据========== */

    /* 从机接收主机发来的数据，并存在u16SlaveRxBuf[]，u16TempBuff[]可以为随机值，主机收到也不做任何处理 */
    SPI_TransmitReceive(SPI, u16TempBuff, u16SlaveRxBuf, LEN); /* 从机接收主机数据 */
    /* =====从机发送数据========== */

    /* 从机发送时，在主机片选拉低之前，将第一个字节数据载入 */
    SPI_SlaveDummyDataTransmit(SPI, u16SlaveRxBuf[0]);

    /* 等待片选信号生效 */
    while (GPIO_PA04_READ())
    {
        ;
    }

    /* 从机把从主机收到的数据u16SlaveRxBuf[]，返回给主机，u16TempBuff[]为主机发来的随机值，从机不做任何处理 */
    SPI_TransmitReceive(SPI, (u16SlaveRxBuf + 1), u16TempBuff, (LEN - 1));

    while (1)
    {
        ;
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

    /* 配置引脚PA1 */
    GPIO_StcInit(&stcGpioInit);                /* 结构体变量初始值初始化 */
    stcGpioInit.u32Mode   = GPIO_MD_OUTPUT_PP; /* 端口方向配置 */
    stcGpioInit.u32Pin    = GPIO_PIN_01;       /* 端口引脚配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE;    /* 端口上拉配置 */
    GPIOA_Init(&stcGpioInit);                  /* 初始化GPIO */

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

    /* SPI模块配置 从机 */
    SPI_StcInit(&stcSpiInit);                       /* 结构体变量初始值初始化 */
    stcSpiInit.u32CPHA      = SPI_CLK_PHASE_1EDGE;  /* 第一个边沿采样(第二个边沿移位) */
    stcSpiInit.u32CPOL      = SPI_CLK_POLARITY_LOW; /* 待机时低电平 */
    stcSpiInit.u32Mode      = SPI_MD_SLAVE;         /* 从机模式 */
    stcSpiInit.u32BitOrder  = SPI_MSB_FIRST;        /* 最高有效位MSB收发在前 */
    stcSpiInit.u32DataWidth = SPI_DATA_WIDTH_8BIT;  /* 8BIT数据宽度 */
    stcSpiInit.u32NSS       = SPI_NSS_HARD_INPUT;   /* NSS信号由IO管脚输入 */
    stcSpiInit.u32TransDir  = SPI_FULL_DUPLEX;      /* 全双工双向 */

    SPI_Init(SPI, &stcSpiInit); /* SPI初始化 */

    SPI_FlagClearALL(SPI); /* 清除所有中断标志位 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
