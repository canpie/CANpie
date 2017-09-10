//============================================================================//
// File:          qcan_socket_canpie.cpp                                      //
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



#include "../../canpie-fd/cp_core.h"
#include "../../canpie-fd/cp_msg.h"
#include "qcan_socket.hpp"


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
/*!
** \class QCanSocketCpFD
** \brief CAN socket for CANpie FD
**
** The QCanSocketCpFD class provides a CANpie FD interface for the
** QCanSocket class. 
**
*/
class QCanSocketCpFD : public QCanSocket
{
   Q_OBJECT

public:
   QCanSocketCpFD();


   friend  CpStatus_tv CpCoreBitrate( CpPort_ts * ptsPortV, int32_t slBitrateV,
         int32_t slBrsClockV);
   
   friend  CpStatus_tv CpCoreBufferConfig( CpPort_ts * ptsPortV,
         uint8_t   ubBufferIdxV,
         uint32_t  ulIdentifierV,
         uint32_t  ulAcceptMaskV,
         uint8_t   ubFormatV,
         uint8_t   ubDirectionV);
   
   friend  CpStatus_tv CpCoreBufferGetData( CpPort_ts * ptsPortV,
         uint8_t   ubBufferIdxV,
         uint8_t * pubDestDataV,
         uint8_t   ubStartPosV,
         uint8_t   ubSizeV);
   
   friend   CpStatus_tv CpCoreBufferGetDlc(  CpPort_ts * ptsPortV,
         uint8_t ubBufferIdxV, uint8_t * pubDlcV);
   
   friend  CpStatus_tv CpCoreBufferRelease( CpPort_ts * ptsPortV,
         uint8_t ubBufferIdxV);
   
   friend  CpStatus_tv CpCoreBufferSend(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV);

   friend  CpStatus_tv CpCoreBufferSetData( CpPort_ts * ptsPortV,
         uint8_t ubBufferIdxV,  uint8_t * pubSrcDataV,
         uint8_t   ubStartPosV, uint8_t   ubSizeV);

   friend  CpStatus_tv CpCoreBufferSetDlc(  CpPort_ts * ptsPortV,
         uint8_t ubBufferIdxV, uint8_t ubDlcV);
   
   friend  CpStatus_tv CpCoreCanMode(CpPort_ts * ptsPortV, uint8_t ubModeV);
   
   friend  CpStatus_tv CpCoreCanState(CpPort_ts * ptsPortV, CpState_ts * ptsStateV);
   
   friend  CpStatus_tv CpCoreDriverInit(uint8_t ubPhyIfV, CpPort_ts * ptsPortV,
         uint8_t ubConfigV);
   
   friend  CpStatus_tv CpCoreDriverRelease(CpPort_ts * ptsPortV);
   
   friend  CpStatus_tv CpCoreFifoConfig(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
         CpFifo_ts * ptsFifoV);
   
   friend  CpStatus_tv CpCoreFifoRead(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
         CpCanMsg_ts * ptsCanMsgV,
         uint32_t * pulMsgCntV);
   
   friend  CpStatus_tv CpCoreFifoRelease(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV);
   
   friend  CpStatus_tv CpCoreFifoWrite(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
         CpCanMsg_ts * ptsCanMsgV,
         uint32_t * pulMsgCntV);
   
   friend  CpStatus_tv CpCoreHDI(CpPort_ts *ptsPortV, CpHdi_ts *ptsHdiV);
   
   friend  CpStatus_tv CpCoreIntFunctions( CpPort_ts * ptsPortV,
         uint8_t (* pfnRcvHandler)(CpCanMsg_ts *, uint8_t),
         uint8_t (* pfnTrmHandler)(CpCanMsg_ts *, uint8_t),
         uint8_t (* pfnErrHandler)(CpState_ts *) );
   
   friend  CpStatus_tv CpCoreStatistic(CpPort_ts * ptsPortV, CpStatistic_ts * ptsStatsV);
   
  
private slots:
   void  onSocketReceive(void);
   void  onSocketConnect(void);
   void  onSocketDisconnect(void);
   
private:
   QCanFrame      fromCpMsg(uint8_t ubMsgBufferV);
   QCanFrame      fromCpMsg(CpCanMsg_ts * ptsCanMsgV);
   CpCanMsg_ts    fromCanFrame(QCanFrame & clCanFrameR);
   
   void           handleApiFrame(QCanFrameApi & clApiFrameR);
   void           handleCanFrame(QCanFrame & clCanFrameR);
   void           handleErrFrame(QCanFrameError & clErrFrameR);
   
   //-------------------------------------------------------------------
   // simulation of CAN message buffer
   //
   CpCanMsg_ts    atsCanMsgP[CP_BUFFER_MAX];
   uint32_t       atsAccMaskP[CP_BUFFER_MAX];


   //-------------------------------------------------------------------
   // these pointers store the callback handlers
   //
   uint8_t        (* pfnRcvIntHandlerP) (CpCanMsg_ts *, uint8_t);
   uint8_t        (* pfnTrmIntHandlerP) (CpCanMsg_ts *, uint8_t);
   uint8_t        (* pfnErrIntHandlerP) (CpState_ts *);

   //-------------------------------------------------------------------
   // these pointers store the FIFOs
   //
   CpFifo_ts *    aptsCanFifoP[CP_BUFFER_MAX];
   
   //-------------------------------------------------------------------
   // store configured nominal bit-rate and data bit-rate
   //
   int32_t        slNomBitRateP;
   int32_t        slDatBitRateP;   

   //-------------------------------------------------------------------
   // statistic counter values
   //
   CpStatistic_ts tsStatisticP;
   
   //-------------------------------------------------------------------
   // CAN state value
   //
   CpState_ts     tsCanStateP;
   
   uint8_t     ubStatusP;
};


CpStatus_tv CpSocketConnectSlots( uint8_t ubPhyIfV, QObject * pclDestObjectV,
                                  const char * pubSockConnectV,
                                  const char * pubSockDisconnectV,
                                  const char * pubSockErrorV);

CpStatus_tv CpSocketSetHostAddress(uint8_t ubPhyIfV, 
                                   QHostAddress clHostAddressV);


