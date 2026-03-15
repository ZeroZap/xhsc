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
 * @file   flash.c
 *
 * @brief  Source file for FLASH functions
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l19x_flash.h"
/**
 *******************************************************************************
 ** \addtogroup FlashGroup
 ******************************************************************************/
//@{

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define FLASH_END_ADDR              (0x0003FFFFu)
#define FLASH_BYPASS()              do{ M0P_FLASH->BYPASS = 0x5A5A;\
                                        M0P_FLASH->BYPASS = 0xA5A5;\
                                    }while(0); 
#define FLASH_IE_TRUE               (0x03)
#define FLASH_IE_FALSE              (0x00)

#define FLASH_TIMEOUT_INIT          (0xFFFFFFu)
#define FLASH_TIMEOUT_PGM           (0xFFFFFFu)
#define FLASH_TIMEOUT_ERASE         (0xFFFFFFu)

#define FLASH_LOCK_ALL              (0u)
#define FLASH_UNLOCK_ALL            (0xFFFFFFFFu)
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/**
 ******************************************************************************
 ** \brief FLASH 编程时间参数配置
 ** 
 ** FLASH编程时间参数配置数组定义 (4MHz)
 ******************************************************************************/
const uint32_t pu32PcgTimer4M[] = { 
                                    0x20u,          //Tnvs   
                                    0x17u,          //Tpgs   
                                    0x1Bu,          //Tprog  
                                    0x4650u,        //Tserase
                                    0x222E0u,       //Tmerase
                                    0x18u,          //Tprcv  
                                    0xF0u,          //Tsrcv  
                                    0x3E8u          //Tmrcv  
                                  };
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
 ** \brief Flash中断标志获取
 **
 **
 ** \param [in]  enFlashIntType          Flash中断类型
 ** 
 ** \retval TRUE or FALSE                                      
 *****************************************************************************/
boolean_t Flash_GetIntFlag(en_flash_int_type_t enFlashIntType)
{
    boolean_t bRetVal = FALSE;
    
    if(M0P_FLASH->IFR & enFlashIntType)
    {
        bRetVal =  TRUE;
    }
    
    return bRetVal;
}

/**
 *****************************************************************************
 ** \brief Flash中断标志清除
 **
 **
 ** \param [in]  enFlashIntType          Flash中断类型
 ** 
 ** \retval Ok or Error                                      
 *****************************************************************************/
en_result_t Flash_ClearIntFlag(en_flash_int_type_t enFlashIntType)
{
    en_result_t enResult = Error;

    M0P_FLASH->ICLR &= ~(uint32_t)enFlashIntType;
    enResult = Ok;
    
    return enResult;
}

/**
 *****************************************************************************
 ** \brief Flash中断使能
 **
 **
 ** \param [in]  enFlashIntType          Flash中断类型
 ** 
 ** \retval Ok or Error                                      
 *****************************************************************************/
en_result_t Flash_EnableIrq (en_flash_int_type_t enFlashIntType)
{
    en_result_t enResult = Error;

    FLASH_BYPASS();
    M0P_FLASH->CR_f.IE |= enFlashIntType;
    
    enResult = Ok;

    return enResult;
}

/**
 *****************************************************************************
 ** \brief Flash中断禁止
 **
 **
 ** \param [in]  enFlashIntType          Flash中断类型
 ** 
 ** \retval Ok or Error                                      
 *****************************************************************************/
en_result_t Flash_DisableIrq(en_flash_int_type_t enFlashIntType)
{
    en_result_t enResult = Error;

    FLASH_BYPASS();
    M0P_FLASH->CR_f.IE &= ~(uint32_t)enFlashIntType;
    
    enResult = Ok;

    return enResult;
}

/**
 *****************************************************************************
 ** \brief FLASH 初始化函数——中断服务程序、编程时间配置及低功耗模�?
 **
 ** 该函数用于配置中断服务函数、低功耗模式、根据系统时钟配置FLASH编程时间相关寄存�?
 **
 ** \param [in]  u8FreqCfg        FLASH编程时钟频率配置(根据HCLK的频率选择配置�?�?
 **                               1      - 4MHz;
 **                               2      - 8MHz;
 **                               4      - 16MHz;
 **                               6      - 24MHz;
 **                               8      - 32MHz;
 **                               12     - 48MHz;
 **                               other   -  无效�?
 ** \param [in] bDpstbEn          TRUE  - 当系统进入DeepSleep模式，FLASH进入低功耗模�?
 **                               FALSE - 当系统进入DeepSleep模式，FLASH不进入低功耗模�?
 ** 
 ** \retval Ok                    操作成功.
 ** \retval ErrorInvalidParameter 参数无效.
 ** \retval ErrorUninitialized    初始化失败�?
 *****************************************************************************/
