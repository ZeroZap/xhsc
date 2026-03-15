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
 * @brief  Source file for LVD example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l19x_lvd.h"
#include "hc32l19x_gpio.h"
#include "hc32l19x_reset.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
static void App_LvdPortInit(void);
static void App_LvdInit(void);

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/
int main(void)
{
    stc_lvd_cfg_t stcLvdCfg;

    DDL_ZERO_STRUCT(stcLvdCfg);     //变量�?

    ///< 首次上电FLAG清零
    if(TRUE == Reset_GetFlag(ResetFlagMskPor5V))
    {
        Reset_ClearFlagAll();    
    }
    
    ///< LVD 端口初始�?
    App_LvdPortInit(); 
    
    ///< 低电压复位检�?
    if(TRUE == Reset_GetFlag(ResetFlagMskLvd))
    {
        Lvd_ClearIrq();
        
        Reset_ClearFlag(ResetFlagMskLvd);
        Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);
        delay1ms(1000);
        Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);
        delay1ms(1000);
    }   
    
    ///< LVD 初始�?
    App_LvdInit();
    
    while (1)
    {
        ;
    }
}


///< LVD相关端口初始�?
static void App_LvdPortInit(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    DDL_ZERO_STRUCT(stcGpioCfg);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);    //开LVD时钟
    
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(STK_LED_PORT,STK_LED_PIN,&stcGpioCfg);
    
    ///< LVD OUT
    Gpio_Init(GpioPortA,GpioPin4,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin4,GpioAf6);
    
    ///< LVD INPUT SOURCE
    Gpio_SetAnalogMode(GpioPortB, GpioPin7);
}

static void App_LvdInit(void)
{
    stc_lvd_cfg_t stcLvdCfg;

    DDL_ZERO_STRUCT(stcLvdCfg);     //变量�?

    Sysctrl_SetPeripheralGate(SysctrlPeripheralVcLvd, TRUE);    //开LVD时钟

    stcLvdCfg.enAct        = LvdActMskReset;                ///< 配置触发产生复位
    stcLvdCfg.enInputSrc   = LvdInputSrcMskPB07;            ///< 配置LVD输入�?
    stcLvdCfg.enThreshold  = LvdMskTH1_8V;                  ///< 配置LVD基准电压
    stcLvdCfg.enFilter     = LvdFilterMskEnable;            ///< 滤波使能
    stcLvdCfg.enFilterTime = LvdFilterMsk28_8ms;            ///< 滤波时间设置
    stcLvdCfg.enIrqType    = LvdIrqMskHigh;                 ///< 复位触发类型
    Lvd_Init(&stcLvdCfg);
      
    ///< LVD 模块使能
    Lvd_Enable();
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


