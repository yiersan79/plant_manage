/****************************************************************************
 (c) Copyright 2010-2014 Freescale Semiconductor, Inc.
 ALL RIGHTS RESERVED.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************

*****************************************************************************
*                                                                           *
*        Standard Software Flash Driver For FTFx                            *
*                                                                           *
* FILE NAME     :  SSD_FTFx.h                                               *
* DATE          :  Oct 10, 2014                                           *
*                                                                           *
* AUTHOR        :  FPT Team                                                 *
* E-mail        :  r56611@freescale.com                                     *
*                                                                           *
*****************************************************************************/

/************************** CHANGES *************************************
0.0.1       06.20.2012      FPT Team      Initial Version
0.1.0       03.16.2013      FPT Team      Update to support K20-512, K20-1M, L4K, LOPA,
                                          L2KM and MCF51JG derivatives
                                          Remove EERAMBlockSize field in ssdConfig
0.1.1       06.10.2013      FPT Team      Add to include "user_config.h" file
0.1.2       06.11.2013      FPT Team      Add derivative FTFx_JX_32K_32K_2K_1K_1K
0.1.3       06.20.2013      FPT Team      Add derivative FTFx_KX_256K_0K_4K_2K_0K
                                          Remove derivative FTFx_KX_512K_0K_0K_2K_0K
                                          Change derivative name as below:
                                          - from FTFx_KX_32K_0K_0K_1K_0K to FTFx_KX_32K_0K_2K_1K_0K
                                          - from FTFx_KX_64K_0K_0K_1K_0K to FTFx_KX_64K_0K_2K_1K_0K
                                          - from FTFx_KX_128K_0K_0K_1K_0K to FTFx_KX_128K_0K_2K_1K_0K
                                          Remove compiler definition CW and IAR
                                          Add derivative for LKM family: FTFx_MX_64K_0K_0K_1K_0K
                                          and FTFx_MX_128K_0K_0K_1K_0K
                                          Remove FTFx_FX_256K_32K_2K_1K_1K
                                          Change FTFx_JX_xxx to FTFx_CX_xxx for coldfire core.
1.0.0       12.25.2013      FPT Team      Add derivative FTFx_KX_512K_0K_0K_2K_0K
                                          Add derivative FTFx_KX_256K_0K_0K_2K_0K
1.0.2       08.04.2014      FPT Team      Update to follow SDK convention(MISRA-C)
                                          Add derivative FTFx_KX_128K_0K_0K_2K_0K
1.0.3       10.10.2014      FPT Team      Add to support Doxygen; KV30, K60_2M and Torq Silver
*************************************************************************/
#ifndef _SSD_FTFx_COMMON_H_
#define _SSD_FTFx_COMMON_H_

#include "SSD_Types.h"

/*------------------------- Configuration Macros -----------------------*/
/* Define derivatives with rule: FTFx_AA_BB_CC_DD_EE_FF
AA: MCU type
BB: P-Flash block size
CC: FlexNVM block size
DD: FlexRAM/AccRam size
EE: P-Flash sector size
FF: D-Flash sector size */

/*!
 * @addtogroup c90tfs_flash_driver
 * @{
 */
/*!
 * @name Supported flash configuration
 * @{
 */
