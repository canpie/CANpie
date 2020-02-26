//====================================================================================================================//
// File:          qcan_defs.hpp                                                                                       //
// Description:   QCAN classes - Definitions                                                                          //
//                                                                                                                    //
// Copyright (C) MicroControl GmbH & Co. KG                                                                           //
// 53844 Troisdorf - Germany                                                                                          //
// www.microcontrol.net                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the   //
// following conditions are met:                                                                                      //
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions, the following   //
//    disclaimer and the referenced file 'LICENSE'.                                                                   //
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       //
//    following disclaimer in the documentation and/or other materials provided with the distribution.                //
// 3. Neither the name of MicroControl nor the names of its contributors may be used to endorse or promote products   //
//    derived from this software without specific prior written permission.                                           //
//                                                                                                                    //
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance     //
// with the License. You may obtain a copy of the License at                                                          //
//                                                                                                                    //
//    http://www.apache.org/licenses/LICENSE-2.0                                                                      //
//                                                                                                                    //
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed   //
// on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for  //
// the specific language governing permissions and limitations under the License.                                     //
//                                                                                                                    //
//====================================================================================================================//


#ifndef QCAN_DEFS_HPP_
#define QCAN_DEFS_HPP_

//------------------------------------------------------------------------------------------------------
/*!
** \file    qcan_defs.hpp
** \brief   Global QCan definitions
**
** This file holds global definitions for the QCanServer and QCanInterface classes.
**
*/


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------------------------------
/*!
** \defgroup QCAN_NW QCan network definitions
**
**
*/

//------------------------------------------------------------------------------------------------------
/*!
** \def     QCAN_WEB_SOCKET_DEFAULT_PORT
** \ingroup QCAN_NW
** \brief   Default port for WebSocket server
**
** This symbol defines the default WebSocket port for the server.
*/
#define  QCAN_WEB_SOCKET_DEFAULT_PORT       55660


//------------------------------------------------------------------------------------------------------
/*!
** \def     QCAN_WEB_SOCKET_MAX
** \ingroup QCAN_NW
** \brief   Maximum number of WebSockets
**
** This symbol defines the maximum number of sockets connected to the WebSocket server.
*/
#define  QCAN_WEB_SOCKET_MAX                64

//------------------------------------------------------------------------------------------------------
/*!
** \def     QCAN_LOCAL_SOCKET_MAX
** \ingroup QCAN_NW
** \brief   Maximum number of local sockets
**
** This symbol defines the maximum number of local sockets connected to the server.
*/
#define  QCAN_LOCAL_SOCKET_MAX              64

//------------------------------------------------------------------------------------------------------
/*!
** \def     QCAN_NETWORK_MAX
** \ingroup QCAN_NW
** \brief   Maximum number of networks
**
** This symbol defines the maximum number of networks.
*/
#define  QCAN_NETWORK_MAX                   8


//------------------------------------------------------------------------------------------------------
/*!
** \defgroup QCAN_IF QCan interface definitions
**
** The QCan interface definitions are used by the method QCanInterface::supportedFeatures().
*/

//------------------------------------------------------------------------------------------------------
/*!
** \def     QCAN_IF_SUPPORT_ERROR_FRAMES
** \ingroup QCAN_IF
** \brief   Support error frames
**
** The bit-mask value defines if error frames are supported by a CAN interface.
*/
#define  QCAN_IF_SUPPORT_ERROR_FRAMES     ((uint32_t) (0x00000001))


//------------------------------------------------------------------------------------------------------
/*!
** \def     QCAN_IF_SUPPORT_LISTEN_ONLY
** \ingroup QCAN_IF
** \brief   Support listen-only mode
**
** The bit-mask value defines if the listen-only mode is supported by a CAN interface.
*/
#define  QCAN_IF_SUPPORT_LISTEN_ONLY      ((uint32_t) (0x00000002))


//------------------------------------------------------------------------------------------------------
/*!
** \def     QCAN_IF_SUPPORT_CAN_FD
** \ingroup QCAN_IF
** \brief   Support CAN FD
**
** The bit-mask value defines if CAN FD is supported by a CAN interface.
*/
#define  QCAN_IF_SUPPORT_CAN_FD           ((uint32_t) (0x00000004))

//------------------------------------------------------------------------------------------------------
/*!
** \def     QCAN_IF_SUPPORT_SPECIFIC_CONFIG
** \ingroup QCAN_IF
** \brief   Support device specific configuration
**
** The bit-mask value defines if CAN interface supports device specific configuration.
*/
#define  QCAN_IF_SUPPORT_SPECIFIC_CONFIG  ((uint32_t) (0x00000008))

#define  QCAN_IF_SUPPORT_MASK             ((uint32_t) (0x0000000F))


//------------------------------------------------------------------------------------------------------
/*!
** \def     QCAN_VERSION_MAJOR
** \ingroup QCAN_VERSION
**
** This symbol defines the major version number.
*/
#define  QCAN_VERSION_MAJOR                 0

//------------------------------------------------------------------------------------------------------
/*!
** \def     QCAN_VERSION_MINOR
** \ingroup QCAN_VERSION
**
** This symbol defines the minor version number.
*/
#define  QCAN_VERSION_MINOR                 99

//------------------------------------------------------------------------------------------------------
/*!
** \def     QCAN_VERSION_BUILD
** \ingroup QCAN_VERSION
**
** This symbol defines the build version number.
*/
#define  QCAN_VERSION_BUILD                 8


#endif // QCAN_DEFS_HPP_
