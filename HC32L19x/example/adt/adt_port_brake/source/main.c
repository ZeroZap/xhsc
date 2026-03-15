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
#include "hc32l19x_flash.h"

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


/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
///< 时钟初始�?
void App_ClockInit(void)
{
    en_flash_waitcycle_t      enFlashWait;
    stc_sysctrl_pll_cfg_t     stcPLLCfg;
    
    DDL_ZERO_STRUCT(stcPLLCfg);
    
    enFlashWait = FlashWaitCycle1;                      //读等待周期设置为1（当HCLK大于24MHz时必须至少为1�?
    Flash_WaitCycle(enFlashWait);                       // Flash 等待1个周�?
    
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;     //RCH 4MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  //PLL 输出48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              //输入时钟源选择RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;            //4MHz x 12 = 48MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);
    Sysctrl_SetPLLStableTime(SysctrlPllStableCycle16384);
    Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
    
    Sysctrl_SysClkSwitch(SysctrlClkPLL);   ///< 时钟切换
}

///< AdvTimer端口初始�?
void App_AdvTimerPortInit(void)
{
    stc_gpio_cfg_t        stcTIM4Port;
    stc_gpio_cfg_t        stcBrakePort;
    stc_gpio_cfg_t        stcBKTestPort;
    
    DDL_ZERO_STRUCT(stcTIM4Port);
    DDL_ZERO_STRUCT(stcBrakePort);
    DDL_ZERO_STRUCT(stcBKTestPort);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //端口外设时钟使能
    
    stcTIM4Port.enDir  = GpioDirIn;
    //PA08设置为TIM4_CHA
    Gpio_Init(GpioPortA, GpioPin8, &stcTIM4Port);
    Gpio_SetAfMode(GpioPortA,GpioPin8,GpioAf6);
    
    //PA11设置为TIM4_CHB
    Gpio_Init(GpioPortA, GpioPin11, &stcTIM4Port);
    Gpio_SetAfMode(GpioPortA,GpioPin11,GpioAf7);
    
    //PB11设置为刹车端�?
    stcBrakePort.enDir  = GpioDirIn;
    Gpio_Init(GpioPortB, GpioPin11, &stcBrakePort);
    
    //PB01设置为输出信号（给刹车端口）
    stcBKTestPort.enDir  = GpioDirOut;
    Gpio_Init(GpioPortB, GpioPin1, &stcBKTestPort);   // PB01 -> PB11
}

///< AdvTimer初始�?
void App_AdvTimerInit(uint16_t u16Period, uint16_t u16CHA_PWMDuty, uint16_t u16CHB_PWMDuty)
{
    en_adt_compare_t         enAdtCompareA;
    en_adt_compare_t         enAdtCompareB;

    stc_adt_basecnt_cfg_t    stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t  stcAdtTIM4ACfg;
    stc_adt_CHxX_port_cfg_t  stcAdtTIM4BCfg;
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM4ACfg);
    DDL_ZERO_STRUCT(stcAdtTIM4BCfg);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE);//ADT外设时钟使能
    
    stcAdtBaseCntCfg.enCntMode = AdtTriangleModeA;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div4;
    Adt_Init(M0P_ADTIM4, &stcAdtBaseCntCfg);                      //ADT载波、计数模式、时钟配�?
    
    Adt_SetPeriod(M0P_ADTIM4, u16Period);                         //周期�?
    
    enAdtCompareA = AdtCompareA;
    Adt_SetCompareValue(M0P_ADTIM4, enAdtCompareA, u16CHA_PWMDuty);    //通用比较基准值寄存器A设置
    
    enAdtCompareB = AdtCompareB;
    Adt_SetCompareValue(M0P_ADTIM4, enAdtCompareB, u16CHB_PWMDuty);    //通用比较基准值寄存器B设置
    
    stcAdtTIM4ACfg.enCap = AdtCHxCompareOutput;            //比较输出
    stcAdtTIM4ACfg.bOutEn = TRUE;                          //CHA输出使能
    stcAdtTIM4ACfg.enPerc = AdtCHxPeriodKeep;              //计数值与周期匹配时CHA电平保持不变
    stcAdtTIM4ACfg.enCmpc = AdtCHxCompareInv;              //计数值与比较值A匹配时，CHA电平翻转
    stcAdtTIM4ACfg.enStaStp = AdtCHxStateSelSS;            //CHA起始结束电平由STACA与STPCA控制
    stcAdtTIM4ACfg.enStaOut = AdtCHxPortOutLow;            //CHA起始电平为低
    stcAdtTIM4ACfg.enStpOut = AdtCHxPortOutLow;            //CHA结束电平为低
    stcAdtTIM4ACfg.enDisSel = AdtCHxDisSel3;               //选择强制输出无效条件3(刹车条件3)
    stcAdtTIM4ACfg.enDisVal = AdtTIMxDisValLow;            //刹车时CHA端口输出低电�?
    Adt_CHxXPortCfg(M0P_ADTIM4, AdtCHxA, &stcAdtTIM4ACfg);   //CHA端口配置
    
    stcAdtTIM4BCfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4BCfg.bOutEn = TRUE;
    stcAdtTIM4BCfg.enPerc = AdtCHxPeriodKeep;
    stcAdtTIM4BCfg.enCmpc = AdtCHxCompareInv;
    stcAdtTIM4BCfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4BCfg.enStaOut = AdtCHxPortOutHigh;
    stcAdtTIM4BCfg.enStpOut = AdtCHxPortOutLow;
    stcAdtTIM4BCfg.enDisSel = AdtCHxDisSel3;
    stcAdtTIM4BCfg.enDisVal = AdtTIMxDisValLow;
    Adt_CHxXPortCfg(M0P_ADTIM4, AdtCHxB, &stcAdtTIM4BCfg); //CHB端口配置
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
    stc_adt_disable_3_cfg_t  stcAdtDisable3;
    
    DDL_ZERO_STRUCT(stcAdtDisable3);
    
    App_ClockInit();     //时钟初始�?
    
    App_AdvTimerInit(0xC000, 0x4000, 0x8000);  //AdvTimer4初始�?
    //配置为三角波模式: 初始化周�?xC000, CHA占空比设�?x4000，CHB占空比设�?x8000
    
    App_AdvTimerPortInit();    //AdvTimer端口初始�?
    
