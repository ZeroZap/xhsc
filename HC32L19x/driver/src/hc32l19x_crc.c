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
 * @file   crc.c
 *
 * @brief  Source file for CRC functions
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l19x_crc.h"
/**
 *******************************************************************************
 ** \addtogroup CrcGroup
 ******************************************************************************/
//@{

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/


/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 *****************************************************************************
 ** \brief CRC16 编码(字节填充方式)
 **
 ** 该函数主要用于生成CRC16编码.
 **
 ** \param [in]  pu8Data          待编码数据指针（字节方式输入�?
 ** \param [in]  u32Len           待编码数据长度（字节数）
 ** 
 ** \retval CRC16                 CRC16编码�?                                
 *****************************************************************************/
uint16_t CRC16_Get8(uint8_t* pu8Data, uint32_t u32Len)
{
    uint32_t u32Index = 0;
    
    M0P_CRC->CR_f.CR = 0;
    M0P_CRC->RESULT = 0xFFFF;
    for(u32Index = 0;u32Index<u32Len;u32Index++)
    {
            *((volatile uint8_t*)(&(M0P_CRC->DATA)))  = pu8Data[u32Index];
    }

    return (M0P_CRC->RESULT_f.RESULT);
}

/**
 *****************************************************************************
 ** \brief CRC16 编码(半字填充方式)
 **
 ** 该函数主要用于生成CRC16编码.
 **
 ** \param [in]  pu16Data          待编码数据指针（半字方式输入�?
 ** \param [in]  u32Len           待编码数据长度（半字数）
 ** 
 ** \retval CRC16                 CRC16编码�?                                
 *****************************************************************************/
uint16_t CRC16_Get16(uint16_t* pu16Data, uint32_t u32Len)
{
    uint32_t u32Index = 0;
    
    M0P_CRC->CR_f.CR = 0;
    M0P_CRC->RESULT_f.RESULT = 0xFFFF;
    for (u32Index=0; u32Index<u32Len; u32Index++)
    {
        *((volatile uint16_t*)(&(M0P_CRC->DATA))) = pu16Data[u32Index];
    }
  
    return (M0P_CRC->RESULT_f.RESULT);
}

/**
 *****************************************************************************
 ** \brief CRC16 编码(字填充方�?
 **
 ** 该函数主要用于生成CRC16编码.
 **
 ** \param [in]  pu32Data          待编码数据指针（字方式输入）
 ** \param [in]  u32Len            待编码数据长度（字数�?
 ** 
 ** \retval CRC16                  CRC16编码�?                                
 *****************************************************************************/
uint16_t CRC16_Get32(uint32_t* pu32Data, uint32_t u32Len)
{
    uint32_t u32Index = 0;
    
    M0P_CRC->CR_f.CR = 0;
    M0P_CRC->RESULT_f.RESULT = 0xFFFF;
    for (u32Index=0; u32Index<u32Len; u32Index++)
    {
        M0P_CRC->DATA_f.DATA = pu32Data[u32Index];
    }
  
    return (M0P_CRC->RESULT_f.RESULT);
}

/**
 *****************************************************************************
 ** \brief CRC16 校验(字节填充方式)
 **
 ** 该函数主要用于对数据及CRC16值进行校�?
 **
 ** \param [in]  pu8Data          待校验数据指针（字节方式输入�?
 ** \param [in]  u32Len           待校验数据长度（字节数）
 ** \param [in]  u16CRC           待校验CRC16�?
 ** 
 ** \retval Ok                    CRC校验正确
 ** \retval Error                 CRC校验错误                                
 *****************************************************************************/
en_result_t CRC16_Check8(uint8_t* pu8Data, uint32_t u32Len, uint16_t u16CRC)
{
    en_result_t enResult = Ok;
    uint32_t    u32Index = 0;
    
    M0P_CRC->CR_f.CR = 0;    
    M0P_CRC->RESULT_f.RESULT = 0xFFFF;
    for (u32Index=0; u32Index<u32Len; u32Index++)
    {
        *((volatile uint8_t*)(&(M0P_CRC->DATA))) = pu8Data[u32Index];
    }
    
     *((volatile uint8_t*)(&(M0P_CRC->DATA))) = (uint8_t)((((uint32_t)u16CRC)>>0)&0xFF);
     *((volatile uint8_t*)(&(M0P_CRC->DATA))) = (uint8_t)(((uint32_t)u16CRC>>8)&0xFF);
    
    enResult = M0P_CRC->CR_f.FLAG ? Ok : Error;
    
    return (enResult); 
}

