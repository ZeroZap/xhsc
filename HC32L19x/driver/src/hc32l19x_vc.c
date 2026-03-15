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
 * @file   vc.c
 *
 * @brief  Source file for VC functions
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l19x_vc.h"

/**
 ******************************************************************************
 ** \addtogroup VcGroup
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

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 *****************************************************************************/

/**
******************************************************************************
    ** \brief  配置VC中断触发方式
    **
    ** @param  Channelx : VcChannelx  x=0�?�?
    ** @param  enSel : VcIrqRise、VcIrqFall、VcIrqHigh
    ** \retval �?
    **
******************************************************************************/
void Vc_CfgItType(en_vc_channel_t Channelx, en_vc_irq_sel_t ItType)
{
    stc_vc_vc0_cr_field_t *stcVcnCr;
        switch(Channelx)
        {
            case VcChannel0:
                stcVcnCr = (stc_vc_vc0_cr_field_t*)&M0P_VC->VC0_CR_f;
                break;
            case VcChannel1:
                stcVcnCr = (stc_vc_vc0_cr_field_t*)&M0P_VC->VC1_CR_f;
                break;
            case VcChannel2:
                stcVcnCr = (stc_vc_vc0_cr_field_t*)&M0P_VC->VC2_CR_f;
                break;
            default:
                break;
        }
    switch (ItType)
    {
        case VcIrqNone:
            stcVcnCr->RISING  = 0u;
            stcVcnCr->FALLING = 0u;
            stcVcnCr->LEVEL   = 0u;
            break;
        case VcIrqRise:
            stcVcnCr->RISING  = 1u;
            break;
        case VcIrqFall:
            stcVcnCr->FALLING = 1u;
            break;
        case VcIrqHigh:
            stcVcnCr->LEVEL   = 1u;
            break;
        default:
            break;
    }
}

/**
******************************************************************************
    ** \brief  VC 中断使能与禁�?
    **
    ** @param  Channelx : VcChannelx  x=0�?�?
    ** @param  NewStatus : TRUE �?FALSE
    ** \retval �?
    **
******************************************************************************/
void  Vc_ItCfg(en_vc_channel_t Channelx, boolean_t NewStatus)
{
    switch(Channelx)
    {
        case VcChannel0:
            SetBit((uint32_t)(&(M0P_VC->VC0_CR)), 15, NewStatus);
        break;
        case VcChannel1:
            SetBit((uint32_t)(&(M0P_VC->VC1_CR)), 15, NewStatus);
        break;
        case VcChannel2:
            SetBit((uint32_t)(&(M0P_VC->VC2_CR)), 15, NewStatus);
        break;
        default:
            break;
    }
}

/**
******************************************************************************
    ** \brief  VC 比较结果获取，包含中断标志位和滤波结�?
    **
    ** @param  Result : 所要读取的结果
    ** \retval TRUE �? FALSE
    **
******************************************************************************/
boolean_t Vc_GetItStatus(en_vc_ifr_t Result)
{
    boolean_t bFlag;
    bFlag = GetBit((uint32_t)(&(M0P_VC->IFR)), Result);
    return bFlag;
}

/**
******************************************************************************
    ** \brief  VC 清除中断标志�?
    **
    ** @param  NewStatus : Vc0_Intf、Vc1_Intf、Vc2_Intf
    ** \retval �?
    **
******************************************************************************/
void Vc_ClearItStatus(en_vc_ifr_t NewStatus)
{
    SetBit((uint32_t)(&(M0P_VC->IFR)), NewStatus, 0);
}

/**
******************************************************************************
    ** \brief  VC 配置DAC相关的内�? VC_CR�?VC_REF2P5_SEL VC_DIV_EN VC_DIV
    **
    ** @param  pstcDacCfg : 
    ** \retval Ok �?ErrorInvalidParameter
    **
******************************************************************************/
en_result_t Vc_DacInit(stc_vc_dac_cfg_t *pstcDacCfg)
{
    if (NULL == pstcDacCfg)
    {
        return ErrorInvalidParameter;
    }

    M0P_VC->CR_f.DIV_EN = pstcDacCfg->bDivEn;
    M0P_VC->CR_f.REF2P5_SEL = pstcDacCfg->enDivVref;

    if (pstcDacCfg->u8DivVal < 0x40)
    {
        M0P_VC->CR_f.DIV = pstcDacCfg->u8DivVal;
    }
    else
    {
        return ErrorInvalidParameter;
    }

    return Ok;
}

