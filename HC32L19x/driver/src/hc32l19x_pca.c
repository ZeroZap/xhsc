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
 * @file   pca.c
 *
 * @brief  Source file for PCA functions
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l19x_pca.h"

/**
 ******************************************************************************
 ** \addtogroup PCNTGroup
 ******************************************************************************/
//@{

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
 * Local variable definitions ('static')
 ******************************************************************************/

/**
******************************************************************************
    ** \brief  获取中断源的标志�?
    ** @param  IT_Src : 中断源标志位
    ** \retval FALSE 或TRUE
    **
******************************************************************************/
boolean_t Pca_GetItStatus(en_pca_ccficlr_t It_Src)
{
    return (((M0P_PCA->CCON)>>It_Src) & 1) > 0? TRUE : FALSE;
}

/**
******************************************************************************
    ** \brief  清除中断源的标志�?
    ** @param  IT_Src : 中断源标志位
    ** \retval �?
    **
******************************************************************************/
void Pca_ClrItStatus(en_pca_ccficlr_t It_Src)
{
    M0P_PCA->ICLR &= ~(uint32_t)(1<<It_Src);
}

/**
******************************************************************************
    ** \brief  PCA 计数器运行控�?  PCA_CCON CR控制�?
    ** @param  NewStatus : TRUE �?FALSE
    ** \retval �?
    **
******************************************************************************/
void Pca_StartPca(boolean_t NewStatus)
{
    SetBit((uint32_t)(&(M0P_PCA->CCON)), 6, NewStatus);
}

/**
******************************************************************************
    ** \brief  PCA 空闲模式IDLE下，PCA是否停止工作设置
    ** @param  NewStatus : TRUE �?FALSE
    ** \retval �?
    **
******************************************************************************/
void Pca_SetCidl(boolean_t NewStatus)
{
    SetBit((uint32_t)(&(M0P_PCA->CMOD)), 7, NewStatus);
}

/**
******************************************************************************
    ** \brief  PCA 模块4的看门狗使能控制
    ** @param  NewStatus : TRUE �?FALSE
    ** \retval �?
    **
******************************************************************************/
void Pca_Set4Wdte(boolean_t NewStatus)
{
    SetBit((uint32_t)(&(M0P_PCA->CMOD)), 6, NewStatus);
}

/**
******************************************************************************
    ** \brief  PCA 计数器中断控制PCA_CMOD中CFIE，对应的控制中断位PCA_CCON中的CF与PCA_ICLR中的CF
    ** @param  NewStatus : TRUE �?FALSE
    ** \retval �?
    **
******************************************************************************/
void Pca_ConfPcaIt(boolean_t NewStatus)
{
    SetBit((uint32_t)(&(M0P_PCA->CMOD)), 0, NewStatus);
}

/**
******************************************************************************
    ** \brief  PCA 比较捕获中断使能控制  PCA_CCAPMx的CCIE
    ** @param  Modulex : pca_module0~4
    ** @param  NewStatus : TRUE �?FALSE
    ** \retval �?
    **
******************************************************************************/
void Pca_ConfModulexIt(en_pca_module_t Modulex, boolean_t NewStatus)
{
    switch(Modulex)
    {
        case PcaModule0:
            SetBit((uint32_t)(&(M0P_PCA->CCAPM0)), 0, NewStatus);
            break;
        case PcaModule1:
            SetBit((uint32_t)(&(M0P_PCA->CCAPM1)), 0, NewStatus);
            break;        
        case PcaModule2:
            SetBit((uint32_t)(&(M0P_PCA->CCAPM2)), 0, NewStatus);
            break;
        case PcaModule3:
            SetBit((uint32_t)(&(M0P_PCA->CCAPM3)), 0, NewStatus);
            break;
        case PcaModule4:
            SetBit((uint32_t)(&(M0P_PCA->CCAPM4)), 0, NewStatus);
            break;
        default:
            break;
    }
}

