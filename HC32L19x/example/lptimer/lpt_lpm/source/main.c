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
 * @brief  Source file for LPTIMER example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "hc32l19x_lptim.h"
#include "hc32l19x_lpm.h"
#include "hc32l19x_gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
//LED
#define LED_PORT     GpioPortD
#define LED_PIN      GpioPin14
//按键
#define Button_PORT  GpioPortA
#define Button_PIN   GpioPin7

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
__IO uint8_t ItFlag;
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static volatile uint32_t u32LptTestFlag = 0;


/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/


/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
static void App_SysClkInit(void);     
static void App_GPIOInit(void);
static void App_LPTimer0Init(void);
static void App_LowPowerModeGpioSet(void);

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
    ///< 系统时钟初始�?
    App_SysClkInit();     
    ///< GPIO初始�?
    App_GPIOInit();
    ///< LPTimer0初始�?
    App_LPTimer0Init(); 
    
    while (Gpio_GetInputIO(Button_PORT,Button_PIN) == 1);   //等待user key按下
    Lptim_Cmd(M0P_LPTIMER0, TRUE);                          //启动lptimer0运行   
    Gpio_WriteOutputIO(LED_PORT, LED_PIN, FALSE);           //熄灭LED

    ///< 休眠模式GPIO配置
    App_LowPowerModeGpioSet();
    Lpm_GotoDeepSleep(TRUE);                                //进入深度睡眠模式
    while (1)
    {
        if(ItFlag == 1)
        {
            ItFlag = 0;
        }
    }
}



/**
 ******************************************************************************
 ** \brief  LPTIMER0中断服务函数
 **
 ** \return �?
 ******************************************************************************/
void LpTim0_IRQHandler(void)
{
    if (TRUE == Lptim_GetItStatus(M0P_LPTIMER0))
    {
        ItFlag = 1;
        Lptim_ClrItStatus(M0P_LPTIMER0);//清除LPTIMER0的中断标志位      

        Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);     //输出高，点亮LED
        delay1ms(500);
        Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);     //输出高，点亮LED
    }
}


static void App_SysClkInit(void)
{
    stc_sysctrl_clk_cfg_t  stcClkCfg;
    
    //CLK INIT
    stcClkCfg.enClkSrc  = SysctrlClkRCH;
    stcClkCfg.enHClkDiv = SysctrlHclkDiv1;
    stcClkCfg.enPClkDiv = SysctrlPclkDiv1;
    Sysctrl_ClkInit(&stcClkCfg);
    
    //使能RCL
    Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);
}

/**
 ******************************************************************************
 ** \brief  初始化外部GPIO引脚
 **
 ** \return �?
 ******************************************************************************/
static void App_GPIOInit(void)
{
    stc_gpio_cfg_t         GpioInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);
    
    //使能GPIO外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    //PD14设置为GPIO-->LED控制�?
    GpioInitStruct.enDir  = GpioDirOut;
    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &GpioInitStruct);

    //PA07作为按键输入
    GpioInitStruct.enDir  = GpioDirIn;
    Gpio_Init(STK_USER_PORT, STK_USER_PIN, &GpioInitStruct);
    
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);     //输出高，点亮LED    
}

/**
 ******************************************************************************
 ** \brief  初始化LPTIMER0
 **
 ** \return �?
 ******************************************************************************/
static void App_LPTimer0Init(void)
{
    stc_lptim_cfg_t    stcLptCfg;    
    DDL_ZERO_STRUCT(stcLptCfg);

    ///< 使能LPTIM0 外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralLpTim0, TRUE);
    
    stcLptCfg.enGate   = LptimGateLow;
    stcLptCfg.enGatep  = LptimGatePLow;
    stcLptCfg.enTcksel = LptimRcl;
    stcLptCfg.enTogen  = LptimTogEnLow;
    stcLptCfg.enCt     = LptimTimerFun;         //计数器功�?
    stcLptCfg.enMd     = LptimMode1;            //工作模式为模�?：无自动重装�?6位计数器/定时�?
    stcLptCfg.u16Arr   = 0;                     //预装载寄存器�?
    Lptim_Init(M0P_LPTIMER0, &stcLptCfg);
    
    Lptim_ClrItStatus(M0P_LPTIMER0);        //清除中断标志�?
    Lptim_ConfIt(M0P_LPTIMER0, TRUE);       //允许LPTIMER中断    
    EnableNvic(LPTIM_0_1_IRQn, IrqLevel3, TRUE); 
}

static void App_LowPowerModeGpioSet(void)
{
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    //swd as gpio
    Sysctrl_SetFunc(SysctrlSWDUseIOEn, TRUE);
    
    ///< 配置为数字端�?
    M0P_GPIO->PAADS = 0;
    M0P_GPIO->PBADS = 0;
    M0P_GPIO->PCADS = 0;
    M0P_GPIO->PDADS = 0;
    M0P_GPIO->PEADS = 0;
    M0P_GPIO->PFADS = 0;
    
    ///< 配置为端口输入（除LED端口外）
    M0P_GPIO->PADIR = 0XFFFF;
    M0P_GPIO->PBDIR = 0XFFFF;
    M0P_GPIO->PCDIR = 0XFFFF;
    M0P_GPIO->PDDIR = 0XBFFF;
    M0P_GPIO->PEDIR = 0XFFFF;
    M0P_GPIO->PFDIR = 0XFFFF;
    
    ///< 输入下拉（除KEY端口以外�?
    M0P_GPIO->PAPD = 0xFF7F;
    M0P_GPIO->PBPD = 0xFFFF;
    M0P_GPIO->PCPD = 0xFFFF;
    M0P_GPIO->PDPD = 0xFFFF;
    M0P_GPIO->PEPD = 0xFFFF;
    M0P_GPIO->PFPD = 0xFFFF;
    
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


