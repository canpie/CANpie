//====================================================================================================================//
// File:          qcan_server_memeory.hpp                                                                             //
// Description:   QCAN classes - CAN server                                                                           //
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


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include "qcan_defs.hpp"

/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#define  QCAN_MEMORY_KEY         "QCAN_SERVER_SHARED_KEY"

#define  QCAN_IF_NAME_LENGTH     64


/*--------------------------------------------------------------------------------------------------------------------*\
** Structures                                                                                                         **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------------------------------
// Server
//
typedef struct Server_s {

   //--------------------------------------------------------------------------
   // Server version: major
   //
   int32_t  slVersionMajor;

   //--------------------------------------------------------------------------
   // Server version: minor
   //
   int32_t  slVersionMinor;

   //--------------------------------------------------------------------------
   // Server version: build
   //
   int32_t  slVersionBuild;

   //--------------------------------------------------------------------------
   // Number of CAN networks supported by the server
   //
   int32_t  slNetworkCount;

   //--------------------------------------------------------------------------
   // Stores the start date / time of the server in milliseconds that have
   // passed since 1970-01-01T00:00:00.000
   //
   int64_t  sqDateTimeStart;

   //--------------------------------------------------------------------------
   // Stores the actual date / time of the server in milliseconds that have
   // passed since 1970-01-01T00:00:00.000
   //
   int64_t  sqDateTimeActual;

   int32_t  slReserved[56];

} Server_ts;


//-----------------------------------------------------------------------------------------------------
// Network
//
typedef struct Network_s {
   int32_t  slActive;
   int32_t  slStatus;
   int32_t  slNomBitRate;
   int32_t  slDatBitRate;
   char     szInterfaceName[QCAN_IF_NAME_LENGTH];
   int32_t  slReserved[108];
} Network_ts;

typedef struct ServerSettings_s {
   Server_ts   tsServer;
   Network_ts  atsNetwork[QCAN_NETWORK_MAX];
} ServerSettings_ts;

