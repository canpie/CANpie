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

#include <QApplication>
#include <QTimer>
#include <QVector>


#include "qcan_frame_api.hpp"
#include "qcan_frame_error.hpp"
#include "qcan_socket_canpie.hpp"


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define  CP_USER_FLAG_RCV        ((uint32_t)(0x00000001))
#define  CP_USER_FLAG_TRM        ((uint32_t)(0x00000002))

//-------------------------------------------------------------------
// time to wait (in ms) for socket connection
//
#define  SOCKET_CONNECT_WAIT     ((int32_t)(50))


/*----------------------------------------------------------------------------*\
** Internal function                                                          **
**                                                                            **
\*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*\
** external functions                                                         **
**                                                                            **
\*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

static QCanSocketCp  aclCanSockListS[QCAN_NETWORK_MAX];




/*----------------------------------------------------------------------------*\
** Function implementation                                                    **
**                                                                            **
\*----------------------------------------------------------------------------*/




//----------------------------------------------------------------------------//
// CpCoreAutobaud()                                                           //
// run automatic baudrate detection                                           //
//----------------------------------------------------------------------------//
#if CP_AUTOBAUD == 0
CpStatus_tv CpCoreAutobaud(CpPort_ts * ptsPortV, 
                           uint8_t *  CPP_PARM_UNUSED(pubBaudSelV),
                           uint16_t * CPP_PARM_UNUSED(puwWaitV))
#else
CpStatus_tv CpCoreAutobaud(CpPort_ts * ptsPortV,
                           uint8_t *   pubBaudSelV,
                           uint16_t *  puwWaitV)
#endif
{
   //----------------------------------------------------------------
   // avoid compiler warning
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   #endif

   //----------------------------------------------------------------
   // check if automatic bitrate detection is enabled
   //
   #if CP_AUTOBAUD == 0
   return(CpErr_NOT_SUPPORTED);
   #else

   if(pubBaudSelV != 0L)
   {
      *pubBaudSelV = CP_BAUD_500K;
   }

   if(puwWaitV != 0L)
   {
      *puwWaitV    = 0;
   }

   return(CpErr_OK);
   #endif
}