/**
 *****************************************************************************
 ** \brief CRC16 校验(半字填充方式)
 **
 ** 该函数主要用于对数据及CRC16值进行校�?
 **
 ** \param [in]  pu16Data         待校验数据指针（半字方式输入�?
 ** \param [in]  u32Len           待校验数据长度（半字数）
 ** \param [in]  u16CRC           待校验CRC16�?
 ** 
 ** \retval Ok                    CRC校验正确
 ** \retval Error                 CRC校验错误                                
 *****************************************************************************/
en_result_t CRC16_Check16(uint16_t* pu16Data, uint32_t u32Len, uint16_t u16CRC)
{
    en_result_t enResult = Ok;
    uint32_t    u32Index = 0;

    M0P_CRC->CR_f.CR = 0;    
    M0P_CRC->RESULT_f.RESULT = 0xFFFF;
    for (u32Index=0; u32Index<u32Len; u32Index++)
    {
        *((volatile uint16_t*)(&(M0P_CRC->DATA))) = pu16Data[u32Index];
    }
    
     *((volatile uint16_t*)(&(M0P_CRC->DATA))) = u16CRC;
    
    enResult = M0P_CRC->CR_f.FLAG ? Ok : Error;
    
    return (enResult); 
}

/**
 *****************************************************************************
 ** \brief CRC16 校验(字填充方�?
 **
 ** 该函数主要用于对数据及CRC16值进行校�?
 **
 ** \param [in]  pu32Data         待校验数据指针（字方式输入）
 ** \param [in]  u32Len           待校验数据长度（字数�?
 ** \param [in]  u16CRC           待校验CRC16�?
 ** 
 ** \retval Ok                    CRC校验正确
 ** \retval Error                 CRC校验错误                                
 *****************************************************************************/
en_result_t CRC16_Check32(uint32_t* pu32Data, uint32_t u32Len, uint16_t u16CRC)
{
    en_result_t enResult = Ok;
    uint32_t    u32Index = 0;

    M0P_CRC->CR_f.CR = 0;    
    M0P_CRC->RESULT_f.RESULT = 0xFFFFFFFFu;
    for (u32Index=0; u32Index<u32Len; u32Index++)
    {
        *((volatile uint32_t*)(&(M0P_CRC->DATA))) = pu32Data[u32Index];
    }
    
     *((volatile uint16_t*)(&(M0P_CRC->DATA))) = ((uint16_t)u16CRC);
    
    enResult = M0P_CRC->CR_f.FLAG ? Ok : Error;
    
    return (enResult);
}

/**
 *****************************************************************************
 ** \brief CRC16 编码(字节填充方式)
 **
 ** 该函数主要用于生成CRC16编码.
 **
 ** \param [in]  pu8Data          待编码数据指针（字节方式输入�?
 ** \param [in]  u32Len           待编码数据长度（字节数）
 ** 
 ** \retval CRC16                 CRC16编码�?                                
 *****************************************************************************/
uint32_t CRC32_Get8(uint8_t* pu8Data, uint32_t u32Len)
{
    uint32_t u32Index = 0;
    
    M0P_CRC->CR_f.CR = 1;
    M0P_CRC->RESULT = 0xFFFFFFFFu;
    for(u32Index = 0;u32Index<u32Len;u32Index++)
    {
            *((volatile uint8_t*)(&(M0P_CRC->DATA)))  = pu8Data[u32Index];
    }

    return (M0P_CRC->RESULT_f.RESULT);
}

/**
 *****************************************************************************
 ** \brief CRC16 编码(半字填充方式)
 **
 ** 该函数主要用于生成CRC16编码.
 **
 ** \param [in]  pu16Data          待编码数据指针（半字方式输入�?
 ** \param [in]  u32Len           待编码数据长度（半字数）
 ** 
 ** \retval CRC16                 CRC16编码�?                                
 *****************************************************************************/
uint32_t CRC32_Get16(uint16_t* pu16Data, uint32_t u32Len)
{
    uint32_t u32Index = 0;
    
    M0P_CRC->CR_f.CR = 1;
    M0P_CRC->RESULT_f.RESULT = 0xFFFFFFFFu;
    for (u32Index=0; u32Index<u32Len; u32Index++)
    {
        *((volatile uint16_t*)(&(M0P_CRC->DATA))) = pu16Data[u32Index];
    }
  
    return (M0P_CRC->RESULT_f.RESULT);
}

/**
 *****************************************************************************
 ** \brief CRC16 编码(字填充方�?
 **
 ** 该函数主要用于生成CRC16编码.
 **
 ** \param [in]  pu32Data          待编码数据指针（字方式输入）
 ** \param [in]  u32Len            待编码数据长度（字数�?
 ** 
 ** \retval CRC16                  CRC16编码�?                                
 *****************************************************************************/
