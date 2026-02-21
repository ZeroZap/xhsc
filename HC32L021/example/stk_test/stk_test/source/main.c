/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of STK_TEST
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
#include "ddl.h"
#include "gpio.h"
#include "lpuart.h"
#include "sysctrl.h"
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
void GpioConfig(void);
void LpuartConfig(void);
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
    /* 板载LED配置 */
    STK_LedConfig();

    /* 板载LED用户按键配置 */
    STK_UserKeyConfig();

    GpioConfig();

    /* LPUART配置 */
    LpuartConfig();

    stc_stk_chip_info_t stcChipInfo = {0};

    uint8_t u8Index;

    /* MCU信息获取 */
    STK_ChipInfoGet(&stcChipInfo);

    /* 信息打印 */
    printf("@ Welcome to use the XHSC MCU !\r\n@ The Chip Info below are:\r\n");
    printf("* %s\r\n", stcChipInfo.pcProductNum);
    printf("* UID - ");
    for (u8Index = 0; u8Index < 10; u8Index++)
    {
        printf("%02X", stcChipInfo.u8UID[u8Index]);
    }
    printf("\r\n");
    printf("* ARM Cortex-M0+ 32-bit, 48MHz\r\n");
    printf("* %dkB RAM, %dkB FLASH\r\n", stcChipInfo.u32RamSize / 1024, stcChipInfo.u32FlashSize / 1024);
    printf("* -40 ~ 105C, 1.8V ~ 5.5V\r\n");

    printf("\r\nPlease connect to \"http://www.xhsc.com.cn\" to obtain SDK!\r\n\r\n");

    while (1)
    {
        if (STK_USER_KEY_PRESSED()) /* USER KEY 按键检测 */
        {
            STK_LED_ON();
            DDL_Delay1ms(100);
            STK_LED_OFF();
            DDL_Delay1ms(100);
        }
        else
        {
            STK_LED_ON();
            DDL_Delay1ms(500);
            STK_LED_OFF();
            DDL_Delay1ms(500);
        }
    }
}

/**
 * @brief  LPUART配置
 * @retval None
 */
void LpuartConfig(void)
{
    stc_lpuart_init_t stcLpuartInit;

    /* 外设模块时钟使能 */
    SYSCTRL_PeriphClockEnable(PeriphClockLpuart1);

    /* LPUART 初始化 */
    LPUART_StcInit(&stcLpuartInit);                                    /* 结构体初始化 */
    stcLpuartInit.u32TransMode              = LPUART_MODE_TX_RX;       /* 收发模式 */
    stcLpuartInit.u32FrameLength            = LPUART_FRAME_LEN_8B_PAR; /* 数据8位，奇偶校验1位*/
    stcLpuartInit.u32Parity                 = LPUART_B8_PARITY_EVEN;   /* 偶校验 */
    stcLpuartInit.u32StopBits               = LPUART_STOPBITS_1;       /* 1停止位 */
    stcLpuartInit.u32BaudRateGenSelect      = LPUART_BAUD_NORMAL;      /* 波特率生成选择：用OVER和SCNT产生波特率 */
    stcLpuartInit.stcBaudRate.u32SclkSelect = LPUART_SCLK_SEL_PCLK;    /* 传输时钟源 */
    stcLpuartInit.stcBaudRate.u32Sclk       = SYSCTRL_HclkFreqGet();   /* HCLK获取 */
    stcLpuartInit.stcBaudRate.u32Baud       = 9600;                    /* 波特率 */
    LPUART_Init(LPUART1, &stcLpuartInit);

    LPUART_IntFlagClearAll(LPUART1); /* 清除所有状态标志 */
}

/**
 * @brief  端口配置
 * @retval None
 */
void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    /* 配置PA01为LPUART1_TX */
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Mode      = GPIO_MD_OUTPUT_PP;
    stcGpioInit.bOutputValue = TRUE;
    stcGpioInit.u32Pin       = GPIO_PIN_01;
    GPIO_Init(GPIOA, &stcGpioInit);
    GPIO_PA01_AF_LPUART1_TXD();
}

/**
 * @brief  串口打印重定向
 */
PUTCHAR_PROTOTYPE
{
    LPUART_TransmitPoll(LPUART1, (uint8_t *)(&ch), 1);
    return ch;
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