en_result_t Flash_Init(uint8_t u8FreqCfg, boolean_t bDpstbEn)
{
    uint32_t                u32Index  = 0;
    volatile uint32_t       u32TimeOut = FLASH_TIMEOUT_INIT;
    en_result_t             enResult  = Error;
    uint32_t                u32PrgTimer[8] = {0}; 
    volatile uint32_t       *pu32PrgTimerReg = (volatile uint32_t*)M0P_FLASH;
    
    if ((1  != u8FreqCfg) && (2  != u8FreqCfg) &&
        (4  != u8FreqCfg) && (6  != u8FreqCfg) &&
        (8  != u8FreqCfg) && (12 != u8FreqCfg))
    {
        enResult = ErrorInvalidParameter;
        return (enResult);
    }
    
    FLASH_BYPASS();
    M0P_FLASH->CR_f.DPSTB_EN = bDpstbEn;
    if(bDpstbEn != M0P_FLASH->CR_f.DPSTB_EN)
    {
        enResult = ErrorUninitialized;
        return (enResult);
    }
    
    //flash时间参数配置值计�?
    for(u32Index=0; u32Index<8; u32Index++)
    {
        u32PrgTimer[u32Index] = u8FreqCfg * pu32PcgTimer4M[u32Index];
    } 
    
    //flash时间参数寄存器配�?
    for(u32Index=0; u32Index<8; u32Index++)
    {
        u32TimeOut = FLASH_TIMEOUT_INIT;
        while(pu32PrgTimerReg[u32Index]  != u32PrgTimer[u32Index])
        {
            if(u32TimeOut--)
            {
                FLASH_BYPASS();
                pu32PrgTimerReg[u32Index] = u32PrgTimer[u32Index];
            }
            else
            {
                enResult = ErrorUninitialized;
                return ErrorUninitialized;
            }
        }
    }
    
    enResult = Ok;
    return (enResult);
}

/**
 *****************************************************************************
 ** \brief FLASH 连续字节编程
 **
 ** 以字节方式向FLASH写入连续的数�?
 **
 ** \param [in]  u32Addr          Flash目标首地址
 ** \param [in]  pu8Data[]        数据Buffer首地址
 ** \param [in]  u32Len           写入数据长度
 ** 
 ** \retval Ok                    写入成功.
 ** \retval ErrorInvalidParameter FLASH地址无效
 ** \retval ErrorTimeout          操作超时
 ** \retval Error                 编程、校验失�?
 ** \retval ErrorInvalidMode      操作模式无效
 *****************************************************************************/
en_result_t Flash_Write8(uint32_t u32Addr, uint8_t pu8Data[], uint32_t u32Len)
{
    en_result_t             enResult = Error;    
    volatile uint32_t       u32TimeOut = FLASH_TIMEOUT_PGM;
    uint32_t                u32Index = 0;

    if(FlashWriteMode != M0P_FLASH->CR_f.OP)
    {
        return ErrorInvalidMode;
    }
    
    if (FLASH_END_ADDR < (u32Addr + u32Len - 1))
    {
        enResult = ErrorInvalidParameter;
        return (enResult);
    }
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    //write data byte
    for(u32Index=0; u32Index<u32Len; u32Index++)
    {
        *((volatile uint8_t*)u32Addr) = pu8Data[u32Index];
    
        //busy?
        u32TimeOut = FLASH_TIMEOUT_PGM;
        while (TRUE == M0P_FLASH->CR_f.BUSY)
        {
            if(0 == u32TimeOut--)
            {
                return ErrorTimeout;
            }
        }
        
        if(pu8Data[u32Index] != *((volatile uint8_t*)u32Addr))
        {
            return Error;
        }
        u32Addr++;
    }

    enResult = Ok;
    return (enResult);
}

/**
 *****************************************************************************
 ** \brief FLASH 连续半字�?6位方式）编程
 **
 ** 以半字方式向FLASH写入连续的数�?
 **
 ** \param [in]  u32Addr          Flash目标首地址
 ** \param [in]  pu16Data[]       数据Buffer首地址
 ** \param [in]  u32Len           写入数据长度
 ** 
 ** \retval Ok                    写入成功.
 ** \retval ErrorInvalidParameter FLASH地址无效
 ** \retval ErrorTimeout          操作超时
 ** \retval Error                 编程、校验失�?
 ** \retval ErrorInvalidMode      操作模式无效
 *****************************************************************************/
