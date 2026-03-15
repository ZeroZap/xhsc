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
volatile uint32_t u32AdcRestult0;
volatile uint32_t u32AdcRestult2;
volatile uint32_t u32AdcRestult5;
volatile uint32_t u32AdcRestultAcc;
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
void App_AdcSQRCfg(void);
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
    
    ///< ADC 采样端口初始�?
    App_AdcPortInit();
    
    ///< ADC 模块初始�?
    App_AdcInit();
    
    ///< ADC 顺序扫描 功能配置
    App_AdcSQRCfg();
    
    while(1)
    {        
        ;
    }
}

 ///< ADC 中断服务程序
void Adc_IRQHandler(void)
{    
    if(TRUE == Adc_GetIrqStatus(AdcMskIrqSqr))
    {
        Adc_ClrIrqStatus(AdcMskIrqSqr);
        
        u32AdcRestult0   = Adc_GetSqrResult(AdcSQRCH0MUX);  //获取顺序扫描通道0
        u32AdcRestult2   = Adc_GetSqrResult(AdcSQRCH1MUX);  //获取顺序扫描通道1
        u32AdcRestult5   = Adc_GetSqrResult(AdcSQRCH2MUX);  //获取顺序扫描通道2
        
        u32AdcRestultAcc = Adc_GetAccResult();
        
        Adc_SQR_Stop();
    }
}

///< ADC 采样端口初始�?
void App_AdcPortInit(void)
{    
    ///< 开启GPIO外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    Gpio_SetAnalogMode(GpioPortA, GpioPin0);        //PA00 (AIN0)
    Gpio_SetAnalogMode(GpioPortA, GpioPin2);        //PA02 (AIN2)
    Gpio_SetAnalogMode(GpioPortA, GpioPin5);        //PA05 (AIN5)
}

///< ADC 模块初始�?
void App_AdcInit(void)
{
    stc_adc_cfg_t              stcAdcCfg;

    DDL_ZERO_STRUCT(stcAdcCfg);
    
    ///< 开启ADC/BGR外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE); 
    
    Bgr_BgrEnable();        ///< 开启BGR
    
    ///< ADC 初始化配�?
    stcAdcCfg.enAdcMode         = AdcScanMode;              ///<采样模式-扫描
    stcAdcCfg.enAdcClkDiv       = AdcMskClkDiv1;            ///<采样分频-1
    stcAdcCfg.enAdcSampCycleSel = AdcMskSampCycle8Clk;      ///<采样周期�?8
    stcAdcCfg.enAdcRefVolSel    = AdcMskRefVolSelAVDD;      ///<参考电压选择-VCC
    stcAdcCfg.enAdcOpBuf        = AdcMskBufDisable;         ///<OP BUF配置-�?
    stcAdcCfg.enInRef           = AdcMskInRefDisable;       ///<内部参考电压使�?�?
    stcAdcCfg.enAdcAlign        = AdcAlignRight;               ///<转换结果对齐方式-�?
    Adc_Init(&stcAdcCfg);
}

///< ADC 顺序扫描 功能配置
void App_AdcSQRCfg(void)
{
    stc_adc_sqr_cfg_t          stcAdcSqrCfg;
    
    DDL_ZERO_STRUCT(stcAdcSqrCfg);
        
    ///< 顺序扫描模式功能及通道配置
    ///< 注意：扫描模式下，当配置转换次数为n时，转换通道的配置范围必须为[SQRCH(0)MUX,SQRCH(n-1)MUX]
    stcAdcSqrCfg.bSqrDmaTrig = FALSE;
    stcAdcSqrCfg.enResultAcc = AdcResultAccEnable;
    stcAdcSqrCfg.u8SqrCnt    = 3;
    Adc_SqrModeCfg(&stcAdcSqrCfg);

    Adc_CfgSqrChannel(AdcSQRCH0MUX, AdcExInputCH0);
    Adc_CfgSqrChannel(AdcSQRCH1MUX, AdcExInputCH2);
    Adc_CfgSqrChannel(AdcSQRCH2MUX, AdcExInputCH5);
    
    ///< ADC 中断使能
    Adc_EnableIrq();
    EnableNvic(ADC_DAC_IRQn, IrqLevel3, TRUE);
    
    ///< 启动顺序扫描采样
    Adc_SQR_Start();

}    


/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


