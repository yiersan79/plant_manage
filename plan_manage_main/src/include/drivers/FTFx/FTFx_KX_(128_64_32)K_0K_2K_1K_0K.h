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
* FILE NAME     :  FTFx_KX_(128_64_32)K_0K_2K_1K_0K.h                   *
* DATE          :  Oct 10, 2014                                       *
*                                                                       *
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

#ifndef _FTFx_KX_128_64_32K_0K_2K_1K_0K_H_
#define _FTFx_KX_128_64_32K_0K_2K_1K_0K_H_

#include "SSD_FTFx_Common.h"
/*!
 * @addtogroup c90tfs_flash_driver
 * @{
 */

/*!
 * @name C90TFS Flash configuration for KX_128_64_32K_0K_2K_1K_0K
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
#define FTFx_PSECTOR_SIZE       0x00000400U      /* 1 KB size */
/*! @brief FlexNVM memory size */
#define DEBLOCK_SIZE            0x00000000U      /* 0 KB size */

/* Address offset and size of PFlash IFR and DFlash IFR */
/*! @brief Program flash IFR map offset */
#define PFLASH_IFR_OFFSET       0x00000000U
/*! @brief Program flash IFR map size */
#define PFLASH_IFR_SIZE         0x00000100U

/* Size for checking alignment of a section */
/*! @brief P-Flash erase block command address alignment */
#define PERSBLK_ALIGN_SIZE      FTFx_LONGWORD_SIZE 
/*! @brief P-Flash Erase sector command address alignment */

#define PERSSEC_ALIGN_SIZE      FTFx_LONGWORD_SIZE
/*! @brief P-Flash Program secton command address alignment */
#define PPGMSEC_ALIGN_SIZE      FTFx_LONGWORD_SIZE
/*! @brief P-Flash read 1s section command address alignment */
#define PRD1SEC_ALIGN_SIZE      FTFx_LONGWORD_SIZE
/*! @brief P-Flash read 1s block command address alignment */
#define PRD1BLK_ALIGN_SIZE      FTFx_LONGWORD_SIZE
/*! @brief program check command address alignment */
#define PGMCHK_ALIGN_SIZE       FTFx_LONGWORD_SIZE
/*! @brief Read resource command address alignment */
#define RDRSRC_ALIGN_SIZE       FTFx_LONGWORD_SIZE

/*! @brief Program unit size */
#define PGM_SIZE_BYTE           FTFx_LONGWORD_SIZE
/*! @brief Resume wait count used in FlashResume function */
#define RESUME_WAIT_CNT         0x20U
/*@}*/
/*! @}*/
#endif  /* _FTFx_KX_(128_64_32)K_0K_2K_1K_0K_H_ */
