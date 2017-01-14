//============================================================================//
// File:          mc_target_os.h                                              //
// Description:   MCU target operating system definitions                     //
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
// SVN  $Date: 2015-08-19 15:12:29 +0200 (Wed, 19. Aug 2015) $
// SVN  $Rev: 7001 $ --- $Author: tiderko $
//------------------------------------------------------------------------------


#ifndef  MC_TARGET_OS_H_
#define  MC_TARGET_OS_H_


//-----------------------------------------------------------------------------
/*!   
** \file    mc_target_os.h
** \brief   MCL - OS specific definitions and includes
**
** This file does the setup for the selected target OS.
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

//-------------------------------------------------------------------
// The symbol MC_TARGET_OS defines the target operating system
//
#ifndef MC_TARGET_OS
#warning  Target (Symbol MC_TARGET_OS) is not defined!
#define MC_TARGET_OS MC_OS_NONE
#endif


/*----------------------------------------------------------------------------*\
** Structures                                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

//----------------------------------------------------------------------------//
// No operating system used                                                   //
//----------------------------------------------------------------------------//
#if (MC_TARGET_OS == MC_OS_NONE)
#include "lwip_netpie.h"
#endif

//----------------------------------------------------------------------------//
// OS: embOS/IP                                                               //
//----------------------------------------------------------------------------//
#if (MC_TARGET_OS == MC_OS_EMBOS_IP)

// include embOS/IP specific header files
#include "rtos.h"
#include "bsp.h"
#include "ip.h"

// define type for a set of socket descriptors
typedef IP_fd_set                McSockDescSet_ts;
typedef int                      McSize_tv;
typedef int                      McSockLen_tv;
typedef IP_PACKET                McUdpPacket_ts;
typedef IP_PACKET                McEthPacket_ts;

// interface port of an ethernet connection
typedef int                  		McIfPort_ts;

#endif

//----------------------------------------------------------------------------//
// OS: Linux                                                                  //
//----------------------------------------------------------------------------//
#if (MC_TARGET_OS == MC_OS_LINUX)

//---------------------------------------------------------
// CANpie specific configuration
//
#define CP_TARGET                      MC_OS_LINUX

#endif

//----------------------------------------------------------------------------//
// OS: Windows                                                                //
//----------------------------------------------------------------------------//
#if (MC_TARGET_OS == MC_OS_WIN)

//---------------------------------------------------------
// CANpie specific configuration
//
#define CP_TARGET                      MC_OS_WIN

#endif

#endif   // MC_TARGET_OS_H_

