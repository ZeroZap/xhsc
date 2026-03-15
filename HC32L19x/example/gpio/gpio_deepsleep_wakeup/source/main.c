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
#include "hc32l19x_lpm.h"
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
static void App_LedInit(void);
static void App_UserKeyInit(void);
static void App_LowPowerModeGpioSet(void);
static void _UserKeyWait(void);

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
    App_LedInit();
    
    ///< KEY 端口初始�?
    App_UserKeyInit();
    
    ///< ===============================================
    ///< ============ 警告，警告，警告！！�?============
    ///< ===============================================
    ///< 本样例程序会进入深度休眠模式，因此以下两行代码起防护作用（防止进入深�?
    ///< 休眠后芯片调试功能不能再次使用）�?
    ///< 在使用本样例时，禁止在没有唤醒机制的情况下删除以下两行代码�?
    delay1ms(2000);
    _UserKeyWait();     ///< 等待按键按下后进入休眠模�?
    
    ///< 配置Demo板上所有不使用的IO为高阻输�?避免端口漏电
    App_LowPowerModeGpioSet();
     
    ///< 打开并配置按键端口为下降沿中�?
    Gpio_EnableIrq(STK_USER_PORT, STK_USER_PIN, GpioIrqFalling);
    EnableNvic(PORTA_IRQn, IrqLevel3, TRUE);
    
    ///< 进入低功耗模式——深度休眠（使能唤醒后退出中断自动休眠特性）
    Lpm_GotoDeepSleep(TRUE);
    
    while(1)
    {
        ;
    }
}



///< PortA 中断服务函数
void PortA_IRQHandler(void)
{
    if(TRUE == Gpio_GetIrqStatus(STK_USER_PORT, STK_USER_PIN))
    {            
        ///< LED点亮
        Gpio_SetIO(STK_LED_PORT, STK_LED_PIN);
        
        delay1ms(2000);
        
        ///< LED关闭
        Gpio_ClrIO(STK_LED_PORT, STK_LED_PIN);  

        Gpio_ClearIrq(STK_USER_PORT, STK_USER_PIN);    
    }

}    


static void _UserKeyWait(void)
{    
    while(1)
    {
        ///< 检测电�?USER按键是否按下(低电�?)
        if(FALSE == Gpio_GetInputIO(STK_USER_PORT, STK_USER_PIN))
        {
            break;
        }
        else
        {
            continue;
        }
    }
}


static void App_LowPowerModeGpioSet(void)
{
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    //swd as gpio
    Sysctrl_SetFunc(SysctrlSWDUseIOEn, TRUE);
    
    ///< 配置为数字端�?
    M0P_GPIO->PAADS = 0;
    M0P_GPIO->PBADS = 0;
    M0P_GPIO->PCADS = 0;
    M0P_GPIO->PDADS = 0;
    M0P_GPIO->PEADS = 0;
    M0P_GPIO->PFADS = 0;
    
    ///< 配置为端口输入（除LED端口外）
    M0P_GPIO->PADIR = 0XFFFF;
    M0P_GPIO->PBDIR = 0XFFFF;
    M0P_GPIO->PCDIR = 0XFFFF;
    M0P_GPIO->PDDIR = 0XBFFF;
    M0P_GPIO->PEDIR = 0XFFFF;
    M0P_GPIO->PFDIR = 0XFFFF;
    
    ///< 输入下拉（除KEY端口以外�?
    M0P_GPIO->PAPD = 0xFF7F;
    M0P_GPIO->PBPD = 0xFFFF;
    M0P_GPIO->PCPD = 0xFFFF;
    M0P_GPIO->PDPD = 0xFFFF;
    M0P_GPIO->PEPD = 0xFFFF;
    M0P_GPIO->PFPD = 0xFFFF;
    
}

static void App_UserKeyInit(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    ///< 端口方向配置->输入
    stcGpioCfg.enDir = GpioDirIn;
    ///< 端口驱动能力配置->高驱动能�?
    stcGpioCfg.enDrv = GpioDrvL;
    ///< 端口上下拉配�?>�?
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdDisable;
    ///< 端口开漏输出配�?>开漏输出关�?
    stcGpioCfg.enOD = GpioOdDisable;
    ///< 端口输入/输出值寄存器总线控制模式配置->AHB
    stcGpioCfg.enCtrlMode = GpioAHB;
    ///< GPIO IO USER KEY初始�?
    Gpio_Init(STK_USER_PORT, STK_USER_PIN, &stcGpioCfg);    
    

}

static void App_LedInit(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    ///< 端口方向配置->输出(其它参数与以上（输入）配置参数一�?
    stcGpioCfg.enDir = GpioDirOut;
    ////< 端口上下拉配�?>�?
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdDisable;
    ///< GPIO IO LED端口初始�?
    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &stcGpioCfg);
    ///< LED关闭
    Gpio_ClrIO(STK_LED_PORT, STK_LED_PIN);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


