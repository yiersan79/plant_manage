/****************************************************************************
 (c) Copyright 2013-2014 Freescale Semiconductor, Inc.
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
* FILE NAME     :  NormalDemo_Flash.c                                   *
* DATE          :  April 08, 2014                                       *
*                                                                       *
* AUTHOR        :  FPT Team                                             *
* E-mail        :  r56611@freescale.com                                 *
*                                                                       *
*************************************************************************/

/************************** CHANGES *************************************
0.1.0       04.23.2013      FPT Team        Initial Version
1.0.0       11.25.2013      FPT Team        Optimize Version
1.0.2       08.04.2014      FPT Team        Update to follow SDK convention(MISRA-C)
*************************************************************************/
/* include the header files */
#include "SSD_FTFx.h"
#include "demo_cfg.h"


FLASH_SSD_CONFIG flashSSDConfig =
{
    FTFx_REG_BASE,          /* FTFx control register base */
    PFLASH_BLOCK_BASE,      /* base address of PFlash block */
    PBLOCK_SIZE,            /* size of PFlash block */
    DEFLASH_BLOCK_BASE,     /* base address of DFlash block */
    0x0U,                   /* size of DFlash block */
    EERAM_BLOCK_BASE,       /* base address of EERAM block */
    0x0U,                   /* size of EEE block */
    DEBUGENABLE,            /* background debug mode enable bit */
    NULL_CALLBACK           /* pointer to callback function */
};

#define CALLBACK_SIZE           0x30U
#define LAUNCH_CMD_SIZE         0x80U

uint8_t DataArray[PGM_SIZE_BYTE];
uint8_t buffer[BUFFER_SIZE_BYTE];
uint32_t gCallBackCnt; /* global counter in callback(). */
pFLASHCOMMANDSEQUENCE g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)0xFFFFFFFF;

uint16_t __ram_func[LAUNCH_CMD_SIZE/2]; /* array to copy __Launch_Command func to RAM */
uint16_t __ram_for_callback[CALLBACK_SIZE/2]; /* length of this array depends on total size of the functions need to be copied to RAM*/
void callback(void);

/*********************************************************************
*
*  Function Name    : main
*  Description      : Main function
*
*  Arguments        : void
*  Return Value     : UNIT32
*
**********************************************************************/
void main(void)
{
    uint32_t ret;          /* Return code from each SSD function */
    uint32_t dest;         /* Address of the target location */
    uint32_t size;
    uint8_t  securityStatus;      /* Return protection status */
    uint32_t  protectStatus;      /* Store Protection Status Value of PFLSH or DFLASH or EEPROM */
    uint32_t FailAddr;
    uint16_t number;      /* Number of longword or phrase to be program or verify*/
    uint32_t sum;
    uint32_t temp;
    uint32_t i;
    uint32_t j;

    gCallBackCnt = 0x0U;
    
#if ((defined(X_TWR_KV10Z32)) || defined(KM34Z50M_BACES) || (defined(TWR_KL46Z48M)))
    CACHE_DISABLE
#else
    //CACHE_DISABLE
#endif

    /* Set CallBack to callback function */
    flashSSDConfig.CallBack = (PCALLBACK)RelocateFunction((uint32_t)__ram_for_callback , CALLBACK_SIZE , (uint32_t)callback);     
    g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)RelocateFunction((uint32_t)__ram_func , LAUNCH_CMD_SIZE ,(uint32_t)FlashCommandSequence);     
    
    /**************************************************************************
    *                               FlashInit()                               *
    ***************************************************************************/
    ret = FlashInit(&flashSSDConfig);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    
#ifdef twr_mc56f82748
    PE_low_level_init();
#endif

