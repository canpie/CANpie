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

#include "qcan_frame_api.hpp"
#include "qcan_frame_error.hpp"
#include "qcan_socket_canpie_fd.hpp"


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

enum DrvInfo_e {
   eDRV_INFO_OFF = 0,
   eDRV_INFO_INIT
};

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

static QCanSocketCpFD  aclCanSockListS[CP_CHANNEL_MAX];


/*----------------------------------------------------------------------------*\
** Function implementation                                                    **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// CheckParam()                                                               //
// check valid port, buffer number and driver state                           //
//----------------------------------------------------------------------------//
static CpStatus_tv CheckParam(const CpPort_ts * ptsPortV, 
                              const uint8_t ubBufferIdxV,
                              const uint8_t ubReqStateV )
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;
   
   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      tvStatusT = eCP_ERR_INIT_MISSING;
      
      //--------------------------------------------------------
      // check for initialisation
      //
      if (ptsPortV->ubDrvInfo >= ubReqStateV)
      {
         tvStatusT = eCP_ERR_BUFFER;
         
         //------------------------------------------------
         // check for valid buffer number
         //
         if (ubBufferIdxV < CP_BUFFER_MAX)
         {
            tvStatusT = eCP_ERR_NONE;
         }
      }
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBitrate()                                                            //
// Setup bit-rate of CAN controller                                           //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBitrate( CpPort_ts * ptsPortV, int32_t slNomBitRateV,
                           int32_t slDatBitRateV)
{
   QCanFrameApi      clFrameT;
   QCanSocketCpFD *  pclSockT;
   CpStatus_tv       tvStatusT = eCP_ERR_CHANNEL;
   
   //----------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpCoreBitrate() ............. :" << slNomBitRateV << slDatBitRateV;

   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV != (CpPort_ts *) 0L)
   {
      if(ptsPortV->ubPhyIf < QCAN_NETWORK_MAX)
      {
         pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);

         //------------------------------------------------
         // The parameter slNomBitRateV uses the same 
         // enumeration values as defined in 
         // QCan::CAN_Bitrate_e, so it can be copied.
         //
         clFrameT.setBitrate(slNomBitRateV, slDatBitRateV);
         tvStatusT = eCP_ERR_NONE;
         if(pclSockT->writeFrame(clFrameT) == false)
         {
            qDebug() << "CpCoreBitrate() ............. : Failed";
            tvStatusT =  eCP_ERR_BITRATE;
         }
      }
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferConfig()                                                       //
// Configure CAN message buffer                                               //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferConfig( CpPort_ts * ptsPortV,
                                uint8_t   ubBufferIdxV,
                                uint32_t  ulIdentifierV,
                                uint32_t  ulAcceptMaskV,
                                uint8_t   ubFormatV,
                                uint8_t   ubDirectionV)
{
   QCanSocketCpFD *  pclSockT;
   CpStatus_tv       tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);
      
      //--------------------------------------------------------
      // test message format and mask identifier
      //
      switch(ubFormatV & CP_MASK_MSG_FORMAT)
      {
         case CP_MSG_FORMAT_CBFF:
         case CP_MSG_FORMAT_FBFF:
            ulIdentifierV = ulIdentifierV & CP_MASK_STD_FRAME;
            ulAcceptMaskV = ulAcceptMaskV & CP_MASK_STD_FRAME;
            break;

         case CP_MSG_FORMAT_CEFF:
         case CP_MSG_FORMAT_FEFF:
            ulIdentifierV = ulIdentifierV & CP_MASK_EXT_FRAME;
            ulAcceptMaskV = ulAcceptMaskV & CP_MASK_EXT_FRAME;
            break;
      }  
      
      //--------------------------------------------------------
      // copy to simulated CAN buffer
      //
      pclSockT->atsCanMsgP[ubBufferIdxV].ulIdentifier  = ulIdentifierV;
      pclSockT->atsCanMsgP[ubBufferIdxV].ubMsgCtrl     = ubFormatV;
      
      //--------------------------------------------------------
      // mark Tx/Rx message
      //
      if(ubDirectionV == eCP_BUFFER_DIR_TRM)
      {
         pclSockT->atsCanMsgP[ubBufferIdxV].ulMsgUser = CP_USER_FLAG_TRM;
      }
      else
      {
         pclSockT->atsCanMsgP[ubBufferIdxV].ulMsgUser = CP_USER_FLAG_RCV;
      }

      //--------------------------------------------------------
      // set acceptance mask to default value
      //
      pclSockT->atsAccMaskP[ubBufferIdxV] = ulAcceptMaskV;   
   }


   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferGetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetData( CpPort_ts * ptsPortV,
                                 uint8_t   ubBufferIdxV,
                                 uint8_t * pubDestDataV,
                                 uint8_t   ubStartPosV,
                                 uint8_t   ubSizeV)
{
   QCanSocketCpFD *  pclSockT;
   CpStatus_tv       tvStatusT;
   uint8_t           ubCntT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);

      //--------------------------------------------------------
      // test start position and size
      //
      if ( (ubStartPosV + ubSizeV) > CP_DATA_SIZE )
      {
         tvStatusT = eCP_ERR_PARAM;
      }
      else
      {
         //---------------------------------------------------
         // copy data from simulated CAN buffer
         //
         for(ubCntT = ubStartPosV; ubCntT < ubSizeV; ubCntT++)
         {
            *pubDestDataV = CpMsgGetData(&(pclSockT->atsCanMsgP[ubBufferIdxV]), 
                                          ubCntT);
            pubDestDataV++;
         }
      }
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferGetDlc()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetDlc(  CpPort_ts * ptsPortV,
                                 uint8_t ubBufferIdxV, uint8_t * pubDlcV)
{
   QCanSocketCpFD *  pclSockT;
   CpStatus_tv       tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);
      
      //----------------------------------------------------------------
      // read DLC from simulated CAN buffer
      //
      *pubDlcV = pclSockT->atsCanMsgP[ubBufferIdxV].ubMsgDLC;   
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferRelease( CpPort_ts * ptsPortV,
                                 uint8_t ubBufferIdxV)
{
   QCanSocketCpFD * pclSockT;
   CpStatus_tv      tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);

      //--------------------------------------------------------
      // clear simulated CAN buffer
      //
      pclSockT->atsCanMsgP[ubBufferIdxV].ulIdentifier = 0;
      pclSockT->atsCanMsgP[ubBufferIdxV].ubMsgDLC     = 0;
      pclSockT->atsCanMsgP[ubBufferIdxV].ubMsgCtrl    = 0;
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSend()                                                         //
// send message out of the CAN controller                                     //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSend(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   QCanFrame        clFrameT;
   QCanSocketCpFD * pclSockT;
   CpStatus_tv      tvStatusT;
   CpTrmHandler_Fn  pfnTrmHandlerT;
   
   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);
      
      //----------------------------------------------------------------
      // write CAN frame
      //
      clFrameT = pclSockT->fromCpMsg(ubBufferIdxV);
      if(pclSockT->writeFrame(clFrameT) == false)
      {
         tvStatusT = eCP_ERR_TRM_FULL;
         qDebug() << "Failed to write message";
      }
      else
      {
         pclSockT->tsStatisticP.ulTrmMsgCount++;

         if (pclSockT->pfnTrmIntHandlerP != 0)
         {
            pfnTrmHandlerT = pclSockT->pfnTrmIntHandlerP;
            tvStatusT = (* pfnTrmHandlerT)(&(pclSockT->atsCanMsgP[ubBufferIdxV]),
                                           ubBufferIdxV);
         }
      }
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSetData( CpPort_ts * ptsPortV,
                                 uint8_t ubBufferIdxV,  uint8_t * pubSrcDataV,
                                 uint8_t   ubStartPosV, uint8_t   ubSizeV)
{
   QCanSocketCpFD *  pclSockT;
   CpStatus_tv       tvStatusT;
   uint8_t           ubCntT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);

      //--------------------------------------------------------
      // test start position and size
      //
      if ( (ubStartPosV + ubSizeV) > CP_DATA_SIZE )
      {
         tvStatusT = eCP_ERR_PARAM;
      }
      else
      {
         //---------------------------------------------------
         // copy data from simulated CAN buffer
         //
         for(ubCntT = ubStartPosV; ubCntT < ubSizeV; ubCntT++)
         {
            CpMsgSetData(&(pclSockT->atsCanMsgP[ubBufferIdxV]), 
                         ubCntT, *pubSrcDataV);
            pubSrcDataV++;
         }
      }
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSetDlc()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSetDlc(  CpPort_ts * ptsPortV,
                                 uint8_t ubBufferIdxV, uint8_t ubDlcV)
{
   QCanSocketCpFD *  pclSockT;
   CpStatus_tv       tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);
      
      //--------------------------------------------------------
      // write DLC to simulated CAN buffer
      //
      pclSockT->atsCanMsgP[ubBufferIdxV].ubMsgDLC = ubDlcV;
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreCanMode()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanMode(CpPort_ts * ptsPortV, uint8_t ubModeV)
{
   QCanFrameApi      clFrameT;
   QCanSocketCpFD *  pclSockT;
   CpStatus_tv       tvStatusT = eCP_ERR_CHANNEL;

   
   //----------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpCoreMode() ........ :" << ubModeV;

   //----------------------------------------------------------------
   // get access to socket
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubPhyIf < QCAN_NETWORK_MAX)
      {
         pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);

         //-----------------------------------------------------
         // switch CAN controller into mode "ubModeV"
         //
         switch(ubModeV)
         {
            //---------------------------------------------
            // Stop the CAN controller (passive on the bus)
            //
            case eCP_MODE_STOP:
               clFrameT.setMode(eCAN_MODE_STOP);
               tvStatusT = eCP_ERR_NONE;
               break;

            //---------------------------------------------
            // Start the CAN controller (active on the bus)
            //
            case eCP_MODE_START:
               pclSockT->tsCanStateP.ubCanErrState  = eCP_STATE_BUS_ACTIVE;
               clFrameT.setMode(eCAN_MODE_START);
               tvStatusT = eCP_ERR_NONE;
               break;

            //---------------------------------------------
            // Start the CAN controller (Listen-Only)
            //
            case eCP_MODE_LISTEN_ONLY:
               clFrameT.setMode(eCAN_MODE_LISTEN_ONLY);
               tvStatusT = eCP_ERR_NONE;
               break;

            //---------------------------------------------
            // Other modes are not supported
            //
            default:
               tvStatusT = eCP_ERR_NOT_SUPPORTED;
               break;
         }

      }
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreCanState()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanState(CpPort_ts * ptsPortV, CpState_ts * ptsStateV)
{
   QCanSocketCpFD *  pclSockT;
   CpStatus_tv       tvStatusT = eCP_ERR_CHANNEL;
   

   //----------------------------------------------------------------
   // get access to socket
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubPhyIf < QCAN_NETWORK_MAX)
      {
         pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);

         tvStatusT = eCP_ERR_PARAM;
         
         if (ptsStateV != (CpState_ts *) 0L)
         {
            //---------------------------------------------
            // copy current CAN state
            //
            ptsStateV->ubCanErrState = pclSockT->tsCanStateP.ubCanErrState;
            ptsStateV->ubCanErrType  = pclSockT->tsCanStateP.ubCanErrType;

            //---------------------------------------------
            // copy current error counter
            //
            ptsStateV->ubCanRcvErrCnt = pclSockT->tsCanStateP.ubCanRcvErrCnt;
            ptsStateV->ubCanTrmErrCnt = pclSockT->tsCanStateP.ubCanTrmErrCnt;
            
            tvStatusT = eCP_ERR_NONE;
         }
      }
   }
   
   return(tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreDriverInit()                                                         //
// init CAN controller                                                        //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverInit(uint8_t ubPhyIfV, CpPort_ts * ptsPortV,
                             uint8_t ubConfigV)
{
   QCanSocketCpFD *   pclSockT;

   //----------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpCoreDriverInit() .......... :" << ubPhyIfV << ubConfigV;

   //----------------------------------------------------------------
   // test parameter
   //
   if ((ubPhyIfV > QCAN_NETWORK_MAX) || (ubPhyIfV == eCAN_CHANNEL_NONE))
   {
      return(eCP_ERR_CHANNEL);
   }

   //----------------------------------------------------------------
   // get access to socket
   //
   pclSockT = &(aclCanSockListS[ubPhyIfV - 1]);
   pclSockT->connectNetwork((CAN_Channel_e) ubPhyIfV, 1000);

   //----------------------------------------------------------------
   // no FIFOs attached to message buffers
   //
   for (uint8_t ubBufferCntT = 0; ubBufferCntT < CP_BUFFER_MAX; ubBufferCntT++)
   {
      pclSockT->aptsCanFifoP[ubBufferCntT] = Q_NULLPTR;
   }
   
   //----------------------------------------------------------------
   // store physical channel information
   //
   ptsPortV->ubPhyIf   = ubPhyIfV;
   ptsPortV->ubDrvInfo = eDRV_INFO_INIT;
   
   //----------------------------------------------------------------
   // clear CAN state
   //
   pclSockT->tsCanStateP.ubCanErrState  = eCP_STATE_INIT;
   pclSockT->tsCanStateP.ubCanErrType   = eCP_ERR_TYPE_NONE;
   pclSockT->tsCanStateP.ubCanRcvErrCnt = 0;
   pclSockT->tsCanStateP.ubCanTrmErrCnt = 0;
   
   //----------------------------------------------------------------
   // clear statistic information
   //
   pclSockT->tsStatisticP.ulErrMsgCount = 0;
   pclSockT->tsStatisticP.ulRcvMsgCount = 0;
   pclSockT->tsStatisticP.ulTrmMsgCount = 0;
   

   qDebug() << "CpCoreDriverInit() .......... : connected";
   return (eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreDriverRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverRelease(CpPort_ts * ptsPortV)
{
   CpStatus_tv tvStatusT;

   tvStatusT = CpCoreCanMode(ptsPortV, eCP_MODE_STOP);
   aclCanSockListS[(ptsPortV->ubPhyIf) - 1].disconnectNetwork();

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreFifoConfig()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoConfig(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                             CpFifo_ts * ptsFifoV)
{
   CpStatus_tv       tvStatusT;
   QCanSocketCpFD *  pclSockT;
   
   
   qDebug() << "CpCoreFifoConfig()........... :" << ubBufferIdxV;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);
      
      pclSockT->aptsCanFifoP[ubBufferIdxV] = ptsFifoV;
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreFifoRead()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoRead(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                           CpCanMsg_ts * ptsCanMsgV,
                           uint32_t * pulMsgCntV)
{
   CpStatus_tv       tvStatusT;
   QCanSocketCpFD *  pclSockT;
   CpFifo_ts *       ptsFifoT;
   CpCanMsg_ts *     ptsFifoMsgT;
   
   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);
      ptsFifoT = pclSockT->aptsCanFifoP[ubBufferIdxV];
      
      if (ptsFifoT == Q_NULLPTR)
      {
         tvStatusT = eCP_ERR_FIFO_PARAM;
      }
      else
      {
         if(CpFifoIsEmpty(ptsFifoT) == 1)
         {
            //--------------------------------------------------------
            // FIFO is empty, no data has been copied
            //
            *pulMsgCntV = 0;
            tvStatusT = eCP_ERR_FIFO_EMPTY;
         }
         else
         {
            ptsFifoMsgT = CpFifoDataOutPtr(ptsFifoT);
            memcpy(ptsCanMsgV, ptsFifoMsgT, sizeof(CpCanMsg_ts));
            CpFifoIncOut(ptsFifoT);
            *pulMsgCntV = 1;
            tvStatusT = eCP_ERR_NONE;
         }
      }
   }
   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreFifoRelease()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoRelease(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   CpStatus_tv       tvStatusT;
   QCanSocketCpFD *  pclSockT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);
      pclSockT->aptsCanFifoP[ubBufferIdxV] = Q_NULLPTR;
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreFifoWrite()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoWrite(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                            CpCanMsg_ts * ptsCanMsgV,
                            uint32_t * pulMsgCntV)
{
   QCanFrame         clFrameT;
   QCanSocketCpFD *  pclSockT;
   CpStatus_tv       tvStatusT;
   uint32_t          ulMsgCntT;
   CpTrmHandler_Fn   pfnTrmHandlerT;
   
   
   qDebug() << "CpCoreFifoWrite() ........... :" << ubBufferIdxV;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      qDebug() << "CpCoreFifoWrite() ........... :" << ubBufferIdxV;

      pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);
      
      //----------------------------------------------------------------
      // write CAN frame
      //
      for (ulMsgCntT = 0; ulMsgCntT < *pulMsgCntV; ulMsgCntT++)
      {
         clFrameT = pclSockT->fromCpMsg(ptsCanMsgV);
         if(pclSockT->writeFrame(clFrameT) == false)
         {
            tvStatusT = eCP_ERR_TRM_FULL;
            qDebug() << "CpCoreFifoWrite() ........... : failed";
            break;
         }

         if (pclSockT->pfnTrmIntHandlerP != 0)
         {
            pfnTrmHandlerT = pclSockT->pfnTrmIntHandlerP;
            tvStatusT = (* pfnTrmHandlerT)(ptsCanMsgV, ubBufferIdxV);
         }

         ptsCanMsgV++;
      }
      *pulMsgCntV = ulMsgCntT;   // store number of messages written

      pclSockT->tsStatisticP.ulTrmMsgCount += ulMsgCntT;
   }
   
   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreHDI()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreHDI(CpPort_ts *ptsPortV, CpHdi_ts *ptsHdiV)
{
   QCanSocketCpFD *  pclSockT;
   CpStatus_tv       tvStatusT = eCP_ERR_CHANNEL;
   

   //----------------------------------------------------------------
   // get access to socket
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubPhyIf < QCAN_NETWORK_MAX)
      {
         pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);
         ptsHdiV->ubVersionMajor   = CP_VERSION_MAJOR;
         ptsHdiV->ubVersionMinor   = CP_VERSION_MINOR;
         ptsHdiV->ubCanFeatures    = 0;
         ptsHdiV->ubDriverFeatures = 0;
         ptsHdiV->ubBufferMax      = CP_BUFFER_MAX;
         ptsHdiV->slNomBitRate     = pclSockT->slNomBitRateP;
         ptsHdiV->slDatBitRate     = pclSockT->slDatBitRateP;
         
         tvStatusT = eCP_ERR_NONE;
      }
   }
   
   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreIntFunctions()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreIntFunctions( CpPort_ts * ptsPortV,
                        uint8_t (* pfnRcvHandler)(CpCanMsg_ts *, uint8_t),
                        uint8_t (* pfnTrmHandler)(CpCanMsg_ts *, uint8_t),
                        uint8_t (* pfnErrHandler)(CpState_ts *) )
{
   QCanSocketCpFD *  pclSockT;
   CpStatus_tv       tvStatusT = eCP_ERR_CHANNEL;
   

   //----------------------------------------------------------------
   // get access to socket
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubPhyIf < QCAN_NETWORK_MAX)
      {
         pclSockT = &(aclCanSockListS[(ptsPortV->ubPhyIf) - 1]);

         //------------------------------------------------
         // store the new callbacks
         //
         pclSockT->pfnRcvIntHandlerP = pfnRcvHandler;
         pclSockT->pfnTrmIntHandlerP = pfnTrmHandler;
         pclSockT->pfnErrIntHandlerP = pfnErrHandler;

         tvStatusT = eCP_ERR_NONE;
      }
   }
   
   return (tvStatusT);
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
      return(eCP_ERR_PARAM);
   }
   #endif

   ptsStatsV->ulErrMsgCount = 0;
   ptsStatsV->ulRcvMsgCount = 0;
   ptsStatsV->ulTrmMsgCount = 0;

   return(eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreBitrate()                                                            //
// Setup bit-rare of CAN controller                                           //
//----------------------------------------------------------------------------//
CpStatus_tv CpSocketConnectSlots(uint8_t ubPhyIfV, QObject * pclDestObjectV,
                               const char * pubSockConnectV,
                               const char * pubSockDisconnectV,
                               const char * pubSockErrorV)
{
   QCanSocketCpFD *   pclSockT;

   //----------------------------------------------------------------
   // test parameter
   //
   if ((ubPhyIfV > QCAN_NETWORK_MAX) || (ubPhyIfV == eCAN_CHANNEL_NONE))
   {
      return(eCP_ERR_CHANNEL);
   }

   
   //----------------------------------------------------------------
   // get pointer to socket
   //
   pclSockT = &(aclCanSockListS[ubPhyIfV - 1]);

   if(pubSockConnectV != Q_NULLPTR)
   {
      QObject::connect( pclSockT, SIGNAL(connected()),
                        pclDestObjectV, pubSockConnectV);
   }

   if(pubSockDisconnectV != Q_NULLPTR)
   {
      QObject::connect( pclSockT, SIGNAL(disconnected()),
                        pclDestObjectV, pubSockDisconnectV);
   }
   
   if(pubSockErrorV != Q_NULLPTR)
   {
      QObject::connect( pclSockT, 
                        SIGNAL(error(QAbstractSocket::SocketError)),
                        pclDestObjectV, pubSockErrorV);
   }
   return (eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpSocketSetHostAddress()                                                   //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpSocketSetHostAddress(uint8_t ubPhyIfV, 
                                   QHostAddress clHostAddressV)
{
   //----------------------------------------------------------------
   // test parameter
   //
   if ((ubPhyIfV > QCAN_NETWORK_MAX) || (ubPhyIfV == eCAN_CHANNEL_NONE))
   {
      return(eCP_ERR_CHANNEL);
   }


   aclCanSockListS[ubPhyIfV - 1].setHostAddress(clHostAddressV);
   return (eCP_ERR_NONE);

}


//----------------------------------------------------------------------------//
// QCanSocketCpFD                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
QCanSocketCpFD::QCanSocketCpFD()
{
   pfnRcvIntHandlerP = 0;
   pfnTrmIntHandlerP = 0;

   ubStatusP = 0;
}


//----------------------------------------------------------------------------//
// fromCanFrame()                                                             //
// message conversion                                                         //
//----------------------------------------------------------------------------//
CpCanMsg_ts QCanSocketCpFD::fromCanFrame(QCanFrame & clCanFrameR)
{
   CpCanMsg_ts    tsCanMsgT;
   uint8_t        ubDataCntT;



   switch(clCanFrameR.frameFormat())
   {
      case QCanFrame::eFORMAT_CAN_STD:
         CpMsgInit(&tsCanMsgT, CP_MSG_FORMAT_CBFF);
         break;

      case QCanFrame::eFORMAT_CAN_EXT:
         CpMsgInit(&tsCanMsgT, CP_MSG_FORMAT_CEFF);
         break;
         
      case QCanFrame::eFORMAT_FD_STD:
         CpMsgInit(&tsCanMsgT, CP_MSG_FORMAT_FBFF);
         break;
         
      case QCanFrame::eFORMAT_FD_EXT:
         CpMsgInit(&tsCanMsgT, CP_MSG_FORMAT_FEFF);
         break;
   }
   
   CpMsgSetIdentifier(&tsCanMsgT, clCanFrameR.identifier());
   CpMsgSetDlc(&tsCanMsgT, clCanFrameR.dlc());

   for(ubDataCntT = 0; ubDataCntT < 64; ubDataCntT++)
   {
      CpMsgSetData(&tsCanMsgT, ubDataCntT, clCanFrameR.data(ubDataCntT));
   }
   return(tsCanMsgT);
}


//----------------------------------------------------------------------------//
// fromCpMsg()                                                                //
// message conversion                                                         //
//----------------------------------------------------------------------------//
QCanFrame QCanSocketCpFD::fromCpMsg(CpCanMsg_ts * ptsCanMsgV)
{
   QCanFrame      clCanFrameT;
   uint8_t        ubDataCntT;

   if(CpMsgIsFastData(ptsCanMsgV))
   {
      if(CpMsgIsExtended(ptsCanMsgV))
      {
         clCanFrameT.setFrameFormat(QCanFrame::eFORMAT_FD_EXT);
      }
      else
      {
         clCanFrameT.setFrameFormat(QCanFrame::eFORMAT_FD_STD);
      }
   }
   else
   {
      if(CpMsgIsExtended(ptsCanMsgV))
      {
         clCanFrameT.setFrameFormat(QCanFrame::eFORMAT_CAN_EXT);
      }
      else
      {
         clCanFrameT.setFrameFormat(QCanFrame::eFORMAT_CAN_STD);
      }
   }
   clCanFrameT.setIdentifier(CpMsgGetIdentifier(ptsCanMsgV));
   clCanFrameT.setDlc(CpMsgGetDlc(ptsCanMsgV));

   for(ubDataCntT = 0; ubDataCntT < clCanFrameT.dataSize(); ubDataCntT++)
   {
      clCanFrameT.setData(ubDataCntT, CpMsgGetData(ptsCanMsgV, ubDataCntT));
   }

   return(clCanFrameT);
}


//----------------------------------------------------------------------------//
// fromCpMsg()                                                                //
// message conversion                                                         //
//----------------------------------------------------------------------------//
QCanFrame QCanSocketCpFD::fromCpMsg(uint8_t ubMsgBufferV)
{
   QCanFrame      clCanFrameT;
   CpCanMsg_ts *  ptsCanMsgT;
   uint8_t        ubDataCntT;

   ptsCanMsgT = &(atsCanMsgP[ubMsgBufferV]);

   if(CpMsgIsFastData(ptsCanMsgT))
   {
      if(CpMsgIsExtended(ptsCanMsgT))
      {
         clCanFrameT.setFrameFormat(QCanFrame::eFORMAT_FD_EXT);
      }
      else
      {
         clCanFrameT.setFrameFormat(QCanFrame::eFORMAT_FD_STD);
      }
   }
   else
   {
      if(CpMsgIsExtended(ptsCanMsgT))
      {
         clCanFrameT.setFrameFormat(QCanFrame::eFORMAT_CAN_EXT);
      }
      else
      {
         clCanFrameT.setFrameFormat(QCanFrame::eFORMAT_CAN_STD);
      }
   }
   clCanFrameT.setIdentifier(CpMsgGetIdentifier(ptsCanMsgT));
   clCanFrameT.setDlc(CpMsgGetDlc(ptsCanMsgT));

   for(ubDataCntT = 0; ubDataCntT < clCanFrameT.dataSize(); ubDataCntT++)
   {
      clCanFrameT.setData(ubDataCntT, CpMsgGetData(ptsCanMsgT, ubDataCntT));
   }

   return(clCanFrameT);
}


//----------------------------------------------------------------------------//
// handleApiFrame()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketCpFD::handleApiFrame(QCanFrameApi & clApiFrameR)
{
   Q_UNUSED(clApiFrameR);
}


//----------------------------------------------------------------------------//
// handleCanFrame()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketCpFD::handleCanFrame(QCanFrame & clCanFrameR)
{
   CpCanMsg_ts    tsCanMsgT;
   CpCanMsg_ts *  ptsCanBufT;
   CpFifo_ts *    ptsFifoT;
   uint32_t       ulAccMaskT;
   uint8_t        ubBufferIdxT;

   tsCanMsgT = fromCanFrame(clCanFrameR);

   //----------------------------------------------------------------
   // run through all possible message buffer
   //
   for (ubBufferIdxT = 0; ubBufferIdxT < CP_BUFFER_MAX; ubBufferIdxT++)
   {
      //--------------------------------------------------------
      // setup pointer to CAN message buffer
      //
      ptsCanBufT = &(this->atsCanMsgP[ubBufferIdxT]);

      //--------------------------------------------------------
      // get acceptance mask
      //
      ulAccMaskT = this->atsAccMaskP[ubBufferIdxT];

      //--------------------------------------------------------
      // test direction flag
      //
      if ( ((ptsCanBufT->ulMsgUser) & CP_USER_FLAG_RCV) == 0) continue;

      //--------------------------------------------------------
      // distinguish frame types
      //
      if (CpMsgIsExtended(ptsCanBufT) == CpMsgIsExtended(&tsCanMsgT))
      {
         //------------------------------------------------
         // check for identifier
         //
         if( (CpMsgGetIdentifier(ptsCanBufT) & ulAccMaskT) ==
             (CpMsgGetIdentifier(&tsCanMsgT) & ulAccMaskT)    )
         {
            //----------------------------------------
            // copy to buffer
            //
            ptsCanBufT->ulIdentifier   = tsCanMsgT.ulIdentifier;
            ptsCanBufT->ubMsgDLC       = tsCanMsgT.ubMsgDLC;
            memcpy(&(ptsCanBufT->aubData[0]),
                   &(tsCanMsgT.aubData[0]),
                   CP_DATA_SIZE );
            
            //----------------------------------------
            // test for receive callback handler
            //
            if (this->aptsCanFifoP[ubBufferIdxT] == 0L)
            {
               //--------------------------------
               // no FIFO available
               //
               if (this->pfnRcvIntHandlerP != 0)
               {
                  (* this->pfnRcvIntHandlerP)(ptsCanBufT, ubBufferIdxT);
               }
            }
            else
            {
               ptsFifoT = this->aptsCanFifoP[ubBufferIdxT];
               if (CpFifoIsFull(ptsFifoT) == 0)
               {
                  ptsCanBufT = CpFifoDataInPtr(ptsFifoT);
                  memcpy(ptsCanBufT, &tsCanMsgT, sizeof(CpCanMsg_ts));
                  CpFifoIncIn(ptsFifoT);
               }
            }

            //-----------------------------------------
            // update statistic
            //
            tsStatisticP.ulRcvMsgCount++;
         }
      }
   }
}


//----------------------------------------------------------------------------//
// handleErrFrame()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void  QCanSocketCpFD::handleErrFrame(QCanFrameError & clErrFrameR)
{
   tsCanStateP.ubCanErrState  = clErrFrameR.errorState();
   tsCanStateP.ubCanErrType   = clErrFrameR.errorType();
   tsCanStateP.ubCanRcvErrCnt = clErrFrameR.errorCounterReceive();
   tsCanStateP.ubCanTrmErrCnt = clErrFrameR.errorCounterTransmit();
   
   tsStatisticP.ulErrMsgCount++;
}


//----------------------------------------------------------------------------//
// onSocketReceive()                                                          //
// receive CAN message                                                        //
//----------------------------------------------------------------------------//
void  QCanSocketCpFD::onSocketConnect(void)
{
   qDebug() << "QCanSocketCpFD::onSocketConnect()";
   ubStatusP = 1;
}

//----------------------------------------------------------------------------//
// onSocketReceive()                                                          //
// receive CAN message                                                        //
//----------------------------------------------------------------------------//
void  QCanSocketCpFD::onSocketDisconnect(void)
{
   qDebug() << "QCanSocketCpFD::onSocketDisconnect()";
   ubStatusP = 2;
}


//----------------------------------------------------------------------------//
// onSocketReceive()                                                          //
// receive CAN message                                                        //
//----------------------------------------------------------------------------//
void QCanSocketCpFD::onSocketReceive()
{
   QByteArray        clCanDataT;
   QCanFrame         clCanFrameT;
   QCanFrameApi      clCanApiT;
   QCanFrameError    clCanErrorT;
   QCanData::Type_e  ubFrameTypeT;
   uint32_t          ulFrameCntT;
   uint32_t          ulFrameMaxT;


   ulFrameMaxT = framesAvailable();
   for(ulFrameCntT = 0; ulFrameCntT < ulFrameMaxT; ulFrameCntT++)
   {
      if (this->read(clCanDataT, &ubFrameTypeT) == true)
      {
         switch (ubFrameTypeT)
         {
            case QCanData::eTYPE_API:
               if (clCanApiT.fromByteArray(clCanDataT) == true)
               {
                  handleApiFrame(clCanApiT);
               }
               break;
               
            case QCanData::eTYPE_CAN:
               if (clCanFrameT.fromByteArray(clCanDataT) == true)
               {
                  handleCanFrame(clCanFrameT);
               }
               break;
               
            case QCanData::eTYPE_ERROR:
               if (clCanErrorT.fromByteArray(clCanDataT) == true)
               {
                  handleErrFrame(clCanErrorT);
               }
               break;
            default:

               break;
               
         }
      }
   }
}





