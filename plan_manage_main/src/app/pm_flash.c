/*
 * pm_flash.c - flash读写模块
 */

#include "SSD_FTFx.h"

#include "pm_flash_cfg.h"

#define USED_SECTOR_NUM     0x01U


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

pFLASHCOMMANDSEQUENCE g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)0xFFFFFFFF;

uint16_t __ram_func[LAUNCH_CMD_SIZE/2]; /* array to copy __Launch_Command func to RAM */

void pm_flash_init(void)
{
    uint32_t ret;

    g_FlashLaunchCommand = 
        (pFLASHCOMMANDSEQUENCE)RelocateFunction((uint32_t)__ram_func ,
                LAUNCH_CMD_SIZE ,(uint32_t)FlashCommandSequence);
    ret = FlashInit(&flashSSDConfig);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    return;
}

/*
 * flash_write() - 将数据写入flash
 * @saddr: 要写入的数据的起始地址
 * @size: 写入数据的字节数，字节数必须要以4对齐！！！
 *
 * 这个函数的执行过程包括擦除和写入，默认的写入范围为flash的最后一个扇区，大小为
 * 1KB，如果要改变大小，改参数就可以了，注意不要覆盖了code，具体可看map文件。
 * 擦除是一次性擦除全部选择的扇区范围，所以在写入的时候要注意保存先前写入的数据
 * flash读取函数的起始地址等于flash写入函数的写入起始地址
 */
void flash_write(uint8_t *saddr, uint16_t nbyte)
{
    uint32_t ret;
    uint32_t dest;
    uint32_t size;
    uint16_t number;
    uint32_t temp;
    
    /*
     * 擦除扇区
     */
    dest = flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize
            - USED_SECTOR_NUM * FTFx_PSECTOR_SIZE);
    while ((dest + BYTE2WORD(FTFx_PSECTOR_SIZE)) <= (flashSSDConfig.PFlashBlockBase
                + BYTE2WORD(flashSSDConfig.PFlashBlockSize)))
    {
        size = FTFx_PSECTOR_SIZE;
        ret = FlashEraseSector(&flashSSDConfig, dest, size, g_FlashLaunchCommand);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }

        /* 检验 */
        number = FTFx_PSECTOR_SIZE / PRD1SEC_ALIGN_SIZE;
        for(uint8_t i = 0x0U; i < 0x2U; i++)
        {
            ret = FlashVerifySection(&flashSSDConfig, dest, number, i, g_FlashLaunchCommand);
            if (FTFx_OK != ret)
            {
                ErrorTrap(ret);
            }
        }
        dest += BYTE2WORD(size);
    }

    /*
     * 写入数据
     */
    dest = flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize
            - (uint32_t)(USED_SECTOR_NUM * FTFx_PSECTOR_SIZE));

    if ((dest + BYTE2WORD(size)) <= (flashSSDConfig.PFlashBlockBase
                + BYTE2WORD(flashSSDConfig.PFlashBlockSize)))
    {
        ret = FlashProgram(&flashSSDConfig, dest, nbyte, \
                                       saddr, g_FlashLaunchCommand);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }
        
        /* 校验和检验 */
        uint32_t sum = temp = 0x0U;
        for (uint16_t i = 0x0U; i < nbyte; i++)
        {
            temp += saddr[i];
        }
        ret = FlashCheckSum(&flashSSDConfig, dest, nbyte, &sum);
        if ((FTFx_OK != ret) || (temp != sum))
        {
            ErrorTrap(ret);
        }
        
        /* 正确测试 */
        uint32_t FailAddr;
        for (uint8_t i = 0x1U; i < 0x3U; i ++)
        {
            ret = FlashProgramCheck(&flashSSDConfig, dest, nbyte, saddr, \
                                        &FailAddr, i, g_FlashLaunchCommand);
            if (FTFx_OK != ret)
            {
                ErrorTrap(ret);
            }
        }
    }
    return;
}

/*
 * flash_read() - 读取flash中写入的数据
 * @daddr: 读取数据要保存的地址
 * @nbyte: 要读取的字节数
 *
 * flash读取函数的起始地址等于flash写入函数的写入起始地址
 */
void flash_read(uint8_t *daddr, uint16_t nbyte)
{
    uint8_t *saddr = (uint8_t *)(flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize
            - (uint32_t)(USED_SECTOR_NUM * FTFx_PSECTOR_SIZE)));

    for (uint16_t i = 0; i < nbyte; i++)
    {
        daddr[i] = saddr[i];
    }
    return;
}

/*
 * ErrorTrap() - flash操作错误处理
 */
void ErrorTrap(uint32_t ret)
{
    while (1)
    {
        printf("flash操作错误，返回码为 %d\n", ret);
    }
    return;
}