en_result_t Flash_Write16(uint32_t u32Addr, uint16_t pu16Data[], uint32_t u32Len)
{
    en_result_t             enResult = Error;    
    volatile uint32_t       u32TimeOut = FLASH_TIMEOUT_PGM;
    uint32_t                u32Index = 0;

    if(FlashWriteMode != M0P_FLASH->CR_f.OP)
    {
        return ErrorInvalidMode;
    }
    
    if (FLASH_END_ADDR < (u32Addr + (u32Len << 1UL) - 1UL))
    {
        enResult = ErrorInvalidParameter;
        return (enResult);
    }
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    //write data byte
    for(u32Index=0; u32Index<u32Len; u32Index++)
    {
        *((volatile uint16_t*)u32Addr) = pu16Data[u32Index];

        //busy?
        u32TimeOut = FLASH_TIMEOUT_PGM;
        while (TRUE == M0P_FLASH->CR_f.BUSY)
        {
            if(0 == u32TimeOut--)
            {
                return ErrorTimeout;
            }
        }
        
        if(pu16Data[u32Index] != *((volatile uint16_t*)u32Addr))
        {
            return Error;
        }
        u32Addr+=2;
    }

    enResult = Ok;    
    return (enResult);
}

/**
 *****************************************************************************
 ** \brief FLASH 连续字（32位方式）编程
 **
 ** 以字方式向FLASH写入连续的数�?
 **
 ** \param [in]  u32Addr          Flash目标首地址
 ** \param [in]  pu32Data[]       数据Buffer首地址
 ** \param [in]  u32Len           写入数据长度
 ** 
 ** \retval Ok                    写入成功.
 ** \retval ErrorInvalidParameter FLASH地址无效
 ** \retval ErrorTimeout          操作超时
 ** \retval Error                 编程、校验失�?
 ** \retval ErrorInvalidMode      操作模式无效
 *****************************************************************************/
en_result_t Flash_Write32(uint32_t u32Addr, uint32_t pu32Data[], uint32_t u32Len)
{
    en_result_t             enResult = Error;    
    volatile uint32_t       u32TimeOut = FLASH_TIMEOUT_PGM;
    uint32_t                u32Index = 0;

    if(FlashWriteMode != M0P_FLASH->CR_f.OP)
    {
        return ErrorInvalidMode;
    }
    
    if (FLASH_END_ADDR < (u32Addr + (u32Len << 2UL) - 1))
    {
        enResult = ErrorInvalidParameter;
        return (enResult);
    }
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    //write data byte
    for(u32Index=0; u32Index<u32Len; u32Index++)
    {
        *((volatile uint32_t*)u32Addr) = pu32Data[u32Index];

        //busy?
        u32TimeOut = FLASH_TIMEOUT_PGM;
        while (TRUE == M0P_FLASH->CR_f.BUSY)
        {
            if(0 == u32TimeOut--)
            {
                return ErrorTimeout;
            }
        }
        
        if(pu32Data[u32Index] != *((volatile uint32_t*)u32Addr))
        {
            return Error;
        }
        u32Addr+=4;
    }
    
    enResult = Ok;    
    return (enResult);
}

/**
 *****************************************************************************
 ** \brief FLASH 扇区擦除
 **
 ** 对目标地址所在的FLASH 扇区进行擦除，擦除后该扇区FLASH数据为全0xFF.
 **
 ** \param [in]  u32SectorAddr    所擦除扇区内的地址
 ** 
 ** \retval Ok                    擦除成功.
 ** \retval ErrorInvalidParameter FLASH地址无效 
 ** \retval ErrorTimeout          操作超时
 ** \retval ErrorInvalidMode      操作模式无效
 *****************************************************************************/
en_result_t Flash_SectorErase(uint32_t u32SectorAddr)
{
    en_result_t             enResult = Ok;    
    volatile uint32_t       u32TimeOut = FLASH_TIMEOUT_ERASE;
    
    if(FlashSectorEraseMode != M0P_FLASH->CR_f.OP)
    {
        return ErrorInvalidMode;
    }
    
    if (FLASH_END_ADDR < u32SectorAddr)
    {
        enResult = ErrorInvalidParameter;
        return (enResult);
    }
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_ERASE;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    //write data
    *((volatile uint32_t*)u32SectorAddr) = 0;
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_ERASE;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    return (enResult);
}

/**
 *****************************************************************************
 ** \brief FLASH 操作模式配置
 **
 ** FLASH 操作模式配置.
 **
 ** \param [in]  enFlashOpMode    @ref en_flash_op_mode_t
 ** 
 ** \retval Ok                    配置成功
 ** \retval Error                 配置失败 
 *****************************************************************************/
