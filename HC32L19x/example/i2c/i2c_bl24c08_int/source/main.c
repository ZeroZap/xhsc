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
 * @brief  Source file for I2C example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l19x_i2c.h"
#include "hc32l19x_gpio.h"
#include "hc32l19x_reset.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define I2C_SLAVEWT 0xA0
#define I2C_SLAVERD 0xA1
#define READLEN   10
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
volatile uint8_t SendFlg   = 0;
volatile uint8_t Comm_flg  = 0;
volatile uint8_t u8SendLen = 0;
volatile uint8_t u8RecvLen = 0;
volatile uint8_t u8State   = 0;
volatile uint8_t u8Addr    = 0x00;  //EEPROM地址字节
volatile uint8_t u8Senddata[10] = {0x12,0x34,0x56,0x78,0x90,0x11,0x22,0x33,0x44,0x56};
volatile uint8_t u8Recdata[10]  = {0x00};
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
void App_I2cCfg(void);
void App_PortCfg(void);
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
    ///< I2C 模块配置
    App_I2cCfg();

    ///< IO端口配置
    App_PortCfg();
        
    ///< 向I2C总线发起开始信�?
    I2C_SetFunc(M0P_I2C0, I2cStart_En);         
    
    while(1)
    {
        if(1 == Comm_flg)                       ///< 发送完，转接收信号
        {
            Comm_flg = 0;                       ///< 启动主机接收模式
            delay1ms(100);
            I2C_ClearFunc(M0P_I2C0, I2cStop_En);///< 清除停止标志�?
            I2C_SetFunc(M0P_I2C0, I2cStart_En); ///< 重新启动I2C
        }
    }  
}

///< I2C 模块配置
void App_I2cCfg(void)
{
    stc_i2c_cfg_t stcI2cCfg;

    DDL_ZERO_STRUCT(stcI2cCfg);                            ///< 初始化结构体变量的值为0
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralI2c0,TRUE); ///< 开启I2C0时钟门控      

    stcI2cCfg.u32Pclk = Sysctrl_GetPClkFreq();  ///< 获取PCLK时钟
    stcI2cCfg.u32Baud = 100000;                 ///< 100KHz
    stcI2cCfg.enMode = I2cMasterMode;           ///< 主机模式
    stcI2cCfg.u8SlaveAddr = 0x55;               ///< 从地址，主模式无效
    stcI2cCfg.bGc = FALSE;                      ///< 广播地址应答使能关闭
    I2C_Init(M0P_I2C0,&stcI2cCfg);              ///< 模块初始�?
    
    EnableNvic(I2C0_IRQn, IrqLevel3, TRUE);     ///< 系统中断使能
}

///< IO端口配置
void App_PortCfg(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcGpioCfg);                           ///< 初始化结构体变量的值为0
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); ///< 开启GPIO时钟门控
        
    stcGpioCfg.enDir = GpioDirOut;                         ///< 端口方向配置->输入      
    stcGpioCfg.enOD = GpioOdEnable;                        ///< 端口开漏输出配�?>开漏输出使�?     
    stcGpioCfg.enPu = GpioPuEnable;                        ///< 端口上拉配置->使能
    stcGpioCfg.enPd = GpioPdDisable;                       ///< 端口下拉配置->禁止
    stcGpioCfg.bOutputVal = TRUE;
    
    Gpio_Init(GpioPortB, GpioPin6, &stcGpioCfg);           ///< 初始化PB06
    Gpio_Init(GpioPortB, GpioPin7, &stcGpioCfg);           ///< 初始化PB07
    
    Gpio_SetAfMode(GpioPortB, GpioPin6,GpioAf1);           ///< 配置PB06为SCL
    Gpio_SetAfMode(GpioPortB, GpioPin7,GpioAf1);           ///< 配置PB07为SDA
}

