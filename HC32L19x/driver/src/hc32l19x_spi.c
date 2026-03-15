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
 * @file   spi.c
 *
 * @brief  Source file for SPI functions
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 *****************************************************************************/
#include "hc32l19x_spi.h"

/**
 ******************************************************************************
 ** \addtogroup SpiGroup
 *****************************************************************************/
//@{

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 *****************************************************************************/

#define IS_VALID_STAT(x)            (   SpiIf == (x)||\
                                        SpiSserr == (x)||\
                                        SpiBusy == (x)||\
                                        SpiMdf == (x)||\
                                        SpiTxe == (x)||\
                                        SpiRxne == (x))


/******************************************************************************/
/* Local function prototypes ('static')                                       */
/******************************************************************************/

/******************************************************************************/
/* Local variable prototypes ('static')                                       */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief  SPI 请求状态获�?
 **
 ** \param [in]SPIx 通道�?enStatus 获取请求
 **
 ** \retval 请求状�?
 **
 ******************************************************************************/
boolean_t Spi_GetStatus(M0P_SPI_TypeDef* SPIx, en_spi_status_t enStatus)
{
    ASSERT(IS_VALID_STAT(enStatus));

    if(SPIx->STAT&enStatus)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}
/**
 ******************************************************************************
 ** \brief  SPI中断清除
 **
 ** \param [in]SPIx 通道选择
 **
 ** \retval 请求状�?
 **
 ******************************************************************************/
