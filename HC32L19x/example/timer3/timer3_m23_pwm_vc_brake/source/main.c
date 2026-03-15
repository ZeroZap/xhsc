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
 * @brief  Source file for TIMER3 example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "hc32l19x_timer3.h"
#include "hc32l19x_gpio.h"
#include "hc32l19x_flash.h"
#include "hc32l19x_adc.h"
#include "hc32l19x_vc.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
volatile uint16_t u16AdcResult02;
volatile uint16_t u16AdcResult03;
volatile uint16_t u16AdcResult05;
volatile uint16_t u16MainLoopCnt;
volatile uint16_t u16MainLoopFlag;
volatile uint16_t u16BkDelayCnt;
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
 * TIM3中断服务函数
 ******************************************************************************/
void Tim3_IRQHandler(void)
{
    static uint8_t i;
    
    //Timer3 模式23 更新中断
    if(TRUE == Tim3_GetIntFlag(Tim3UevIrq))
    {
        if(0 == i)
        {
            Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN,TRUE);  //LED 引脚输出高电�?
            i++;
        }
        else if(1 == i)
        {
            Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN,FALSE); //LED 引脚输出低电�?
            i = 0;
        }
        
        Tim3_ClearIntFlag(Tim3UevIrq);  //清中断标�?
    }
}

/*******************************************************************************
 * ADC中断服务函数
 ******************************************************************************/
void Adc_IRQHandler(void)
{
    static uint8_t i;
  
    //ADC 插队(JQR)扫描采样完成中断
    if(TRUE == Adc_GetIrqStatus(AdcMskIrqJqr))
    {
        u16AdcResult02 = (uint16_t)Adc_GetJqrResult(AdcJQRCH0MUX);//读取JQR通道0(PA02)的采样�?
        u16AdcResult03 = (uint16_t)Adc_GetJqrResult(AdcJQRCH1MUX);//读取JQR通道1(PA03)的采样�?
        u16AdcResult05 = (uint16_t)Adc_GetJqrResult(AdcJQRCH2MUX);//读取JQR通道2(PA05)的采样�?
    
        if(0 == i)
        {
            Tim3_M23_CCR_Set(Tim3CCR0A, 0xA00); //设置CH0 通道A比较�?
            Tim3_M23_CCR_Set(Tim3CCR1A, 0xA00); //设置CH1 通道A比较�?
            Tim3_M23_CCR_Set(Tim3CCR2A, 0xA00); //设置CH2 通道A比较�?
            i++;
        }
        else if(1 == i)
        {
            Tim3_M23_CCR_Set(Tim3CCR0A, 0x600); //设置CH0 通道A比较�?
            Tim3_M23_CCR_Set(Tim3CCR1A, 0x600); //设置CH1 通道A比较�?
            Tim3_M23_CCR_Set(Tim3CCR2A, 0x600); //设置CH2 通道A比较�?
            i = 0;
        }
        
        u16MainLoopCnt++;
        if(u16MainLoopCnt >= 10)    //为主循环延时1ms
        {
            u16MainLoopCnt = 0;
            u16MainLoopFlag = 1;
        }
        
        Adc_ClrIrqStatus(AdcMskIrqJqr);  //清中断标�?
    }
}

/*******************************************************************************
 * VC1中断服务函数
 ******************************************************************************/
void Vc1_IRQHandler(void)
{
    //VC1中断
    if(TRUE == Vc_GetItStatus(Vc1_Intf))
    {
        //当VC高电平中断有效时，触发Timer3PWM刹车动作，MOE会自动清零，（AOE不要使能�?
        //当MOE自动清零，PWM输出就会关断，若需要恢复PWM输出，则需要再次使能MOE
        
        Vc_ClearItStatus(Vc1_Intf);
    }
}

//时钟初始�?
void App_ClockCfg(void)
{
    en_flash_waitcycle_t      enFlashWait;
    stc_sysctrl_pll_cfg_t     stcPLLCfg;
    
    //结构体初始化清零
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
    
    Sysctrl_SysClkSwitch(SysctrlClkPLL);///< 时钟切换
}