/**
******************************************************************************
    ** \brief  PCA 模块0的初始化
    ** @param  InitStruct : PCA初始化配置的结构�?
    ** \retval �?
    **
******************************************************************************/
void Pca_M0Init(stc_pcacfg_t* InitStruct)
{
    M0P_PCA->CMOD_f.CPS = InitStruct->pca_clksrc;
    M0P_PCA->CMOD_f.CIDL = InitStruct->pca_cidl;
    M0P_PCA->CCAPM0_f.ECOM = InitStruct->pca_ecom;
    M0P_PCA->CCAPM0_f.CAPP = InitStruct->pca_capp;
    M0P_PCA->CCAPM0_f.CAPN = InitStruct->pca_capn;
    M0P_PCA->CCAPM0_f.MAT  = InitStruct->pca_mat;
    M0P_PCA->CCAPM0_f.TOG  = InitStruct->pca_tog;
    M0P_PCA->CCAPM0_f.PWM  = InitStruct->pca_pwm;
    M0P_PCA->EPWM_f.EPWM   = InitStruct->pca_epwm;
    if(InitStruct->pca_pwm == PcaPwm8bitEnable)
    {
        M0P_PCA->CCAP0L_f.CCAP0 = InitStruct->pca_ccapl;
        M0P_PCA->CCAP0H_f.CCAP0 = InitStruct->pca_ccaph;
    }
    else
    {
        M0P_PCA->CCAP0_f.CCAP0 = InitStruct->pca_ccap;        
    }
    M0P_PCA->CARR_f.CARR    = InitStruct->pca_carr;
}

/**
******************************************************************************
    ** \brief  PCA 模块1的初始化
    ** @param  InitStruct : PCA初始化配置的结构�?
    ** \retval �?
    **
******************************************************************************/
void Pca_M1Init(stc_pcacfg_t* InitStruct)
{
    M0P_PCA->CMOD_f.CPS = InitStruct->pca_clksrc;
    M0P_PCA->CMOD_f.CIDL = InitStruct->pca_cidl;
    M0P_PCA->CCAPM1_f.ECOM = InitStruct->pca_ecom;
    M0P_PCA->CCAPM1_f.CAPP = InitStruct->pca_capp;
    M0P_PCA->CCAPM1_f.CAPN = InitStruct->pca_capn;
    M0P_PCA->CCAPM1_f.MAT  = InitStruct->pca_mat;
    M0P_PCA->CCAPM1_f.TOG  = InitStruct->pca_tog;
    M0P_PCA->CCAPM1_f.PWM  = InitStruct->pca_pwm;
    M0P_PCA->EPWM_f.EPWM   = InitStruct->pca_epwm;
    if(InitStruct->pca_pwm == PcaPwm8bitEnable)
    {
        M0P_PCA->CCAP1L_f.CCAP1 = InitStruct->pca_ccapl;
        M0P_PCA->CCAP1H_f.CCAP1 = InitStruct->pca_ccaph;
    }
    else
    {
        M0P_PCA->CCAP1_f.CCAP1 = InitStruct->pca_ccap;        
    }
    M0P_PCA->CARR_f.CARR    = InitStruct->pca_carr;
}

/**
******************************************************************************
    ** \brief  PCA 模块2的初始化
    ** @param  InitStruct : PCA初始化配置的结构�?
    ** \retval �?
    **
******************************************************************************/
void Pca_M2Init(stc_pcacfg_t* InitStruct)
{
    M0P_PCA->CMOD_f.CPS = InitStruct->pca_clksrc;
    M0P_PCA->CMOD_f.CIDL = InitStruct->pca_cidl;
    M0P_PCA->CCAPM2_f.ECOM = InitStruct->pca_ecom;
    M0P_PCA->CCAPM2_f.CAPP = InitStruct->pca_capp;
    M0P_PCA->CCAPM2_f.CAPN = InitStruct->pca_capn;
    M0P_PCA->CCAPM2_f.MAT  = InitStruct->pca_mat;
    M0P_PCA->CCAPM2_f.TOG  = InitStruct->pca_tog;
    M0P_PCA->CCAPM2_f.PWM  = InitStruct->pca_pwm;
    M0P_PCA->EPWM_f.EPWM   = InitStruct->pca_epwm;
    if(InitStruct->pca_pwm == PcaPwm8bitEnable)
    {
        M0P_PCA->CCAP2L_f.CCAP2 = InitStruct->pca_ccapl;
        M0P_PCA->CCAP2H_f.CCAP2 = InitStruct->pca_ccaph;
    }
    else
    {
        M0P_PCA->CCAP2_f.CCAP2 = InitStruct->pca_ccap;        
    }
    M0P_PCA->CARR_f.CARR    = InitStruct->pca_carr;
}

