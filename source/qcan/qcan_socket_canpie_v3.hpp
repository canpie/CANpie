//============================================================================//
// File:          qcan_socket_canpie_v2.hpp                                   //
// Description:   QCAN classes - CAN socket for CANpie version 2              //
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


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/



#include "../canpie/canpie-fd/cp_core.h"
#include "../canpie/canpie-fd/cp_msg.h"
#include "qcan_socket.hpp"


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
/*!
** \class QCanSocketCp3
** \brief CAN socket for CANpie version 3
**
**
*/
class QCanSocketCp3 : public QCanSocket
{
   Q_OBJECT

public:
   QCanSocketCp3();

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
