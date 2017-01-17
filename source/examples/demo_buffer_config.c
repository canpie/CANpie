//============================================================================//
// File:          demo_buffer_config.c                                        //
// Description:   Example for CANpie message buffer configuration             //
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


//----------------------------------------------------------------------------//
// DemoReceiveBufferConfiguration()                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void DemoReceiveBufferConfiguration(CpPort_ts * ptsPortV)
{
   //----------------------------------------------------------------
   // set message buffer 2 as receive buffer for classic CAN frame
   // with Standard Identifier 211h
   //
   CpCoreBufferConfig(CpPort_ts * ptsPortV, 
                      eCP_BUFFER_2,
                      (uint32_t) 0x211,
                      CP_MASK_STD_FRAME,
                      CP_MSG_FORMAT_CBFF,
                      eCP_BUFFER_DIR_RCV);

   //----------------------------------------------------------------
   // set message buffer 3 as receive buffer for classic CAN frame
   // with Extended Identifier 18EEFF00h
   //
   CpCoreBufferConfig(CpPort_ts * ptsPortV, 
                      eCP_BUFFER_3,
                      (uint32_t) 0x18EEFF00,
                      CP_MASK_EXT_FRAME,
                      CP_MSG_FORMAT_CEFF,
                      eCP_BUFFER_DIR_RCV);

}


//----------------------------------------------------------------------------//
// DemoTransmitBufferConfiguration()                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void DemoTransmitBufferConfiguration(CpPort_ts * ptsPortV)
{
   //----------------------------------------------------------------
   // set message buffer 1 as transmit buffer for classic CAN frame
   // with Standard Identifier 123h, DLC = 4
   //
   CpCoreBufferConfig(CpPort_ts * ptsPortV, eCP_BUFFER_1,
                      (uint32_t) 0x123,
                      CP_MASK_STD_FRAME,
                      CP_MSG_FORMAT_CBFF,
                      eCP_BUFFER_DIR_TRM);
   
   CpCoreBufferConfig(CpPort_ts * ptsPortV, eCP_BUFFER_1, 4);
}