/*! @brief  MCU type: Kinetis, P-Flash block size: 256K, FlexNVM block size: 256K, FlexRAM/AccRam size: 4K, P-Flash sector size: 2K, FlexNVM sector size: 2K */
#define FTFx_KX_256K_256K_4K_2K_2K          1          /* Kinetis - K40, K60 ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 512K, FlexNVM block size: 0K, FlexRAM/AccRam size: 4K, P-Flash sector size: 2K, FlexNVM sector size: 0K */
#define FTFx_KX_512K_0K_4K_2K_0K            2          /* Kinetis - K20, K40, K60 ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 512K, FlexNVM block size: 512K, FlexRAM/AccRam size: 16K, P-Flash sector size: 4K, FlexNVM sector size: 4K */
#define FTFx_KX_512K_512K_16K_4K_4K         3          /* Kinetis - ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 1024K, FlexNVM block size: 0K, FlexRAM/AccRam size: 16K, P-Flash sector size: 4K, FlexNVM sector size: 0K */
#define FTFx_KX_1024K_0K_16K_4K_0K          4          /* Kinetis - ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 32K, FlexNVM block size: 0K, FlexRAM/AccRam size: 2K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_KX_32K_0K_2K_1K_0K             5          /* Kinetis - ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 32K, FlexNVM block size: 32K, FlexRAM/AccRam size: 2K, P-Flash sector size: 1K, FlexNVM sector size: 1K */
#define FTFx_KX_32K_32K_2K_1K_1K            6          /* Kinetis - ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 64K, FlexNVM block size: 0K, FlexRAM/AccRam size: 2K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_KX_64K_0K_2K_1K_0K             7          /* Kinetis - ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 64K, FlexNVM block size: 32K, FlexRAM/AccRam size: 2K, P-Flash sector size: 1K, FlexNVM sector size: 1K */
#define FTFx_KX_64K_32K_2K_1K_1K            8          /* Kinetis - ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 128K, FlexNVM block size: 0K, FlexRAM/AccRam size: 2K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_KX_128K_0K_2K_1K_0K            9          /* Kinetis - ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size:128K, FlexNVM block size: 32K, FlexRAM/AccRam size: 2K, P-Flash sector size: 1K, FlexNVM sector size: 1K */
#define FTFx_KX_128K_32K_2K_1K_1K           10         /* Kinetis - ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 64K, FlexNVM block size: 32K, FlexRAM/AccRam size: 2K, P-Flash sector size: 2K, FlexNVM sector size: 1K */
#define FTFx_KX_64K_32K_2K_2K_1K            11         /* Kinetis - ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 128K, FlexNVM block size: 32K, FlexRAM/AccRam size: 2K, P-Flash sector size: 2K, FlexNVM sector size: 1K */
#define FTFx_KX_128K_32K_2K_2K_1K           12         /* Kinetis - ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 256K, FlexNVM block size: 32K, FlexRAM/AccRam size: 2K, P-Flash sector size: 2K, FlexNVM sector size: 1K */
#define FTFx_KX_256K_32K_2K_2K_1K           13         /* Kinetis - ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 1024K, FlexNVM block size: 0K, FlexRAM/AccRam size: 4K, P-Flash sector size: 4K, FlexNVM sector size: 0K */
#define FTFx_KX_1024K_0K_4K_4K_0K           14         /* Kinetis - ARM Cortex M4 core - K20 1M*/
/*! @brief  MCU type: Kinetis, P-Flash block size: 512K, FlexNVM block size: 128K, FlexRAM/AccRam size: 4K, P-Flash sector size: 4K, FlexNVM sector size: 4K */
#define FTFx_KX_512K_128K_4K_4K_4K          15         /* Kinetis - ARM Cortex M4 core - K20 1M - 512*/
/*! @brief  MCU type: Kinetis, P-Flash block size: 256K, FlexNVM block size: 64K, FlexRAM/AccRam size: 4K, P-Flash sector size: 2K, FlexNVM sector size: 2K */
#define FTFx_KX_256K_64K_4K_2K_2K           16         /* Kinetis - ARM Cortex M4 core - K20 256K*/
/*! @brief  MCU type: Kinetis, P-Flash block size: 128K, FlexNVM block size: 64K, FlexRAM/AccRam size: 4K, P-Flash sector size: 2K, FlexNVM sector size: 2K */
#define FTFx_KX_128K_64K_4K_2K_2K           17         /* Kinetis - ARM Cortex M4 core - K20 128K*/
/*! @brief  MCU type: Kinetis, P-Flash block size: 256K, FlexNVM block size: 0K, FlexRAM/AccRam size: 4K, P-Flash sector size: 2K, FlexNVM sector size: 0K */
#define FTFx_KX_256K_0K_4K_2K_0K            18         /* Kinetis - K60 ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 128K, FlexNVM block size: 128K, FlexRAM/AccRam size: 4K, P-Flash sector size: 2K, FlexNVM sector size: 2K */
#define FTFx_KX_128K_128K_4K_2K_2K          19         /* Kinetis -  ARM Cortex M4 core */
/*! @brief  MCU type: Nevis2, P-Flash block size: 256K, FlexNVM block size: 32K, FlexRAM/AccRam size: 2K, P-Flash sector size: 2K, FlexNVM sector size: 1K */
#define FTFx_NX_256K_32K_2K_2K_1K           20         /* Nevis2 - 56800EX 32 bit DSC core */
/*! @brief  MCU type: Nevis2, P-Flash block size: 128K, FlexNVM block size: 32K, FlexRAM/AccRam size: 2K, P-Flash sector size: 2K, FlexNVM sector size: 1K */
#define FTFx_NX_128K_32K_2K_2K_1K           21         /* Nevis2 - 56800EX 32 bit DSC core */
/*! @brief  MCU type: Nevis2, P-Flash block size: 96K, FlexNVM block size: 32K, FlexRAM/AccRam size: 2K, P-Flash sector size: 2K, FlexNVM sector size: 1K */
#define FTFx_NX_96K_32K_2K_2K_1K            22         /* Nevis2 - 56800EX 32 bit DSC core */
/*! @brief  MCU type: Nevis2, P-Flash block size: 64K, FlexNVM block size: 32K, FlexRAM/AccRam size: 2K, P-Flash sector size: 2K, FlexNVM sector size: 1K */
#define FTFx_NX_64K_32K_2K_2K_1K            23         /* Nevis2 - 56800EX 32 bit DSC core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 128K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_LX_128K_0K_0K_1K_0K            24         /* L2K -  ARM Cortex M0 core */ /* L2KM, L4K*/
/*! @brief  MCU type: Kinetis, P-Flash block size: 64K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_LX_64K_0K_0K_1K_0K             25         /* L2K -  ARM Cortex M0 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 32K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_LX_32K_0K_0K_1K_0K             26         /* L2K & L1PT -  ARM Cortex M0 core */ /* LOPA */
/*! @brief  MCU type: Kinetis, P-Flash block size: 16K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_LX_16K_0K_0K_1K_0K             27         /* L1PT -  ARM Cortex M0 core */ /* LOPA */
/*! @brief  MCU type: Kinetis, P-Flash block size: 8K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_LX_8K_0K_0K_1K_0K              28         /* L1PT -  ARM Cortex M0 core */ /* LOPA */
/*! @brief  MCU type: Kinetis, P-Flash block size: 256K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_LX_256K_0K_0K_1K_0K            29         /* L4K -  ARM Cortex M0 core */
/*! @brief  MCU type: Anguilla Silever, P-Flash block size: 64K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_AX_64K_0K_0K_1K_0K             30         /* Anguilla_Silver - 56800EX 32 bit DSC core */
/*! @brief  MCU type: Anguilla Silever, P-Flash block size: 48K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_AX_48K_0K_0K_1K_0K             31         /* Anguilla_Silver - 56800EX 32 bit DSC core */
/*! @brief  MCU type: Anguilla Silever, P-Flash block size: 62K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_AX_32K_0K_0K_1K_0K             32         /* Anguilla_Silver - 56800EX 32 bit DSC core */
/*! @brief  MCU type: Anguilla Silever, P-Flash block size: 16K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_AX_16K_0K_0K_1K_0K             33         /* Anguilla_Silver - 56800EX 32 bit DSC core */
/*! @brief  MCU type: Coldfire, P-Flash block size: 128K, FlexNVM block size: 32K, FlexRAM/AccRam size: 2K, P-Flash sector size: 1K, FlexNVM sector size: 1K */
#define FTFx_CX_128K_32K_2K_1K_1K           34         /* ColdFire core, MCF51JF/JU 128K */
/*! @brief  MCU type: Coldfire, P-Flash block size: 64K, FlexNVM block size: 32K, FlexRAM/AccRam size: 2K, P-Flash sector size: 1K, FlexNVM sector size: 1K */
#define FTFx_CX_64K_32K_2K_1K_1K            35         /* ColdFire core, MCF51JF/JU 64K */
/*! @brief  MCU type: Coldfire, P-Flash block size: 32K, FlexNVM block size: 32K, FlexRAM/AccRam size: 2K, P-Flash sector size: 1K, FlexNVM sector size: 1K */
#define FTFx_CX_32K_32K_2K_1K_1K            36         /* ColdFire core, MCF51JF/JU 32K */
/*! @brief  MCU type: Coldfire, P-Flash block size: 256K, FlexNVM block size: 32K, FlexRAM/AccRam size: 2K, P-Flash sector size: 1K, FlexNVM sector size: 1K */
#define FTFx_CX_256K_32K_2K_1K_1K           37         /* ColdFire core, MCF51JG256 , MCF51FD256*/
/*! @brief  MCU type: Kinetis, P-Flash block size: 64K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_MX_64K_0K_0K_1K_0K             38         /* LKM34 */
/*! @brief  MCU type: Kinetis, P-Flash block size: 128K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 1K, FlexNVM sector size: 0K */
#define FTFx_MX_128K_0K_0K_1K_0K            39         /* LKM34 */
/*! @brief  MCU type: Kinetis, P-Flash block size: 512K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 2K, FlexNVM sector size: 0K */
#define FTFx_KX_512K_0K_0K_2K_0K            40         /* Senna K22FN512 */
/*! @brief  MCU type: Kinetis, P-Flash block size: 256K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 2K, FlexNVM sector size: 0K */
#define FTFx_KX_256K_0K_0K_2K_0K            41         /* Senna K22FN256 */
/*! @brief  MCU type: Kinetis, P-Flash block size: 128K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 2K, FlexNVM sector size: 0K */
#define FTFx_KX_128K_0K_0K_2K_0K            42         /* KV30 MKV30F12810/ K02 MK02FN12810/K22FN128 */
/*! @brief  MCU type: Kinetis, P-Flash block size: 64K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 2K, FlexNVM sector size: 0K */
#define FTFx_KX_64K_0K_0K_2K_0K             43         /* KV30 MKV30F6410/ K02 MK02FN6410 */
/*! @brief  MCU type: Kinetis, P-Flash block size: 256K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 4K, FlexNVM sector size: 0K */
#define FTFx_KX_256K_0K_0K_4K_0K            44         /* Torq Silver KV4F256 and K24s MK24F25612 */
/*! @brief  MCU type: Kinetis, P-Flash block size: 128K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 4K, FlexNVM sector size: 0K */
#define FTFx_KX_128K_0K_0K_4K_0K            45         /* Torq Silver KV4F128 */
/*! @brief  MCU type: Kinetis, P-Flash block size: 64K, FlexNVM block size: 0K, FlexRAM/AccRam size: 0K, P-Flash sector size: 4K, FlexNVM sector size: 0K */
#define FTFx_KX_64K_0K_0K_4K_0K             46         /* Torq Silver KV4F64 */
/*! @brief  MCU type: Kinetis, P-Flash block size: 2048K, FlexNVM block size: 0K, FlexRAM/AccRam size: 4K, P-Flash sector size: 4K, FlexNVM sector size: 0K */
#define FTFx_KX_2048K_0K_4K_4K_0K           47         /* Kinetis - K60_2M ARM Cortex M4 core */
/*! @brief  MCU type: Kinetis, P-Flash block size: 1024K, FlexNVM block size: 256K, FlexRAM/AccRam size: 4K, P-Flash sector size: 4K, FlexNVM sector size: 4K */
#define FTFx_KX_1024K_256K_4K_4K_4K         48         /* Kinetis - ARM Cortex M4 core - K65/K66 */
/*@}*/




