/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of HSI2C
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-02       MADS            First version
   2025-07-08       MADS            Modify interrupt config API
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2024~2025, Xiaohua Semiconductor Co., Ltd. All rights reserved.
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
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define BAUD_RATE        (400000)           /* 波特率 */
#define DEVICE_ADDR      (0x50u)            /* 从机设备地址 */
#define DEVICE_PAGE_SIZE (8u)               /* 设备页字节长度 */
#define TRANS_SIZE       (DEVICE_PAGE_SIZE) /* 传输字节数 */

#define TRANS_TIMEOUT (0xFFFFFFFFu) /* 传输超时保护计数值 */
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void SysClockConfig(void);
static void GpioConfig(void);
static void Hsi2cMasterReadConfig(void);
static void Hsi2cMasterWriteConfig(void);
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static uint8_t u8WriteData[TRANS_SIZE] = { 0 };
static uint8_t u8ReadData[TRANS_SIZE]  = { 0 };
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    uint32_t u32Index = 0;

    /* 系统时钟配置 */
    SysClockConfig();

    /* LED端口配置 */
    STK_LedConfig();

    /* 测试数据初始化 */
    for (u32Index = 0; u32Index < TRANS_SIZE; u32Index++) {
        u8WriteData[u32Index] = u32Index + 0x10u;
        u8ReadData[u32Index]  = u32Index + 0x20u;
    }

    /* 端口配置 */
    GpioConfig();

    /* HSI2C配置：主机写 */
    Hsi2cMasterWriteConfig();
    /* 主机中断设置 */
    EnableNvic(HSI2C_IRQn, IrqPriorityLevel1, TRUE);
    /* 主机中断传输 */
    HSI2C_MasterTransferInt(HSI2C, u8WriteData, sizeof(u8WriteData));

    while (stcHsi2cCom.enComStatus == Hsi2cComBusy) {
        ;
    }

    /* 根据实际使用场景设置合适的延时时间 */
    DDL_Delay1ms(10);

    /* HSI2C配置：主机读 */
    Hsi2cMasterReadConfig();
    /* 主机中断传输 */
    HSI2C_MasterTransferInt(HSI2C, u8ReadData, sizeof(u8ReadData));

    while (stcHsi2cCom.enComStatus == Hsi2cComBusy) {
        ;
    }

    STK_LED_ON();

    while (1) {
        ;
    }
}

/**
 * @brief  HSI2C主机写配置
 * @retval None
 * @note   时序：
 *         u8SubAddrSize = 0：
 *             Start + 7-bit Slave Address(W) + u8WriteData[TRANS_SIZE] + Stop
 *         u8SubAddrSize = 2：
 *             Start + 7-bit Slave Address(W) + Sub Address(2bytes) +
 * u8WriteData[TRANS_SIZE] + Stop
 */
static void Hsi2cMasterWriteConfig(void)
{
    /* 结构体初始化 */
    HSI2C_MasterStcInit(&stcHsi2cMasterInit);

    /* 开启HSI2C外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockHsi2c);

    stcHsi2cMasterInit.bResetBeforeInit = TRUE; /* 初始化前复位 */
    stcHsi2cMasterInit.u32EnableDebug   = HSI2C_MASTER_DEBUG_OFF;
    stcHsi2cMasterInit.u32BaudRateHz    = BAUD_RATE;   /* 波特率 */
    stcHsi2cMasterInit.u8SlaveAddr      = DEVICE_ADDR; /* 从机地址 */
    stcHsi2cMasterInit.u32SubAddr       = 0x0000u;     /* Sub address */
    stcHsi2cMasterInit.u8SubAddrSize    = 2;           /* Sub address size */
    stcHsi2cMasterInit.enDir            = Hsi2cMasterWriteSlaveRead;

    stcHsi2cMasterInit.stcMasterConfig2.u32SdaFilterEnable =
        HSI2C_MASTER_FILTBPSDA_ENABLE;
    stcHsi2cMasterInit.stcMasterConfig2.u32SclFilterEnable =
        HSI2C_MASTER_FILTBPSCL_ENABLE;

    if (Ok != HSI2C_MasterInit(HSI2C, &stcHsi2cMasterInit, SystemCoreClock)) {
        while (1) {
            ;
        }
    }
}

/**
 * @brief  HSI2C主机读配置
 * @retval None
 * @note   时序：
 *         u8SubAddrSize = 0：
 *             Start + 7-bit Slave Address(R) + u8ReadData[TRANS_SIZE] + Stop
 *         u8SubAddrSize = 2：
 *             Start + 7-bit Slave Address(W) + Sub Address(2bytes) + Restart +
 * 7-bit Slave Address(R) + u8ReadData[TRANS_SIZE] + Stop
 */
