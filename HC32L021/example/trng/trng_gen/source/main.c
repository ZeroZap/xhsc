/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of TRNG
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-03       MADS            First version
   2025-07-21       MADS            Modify variable name
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2024~2025, Xiaohua Semiconductor Co., Ltd. All rights reserved.
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
#include "ddl.h"
#include "trng.h"
/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
uint32_t au32Data[2] = {0xFFFFFFFFu, 0xFFFFFFFFu};
/*******************************************************************************
 * Local function prototypes ('static')
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
    /* 打开RNG模块时钟门控 */
    SYSCTRL_PeriphClockEnable(PeriphClockTrng);

    /* 上电第一次生成随机数必须使用初始化函数生成并获取随机数 */
    /* 上电第一次随机数生成并获取 */
    TRNG_Init();

    au32Data[0] = TRNG_Data0Get();
    au32Data[1] = TRNG_Data1Get();

    /* 随机数不应该为全0或全1 */
    if ((au32Data[0] != 0u) && (au32Data[1] != 0u) && (au32Data[0] != 0xFFFFFFFFu) && (au32Data[1] != 0xFFFFFFFFu))
    {
        ;
    }
    else
    {
        while (1)
        {
            ;
        }
    }

    /* 如果在使用中有多次生成随机数的需要，则在第一次生产随机数后通过如下流程实现和获取随机数 */
    /* 非上电第一次随机数生成并获取 */
    TRNG_Generate();

    /* 两次生产的随机数不相同 */
    if (au32Data[0] != TRNG_Data0Get())
    {
        if (au32Data[1] != TRNG_Data1Get())
        {
            ;
        }
        else
        {
            while (1)
            {
                ;
            }
        }
    }
    else
    {
        while (1)
        {
            ;
        }
    }

    while (1)
    {
        ;
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
