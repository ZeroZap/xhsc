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
#include "hc32l19x_lpm.h"
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
/**
 ******************************************************************************
 ** \brief  LPTIMER0中断服务函数
 **
 ** \return �?
 ******************************************************************************/
void Vc0_IRQHandler(void)
{
    if(Vc_GetItStatus(Vc0_Intf) == TRUE)
    {
        Vc_ClearItStatus(Vc0_Intf);             //清除VC_CH0中断标志�?
    }
}

/**
 ******************************************************************************
 ** \brief  初始化外部GPIO引脚
 **
 ** \return �?
 ******************************************************************************/
void GPIO_Cfg(void)
{
    stc_gpio_cfg_t GpioInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);
    
    GpioInitStruct.enDrv = GpioDrvH;
    GpioInitStruct.enDir = GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin6,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA,GpioPin6,GpioAf5);              //PA06作为VC0_OUT

    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &GpioInitStruct);   //PD14配置成输出，控制板上蓝色LED
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);    //输出低，熄灭LED
    
    GpioInitStruct.enDir = GpioDirIn;
    Gpio_Init(STK_USER_PORT,STK_USER_PIN, &GpioInitStruct);
    
    Gpio_SetAnalogMode(GpioPortC,GpioPin0);                  //模拟输入
    Gpio_SetAnalogMode(GpioPortA,GpioPin0);                  //模拟输入    
}

/**
 ******************************************************************************
 ** \brief  初始化VC0
 **
 ** \return �?
 ******************************************************************************/
void VC_Cfg(void)
{
    stc_vc_channel_cfg_t VcInitStruct;
    DDL_ZERO_STRUCT(VcInitStruct);
    VcInitStruct.enVcChannel = VcChannel0;
    VcInitStruct.enVcCmpDly  = VcDelay10mv;          //VC0迟滞电压约为10mV
    VcInitStruct.enVcBiasCurrent = VcBias10ua;       //VC0功耗为10uA
    VcInitStruct.enVcFilterTime  = VcFilter28us;     //VC输出滤波时间约为28us
    VcInitStruct.enVcInPin_P     = VcInPCh0;         //VC0_CH的P端连接PC00
    VcInitStruct.enVcInPin_N     = VcInNCh0;         //VC0_CH的N端连接PA00
    VcInitStruct.enVcOutCfg      = VcOutDisable;     //不输出给配置寄存器所定义的外�?
    VcInitStruct.bFlten          = TRUE;             //是能滤波
    Vc_Init(&VcInitStruct);
    Vc_CfgItType(VcChannel0, VcIrqRise);          //配置VC0为上升沿中断
}

/**
******************************************************************************
    ** \brief  主函�?
    ** 
  ** @param  �?
    ** \retval �?
    **
******************************************************************************/ 
int main(void)
{
    stc_vc_channel_cfg_t stcChannelCfg;
    DDL_ZERO_STRUCT(stcChannelCfg);//变量�?

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);//开GPIO时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralVcLvd, TRUE);//开LVD时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE);//开adc时钟
    Bgr_BgrEnable();                 //BGR必须使能
    GPIO_Cfg();
    VC_Cfg();
    Vc_ClearItStatus(Vc0_Intf);
    EnableNvic(VC0_IRQn, IrqLevel3, TRUE);
    Vc_ItCfg(VcChannel0, TRUE);
    Vc_Cmd(VcChannel0, TRUE);

    while(Gpio_GetInputIO(STK_USER_PORT,STK_USER_PIN) == 1);
    Lpm_GotoDeepSleep(FALSE);
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);//唤醒后点亮LED3    
    
    while (1)
    {

    }
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