static void Hsi2cMasterReadConfig(void)
{
    /* 结构体初始化 */
    HSI2C_MasterStcInit(&stcHsi2cMasterInit);

    /* 开启HSI2C外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockHsi2c);

    stcHsi2cMasterInit.bResetBeforeInit = TRUE; /* 初始化前复位 */
    stcHsi2cMasterInit.u32EnableDebug   = HSI2C_MASTER_DEBUG_OFF;
    stcHsi2cMasterInit.u32BaudRateHz    = BAUD_RATE;   /* 波特率 */
    stcHsi2cMasterInit.u8SlaveAddr      = DEVICE_ADDR; /* 从机地址 */
    stcHsi2cMasterInit.u32SubAddr       = 0x0000u;     /* Sub address */
    stcHsi2cMasterInit.u8SubAddrSize    = 2;           /* Sub address size */
    stcHsi2cMasterInit.enDir            = Hsi2cMasterReadSlaveWrite;

    stcHsi2cMasterInit.stcMasterConfig2.u32SdaFilterEnable =
        HSI2C_MASTER_FILTBPSDA_ENABLE;
    stcHsi2cMasterInit.stcMasterConfig2.u32SclFilterEnable =
        HSI2C_MASTER_FILTBPSCL_ENABLE;

    if (Ok != HSI2C_MasterInit(HSI2C, &stcHsi2cMasterInit, SystemCoreClock)) {
        while (1) {
            ;
        }
    }
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
    if (stcHsi2cCom.HSI2CxBaseAddr->MCR_f.MEN == 0x1u) {
        u32Flag                          = stcHsi2cCom.HSI2CxBaseAddr->MSR;
        stcHsi2cCom.HSI2CxBaseAddr->MSCR = u32Flag;

        if (Hsi2cComSuccess == HSI2C_MasterErrorGet(u32Flag)) {
            switch (enMasterStatusMachine) {
            case Hsi2cStatusStart:
                HSI2C_MasterStatusMachineStart();
                break;
            case Hsi2cStatusSubAddr:
                HSI2C_MasterStatusMachineSubAddr();
                break;
            case Hsi2cStatusRestart:
                HSI2C_MasterStatusMachineRestart();
                break;
            case Hsi2cStatusRxCmd:
                if ((u32Flag & HSI2C_MSR_TDF_Msk) == HSI2C_MSR_TDF_Msk) {
                    /* Disable TX interrupt */
                    if (stcHsi2cMasterInit.enDir == Hsi2cMasterReadSlaveWrite) {
                        HSI2C_MasterIntDisable(
                            stcHsi2cCom.HSI2CxBaseAddr, HSI2C_MIER_TDIE_Msk);
                    }
                    HSI2C_MasterStatusMachineRxCmd(&stcHsi2cCom.u32RxSize);
                }
                break;
            case Hsi2cStatusTransData:
                if (stcHsi2cCom.u32DataIndex
                    < stcHsi2cCom.u32Len) /* Data transmit or receive */
                {
                    if (stcHsi2cMasterInit.enDir
                        == Hsi2cMasterWriteSlaveRead) /* Transmit */
                    {
                        HSI2C_MasterWriteData(
                            stcHsi2cCom.HSI2CxBaseAddr,
                            stcHsi2cCom.pu8Buf[stcHsi2cCom.u32DataIndex++]);
                    } else /* Recieve */
                    {
                        if (Ok
                            == HSI2C_MasterReadData(
                                stcHsi2cCom.HSI2CxBaseAddr,
                                &stcHsi2cCom
                                     .pu8Buf[stcHsi2cCom.u32DataIndex])) {
                            stcHsi2cCom.u32DataIndex++;
                        }
                    }
                }
                HSI2C_MasterStatusMachineTrans();
                break;
            case Hsi2cStatusStop:
                HSI2C_MasterStatusMachineStop();
                break;
            case Hsi2cStatusWaitEnd:
                /* Wait for transfer end */
                if (u32Flag & HSI2C_MSR_SDF_Msk) {
                    HSI2C_MasterIntDisable(
                        stcHsi2cCom.HSI2CxBaseAddr, HSI2C_MASTER_INT_ALL);
                    stcHsi2cCom.enComStatus = Hsi2cComIdle;
                }
                break;
            default:
                break;
            }
        } else /* Error handling */
        {
            /* Disable all master interrupts */
            HSI2C_MasterIntDisable(
                stcHsi2cCom.HSI2CxBaseAddr, HSI2C_MASTER_INT_ALL);

            /* Arbitration lost? */
            if (u32Flag & HSI2C_MSR_ALF_Msk) {
                /* Reset master */
                HSI2C_MasterReset(stcHsi2cCom.HSI2CxBaseAddr);
            }
            /* Pin low timeout? */
            else if (u32Flag & HSI2C_MSR_PLTF_Msk) {
                /* Reset master */
                HSI2C_MasterReset(stcHsi2cCom.HSI2CxBaseAddr);
                /* ======================================= */
                /* ==========       WARNING       ======== */
                /* ======================================= */
                /* 1. 根据实际情况添加代码处理引脚低条件      */
                /* 2. 当引脚低超时持续时，PLTF不能被清除掉    */
                /* 3. 在I2C可以开始启动条件之前，必须清除PLTF */
            }
            /* Master busy? */
            else if (u32Flag & HSI2C_MSR_MBF_Msk) {
                /* Send stop command */
                HSI2C_MasterStop(stcHsi2cCom.HSI2CxBaseAddr);
            } else {
                ;
            }

            stcHsi2cCom.enComStatus = Hsi2cComIdle;
        }
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