#if ((!(defined(FTFA_M))) || (defined(BLOCK_COMMANDS)))
    /**************************************************************************
    *                     FlashEraseBlock() and FlashVerifyBlock()            *
    ***************************************************************************/
    /* Erase for each individual Pflash block */
    for (i = 0x1U; i < PBLOCK_NUM; i++)
    {
        dest = flashSSDConfig.PFlashBlockBase + BYTE2WORD(i*flashSSDConfig.PFlashBlockSize/PBLOCK_NUM);
        ret = FlashEraseBlock(&flashSSDConfig, dest, g_FlashLaunchCommand);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }
        /* verification for normal and user margin levels */
        for (j = 0x0U; j < 0x2U; j++)
        {
            ret = FlashVerifyBlock(&flashSSDConfig, dest, j, g_FlashLaunchCommand);
            if (FTFx_OK != ret)
            {
                ErrorTrap(ret);
            }
        }
    }    
#endif

    /**************************************************************************
    *               FlashEraseSector()  and FlashVerifySection()              *
    ***************************************************************************/
    /* Erase several sectors on Pflash*/
    dest = flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize - 0x4U * FTFx_PSECTOR_SIZE);
    while ((dest + BYTE2WORD(FTFx_PSECTOR_SIZE)) < (flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize)))
    {
        size = FTFx_PSECTOR_SIZE;
        ret = FlashEraseSector(&flashSSDConfig, dest, size, g_FlashLaunchCommand);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }

        /* Verify section for several sector of PFLASH */
        number = FTFx_PSECTOR_SIZE / PRD1SEC_ALIGN_SIZE;
        for(i = 0x0U; i < 0x2U; i++)
        {
            ret = FlashVerifySection(&flashSSDConfig, dest, number, i, g_FlashLaunchCommand);
            if (FTFx_OK != ret)
            {
                ErrorTrap(ret);
            }
        }
        dest += BYTE2WORD(size);
    }
    
#if (0x0U != DEBLOCK_SIZE)    
    /* Erase several sectors on Dflash*/
    dest = flashSSDConfig.DFlashBlockBase;
    while ((dest + BYTE2WORD(FTFx_DSECTOR_SIZE)) < (flashSSDConfig.DFlashBlockBase + BYTE2WORD(4*FTFx_DSECTOR_SIZE)))
    {
        size = FTFx_DSECTOR_SIZE;
        ret = FlashEraseSector(&flashSSDConfig, dest, size, g_FlashLaunchCommand);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }
      
            /* Verify section for several sector of DFLASH */
            number = FTFx_DSECTOR_SIZE/DRD1SEC_ALIGN_SIZE;
            for(i = 0x0U; i < 0x2U; i ++)
            {
                ret = FlashVerifySection(&flashSSDConfig, dest, number, i, g_FlashLaunchCommand);
                if (FTFx_OK != ret)
                {
                    ErrorTrap(ret);
                }
            }
        dest += BYTE2WORD(FTFx_DSECTOR_SIZE);

    }    
#endif /* DEBLOCK_SIZE */ 

    /**************************************************************************
    *    FlashProgram() FlashCheckSum and  FlashProgramCheck()        *
    ***************************************************************************/
    /* Initialize source buffer */
    for (i = 0x0U; i < BUFFER_SIZE_BYTE; i++)
    {
        /* Set source buffer */
        buffer[i] = i;
    }
    
    /* Program to the end location of PFLASH */
    size = BUFFER_SIZE_BYTE;
    dest = flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize - (uint32_t)(0x3U * FTFx_PSECTOR_SIZE));

    while ((dest + BYTE2WORD(size)) < (flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize)))
    {
        ret = FlashProgram(&flashSSDConfig, dest, size, \
                                       buffer, g_FlashLaunchCommand);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }
        
        /* Flash CheckSum */
        sum = temp = 0x0U;
        for (i = 0x0U; i < size; i++)
        {
            temp += buffer[i];
        }
        ret = FlashCheckSum(&flashSSDConfig, dest, size, &sum);
        if ((FTFx_OK != ret) || (temp != sum))
        {
            ErrorTrap(ret);
        }
        
        /* Program Check for normal and user margin levels*/
        for (i = 0x1U; i < 0x3U; i ++)
        {
            ret = FlashProgramCheck(&flashSSDConfig, dest, size, buffer, \
                                        &FailAddr, i, g_FlashLaunchCommand);
            if (FTFx_OK != ret)
            {
                ErrorTrap(ret);
            }
        }
        
        dest += BYTE2WORD(BUFFER_SIZE_BYTE);
    }
    