/* Endianness */
/*!
 * @name Endianness definition
 * @{
 */
/*! @brief  Big Endian */
#define BIG_ENDIAN                        0
/*! @brief  Little Endian */
#define LITTLE_ENDIAN                     1
/*@}*/
/*!
 * @name CPU core types
 * @{
 */
/*! @brief  ColdFire core */
#define COLDFIRE                          0
/*! @brief  ARM Cortex M core */
#define ARM_CORTEX_M                      1
/*! @brief  DSC_56800EX core */
#define DSC_56800EX                       2
/*@}*/
/*!
 * @name Size macro
 * @{
 */
/*! @brief  Word size */
#define FTFx_WORD_SIZE                    0x0002U     /* 2 bytes */
/*! @brief  Longword size */
#define FTFx_LONGWORD_SIZE                0x0004U     /* 4 bytes */
/*! @brief  Phrase size */
#define FTFx_PHRASE_SIZE                  0x0008U     /* 8 bytes */
/*! @brief  Double-phrase size */
#define FTFx_DPHRASE_SIZE                 0x0010U     /* 16 bytes */

/*@}*/

/*! @}*/
/* Flash security status */
#define FLASH_SECURITY_STATE_KEYEN         0x80U
#define FLASH_SECURITY_STATE_UNSECURED     0x02U

