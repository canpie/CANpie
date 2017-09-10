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


#define  CP_AUTOBAUD                0

#define  CP_BUFFER_MAX              32

#define  CP_CAN_FD                  1

#define  CP_CAN_MSG_MACRO           0

#define  CP_CAN_MSG_MARKER          1

#define  CP_CAN_MSG_TIME            1

#define  CP_CAN_MSG_USER            1

#define  CP_CHANNEL_MAX             8

#define  CP_SMALL_CODE              0

#define  CP_STATISTIC               1



/*----------------------------------------------------------------------------*\
** Structures                                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*!
** \struct  CpPortQCan_s
** \brief   Port structure for QCan target
**
*/
struct CpPortQCan_s {

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

typedef struct CpPortQCan_s      CpPort_ts;

typedef uint8_t                  CpStatus_tv;









#endif   /* CP_PLATFORM_H_   */

