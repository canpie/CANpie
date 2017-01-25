//****************************************************************************//
// File:          cp_cc.h                                                     //
// Description:   Definitions for CAN controller chips / targets              //
// Author:        Uwe Koppe                                                   //
// e-mail:        koppe@microcontrol.net                                      //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53844 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions         //
// are met:                                                                   //
// 1. Redistributions of source code must retain the above copyright          //
//    notice, this list of conditions, the following disclaimer and           //
//    the referenced file 'COPYING'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'COPYING' file.                   //
//                                                                            //
//----------------------------------------------------------------------------//
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 29.07.2003  Initial version                                                //
//                                                                            //
//****************************************************************************//


#ifndef  CP_CC_H_
#define  CP_CC_H_


//------------------------------------------------------------------------------
// SVN  $Date: 2014-03-05 09:53:34 +0100 (Mi, 05 Mrz 2014) $
// SVN  $Rev: 5745 $ --- $Author: tiderko $
//------------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/*!
** \file    cp_cc.h
** \brief   CANpie constant values for targets
**
** This file defines values for the available silicon with CAN
** functionality. Each CAN controller (silicon) has its unique
** number, as shown in the following snippet:
**
** \code
** //--- CANary, AT89C51CC01 -------------
** #define CP_CC_CC01            0x1000
**
** //--- CANary, AT89C51CC02 -------------
** #define CP_CC_CC02            0x1001
**
** //--- CANary, AT89C51CC03 -------------
** #define CP_CC_CC03            0x1002
**
** \endcode
**
** The number must be assigned to the symbol CP_TARGET for the
** compilation process. This can be done by setting it in the
** file cp_cc.h (at the end) or within the Makefile / compiler
** setup. If you forget to set the symbol CP_TARGET, you will
** get the following error message: <br>
**
** <b>Target (Symbol CP_TARGET) is not defined! Check file cp_cc.h!</b>
*/


//-------------------------------------------------------//
// Generic                                               //
//-------------------------------------------------------//

//--- LINUX ---------------------------
#define CP_CC_LINUX           0x0010

//-------------------------------------------------------//
// Embedded OS                               (0x04x)     //
//-------------------------------------------------------//
#define CP_CC_EMBOS           0x0040


//-------------------------------------------------------//
// Analog Devices                            (0x08xx)    //
//-------------------------------------------------------//

//-------------------------------------------------------//
// ATMEL                                     (0x10xx)    //
//-------------------------------------------------------//

//--- CANary, AT89C51CC01 -------------
#define CP_CC_CC01            0x1000

//--- CANary, AT89C51CC02 -------------
#define CP_CC_CC02            0x1001

//--- CANary, AT89C51CC03 -------------
#define CP_CC_CC03            0x1002

//--- AVR, AT90CAN32 ------------------
#define CP_CC_AT90CAN32       0x1020

//--- AVR, AT90CAN64 ------------------
#define CP_CC_AT90CAN64       0x1021

//--- AVR, AT90CAN128 -----------------
#define CP_CC_AT90CAN128      0x1022


//--- ARM, AT91SAM7X ------------------
#define CP_CC_AT91SAM7X       0x1040



//-------------------------------------------------------//
// Bosch                                     (0x14xx)    //
//-------------------------------------------------------//

//-------------------------------------------------------//
// Cygnal                                    (0x18xx)    //
//-------------------------------------------------------//

//-------------------------------------------------------//
// Dallas                                    (0x1Cxx)    //
//-------------------------------------------------------//

//-------------------------------------------------------//
// Freescale                                 (0x1Dxx)    //
//-------------------------------------------------------//
#define CP_CC_XGATE           0x1D00

#define CP_CC_MCF523x         0x1D01

#define CP_CC_MCF5441x        0x1D10

//-------------------------------------------------------//
// Fujitsu                                   (0x20xx)    //
//-------------------------------------------------------//

#define CP_CC_LX340           0x2010
#define CP_CC_LX385           0x2020
#define CP_CC_LX495           0x2030

//--- MB96340 series (FX) -------------
#define CP_CC_FX340           0x2040


//-------------------------------------------------------//
// Infineon                                  (0x24xx)    //
//-------------------------------------------------------//

//--- Infineon C505 -------------------
#define CP_CC_C505            0x2400

//--- Infineon C515 -------------------
#define CP_CC_C515            0x2401

//--- Infineon C161 -------------------
#define CP_CC_C161            0x2402

//--- Infineon C164 -------------------
#define CP_CC_C164            0x2403

//--- Infineon C167 -------------------
#define CP_CC_C167            0x2404

//--- Infineon 81C90 ------------------
#define CP_CC_81C90           0x2405

//--- Infineon 81C91 ------------------
#define CP_CC_81C91           0x2406

//--- Infineon XC164 ------------------
#define CP_CC_XC164           0x2410