#if (0x0 != DEBLOCK_SIZE)
    /* Program to the DFLASH block*/
    size = BUFFER_SIZE_BYTE;
    dest = flashSSDConfig.DFlashBlockBase;
    while ((dest + BYTE2WORD(size)) < (flashSSDConfig.DFlashBlockBase + BYTE2WORD(0x4U * FTFx_DSECTOR_SIZE)))
    {
        ret = FlashProgram(&flashSSDConfig, dest, size, \
                                       buffer, g_FlashLaunchCommand);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }
        
        /* Flash CheckSum */
        sum = 0x0U;
        ret = FlashCheckSum(&flashSSDConfig, dest, size, &sum);
        if ((FTFx_OK != ret) || (temp != sum))
        {
            ErrorTrap(ret);
        }
        
        /* Program Check for normal and user margin levels*/
        for (i = 0x1U; i < 0x3U; i++)
        {
            ret = FlashProgramCheck(&flashSSDConfig, dest, size, buffer, \
                                        &FailAddr, i, g_FlashLaunchCommand);
            if (FTFx_OK != ret)
            {
                ErrorTrap(ret);
            }
        }
        dest += BYTE2WORD(BUFFER_SIZE_BYTE);
    }
    
    /**************************************************************************
    *                            SetEEEEnable()                               *
    ***************************************************************************/
  
    ret = SetEEEEnable(&flashSSDConfig, RAM_ENABLE ,g_FlashLaunchCommand);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
            
#endif /* DEBLOCK_SIZE */

#ifndef FTFA_M    
    /**************************************************************************
    *                          FlashProgramSection()                          *
    ***************************************************************************/
    /* Write some values to EERAM */
    for (i = 0x0U; i < 0x100U; i += 0x4U)
    {
        WRITE32(flashSSDConfig.EERAMBlockBase + i,0x11223344U);
    }
    /* Erase sector for program section */
    dest = flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize/PBLOCK_NUM - 0x4U * FTFx_PSECTOR_SIZE);
    ret = FlashEraseSector(&flashSSDConfig, dest, FTFx_PSECTOR_SIZE, \
                                     g_FlashLaunchCommand);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }   
 
    /* Program section to the flash block*/
    number = 0x2U;
    ret = FlashProgramSection(&flashSSDConfig, dest, number, g_FlashLaunchCommand);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
#endif
 
#if (0x0U != DEBLOCK_SIZE)    
    /* Erase the first sector of DFlash */
    dest = flashSSDConfig.DFlashBlockBase;
    ret = FlashEraseSector(&flashSSDConfig, dest, FTFx_PSECTOR_SIZE, \
                                     g_FlashLaunchCommand);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    /* Program the 1st sector of DFLASH */
    number = FTFx_DSECTOR_SIZE / (DPGMSEC_ALIGN_SIZE * 0x4U);
    dest = flashSSDConfig.DFlashBlockBase;  
    ret = FlashProgramSection(&flashSSDConfig, dest, number, g_FlashLaunchCommand);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
#endif /* DEBLOCK_SIZE */

    /**************************************************************************
    *                          FlashGetSecurityState()                        *
    ***************************************************************************/
    securityStatus = 0x0U;
    ret = FlashGetSecurityState(&flashSSDConfig, &securityStatus);

    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    /**************************************************************************
     *                          FlashReadResource()                            *
     ***************************************************************************/
     /* Read on P-Flash */
     dest = flashSSDConfig.PFlashBlockBase + PFLASH_IFR; /* Start address of Program Once Field */
     ret = FlashReadResource(&flashSSDConfig, dest, DataArray, 0x0U, g_FlashLaunchCommand);

     if (FTFx_OK != ret)
     {
         ErrorTrap(ret);
     }
     
