/****************************************************************************
 (c) Copyright 2012-2014 Freescale Semiconductor, Inc.
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

*************************************************************************
*                                                                       *
*        Standard Software Flash Driver For FTFx                        *
*                                                                       *
* FILE NAME     :  FTFx_KX_(128_64_32)K_32K_2K_1K_1K.h                  *
* DATE          :  Oct 10, 2014                                       *
* AUTHOR        :  FPT Team                                             *
* E-mail        :  r56611@freescale.com                                 *
*                                                                       *
*************************************************************************/

/************************** CHANGES *************************************
0.0.1       06.19.2012      FPT Team      Initial Version
0.1.0       03.16.2013      FPT Team      Update prototype for FlashReadResource(),
                                          FlashProgramLongword() functions.
                                          Add new macros and remove unnecessary ones.
                                          Add FlashLaunchCommand() prototype.
0.1.1       06.20.2013      FPT Team      Update function prototype of
                                          FlashProgramCheck by removing pFailData
1.0.0       12.25.2013      FPT Team      Modify define macros
1.0.2       08.04.2014      FPT Team      Update to follow SDK convention(MISRA-C)
                                          Delete function prototypes for Flash SSD
                                          Change define ARM_CM4 to ARM_CORTEX_M
1.0.3       10.10.2014      FPT Team      Add to support Doxygen
*************************************************************************/

#ifndef _FTFx_KX_128_64_32K_32K_2K_1K_1K_H_
#define _FTFx_KX_128_64_32K_32K_2K_1K_1K_H_

#include "SSD_FTFx_Common.h"
/*!
 * @addtogroup c90tfs_flash_driver
 * @{
 */

/*!
 * @name C90TFS Flash configuration for KX_128_64_32K_32K_2K_1K_1K
 * @{
 */

/*! @brief Convert from byte address to word(2 bytes) address */
#define BYTE2WORD(x) (x)
/*! @brief Convert from word(2 bytes) address to byte address */
#define WORD2BYTE(x) (x)

/*! @brief Endianness */
#define ENDIANNESS              LITTLE_ENDIAN

/*! @brief cpu core */
#define CPU_CORE                ARM_CORTEX_M

/*! @brief P-Flash sector size */
#define FTFx_PSECTOR_SIZE        0x00000400U     /* 1 KB size */
/*! @brief D-Flash sector size */
#define FTFx_DSECTOR_SIZE        0x00000400U     /* 1 KB size */
/*! @brief FlexNVM memory size */
#define DEBLOCK_SIZE             0x00008000U     /* 32 KB size */

/*! @brief Emulated eeprom size code 0000 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_0000            0xFFFFFFFFU      /* Reserved */
/*! @brief Emulated eeprom size code 0001 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_0001            0xFFFFFFFFU      /* Reserved */
/*! @brief Emulated eeprom size code 0010 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_0010            0xFFFFFFFFU      /* Reserved */
/*! @brief Emulated eeprom size code 0011 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_0011            0x00000800U
/*! @brief Emulated eeprom size code 0100 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_0100            0x00000400U
/*! @brief Emulated eeprom size code 0101 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_0101            0x00000200U
/*! @brief Emulated eeprom size code 0110 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_0110            0x00000100U
/*! @brief Emulated eeprom size code 0111 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_0111            0x00000080U
/*! @brief Emulated eeprom size code 1000 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_1000            0x00000040U
/*! @brief Emulated eeprom size code 1001 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_1001            0x00000020U
/*! @brief Emulated eeprom size code 1010 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_1010            0xFFFFFFFFU      /* Reserved */
/*! @brief Emulated eeprom size code 1011 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_1011            0xFFFFFFFFU      /* Reserved */
/*! @brief Emulated eeprom size code 1100 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_1100            0xFFFFFFFFU      /* Reserved */
/*! @brief Emulated eeprom size code 1101 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_1101            0xFFFFFFFFU      /* Reserved */
/*! @brief Emulated eeprom size code 1110 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_1110            0xFFFFFFFFU      /* Reserved */
/*! @brief Emulated eeprom size code 1111 mapping to emulated eeprom size in bytes (0xFFFFFFFF = reserved) */
#define EEESIZE_1111            0x00000000U      /* Default value */

