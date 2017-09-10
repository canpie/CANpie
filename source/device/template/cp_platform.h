//============================================================================//
// File:          cp_platform.h                                               //
// Description:   CANpie platform configuration                               //
//                                                                            //
// Copyright 2017 MicroControl GmbH & Co. KG                                  //
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
// Licensed under the Apache License, Version 2.0 (the "License");            //
// you may not use this file except in compliance with the License.           //
// You may obtain a copy of the License at                                    //
//                                                                            //
//    http://www.apache.org/licenses/LICENSE-2.0                              //
//                                                                            //
// Unless required by applicable law or agreed to in writing, software        //
// distributed under the License is distributed on an "AS IS" BASIS,          //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   //
// See the License for the specific language governing permissions and        //
// limitations under the License.                                             //
//============================================================================//


#ifndef  CP_PLATFORM_H_
#define  CP_PLATFORM_H_



//-----------------------------------------------------------------------------
/*!
** \file    cp_platform.h
** \brief   CANpie configuration options
**
** 
*/


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*\
** Definitions & Enumerations                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
/*!
** \defgroup CP_CONF  CANpie configuration options
**
** The CANpie driver can be configured during compile time via
** several configuration options. They are typically defined in
** the \c cp_platform.h configuration file. The symbol #CP_TARGET is used
** to select an existing definition scheme from this file.
** <p>
** If symbols are not defined, they get a default value which is
** assigned in the \c canpie.h header file.
*/

/*-------------------------------------------------------------------*/
/*!
** \def  CP_AUTOBAUD
** \ingroup CP_CONF
**
** This symbol enables or disables the autobaud feature of the
** CAN driver (CpCoreAutobaud()).
** - 0 = disable autobaud feature (not supported by hardware)
** - 1 = enable autobaud feature
*/
#define  CP_AUTOBAUD                0

/*-------------------------------------------------------------------*/
/*!
** \def  CP_BUFFER_MAX
** \ingroup CP_CONF
**
** This symbol defines the number of message buffers (mailboxes)
** of a CAN controller. In case the controller has no message buffers,
** it is also possible to emulate these. A value of 0 denotes that
** there are no message buffers available. This also means all buffer
** functions (e.g. CpCoreBufferInit(), etc.) return the error code
** #CpErr_NOT_SUPPORTED.
*/
#define  CP_BUFFER_MAX              8

/*-------------------------------------------------------------------*/
/*!
** \def  CP_CAN_FD
** \ingroup CP_CONF
**
** This symbol defines if ISO CAN FD support is enabled.
** - 0 = disable ISO CAN FD support
** - 1 = enable ISO CAN FD support
*/
#ifndef  CP_CAN_FD
#define  CP_CAN_FD                  1
#endif

/*-------------------------------------------------------------------*/
/*!
** \def  CP_CAN_MSG_MACRO
** \ingroup CP_CONF
**
** This symbol defines if access to the CAN message structure
** CpCanMsg_s is done via macros or via functions.
** - 0 = access via functions
** - 1 = access via macros
*/
#ifndef  CP_CAN_MSG_MACRO
#define  CP_CAN_MSG_MACRO           0
#endif

/*-------------------------------------------------------------------*/
/*!
** \def  CP_CAN_MSG_MARKER
** \ingroup CP_CONF
**
** This symbol defines if the CAN message structure CpCanMsg_s
** has a marker field.
** - 0 = no marker field (not supported by hardware / driver)
** - 1 = include marker field
*/
#define  CP_CAN_MSG_MARKER          1

/*-------------------------------------------------------------------*/
/*!
** \def  CP_CAN_MSG_TIME
** \ingroup CP_CONF
**
** This symbol defines if the CAN message structure CpCanMsg_s
** has a time-stamp field.
** - 0 = no time-stamp field (not supported by hardware / driver)
** - 1 = include time-stamp field
*/
#define  CP_CAN_MSG_TIME            1

/*-------------------------------------------------------------------*/
/*!
** \def  CP_CAN_MSG_USER
** \ingroup CP_CONF
**
** This symbol defines if the CAN message structure CpCanMsg_s
** has a user-defined field.
** - 0 = no user-defined field (not supported by driver)
** - 1 = include user-defined field
*/
#define  CP_CAN_MSG_USER            1

/*-------------------------------------------------------------------*/
/*!
** \def  CP_CHANNEL_MAX
** \ingroup CP_CONF
**
** This symbol defines the total number of physical CAN interfaces
** supported by the driver. For a LPC2294 microcontroller the value
** would be 4 (4 CAN channels).
*/
#define  CP_CHANNEL_MAX             2

/*-------------------------------------------------------------------*/
/*!
** \def  CP_SMALL_CODE
** \ingroup CP_CONF
**
** This symbol is used to control the usage of the \c ptsPortV
** parameter in the core functions during compilation time. For
** microcontrollers with small resources and only one CAN channel
** the port parameter can be omitted.
** - 0 = use \c ptsPortV parameter in core functions
** - 1 = do not use \c ptsPortV parameter in core functions
*/
#define  CP_SMALL_CODE              0

/*-------------------------------------------------------------------*/
/*!
** \def  CP_STATISTIC
** \ingroup CP_CONF
**
** This symbol defines if the driver support statistic information.
** A value of 0 denotes that no statistic information is available.
** This also means that the function CpCoreStatistic() returns the
** error code #CpErr_NOT_SUPPORTED.
** - 0 = no statistic information (not supported by driver)
** - 1 = enable statistic information
*/
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