//Timer3 Port端口配置
void App_Timer3PortCfg(void)
{
    stc_gpio_cfg_t               stcTIM3Port;
    stc_gpio_cfg_t               stcLEDPort;
    stc_gpio_cfg_t               stcBKPort;
    
    //结构体初始化清零
    DDL_ZERO_STRUCT(stcTIM3Port);
    DDL_ZERO_STRUCT(stcLEDPort);
    DDL_ZERO_STRUCT(stcBKPort);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //端口外设时钟使能
    
    stcLEDPort.enDir  = GpioDirOut;
    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &stcLEDPort);        //PD14设置为LED输出
    
    stcBKPort.enDir = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin6, &stcBKPort);
    Gpio_SetAfMode(GpioPortA,GpioPin6,GpioAf3);               //PA06作为刹车输入端口
    
    stcTIM3Port.enDir  = GpioDirOut;
    
    Gpio_Init(GpioPortA, GpioPin8, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortA,GpioPin8,GpioAf2);               //PA08设置为TIM3_CH0A
    
    Gpio_Init(GpioPortA, GpioPin7, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortA,GpioPin7,GpioAf4);               //PA07设置为TIM3_CH0B
    
    Gpio_Init(GpioPortB, GpioPin10, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortB,GpioPin10,GpioAf5);              //PB10设置为TIM3_CH1A
    
    Gpio_Init(GpioPortB, GpioPin0, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortB,GpioPin0,GpioAf2);               //PB00设置为TIM3_CH1B
    
    Gpio_Init(GpioPortB, GpioPin8, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortB,GpioPin8,GpioAf6);               //PB08设置为TIM3_CH2A
    
    Gpio_Init(GpioPortB, GpioPin15, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortB,GpioPin15,GpioAf2);              //PB15设置为TIM3_CH2B
}

//Timer3 配置
void App_Timer3Cfg(uint16_t u16Period, uint16_t u16CHxACompare, uint16_t u16CHxBCompare)
{
    uint16_t                     u16CntValue;
    uint8_t                      u8ValidPeriod;
    stc_tim3_mode23_cfg_t        stcTim3BaseCfg;
    stc_tim3_m23_compare_cfg_t   stcTim3PortCmpCfg;
    stc_tim3_m23_adc_trig_cfg_t  stcTim3TrigAdc;
    stc_tim3_m23_dt_cfg_t        stcTim3DeadTimeCfg;
    stc_tim3_m23_bk_input_cfg_t  stcBkCfg;
    
    //结构体初始化清零
    DDL_ZERO_STRUCT(stcTim3BaseCfg);
    DDL_ZERO_STRUCT(stcTim3PortCmpCfg);
    DDL_ZERO_STRUCT(stcTim3TrigAdc);
    DDL_ZERO_STRUCT(stcTim3DeadTimeCfg);
    DDL_ZERO_STRUCT(stcBkCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3, TRUE);   //Timer3外设时钟使能
        
    stcTim3BaseCfg.enWorkMode    = Tim3WorkMode3;             //三角波模�?
    stcTim3BaseCfg.enCT          = Tim3Timer;                 //定时器功能，计数时钟为内部PCLK
    stcTim3BaseCfg.enPRS         = Tim3PCLKDiv1;              //PCLK
    //stcTim3BaseCfg.enCntDir    = Tim3CntUp;                 //向上计数，在三角波模式时只读
    stcTim3BaseCfg.enPWMTypeSel  = Tim3ComplementaryPWM;      //互补输出PWM
    stcTim3BaseCfg.enPWM2sSel    = Tim3SinglePointCmp;        //单点比较功能
    stcTim3BaseCfg.bOneShot      = FALSE;                     //循环计数
    stcTim3BaseCfg.bURSSel       = FALSE;                     //上下溢更�?
    
    Tim3_Mode23_Init(&stcTim3BaseCfg);                        //TIM3 的模�?3功能初始�?
    
    //48M, f = 10K,
    Tim3_M23_ARRSet(u16Period, TRUE);                         //设置重载�?并使能缓�?
    
    Tim3_M23_CCR_Set(Tim3CCR0A, u16CHxACompare);              //设置比较值A,(PWM互补模式下只需要设置比较值A)
    Tim3_M23_CCR_Set(Tim3CCR1A, u16CHxACompare);
    Tim3_M23_CCR_Set(Tim3CCR2A, u16CHxACompare);
    
    stcTim3PortCmpCfg.enCHxACmpCtrl   = Tim3PWMMode2;         //OCREFA输出控制OCMA:PWM模式2
    stcTim3PortCmpCfg.enCHxAPolarity  = Tim3PortPositive;     //正常输出
    stcTim3PortCmpCfg.bCHxACmpBufEn   = TRUE;                 //A通道缓存控制
    stcTim3PortCmpCfg.enCHxACmpIntSel = Tim3CmpIntNone;       //A通道比较控制:�?
    
    stcTim3PortCmpCfg.enCHxBCmpCtrl   = Tim3PWMMode2;         //OCREFB输出控制OCMB:PWM模式2(PWM互补模式下也要设置，避免强制输出)
    stcTim3PortCmpCfg.enCHxBPolarity  = Tim3PortPositive;     //正常输出
    //stcTim3PortCmpCfg.bCHxBCmpBufEn = TRUE;                 //B通道缓存控制使能
    stcTim3PortCmpCfg.enCHxBCmpIntSel = Tim3CmpIntNone;       //B通道比较控制:�?
    
    Tim3_M23_PortOutput_Cfg(Tim3CH0, &stcTim3PortCmpCfg);  //比较输出端口配置
    Tim3_M23_PortOutput_Cfg(Tim3CH1, &stcTim3PortCmpCfg);  //比较输出端口配置
    Tim3_M23_PortOutput_Cfg(Tim3CH2, &stcTim3PortCmpCfg);  //比较输出端口配置
    
    stcTim3TrigAdc.bEnTrigADC    = TRUE;       //使能ADC触发全局控制
    stcTim3TrigAdc.bEnUevTrigADC = TRUE;       //Uev更新触发ADC
    Tim3_M23_TrigADC_Cfg(&stcTim3TrigAdc);     //触发ADC配置
    
    
    stcTim3DeadTimeCfg.bEnDeadTime      = TRUE;
    stcTim3DeadTimeCfg.u8DeadTimeValue  = 0xFF;
    Tim3_M23_DT_Cfg(&stcTim3DeadTimeCfg);       //死区设置
    
    u8ValidPeriod = 1;                          //事件更新周期设置�?表示三角波每半个周期更新一次，�?1代表延迟半个周期
    Tim3_M23_SetValidPeriod(u8ValidPeriod);     //间隔周期设置
    
    stcBkCfg.enBkCH0AStat = Tim3CHxBksLow;      //刹车使能时，CHx端口输出低电�?
    stcBkCfg.enBkCH0BStat = Tim3CHxBksLow;
    stcBkCfg.enBkCH1AStat = Tim3CHxBksLow;
    stcBkCfg.enBkCH1BStat = Tim3CHxBksLow;
    stcBkCfg.enBkCH2AStat = Tim3CHxBksLow;
    stcBkCfg.enBkCH2BStat = Tim3CHxBksLow;
    stcBkCfg.bEnVCBrake = TRUE;                 //Vc 刹车使能
    //stcBkCfg.bEnBrake = TRUE;
    Tim3_M23_BrakeInput_Cfg(&stcBkCfg);         //刹车功能配置
    
    u16CntValue = 0;
    Tim3_M23_Cnt16Set(u16CntValue);             //设置计数初�?
    
    Tim3_ClearAllIntFlag();                     //清中断标�?
    Tim3_Mode23_EnableIrq(Tim3UevIrq);          //使能TIM3 UEV更新中断
    EnableNvic(TIM3_IRQn, IrqLevel0, TRUE);     //TIM3中断使能
}



