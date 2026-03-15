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
 * @brief  Source file for ADT example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l19x_adt.h"
#include "hc32l19x_gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/


/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
uint16_t u16Capture;
uint16_t u16CaptureBuf;
uint16_t u16CapDuty;
/******************************************************************************
 * Local type definitions ('typedef')                                         
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/


/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

void Tim4_IRQHandler(void)
{
    //AdvTimer4 捕获中断A
    if(TRUE == Adt_GetIrqFlag(M0P_ADTIM4, AdtCMAIrq))
    {    
        Adt_GetCaptureValue(M0P_ADTIM4, AdtCHxA, &u16Capture);  //读取捕获值A
        Adt_GetCaptureBuf(M0P_ADTIM4, AdtCHxA, &u16CaptureBuf);  //读取捕获值A的缓存�?
        
        if(1 == Gpio_GetInputIO(GpioPortA, GpioPin8))      //PA08为高电平�?
        {
            u16CapDuty = (u16CaptureBuf*100) / u16Capture;      //计算占空�?
        }
        
        Adt_ClearIrqFlag(M0P_ADTIM4, AdtCMAIrq);    //清除捕获中断A的标�?
    }
}

///< AdvTimer端口初始�?
void App_AdtPortInit(void)
{
    stc_gpio_cfg_t           stcTIM4Port;
    
    DDL_ZERO_STRUCT(stcTIM4Port);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //端口外设时钟使能
    
    //PA08设置为TIM4_CHA
    stcTIM4Port.enDir  = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin8, &stcTIM4Port);
    Gpio_SetAfMode(GpioPortA,GpioPin8,GpioAf6);
}

///< AdvTimer初始�?
void App_AdvTimerInit(void)
{
    stc_adt_basecnt_cfg_t    stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t  stcAdtTIM4ACfg;
    stc_adt_CHxX_port_cfg_t  stcAdtTIM4BCfg;

    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM4ACfg);
    DDL_ZERO_STRUCT(stcAdtTIM4BCfg);
    

    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE); //ADT外设时钟使能
    
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;            //Sawtooth Mode
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;                    // Cnt up
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div4;          // PCLK0/4
    Adt_Init(M0P_ADTIM4, &stcAdtBaseCntCfg);                 //ADT载波、计数模式、时钟配�?
    
    Adt_SetPeriod(M0P_ADTIM4, 0xFFFF);                       //周期设置
    
    stcAdtTIM4ACfg.enCap = AdtCHxCompareInput;               //ChannelA 捕获输入
    stcAdtTIM4ACfg.bFltEn = TRUE;
    stcAdtTIM4ACfg.enFltClk = AdtFltClkPclk0Div16;
    Adt_CHxXPortCfg(M0P_ADTIM4, AdtCHxA, &stcAdtTIM4ACfg);  //ChannelA配置捕获输入 & GPIO CHA 输入滤波使能
    
    Adt_EnableValueBuf(M0P_ADTIM4, AdtCHxA, TRUE);          //缓存传送功能打开
    
    
    Adt_CfgHwCaptureA(M0P_ADTIM4, AdtHwTrigCHxARise);  //硬件捕获A条件配置:
    Adt_CfgHwCaptureA(M0P_ADTIM4, AdtHwTrigCHxAFall);  //硬件捕获A条件配置:
    
    Adt_CfgHwClear(M0P_ADTIM4, AdtHwTrigCHxARise);     //硬件清零条件：CHA 端口采样到上升沿
    Adt_EnableHwClear(M0P_ADTIM4);
    
    
    Adt_ClearAllIrqFlag(M0P_ADTIM4);
    Adt_CfgIrq(M0P_ADTIM4, AdtCMAIrq, TRUE);           //捕获输入A中断配置
    EnableNvic(ADTIM4_IRQn, IrqLevel3, TRUE);          //AdvTimer4中断使能
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
    App_AdtPortInit();         //AdvTimer4 端口初始�?
    
    App_AdvTimerInit();        //AdvTimer4 初始�?
    
    Adt_StartCount(M0P_ADTIM4);   //AdvTimer4 运行
    
    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