en_result_t Flash_OpModeConfig(en_flash_op_mode_t enFlashOpMode)
{
    en_result_t enResult = Error;

    FLASH_BYPASS();
    M0P_FLASH->CR_f.OP = enFlashOpMode;
    
    if(enFlashOpMode == M0P_FLASH->CR_f.OP)
    {
        enResult = Ok;
    }
    
    return enResult;
}

/**
 *****************************************************************************
 ** \brief FLASH 编程保护加锁
 **
 ** 
 ** \retval Null                               
 *****************************************************************************/
en_result_t Flash_LockAll(void)
{
    en_result_t enResult = Error;
    
    FLASH_BYPASS();
    M0P_FLASH->SLOCK0 = FLASH_LOCK_ALL;
    FLASH_BYPASS();
    M0P_FLASH->SLOCK1 = FLASH_LOCK_ALL;    
    FLASH_BYPASS();
    M0P_FLASH->SLOCK2 = FLASH_LOCK_ALL;
    FLASH_BYPASS();
    M0P_FLASH->SLOCK3 = FLASH_LOCK_ALL;

    if( (FLASH_LOCK_ALL == M0P_FLASH->SLOCK0) &&
        (FLASH_LOCK_ALL == M0P_FLASH->SLOCK1) &&
        (FLASH_LOCK_ALL == M0P_FLASH->SLOCK2) &&
        (FLASH_LOCK_ALL == M0P_FLASH->SLOCK3) )
    {
        enResult = Ok;
    }
    
    return enResult;
}

/**
 *****************************************************************************
 ** \brief FLASH 编程保护解锁
 **
 ** 
 ** \retval Ok          加锁成功
 ** \retval Error       加锁失败                            
 *****************************************************************************/
en_result_t Flash_UnlockAll(void)
{
    en_result_t enResult = Error;
    
    FLASH_BYPASS();
    M0P_FLASH->SLOCK0 = FLASH_UNLOCK_ALL;
    FLASH_BYPASS();
    M0P_FLASH->SLOCK1 = FLASH_UNLOCK_ALL;
    FLASH_BYPASS();
    M0P_FLASH->SLOCK2 = FLASH_UNLOCK_ALL;
    FLASH_BYPASS();
    M0P_FLASH->SLOCK3 = FLASH_UNLOCK_ALL;
    
    if( (FLASH_UNLOCK_ALL == M0P_FLASH->SLOCK0) &&
        (FLASH_UNLOCK_ALL == M0P_FLASH->SLOCK1) &&
        (FLASH_UNLOCK_ALL == M0P_FLASH->SLOCK2) &&
        (FLASH_UNLOCK_ALL == M0P_FLASH->SLOCK3) )
    {
        enResult = Ok;
    }

    return enResult;
}

/**
 *****************************************************************************
 ** \brief FLASH 读等待周期设�?
 **
 ** \param [in]  enWaitCycle  插入FLASH读等待周期数枚举类型
 ** 
 ** \retval Ok          设置成功
 ** \retval Error       设置失败                                
 *****************************************************************************/
en_result_t Flash_WaitCycle(en_flash_waitcycle_t enWaitCycle)
{
    en_result_t enResult = Error;
    
    FLASH_BYPASS();
    M0P_FLASH->CR_f.WAIT = enWaitCycle;
    
    if(enWaitCycle == M0P_FLASH->CR_f.WAIT)
    {
        enResult =  Ok;
    }
    
    return enResult;
}

/**
 *****************************************************************************
 ** \brief FLASH LOCK 设置
 **
 ** \param [in]  enLock  @ref en_flash_lock_t
 ** \param [in]  u32LockValue 32bits，对应bit=0：加锁，对应Sector不允许擦写；对应bit=1：解锁�?
 ** \note  加解锁范围Sector：[enLock*128 + i*4, enLock*128 + i*4+3]
 **        -i: 表示u32LockValue的bit位置�?~31; 
 **        -enLock: 表示枚举编号(FlashLock[n]:n=0~3),并非枚举�?)
 **        例如：enLock = FlashLock1, u32LockValue = 0x00000005,
 **              则FLASH解锁范围为：[Sector128,Sector131]和[Sector136,Sector139]
 ** \retval Ok      解锁成功
 ** \retval Error   解锁失败                                
 *****************************************************************************/
en_result_t Flash_LockSet(en_flash_lock_t enLock, uint32_t u32LockValue)
{
    en_result_t enResult = Error;
    
    FLASH_BYPASS();
    *((&M0P_FLASH->SLOCK0) + enLock) = u32LockValue;
    
    if(u32LockValue == *((&M0P_FLASH->SLOCK0) + enLock))
    {
        return enResult = Ok;
    }
    
    return enResult;
}
//@} // FlashGroup

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
