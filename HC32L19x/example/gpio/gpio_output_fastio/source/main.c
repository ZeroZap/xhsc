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
 * @brief  Source file for GPIO example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
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
#define LED_TOGGLE8()      {M0P_GPIO->PDOUT = u32High;\
                            M0P_GPIO->PDOUT = u32Low;\
                            M0P_GPIO->PDOUT = u32High;\
                            M0P_GPIO->PDOUT = u32Low;\
                            M0P_GPIO->PDOUT = u32High;\
                            M0P_GPIO->PDOUT = u32Low;\
                            M0P_GPIO->PDOUT = u32High;\
                            M0P_GPIO->PDOUT = u32Low;}

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
static void App_LedInit(void);


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
    uint32_t u32High = 0x00004000;  ///< PD14
    uint32_t u32Low  = 0x00000000;
  
    ///< LED端口初始�?
    App_LedInit();

    while(1)
    {
        ///< LED 翻转输出�?MHz（HCLK = RCH4MHz�?
        LED_TOGGLE8();
        LED_TOGGLE8();
        LED_TOGGLE8();
        LED_TOGGLE8();
        LED_TOGGLE8();
        LED_TOGGLE8();
        LED_TOGGLE8();
        LED_TOGGLE8();        
    }
}



static void App_LedInit(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    ///< 端口方向配置->输出(其它参数与以上（输入）配置参数一�?
    stcGpioCfg.enDir = GpioDirOut;
    ///< 端口上下拉配�?>下拉
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdDisable;
    
    ///< LED关闭
    Gpio_ClrIO(STK_LED_PORT, STK_LED_PIN);
    
    ///< GPIO IO LED端口初始�?
    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &stcGpioCfg);
    

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