uint32_t CRC32_Get32(uint32_t* pu32Data, uint32_t u32Len)
{
    uint32_t u32Index = 0;
    
    M0P_CRC->CR_f.CR = 1;    
    M0P_CRC->RESULT_f.RESULT = 0xFFFFFFFFu;
    for (u32Index=0; u32Index<u32Len; u32Index++)
    {
        M0P_CRC->DATA_f.DATA = pu32Data[u32Index];
    }
  
    return (M0P_CRC->RESULT_f.RESULT);
}

/**
 *****************************************************************************
 ** \brief CRC16 校验(字节填充方式)
 **
 ** 该函数主要用于对数据及CRC16值进行校�?
 **
 ** \param [in]  pu8Data          待校验数据指针（字节方式输入�?
 ** \param [in]  u32Len           待校验数据长度（字节数）
 ** \param [in]  u16CRC           待校验CRC16�?
 ** 
 ** \retval Ok                    CRC校验正确
 ** \retval Error                 CRC校验错误                                
 *****************************************************************************/
en_result_t CRC32_Check8(uint8_t* pu8Data, uint32_t u32Len, uint32_t u32CRC)
{
    en_result_t enResult = Ok;
    uint32_t    u32Index = 0;
    
    M0P_CRC->CR_f.CR = 1;     
    M0P_CRC->RESULT_f.RESULT = 0xFFFFFFFFu;
    for (u32Index=0; u32Index<u32Len; u32Index++)
    {
        *((volatile uint8_t*)(&(M0P_CRC->DATA))) = pu8Data[u32Index];
    }
    
     *((volatile uint8_t*)(&(M0P_CRC->DATA))) = (uint8_t)((u32CRC>>0)&0xFF);
     *((volatile uint8_t*)(&(M0P_CRC->DATA))) = (uint8_t)((u32CRC>>8)&0xFF);
     *((volatile uint8_t*)(&(M0P_CRC->DATA))) = (uint8_t)((u32CRC>>16)&0xFF);
     *((volatile uint8_t*)(&(M0P_CRC->DATA))) = (uint8_t)((u32CRC>>24)&0xFF);
        
    enResult = M0P_CRC->CR_f.FLAG ? Ok : Error;
    
    return (enResult); 
}

/**
 *****************************************************************************
 ** \brief CRC16 校验(半字填充方式)
 **
 ** 该函数主要用于对数据及CRC16值进行校�?
 **
 ** \param [in]  pu16Data         待校验数据指针（半字方式输入�?
 ** \param [in]  u32Len           待校验数据长度（半字数）
 ** \param [in]  u16CRC           待校验CRC16�?
 ** 
 ** \retval Ok                    CRC校验正确
 ** \retval Error                 CRC校验错误                                
 *****************************************************************************/
en_result_t CRC32_Check16(uint16_t* pu16Data, uint32_t u32Len, uint32_t u32CRC)
{
    en_result_t enResult = Ok;
    uint32_t    u32Index = 0;
    
    M0P_CRC->CR_f.CR = 1;     
    M0P_CRC->RESULT_f.RESULT = 0xFFFFFFFFu;
    for (u32Index=0; u32Index<u32Len; u32Index++)
    {
        *((volatile uint16_t*)(&(M0P_CRC->DATA))) = pu16Data[u32Index];
    }
    
     *((volatile uint16_t*)(&(M0P_CRC->DATA))) = (uint16_t)((u32CRC>>0)&0xFFFF);
     *((volatile uint16_t*)(&(M0P_CRC->DATA))) = (uint16_t)((u32CRC>>16)&0xFFFF);

    
    enResult = M0P_CRC->CR_f.FLAG ? Ok : Error;
    
    return (enResult); 
}

/**
 *****************************************************************************
 ** \brief CRC16 校验(字填充方�?
 **
 ** 该函数主要用于对数据及CRC16值进行校�?
 **
 ** \param [in]  pu32Data         待校验数据指针（字方式输入）
 ** \param [in]  u32Len           待校验数据长度（字数�?
 ** \param [in]  u16CRC           待校验CRC16�?
 ** 
 ** \retval Ok                    CRC校验正确
 ** \retval Error                 CRC校验错误                                
 *****************************************************************************/
en_result_t CRC32_Check32(uint32_t* pu32Data, uint32_t u32Len, uint32_t u32CRC)
{
    en_result_t enResult = Ok;
    uint32_t    u32Index = 0;
    
    M0P_CRC->CR_f.CR = 1;     
    M0P_CRC->RESULT_f.RESULT = 0xFFFFFFFFu;
    for (u32Index=0; u32Index<u32Len; u32Index++)
    {
        *((volatile uint32_t*)(&(M0P_CRC->DATA))) = pu32Data[u32Index];
    }
    
     *((volatile uint32_t*)(&(M0P_CRC->DATA))) = u32CRC;
    
    enResult = M0P_CRC->CR_f.FLAG ? Ok : Error;
    
    return (enResult);
}
//@} // CrcGroup

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