/**
 ******************************************************************************
 ** \brief  VC cfg
 **
 
 **
 ** This sample
 **
 ******************************************************************************/
void App_VcCfg(void)
{
    stc_gpio_cfg_t        stcVC1PInPort;
    stc_gpio_cfg_t        stcVC1OutPort;
    stc_vc_channel_cfg_t  stcChannelCfg;
    stc_vc_dac_cfg_t      stcVcDacCfg;
    
    //结构体初始化清零
    DDL_ZERO_STRUCT(stcChannelCfg);
    DDL_ZERO_STRUCT(stcVcDacCfg);
    DDL_ZERO_STRUCT(stcVC1PInPort);
    DDL_ZERO_STRUCT(stcVC1OutPort);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralVcLvd, TRUE);
        
    //PB14设置为VC1_IN_P
    Gpio_SetAnalogMode(GpioPortB, GpioPin14);
    
    //PA02设置为VC1_OUT
    stcVC1OutPort.enDir  = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin2, &stcVC1OutPort);
    Gpio_SetAfMode(GpioPortA,GpioPin2,GpioAf3);
    
    stcVcDacCfg.bDivEn = TRUE;
    stcVcDacCfg.u8DivVal =0x10;                       //Vc_Ref = (16/64)*Vcc
    stcVcDacCfg.enDivVref = VcDivVrefAvcc;
    Vc_DacInit(&stcVcDacCfg);
    
    stcChannelCfg.enVcChannel      = VcChannel1;    //通道1
    stcChannelCfg.enVcCmpDly       = VcDelayoff;    //迟滞关闭
    stcChannelCfg.enVcBiasCurrent  = VcBias10ua;    //偏置电流10ua
    stcChannelCfg.enVcFilterTime   = VcFilter7us;   //输出滤波时间7us
    stcChannelCfg.bFlten           = FALSE;         //滤波功能关闭
    stcChannelCfg.enVcInPin_P      = VcInPCh11;     //VC1_IN_P 选择PB14
    stcChannelCfg.enVcInPin_N      = ResDivOut;     //VC1_IN_N 选择VC自带6bitDAC(Vc_DIV)
    stcChannelCfg.enVcOutCfg    = VcOutTIMBK;       //结果使能输出到Timer(012)3刹车
    
    Vc_Init(&stcChannelCfg);                        //VC1初始�?
    
    Vc_CfgItType(VcChannel1, VcIrqHigh);           //高电平触发中�?
    
    Vc_ClearItStatus(Vc1_Intf);
    Vc_ItCfg(VcChannel1, TRUE);
    EnableNvic(VC1_2_IRQn, IrqLevel3, TRUE);
    
    Vc_Cmd(VcChannel1, TRUE);                         //VC1使能
}

