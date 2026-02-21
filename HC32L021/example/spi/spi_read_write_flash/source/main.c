/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of SPI
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-15       MADS            First version
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
#include "w25qxx.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define DATA_SIZE  (0xFFu)
#define FLASH_ADDR (0x00u)
/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
static uint16_t          u16WriteData[DATA_SIZE] = {0};
static uint16_t          u16ReadData[DATA_SIZE]  = {0};
static volatile uint16_t u16W25QXXID             = 0;
/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void GpioConfig(void);
static void SpiConfig(void);
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function of project
 * @retval None
 */
int32_t main(void)
{
    uint16_t u16Count    = 0;
    uint16_t u16ErrorNum = 0;

    STK_LedConfig();     /* 指示灯GPIO初始化 */
    STK_UserKeyConfig(); /* USER KEY 初始化 */

    GpioConfig(); /* SPI GPIO初始化 */

    SpiConfig();     /* SPI配置 */
    SPI_Enable(SPI); /* 使能主机SPI*/

    for (u16Count = 0; u16Count < DATA_SIZE; u16Count++)
    {
        u16WriteData[u16Count] = u16Count;
    }

    while (FALSE == STK_USER_KEY_PRESSED()) /* 等待按键按下 */
    {
        ;
    }
    DDL_Delay1ms(200);

    if (W25QXX_OK != W25QXX_Init()) /* W25QXX 初始化 */
    {
        while (1)
        {
            ;
        }
    }

    u16W25QXXID = W25QXX_ReadID();                  /* 获取W25QXX的ID号 */
    if (W25QXX_OK != W25QXX_BlockErase(FLASH_ADDR)) /* W25QXX擦 */
    {
        while (1)
        {
            ;
        }
    }
    DDL_Delay1ms(100);

    if (W25QXX_OK != W25QXX_Write(u16WriteData, FLASH_ADDR, DATA_SIZE)) /* W25QXX写 */
    {
        while (1)
        {
            ;
        }
    }
    DDL_Delay1ms(100);

    if (W25QXX_OK != W25QXX_Read(u16ReadData, FLASH_ADDR, DATA_SIZE)) /* W25QXX读 */
    {
        while (1)
        {
            ;
        }
    }

    /* 判断是否有数据读错*/
    for (u16Count = 0; u16Count < DATA_SIZE; u16Count++)
    {
        if (u16WriteData[u16Count] != u16ReadData[u16Count])
        {
            u16ErrorNum++;
        }
    }

    while (1)
    {
        if (u16ErrorNum != 0)
        {
            STK_LED_ON(); /* 数据有错误，LED常亮 */
        }
        else
        {
            DDL_Delay1ms(200); /* 读出数据全部正确，LED闪烁 */
            STK_LED_ON();
            DDL_Delay1ms(200);
            STK_LED_OFF();
        }
    }
}

/**
 * @brief  SPI端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockGpio); /* 开启GPIO时钟 */

    GPIO_StcInit(&stcGpioInit); /* 结构体变量初始值初始化 */

    /* 配置引脚PA2(SPI_MOSI)、PA3(SPI_SCK)、PA4(SPI_CS) */
    GPIO_StcInit(&stcGpioInit);                                      /* 结构体变量初始值初始化 */
    stcGpioInit.u32Mode   = GPIO_MD_OUTPUT_PP;                       /* 端口方向配置 */
    stcGpioInit.u32Pin    = GPIO_PIN_02 | GPIO_PIN_03 | GPIO_PIN_04; /* 端口引脚配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_NONE;                          /* 端口上拉配置 */
    GPIOA_Init(&stcGpioInit);                                        /* 初始化GPIO */

    /* 配置引脚PA1 */
    GPIO_StcInit(&stcGpioInit);            /* 结构体变量初始值初始化 */
    stcGpioInit.u32Mode   = GPIO_MD_INPUT; /* 端口方向配置 */
    stcGpioInit.u32Pin    = GPIO_PIN_01;   /* 端口引脚配置 */
    stcGpioInit.u32PullUp = GPIO_PULL_UP;  /* 端口上拉配置 */
    GPIOA_Init(&stcGpioInit);              /* 初始化GPIO */

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

    /* SPI模块配置 */
    SPI_StcInit(&stcSpiInit);                           /* 结构体变量初始值初始化 */
    stcSpiInit.u32BaudRate    = SPI_BAUDRATE_PCLK_DIV2; /* PCLK/2 */
    stcSpiInit.u32CPHA        = SPI_CLK_PHASE_1EDGE;    /* 第一个边沿采样(第二个边沿移位) */
    stcSpiInit.u32CPOL        = SPI_CLK_POLARITY_LOW;   /* 待机时低电平 */
    stcSpiInit.u32Mode        = SPI_MD_MASTER;          /* 主机模式 */
    stcSpiInit.u32BitOrder    = SPI_MSB_FIRST;          /* 最高有效位MSB收发在前 */
    stcSpiInit.u32DataWidth   = SPI_DATA_WIDTH_8BIT;    /* 8BIT数据宽度 */
    stcSpiInit.u32NSS         = SPI_NSS_HARD_OUTPUT;    /* NSS信号由IO管脚输出 */
    stcSpiInit.u32TransDir    = SPI_FULL_DUPLEX;        /* 全双工双向 */
    stcSpiInit.u32SampleDelay = SPI_SAMPLE_NORMAL;      /* 正常采样 */

    SPI_Init(SPI, &stcSpiInit); /* SPI初始化 */

    SPI_FlagClearALL(SPI); /* 清除所有中断标志位 */
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