/*!
 * @addtogroup c90tfs_flash_driver
 * @{
 */

/*------------ Return Code Definition for FTFx SSD ---------------------*/
/*!
 * @name Return Code Definition for FTFx SSD
 * @{
 */
/*! @brief  Function executes successfully */
#define FTFx_OK                            0x0000U
/*!@brief  MGSTAT0 bit is set in the FSTAT register
*
* Possible causes:
*
* MGSTAT0 bit in FSTAT register is set. Refer to corresponding command description
* of each API on reference manual to get detail reasons
*
* Solution:
*
* Hardware error
*
*/
#define FTFx_ERR_MGSTAT0                   0x0001U
/*! @brief  Protection violation is set in FSTAT register
*
* Possible causes:
*
* FPVIOL bit in FSTAT register is set. Refer to corresponding command description
* of each API  on reference manual to get detail reasons
*
* Solution:
*
* The  flash location targeted to program/erase operation must be unprotected. Swap
* indicator must not be programed/erased except in Update or Update-Erase state.
*
*/
#define FTFx_ERR_PVIOL                     0x0010U
/*! @brief  Access error is set in the FSTAT register
*
* Possible causes:
*
* ACCERR bit in FSTAT register is set. Refer to corresponding command description
* of each API  on reference manual to get detail reasons.
*
* Solution:
*
* Provide valid input parameters for each API according to specific flash module.
*
*/
#define FTFx_ERR_ACCERR                    0x0020U
/*! @brief  Can not change protection status
*
* Possible causes:
*
* Violate protection transition
*
* Solution:
*
* In NVM normal mode, protection size cannot be decreased. So, only increasing
* protection size is permitted if the device is operating in this mode.
*
*/
#define FTFx_ERR_CHANGEPROT                0x0100U
/*! @brief  FlexRAM is not set for EEPROM use
*
* Possible causes:
*
* User accesses to EEPROM operation but there is no EEPROM backup enabled.
*
* Solution:
*
* Need to enable EEPROM by partitioning FlexNVM to have EEPROM backup and/or
* enable it by SetEEEnable API.
*
*/
#define FTFx_ERR_NOEEE                     0x0200U
/*! @brief  FlexNVM is set for full EEPROM backup
*
* Possible causes:
*
* User accesses to D-Flash operation but there is no D-Flash on FlexNVM.
*
* Solution:
*
* Need to partition FlexNVM to have D-Flash.
*
*/
#define FTFx_ERR_EFLASHONLY                0x0400U
/*! @brief  Programming acceleration RAM is not available
*
* Possible causes:
*
* User invokes flash program section command but FlexRam is being set for EEPROM emulation.
*
* Solution:
*
* Need to set FlexRam as traditional  Ram by SetEEEnable API.
*
*/
#define FTFx_ERR_RAMRDY                    0x0800U
/*! @brief  Address is out of the valid range
*
* Possible causes:
*
* The size or destination provided by user makes start address or end address
* out of valid range.
*
* Solution:
*
* Make sure the destination and (destination + size) within valid address range.
*
*/
#define FTFx_ERR_RANGE                     0x1000U
/*! @brief  Misaligned size
*
* Possible causes:
*
* The size provided by user is misaligned.
*
* Solution:
*
* Size must be an aligned value according to specific constrain of each API.
*
*/
#define FTFx_ERR_SIZE                      0x2000U
/*@}*/

