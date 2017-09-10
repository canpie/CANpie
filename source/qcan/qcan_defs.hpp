//============================================================================//
// File:          qcan_defs.hpp                                               //
// Description:   QCAN classes - Definitions                                  //
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


#ifndef QCAN_DEFS_HPP_
#define QCAN_DEFS_HPP_

//-----------------------------------------------------------------------------
/*!
** \file    qcan_defs.hpp
** \brief   Global QCan definitions
**
** This file holds global definitions for the QCanServer and QCanInterface
** classes.
**
*/

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-------------------------------------------------------------------
/*!
** \defgroup QCAN_NW QCan network definitions
**
**
*/

//-------------------------------------------------------------------
/*!
** \def     QCAN_TCP_DEFAULT_PORT
** \ingroup QCAN_NW
** \brief   Default port for TCP server
**
** This symbol defines the default TCP port for the server.
*/
#define  QCAN_TCP_DEFAULT_PORT      55660


//-------------------------------------------------------------------
/*!
** \def     QCAN_TCP_SOCKET_MAX
** \ingroup QCAN_NW
** \brief   Maximum number of TCP sockets
**
** This symbol defines the maximum number of sockets connected to
** the TCP server.
*/
#define  QCAN_TCP_SOCKET_MAX        16


//-------------------------------------------------------------------
/*!
** \def     QCAN_NETWORK_MAX
** \ingroup QCAN_NW
** \brief   Maximum number of networks
**
** This symbol defines the maximum number of networks.
*/
#define  QCAN_NETWORK_MAX           8


//-------------------------------------------------------------------
/*!
** \defgroup QCAN_IF QCan interface definitions
**
** The QCan interface definitions are used by the method
** QCanInterface::supportedFeatures().
*/

//-------------------------------------------------------------------
/*!
** \def     QCAN_IF_SUPPORT_ERROR_FRAMES
** \ingroup QCAN_IF
** \brief   Support error frames
**
** The bit-mask value defines if error frames are supported by
** a CAN interface.
*/
#define  QCAN_IF_SUPPORT_ERROR_FRAMES     ((uint32_t) (0x00000001))


//-------------------------------------------------------------------
/*!
** \def     QCAN_IF_SUPPORT_LISTEN_ONLY
** \ingroup QCAN_IF
** \brief   Support listen-only mode
**
** The bit-mask value defines if the listen-only mode is supported by
** a CAN interface.
*/
#define  QCAN_IF_SUPPORT_LISTEN_ONLY      ((uint32_t) (0x00000002))


//-------------------------------------------------------------------
/*!
** \def     QCAN_IF_SUPPORT_CAN_FD
** \ingroup QCAN_IF
** \brief   Support CAN FD
**
** The bit-mask value defines if CAN FD is supported by
** a CAN interface.
*/
#define  QCAN_IF_SUPPORT_CAN_FD           ((uint32_t) (0x00000004))



#endif // QCAN_DEFS_HPP_