/**
******************************************************************************
    ** \brief  PCA 模块3的初始化
    ** @param  InitStruct : PCA初始化配置的结构�?
    ** \retval �?
    **
******************************************************************************/
void Pca_M3Init(stc_pcacfg_t* InitStruct)
{
    M0P_PCA->CMOD_f.CPS = InitStruct->pca_clksrc;
    M0P_PCA->CMOD_f.CIDL = InitStruct->pca_cidl;
    M0P_PCA->CCAPM3_f.ECOM = InitStruct->pca_ecom;
    M0P_PCA->CCAPM3_f.CAPP = InitStruct->pca_capp;
    M0P_PCA->CCAPM3_f.CAPN = InitStruct->pca_capn;
    M0P_PCA->CCAPM3_f.MAT  = InitStruct->pca_mat;
    M0P_PCA->CCAPM3_f.TOG  = InitStruct->pca_tog;
    M0P_PCA->CCAPM3_f.PWM  = InitStruct->pca_pwm;
    M0P_PCA->EPWM_f.EPWM   = InitStruct->pca_epwm;
    if(InitStruct->pca_pwm == PcaPwm8bitEnable)
    {
        M0P_PCA->CCAP3L_f.CCAP3 = InitStruct->pca_ccapl;
        M0P_PCA->CCAP3H_f.CCAP3 = InitStruct->pca_ccaph;
    }
    else
    {
        M0P_PCA->CCAP3_f.CCAP3 = InitStruct->pca_ccap;        
    }
    M0P_PCA->CARR_f.CARR    = InitStruct->pca_carr;
}

/**
******************************************************************************
    ** \brief  PCA 模块4的初始化
    ** @param  InitStruct : PCA初始化配置的结构�?
    ** \retval �?
    **
******************************************************************************/
void Pca_M4Init(stc_pcacfg_t* InitStruct)
{
    M0P_PCA->CMOD_f.CPS = InitStruct->pca_clksrc;
    M0P_PCA->CMOD_f.CIDL = InitStruct->pca_cidl;
    M0P_PCA->CCAPM4_f.ECOM = InitStruct->pca_ecom;
    M0P_PCA->CCAPM4_f.CAPP = InitStruct->pca_capp;
    M0P_PCA->CCAPM4_f.CAPN = InitStruct->pca_capn;
    M0P_PCA->CCAPM4_f.MAT  = InitStruct->pca_mat;
    M0P_PCA->CCAPM4_f.TOG  = InitStruct->pca_tog;
    M0P_PCA->CCAPM4_f.PWM  = InitStruct->pca_pwm;
    M0P_PCA->EPWM_f.EPWM   = InitStruct->pca_epwm;
    if(InitStruct->pca_pwm == PcaPwm8bitEnable)
    {
        M0P_PCA->CCAP4L_f.CCAP4 = InitStruct->pca_ccapl;
        M0P_PCA->CCAP4H_f.CCAP4 = InitStruct->pca_ccaph;
    }
    else
    {
        M0P_PCA->CCAP4_f.CCAP4 = InitStruct->pca_ccap;        
    }
    M0P_PCA->CARR_f.CARR    = InitStruct->pca_carr;
}

/**
******************************************************************************
    ** \brief  PCA 读取CNT寄存器的数�?
    ** @param  �?
    ** \retval CNT的低半字�?
    **
******************************************************************************/
uint16_t Pca_GetCnt(void)
{
    return (uint16_t)(M0P_PCA->CNT);
}

/**
******************************************************************************
    ** \brief  PCA 向CNT寄存器写入数�?
    ** @param  cnt : 所要写入的数�?
    ** \retval �?
    **
******************************************************************************/
void Pca_SetCnt(uint16_t cnt)
{
    if(GetBit((uint32_t)(&(M0P_PCA->CCON)), 6)==TRUE)
    {
        Pca_StartPca(FALSE);
        M0P_PCA->CNT_f.CNT = cnt;        
        Pca_StartPca(TRUE);        
    }
    else
    {
        M0P_PCA->CNT_f.CNT = cnt;
    }

}

/**
******************************************************************************
    ** \brief  PCA 返回指定通道比较高速输出标志寄存器的�?
    ** @param  Modulex : 通道号x=0�?�?�?�?
    ** \retval TRUE �?FALSE
    **
******************************************************************************/
boolean_t Pca_GetOut(en_pca_module_t Modulex)
{
    return GetBit((uint32_t)(&(M0P_PCA->CCAPO)), Modulex);
}

/**
******************************************************************************
    ** \brief  PCA 设置比较捕获16位寄存器CCAPx数�?
    ** @param  Modulex : 通道号x=0�?�?�?�?
    ** @param  Value:  所要设置的�?
    ** \retval �?
    **
******************************************************************************/
void Pca_SetCcap(en_pca_module_t Modulex, uint16_t Value)
{
    switch(Modulex)
    {
        case 0:
            M0P_PCA->CCAP0_f.CCAP0 = Value;
        break;
        case 1:
            M0P_PCA->CCAP1_f.CCAP1 = Value;
            break;
        case 2:
            M0P_PCA->CCAP2_f.CCAP2 = Value;
            break;
        case 3:
            M0P_PCA->CCAP3_f.CCAP3 = Value;
            break;
        case 4:
            M0P_PCA->CCAP4_f.CCAP4 = Value;
            break;
        default:
            break;
    }    
}

