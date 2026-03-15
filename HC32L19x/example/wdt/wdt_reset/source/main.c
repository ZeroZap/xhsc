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
 * @brief  Source file for WDT example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/

#include "hc32l19x_wdt.h"
#include "hc32l19x_lpm.h"
#include "hc32l19x_gpio.h"

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
static void App_GpioInit(void);
static void App_WdtInit(void);

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/
int32_t main(void)
{
    ///< LED 端口初始�?
    App_GpioInit();

    ///< WDT 初始�?
    App_WdtInit();
   
    ///< 启动 WDT
    Wdt_Start();

    while (1)
    {
        ///< 开启喂狗后，将不会产生复位
        delay1ms(800);
        //Wdt_Feed();   // 喂狗
        Gpio_WriteOutputIO(STK_LED_PORT,STK_LED_PIN,FALSE);
    }
}


static void App_WdtInit(void)
{
    ///< 开启WDT外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralWdt,TRUE);
    ///< WDT 初始�?
    Wdt_Init(WdtResetEn, WdtT820ms);
}


static void App_GpioInit(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    ///< 开启GPIO外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); 
    
    ///< LED 初始�?
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_WriteOutputIO(STK_LED_PORT,STK_LED_PIN,TRUE);
    Gpio_Init(STK_LED_PORT,STK_LED_PIN,&stcGpioCfg);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