/*!
 * @name Flash security status
 * @{
 */
/*! @brief  Flash currently not in secure state */
#define FLASH_NOT_SECURE                   0x01U
/*! @brief  Flash is secured and backdoor key access enabled */
#define FLASH_SECURE_BACKDOOR_ENABLED      0x02U
/*! @brief  Flash is secured and backdoor key access disabled */
#define FLASH_SECURE_BACKDOOR_DISABLED     0x04U
/*@}*/

/*! @}*/
/*-------------- Read/Write/Set/Clear Operation Macros -----------------*/
#define REG_BIT_SET(address, mask)      (*(vuint8_t*)(address) |= (mask))
#define REG_BIT_CLEAR(address, mask)    (*(vuint8_t*)(address) &= ~(mask))
#define REG_BIT_GET(address, mask)      (*(vuint8_t *)(address) & (uint8_t)(mask))
#define REG_WRITE(address, value)       (*(vuint8_t*)(address) = (value))
#define REG_READ(address)               ((uint8_t)(*(vuint8_t*)(address)))
#define REG_WRITE32(address, value)     (*(vuint32_t*)(address) = (value))
#define REG_READ32(address)             ((uint32_t)(*(vuint32_t*)(address)))

#define WRITE8(address, value)          (*(vuint8_t*)(address) = (value))
#define READ8(address)                  ((uint8_t)(*(vuint8_t*)(address)))
#define SET8(address, value)            (*(vuint8_t*)(address) |= (value))
#define CLEAR8(address, value)          (*(vuint8_t*)(address) &= ~(value))
#define TEST8(address, value)           (*(vuint8_t*)(address) & (value))

