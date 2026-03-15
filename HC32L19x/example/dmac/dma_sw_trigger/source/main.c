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
 * @brief  Source file for DMAC example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
* Include files
******************************************************************************/
#include "ddl.h"
#include "hc32l19x_dmac.h"
/******************************************************************************
* Local pre-processor symbols/macros ('#define')                            
******************************************************************************/
#define BUFFER_SIZE 32U

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
/* Private variables ---------------------------------------------------------*/
// DMA传输源地址
static uint32_t aSRC_Const_Buffer[BUFFER_SIZE] =
{
  0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
  0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
  0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
  0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
  0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
  0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,
  0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
  0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};
// DMA传输目标地址
static uint32_t aDST_Buffer[BUFFER_SIZE];
/*****************************************************************************
* Function implementation - global ('extern') and local ('static')
******************************************************************************/
void App_DmaCfg(void);
static int32_t myMemcmp(uint8_t *mem1,uint8_t *mem2,uint32_t bytesize);
static void Error_Handle();
/**
******************************************************************************
** \brief  Main function of project
**
** \return uint32_t return value, if needed
**
******************************************************************************/
int32_t main(void)
{   
    // DMA通道配置
    App_DmaCfg();
    
    // 软件触发DMA 通道0传输
    Dma_SwStart(DmaCh0);
  
    //等待传输完成
    while(Dma_GetStat(DmaCh0) != DmaTransferComplete);
    
    //清除DMA状�?
    Dma_ClrStat(DmaCh0);   
  
    //判断传输的数据是否正�?
    if(myMemcmp((uint8_t *)&aDST_Buffer[0],(uint8_t *)&aSRC_Const_Buffer[0],BUFFER_SIZE) == -1)
    {
        Error_Handle();  //如果数据对比有错误，进入错误处理函数
    }

    while(1)
    {
        ;
    }
}

//内存数据比较
static int32_t myMemcmp(uint8_t *mem1,uint8_t *mem2,uint32_t bytesize)
{
    int i = 0;
    uint8_t *p = mem1;
    uint8_t *q = mem2;
    if(p == NULL|| q == NULL)
    {
        return -1;
    }
    
    for(i = 0;i < bytesize;i++,p++,q++)
    {
        if(*p != *q)
        {
            return -1;
        }
    }
    return 0;
}

// 错误处理函数
static void Error_Handle()
{
    while(1);
}

// DMA通道配置
void App_DmaCfg(void)
{ 
    stc_dma_cfg_t stcDmaCfg;
  
    //打开DMA时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralDma,TRUE);
  
    DDL_ZERO_STRUCT(stcDmaCfg);
    DDL_ZERO_STRUCT(aDST_Buffer);
  
    stcDmaCfg.enMode =  DmaMskBlock;                           //选择块传�?
    stcDmaCfg.u16BlockSize = 0x01;                             //块传输个�?
    stcDmaCfg.u16TransferCnt = BUFFER_SIZE;                    //块传输次数，一次传输数据大小为 块传输个�?BUFFER_SIZE
    stcDmaCfg.enTransferWidth = DmaMsk32Bit;                   //传输数据的宽度，此处选择�?32Bit)宽度
    stcDmaCfg.enSrcAddrMode = DmaMskSrcAddrInc;                //源地址自增
    stcDmaCfg.enDstAddrMode = DmaMskDstAddrInc;                //目的地址自增
    stcDmaCfg.enDestAddrReloadCtl = DmaMskDstAddrReloadDisable;//禁止重新加载传输目的地址
    stcDmaCfg.enSrcAddrReloadCtl = DmaMskSrcAddrReloadDisable; //禁止重新加载传输源地址
    stcDmaCfg.enSrcBcTcReloadCtl = DmaMskBcTcReloadDisable;    //禁止重新加载BC/TC�?
    stcDmaCfg.u32SrcAddress = (uint32_t)&aSRC_Const_Buffer[0]; //指定传输源地址
    stcDmaCfg.u32DstAddress = (uint32_t)&aDST_Buffer[0];       //指定传输目的地址
    stcDmaCfg.enRequestNum = DmaSWTrig;                        //设置为软件触�?
    stcDmaCfg.enTransferMode = DmaMskOneTransfer;              //dma只传输一次，DMAC传输完成时清除CONFA:ENS�?
    stcDmaCfg.enPriority = DmaMskPriorityFix;                  //各通道固定优先级，CH0优先�?> CH1优先�?
    
    Dma_InitChannel(DmaCh0,&stcDmaCfg);                        //初始化dma通道0
  
    //使能DMA，使能DMA0，清除DMA0状�?
    Dma_Enable();
    Dma_EnableChannel(DmaCh0);
    Dma_ClrStat(DmaCh0);    
}

/******************************************************************************
* EOF (not truncated)
******************************************************************************/


