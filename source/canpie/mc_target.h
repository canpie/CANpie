//============================================================================//
// File:          mc_target.h                                                 //
// Description:   MCU target definitions                                      //
// Author:        Uwe Koppe                                                   //
// e-mail:        koppe@microcontrol.net                                      //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// Junkersring 23                                                             //
// 53844 Troisdorf                                                            //
// Germany                                                                    //
// Tel: +49-2241-25659-0                                                      //
// Fax: +49-2241-25659-11                                                     //
//                                                                            //
// The copyright to the computer program(s) herein is the property of         //
// MicroControl GmbH & Co. KG, Germany. The program(s) may be used            //
// and/or copied only with the written permission of MicroControl GmbH &      //
// Co. KG or in accordance with the terms and conditions stipulated in        //
// the agreement/contract under which the program(s) have been supplied.      //
//----------------------------------------------------------------------------//
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 11.04.2014  Initial version                                                //
//                                                                            //
//============================================================================//


//------------------------------------------------------------------------------
// SVN  $Date: 2016-09-20 11:20:15 +0200 (Tue, 20. Sep 2016) $
// SVN  $Rev: 7721 $ --- $Author: koppe $
//------------------------------------------------------------------------------


#ifndef  MC_TARGET_H_
#define  MC_TARGET_H_


//-----------------------------------------------------------------------------
/*!
** \file    mc_target.h
** \brief   MCL - MCU architecture definitions
**
** This file does the setup for the selected target MCU. The following
** symbols are defined here:
** \li  MC_MCU_SIZE_FLASH
** \li  MC_MCU_SIZE_RAM
**
** <p>
**
** \def MC_MCU_SIZE_FLASH
** The symbol \c MC_MCU_SIZE_FLASH defines the size of the internal flash
** in bytes.
**
** \def MC_MCU_SIZE_RAM
** The symbol \c MC_MCU_SIZE_RAM defines the size of the internal flash
** in bytes.
**
*/

/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "mc_target_defs.h"


