/******************************************************************************
 * Copyright (C) 2021, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************/

/******************************************************************************
 * @file   main.c
 *
 * @brief  Source file for OPA example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "hc32l19x_gpio.h"
#include "hc32l19x_opa.h"
#include "hc32l19x_bgr.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
 
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
static void App_GpioInit(void);
static void App_OpaInit(void);
/**
 ******************************************************************************
 ** \brief  主函�?
 **
 ** \return �?
 ******************************************************************************/
int32_t main(void)
{   
    ///< GPIO 初始�?
    App_GpioInit();

    ///< OPA 初始�?
    App_OpaInit();
    
    while (1)
    { 
        ;
    }
}

static void App_GpioInit(void)
{
    stc_gpio_cfg_t GpioInitStruct;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);   // 使能GPIO模块时钟
    
    // 引脚配置
    GpioInitStruct.enDir = GpioDirIn;
    Gpio_Init(GpioPortB, GpioPin0, &GpioInitStruct);     // OPA_INP引脚：PB00
}

static void App_OpaInit(void)
{
    stc_opa_zcfg_t OpaInitStruct;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralOpa, TRUE);    // 使能OPA模块时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE); // 使能ADC模块时钟
    Bgr_BgrEnable();                                          // 使能BGR模块，使用OPA功能，必须使能ADC模块时钟及BGR模块
    
    Opa_Cmd(TRUE);                                       // 使能OPA通道
    
    OpaInitStruct.bAzen = TRUE;                          // 自动校零使能
    OpaInitStruct.bClk_sw_set = TRUE;                    // 软件校准使能
    OpaInitStruct.bAz_pulse = TRUE;                      // 软件校准
    Opa_SetZero(&OpaInitStruct);                         // 启动OPA软件校零
    
    delay10us(20);                                       // 延时20us，等待教零完�?
    
    Opa_ZeroBitCtrl(enAzPulse, FALSE);                   // 校零结束，清�?
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


