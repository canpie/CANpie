//============================================================================//
// File:          demo_buffer_fifo.c                                          //
// Description:   Example for CANpie FIFO configuration                       //
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


#include "cp_core.h"
#include "cp_msg.h"


#define  FIFO_RCV_SIZE  32

static CpFifo_ts        tsFifoRcvS;
static CpCanMsg_ts      atsCanMsgRcvS[FIFO_RCV_SIZE];

//----------------------------------------------------------------------------//
// DemoFifoConfig()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void DemoFifoConfig(CpPort_ts * ptsCanPortV)
{
   //------------------------------------------------------
   // set message buffer 2 as receive buffer for classic
   // CAN frame with identifier 180h .. 18Fh
   //
   CpCoreBufferConfig(ptsCanPortV, eCP_BUFFER_2,
                      (uint32_t) 0x180,
                      (uint32_t) 0x7F0,   // mask
                      CP_MSG_FORMAT_CBFF,
                      eCP_BUFFER_DIR_RCV);

   CpFifoInit(&tsFifoRcvS, &atsCanMsgRcvS[0], FIFO_RCV_SIZE); 
   CpCoreFifoConfig(ptsCanPortV, eCP_BUFFER_2, &tsFifoRcvS);
}


//----------------------------------------------------------------------------//
// DemoFifoRead()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
void DemoFifoRead(CpPort_ts * ptsCanPortV)
{
   CpCanMsg_ts  tsCanMsgReadT;
   uint32_t     ulMsgCntT;
   //------------------------------------------------------
   // try to read one CAN message
   //
   ulMsgCntT = 1;
   CpCoreFifoRead(ptsCanPortV, eCP_BUFFER_2,
                  &tsCanMsgReadT,
                  &ulMsgCntT);
}
