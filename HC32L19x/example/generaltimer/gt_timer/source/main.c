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
 * @brief  Source file for General Timer example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "hc32l19x_bt.h"
#include "hc32l19x_gpio.h"
#include "hc32l19x_flash.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
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

/*******************************************************************************
 * TIM0中断服务函数
 ******************************************************************************/
void Tim0_IRQHandler(void)
{
    static uint8_t i;
    
    //Timer0 模式0 溢出中断
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {
        if(i == 0)
        {
            Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);   //LED 引脚输出高电�?
            
            i = 1;
        }
        else
        {
            Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);  //LED引脚输出低电�?
            
            i = 0;
        }
        
        Bt_ClearIntFlag(TIM0,BtUevIrq); //中断标志清零
    }
}

//时钟配置初始�?
void App_ClockCfg(void)
{
    en_flash_waitcycle_t      enWaitCycle;
    stc_sysctrl_pll_cfg_t     stcPLLCfg;
    
    //结构体初始化清零
    DDL_ZERO_STRUCT(stcPLLCfg);
    
    enWaitCycle = FlashWaitCycle1;
    Flash_WaitCycle(enWaitCycle);
    
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;     //RCH 4MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  //PLL 输出48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              //输入时钟源选择RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;            //4MHz x 12 = 48MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);
    Sysctrl_SetPLLStableTime(SysctrlPllStableCycle16384);
    Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
    
    Sysctrl_SysClkSwitch(SysctrlClkPLL);  ///< 时钟切换
}

//端口配置初始�?
void App_Timer0PortCfg(void)
{
    stc_gpio_cfg_t         stcLEDPortCfg;
    
    DDL_ZERO_STRUCT(stcLEDPortCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //GPIO 外设时钟使能
    
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);
    stcLEDPortCfg.enDir  = GpioDirOut;
    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &stcLEDPortCfg);   //PD14设置为LED 控制引脚
}

//Timer0配置初始�?
void App_Timer0Cfg(uint16_t u16Period)
{
    uint16_t                  u16ArrValue;
    uint16_t                  u16CntValue;
    stc_bt_mode0_cfg_t     stcBtBaseCfg;
    
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer外设时钟使能
    
    stcBtBaseCfg.enWorkMode = BtWorkMode0;                  //定时器模�?
    stcBtBaseCfg.enCT       = BtTimer;                      //定时器功能，计数时钟为内部PCLK
    stcBtBaseCfg.enPRS      = BtPCLKDiv256;                 //PCLK/256
    stcBtBaseCfg.enCntMode  = Bt16bitArrMode;               //自动重载16位计数器/定时�?
    stcBtBaseCfg.bEnTog     = FALSE;
    stcBtBaseCfg.bEnGate    = FALSE;
    stcBtBaseCfg.enGateP    = BtGatePositive;
    Bt_Mode0_Init(TIM0, &stcBtBaseCfg);                     //TIM0 的模�?功能初始�?
    
    u16ArrValue = 0x10000 - u16Period;
    Bt_M0_ARRSet(TIM0, u16ArrValue);                        //设置重载�?ARR = 0x10000 - 周期)
    
    u16CntValue = 0x10000 - u16Period;
    Bt_M0_Cnt16Set(TIM0, u16CntValue);                      //设置计数初�?
    
    Bt_ClearIntFlag(TIM0,BtUevIrq);                         //清中断标�?  
    Bt_Mode0_EnableIrq(TIM0);                               //使能TIM0中断(模式0时只有一个中�?
    EnableNvic(TIM0_IRQn, IrqLevel3, TRUE);                 //TIM0中断使能
}
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
    App_ClockCfg();         //时钟配置初始�?
    
    App_Timer0PortCfg();     //端口配置初始�?
    
    App_Timer0Cfg(48000);   //Timer0配置初始�?周期 = 48000*(1/48M)*256 = 256ms)
    
    Bt_M0_Run(TIM0);        //TIM0 运行�?
    
    while (1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