///< I2c0中断函数
void I2c0_IRQHandler(void)
{
    u8State = I2C_GetState(M0P_I2C0);
    
    switch(u8State)
    {
        case 0x08:                                   //已发送起始条件，将发送SLA+W
            I2C_ClearFunc(M0P_I2C0, I2cStart_En);
            I2C_ClearFunc(M0P_I2C0, I2cStop_En);
            I2C_WriteByte(M0P_I2C0, I2C_SLAVEWT);    //写命�?
            break;
        case 0x10:                                   //已发送重复起始条�?
            I2C_ClearFunc(M0P_I2C0, I2cStart_En);
            I2C_ClearFunc(M0P_I2C0, I2cStop_En);
            if(0 == SendFlg)                         //判断当前发送SLA+W还是SLA+R
            {
                I2C_WriteByte(M0P_I2C0, I2C_SLAVEWT);//写命�?
            }
            else
            {
                I2C_WriteByte(M0P_I2C0, I2C_SLAVERD);//读命�?
            }
            break;
        case 0x18:                                   //已发送SLA+W,并接收到ACK
            I2C_ClearFunc(M0P_I2C0, I2cStart_En);
            I2C_ClearFunc(M0P_I2C0, I2cStop_En);
            I2C_WriteByte(M0P_I2C0, u8Addr);         //EEPROM地址字节
            break;
        case 0x20:                                   //上一次发送SLA+W后，收到NACK
        case 0x38:                                   //上一次在SLA+读或写时丢失仲裁
        case 0x30:                                   //已发送I2Cx_DATA中的数据，收到NACK
        case 0x48:                                   //发送SLA+R后，收到一个NACK
                  I2C_SetFunc(M0P_I2C0,I2cStop_En);
            I2C_SetFunc(M0P_I2C0,I2cStart_En);            
            break;            
        case 0x58:                                          //已接收到最后一个数据，NACK已返�?
                  u8Recdata[u8RecvLen++] = I2C_ReadByte(M0P_I2C0);//读最后一字节数据返回NACK后，读取数据并发送停止条�?
            I2C_ClearFunc(M0P_I2C0, I2cStart_En);    
            I2C_SetFunc(M0P_I2C0, I2cStop_En);              //停止条件
            break;                
        case 0x28:                                          //已发送数据，接收到ACK, 此处是已发送从机内存地址u8Addr并接收到ACK
            if(SendFlg == 1)                                //读数据发送完地址字节后，重复起始条件
            {
                I2C_SetFunc(M0P_I2C0,I2cStart_En);
                I2C_ClearFunc(M0P_I2C0,I2cStop_En);
            }
            else
            {                
                if(10 <= u8SendLen)
                {
                    u8SendLen = 0;
                    Comm_flg  = 1;
                    SendFlg   = 1;
                    
                    I2C_ClearFunc(M0P_I2C0,I2cStart_En);
                    I2C_SetFunc(M0P_I2C0, I2cStop_En);       //将发送停止条�?
                }else
                {
                    I2C_WriteByte(M0P_I2C0, u8Senddata[u8SendLen++]);
                }
            }   
            break;
        case 0x40:                                           //已发送SLA+R，并接收到ACK
            u8RecvLen = 0;
            I2C_ClearFunc(M0P_I2C0, I2cStart_En);
            I2C_ClearFunc(M0P_I2C0, I2cStop_En);
            if(1 < READLEN)
            {
                I2C_SetFunc(M0P_I2C0, I2cAck_En);            //读取数据超过1个字节才发送ACK
            }
            break;    
        case 0x50:                                           //已发送SLA+R，并接收到ACK
                  u8Recdata[u8RecvLen++] = I2C_ReadByte(M0P_I2C0); //读取数据
            I2C_ClearFunc(M0P_I2C0, I2cStart_En);
            I2C_ClearFunc(M0P_I2C0, I2cStop_En);
            if(u8RecvLen == READLEN - 1)                     //准备接收最后一字节后返回NACK
            {
                I2C_ClearFunc(M0P_I2C0, I2cAck_En);
            }
            break;
        default:
            break;
    } 
    
    I2C_ClearIrq(M0P_I2C0);                                 //清除中断状态标志位
}


/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


