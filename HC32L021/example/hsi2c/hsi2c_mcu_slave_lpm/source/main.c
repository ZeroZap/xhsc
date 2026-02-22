/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of HSI2C
 @verbatim
   Change Logs:
   Date             Author          Notes
   2025-08-08       MADS            First version
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

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l021_flash.h"
#include "hc32l021_gpio.h"
#include "hc32l021_hsi2c.h"
#include "hc32l021_sysctrl.h"
#include "hc32l021_lpm.h"
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define TRANS_TIMEOUT (0xFFFFFFFFu) /* 传输超时保护计数值 */
#define TRANS_SIZE    (30)          /* 传输字节数 */
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void SysClockConfig(void);
static void GpioConfig(void);
static void Hsi2cSlaveReadConfig(void);
static void Hsi2cSlaveWriteConfig(void);
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static uint8_t u8Data[TRANS_SIZE] = { 0 };
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    /* 系统时钟配置 */
    SysClockConfig();

    /* LED端口配置 */
    STK_LedConfig();

    /* 端口配置 */
    GpioConfig();

    /* HSI2C配置：从机读 */
    Hsi2cSlaveReadConfig();
    /* 从机中断设置 */
    EnableNvic(HSI2C_IRQn, IrqPriorityLevel1, TRUE);

    /* 从机中断传输 */
    HSI2C_SlaveTransferInt(HSI2C, u8Data, sizeof(u8Data));

    /* =============================================== */
    /* ==============       WARNING       ============ */
    /* =============================================== */
    /* 本样例程序会进入深度休眠模式，因此以下代码起防护作用*/
    /* （防止进入深度休眠后芯片调试功能不能再次使用） */
    /* 在使用本样例时，禁止在没有唤醒机制的情况下删除以下代码 */
    DDL_Delay1ms(2000);
    LPM_GotoDeepSleep(
        FALSE); /* 进入低功耗模式——深度休眠（禁止唤醒后退出中断自动休眠特性） */

    while (stcHsi2cCom.enComStatus == Hsi2cComBusy) {
        ;
    }

    /* HSI2C配置：从机写 */
    Hsi2cSlaveWriteConfig();
    /* 从机中断传输 */
    HSI2C_SlaveTransferInt(HSI2C, u8Data, sizeof(u8Data));

    LPM_GotoDeepSleep(
        FALSE); /* 进入低功耗模式——深度休眠（禁止唤醒后退出中断自动休眠特性） */

    while (stcHsi2cCom.enComStatus == Hsi2cComBusy) {
        ;
    }

    STK_LED_ON();

    while (1) {
        ;
    }
}

/**
 * @brief  HSI2C从机读配置
 * @retval None
 */
static void Hsi2cSlaveReadConfig(void)
{
    /* 结构体初始化 */
    HSI2C_SlaveStcInit(&stcHsi2cSlaveInit);

    /* 开启HSI2C外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockHsi2c);

    stcHsi2cSlaveInit.bResetBeforeInit = TRUE;  /* 初始化前复位 */
    stcHsi2cSlaveInit.u32SlaveAddr0    = 0x06u; /* 从机地址0 */
    stcHsi2cSlaveInit.u32SlaveAddr1    = 0x0Fu; /* 从机地址1 */
    stcHsi2cSlaveInit.u8SubAddrSize    = 2;     /* Sub address
                                                   size：!=0时，接收的Subaddress存放在stcHsi2cCom.u32SubAddr中，否则存放在u8Data[]数组中
                                                 */
    stcHsi2cSlaveInit.enDir = Hsi2cMasterWriteSlaveRead;

    stcHsi2cSlaveInit.stcSlaveConfig1.u32FuncSelect =
        HSI2C_SLAVE_ADRSTALL_ENABLE;
    stcHsi2cSlaveInit.stcSlaveConfig1.u32ConfigAddrMatchMode =
        HSI2C_SLAVE_ADDR0_7B_OR_ADDR1_7B;

    stcHsi2cSlaveInit.stcSlaveConfig2.u32FilterEnable = HSI2C_SLAVE_FILTER_OFF;
    HSI2C_SlaveInit(HSI2C, &stcHsi2cSlaveInit, SystemCoreClock);
}

/**
 * @brief  HSI2C从机写配置
 * @retval None
 */
static void Hsi2cSlaveWriteConfig(void)
{
    /* 结构体初始化 */
    HSI2C_SlaveStcInit(&stcHsi2cSlaveInit);

    /* 开启HSI2C外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockHsi2c);

    stcHsi2cSlaveInit.bResetBeforeInit = TRUE;  /* 初始化前复位 */
    stcHsi2cSlaveInit.u32SlaveAddr0    = 0x06u; /* 从机地址0 */
    stcHsi2cSlaveInit.u32SlaveAddr1    = 0x0Fu; /* 从机地址1 */
    stcHsi2cSlaveInit.u8SubAddrSize    = 2;     /* Sub address
                                                   size：!=0时，接收的Subaddress存放在stcHsi2cCom.u32SubAddr中，否则存放在u8Data[]数组中
                                                 */
    stcHsi2cSlaveInit.enDir = Hsi2cMasterReadSlaveWrite;

    stcHsi2cSlaveInit.stcSlaveConfig1.u32FuncSelect =
        HSI2C_SLAVE_TXDSTALL_ENABLE | HSI2C_SLAVE_ADRSTALL_ENABLE;
    stcHsi2cSlaveInit.stcSlaveConfig1.u32ConfigAddrMatchMode =
        HSI2C_SLAVE_ADDR0_7B_OR_ADDR1_7B;

    stcHsi2cSlaveInit.stcSlaveConfig2.u32FilterEnable = HSI2C_SLAVE_FILTER_OFF;

    HSI2C_SlaveInit(HSI2C, &stcHsi2cSlaveInit, SystemCoreClock);
}

