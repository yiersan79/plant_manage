

#include "SSD_FTFx.h"

#include "flash.h"



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

#define LAUNCH_CMD_SIZE         0x80U

uint8_t buffer[BUFFER_SIZE_BYTE];


pFLASHCOMMANDSEQUENCE g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)0xFFFFFFFF;

uint16_t __ram_func[LAUNCH_CMD_SIZE/2]; /* array to copy __Launch_Command func to RAM */


void main(void)
{
    uint32_t ret;          /* Return code from each SSD function */
    uint32_t dest;         /* Address of the target location */
    uint32_t size;
    uint32_t FailAddr;
    uint16_t number;      /* Number of longword or phrase to be program or verify*/
    uint32_t sum;
    uint32_t temp;
    uint32_t i;
    
    
    g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)RelocateFunction((uint32_t)__ram_func , LAUNCH_CMD_SIZE ,(uint32_t)FlashCommandSequence);     

    
    /**************************************************************************
    *                               FlashInit()                               *
    ***************************************************************************/
    ret = FlashInit(&flashSSDConfig);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    
    
    /*
     * ³·Ïú±£»¤
     */
    //ret = PFlashSetProtection(&flashSSDConfig, 1);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    
    
    /**************************************************************************
    *               FlashEraseSector()  and FlashVerifySection()              *
    ***************************************************************************/
    /* Erase several sectors on Pflash*/
    dest = flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize - 0x3U * FTFx_PSECTOR_SIZE);
    while ((dest + BYTE2WORD(FTFx_PSECTOR_SIZE)) <= (flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize)))
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
    
    /**************************************************************************
    *    FlashProgram() FlashCheckSum and  FlashProgramCheck()        *
    ***************************************************************************/
    /* Initialize source buffer */
    for (i = 0x0U; i < BUFFER_SIZE_BYTE; i++)
    {
        /* Set source buffer */
        buffer[i] = 0x77;
    }
    
    /* Program to the end location of PFLASH */
    size = BUFFER_SIZE_BYTE;
    dest = flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize - (uint32_t)(0x1U * FTFx_PSECTOR_SIZE));

    while ((dest + BYTE2WORD(size)) <= (flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize)))
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