/*****************************************************************************
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
******************************************************************************

******************************************************************************
*                                                                            *
*        Standard Software Flash Driver For FTFx                             *
*                                                                            *
* FILE NAME     :  FlashInit.c                                               *
* DATE          :  Oct 10, 2014                                              *
*                                                                            *
* AUTHOR        :  FPT Team                                                  *
* E-mail        :  r56611@freescale.com                                      *
*                                                                            *
******************************************************************************/

/************************** CHANGES ******************************************
0.0.1       06.09.2010      FPT Team      Initial Version
1.0.0       12.25.2013      FPT Team      Optimize code
1.0.2       08.04.2014      FPT Team      Update to follow SDK convention(MISRA-C)
                                          Add uint32_t temp variable
1.0.3       10.10.2014      FPT Team      Add compile switching for C90TFS_ENABLE_DEBUG
******************************************************************************/
/* include the header files */
#include "SSD_FTFx.h"

/************************************************************************
*
*  Function Name    : FlashInit.c
*  Description      : Initialize the Flash memory
*  Arguments        : PFLASH_SSD_CONFIG
*  Return Value     : uint32_t
*
*************************************************************************/

/* Enable size optimization */
#if(ARM_CORTEX_M != CPU_CORE)
#pragma optimize_for_size on
#pragma optimization_level 4
#endif /* End of CPU_CORE */

uint32_t SIZE_OPTIMIZATION FlashInit (PFLASH_SSD_CONFIG pSSDConfig)
{
#if (DEBLOCK_SIZE != 0x0U)
    uint8_t  EEEDataSetSize;    /* store EEE Data Set Size */
    uint8_t  DEPartitionCode;    /* store D/E-Flash Partition Code */
    uint32_t temp;               /* temporary variable */

    /* clear RDCOLERR & ACCERR & FPVIOL flag in flash status register. Write 1 to clear*/
    temp = pSSDConfig->ftfxRegBase + FTFx_SSD_FSTAT_OFFSET;
    REG_WRITE(temp, FTFx_SSD_FSTAT_ERROR_BITS);

    /* Write Command Code to FCCOB0 */
    temp = pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB0_OFFSET;
    REG_WRITE(temp, FTFx_READ_RESOURCE);

    /* Write address to FCCOB1/2/3 */
    temp = pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB1_OFFSET;
    REG_WRITE(temp, GET_BIT_16_23(DFLASH_IFR_READRESOURCE_ADDRESS));

    temp = pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB2_OFFSET;
    REG_WRITE(temp, GET_BIT_8_15(DFLASH_IFR_READRESOURCE_ADDRESS));

    temp = pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB3_OFFSET;
    REG_WRITE(temp, GET_BIT_0_7(DFLASH_IFR_READRESOURCE_ADDRESS));

    /* Write Resource Select Code of 0 to FCCOB8 to select IFR. Without this, */
    /* an access error may occur if the register contains data from a previous command. */
    /* for FTFE module, resource code is FCCOB4. For others, recource code is FCCOB8 */
    temp = pSSDConfig->ftfxRegBase + RSRC_CODE_OFSSET;
    REG_WRITE(temp, 0x0U);

    /* clear CCIF bit */
    temp = pSSDConfig->ftfxRegBase + FTFx_SSD_FSTAT_OFFSET;
    REG_BIT_SET(temp, FTFx_SSD_FSTAT_CCIF);

    /* check CCIF bit */
    temp = pSSDConfig->ftfxRegBase + FTFx_SSD_FSTAT_OFFSET;
    while((REG_BIT_GET(temp, FTFx_SSD_FSTAT_CCIF)) == 0x0U)
    {
        /* wait till CCIF bit is set */
    }
    /* read out EEdata set size and DEpartition code from FCCOBA, FCCOBB for FTFE module, from FCCOB6 and FCCOB7 for others */
    #ifdef FTFE_M
        temp = pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOBA_OFFSET;
        EEEDataSetSize = REG_READ(temp);

        temp = pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOBB_OFFSET;
        DEPartitionCode = REG_READ(temp);
    #else
        temp = pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB6_OFFSET;
        EEEDataSetSize = REG_READ(temp);

        temp = pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB7_OFFSET;
        DEPartitionCode = REG_READ(temp);
    #endif
    DEPartitionCode = DEPartitionCode & 0x0FU;
    EEEDataSetSize = EEEDataSetSize & 0x0FU;
        /* Calculate D-Flash size and EEE size */
    if       (0x0U == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_0000;}
    else if (0x01U == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_0001;}
    else if (0x02U == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_0010;}
    else if (0x03U == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_0011;}
    else if (0x04U == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_0100;}
    else if (0x05U == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_0101;}
    else if (0x06U == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_0110;}
    else if (0x07U == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_0111;}
    else if (0x08U == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_1000;}
    else if (0x09U == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_1001;}
    else if (0x0AU == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_1010;}
    else if (0x0BU == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_1011;}
    else if (0x0CU == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_1100;}
    else if (0x0DU == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_1101;}
    else if (0x0EU == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_1110;}
    else if (0x0FU == DEPartitionCode) {pSSDConfig->DFlashBlockSize = DEPART_1111;}

    if       (0x0U == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_0000;}
    else if (0x01U == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_0001;}
    else if (0x02U == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_0010;}
    else if (0x03U == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_0011;}
    else if (0x04U == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_0100;}
    else if (0x05U == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_0101;}
    else if (0x06U == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_0110;}
    else if (0x07U == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_0111;}
    else if (0x08U == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_1000;}
    else if (0x09U == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_1001;}
    else if (0x0AU == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_1010;}
    else if (0x0BU == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_1011;}
    else if (0x0CU == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_1100;}
    else if (0x0DU == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_1101;}
    else if (0x0EU == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_1110;}
    else if (0x0FU == EEEDataSetSize) {pSSDConfig->EEEBlockSize = EEESIZE_1111;}

#else /* DEBLOCK_SIZE == 0 */
    /* If size of D/E-Flash = 0 */
    pSSDConfig->DFlashBlockSize = 0x0U;
    pSSDConfig->EEEBlockSize = 0x0U;
#endif /* end of DEBLOCK_SIZE */
#if C90TFS_ENABLE_DEBUG
    /* Enter Debug state if enabled */
    if (TRUE == (pSSDConfig->DebugEnable))
    {
        ENTER_DEBUG_MODE;
    }
#endif

    return(FTFx_OK);
}
/* End of file */