#if (DEBLOCK_SIZE != 0)
     /* Read on D-Flash */
     dest = flashSSDConfig.DFlashBlockBase + DFLASH_IFR;
     ret = FlashReadResource(&flashSSDConfig, dest, DataArray, 0x0U, g_FlashLaunchCommand);

     if (FTFx_OK != ret)
     {
         ErrorTrap(ret);
     }
  
    /**************************************************************************
    *                            DEFlashPartition()                           *
    ***************************************************************************/
    if (0x0U == flashSSDConfig.EEEBlockSize)
    {
        ret = DEFlashPartition(&flashSSDConfig, \
                                            EEE_DATA_SIZE_CODE, \
                                            DE_PARTITION_CODE, \
                                            g_FlashLaunchCommand);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }  
        
        /* Call FlashInit again to get the new Flash configuration */
        ret = FlashInit(&flashSSDConfig);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }
    }
    
    /**************************************************************************
    *                               EEEWrite()                                *
    ***************************************************************************/
    dest = flashSSDConfig.EERAMBlockBase;
    size = FTFx_WORD_SIZE;
    ret = EEEWrite(&flashSSDConfig, dest, size, buffer);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    
    /**************************************************************************
    *                               EEEWrite()                                *
    ***************************************************************************/
    ret = SetEEEEnable(&flashSSDConfig, EE_ENABLE ,g_FlashLaunchCommand);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
            
    dest = flashSSDConfig.EERAMBlockBase;
    size = FTFx_WORD_SIZE;
    ret = EEEWrite(&flashSSDConfig, dest, size, buffer);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
        
    /**************************************************************************
    *                          EERAMGetProtection()                           *
    ***************************************************************************/
    ret = EERAMGetProtection(&flashSSDConfig, (uint8_t *)&protectStatus);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    
    /**************************************************************************
    *                          EERAMSetProtection()                           *
    ***************************************************************************/
    protectStatus = 0xABU;
    ret = EERAMSetProtection(&flashSSDConfig, (uint8_t)protectStatus);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    
    /* Call EERAMGetProtection to verify the set step */
    ret = EERAMGetProtection(&flashSSDConfig, (uint8_t *)&protectStatus);
    if ((FTFx_OK != ret) || (0xABU != (uint8_t)protectStatus))
    {
        ErrorTrap(ret);
    } 
    
    /**************************************************************************
    *                          DFlashGetProtection()                          *
    ***************************************************************************/
    ret = DFlashGetProtection(&flashSSDConfig, (uint8_t *)&protectStatus);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    
    /**************************************************************************
    *                          DFlashSetProtection()                          *
    ***************************************************************************/
    protectStatus = 0xAAU;
    ret = DFlashSetProtection(&flashSSDConfig, (uint8_t)protectStatus);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    
    /* Call DFlashGetProtection to verify the set step */
    ret = DFlashGetProtection(&flashSSDConfig, (uint8_t *)&protectStatus);
    if ((FTFx_OK != ret) || (0xAAU != (uint8_t)protectStatus))
    {
        ErrorTrap(ret);
    }

#endif /* DEBLOCK_SIZE */
   
    /**************************************************************************
    *                          PFlashGetProtection()                          *
    ***************************************************************************/
    ret = PFlashGetProtection(&flashSSDConfig, &protectStatus);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    
    /**************************************************************************
    *                          PFlashSetProtection()                          *
    ***************************************************************************/
    protectStatus = 0x12ABCDEFU;
    ret = PFlashSetProtection(&flashSSDConfig, protectStatus);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    
    /* Call PFlashGetProtection to verify the set step */
    ret = PFlashGetProtection(&flashSSDConfig, &protectStatus);
    if ((FTFx_OK != ret) || (0x12ABCDEFU != protectStatus))
    {
        ErrorTrap(ret);
    }

    /**************************************************************************/
    
    while(1);
        
}

/*********************************************************************
*
*  Function Name    : ErrorTrap
*  Description      : Gets called when an error occurs.
*  Arguments        : uint32_t
*  Return Value     :
*
*********************************************************************/
void ErrorTrap(uint32_t ret)
{
    while (1)
    {
        ;
    }
}


void callback(void)
{
    /* just increase this variable to observer that this callback() func has been involked */
    gCallBackCnt++;
}
/* end of file */
