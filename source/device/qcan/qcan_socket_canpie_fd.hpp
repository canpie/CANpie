//====================================================================================================================//
// File:          qcan_socket_canpie_fd.hpp                                                                           //
// Description:   QCAN classes - CAN socket for CANpie FD                                                             //
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


//------------------------------------------------------------------------------------------------------
// CANpie FD header files
//
#include "../../canpie-fd/cp_core.h"
#include "../../canpie-fd/cp_msg.h"
#if CP_NRL_CHANNEL_MAX > 0
#include "../../canpie-fd/cp_nrl.h"       // NRL support
#endif

//------------------------------------------------------------------------------------------------------
// QCan socket support
//
#include "qcan_socket.hpp"



#ifndef QCAN_SOCKET_CANPIE_HPP_
#define QCAN_SOCKET_CANPIE_HPP_


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//------------------------------------------------------------------------------------------------------
// Define macro for NRL support
//
#if CP_NRL_CHANNEL_MAX > 0
#define  CP_NRL_SUPPORT       1
#define  CP_API(func)         func##_Nrl
#else
#define  CP_NRL_SUPPORT       0
#define  CP_API(func)         func
#endif


//----------------------------------------------------------------------------------------------------------------
/*!
** \class   QCanSocketCpFD
**
** The QCanSocketCpFD class provides an interface between the %CANpie FD core functions and the QCanSocket class.
** Hence it is possible to write applications using the C API of %CANpie FD.
*/
class QCanSocketCpFD : public QCanSocket
{
   Q_OBJECT

public:
   QCanSocketCpFD();

   ~QCanSocketCpFD() override;

   #if CP_NRL_SUPPORT > 0
   void flushReceiveMailbox(CpNrlCoreData_ts * ptsCoreDataV);
   bool hasFault(void)                    { return btFaultOnLineP;      }

   void simulateFault(bool btEnableV)     { btFaultOnLineP = btEnableV; }
   #endif
   
   friend  CpStatus_tv CP_API(CpCoreBitrate)( CpPort_ts *ptsPortV, int32_t slNomBitRateV, int32_t slDatBitRateV);
   
   friend  CpStatus_tv CP_API(CpCoreBufferConfig)( CpPort_ts * ptsPortV,
         uint8_t   ubBufferIdxV,
         uint32_t  ulIdentifierV,
         uint32_t  ulAcceptMaskV,
         uint8_t   ubFormatV,
         uint8_t   ubDirectionV);
   
   friend  CpStatus_tv CP_API(CpCoreBufferGetData)( CpPort_ts * ptsPortV,
         uint8_t   ubBufferIdxV,
         uint8_t * pubDestDataV,
         uint8_t   ubStartPosV,
         uint8_t   ubSizeV);
   
   friend  CpStatus_tv CP_API(CpCoreBufferGetDlc)(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV, uint8_t * pubDlcV);
   
   friend  CpStatus_tv CP_API(CpCoreBufferRelease)( CpPort_ts * ptsPortV,uint8_t ubBufferIdxV);
   
   friend  CpStatus_tv CP_API(CpCoreBufferSend)(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV);

   friend  CpStatus_tv CP_API(CpCoreBufferSetData)( CpPort_ts * ptsPortV,
         uint8_t ubBufferIdxV,  uint8_t * pubSrcDataV,
         uint8_t   ubStartPosV, uint8_t   ubSizeV);

   friend  CpStatus_tv CP_API(CpCoreBufferSetDlc)(  CpPort_ts * ptsPortV,
         uint8_t ubBufferIdxV, uint8_t ubDlcV);
   
   friend  CpStatus_tv CP_API(CpCoreCanMode)(CpPort_ts * ptsPortV, uint8_t ubModeV);
   
   friend  CpStatus_tv CP_API(CpCoreCanState)(CpPort_ts * ptsPortV, CpState_ts * ptsStateV);
   
   friend  CpStatus_tv CP_API(CpCoreDriverInit)(uint8_t ubPhyIfV, CpPort_ts * ptsPortV,
         uint8_t ubConfigV);
   
   friend  CpStatus_tv CP_API(CpCoreDriverRelease)(CpPort_ts * ptsPortV);
   
   friend  CpStatus_tv CP_API(CpCoreFifoConfig)(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
         CpFifo_ts * ptsFifoV);
   
