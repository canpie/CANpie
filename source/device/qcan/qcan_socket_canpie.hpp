//============================================================================//
// File:          qcan_socket_canpie.hpp                                      //
// Description:   QCAN classes - CAN socket for CANpie version 2              //
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


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/



#include "qcan_socket.hpp"
#include "../../canpie/cp_core.h"
#include "../../canpie/cp_msg.h"

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
/*!
** \class QCanSocketCp
** \brief CAN socket for CANpie version 2
**
**
*/
class QCanSocketCp : public QCanSocket
{
   Q_OBJECT

public:
   QCanSocketCp();

   QCanFrame   fromCpMsg(uint8_t ubMsgBufferV);
   CpCanMsg_ts fromCanFrame(QCanFrame & clCanFrameR);

   //-------------------------------------------------------------------
   // simulation of CAN message buffer
   //
   CpCanMsg_ts atsCanMsgM[CP_BUFFER_MAX];
   uint32_t    atsAccMaskM[CP_BUFFER_MAX];


   //-------------------------------------------------------------------
   // these pointers store the callback handlers
   //
   uint8_t     (* pfnRcvIntHandlerP) (CpCanMsg_ts *, uint8_t);
   uint8_t     (* pfnTrmIntHandlerP) (CpCanMsg_ts *, uint8_t);
   uint8_t     (* pfnErrIntHandlerP) (CpState_ts *);

private slots:
   void  onSocketReceive(void);

private:

};
