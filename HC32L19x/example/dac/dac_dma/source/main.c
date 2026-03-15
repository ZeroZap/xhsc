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
 * @brief  Source file for DAC example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l19x_sysctrl.h"
#include "hc32l19x_dac.h"
#include "hc32l19x_flash.h"
#include "hc32l19x_gpio.h"
#include "math.h"
#include "hc32l19x_dmac.h"
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
uint16_t SendBuf[128];
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                             
 ******************************************************************************/
#define PI 3.14159265358979           //圆周�?
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
static void App_SysClkInit(void);               ///< 系统时钟初始�?
static void App_GpioInit(void);                 ///< GPIO初始�?
static void App_DACInit(void);                  ///< DAC初始�?
static void App_DMAInit(void);                  ///< DMA初始�?

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
    App_SysClkInit();               ///< 系统时钟初始�?
    App_GpioInit();                 ///< GPIO初始�?
    App_DACInit();                  ///< DAC初始�?
    App_DMAInit();                  ///< DMA初始�?
    
    while(1)
    {
        Dac_SoftwareTriggerCmd();   ///< 软件触发
        delay10us(12);
    }
}    

/**
 ******************************************************************************
 ** \brief  初始化外部系统时�?
 **
 ** \return �?
 ******************************************************************************/
static void App_SysClkInit(void)
{
    stc_sysctrl_clk_cfg_t stcCfg;
    stc_sysctrl_pll_cfg_t stcPLLCfg;
    
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralFlash, TRUE);    ///< 使能FLASH模块的外设时�?
    Flash_WaitCycle(FlashWaitCycle1);
    Sysctrl_SetRCHTrim(SysctrlRchFreq4MHz);             ///< PLL使用RCH作为时钟源，因此需要先设置RCH    
    
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;     ///< RCH 4MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  ///< PLL 输出48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              ///< 输入时钟源选择RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;            ///< 4MHz x 12 = 48MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);
    
    ///< 选择PLL作为HCLK时钟�?
    stcCfg.enClkSrc  = SysctrlClkPLL;
    ///< HCLK SYSCLK/2
    stcCfg.enHClkDiv = SysctrlHclkDiv1;
    ///< PCLK 为HCLK/8
    stcCfg.enPClkDiv = SysctrlPclkDiv1;
    ///< 系统时钟初始�?
    Sysctrl_ClkInit(&stcCfg);
}



/**
 ******************************************************************************
 ** \brief  初始化外部GPIO引脚
 **
 ** \return �?
 ******************************************************************************/
static void App_GpioInit(void)
{    
    ///< 开启GPIO外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);    // 使能GPIO模块的外设时�?
    
    Gpio_SetAnalogMode(GpioPortA, GpioPin4);       //PA04作为DAC的模拟输�?
}

/**
 ******************************************************************************
 ** \brief  产生正弦波的采样�?
 ** \param  buf:用于存放正弦波采样点的缓�?
 ** \return �?
 ******************************************************************************/
static void _BufProduce(uint16_t* buf)
{
    uint8_t tmp;
    double tmp1;
    tmp1=PI/64;
    for(tmp=0; tmp<128; tmp++)
    {
        buf[tmp] =(uint16_t)(((sin(tmp1*tmp))*2047)+2048);
    }        
}

/**
******************************************************************************
    ** \brief  配置DAC
    ** 
    ** @param  �?
    ** \retval �?
    **
******************************************************************************/ 
static void App_DACInit(void)
{
    stc_dac_cfg_t  dac_initstruct;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralDac, TRUE);     ///< 使能DAC模块的时�?
    
    dac_initstruct.boff_t = DacBoffDisable;
    dac_initstruct.ten_t  = DacTenEnable;
    dac_initstruct.sref_t = DacVoltageAvcc;
    dac_initstruct.mamp_t = DacMenp4095;
    dac_initstruct.tsel_t = DacSwTriger;      ///< 软件触发方式
    dac_initstruct.align  = DacRightAlign;    ///< 右对�?
    Dac_Init(&dac_initstruct);
    Dac_Cmd(TRUE);
    Dac_DmaCmd(TRUE);                       ///< DAC通道DMA使能
    
    _BufProduce(SendBuf);                   ///< 产生正弦波的采样�?
    
}

/**
******************************************************************************
    ** \brief  配置DMA
    ** 
    ** @param  �?
    ** \retval �?
    **
******************************************************************************/ 
static void App_DMAInit(void)
{
    stc_dma_cfg_t DmaInitStruct;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralDma, TRUE);          ///< 使能DMA模块的外设时�?
    
    DmaInitStruct.enMode =  DmaMskBlock;                            ///< 选择块传�?
    DmaInitStruct.u16BlockSize = 1;                                 ///< 块传输个�?
    DmaInitStruct.u16TransferCnt = 128;                             ///< 块传输次数，一次传输数据大小为 块传输个�?BUFFER_SIZE
    DmaInitStruct.enTransferWidth = DmaMsk16Bit;                    ///< 传输数据的宽度，此处选择�?16Bit)宽度
    DmaInitStruct.enSrcAddrMode = DmaMskSrcAddrInc;                 ///< 源地址自增
    DmaInitStruct.enDstAddrMode = DmaMskDstAddrFix;                 ///< 目的地址自增
    DmaInitStruct.enDestAddrReloadCtl = DmaMskDstAddrReloadDisable; ///< 禁止重新加载传输目的地址
    DmaInitStruct.enSrcAddrReloadCtl = DmaMskSrcAddrReloadEnable;   ///< 使能重新加载传输源地址
    DmaInitStruct.enSrcBcTcReloadCtl = DmaMskBcTcReloadEnable;      ///< 使能重新加载BC/TC�?
    DmaInitStruct.u32SrcAddress = (uint32_t)SendBuf;                ///< 源地址
    DmaInitStruct.u32DstAddress = 0x40002508;                       ///< 目标地址：DAC_DHR12R0 
    DmaInitStruct.enRequestNum = DmaDAC0Trig;                       ///< 设置DAC0触发
    DmaInitStruct.enTransferMode = DmaMskContinuousTransfer;        ///< 连续传输
    DmaInitStruct.enPriority = DmaMskPriorityFix;                   ///< 各通道固定优先级，CH0优先�?> CH1优先�?   
    Dma_InitChannel(DmaCh0,&DmaInitStruct);                         ///< 初始化dma通道0
    
    Dma_Enable();                                                   ///< 使能DMA
    Dma_EnableChannel(DmaCh0);                                      ///< 使能DMA通道0
    Dma_ClrStat(DmaCh0);                                            ///< 清零：STAT[2:0]           
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


