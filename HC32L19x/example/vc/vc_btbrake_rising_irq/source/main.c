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
 * @brief  Source file for VC example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l19x_vc.h"
#include "hc32l19x_gpio.h"
#include "hc32l19x_adc.h"
#include "hc32l19x_bt.h"
#include "hc32l19x_flash.h"
#include "hc32l19x_bgr.h"
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
static void App_VC0Init(void);
static void App_Tim0Mode0Init(void);
/**
 ******************************************************************************
 ** \brief  主函�?
 **
 ** \return �?
 ******************************************************************************/
int main(void)
{
    App_GpioInit();                         //配置GPIO
    App_Tim0Mode0Init();                    //配置TIM0
    App_VC0Init();                          //配置VC

    Vc_Cmd(VcChannel0, TRUE);               //使能VC0
    Bt_M0_Run(TIM0);                        //使能TIM0工作
    
    while (1)
    {
        ;
    }
}


void Vc0_IRQHandler(void)
{
    Vc_ClearItStatus(Vc0_Intf);
    
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);
    delay1ms(1000);
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);
    delay1ms(1000);
}
/**
 ******************************************************************************
 ** \brief  初始化外部GPIO引脚
 **
 ** \return �?
 ******************************************************************************/
static void App_GpioInit(void)
{
    stc_gpio_cfg_t GpioInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);//开GPIO时钟
     
    GpioInitStruct.enDrv = GpioDrvH;
    GpioInitStruct.enDir = GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin6,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA,GpioPin6,GpioAf5);              //PA06作为VC0_OUT

    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &GpioInitStruct);   //PD14配置成输出，控制板上蓝色LED
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);    
    
    Gpio_SetAnalogMode(GpioPortC,GpioPin0);                  //VC0的P端输�?

    GpioInitStruct.enDir  = GpioDirOut;   
    Gpio_Init(GpioPortA, GpioPin0, &GpioInitStruct);
    Gpio_SetAfMode(GpioPortA,GpioPin0,GpioAf7);            //PA00设置为TIM0_CHA
    
    Gpio_Init(GpioPortA, GpioPin1, &GpioInitStruct);
    Gpio_SetAfMode(GpioPortA,GpioPin1,GpioAf3);            //PA01设置为TIM0_CHB    
}

/**
 ******************************************************************************
 ** \brief  初始化VC0
 **
 ** \return �?
 ******************************************************************************/
static void App_VC0Init(void)
{
    stc_vc_channel_cfg_t VcInitStruct;
    DDL_ZERO_STRUCT(VcInitStruct);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralVcLvd, TRUE);//开LVD时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE);//开adc时钟  
        
    Bgr_BgrEnable();                 //BGR必须使能
    
    VcInitStruct.enVcChannel = VcChannel0;
    VcInitStruct.enVcCmpDly  = VcDelay10mv;          //VC0迟滞电压约为10mV
    VcInitStruct.enVcBiasCurrent = VcBias10ua;       //VC0功耗为10uA
    VcInitStruct.enVcFilterTime  = VcFilter28us;     //VC输出滤波时间约为28us
    VcInitStruct.enVcInPin_P     = VcInPCh0;         //VC0_CH的P端连接PC00
    VcInitStruct.enVcInPin_N     = AiLdo;            //VC0_CH的N端连接内�?.5V
    VcInitStruct.enVcOutCfg      = VcOutTIMBK;       //作为定时器刹�?
    VcInitStruct.bFlten          = TRUE;             //使能滤波
    Vc_Init(&VcInitStruct);
    
    ///< VC0中断配置
    Vc_CfgItType(VcChannel0, VcIrqRise);          //配置VC0为上升沿中断
    Vc_ClearItStatus(Vc0_Intf);    
    Vc_ItCfg(VcChannel0, TRUE);    
    EnableNvic(VC0_IRQn, IrqLevel3, TRUE);

}
/**
 ******************************************************************************
 ** \brief  初始化TIM2
 **
 ** \return �?
 ******************************************************************************/