/********************************高电平刹车功能测试开�?**********************************/
    
    Gpio_WriteOutputIO(GpioPortB, GpioPin1, FALSE);   //PB01输出�?
    
    //cfg Port high break
    stcAdtDisable3.stcBrkPtCfg[AdtTrigxSelPB11].bPortEn = TRUE;        //PB11配置为刹车端�?
    stcAdtDisable3.stcBrkPtCfg[AdtTrigxSelPB11].enPol = AdtPtBrkHigh;  //刹车电平：高电平有效
    stcAdtDisable3.bFltEn = TRUE;
    stcAdtDisable3.enFltClk = AdtFltClkPclk0;
    Adt_Disable3Cfg(&stcAdtDisable3);                                  //失效条件3端口刹车初始化配�?

    
    Adt_StartCount(M0P_ADTIM4); 
    
    delay1ms(1000);
    
    Gpio_WriteOutputIO(GpioPortB, GpioPin1, TRUE);                    //PB01输出�?->PB11,使能Port brake，PWM输出指定的电�?低电�?
    
    delay1ms(1000);
        
    Gpio_WriteOutputIO(GpioPortB, GpioPin1, FALSE);                   //PB01输出�?
    
    delay1ms(1);
    
    Adt_ClearPortBrakeFlag();                                         //PB01输出低以后，再清除端口刹车标志，恢复PWM输出
      
    // disable port break
    DDL_ZERO_STRUCT(stcAdtDisable3);
    stcAdtDisable3.stcBrkPtCfg[AdtTrigxSelPB11].bPortEn = FALSE;
    Adt_Disable3Cfg(&stcAdtDisable3);                                 //端口刹车功能解除
    
    delay1ms(1000);
/********************************高电平刹车功能测试结�?**********************************/
    
    
/********************************低电平刹车功能测试开�?**********************************/
    delay1ms(1000);
    
    Gpio_WriteOutputIO(GpioPortB, GpioPin1, TRUE);                    //PB01输出�?
    
    //cfg port low break
    stcAdtDisable3.stcBrkPtCfg[AdtTrigxSelPB11].bPortEn = TRUE;       //PB11配置为刹车端�?
    stcAdtDisable3.stcBrkPtCfg[AdtTrigxSelPB11].enPol = AdtPtBrkLow;  //刹车电平：低电平有效
    stcAdtDisable3.bFltEn = TRUE;
    stcAdtDisable3.enFltClk = AdtFltClkPclk0Div4;
    Adt_Disable3Cfg(&stcAdtDisable3);                                 //失效条件3：端口刹车初始化配置
    
    delay1ms(1000);
        
    Gpio_WriteOutputIO(GpioPortB, GpioPin1, FALSE);                   //PB01输出�?->PB11,使能Port brake，PWM输出指定的电�?低电�?
    
    delay1ms(1000);

    Gpio_WriteOutputIO(GpioPortB, GpioPin1, TRUE);                    //PB01输出�?
    
    delay1ms(1);
    
    Adt_ClearPortBrakeFlag();                                         //PB01输出高以后，再清除端口刹车标志，恢复PWM输出

    // disable port break
    DDL_ZERO_STRUCT(stcAdtDisable3);
    stcAdtDisable3.stcBrkPtCfg[AdtTrigxSelPB11].bPortEn = FALSE;
    Adt_Disable3Cfg(&stcAdtDisable3);                                //端口刹车功能解除
    
    delay1ms(1000);  
/********************************低电平刹车功能测试结�?**********************************/


/********************************软件刹车功能测试开�?**********************************/
    delay1ms(1000);  
    
    //cfg software break 
    Adt_SwBrake(TRUE);  //软件刹车
    
    delay1ms(2000); 
    
    Adt_SwBrake(FALSE);  //软件刹车解除，恢复PWM输出
    
    delay1ms(2000); 
/********************************软件刹车功能测试结束***********************************/
    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