/*----------------------------------------------------------------------------*\
** Definitions & Enumerations                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

//#define MC_TARGET MC_MCU_MC9S12XDP512

//-------------------------------------------------------------------
// The symbol MC_TARGET defines the microcontroller
//
#ifndef MC_TARGET
#error  Target (Symbol MC_TARGET) is not defined!
#endif


/*----------------------------------------------------------------------------*\
** Structures                                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

//----------------------------------------------------------------------------//
// Generic MCU                                                                //
//----------------------------------------------------------------------------//
#if (MC_TARGET == MC_MCU_GENERIC)

#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00008000)

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_GENERIC

#endif

//----------------------------------------------------------------------------//
// ATMEL AT90CAN family                                                       //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_AT90CAN)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_AT90CAN

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 1
#endif



//----------------------------------------------------------------------------//
// ATMEL AT32UC3C family                                                      //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_AT32UC3C)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_AT32UC3C

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if (MC_TARGET == MC_MCU_AT32UC3C064C)
//---------------------------------------------------------
// 64 kByte Flash, 16 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00004000)
#endif

#if (MC_TARGET == MC_MCU_AT32UC3C0128C)
//---------------------------------------------------------
// 128 kByte Flash, 32 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00008000)
#endif

#if (MC_TARGET == MC_MCU_AT32UC3C0256C)
//---------------------------------------------------------
// 256 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#if (MC_TARGET == MC_MCU_AT32UC3C0512C)
//---------------------------------------------------------
// use processor include file from AVR
//
#include <avr32/uc3c0512c.h>

//---------------------------------------------------------
// 512 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#endif


//----------------------------------------------------------------------------//
// Freescale MC9S12X family                                                   //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_MC9S12X)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_MC9S12X

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      CP_CC_XGATE
#define CP_CHANNEL_MAX                 4

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if (MC_TARGET == MC_MCU_MC9S12XDP512)
//---------------------------------------------------------
// 64 kByte Flash, 16 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00004000)
#endif

#endif


//----------------------------------------------------------------------------//
// Fujitsu (Cypress) MC_FAM_MB9BF family                                      //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_MB9BF52X)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_MB9BF52X

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 1


//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if (MC_TARGET == MC_MCU_MB9BF524K)

//---------------------------------------------------------
// include target dependent header files
//
#ifndef  _MB9B520K_H_
#include "mb9b520k.h"
#endif


//---------------------------------------------------------
// 256 kByte Flash, 32 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00008000)
#endif

#endif


//----------------------------------------------------------------------------//
// Infineon TC179x family                                                     //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_TC179X)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_TC179X

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 4

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if (MC_TARGET == MC_MCU_TC1798)

//---------------------------------------------------------
// include target dependent header files
//
#include "TC1798.h"

//---------------------------------------------------------
// 4 MByte Flash, 288 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00400000)
#define MC_MCU_SIZE_RAM                (0x00048000)
#endif

#endif


//----------------------------------------------------------------------------//
// Infineon XE166 family                                                      //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_XE166)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_XE166

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY

//---------------------------------------------------------
// include target dependent header files
//
#include "XE162FM.H"

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//

#if (MC_TARGET == MC_MCU_XE162FM)
//---------------------------------------------------------
// 576 kByte Flash, 32 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00090000)
#define MC_MCU_SIZE_RAM                (0x00008000)
#endif


#endif

//----------------------------------------------------------------------------//
// Infineon XMC450x family                                                    //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_XMC450X)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_XMC450X

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY


//---------------------------------------------------------
// include target dependent header files
//
#include "xmc_device.h"
#include "XMC4500.h"

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//

#if (MC_TARGET == MC_MCU_XMC4500_1024)
//---------------------------------------------------------
// 1 MByte Flash, 160 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00100000)
#define MC_MCU_SIZE_RAM                (0x00028000)
#endif


#if ((MC_TARGET == MC_MCU_XMC4500_768) || (MC_TARGET == MC_MCU_XMC4502_768))
//---------------------------------------------------------
// 768 kByte Flash, 160 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x000C0000)
#define MC_MCU_SIZE_RAM                (0x00028000)
#endif


#endif


//----------------------------------------------------------------------------//
// Microchip PIC18F family                                                    //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_PIC18F)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_PIC18F

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_FAM_PIC18F
#define CP_CHANNEL_MAX                 1

#endif

//----------------------------------------------------------------------------//
// Microchip dsPIC33F family                                                  //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_DSPIC33F)

//---------------------------------------------------------
// include target dependent header files
//
#include <p33Fxxxx.h>

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_DSPIC33F

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_FAM_DSPIC33F
#define CP_CHANNEL_MAX                 2

#endif


//----------------------------------------------------------------------------//
// Nuvoton NUC140 family                                                      //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_NUC140)

//---------------------------------------------------------
// include target dependent header files
//
#include "NUC1xx.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_NUC140

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 1

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if ((MC_TARGET == MC_MCU_NUC140LC1CN) || (MC_TARGET == MC_MCU_NUC140RC1CN))
//---------------------------------------------------------
// 32 kByte Flash, 4 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00008000)
#define MC_MCU_SIZE_RAM                (0x00001000)
#endif

#if ((MC_TARGET == MC_MCU_NUC140LD2CN) || (MC_TARGET == MC_MCU_NUC140RD2CN))
//---------------------------------------------------------
// 64 kByte Flash, 8 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00002000)
#endif

#if ((MC_TARGET == MC_MCU_NUC140LE3CN) || (MC_TARGET == MC_MCU_NUC140RE3CN))
//---------------------------------------------------------
// 128 kByte Flash, 16 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00004000)
#endif

#if (MC_TARGET == MC_MCU_NUC140VE3CN)
//---------------------------------------------------------
// 128 kByte Flash, 16 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00004000)
#endif

#endif


//----------------------------------------------------------------------------//
// Nuvoton NUC240 family                                                      //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_NUC240)

//---------------------------------------------------------
// include target dependent header files
//
#include "NUC230_240.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_NUC240

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 1

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if ((MC_TARGET == MC_MCU_NUC240LC2AE) || (MC_TARGET == MC_MCU_NUC240SC2AE))
//---------------------------------------------------------
// 32 kByte Flash, 8 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00008000)
#define MC_MCU_SIZE_RAM                (0x00002000)
#endif

#if ((MC_TARGET == MC_MCU_NUC240LD2AE) || (MC_TARGET == MC_MCU_NUC240SD2AE))
//---------------------------------------------------------
// 64 kByte Flash, 8 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00002000)
#endif

#if ((MC_TARGET == MC_MCU_NUC240LE3AE) || (MC_TARGET == MC_MCU_NUC240SE3AE))
//---------------------------------------------------------
// 128 kByte Flash, 16 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00004000)
#endif

#if (MC_TARGET == MC_MCU_NUC240VE3AE)
//---------------------------------------------------------
// 128 kByte Flash, 16 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00004000)
#endif

#endif

//----------------------------------------------------------------------------//
// NXP LPC176x family                                                         //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_LPC176x)

//---------------------------------------------------------
// include target dependent header files
//
#include "LPC17xx.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_LPC176x

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if ((MC_TARGET == MC_MCU_LPC1764FBD100))
//---------------------------------------------------------
// 128 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#if ((MC_TARGET == MC_MCU_LPC1765FET100) || (MC_TARGET == MC_MCU_LPC1765FBD100))
//---------------------------------------------------------
// 256 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#if ((MC_TARGET == MC_MCU_LPC1766FBD100))
//---------------------------------------------------------
// 256 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#if ((MC_TARGET == MC_MCU_LPC1768UK) || (MC_TARGET == MC_MCU_LPC1768FET100))
//---------------------------------------------------------
// 512 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#if ((MC_TARGET == MC_MCU_LPC1768FBD100) || (MC_TARGET == MC_MCU_LPC1769FBD100))
//---------------------------------------------------------
// 512 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif


#endif

//----------------------------------------------------------------------------//
// Renesas RX631 family                                                       //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_RX631)

#endif


//----------------------------------------------------------------------------//
// Renesas RX63N family                                                       //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_RX63N)

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_FAM_RX63N
#define CP_CHANNEL_MAX                 3


#endif


//----------------------------------------------------------------------------//
// ST STM32F103 family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F103)

//---------------------------------------------------------
// Define line the MCU belongs to, so corresponding
// register and defines are available
//
//#ifndef STM32F10X_HD
//#define STM32F10X_HD                   1
//#endif

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f10x.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F103

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 1

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if ((MC_TARGET == MC_MCU_STM32F103RC) || (MC_TARGET == MC_MCU_STM32F103VC))
//------------------------------------------------
// 256 kByte Flash, 48 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x0000C000)
#endif

//------------------------------------------------
// 384 kByte Flash, 64 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F103RD) || (MC_TARGET == MC_MCU_STM32F103VD))
#define MC_MCU_SIZE_FLASH              (0x00060000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

//------------------------------------------------
// 512 kByte Flash, 64 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F103ZC) || (MC_TARGET == MC_MCU_STM32F103ZD))
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#endif


//----------------------------------------------------------------------------//
// ST STM32F105 family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F105)

//---------------------------------------------------------
// Define MCU product line, so corresponding
// register and definitions are available
//
#ifndef STM32F10X_CL
#define STM32F10X_CL                   1
#endif

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f107_macro.h"
#include "stm32f10x.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F105

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if ((MC_TARGET == MC_MCU_STM32F105R8) || (MC_TARGET == MC_MCU_STM32F105V8))
//------------------------------------------------
// 64 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

//------------------------------------------------
// 128 kByte Flash, 64 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F105RB) || (MC_TARGET == MC_MCU_STM32F105VB))
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

//------------------------------------------------
// 256 kByte Flash, 64 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F105RC) || (MC_TARGET == MC_MCU_STM32F105VC))
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#endif


//----------------------------------------------------------------------------//
// ST STM32F107 family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F107)

//---------------------------------------------------------
// Define MCU product line, so corresponding
// register and definitions are available
//
#ifndef STM32F10X_CL
#define STM32F10X_CL                   1
#endif

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f107_macro.h"
#include "stm32f10x.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F107

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2


//------------------------------------------------
// 128 kByte Flash, 64 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F107RB) || (MC_TARGET == MC_MCU_STM32F107VB))
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

//------------------------------------------------
// 256 kByte Flash, 64 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F107RC) || (MC_TARGET == MC_MCU_STM32F107VC))
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#endif


//----------------------------------------------------------------------------//
// ST STM32F2xx family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F205)

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f2xx_macro.h"
#include "stm32f2xx.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F205

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_FAM_STM32F205
#define CP_CHANNEL_MAX                 2


//------------------------------------------------
// 128 kByte Flash, 64 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F205RB) || (MC_TARGET == MC_MCU_STM32F205VB))
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

//------------------------------------------------
// 256 kByte Flash, 96 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F205RC) || (MC_TARGET == MC_MCU_STM32F205VC))
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00018000)
#endif

//------------------------------------------------
// 512 kByte Flash, 128 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F205RE) || (MC_TARGET == MC_MCU_STM32F205VE))
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00020000)
#endif

//------------------------------------------------
// 768 kByte Flash, 128 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F205RF) || (MC_TARGET == MC_MCU_STM32F205VF))
#define MC_MCU_SIZE_FLASH              (0x000C0000)
#define MC_MCU_SIZE_RAM                (0x00020000)
#endif

//------------------------------------------------
// 1024 kByte Flash, 128 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F205RG) || (MC_TARGET == MC_MCU_STM32F205VG))
#define MC_MCU_SIZE_FLASH              (0x00100000)
#define MC_MCU_SIZE_RAM                (0x00020000)
#endif

#endif


//----------------------------------------------------------------------------//
// ST STM32F373 family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F373)

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f3xx_macro.h"
#include "stm32f37x.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F3

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 1

//---------------------------------------------------------
// Type dependent size of Flash and SRAM memory
//
#if (MC_TARGET == MC_MCU_STM32F373C8) || (MC_TARGET == MC_MCU_STM32F373R8)
//------------------------------------------------
// 64 kByte Flash, 16 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00004000)
#endif

#if (MC_TARGET == MC_MCU_STM32F373V8)
//---------------------------------------------------------
// 64 kByte Flash, 16 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00004000)
#endif

#if (MC_TARGET == MC_MCU_STM32F373CB) || (MC_TARGET == MC_MCU_STM32F373RB)
//---------------------------------------------------------
// 128 kByte Flash, 24 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00006000)
#endif

#if (MC_TARGET == MC_MCU_STM32F373VB)
//---------------------------------------------------------
// 128 kByte Flash, 24 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00006000)
#endif

#if (MC_TARGET == MC_MCU_STM32F373CC) || (MC_TARGET == MC_MCU_STM32F373RC)
//---------------------------------------------------------
// 256 kByte Flash, 32 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00008000)
#endif

#if (MC_TARGET == MC_MCU_STM32F373VC)
//---------------------------------------------------------
// 256 kByte Flash, 32 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00008000)
#endif

#endif


//----------------------------------------------------------------------------//
// ST STM32F405 family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F405)

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f4xx_macro.h"
#include "stm32f4xx.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F4

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2

//---------------------------------------------------------
// Type dependent size of Flash and SRAM memory
//
#if (MC_TARGET == MC_MCU_STM32F405RG) || (MC_TARGET == MC_MCU_STM32F405OG)
//-----------------------------------------------
// 1024 kByte Flash, 192 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00100000)
#define MC_MCU_SIZE_RAM                (0x00030000)
#endif

#if (MC_TARGET == MC_MCU_STM32F405VG) || (MC_TARGET == MC_MCU_STM32F405ZG)
//-----------------------------------------------
// 1024 kByte Flash, 192 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00100000)
#define MC_MCU_SIZE_RAM                (0x00030000)
#endif

#if (MC_TARGET == MC_MCU_STM32F405OE)
//-----------------------------------------------
// 512 kByte Flash, 192 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00030000)
#endif

#endif


//----------------------------------------------------------------------------//
// ST STM32F407 family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F407)

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f4xx_macro.h"
#include "stm32f4xx.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F4

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2

//---------------------------------------------------------
// Type dependent size of Flash and SRAM memory
//
#if (MC_TARGET == MC_MCU_STM32F407VE) || (MC_TARGET == MC_MCU_STM32F407ZE)
//-----------------------------------------------
// 512 kByte Flash, 192 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00030000)
#endif

#if (MC_TARGET == MC_MCU_STM32F407IE)
//-----------------------------------------------
// 512 kByte Flash, 192 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00030000)
#endif

#if (MC_TARGET == MC_MCU_STM32F407VG) || (MC_TARGET == MC_MCU_STM32F407ZG)
//-----------------------------------------------
// 1024 kByte Flash, 192 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00100000)
#define MC_MCU_SIZE_RAM                (0x00030000)
#endif

#if (MC_TARGET == MC_MCU_STM32F407IG)
//-----------------------------------------------
// 1024 kByte Flash, 192 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00100000)
#define MC_MCU_SIZE_RAM                (0x00030000)
#endif

#endif


//----------------------------------------------------------------------------//
// ST STM32F437 family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F437)

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f4xx_macro.h"
#include "stm32f4xx.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F4

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if (MC_TARGET == MC_MCU_STM32F437VG) || (MC_TARGET == MC_MCU_STM32F437ZG)
//-----------------------------------------------
// 1024 kByte Flash, 256 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00100000)
#define MC_MCU_SIZE_RAM                (0x00040000)
#endif

#if (MC_TARGET == MC_MCU_STM32F437IG)
//-----------------------------------------------
// 1024 kByte Flash, 256 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00100000)
#define MC_MCU_SIZE_RAM                (0x00040000)
#endif

#if (MC_TARGET == MC_MCU_STM32F437VI) || (MC_TARGET == MC_MCU_STM32F437ZI)
//-----------------------------------------------
// 2048 kByte Flash, 256 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00200000)
#define MC_MCU_SIZE_RAM                (0x00040000)
#endif

#if (MC_TARGET == MC_MCU_STM32F437AI) || (MC_TARGET == MC_MCU_STM32F437II)
//-----------------------------------------------
// 2048 kByte Flash, 256 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00200000)
#define MC_MCU_SIZE_RAM                (0x00040000)
#endif

#endif

//----------------------------------------------------------------------------//
// TI DSP Fixed Point TMS320F280x/1X family                                   //
//----------------------------------------------------------------------------//
#if (MC_TARGET == MC_MCU_TMS320F2808)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_DSP_TMS320F

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2

//-----------------------------------------------
// 128 kByte Flash, 36 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00009000)

#endif

//----------------------------------------------------------------------------//
// TI DSP Piccolo family                                                      //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_DSP_PICCOLO)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_DSP_PICCOLO

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 1

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//

#if (MC_TARGET == MC_MCU_TMS320F28030)
//-----------------------------------------------
// 16 kByte Flash, 6 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00004000)
#define MC_MCU_SIZE_RAM                (0x00001800)
#endif

#if (MC_TARGET == MC_MCU_TMS320F28031)
//-----------------------------------------------
// 32 kByte Flash, 8 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00008000)
#define MC_MCU_SIZE_RAM                (0x00002000)
#endif

#if (MC_TARGET == MC_MCU_TMS320F28032) || (MC_TARGET == MC_MCU_TMS320F28033)
//-----------------------------------------------
// 32 kByte Flash, 10 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00008000)
#define MC_MCU_SIZE_RAM                (0x00002800)
#endif

#if (MC_TARGET == MC_MCU_TMS320F28034) || (MC_TARGET == MC_MCU_TMS320F28035)
//-----------------------------------------------
// 64 kByte Flash, 10 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00002800)
#endif

#endif

//----------------------------------------------------------------------------//
// TI Dual core Concerto series                                               //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_DUAL_CONCERTO)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_DUAL_CONCERTO

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2
#endif


//----------------------------------------------------------------------------//
// TI TM4C123x family                                                         //
//----------------------------------------------------------------------------//

#if (MC_TARGET == MC_MCU_TM4C123GH6PGE)

//---------------------------------------------------------
// use processor include file from TIVA-C library
//
#include <inc/tm4c123gh6pge.h>

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_TM4C123

//---------------------------------------------------------
// 256 kByte Flash, 32 kByte RAM, 2 kByte EEPROM
//
#define MC_MCU_SIZE_EEPROM             (0x00000800)
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00008000)

//---------------------------------------------------------
// CANpie specific configuration
//
#define CP_TARGET                      MC_FAM_TM4C123
#define CP_CHANNEL_MAX                 2

#endif

//----------------------------------------------------------------------------//
// OS: Linux                                                                  //
//----------------------------------------------------------------------------//

#if (MC_TARGET == MC_OS_LINUX)

//---------------------------------------------------------
// CANpie specific configuration
//
#define CP_TARGET                      MC_OS_LINUX

#endif

//----------------------------------------------------------------------------//
// OS: Windows                                                                //
//----------------------------------------------------------------------------//

#if (MC_TARGET == MC_OS_WIN)

//---------------------------------------------------------
// CANpie specific configuration
//
#define CP_TARGET                      MC_OS_WIN

#endif


//----------------------------------------------------------------------------//
// QCAN driver interface                                                      //
//----------------------------------------------------------------------------//

#if (MC_TARGET == MC_QCAN)

//---------------------------------------------------------
// CANpie specific configuration
//
#define CP_TARGET                      MC_QCAN

#endif

#endif   // MC_TARGET_H_

