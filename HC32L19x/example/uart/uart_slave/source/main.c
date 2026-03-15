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
#include "hc32l19x_uart.h"
#include "hc32l19x_gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/
#define     T1_PORT                 (3)
#define     T1_PIN                  (3)
#define     SLAVEADDR               0xc0

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
uint8_t u8TxData[2] = {0xaa,0x55};
volatile uint8_t u8RxData[2] = {0x00};
uint16_t u16AddrData = 0;
volatile uint8_t u8TxCnt=0,u8RxCnt=0;
uint8_t u8AddrFlg = 0;

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
    //端口初始�?
    App_PortInit();
     
    //串口模块配置
    App_UartCfg();
    
    while(1)
    {
        if(u8RxCnt>1)                                       //如果接收�?个数据字�?
        {
            u8RxCnt = 0;
            u8TxCnt = 0;
            Uart_EnableIrq(M0P_UART1,UartTxIrq);            //使能发送中�?
            Uart_SendDataIt(M0P_UART1, u8TxData[u8TxCnt]);  //发送第一个数据字�?
            u8TxCnt++;
        }
    }
}

// UART1中断处理函数
void Uart1_IRQHandler(void)
{
    //数据接收中断
    if(Uart_GetStatus(M0P_UART1, UartRC))
    {
        Uart_ClrStatus(M0P_UART1, UartRC);
        if(0 == u8AddrFlg)
        {
            u8AddrFlg = 1;
            u16AddrData = M0P_UART1->SBUF;                     //首字节为地址字节
            if(u16AddrData&0x100)
            {
                M0P_UART1->SCON_f.ADRDET = 0;                  //清零，准备接收数据帧
            }
        }else
        {
            u8RxData[u8RxCnt++] = Uart_ReceiveData(M0P_UART1); //数据字节
        } 
    }
    //数据发送中�?
    if(Uart_GetStatus(M0P_UART1, UartTC))
    {
        Uart_ClrStatus(M0P_UART1, UartTC);
        if(u8TxCnt>1)                                          //如果已经发送两个数据字�?
        {
            u8RxCnt = 0;
            Uart_DisableIrq(M0P_UART1,UartTxIrq);              //禁止发送中�?
            Uart_EnableIrq(M0P_UART1,UartRxIrq);               //使能接收中断
        }else                                                  //如果还没有发送两个字�?
        {
            Uart_SendDataIt(M0P_UART1, u8TxData[u8TxCnt]);     //继续发送数据字�?
            u8TxCnt++;
        }
    }

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
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart1,TRUE);//使能UART1外设时钟门控开�?
    
    ///<UART Init
    stcCfg.enRunMode        = UartMskMode3;                //模式3
    stcCfg.enStopBit        = UartMsk1bit;                 //1bit停止�?
    stcCfg.enMmdorCk        = UartMskDataOrAddr;           //多机模式
    stcCfg.stcBaud.u32Baud  = 9600;                        //波特�?600
    stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;             //通道采样分频配置
    stcCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq();       //获得外设时钟（PCLK）频率�?       
    Uart_Init(M0P_UART1, &stcCfg);                         ///<串口初始�?
        
    stcMulti.u8SaddEn = 0xff;                              //从机地址掩码
    stcMulti.u8SlaveAddr = SLAVEADDR;                      //从机地址
    Uart_SetMultiMode(M0P_UART1,&stcMulti);                //多机单独配置, 必须在Uart_Init函数之后调用此函�?
    
    ///<UART中断使能
    Uart_ClrStatus(M0P_UART1,UartRC);                      ///<清接收请�?
    Uart_ClrStatus(M0P_UART1,UartTC);                      ///<清接收请�?
    Uart_EnableIrq(M0P_UART1,UartRxIrq);                   ///<使能串口接收中断 
    EnableNvic(UART1_3_IRQn, IrqLevel3, TRUE);             ///<系统中断使能    
}

//串口引脚配置
void App_PortInit(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); //使能GPIO外设时钟门控开�?
    
    stcGpioCfg.enDir = GpioDirOut;
    stcGpioCfg.bOutputVal = TRUE;
    Gpio_Init(GpioPortA,GpioPin2,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin2,GpioAf1);            //配置PA02 TX
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortA,GpioPin3,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin3,GpioAf1);            //配置PA03 RX
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


