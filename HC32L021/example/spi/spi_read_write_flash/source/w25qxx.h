/**
 *******************************************************************************
 * @file  w25qxx.h
 * @brief This file contains all the functions prototypes of the
 *        w25qxx driver library.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-06       MADS            First version
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

#ifndef __W25QXX_H
#define __W25QXX_H

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "spi.h"

/** @addtogroup BSP
 * @{
 */

/** @addtogroup Components
 * @{
 */

/** @addtogroup W25QXX
 * @{
 */

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/

/** @defgroup W25QXX_Global_Macros W25QXX Global Macros
 * @{
 */

/**
 * @brief  W25QXX Configuration
 */
#define W25QXX_SPI SPI
/* W25Q16 */
#define W25QXX_FLASH_SIZE               (0x0200000u) /*!< 16 MBits => 2MBytes */
#define W25QXX_BLOCK_COUNT              (0x20u)      /*!< 32 Blocks           */
#define W25QXX_BLOCK_SIZE               (0x10000u)   /*!< 64 KBytes           */
#define W25QXX_SECTOR_COUNT             (0x0200u)    /*!< 512 sectors         */
#define W25QXX_SECTOR_SIZE              (0x1000u)    /*!< 4  KBytes           */
#define W25QXX_PAGE_SIZE                (0x100u)     /*!< 8192 pages of 256 bytes */

#define W25QXX_DUMMY_CYCLES_READ        (4u)
#define W25QXX_DUMMY_CYCLES_READ_QUAD   (10u)

#define W25QXX_BULK_ERASE_MAX_TIME      (250000u)
#define W25QXX_SECTOR_ERASE_MAX_TIME    (3000u)
#define W25QXX_SUBSECTOR_ERASE_MAX_TIME (800u)
#define W25QXX_TIMEOUT_VALUE            (0xFFFFFFu)

#define W25QXX_Enable()                 (SPI_MasterNSSOutput(W25QXX_SPI, SPI_NSS_CONFIG_ENABLE))  /* SPI CS置低 */
#define W25QXX_Disable()                (SPI_MasterNSSOutput(W25QXX_SPI, SPI_NSS_CONFIG_DISABLE)) /* SPI CS置高 */

/**
 * @}
 */

/**
 * @defgroup W25QXX_Command W25QXX Command
 * @{
 */
/* Reset Operations */
#define RESET_ENABLE_CMD   (0x66u)
#define RESET_MEMORY_CMD   (0x99u)

#define ENTER_QPI_MODE_CMD (0x38u)
#define EXIT_QPI_MODE_CMD  (0xFFu)

/* Identification Operations */
#define READ_ID_CMD       (0x90u)
#define DUAL_READ_ID_CMD  (0x92u)
#define QUAD_READ_ID_CMD  (0x94u)
#define READ_JEDEC_ID_CMD (0x9Fu)

/* Read Operations */
#define READ_CMD                 (0x03u)
#define FAST_READ_CMD            (0x0Bu)
#define DUAL_OUT_FAST_READ_CMD   (0x3Bu)
#define DUAL_INOUT_FAST_READ_CMD (0xBBu)
#define QUAD_OUT_FAST_READ_CMD   (0x6Bu)
#define QUAD_INOUT_FAST_READ_CMD (0xEBu)

/* Write Operations */
#define WRITE_ENABLE_CMD  (0x06u)
#define WRITE_DISABLE_CMD (0x04u)

/* Register Operations */
#define READ_STATUS_REG1_CMD  (0x05u)
#define READ_STATUS_REG2_CMD  (0x35u)
#define READ_STATUS_REG3_CMD  (0x15u)

#define WRITE_STATUS_REG1_CMD (0x01u)
#define WRITE_STATUS_REG2_CMD (0x31u)
#define WRITE_STATUS_REG3_CMD (0x11u)

/* Program Operations */
#define PAGE_PROG_CMD            (0x02u)
#define QUAD_INPUT_PAGE_PROG_CMD (0x32u)

/* Erase Operations */
#define SECTOR_ERASE_CMD       (0x20u)
#define CHIP_ERASE_CMD         (0xC7u)

#define PROG_ERASE_RESUME_CMD  (0x7Au)
#define PROG_ERASE_SUSPEND_CMD (0x75u)

/* Flag Status Register */
#define W25QXX_FSR_BUSY (0x01u) /*!< busy */
#define W25QXX_FSR_WREN (0x02u) /*!< write enable */
#define W25QXX_FSR_QE   (0x02u) /*!< quad enable */

#define W25QXX_OK       (0x00u)
#define W25QXX_ERROR    (0x01u)
#define W25QXX_BUSY     (0x02u)
#define W25QXX_TIMEOUT  (0x03u)

/**
 * @}
 */

/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/
/*******************************************************************************
  Global function prototypes (definition in C source)
 ******************************************************************************/

/**
 * @addtogroup SYSCTRL_Global_Functions SYSCTRL全局函数定义
 * @{
 */
uint8_t        W25QXX_Init(void);                                                         /* W25QXX初始化 */
static void    W25QXX_Reset(void);                                                        /* W25QXX复位 */
static uint8_t W25QXX_StatusGet(void);                                                    /* W25QXX获取状态 */
uint8_t        W25QXX_WriteEnable(void);                                                  /* W25QXX写使能 */
uint8_t        W25QXX_WriteDisable(void);                                                 /* W25QXX写关闭 */
uint16_t       W25QXX_ReadID(void);                                                       /* W25QXX读取ID */
uint8_t        W25QXX_Read(uint16_t *pu16Data, uint32_t u32ReadAddr, uint32_t u32Size);   /* W25QXX读取数据 */
uint8_t        W25QXX_Write(uint16_t *pu16Data, uint32_t u32WriteAddr, uint32_t u32Size); /* W25QXX写数据 */
uint8_t        W25QXX_BlockErase(uint32_t u32Addr);                                       /* W25QXX擦除块 */
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __W25QXX_H */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
