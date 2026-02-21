/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of GPIO
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-02       MADS            First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2024, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "gpio.h"
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    STK_LedConfig(); /* LED端口配置 */

    STK_UserKeyConfig(); /* USER KEY端口配置 */

    while (1)
    {
        if (STK_USER_KEY_PRESSED()) /* 检测USER KEY是否按下 */
        {
            STK_LED_ON(); /* LED点亮 */
            DDL_Delay1ms(1000);

            STK_LED_OFF(); /* LED关闭 */
            DDL_Delay1ms(1000);
        }
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