static void App_Tim0Mode0Init(void)
{
    uint16_t                      u16ArrValue;
    uint16_t                      u16CompareAValue;
    uint16_t                      u16CompareBValue;
    uint16_t                      u16CntValue;
    uint8_t                       u8ValidPeriod;

    stc_vc_channel_cfg_t       stcChannelCfg;    
    stc_bt_mode23_cfg_t        stcBtBaseCfg;
    stc_bt_m23_compare_cfg_t   stcBtPortCmpCfg;  
    stc_bt_m23_bk_input_cfg_t  stcBtBkInputCfg;

    DDL_ZERO_STRUCT(stcChannelCfg);
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcBtPortCmpCfg);   
    DDL_ZERO_STRUCT(stcBtBkInputCfg);   

    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer外设时钟使能  
    
    stcBtBaseCfg.enWorkMode    = BtWorkMode2;              //锯齿波模�?
    stcBtBaseCfg.enCT          = BtTimer;                  //定时器功能，计数时钟为内部PCLK
    stcBtBaseCfg.enPRS         = BtPCLKDiv1;               //PCLK
    stcBtBaseCfg.enCntDir      = BtCntUp;                  //向上计数
    stcBtBaseCfg.enPWMTypeSel  = BtIndependentPWM;         //独立输出PWM
    stcBtBaseCfg.enPWM2sSel    = BtSinglePointCmp;         //单点比较功能
    stcBtBaseCfg.bOneShot      = FALSE;                    //循环计数
    stcBtBaseCfg.bURSSel       = FALSE;                    //上下溢更�?
    Bt_Mode23_Init(TIM0, &stcBtBaseCfg);                   //TIM0 的模�?功能初始�?
     
    u16ArrValue = 0x9000;
    Bt_M23_ARRSet(TIM0, u16ArrValue, TRUE);                //设置重载�?并使能缓�?

    u16CompareAValue = 0x6000;
    Bt_M23_CCR_Set(TIM0, BtCCR0A, u16CompareAValue);       //设置比较值A
    u16CompareBValue = 0x3000;
    Bt_M23_CCR_Set(TIM0, BtCCR0B, u16CompareBValue);       //设置比较值B

    stcBtPortCmpCfg.enCH0ACmpCtrl   = BtPWMMode2;          //OCREFA输出控制OCMA:PWM模式2
    stcBtPortCmpCfg.enCH0APolarity  = BtPortPositive;      //正常输出
    stcBtPortCmpCfg.bCh0ACmpBufEn   = TRUE;                //A通道缓存控制
    stcBtPortCmpCfg.enCh0ACmpIntSel = BtCmpIntNone;        //A通道比较控制:�?

    stcBtPortCmpCfg.enCH0BCmpCtrl   = BtPWMMode2;          //OCREFB输出控制OCMB:PWM模式2
    stcBtPortCmpCfg.enCH0BPolarity  = BtPortPositive;      //正常输出
    stcBtPortCmpCfg.bCH0BCmpBufEn   = TRUE;                //B通道缓存控制使能
    stcBtPortCmpCfg.enCH0BCmpIntSel = BtCmpIntNone;        //B通道比较控制:�?
    Bt_M23_PortOutput_Cfg(TIM0, &stcBtPortCmpCfg);      //比较输出端口配置

    u8ValidPeriod = 0;                                     //事件更新周期设置�?表示锯齿波每个周期更新一次，�?1代表延迟1个周�?
    Bt_M23_SetValidPeriod(TIM0,u8ValidPeriod);             //间隔周期设置
        
    u16CntValue = 0;   
    Bt_M23_Cnt16Set(TIM0, u16CntValue);                    //设置计数初�?  
    Bt_M23_EnPWM_Output(TIM0, TRUE, FALSE);                //TIM0 端口输出使能

    stcBtBkInputCfg.bEnBrake = TRUE;                       //使能刹车
    stcBtBkInputCfg.bEnVCBrake = TRUE;                     //使能VC刹车
    stcBtBkInputCfg.bEnSafetyBk = FALSE;
    stcBtBkInputCfg.bEnBKSync =   FALSE;
    stcBtBkInputCfg.enBkCH0AStat =BtCHxBksLow;             //刹车时强制输出低电平
    stcBtBkInputCfg.enBkCH0BStat =BtCHxBksLow;             //刹车时强制输出低电平 
    stcBtBkInputCfg.enBrakePolarity = BtPortPositive;
    stcBtBkInputCfg.enBrakeFlt = BtFltNone;
    Bt_M23_BrakeInput_Cfg(TIM0,&stcBtBkInputCfg);
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