/**
******************************************************************************
    ** \brief  PCA 读取比较捕获16位寄存器CCAPx数�?
    ** @param  Modulex : 通道号x=0�?�?�?�?
    ** \retval CCAPx的值，x=0�?�?�?�?
    **
******************************************************************************/
uint16_t Pca_GetCcap(en_pca_module_t Modulex)
{
    uint16_t tmp;
    switch(Modulex)
    {
        case 0:
            tmp = M0P_PCA->CCAP0_f.CCAP0;
        break;
        case 1:
            tmp = M0P_PCA->CCAP1_f.CCAP1;
            break;
        case 2:
            tmp = M0P_PCA->CCAP2_f.CCAP2;
            break;
        case 3:
            tmp = M0P_PCA->CCAP3_f.CCAP3;
            break;
        case 4:
            tmp = M0P_PCA->CCAP4_f.CCAP4;
            break;
        default:
            break;
    }
    return tmp;
}

/**
******************************************************************************
    ** \brief  PCA 设置自动重装载寄存器数�?
    ** @param  �?
    ** \retval �?
    **
******************************************************************************/
void Pca_SetCarr(uint16_t Value)
{
    M0P_PCA->CARR_f.CARR = Value;
}

/**
******************************************************************************
    ** \brief  PCA 获取自动重装载寄存器数�?
    ** @param  �?
    ** \retval �?
    **
******************************************************************************/
uint16_t Pca_GetCarr(void)
{
    return M0P_PCA->CARR_f.CARR;
}

/**
******************************************************************************
    ** \brief  PCA 设置比较捕获寄存器的�?位和�?�?
    ** @param  Modulex : 通道号x=0�?�?�?�?
    ** @param  ValueH : 要写入高8位的数�?
    ** @param  ValueL : 要写入低8位的数�?
    ** \retval �?
    **
******************************************************************************/
void Pca_SetCcapHL(en_pca_module_t Modulex, uint8_t ValueH, uint8_t ValueL)
{
    switch(Modulex)
    {
        case 0:
            M0P_PCA->CCAP0H_f.CCAP0 = ValueH;
            M0P_PCA->CCAP0L_f.CCAP0 = ValueL;
            break;
        case 1:
            M0P_PCA->CCAP1H_f.CCAP1 = ValueH;
            M0P_PCA->CCAP1L_f.CCAP1 = ValueL;
            break;
        case 2:
            M0P_PCA->CCAP2H_f.CCAP2 = ValueH;
            M0P_PCA->CCAP2L_f.CCAP2 = ValueL;
            break;
        case 3:
            M0P_PCA->CCAP3H_f.CCAP3 = ValueH;
            M0P_PCA->CCAP3L_f.CCAP3 = ValueL;
            break;
        case 4:
            M0P_PCA->CCAP4H_f.CCAP4 = ValueH;
            M0P_PCA->CCAP4L_f.CCAP4 = ValueL;
            break;
        default:
            break;
    }
}

/**
******************************************************************************
    ** \brief  PCA 读取比较捕获寄存器的�?位和�?�?
    ** @param  Modulex : 通道号x=0�?�?�?�?
    ** @param  ValueH : CCAPx�?位的数�?
    ** @param  ValueL : CCAPx�?位的数�?
    ** \retval �?
    **
******************************************************************************/
void Pca_GetCcapHL(en_pca_module_t Modulex, uint8_t *ValueH, uint8_t *ValueL)
{
    switch(Modulex)
    {
        case 0:
            *ValueH = M0P_PCA->CCAP0H_f.CCAP0;
            *ValueL = M0P_PCA->CCAP0L_f.CCAP0;
            break;
        case 1:
            *ValueH = M0P_PCA->CCAP1H_f.CCAP1;
            *ValueL = M0P_PCA->CCAP1L_f.CCAP1;
            break;
        case 2:
            *ValueH = M0P_PCA->CCAP2H_f.CCAP2;
            *ValueL = M0P_PCA->CCAP2L_f.CCAP2;
            break;
        case 3:
            *ValueH = M0P_PCA->CCAP3H_f.CCAP3;
            *ValueL = M0P_PCA->CCAP3L_f.CCAP3;
            break;
        case 4:
            *ValueH = M0P_PCA->CCAP4H_f.CCAP4;
            *ValueL = M0P_PCA->CCAP4L_f.CCAP4;
            break;
        default:
            break;
    }    
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/





