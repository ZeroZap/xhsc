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
 * @brief  Source file for PCA example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l19x_pca.h"
#include "hc32l19x_lpm.h"
#include "hc32l19x_gpio.h"

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

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static volatile uint32_t u32PcaTestFlag = 0;
static volatile uint16_t u16CcapData[8] = {0};

__IO uint8_t rising_flag;
__IO uint8_t falling_flag;
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void App_GpioInit(void); 
static void App_PcaInit(void);

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/


/**
******************************************************************************
    ** \brief  主函�?
    ** 
    ** @param  �?
    ** \retval �?
    **
******************************************************************************/ 
int32_t main(void)
{
    App_GpioInit();
    App_PcaInit();
    
    Pca_StartPca(TRUE);
    
    while (1)
    {
        
    }
}

 /**
******************************************************************************
    ** \brief  RTC中断入口函数
    ** 
    ** @param  �?
    ** \retval �?
    **
******************************************************************************/ 
void Pca_IRQHandler(void)
{
    if(Pca_GetItStatus(PcaCcf1) != FALSE)
    {
        Pca_ClrItStatus(PcaCcf1);
        if(Gpio_GetInputIO(GpioPortA,GpioPin6)== TRUE)
        {
            rising_flag = 1;
            falling_flag = 0;
        }
        else
        {
            rising_flag = 0;
            falling_flag = 1;
        }
    }
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

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    //PA07设置为PCA_CH1
    GpioInitStruct.enDrv  = GpioDrvH;
    GpioInitStruct.enDir  = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin7, &GpioInitStruct);
    Gpio_SetAfMode(GpioPortA,GpioPin7,GpioAf2);

    //PA06设置为PCA_CH0
    GpioInitStruct.enDir  = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin6, &GpioInitStruct);
    Gpio_SetAfMode(GpioPortA, GpioPin6, GpioAf2);
}

/**
 ******************************************************************************
 ** \brief  配置PCA
 **
 ** \return �?
 ******************************************************************************/
static void App_PcaInit(void)
{
    stc_pcacfg_t  PcaInitStruct;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralPca, TRUE);    
    
    PcaInitStruct.pca_clksrc = PcaPclkdiv32;
    PcaInitStruct.pca_cidl   = FALSE;
    PcaInitStruct.pca_ecom   = PcaEcomDisable;       //允许比较器功�?
    PcaInitStruct.pca_capp   = PcaCappEnable;        //禁止上升沿捕�?
    PcaInitStruct.pca_capn   = PcaCapnEnable;        //禁止下降沿捕�?
    PcaInitStruct.pca_mat    = PcaMatDisable;        //禁止匹配功能
    PcaInitStruct.pca_tog    = PcaTogDisable;        //禁止翻转控制功能
    PcaInitStruct.pca_pwm    = PcaPwm8bitDisable;    //使能PWM控制输出
    PcaInitStruct.pca_epwm   = PcaEpwmDisable;       //禁止16bitPWM输出
    Pca_M1Init(&PcaInitStruct);    
    
    PcaInitStruct.pca_cidl   = FALSE;
    PcaInitStruct.pca_ecom   = PcaEcomEnable;       //允许比较器功�?
    PcaInitStruct.pca_capp   = PcaCappDisable;      //禁止上升沿捕�?
    PcaInitStruct.pca_capn   = PcaCapnDisable;      //禁止下降沿捕�?
    PcaInitStruct.pca_pwm    = PcaPwm8bitEnable;    //使能PWM控制输出
    PcaInitStruct.pca_ccapl  = 120;
    PcaInitStruct.pca_ccaph  = 120;
    Pca_M0Init(&PcaInitStruct);   

    Pca_ClrItStatus(PcaCcf1);
    Pca_ConfModulexIt(PcaModule1, TRUE);
    EnableNvic(PCA_IRQn, IrqLevel3, TRUE);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