/**
 * @brief  系统时钟配置
 * @retval None
 */
static void SysClockConfig(void)
{
    stc_sysctrl_clock_init_t stcSysClockInit = { 0 };

    /* 结构体初始化 */
    SYSCTRL_ClockStcInit(&stcSysClockInit);

    stcSysClockInit.u32SysClockSrc =
        SYSCTRL_CLK_SRC_RC48M_48M; /* 选择系统默认RC48M 48MHz作为Hclk时钟源 */
    stcSysClockInit.u32HclkDiv = SYSCTRL_HCLK_PRS_DIV1; /* Hclk 1分频 */
    SYSCTRL_ClockInit(&stcSysClockInit); /* 系统时钟初始化 */
}

/**
 * @brief  端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = { 0 };

    /* 结构体初始化 */
    GPIO_StcInit(&stcGpioInit);

    /* 开启GPIO外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    /* 配置PA10(SDA)、PA11(SCL)端口 */
    stcGpioInit.bOutputValue = TRUE;
    stcGpioInit.u32Pin       = GPIO_PIN_10 | GPIO_PIN_11;
    stcGpioInit.u32Mode      = GPIO_MD_OUTPUT_OD;
    GPIOA_Init(&stcGpioInit);
    /* 设置PA10(SDA)、PA11(SCL)功能 */
    GPIO_PA10_AF_HSI2C_SDA();
    GPIO_PA11_AF_HSI2C_SCL();
}

/**
 * @brief  HSI2C中断处理
 * @retval None
 */
void Hsi2c_IRQHandler(void)
{
    uint32_t u32Flag = 0;
    if (stcHsi2cCom.HSI2CxBaseAddr->SCR0_f.SEN == 1) {
        u32Flag                          = stcHsi2cCom.HSI2CxBaseAddr->SSR;
        stcHsi2cCom.HSI2CxBaseAddr->SSCR = u32Flag;

        if (Hsi2cComSuccess == HSI2C_SlaveErrorGet(u32Flag)) {
            /* Receive */
            if (u32Flag & HSI2C_SLAVE_FLAG_RDF) {
                uint32_t u32RxData = stcHsi2cCom.HSI2CxBaseAddr->SRDR;
                if (!(u32RxData & HSI2C_SRDR_REMP_Msk)) {
                    if (0 != stcHsi2cCom.u32RxSize) /* 需要接收Sub address */
                    {
                        stcHsi2cCom.u8SubAddr[stcHsi2cCom.u32RxSize - 1] =
                            (uint8_t)(u32RxData);
                        stcHsi2cCom.u32RxSize--;
                    } else /* 不需要接收Sub address */
                    {
                        if (stcHsi2cCom.u32DataIndex < stcHsi2cCom.u32Len) {
                            stcHsi2cCom.pu8Buf[stcHsi2cCom.u32DataIndex++] =
                                (uint8_t)(u32RxData);
                        }
                        if (stcHsi2cCom.u32DataIndex >= stcHsi2cCom.u32Len) {
                            stcHsi2cCom.HSI2CxBaseAddr->STAR =
                                HSI2C_STAR_TXNACK_Msk; /* Send NACK */
                            stcHsi2cCom.u32DataIndex = 0;
                        }
                    }
                }
            }

            /* Transmit */
            if (u32Flag & HSI2C_SLAVE_FLAG_TDF) {
                if (stcHsi2cCom.u32DataIndex < stcHsi2cCom.u32Len) {
                    HSI2C_SlaveWriteData(
                        stcHsi2cCom.HSI2CxBaseAddr,
                        stcHsi2cCom.pu8Buf[stcHsi2cCom.u32DataIndex++]);
                }
                if (stcHsi2cCom.u32DataIndex >= stcHsi2cCom.u32Len) {
                    stcHsi2cCom.u32DataIndex = 0;
                }
            }

            /* Stop or restart */
            if (u32Flag & (HSI2C_SLAVE_FLAG_RSF | HSI2C_SLAVE_FLAG_SDF)) {
                if (u32Flag & HSI2C_SLAVE_FLAG_SDF) {
                    stcHsi2cCom.enComStatus = Hsi2cComIdle;
                }
                stcHsi2cCom.HSI2CxBaseAddr->STAR = 0U;
            }

            /* Valid address */
            if (u32Flag
                & (HSI2C_SLAVE_FLAG_AVF | HSI2C_SLAVE_FLAG_AM0F
                   | HSI2C_SLAVE_FLAG_AM1F)) {
                /* Get received slave address */
                volatile uint32_t u32Temp = stcHsi2cCom.HSI2CxBaseAddr->SASR;
            }

            /* Transmit ACK */
            if (u32Flag & HSI2C_SLAVE_FLAG_TAF) {
                stcHsi2cCom.HSI2CxBaseAddr->STAR = 0U;
            }
        } else /* Error handling */
        {
            HSI2C_SlaveTransAbort(stcHsi2cCom.HSI2CxBaseAddr);
            stcHsi2cCom.enComStatus = Hsi2cComIdle;
        }
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
