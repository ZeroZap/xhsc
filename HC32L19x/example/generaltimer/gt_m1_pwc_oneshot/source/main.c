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
#include "hc32l19x_flash.h"
#include "hc32l19x_gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
volatile uint32_t u32PwcCapValue;
volatile uint16_t u16TIM0_CntValue;

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
    static uint16_t u16TIM0_OverFlowCnt;
    
    static uint16_t u16TIM0_CapValue;
    
    //Timer0 模式1 计数溢出中断
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {
        u16TIM0_OverFlowCnt++;        //计数脉宽测量开始边沿到结束边沿过程中timer的溢出次�?
        
        Bt_ClearIntFlag(TIM0,BtUevIrq); //清除中断标志
    }

    //Timer0 模式1 PWC脉宽测量中断
    if(TRUE == Bt_GetIntFlag(TIM0, BtCA0Irq))
    {
        u16TIM0_CntValue = Bt_M1_Cnt16Get(TIM0);         //读取当前计数�?
        u16TIM0_CapValue = Bt_M1_PWC_CapValueGet(TIM0);  //读取脉宽测量�?
        
        u32PwcCapValue = u16TIM0_OverFlowCnt*0x10000 + u16TIM0_CapValue;
        
        u16TIM0_OverFlowCnt = 0;
        
        Bt_ClearIntFlag(TIM0, BtCA0Irq); //清除中断标志
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
    stc_gpio_cfg_t          stcTIM0Port;
    
    //结构体初始化清零
    DDL_ZERO_STRUCT(stcTIM0Port);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //GPIO 外设时钟使能
    
    //PA00设置为TIM0_CHA
    stcTIM0Port.enDir  = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin0, &stcTIM0Port);
    Gpio_SetAfMode(GpioPortA,GpioPin0,GpioAf7);
    
    //PA01设置为TIM0_CHB
    //Gpio_Init(GpioPortA, GpioPin1, &stcTIM0Port);
    //Gpio_SetAfMode(GpioPortA,GpioPin1,GpioAf3);
}

//Timer0 配置
void App_Timer0Cfg(void)
{
    uint16_t                u16CntValue;
    stc_bt_mode1_cfg_t      stcBtBaseCfg;
    stc_bt_pwc_input_cfg_t  stcBtPwcInCfg;

    //结构体初始化清零
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcBtPwcInCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer外设时钟使能
    
    stcBtBaseCfg.enWorkMode = BtWorkMode1;                  //定时器模�?
    stcBtBaseCfg.enCT       = BtTimer;                      //定时器功能，计数时钟为内部PCLK
    stcBtBaseCfg.enPRS      = BtPCLKDiv1;                   //PCLK
    stcBtBaseCfg.enOneShot  = BtPwcOneShotDetect;           //PWC循环检�?
    
    Bt_Mode1_Init(TIM0, &stcBtBaseCfg);                     //TIM0 的模�?功能初始�?
    
    stcBtPwcInCfg.enTsSel  = BtTs6IAFP;                     //PWC输入选择 CHA
    stcBtPwcInCfg.enIA0Sel = BtIA0Input;                    //CHA选择IA0
    stcBtPwcInCfg.enFltIA0 = BtFltPCLKDiv16Cnt3;            //PCLK/16 3个连续有�?
    //stcBtPwcInCfg.enIB0Sel = BtIB0Input;                  //CHB选择IB0
    //stcBtPwcInCfg.enFltIB0 = BtFltPCLKDiv16Cnt3;          //PCLK/16 3个连续有�?
    Bt_M1_Input_Cfg(TIM0, &stcBtPwcInCfg);                  //PWC输入设置
    
    M0P_TIM0_MODE23->CRCH0_f.CSA = 1;                       //CHA设置为捕获模�?

    Bt_M1_PWC_Edge_Sel(TIM0, BtPwcRiseToFall);              //上升沿到下降沿捕�?

    u16CntValue = 0;
    Bt_M1_Cnt16Set(TIM0, u16CntValue);                      //设置计数初�?

    Bt_ClearIntFlag(TIM0,BtUevIrq);                         //清Uev中断标志
    Bt_ClearIntFlag(TIM0,BtCA0Irq);                         //清捕捉中断标�?
    Bt_Mode1_EnableIrq(TIM0, BtUevIrq);                     //使能TIM0溢出中断
    Bt_Mode1_EnableIrq(TIM0, BtCA0Irq);                     //使能TIM0捕获中断
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
    
    App_Timer0PortCfg();    //端口配置初始�?
    
    App_Timer0Cfg();        //Timer0配置初始�?
    
    Bt_M1_Run(TIM0);        //TIM0 运行�?
    
    while (1)
    {
        delay1ms(2000);
        Bt_M1_Run(TIM0);    //重复使能PWC功能
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


