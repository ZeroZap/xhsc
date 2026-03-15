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
 * @brief  Source file for UART example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "hc32l19x_uart.h"
#include "hc32l19x_gpio.h"
#include "hc32l19x_sysctrl.h"

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
uint8_t u8TxData[2] = {0x00,0x55};

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
void App_PortInit(void);
void App_UartCfg(void);

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
    uint8_t i;
    
    App_PortInit();       //端口初始�?
    
    App_UartCfg();        //串口模块配置
    
    while(1)
    {
        if((Uart_GetStatus(M0P_UART0, UartFE))||(Uart_GetStatus(M0P_UART0, UartPE)))  //错误请求
        {
            Uart_ClrStatus(M0P_UART0, UartFE);            //清除帧错误标�?
            Uart_ClrStatus(M0P_UART0, UartPE);            //清除奇偶校验错误标记
        }
        if(Uart_GetStatus(M0P_UART0,UartRC))              //接收到数�?
        {
            Uart_ClrStatus(M0P_UART0,UartRC);
            u8TxData[0] = Uart_ReceiveData(M0P_UART0);    //接收数据
            for(i=0;i<2;i++)
            {
                Uart_SendDataPoll(M0P_UART0,u8TxData[i]); //查询方式发送数�?
            }
        }
    }
}

//串口引脚配置
void App_PortInit(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); //GPIO外设模块时钟使能
    
    stcGpioCfg.enDir = GpioDirOut;
    stcGpioCfg.bOutputVal = TRUE;
    Gpio_Init(GpioPortA,GpioPin9,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin9,GpioAf1); //配置PA09 为UART0 TX
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortA,GpioPin10,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin10,GpioAf1);//配置PA10 为UART0 RX
}

//串口模块配置
void App_UartCfg(void)
{
    stc_uart_cfg_t  stcCfg;
    stc_uart_multimode_t stcMulti;
    stc_uart_baud_t stcBaud;

    DDL_ZERO_STRUCT(stcCfg);
    DDL_ZERO_STRUCT(stcMulti);
    DDL_ZERO_STRUCT(stcBaud);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);//UART0外设模块时钟使能
    
    stcCfg.enRunMode = UartMskMode3;     //模式3
    stcCfg.enStopBit = UartMsk1bit;      //1位停止位
    stcCfg.enMmdorCk = UartMskEven;      //偶校�?
    stcCfg.stcBaud.u32Baud = 9600;       //波特�?600
    stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;         //通道采样分频配置
    stcCfg.stcBaud.u32Pclk = Sysctrl_GetPClkFreq();    //获得外设时钟（PCLK）频率�?
    Uart_Init(M0P_UART0, &stcCfg);       //串口初始�?

    Uart_ClrStatus(M0P_UART0,UartRC);    //清接收请�?
    Uart_ClrStatus(M0P_UART0,UartTC);    //清发送请�?
    Uart_EnableIrq(M0P_UART0,UartRxIrq); //使能串口接收中断
    Uart_EnableIrq(M0P_UART0,UartTxIrq); //使能串口发送中�?

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