//----------------------------------------------------------------------------//
// CpCoreBaudrate()                                                           //
// Setup baudrate of CAN controller                                           //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBaudrate(CpPort_ts * ptsPortV, uint8_t ubBaudSelV)
{
   QCanFrameApi      clFrameT;
   QCanSocketCp *   pclSockT;
   int32_t           slBitrateT;

   //----------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpCoreBaudrate() .... :" << ubBaudSelV;

   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(CpErr_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);


   //----------------------------------------------------------------
   // test parameter value
   //
   if(ubBaudSelV > CP_BAUD_MAX)
   {
      return(CpErr_BAUDRATE);
   }

   //----------------------------------------------------------------
   // Make sure that the socket is connected.
   //
   QTimer clTimeOutT;
   clTimeOutT.start(SOCKET_CONNECT_WAIT);
   while(pclSockT->isConnected() == false)
   {
      QApplication::processEvents();
      if(clTimeOutT.isActive() == false)
      {
         return(CpErr_INIT_FAIL);
      }
   }

   //----------------------------------------------------------------
   // The parameter ubBaudSelV uses the same enumeration values
   // as defined in QCan::CAN_Bitrate_e, so it can be copied.
   //
   slBitrateT = ubBaudSelV;
   clFrameT.setBitrate(slBitrateT, eCAN_BITRATE_NONE);

   if(pclSockT->writeFrame(clFrameT) == false)
   {
      qDebug() << "CpCoreBaudrate(): Failed to set bitrate";
      return(CpErr_INIT_FAIL);
   }


   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferAccMask()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferAccMask( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint32_t ulAccMaskV)
{
   QCanSocketCp *   pclSockT;

   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(CpErr_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);


   //----------------------------------------------------------------
   // set acceptance mask
   //
   pclSockT->atsAccMaskM[ubBufferIdxV - 1] = ulAccMaskV;

   return(CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferGetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetData( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubDataV)
{
   uint8_t           ubCntT;
   QCanSocketCp *   pclSockT;

   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(CpErr_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);


   //----------------------------------------------------------------
   // copy data from simulated CAN buffer
   //
   for(ubCntT = 0; ubCntT < 8; ubCntT++)
   {
      *pubDataV = CpMsgGetData(&(pclSockT->atsCanMsgM[ubBufferIdxV - 1]),
                               ubCntT);
      pubDataV++;
   }

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferGetDlc()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetDlc(  CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubDlcV)
{
   QCanSocketCp *   pclSockT;

   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(CpErr_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);


   //----------------------------------------------------------------
   // read DLC from simulated CAN buffer
   //
   *pubDlcV = pclSockT->atsCanMsgM[ubBufferIdxV - 1].ubMsgDLC;

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferInit()                                                         //
// Initialise CAN message buffer                                              //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferInit( CpPort_ts * ptsPortV, CpCanMsg_ts * ptsCanMsgV,
                              uint8_t ubBufferIdxV, uint8_t ubDirectionV)
{
   QCanSocketCp *   pclSockT;

   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(CpErr_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);

   //----------------------------------------------------------------
   // align buffer index
   //
   ubBufferIdxV = ubBufferIdxV - 1;

   //----------------------------------------------------------------
   // copy to simulated CAN buffer
   //
   pclSockT->atsCanMsgM[ubBufferIdxV].tuMsgId.ulExt = ptsCanMsgV->tuMsgId.ulExt;
   pclSockT->atsCanMsgM[ubBufferIdxV].ubMsgDLC      = ptsCanMsgV->ubMsgDLC;
   pclSockT->atsCanMsgM[ubBufferIdxV].ubMsgCtrl     = ptsCanMsgV->ubMsgCtrl;

   //----------------------------------------------------------------
   // mark Tx/Rx message
   //
   if(ubDirectionV == CP_BUFFER_DIR_TX)
   {
      pclSockT->atsCanMsgM[ubBufferIdxV].ulMsgUser = CP_USER_FLAG_TRM;
   }
   else
   {
      pclSockT->atsCanMsgM[ubBufferIdxV].ulMsgUser = CP_USER_FLAG_RCV;
   }


   //----------------------------------------------------------------
   // set acceptance mask to default value
   //
   pclSockT->atsAccMaskM[ubBufferIdxV] = 0x1FFFFFFF;

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferRelease( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   QCanSocketCp *   pclSockT;

   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(CpErr_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);

   //----------------------------------------------------------------
   // clear simulated CAN buffer
   //
   pclSockT->atsCanMsgM[ubBufferIdxV - 1].tuMsgId.ulExt = 0;
   pclSockT->atsCanMsgM[ubBufferIdxV - 1].ubMsgDLC      = 0;
   pclSockT->atsCanMsgM[ubBufferIdxV - 1].ubMsgCtrl     = 0;

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSend()                                                         //
// send message out of the CAN controller                                     //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSend(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   QCanFrame         clFrameT;
   QCanSocketCp *   pclSockT;

   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(CpErr_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);

   //----------------------------------------------------------------
   // align buffer index
   //
   ubBufferIdxV = ubBufferIdxV - 1;


   //----------------------------------------------------------------
   // write CAN frame
   //
   clFrameT = pclSockT->fromCpMsg(ubBufferIdxV);
   if(pclSockT->writeFrame(clFrameT) == false)
   {
      qDebug() << "Failed to write message";
   }
   else
   {
      if(pclSockT->pfnTrmIntHandlerP != 0)
      {
         (* pclSockT->pfnTrmIntHandlerP)(&(pclSockT->atsCanMsgM[ubBufferIdxV]),
                                         ubBufferIdxV + 1);
      }
   }

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSetData( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubDataV)
{
   uint8_t           ubCntT;
   QCanSocketCp *   pclSockT;

   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(CpErr_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);

   //----------------------------------------------------------------
   // copy data to simulated CAN buffer
   //
   for(ubCntT = 0; ubCntT < 8; ubCntT++)
   {
      CpMsgSetData(&(pclSockT->atsCanMsgM[ubBufferIdxV - 1]),
                   ubCntT, *pubDataV);
      pubDataV++;
   }

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSetDlc()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSetDlc(  CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t ubDlcV)
{
   QCanSocketCp *   pclSockT;

   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(CpErr_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);

   //----------------------------------------------------------------
   // write DLC to simulated CAN buffer
   //
   pclSockT->atsCanMsgM[ubBufferIdxV - 1].ubMsgDLC = ubDlcV;

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferTransmit()                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferTransmit(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 CpCanMsg_ts * ptsCanMsgV)
{
   QCanSocketCp *   pclSockT;
   QCanFrame         clFrameT;


   qDebug() << "CpCoreBufferTransmit()...";

   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(CpErr_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);

   //----------------------------------------------------------------
   // align buffer index
   //
   ubBufferIdxV = ubBufferIdxV - 1;

   //----------------------------------------------------------------
   // copy to simulated CAN buffer
   //
   pclSockT->atsCanMsgM[ubBufferIdxV].tuMsgId.ulExt = ptsCanMsgV->tuMsgId.ulExt;
   pclSockT->atsCanMsgM[ubBufferIdxV].ubMsgDLC      = ptsCanMsgV->ubMsgDLC;
   pclSockT->atsCanMsgM[ubBufferIdxV].ubMsgCtrl     = ptsCanMsgV->ubMsgCtrl;
   pclSockT->atsCanMsgM[ubBufferIdxV].tuMsgData.aulLong[0] = ptsCanMsgV->tuMsgData.aulLong[0];
   pclSockT->atsCanMsgM[ubBufferIdxV].tuMsgData.aulLong[1] = ptsCanMsgV->tuMsgData.aulLong[1];
   pclSockT->atsCanMsgM[ubBufferIdxV].ulMsgUser     = CP_USER_FLAG_TRM;


   //----------------------------------------------------------------
   // write CAN frame
   //
   clFrameT = pclSockT->fromCpMsg(ubBufferIdxV);
   if(pclSockT->writeFrame(clFrameT) == false)
   {
      qDebug() << "Failed to write message";
   }
   else
   {
      if(pclSockT->pfnTrmIntHandlerP != 0)
      {
         (* pclSockT->pfnTrmIntHandlerP)(&(pclSockT->atsCanMsgM[ubBufferIdxV]),
                                         ubBufferIdxV + 1);
      }
   }


   return (CpErr_OK);
}

//----------------------------------------------------------------------------//
// CpCoreCanMode()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanMode(CpPort_ts * ptsPortV, uint8_t ubModeV)
{
   QCanFrameApi      clFrameT;
   QCanSocketCp *   pclSockT;

   //----------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpCoreMode() ........ :" << ubModeV;


   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(CpErr_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);


   //----------------------------------------------------------------
   // check connection state
   //
   if(pclSockT->isConnected() == false)
   {
      if(ubModeV == CP_MODE_STOP)
      {
         return(CpErr_OK);
      }
      return(CpErr_GENERIC);
   }


   //----------------------------------------------------------------
   // switch CAN controller into mode "ubModeV"
   //
   switch(ubModeV)
   {
      //--------------------------------------------------------
      // Stop the CAN controller (passive on the bus)
      //
      case CP_MODE_STOP:
         clFrameT.setMode(eCAN_MODE_STOP);
         break;

      //--------------------------------------------------------
      // Start the CAN controller (active on the bus)
      //
      case CP_MODE_START:
         clFrameT.setMode(eCAN_MODE_START);
         break;

      //--------------------------------------------------------
      // Start the CAN controller (Listen-Only)
      //
      case CP_MODE_LISTEN_ONLY:
         clFrameT.setMode(eCAN_MODE_LISTEN_ONLY);
         break;

      //--------------------------------------------------------
      // Other modes are not supported
      //
      default:
         return(CpErr_NOT_SUPPORTED);
         break;
   }


   return(CpErr_OK);
}



//----------------------------------------------------------------------------//
// CpCoreCanState()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanState(CpPort_ts * ptsPortV, CpState_ts * ptsStateV)
{
   //----------------------------------------------------------------
   // check port
   //
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }

   ptsStateV->ubCanErrState = CP_STATE_BUS_ACTIVE;
   ptsStateV->ubCanErrType  = CP_ERR_TYPE_NONE;

   //----------------------------------------------------------------
   // get current error counter
   //
   ptsStateV->ubCanTrmErrCnt = 0;
   ptsStateV->ubCanRcvErrCnt = 0;

   return(CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreDriverInit()                                                         //
// init CAN controller                                                        //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverInit(uint8_t ubPhyIfV, CpPort_ts * ptsPortV)
{
   QCanSocketCp *   pclSockT;

   //----------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpCoreDriverInit() .. :" << ubPhyIfV;

   //----------------------------------------------------------------
   // test parameter
   //
   if(ubPhyIfV >= QCAN_NETWORK_MAX)
   {
      return(CpErr_CHANNEL);
   }


   aclCanSockListS[ubPhyIfV].connectNetwork((CAN_Channel_e) ubPhyIfV);

   //----------------------------------------------------------------
   // get access to socket
   //
   pclSockT = &(aclCanSockListS[ubPhyIfV]);
   pclSockT->connectNetwork((CAN_Channel_e) ubPhyIfV);


   //----------------------------------------------------------------
   // Make sure that the socket is connected.
   //
   QTimer clTimeOutT;
   clTimeOutT.start(SOCKET_CONNECT_WAIT);
   while(pclSockT->isConnected() == false)
   {
      QApplication::processEvents();
      if(clTimeOutT.isActive() == false)
      {
         ptsPortV->ubPhyIf = 255;   // make channel out of range
         return(CpErr_INIT_FAIL);
      }
   }

   //----------------------------------------------------------------
   // store physical channel information
   //
   ptsPortV->ubPhyIf = ubPhyIfV;
   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreDriverRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverRelease(CpPort_ts * ptsPortV)
{
   CpStatus_tv tvStatusT;

   tvStatusT = CpCoreCanMode(ptsPortV, CP_MODE_STOP);
   aclCanSockListS[ptsPortV->ubPhyIf].disconnectNetwork();

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreIntFunctions()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreIntFunctions(CpPort_ts * ptsPortV,
                        uint8_t (* pfnRcvHandler)(CpCanMsg_ts *, uint8_t),
                        uint8_t (* pfnTrmHandler)(CpCanMsg_ts *, uint8_t),
                        uint8_t (* pfnErrHandler)(CpState_ts *) )
{
   QCanSocketCp *   pclSockT;

   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(CpErr_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);

   //----------------------------------------------------------------
   // store the new callbacks
   //
   pclSockT->pfnRcvIntHandlerP = pfnRcvHandler;
   pclSockT->pfnTrmIntHandlerP = pfnTrmHandler;
   pclSockT->pfnErrIntHandlerP = pfnErrHandler;


   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreMsgRead()                                                            //
// dummy implementation, use CPP_PARM_UNUSED to avoid compiler warning        //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreMsgRead( CpPort_ts *   CPP_PARM_UNUSED(ptsPortV), 
                           CpCanMsg_ts * CPP_PARM_UNUSED(ptsBufferV),
                           uint32_t *    CPP_PARM_UNUSED(pulBufferSizeV))
{
   return (CpErr_RCV_EMPTY);
}


//----------------------------------------------------------------------------//
// CpCoreStatistic()                                                          //
// return statistical information                                             //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreStatistic(CpPort_ts * ptsPortV, CpStatistic_ts * ptsStatsV)
{
   //----------------------------------------------------------------
   // avoid compiler warning
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   #endif

   ptsStatsV->ulErrMsgCount = 0;
   ptsStatsV->ulRcvMsgCount = 0;
   ptsStatsV->ulTrmMsgCount = 0;

   return(CpErr_OK);
}

QCanSocketCp::QCanSocketCp()
{
   pfnRcvIntHandlerP = 0;
   pfnTrmIntHandlerP = 0;

}

//----------------------------------------------------------------------------//
// fromCpMsg()                                                                //
// message conversion                                                         //
//----------------------------------------------------------------------------//
QCanFrame QCanSocketCp::fromCpMsg(uint8_t ubMsgBufferV)
{
   QCanFrame      clCanFrameT;
   CpCanMsg_ts *  ptsCanMsgT;
   uint8_t        ubDataCntT;

   ptsCanMsgT = &(atsCanMsgM[ubMsgBufferV]);

   if(CpMsgIsExtended(ptsCanMsgT))
   {
      clCanFrameT.setExtId(CpMsgGetExtId(ptsCanMsgT));
   }
   else
   {
      clCanFrameT.setStdId(CpMsgGetStdId(ptsCanMsgT));
   }

   clCanFrameT.setDlc(CpMsgGetDlc(ptsCanMsgT));
   for(ubDataCntT = 0; ubDataCntT < CpMsgGetDlc(ptsCanMsgT); ubDataCntT++)
   {
      clCanFrameT.setData(ubDataCntT, CpMsgGetData(ptsCanMsgT, ubDataCntT));
   }

   return(clCanFrameT);
}


//----------------------------------------------------------------------------//
// onSocketReceive()                                                          //
// receive CAN message                                                        //
//----------------------------------------------------------------------------//
void QCanSocketCp::onSocketReceive()
{
   QCanFrame   clFrameT;
   CpCanMsg_ts    tsCanMsgT;
   CpCanMsg_ts *  ptsCanBufT;
   uint32_t    ulFrameCntT;
   uint32_t    ulFrameMaxT;
   uint32_t       ulAccMaskT;
   uint8_t        ubBufferIdxT;

   ulFrameMaxT = framesAvailable();
   for(ulFrameCntT = 0; ulFrameCntT < ulFrameMaxT; ulFrameCntT++)
   {
      readFrame(clFrameT);
      tsCanMsgT = fromCanFrame(clFrameT);

      //----------------------------------------------------------------
      // run through all possible message buffer
      //
      for(ubBufferIdxT = 0; ubBufferIdxT < CP_BUFFER_MAX; ubBufferIdxT++)
      {
         //--------------------------------------------------------
         // setup pointer to CAN message buffer
         //
         ptsCanBufT = &(this->atsCanMsgM[ubBufferIdxT]);

         //--------------------------------------------------------
         // get acceptance mask
         //
         ulAccMaskT = this->atsAccMaskM[ubBufferIdxT];

         //--------------------------------------------------------
         // test direction flag
         //
         if( ((ptsCanBufT->ulMsgUser) & CP_USER_FLAG_RCV) == 0) continue;

         //--------------------------------------------------------
         // distinguish frame types
         //
         if(CpMsgIsExtended(&tsCanMsgT))
         {
            //------------------------------------------------
            // check for extended frames
            //
            if( (CpMsgGetExtId(ptsCanBufT) & ulAccMaskT) ==
                (CpMsgGetExtId(&tsCanMsgT) & ulAccMaskT)    )
            {
               //----------------------------------------
               // copy to buffer
               //
               ptsCanBufT->tuMsgId.ulExt        = tsCanMsgT.tuMsgId.ulExt;
               ptsCanBufT->ubMsgDLC             = tsCanMsgT.ubMsgDLC;
               ptsCanBufT->tuMsgData.aulLong[0] = tsCanMsgT.tuMsgData.aulLong[0];
               ptsCanBufT->tuMsgData.aulLong[1] = tsCanMsgT.tuMsgData.aulLong[1];
               if(this->pfnRcvIntHandlerP != 0)
               {
                  (* this->pfnRcvIntHandlerP)(ptsCanBufT, ubBufferIdxT + 1);
               }

            }
         }
         else
          {
             //------------------------------------------------
             // check for standard frames
             //
             if( (CpMsgGetStdId(ptsCanBufT) & ulAccMaskT) ==
                 (CpMsgGetStdId(&tsCanMsgT) & ulAccMaskT)    )
             {
                //----------------------------------------
                // copy to buffer
                //
                ptsCanBufT->tuMsgId.uwStd        = tsCanMsgT.tuMsgId.uwStd;
                ptsCanBufT->ubMsgDLC             = tsCanMsgT.ubMsgDLC;
                ptsCanBufT->tuMsgData.aulLong[0] = tsCanMsgT.tuMsgData.aulLong[0];
                ptsCanBufT->tuMsgData.aulLong[1] = tsCanMsgT.tuMsgData.aulLong[1];

                if(this->pfnRcvIntHandlerP != 0)
                {
                   (* this->pfnRcvIntHandlerP)(ptsCanBufT, ubBufferIdxT + 1);
                }

             }
          }
      }

   }
}


//----------------------------------------------------------------------------//
// fromCanFrame()                                                             //
// message conversion                                                         //
//----------------------------------------------------------------------------//
CpCanMsg_ts QCanSocketCp::fromCanFrame(QCanFrame & clCanFrameR)
{
   CpCanMsg_ts    tsCanMsgT;
   uint8_t        ubDataCntT;


   CpMsgClear(&tsCanMsgT);

   if(clCanFrameR.isExtended() == true)
   {
      CpMsgSetExtId(&tsCanMsgT, clCanFrameR.identifier());
   }
   else
   {
      CpMsgSetStdId(&tsCanMsgT, clCanFrameR.identifier());
   }

   CpMsgSetDlc(&tsCanMsgT, clCanFrameR.dlc());

   for(ubDataCntT = 0; ubDataCntT < 8; ubDataCntT++)
   {
      CpMsgSetData(&tsCanMsgT, ubDataCntT, clCanFrameR.data(ubDataCntT));
   }

   return(tsCanMsgT);
}
