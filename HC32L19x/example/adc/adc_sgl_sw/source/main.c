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
 * @brief  Source file for ADC example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l19x_adc.h"
#include "hc32l19x_gpio.h"
#include "hc32l19x_bgr.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/


/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
volatile uint32_t u32AdcRestult;
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
void App_AdcPortInit(void);
void App_AdcInit(void);
void App_AdcSglCfg(void);

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
    ///< ADC采样端口初始�?
    App_AdcPortInit();
    
    ///< ADC模块 初始�?
    App_AdcInit();
    
    ///< ADC 顺序扫描模式 配置
    App_AdcSglCfg();
    
    while(1)
    {
        ;
    }
}

 ///< ADC 中断服务程序
void Adc_IRQHandler(void)
{    
    if(TRUE == Adc_GetIrqStatus(AdcMskIrqSgl))
    {
        Adc_ClrIrqStatus(AdcMskIrqSgl);       ///< 清除中断标志�?
        
        u32AdcRestult = Adc_GetSglResult();   ///< 获取采样�?
        
        Adc_SGL_Stop();                       ///< ADC 单次转换停止
    }
}

///< ADC 采样端口初始�?
void App_AdcPortInit(void)
{    
    ///< 开�?GPIO外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    Gpio_SetAnalogMode(GpioPortA, GpioPin0);        //PA00 (AIN0)
}


///< ADC模块 初始�?
void App_AdcInit(void)
{
    stc_adc_cfg_t              stcAdcCfg;

    DDL_ZERO_STRUCT(stcAdcCfg);
    
    ///< 开启ADC/BGR 外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE); 
    
    Bgr_BgrEnable();        ///< 开启BGR
    
    ///< ADC 初始化配�?
    stcAdcCfg.enAdcMode         = AdcSglMode;               ///<采样模式-单次
    stcAdcCfg.enAdcClkDiv       = AdcMskClkDiv1;            ///<采样分频-1
    stcAdcCfg.enAdcSampCycleSel = AdcMskSampCycle12Clk;     ///<采样周期�?12
    stcAdcCfg.enAdcRefVolSel    = AdcMskRefVolSelInBgr2p5;  ///<参考电压选择-内部2.5V
    stcAdcCfg.enAdcOpBuf        = AdcMskBufDisable;         ///<OP BUF配置-�?
    stcAdcCfg.enInRef           = AdcMskInRefEnable;        ///<内部参考电压使�?开
    stcAdcCfg.enAdcAlign        = AdcAlignRight;            ///<转换结果对齐方式-�?
    Adc_Init(&stcAdcCfg);
}

///< ADC 单次扫描模式 配置
void App_AdcSglCfg(void)
{
    ///< ADC 采样通道配置
    Adc_CfgSglChannel(AdcExInputCH0);
    
    ///< ADC 中断使能
    Adc_EnableIrq();
    EnableNvic(ADC_DAC_IRQn, IrqLevel3, TRUE);
    
    ///< 启动单次转换采样
    Adc_SGL_Start();   

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