en_result_t Spi_ClearStatus(M0P_SPI_TypeDef* SPIx)
{
    SPIx->ICLR = 0;

    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI 中断使能函数
 **
 ** \param [in] SPIx 通道
 **
 ** \retval Ok成功
 **
 ******************************************************************************/
en_result_t Spi_IrqEnable(M0P_SPI_TypeDef* SPIx)
{
    SPIx->CR2 |= 0x4u;

    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI 中断禁止函数
 **
 ** \param [in] enCh通道
 **
 ** \retval Ok成功
 **
 ******************************************************************************/
en_result_t Spi_IrqDisable(M0P_SPI_TypeDef* SPIx)
{
    SPIx->CR2 &= ~0x4u;

    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI 功能使能函数
 **
 ** \param [in] SPIx 通道，enFunc功能
 **
 ** \retval Ok初始化成�?
 **
 ******************************************************************************/
en_result_t Spi_FuncEnable(M0P_SPI_TypeDef* SPIx, en_spi_func_t enFunc)
{
    SPIx->CR2 |= enFunc;

    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI 功能禁止函数
 **
 ** \param [in] SPIx 通道，enFunc功能
 **
 ** \retval Ok初始化成�?
 **
 ******************************************************************************/
en_result_t Spi_FuncDisable(M0P_SPI_TypeDef* SPIx, en_spi_func_t enFunc)
{
    SPIx->CR2 &= ~(uint32_t)enFunc;

    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI 总体初始化函�?
 **
 ** \param [in] SPIx 通道
 ** \param [in] pstcSpiCfg 初始化结构体
 **
 ** \retval Ok初始化成�?
 ** \retval ErrorInvalidParameter 初始化错�?
 ******************************************************************************/
en_result_t Spi_Init(M0P_SPI_TypeDef* SPIx, stc_spi_cfg_t *pstcSpiCfg)
{
    ASSERT(NULL != pstcSpiCfg);

    SPIx->CR = 0;

    SPIx->SSN = TRUE;

    SPIx->CR =  (uint32_t)pstcSpiCfg->enSpiMode |
                (uint32_t)pstcSpiCfg->enPclkDiv |
                (uint32_t)pstcSpiCfg->enCPOL    |
                (uint32_t)pstcSpiCfg->enCPHA    |
                (uint32_t)0x40;

    SPIx->STAT = 0x00;

    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI 配置主发送的电平
 **
 ** \param [in] SPIx 通道选择，bFlag高低电平
 **
 ** \retval �?
 **
 ******************************************************************************/
void Spi_SetCS(M0P_SPI_TypeDef* SPIx, boolean_t bFlag)
{
    SPIx->SSN = bFlag;
}
/**
 ******************************************************************************
 ** \brief  SPI 发送一字节函数
 **
 ** \param [in] SPIx 通道选择，u8Data发送字�?
 **
 ** \retval Ok发送成�?
 **
 ******************************************************************************/
en_result_t Spi_SendData(M0P_SPI_TypeDef* SPIx, uint8_t u8Data)
{
    SPIx->DATA = u8Data;

    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI �?写一字节函数
 **
 ** \param [in] SPIx   通道选择
 ** \param [in] u8Data 发送一字节数据
 **
 ** \retval 接收一字节数据
 **
 ******************************************************************************/
uint8_t Spi_RWByte(M0P_SPI_TypeDef* SPIx, uint8_t u8Data)
{
    while(FALSE == SPIx->STAT_f.TXE){;}
    SPIx->DATA = u8Data;
    while(FALSE == SPIx->STAT_f.RXNE){;}
    return SPIx->DATA;
}

/**
 ******************************************************************************
** \brief  SPI 从机预准备第一字节数据
 **
 ** \param [in] SPIx   通道选择
 ** \param [in] u8Data 预准备第一字节数据
 **
 ** \retval None
 **
 ******************************************************************************/
void Spi_Slave_DummyWriteData(M0P_SPI_TypeDef* SPIx, uint8_t u8Data)
{
    while(FALSE == SPIx->STAT_f.TXE){;}
    SPIx->DATA = u8Data;
}

/**
 ******************************************************************************
 ** \brief  SPI 连续发送多字节函数
 **
 ** \param [in] SPIx   通道选择
 ** \param [in] pu8Buf 发送数据指�?
 **
 ** \retval Ok发送成�?
 **
 ******************************************************************************/
en_result_t Spi_SendBuf(M0P_SPI_TypeDef* SPIx, uint8_t* pu8Buf, uint32_t u32Len)
{
    uint32_t u32Index=0;

    for(u32Index=0; u32Index<u32Len; u32Index++)
    {
        while(FALSE == SPIx->STAT_f.TXE){;}
        SPIx->DATA = pu8Buf[u32Index];
        while(FALSE == SPIx->STAT_f.RXNE){;}
        pu8Buf[u32Index] = SPIx->DATA;
    }

    while(FALSE == SPIx->STAT_f.TXE){;}
    while(TRUE == SPIx->STAT_f.BUSY){;}

    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI 接收一字节函数
 **
 ** \param [in] SPIx接收通道
 **
 ** \retval 接收一字节数据
 **
 ******************************************************************************/
uint8_t Spi_ReceiveData(M0P_SPI_TypeDef* SPIx)
{
    return SPIx->DATA;
}

/**
 ******************************************************************************
 ** \brief  SPI 连续接收多字节函�?
 **
 ** \param [in] SPIx   通道选择
 ** \param [in] pu8Buf 发送数据指�?
 **
 ** \retval Ok发送成�?
 **
 ******************************************************************************/
en_result_t Spi_ReceiveBuf(M0P_SPI_TypeDef* SPIx, uint8_t* pu8Buf, uint32_t u32Len)
{
    uint32_t u32Index=0;

    for(u32Index=0; u32Index<u32Len; u32Index++)
    {
        while(FALSE == SPIx->STAT_f.TXE){;}
        SPIx->DATA = 0x00;
        while(FALSE == SPIx->STAT_f.RXNE){;}
        pu8Buf[u32Index] = SPIx->DATA;
    }

    while(TRUE == SPIx->STAT_f.BUSY){;}

    return Ok;
}

//@} // SpiGroup
/******************************************************************************
 * EOF (not truncated)
 *****************************************************************************/

