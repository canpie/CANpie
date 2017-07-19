//============================================================================//
// File:          cp_platform.h                                               //
// Description:   CANpie platform configuration                               //
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
//    the referenced file 'LICENSE'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'LICENSE' file.                   //
//                                                                            //
//============================================================================//


#ifndef  CP_PLATFORM_H_
#define  CP_PLATFORM_H_



//-----------------------------------------------------------------------------
/*!
** \file    cp_platform.h
** \brief   CANpie platform configuration
**
** The %CANpie<sup>FD</sup> driver can be configured during compile time
** via several configuration options. They are typically defined in the
** \c cp_platform.h configuration file. See \ref CP_CONF for a detailed
** description of all options. If symbols are not defined, they get a
** default value which is assigned in the \c canpie.h header file.
** 
*/


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


#define  CP_AUTOBAUD                0

#define  CP_BUFFER_MAX              32

#define  CP_CAN_FD                  1

#define  CP_CAN_MSG_MACRO           1

#define  CP_CAN_MSG_MARKER          1

#define  CP_CAN_MSG_TIME            1

#define  CP_CAN_MSG_USER            1

#define  CP_CHANNEL_MAX             2

#define  CP_SMALL_CODE              0

#define  CP_STATISTIC               1



/*----------------------------------------------------------------------------*\
** Structures                                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*!
** \struct  CpPortEmbedded_s
** \brief   Port structure for embedded target
**
*/
struct CpPortEmbedded_s {

   /*!   Physical CAN interface number,
   **    first CAN channel (index) is eCP_CHANNEL_1
   */
   uint8_t     ubPhyIf;

   /*!   Private driver information 
   */
   uint8_t     ubDrvInfo;

};

/*----------------------------------------------------------------------------*\
** Type definitions                                                           **
**                                                                            **
\*----------------------------------------------------------------------------*/

typedef struct CpPortEmbedded_s  CpPort_ts;

typedef uint8_t                  CpStatus_tv;




#endif   /* CP_PLATFORM_H_   */