   friend  CpStatus_tv CP_API(CpCoreFifoRead)(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
         CpCanMsg_ts * ptsCanMsgV,
         uint32_t * pulMsgCntV);
   
   friend  CpStatus_tv CP_API(CpCoreFifoRelease)(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV);
   
   friend  CpStatus_tv CP_API(CpCoreFifoWrite)(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
         CpCanMsg_ts * ptsCanMsgV,
         uint32_t * pulMsgCntV);
   
   friend  CpStatus_tv CP_API(CpCoreHDI)(CpPort_ts *ptsPortV, CpHdi_ts *ptsHdiV);
   
   friend  CpStatus_tv CP_API(CpCoreIntFunctions)( CpPort_ts * ptsPortV,
         uint8_t (* pfnRcvHandler)(CpCanMsg_ts *, uint8_t),
         uint8_t (* pfnTrmHandler)(CpCanMsg_ts *, uint8_t),
         uint8_t (* pfnErrHandler)(CpState_ts *) );
   
   friend  CpStatus_tv CP_API(CpCoreStatistic)(CpPort_ts * ptsPortV, CpStatistic_ts * ptsStatsV);
   
   friend  CpStatus_tv CP_API(CpCoreStatisticClear)(CpPort_ts * ptsPortV);
  
   friend  bool CpNrlMboxCounterEvent(uint8_t ubPhyIfV, uint8_t ubBufferIdxV);
   friend  void CpNrlMboxTickEvent(void);

   //----------------------------------------------------------------------------------------------
   // Pointer to core data of NRL, configured inside CpCoreDriverInit()
   //
   #if CP_NRL_SUPPORT > 0
   CpNrlCoreData_ts *   ptsNrlCoreDataM;
   #endif

private slots:
   void           onSocketReceive(void);

   
private:
   QCanFrame      fromCpMsg(uint8_t ubMsgBufferV);
   QCanFrame      fromCpMsg(CpCanMsg_ts * ptsCanMsgV);
   CpCanMsg_ts    fromCanFrame(QCanFrame & clCanFrameR);
   
   void           handleCanFrame(QCanFrame & clCanFrameR);
   
   //----------------------------------------------------------------------------------------------
   // simulation of CAN message buffer
   //
   CpCanMsg_ts    atsCanMsgP[CP_BUFFER_MAX];
   uint32_t       atsAccMaskP[CP_BUFFER_MAX];


   //----------------------------------------------------------------------------------------------
   // these pointers store the callback handlers
   //
   uint8_t        (* pfnRcvIntHandlerP) (CpCanMsg_ts *, uint8_t);
   uint8_t        (* pfnTrmIntHandlerP) (CpCanMsg_ts *, uint8_t);
   uint8_t        (* pfnErrIntHandlerP) (CpState_ts *);

   //----------------------------------------------------------------------------------------------
   // these pointers store the FIFOs
   //
   CpFifo_ts *    aptsCanFifoP[CP_BUFFER_MAX];
   
   //----------------------------------------------------------------------------------------------
   // store configured nominal bit-rate and data bit-rate
   //
   int32_t        slNomBitRateP;
   int32_t        slDatBitRateP;   

   //----------------------------------------------------------------------------------------------
   // statistic counter values
   //
   CpStatistic_ts tsStatisticP;
   
   //----------------------------------------------------------------------------------------------
   // CAN state value
   //
   CpState_ts     tsCanStateP;

   //----------------------------------------------------------------------------------------------
   // Physical CAn interface
   //
   uint8_t        ubInterfaceP;

   //----------------------------------------------------------------------------------------------
   // Flag for fault simulation
   //
   #if CP_NRL_SUPPORT > 0
   bool                 btFaultOnLineP;
   #endif
};


CpStatus_tv    CpSocketConnectSlots( uint8_t ubPhyIfV, QObject * pclDestObjectV,
                                     const char * pubSockConnectV,
                                     const char * pubSockDisconnectV,
                                     const char * pubSockErrorV);

QCanSocket *   CpSocketInstance(uint8_t ubPhyIfV);

CpStatus_tv    CpSocketSetHostAddress(uint8_t ubPhyIfV, QHostAddress clHostAddressV);


//-------------------------------------------------------------------//
// The following function has C binding                              //
#ifdef __cplusplus                                                   //
extern "C" {                                                         //
#endif                                                               //
//-------------------------------------------------------------------//

void CpSocketProcessEvents();

//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//

#endif   // QCAN_SOCKET_CANPIE_HPP_
