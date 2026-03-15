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
 * @brief  Source file for AES example
 *
 * @author MADS Team 
 *
 ******************************************************************************/
 
/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "hc32l19x_aes.h"

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
//< 存放待加密的数据
uint32_t u32AESTestData[4] = {0x33221100, 0x77665544, 0xBBAA9988, 0xFFEEDDCC};
//< AEC密匙
uint32_t u32AESTestKey[8]  = {0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C,\
                              0x13121110, 0x17161514, 0x1B1A1918, 0x1F1E1D1C};
//< 存放加密后的数据
uint32_t pu32Ciphertext[4] = {0};
//< 存放解密后的数据
uint32_t pu32Plaintext[4]  = {0};
/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                             
 ******************************************************************************/

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
void App_AesKey128Test (void);
void App_AesKey192Test (void);
void App_AesKey256Test (void);
/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample shows how to use AES to encrypt or decrypt data.
 **
 ******************************************************************************/
int32_t main(void)
{    
    ///< 使用128位密匙加密和解密测试
    App_AesKey128Test();
    
    ///< 使用192位密匙加密和解密测试
    //App_AesKey192Test();
    
    ///< 使用256位密匙加密和解密测试
    //App_AesKey256Test();
    
    while (1) ///< 如果测试失败，则无法运行到此�?
    {
        ;
    }
}

///< 使用128位密匙加密和解密测试
void App_AesKey128Test (void)
{
    stc_aes_cfg_t stcAesCfg;                                  
 
    uint8_t u8Idx;
    
    ///< 打开AES外设门控时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAes, TRUE);
    
    ///<KEY 128
    stcAesCfg.pu32Plaintext = u32AESTestData;       ///< AES 明文指针
    stcAesCfg.pu32Cipher    = pu32Ciphertext;       ///< AES 密文指针
    stcAesCfg.pu32Key       = u32AESTestKey;        ///< AES 密钥指针
    stcAesCfg.enKeyLen      = AesKey128;            ///< AES 密钥长度类型
    ///< AES 加密
    AES_Encrypt(&stcAesCfg);
    ///< AES 解密
    stcAesCfg.pu32Plaintext = pu32Plaintext;
    AES_Decrypt(&stcAesCfg);
  
    for(u8Idx = 0;u8Idx< sizeof(pu32Ciphertext)/sizeof(pu32Ciphertext[0]);u8Idx++)
    {
        ///< 如果解密出来的数据和原数据不匹配，则在此死循环�?
        while(pu32Plaintext[u8Idx] != u32AESTestData[u8Idx]);
    }
}

///< 使用192位密匙加密和解密测试
void App_AesKey192Test (void)
{
    stc_aes_cfg_t stcAesCfg;                                  
 
    uint8_t u8Idx;
    
    ///< 打开AES外设门控时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAes, TRUE);
    
    ///<KEY 192
    stcAesCfg.pu32Plaintext = u32AESTestData;       ///< AES 明文指针
    stcAesCfg.pu32Cipher    = pu32Ciphertext;       ///< AES 密文指针
    stcAesCfg.enKeyLen      = AesKey192;            ///< AES 密钥长度类型
    ///< AES 加密
    AES_Encrypt(&stcAesCfg);
    ///< AES 解密
    stcAesCfg.pu32Plaintext = pu32Plaintext;
    AES_Decrypt(&stcAesCfg);
  
    for(u8Idx = 0;u8Idx< sizeof(pu32Ciphertext)/sizeof(pu32Ciphertext[0]);u8Idx++)
    {
        ///< 如果解密出来的数据和原数据不匹配，则在此死循环�?
        while(pu32Plaintext[u8Idx] != u32AESTestData[u8Idx]);
    }
    
}

///< 使用256位密匙加密和解密测试
void App_AesKey256Test (void)
{
    stc_aes_cfg_t stcAesCfg;                                  
 
    uint8_t u8Idx;
    
    ///< 打开AES外设门控时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAes, TRUE);
      
    ///<KEY 256
    stcAesCfg.pu32Plaintext = u32AESTestData;       ///< AES 明文指针
    stcAesCfg.pu32Cipher    = pu32Ciphertext;       ///< AES 密文指针
    stcAesCfg.enKeyLen      = AesKey256;            ///< AES 密钥长度类型
    ///< AES 加密
    AES_Encrypt(&stcAesCfg);
    ///< AES 解密
    stcAesCfg.pu32Plaintext = pu32Plaintext;
    AES_Decrypt(&stcAesCfg);
  
    for(u8Idx = 0;u8Idx< sizeof(pu32Ciphertext)/sizeof(pu32Ciphertext[0]);u8Idx++)
    {
        ///< 如果解密出来的数据和原数据不匹配，则在此死循环�?
        while(pu32Plaintext[u8Idx] != u32AESTestData[u8Idx]);
    }

}
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/