//-------------------------------------------------------//
// Microchip                                 (0x28xx)    //
//-------------------------------------------------------//
#define CP_CC_18Fxx8x         0x2810
#define CP_CC_18F6680         0x2812
#define CP_CC_18F8680         0x2814
#define CP_CC_PIC24E          0x2820
#define CP_CC_PIC32MX7XX      0x2830

//-------------------------------------------------------//
// Micronas                                  (0x2Cxx)    //
//-------------------------------------------------------//

//-------------------------------------------------------//
// Motorola                                  (0x30xx)    //
//-------------------------------------------------------//

//-------------------------------------------------------//
// National Semi.                            (0x34xx)    //
//-------------------------------------------------------//

//-------------------------------------------------------//
// NEC                                       (0x38xx)    //
//-------------------------------------------------------//

//-------------------------------------------------------//
// Philips / NXP                             (0x3Cxx)    //
//-------------------------------------------------------//

//--- Philips 82C200 ------------------
#define CP_CC_82C200          0x3C00

//--- Philips SJA1000 -----------------
#define CP_CC_SJA1000         0x3C01

//--- Philips 80C591 ------------------
#define CP_CC_80C591          0x3C02

//--- Philips 80C592 ------------------
#define CP_CC_80C592          0x3C03

//--- NXP LPC2109 ---------------------
#define CP_CC_LPC2109         0x3C0A

//--- NXP LPC2119 ---------------------
#define CP_CC_LPC2119         0x3C10

//--- NXP LPC2129 ---------------------
#define CP_CC_LPC2129         0x3C11

//--- NXP LPC2194 ---------------------
#define CP_CC_LPC2194         0x3C18

//--- NXP LPC2290 ---------------------
#define CP_CC_LPC2290         0x3C40

//--- NXP LPC2292 ---------------------
#define CP_CC_LPC2292         0x3C42

//--- NXP LPC2294 ---------------------
#define CP_CC_LPC2294         0x3C44

//--- NXP LPC2364 ---------------------
#define CP_CC_LPC2364         0x3C64

//--- NXP LPC2366 ---------------------
#define CP_CC_LPC2366         0x3C66

//--- NXP LPC2368 ---------------------
#define CP_CC_LPC2368         0x3C68

//--- NXP LPC2378 ---------------------
#define CP_CC_LPC2378         0x3C78

//--- NXP LPC2468 ---------------------
#define CP_CC_LPC2468         0x3C80

//--- NXP LPC2470 ---------------------
#define CP_CC_LPC2470         0x3C82

//--- NXP LPC2478 ---------------------
#define CP_CC_LPC2478         0x3C88

//--- NXP LPC177x / LPC178x family ----
#define CP_CC_LPC177x_8x      0x3CA0

//--- NXP LPC18xx family --------------
#define CP_CC_LPC18xx         0x3CB0



//-------------------------------------------------------//
// Renesas                                   (0x40xx)    //
//-------------------------------------------------------//

//--- Renesas R8C/34W family ----------
#define CP_CC_R8C_34W         0x4000

//-------------------------------------------------------//
// Silicon Laboratories                      (0x42xx)    //
//-------------------------------------------------------//
#define CP_CC_C8051F040 0x4200

//-------------------------------------------------------//
// ST                                        (0x44xx)    //
//-------------------------------------------------------//

//--- ST STR71x family ----------------
#define CP_CC_STR71x          0x4410

//--- ST STR710 -----------------------
#define CP_CC_STR710          0x4411

//--- ST STR712 -----------------------
#define CP_CC_STR712          0x4412

//--- ST STR91x family ----------------
#define CP_CC_STR91x          0x4420

//--- ST STR910 -----------------------
#define CP_CC_STR910          0x4421

//--- ST STR911 -----------------------
#define CP_CC_STR911          0x4422

//--- ST STR912 -----------------------
#define CP_CC_STR912          0x4423

//--- ST STM32F103x family ------------
#define CP_CC_STM32F103x      0x4430

//--- ST STM32F107x family ------------
#define CP_CC_STM32F107x      0x4432

//--- ST STM32F2xx family -------------
#define CP_CC_STM32F2xx       0x4438

//--- ST STM32F3xx family -------------
#define CP_CC_STM32F3xx       0x443C

//--- ST STM32F4xx family -------------
#define CP_CC_STM32F4xx       0x4440

//--- ST 10F2xx family ----------------
#define CP_CC_ST10F2xx        0x4450


//-------------------------------------------------------//
// Texas Instruments                         (0x48xx)    //
//-------------------------------------------------------//

#define CP_CC_LM3S2918        0x4880

#define CP_CC_TMS320F2808     0x4888

//-------------------------------------------------------//
// Toshiba                                   (0x4Cxx)    //
//-------------------------------------------------------//



//----------------------------------------------------------------------------//
//                                                                            //
// Define the CANpie target here (or in the Makefile)                         //
//                                                                            //
//----------------------------------------------------------------------------//
#ifndef  CP_TARGET
//#define  CP_TARGET   CP_CC_LINUX
#endif


#endif   /* CP_CC_H_   */