#define WRITE16(address, value)         (*(vuint16_t*)(address) = (value))
#define READ16(address)                 ((uint16_t)(*(vuint16_t*)(address)))
#define SET16(address, value)           (*(vuint16_t*)(address) |= (value))
#define CLEAR16(address, value)         (*(vuint16_t*)(address) &= ~(value))
#define TEST16(address, value)          (*(vuint16_t*)(address) & (value))

#define WRITE32(address, value)         (*(vuint32_t*)(address) = (value))
#define READ32(address)                 ((uint32_t)(*(vuint32_t*)(address)))
#define SET32(address, value)           (*(vuint32_t*)(address) |= (value))
#define CLEAR32(address, value)         (*(vuint32_t*)(address) &= ~(value))
#define TEST32(address, value)          (*(vuint32_t*)(address) & (value))

#define GET_BIT_0_7(value)              ((uint8_t)((value) & 0xFFU))
#define GET_BIT_8_15(value)             ((uint8_t)(((value)>>8) & 0xFFU))
#define GET_BIT_16_23(value)            ((uint8_t)(((value)>>16) & 0xFFU))
#define GET_BIT_24_31(value)            ((uint8_t)((value)>>24))

/*--------------------- CallBack function period -----------------------*/
#ifndef FLASH_CALLBACK_CS
#define FLASH_CALLBACK_CS               0x0AU          /* Check Sum */
#endif

/*!
 * @addtogroup c90tfs_flash_driver
 * @{
 */
/*--------------------Null Callback function definition ----------------*/
/*!
 * @name Null Callback function definition
 * @{
 */
/*! @brief  Null callback */
#define NULL_CALLBACK                   ((PCALLBACK)0xFFFFFFFF)
/*! @brief  Null swap callback */
#define NULL_SWAP_CALLBACK              ((PFLASH_SWAP_CALLBACK)0xFFFFFFFF)
/*@}*/

/*! @}*/
#endif /* _SSD_FTFx_COMMON_H_ */