/**
 ******************************************************************************
 ** \brief  ADC cfg
 **
 
 **
 ** This sample
 **
 ******************************************************************************/
void App_AdcCfg(void)
{
    stc_adc_cfg_t              stcAdcCfg;
    stc_adc_jqr_cfg_t          stcAdcJqrCfg;
    
    DDL_ZERO_STRUCT(stcAdcCfg);
    DDL_ZERO_STRUCT(stcAdcJqrCfg);
        
    //Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE);
    
    Gpio_SetAnalogMode(GpioPortA, GpioPin2);        //PA02
    Gpio_SetAnalogMode(GpioPortA, GpioPin3);        //PA03    
    Gpio_SetAnalogMode(GpioPortA, GpioPin5);        //PA05
    
    
    //ADC配置
    Adc_Enable();
    M0P_BGR->CR_f.BGR_EN = 0x1u;//BGR必须使能
    M0P_BGR->CR_f.TS_EN  = 0x0u;
    delay100us(1);
    
    stcAdcCfg.enAdcMode         = AdcScanMode;          //连续采样模式
    stcAdcCfg.enAdcClkDiv       = AdcMskClkDiv2;        //Adc工作时钟 PCLK/2
    stcAdcCfg.enAdcSampCycleSel = AdcMskSampCycle8Clk;  //采样时钟 8个周�?
    stcAdcCfg.enAdcRefVolSel    = AdcMskRefVolSelAVDD;  //内部AVDD
    stcAdcCfg.enAdcOpBuf        = AdcMskBufDisable;     //内部电压跟随器关�?
    stcAdcCfg.enInRef           = AdcMskInRefDisable;   //内部参考电压Disable
    
    Adc_Init(&stcAdcCfg);                               //Adc初始�?
    
    Adc_CfgJqrChannel(AdcJQRCH0MUX, AdcExInputCH2);     //配置插队扫描转换通道
    Adc_CfgJqrChannel(AdcJQRCH1MUX, AdcExInputCH3);
    Adc_CfgJqrChannel(AdcJQRCH2MUX, AdcExInputCH5);     //采样顺序CH2 --> CH1 --> CH0
    
    stcAdcJqrCfg.bJqrDmaTrig = FALSE;
    stcAdcJqrCfg.u8JqrCnt = 3;                       //转换起始通道(3-1已在库函数内计算)
    Adc_JqrModeCfg(&stcAdcJqrCfg);                //配置插队扫描转换模式
    
    Adc_JqrExtTrigCfg(AdcMskTrigTimer3, TRUE);          //Timer0触发插队扫描转换
    
    Adc_EnableIrq();                                    //使能Adc中断
    EnableNvic(ADC_DAC_IRQn, IrqLevel1, TRUE);          //Adc开中断
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
    App_ClockCfg();                       //时钟初始�?
    
    App_Timer3Cfg(0x12C0, 0x900, 0);      //Timer3 配置:周期 0x12C0(clk 48M, f = 5K); CH0/1/0x900; CH0/1/2通道B比较值互补模式不需要设�?
    
    App_Timer3PortCfg();                  //Timer3 Port端口配置
    
    App_VcCfg();                          //VC1比较输出刹车到Timer3
    
    App_AdcCfg();                         //ADC 插队扫描采样配置
    
    Tim3_M23_EnPWM_Output(TRUE, FALSE);   //端口输出使能
    
    Tim3_M23_Run();                       //运行
    
    while (1)
    {
        if(1 == u16MainLoopFlag)   //1ms延时
        {
            u16MainLoopFlag = 0;
            
            if(TRUE == Tim3_GetIntFlag(Tim3BkIrq))  //是否发生刹车事件
            {
                u16BkDelayCnt++;
                if(u16BkDelayCnt >= 4000)    //发生Brake，延�?后恢复PWM输出
                {
                    u16BkDelayCnt = 0;
                    Tim3_ClearIntFlag(Tim3BkIrq);             //清除刹车标志
                    Tim3_M23_EnPWM_Output(TRUE, FALSE);       //端口输出使能
                }
            }
        }
    }

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