/* D/E-Flash Partition Code Field Description */
/*! @brief FlexNVM partition code 0000 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_0000             0x00008000U
/*! @brief FlexNVM partition code 0001 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_0001             0x00006000U
/*! @brief FlexNVM partition code 0010 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_0010             0x00004000U
/*! @brief FlexNVM partition code 0011 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_0011             0x00000000U
/*! @brief FlexNVM partition code 0100 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_0100             0xFFFFFFFFU      /* Reserved */
/*! @brief FlexNVM partition code 0101 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_0101             0xFFFFFFFFU      /* Reserved */
/*! @brief FlexNVM partition code 0110 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_0110             0xFFFFFFFFU      /* Reserved */
/*! @brief FlexNVM partition code 0111 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_0111             0xFFFFFFFFU      /* Reserved */
/*! @brief FlexNVM partition code 1000 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_1000             0x00000000U
/*! @brief FlexNVM partition code 1001 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_1001             0x00002000U
/*! @brief FlexNVM partition code 1010 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_1010             0x00004000U
/*! @brief FlexNVM partition code 1011 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_1011             0x00008000U
/*! @brief FlexNVM partition code 1100 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_1100             0xFFFFFFFFU      /* Reserved */
/*! @brief FlexNVM partition code 1101 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_1101             0xFFFFFFFFU      /* Reserved */
/*! @brief FlexNVM partition code 1110 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_1110             0xFFFFFFFFU      /* Reserved */
/*! @brief FlexNVM partition code 1111 mapping to data flash size in bytes (0xFFFFFFFF = reserved) */
#define DEPART_1111             0x00008000U      /* Default value */

/*! @brief Data flash IFR map */
#define DFLASH_IFR_READRESOURCE_ADDRESS   0x8000FCU
/*! @brief Program flash IFR map offset */
#define PFLASH_IFR_OFFSET       0x00000000U
/*! @brief Program flash IFR map size */
#define PFLASH_IFR_SIZE         0x00000100U
/*! @brief Data flash IFR map offset */
#define DFLASH_IFR_OFFSET       0x00000000U
/*! @brief Data flash IFR map size */
#define DFLASH_IFR_SIZE         0x00000100U

/* Size for checking alignment of a function */
/*! @brief P-Flash Erase/Read 1st block command address alignment */
#define PERSBLK_ALIGN_SIZE      FTFx_LONGWORD_SIZE
/*! @brief FlexNVM Erase/Read 1st block command address alignment */
#define DERSBLK_ALIGN_SIZE      FTFx_LONGWORD_SIZE
/*! @brief P-Flash Erase sector command address alignment */
#define PERSSEC_ALIGN_SIZE      FTFx_LONGWORD_SIZE
/*! @brief FlexNVM Erase sector command address alignment */
#define DERSSEC_ALIGN_SIZE      FTFx_LONGWORD_SIZE
/*! @brief P-Flash Program section command address alignment */
#define PPGMSEC_ALIGN_SIZE      FTFx_LONGWORD_SIZE
/*! @brief FlexNVM Program section command address alignment */
#define DPGMSEC_ALIGN_SIZE      FTFx_LONGWORD_SIZE
/*! @brief P-Flash Erase/Read 1st block command address alignment */
#define PRD1BLK_ALIGN_SIZE      FTFx_LONGWORD_SIZE
/*! @brief FlexNVM Erase/Read 1st block command address alignment */
#define DRD1BLK_ALIGN_SIZE      FTFx_LONGWORD_SIZE
/*! @brief P-Flash read 1s section command address alignment */
#define PRD1SEC_ALIGN_SIZE      FTFx_LONGWORD_SIZE
/*! @brief FlexNVM read 1s section command address alignment */
#define DRD1SEC_ALIGN_SIZE      FTFx_LONGWORD_SIZE
/*! @brief Program check command address alignment */
#define PGMCHK_ALIGN_SIZE       FTFx_LONGWORD_SIZE
/*! @brief Read resource command address alignment */
#define RDRSRC_ALIGN_SIZE       FTFx_LONGWORD_SIZE
/*! @brief Program unit size */
#define PGM_SIZE_BYTE           FTFx_LONGWORD_SIZE

/*! @brief Resume wait count used in FlashResume function */
#define RESUME_WAIT_CNT         0x20U
/*@}*/

/*! @}*/
#endif  /* _FTFx_KX_(128_64_32)K_32K_2K_1K_1K_H_ */