/**
******************************************************************************
    ** \brief  VC通道初始�?
    **
    ** @param  pstcDacCfg : 
    ** \retval �?
    **
******************************************************************************/
void Vc_Init(stc_vc_channel_cfg_t *pstcChannelCfg)
{
    if (VcChannel0 == pstcChannelCfg->enVcChannel)
    {
        M0P_VC->CR_f.VC0_HYS_SEL = pstcChannelCfg->enVcCmpDly;
        M0P_VC->CR_f.VC0_BIAS_SEL = pstcChannelCfg->enVcBiasCurrent;
        M0P_VC->VC0_CR_f.DEBOUNCE_TIME = pstcChannelCfg->enVcFilterTime;
        M0P_VC->VC0_CR_f.P_SEL = pstcChannelCfg->enVcInPin_P;
        M0P_VC->VC0_CR_f.N_SEL = pstcChannelCfg->enVcInPin_N;
                M0P_VC->VC0_CR_f.FLTEN = pstcChannelCfg->bFlten;
        M0P_VC->VC0_OUT_CFG = 1<<pstcChannelCfg->enVcOutCfg;
    }
    else if (VcChannel1 == pstcChannelCfg->enVcChannel)
    {
        M0P_VC->CR_f.VC1_HYS_SEL = pstcChannelCfg->enVcCmpDly;
        M0P_VC->CR_f.VC1_BIAS_SEL = pstcChannelCfg->enVcBiasCurrent;
        M0P_VC->VC1_CR_f.DEBOUNCE_TIME = pstcChannelCfg->enVcFilterTime;
        M0P_VC->VC1_CR_f.P_SEL = pstcChannelCfg->enVcInPin_P;
        M0P_VC->VC1_CR_f.N_SEL = pstcChannelCfg->enVcInPin_N;
                M0P_VC->VC1_CR_f.FLTEN = pstcChannelCfg->bFlten;
        M0P_VC->VC1_OUT_CFG = 1<<pstcChannelCfg->enVcOutCfg;
    }
    else if(VcChannel2 ==  pstcChannelCfg->enVcChannel)
    {
    M0P_VC->CR_f.VC2_HYS_SEL = pstcChannelCfg->enVcCmpDly;
    M0P_VC->CR_f.VC2_BIAS_SEL = pstcChannelCfg->enVcBiasCurrent;
    M0P_VC->VC2_CR_f.DEBOUNCE_TIME = pstcChannelCfg->enVcFilterTime;
    M0P_VC->VC2_CR_f.P_SEL = pstcChannelCfg->enVcInPin_P;
    M0P_VC->VC2_CR_f.N_SEL = pstcChannelCfg->enVcInPin_N;
            M0P_VC->VC2_CR_f.FLTEN = pstcChannelCfg->bFlten;
    M0P_VC->VC2_OUT_CFG = 1<<pstcChannelCfg->enVcOutCfg;            
    }
    else
    {
        ;
    }
}

/**
******************************************************************************
    ** \brief  VC 通道使能
    **
    ** \param  enChannel :  通道号VcChannel0 VcChannel1 VcChannel2
    ** \param  NewStatus : TRUE FALSE    
    ** \retval NewStatus : TRUE FALSE
    **
******************************************************************************/
void Vc_Cmd(en_vc_channel_t enChannel, boolean_t NewStatus)
{
    switch(enChannel)
    {
        case VcChannel0:
            SetBit((uint32_t)(&(M0P_VC->VC0_CR)), 16, NewStatus);
            break;
        case VcChannel1:
            SetBit((uint32_t)(&(M0P_VC->VC1_CR)), 16, NewStatus);
            break;
        case VcChannel2:
            SetBit((uint32_t)(&(M0P_VC->VC2_CR)), 16, NewStatus);
            break;
        default:
            break;
    }
}

//@} // VcGroup

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

