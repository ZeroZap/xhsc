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
 * @brief  Source file for RTC example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32l19x_rtc.h"
#include "hc32l19x_gpio.h"

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
__IO uint8_t flag;
__IO uint8_t second, minute, hour, week, day, month, year;
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                             
 ******************************************************************************/
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
******************************************************************************
    ** \brief  RTC中断入口函数
    ** 
  ** @param  �?
    ** \retval �?
    **
******************************************************************************/  
void Rtc_IRQHandler(void)
{
    if(Rtc_GetPridItStatus() == TRUE)
    {
        flag = 1;
        Rtc_ClearPrdfItStatus();             //清除中断标志�?
    }
}

/**
******************************************************************************
    ** \brief  配置RTC
    ** 
  ** @param  �?
    ** \retval �?
    **
******************************************************************************/ 
void RTC_Cfg(void)
{
    stc_rtc_initstruct_t RtcInitStruct;
    RtcInitStruct.rtcAmpm = RtcPm;        //24小时�?
    RtcInitStruct.rtcClksrc = RtcClkRcl;  //内部低速时�?
    RtcInitStruct.rtcPrdsel.rtcPrdsel = RtcPrds;  //周期中断类型PRDS
    RtcInitStruct.rtcPrdsel.rtcPrds = Rtc1S;      //周期中断事件间隔
    RtcInitStruct.rtcTime.u8Second = 0x55;
    RtcInitStruct.rtcTime.u8Minute = 0x01;
    RtcInitStruct.rtcTime.u8Hour   = 0x10;
    RtcInitStruct.rtcTime.u8Day    = 0x17;
    RtcInitStruct.rtcTime.u8DayOfWeek = 0x04;
    RtcInitStruct.rtcTime.u8Month  = 0x04;
    RtcInitStruct.rtcTime.u8Year   = 0x19;
    RtcInitStruct.rtcCompen = RtcCompenEnable;
    RtcInitStruct.rtcCompValue = 0;//补偿值根据实际情况进行补�?
    Rtc_Init(&RtcInitStruct);
    Rtc_AlmIeCmd(TRUE);                  //使能闹钟中断
}


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
    stc_rtc_time_t readtime;
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);//GPIO外设时钟打开
    Sysctrl_SetPeripheralGate(SysctrlPeripheralRtc,TRUE);//RTC模块时钟打开
    Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);
    RTC_Cfg();                                         //配置RTC
    EnableNvic(RTC_IRQn, IrqLevel3, TRUE);                //使能RTC中断向量
    Rtc_Cmd(TRUE);                                        //使能RTC开始计�?
    while (1)
    {
        if(flag == 1)
        {
            flag = 0;
            Rtc_ReadDateTime(&readtime);
            second = readtime.u8Second;
            minute = readtime.u8Minute;
            hour   = readtime.u8Hour;
            day    = readtime.u8Day;
            week   = readtime.u8DayOfWeek;
            month  = readtime.u8Month;
            year   = readtime.u8Year;
        }
    }

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


