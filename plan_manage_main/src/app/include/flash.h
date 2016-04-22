/****************************************************************************
 Copyright (c) 2013-2014 Freescale Semiconductor, Inc.
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
*        Standard Software Flash Driver For FTFL                        *
*                                                                       *
* FILE NAME     :  demo_cfg.h                                           *
* DATE          :  April 08, 2014                                         *
*                                                                       *
* AUTHOR        :  FPT Team                                             *
* E-mail        :  r56611@freescale.com                                 *
*                                                                       *
*************************************************************************/

/************************** CHANGES *************************************
0.1.0       04.23.2013      FPT Team        Initial Version
1.0.0       12.25.2013      FPT Team        Finalize to version 1.0.0
1.0.2       08.04.2014      FPT Team        Update to follow SDK convention(MISRA-C)
*************************************************************************/

#ifndef FLASH_H
#define FLASH_H

#include "common.h"

#define BUFFER_SIZE_BYTE          0x100U

#define EE_ENABLE                 0x00U
#define RAM_ENABLE                0xFFU
#define DEBUGENABLE               0x00U

#define PSECTOR_SIZE              0x00000400U  /* 1 KB size */
#define DSECTOR_SIZE              0x00000000U 

/* FTFL module base */
#define FTFx_REG_BASE             0x40020000U         
#define PFLASH_BLOCK_BASE         0x00000000U
#define DEFLASH_BLOCK_BASE        0xFFFFFFFFU 
#define EERAM_BLOCK_BASE          0xFFFFFFFFU 

#define PBLOCK_SIZE               0x00020000U      /* 128KB size */
#define EERAM_BLOCK_SIZE          0x00000000U     

#define PBLOCK_NUM                1 /* number of individual Pflash block */

/* destination to program security key back to flash location */
#define SECURITY_LOCATION         0x40CU
#define BACKDOOR_KEY_LOCATION     0x400U

#define PFLASH_IFR                0xC0U /*Program flash IFR map*/

#define CC_RDCOL_ISR_NUM          21

//#define CACHE_DISABLE
#define CACHE_DISABLE             MCM_PLACR |= MCM_PLACR_DFCDA_MASK;

void ErrorTrap(uint32_t returnCode);


#endif /* FLASH_H */