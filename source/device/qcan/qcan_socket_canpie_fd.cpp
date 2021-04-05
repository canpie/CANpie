//====================================================================================================================//
// File:          qcan_socket_canpie_fd.cpp                                                                           //
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

#include "qcan_socket_canpie_fd.hpp"

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//---------------------------------------------------------------------------------------------------
/*!
** \def     CP_DRIVER_MAJOR
** \ingroup CP_VERSION
**
** Major version number of CANpie driver implementation.
*/
#define  CP_DRIVER_MAJOR          1

//---------------------------------------------------------------------------------------------------
/*!
** \def     CP_DRIVER_MINOR
** \ingroup CP_VERSION
**
** Minor version number of CANpie driver implementation.
*/
#define  CP_DRIVER_MINOR          4


#define  CP_USER_FLAG_RCV        ((uint32_t)(0x00000001))
#define  CP_USER_FLAG_TRM        ((uint32_t)(0x00000002))


enum DrvInfo_e {
   eDRV_INFO_OFF = 0,
   eDRV_INFO_INIT
};


/*--------------------------------------------------------------------------------------------------------------------*\
** Variables of module                                                                                                **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------------------------------
//
static QCoreApplication *  pclApplicationS = 0L;


//------------------------------------------------------------------------------------------------------
//
static QCanSocketCpFD      aclCanSockListS[QCAN_NETWORK_MAX];


/*--------------------------------------------------------------------------------------------------------------------*\
** Function implementation                                                                                            **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// CheckParam()                                                                                                       //
// check valid port, buffer number and driver state                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
static CpStatus_tv CheckParam(const CpPort_ts * ptsPortV, const uint8_t ubBufferIdxV, const uint8_t ubReqStateV )
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;
   
   //---------------------------------------------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      tvStatusT = eCP_ERR_INIT_MISSING;
      
      //-------------------------------------------------------------------------------------------
      // check for initialisation
      //
      if (ptsPortV->ubDrvInfo >= ubReqStateV)
      {
         tvStatusT = eCP_ERR_BUFFER;
         
         //-----------------------------------------------------------------------------------
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


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreBitrate()                                                                                                    //
// Setup bit-rate of CAN controller                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreBitrate( CpPort_ts * ptsPortV, int32_t slNomBitRateV, int32_t slDatBitRateV)
{
   // QCanSocketCpFD *  pclSockT;
   CpStatus_tv       tvStatusT = eCP_ERR_CHANNEL;
   
   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpCoreBitrate() ............. :" << slNomBitRateV << slDatBitRateV;

   //---------------------------------------------------------------------------------------------------
   // get access to socket
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubPhyIf < QCAN_NETWORK_MAX)
      {
         //----------------------------------------------------------------------------------------
         // This function does not modify the bitrate settings of the CANpie FD server, because
         // alter the bitrate settings might be denied by the server. So we just return here.
         // 
         tvStatusT = eCP_ERR_NONE;
      }
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreBufferConfig()                                                                                               //
// Configure CAN message buffer                                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferConfig( CpPort_ts * ptsPortV, uint8_t   ubBufferIdxV,
                                uint32_t  ulIdentifierV, uint32_t  ulAcceptMaskV,
                                uint8_t   ubFormatV, uint8_t   ubDirectionV)
{
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   CpStatus_tv       tvStatusT;

   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }

   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if ( pclSockT != (QCanSocketCpFD *) 0L ) 
   {
      
      //-------------------------------------------------------------------------------------------
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
      
      //-------------------------------------------------------------------------------------------
      // copy to simulated CAN buffer
      //
      pclSockT->atsCanMsgP[ubBufferIdxV].ulIdentifier  = ulIdentifierV;
      pclSockT->atsCanMsgP[ubBufferIdxV].ubMsgCtrl     = ubFormatV;
      
      //-------------------------------------------------------------------------------------------
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

      //-------------------------------------------------------------------------------------------
      // set acceptance mask to default value
      //
      pclSockT->atsAccMaskP[ubBufferIdxV] = ulAcceptMaskV;   
   }


   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreBufferGetData()                                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetData( CpPort_ts * ptsPortV, uint8_t   ubBufferIdxV,
                                 uint8_t * pubDestDataV, uint8_t   ubStartPosV, uint8_t   ubSizeV)
{
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   CpStatus_tv       tvStatusT;
   uint8_t           ubCntT;

   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }

   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if ( pclSockT != (QCanSocketCpFD *) 0L ) 
   {

      //-------------------------------------------------------------------------------------------
      // test start position and size
      //
      if ( (ubStartPosV + ubSizeV) > CP_DATA_SIZE )
      {
         tvStatusT = eCP_ERR_PARAM;
      }
      else
      {
         //-----------------------------------------------------------------------------------
         // copy data from simulated CAN buffer
         //
         for(ubCntT = ubStartPosV; ubCntT < (ubStartPosV + ubSizeV); ubCntT++)
         {
            *pubDestDataV = CpMsgGetData(&(pclSockT->atsCanMsgP[ubBufferIdxV]), ubCntT);
            pubDestDataV++;
         }
      }
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreBufferGetDlc()                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetDlc(  CpPort_ts * ptsPortV, uint8_t ubBufferIdxV, uint8_t * pubDlcV)
{
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   CpStatus_tv       tvStatusT;

   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }

   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if ( pclSockT != (QCanSocketCpFD *) 0L ) 
   {
      
      //-------------------------------------------------------------------------------------------
      // read DLC from simulated CAN buffer
      //
      *pubDlcV = pclSockT->atsCanMsgP[ubBufferIdxV].ubMsgDLC;   
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreBufferRelease()                                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferRelease( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   CpStatus_tv       tvStatusT;

   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }

   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if ( pclSockT != (QCanSocketCpFD *) 0L ) 
   {

      //-------------------------------------------------------------------------------------------
      // clear simulated CAN buffer
      //
      pclSockT->atsCanMsgP[ubBufferIdxV].ulIdentifier = 0;
      pclSockT->atsCanMsgP[ubBufferIdxV].ubMsgDLC     = 0;
      pclSockT->atsCanMsgP[ubBufferIdxV].ubMsgCtrl    = 0;

      CpCoreFifoRelease(ptsPortV, ubBufferIdxV);
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreBufferSend()                                                                                                 //
// send message out of the CAN controller                                                                             //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSend(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   QCanFrame         clFrameT;
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   CpStatus_tv       tvStatusT;
   CpTrmHandler_Fn   pfnTrmHandlerT;
   
   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }

   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if ( pclSockT != (QCanSocketCpFD *) 0L ) 
   {
      //-------------------------------------------------------------------------------------------
      // write CAN frame
      //
      clFrameT = pclSockT->fromCpMsg(ubBufferIdxV);
      if(pclSockT->write(clFrameT) == false)
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
            tvStatusT = (* pfnTrmHandlerT)(&(pclSockT->atsCanMsgP[ubBufferIdxV]), ubBufferIdxV);
         }
      }
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreBufferSetData()                                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSetData( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,  uint8_t * pubSrcDataV,
                                 uint8_t   ubStartPosV, uint8_t   ubSizeV)
{
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   CpStatus_tv       tvStatusT;
   uint8_t           ubCntT;

   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }

   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if ( pclSockT != (QCanSocketCpFD *) 0L ) 
   {

      //-------------------------------------------------------------------------------------------
      // test start position and size
      //
      if ( (ubStartPosV + ubSizeV) > CP_DATA_SIZE )
      {
         tvStatusT = eCP_ERR_PARAM;
      }
      else
      {
         //-----------------------------------------------------------------------------------
         // copy data from simulated CAN buffer
         //
         for (ubCntT = ubStartPosV; ubCntT < (ubStartPosV + ubSizeV); ubCntT++)
         {
            CpMsgSetData(&(pclSockT->atsCanMsgP[ubBufferIdxV]), ubCntT, *pubSrcDataV);
            pubSrcDataV++;
         }
      }
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreBufferSetDlc()                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSetDlc(  CpPort_ts * ptsPortV, uint8_t ubBufferIdxV, uint8_t ubDlcV)
{
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   CpStatus_tv       tvStatusT;

   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }

   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if ( pclSockT != (QCanSocketCpFD *) 0L ) 
   {
      
      //-------------------------------------------------------------------------------------------
      // write DLC to simulated CAN buffer
      //
      pclSockT->atsCanMsgP[ubBufferIdxV].ubMsgDLC = ubDlcV;
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreCanMode()                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreCanMode(CpPort_ts * ptsPortV, uint8_t ubModeV)
{
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   CpStatus_tv       tvStatusT = eCP_ERR_CHANNEL;

   
   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpCoreCanMode() ............. :" << ubModeV;

   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV 
   //
   tvStatusT = CheckParam(ptsPortV, eCP_BUFFER_1, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }

   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if ( pclSockT != (QCanSocketCpFD *) 0L ) 
   {
      //-------------------------------------------------------------------------------------------
      // switch CAN controller into mode "ubModeV"
      //
      switch(ubModeV)
      {
         //-----------------------------------------------------------------------------------
         // Stop the CAN controller (passive on the bus)
         //
         case eCP_MODE_INIT:
            pclSockT->tsCanStateP.ubCanErrState  = eCP_STATE_INIT;
            tvStatusT = eCP_ERR_NONE;
            break;

         //-----------------------------------------------------------------------------------
         // Start the CAN controller (active on the bus)
         //
         case eCP_MODE_START:
            pclSockT->tsCanStateP.ubCanErrState  = eCP_STATE_BUS_ACTIVE;
            tvStatusT = eCP_ERR_NONE;
            break;

         //-----------------------------------------------------------------------------------
         // Start the CAN controller (Listen-Only)
         //
         case eCP_MODE_LISTEN_ONLY:
            pclSockT->tsCanStateP.ubCanErrState  = eCP_STATE_BUS_ACTIVE;
            tvStatusT = eCP_ERR_NONE;
            break;

         //-----------------------------------------------------------------------------------
         // Other modes are not supported
         //
         default:
            tvStatusT = eCP_ERR_NOT_SUPPORTED;
            break;
      }

   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreCanState()                                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreCanState(CpPort_ts * ptsPortV, CpState_ts * ptsStateV)
{
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   CpStatus_tv       tvStatusT = eCP_ERR_CHANNEL;
   
   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV 
   //
   tvStatusT = CheckParam(ptsPortV, eCP_BUFFER_1, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }


   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if ( pclSockT != (QCanSocketCpFD *) 0L ) 
   {
      tvStatusT = eCP_ERR_PARAM;

      if ( ptsStateV != (CpState_ts *) 0L )
      {
         //----------------------------------------------------------------------------------------
         // copy current CAN state
         //
         ptsStateV->ubCanErrState = pclSockT->tsCanStateP.ubCanErrState;
         ptsStateV->ubCanErrType  = pclSockT->tsCanStateP.ubCanErrType;

         //----------------------------------------------------------------------------------------
         // copy current error counter
         //
         ptsStateV->ubCanRcvErrCnt = pclSockT->tsCanStateP.ubCanRcvErrCnt;
         ptsStateV->ubCanTrmErrCnt = pclSockT->tsCanStateP.ubCanTrmErrCnt;
            
         tvStatusT = eCP_ERR_NONE;
      }
   }
   
   return(tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreDriverInit()                                                                                                 //
// init CAN controller                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverInit(uint8_t ubPhyIfV, CpPort_ts * ptsPortV, uint8_t ubConfigV)
{
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   CpStatus_tv       tvStatusT = eCP_ERR_GENERIC;

   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpCoreDriverInit() .......... :" << ubPhyIfV << ubConfigV;

   //---------------------------------------------------------------------------------------------------
   // Check if an instance of QCoreApplication is active. In case there is no QCoreApplication there
   // is no QEventLoop and we need a thread that polls data from the CANpie server.
   //
   if (pclApplicationS == 0L)
   {
      if (QCoreApplication::instance() == 0L)
      {
         int32_t slAppArgcT = 0;
         char ** szAppArgvT = NULL;
         pclApplicationS = new QCoreApplication(slAppArgcT, szAppArgvT);
      
         qDebug() << "QCanSocketCpFD() ............ : create new QCoreApplication";
      }
   }


   //---------------------------------------------------------------------------------------------------
   // test parameter
   //
   if ((ubPhyIfV > QCAN_NETWORK_MAX) || (ubPhyIfV == eCAN_CHANNEL_NONE))
   {
      tvStatusT = eCP_ERR_CHANNEL;
   }
   else
   {
      //-------------------------------------------------------------------------------------------
      // Get access to socket and check if it is already connected
      //
      pclSockT = &aclCanSockListS[ubPhyIfV - 1];
      if (pclSockT->isConnected() == false)
      {
         pclSockT->connectNetwork((CAN_Channel_e) ubPhyIfV);

         //-----------------------------------------------------------------------------------
         // Wait here until the connection is established
         //
         QTimer clConnectionTimerT;
         clConnectionTimerT.setSingleShot(true);
         clConnectionTimerT.start(100);
         bool btConnectionEstablishedT = false;
         while ( (btConnectionEstablishedT == false) && (clConnectionTimerT.isActive()) )
         {
            QCoreApplication::instance()->processEvents();
            btConnectionEstablishedT = pclSockT->isConnected();
         }
      }

      //-------------------------------------------------------------------------------------------
      // initialise members if socket is conneted
      //
      if (pclSockT->isConnected())
      {
         //-----------------------------------------------------------------------------------
         // connect the readyRead() signal with the slot for message reception
         //
         pclSockT->connect(pclSockT, &QCanSocketCpFD::readyRead, pclSockT, &QCanSocketCpFD::onSocketReceive);

         //-----------------------------------------------------------------------------------
         // no FIFOs attached to message buffers
         //
         for (uint8_t ubBufferCntT = 0; ubBufferCntT < CP_BUFFER_MAX; ubBufferCntT++)
         {
            pclSockT->aptsCanFifoP[ubBufferCntT] = Q_NULLPTR;
         }

         //---------------------------------------------------------------------------
         // store physical channel information
         //
         ptsPortV->ubPhyIf   = ubPhyIfV;
         ptsPortV->ubDrvInfo = eDRV_INFO_INIT;

         //---------------------------------------------------------------------------
         // clear CAN state
         //
         pclSockT->tsCanStateP.ubCanErrState  = eCP_STATE_INIT;
         pclSockT->tsCanStateP.ubCanErrType   = eCP_ERR_TYPE_NONE;
         pclSockT->tsCanStateP.ubCanRcvErrCnt = 0;
         pclSockT->tsCanStateP.ubCanTrmErrCnt = 0;

         //---------------------------------------------------------------------------
         // clear statistic information
         //
         pclSockT->tsStatisticP.ulErrMsgCount = 0;
         pclSockT->tsStatisticP.ulRcvMsgCount = 0;
         pclSockT->tsStatisticP.ulTrmMsgCount = 0;

         tvStatusT = eCP_ERR_NONE;

      }
      else
      {
         tvStatusT = eCP_ERR_INIT_FAIL;
      }
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreDriverRelease()                                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverRelease(CpPort_ts * ptsPortV)
{
   CpStatus_tv       tvStatusT;
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;

   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpCoreDriverRelease() ....... :" << (ptsPortV->ubPhyIf);

   //---------------------------------------------------------------------------------------------------
   // release ready read connection
   //
   pclSockT->disconnect(pclSockT, &QCanSocketCpFD::readyRead, pclSockT, &QCanSocketCpFD::onSocketReceive);

   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV 
   //
   tvStatusT = CheckParam(ptsPortV, eCP_BUFFER_1, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }

   CpCoreCanMode(ptsPortV, eCP_MODE_INIT);


   //---------------------------------------------------------------------------------------------------
   // Disconnect the socket
   //
   if (pclSockT != (QCanSocketCpFD *) 0L)
   {
      pclSockT->disconnectNetwork();
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreFifoConfig()                                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoConfig(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV, CpFifo_ts * ptsFifoV)
{
   CpStatus_tv       tvStatusT;
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   
   
   qDebug() << "CpCoreFifoConfig()........... :" << ubBufferIdxV;

   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }

   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if ( pclSockT != (QCanSocketCpFD *) 0L ) 
   {
      
      pclSockT->aptsCanFifoP[ubBufferIdxV] = ptsFifoV;
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreFifoRead()                                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoRead(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV, CpCanMsg_ts * ptsCanMsgV, uint32_t * pulMsgCntV)
{
   CpStatus_tv       tvStatusT;
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   CpFifo_ts *       ptsFifoT;
   CpCanMsg_ts *     ptsFifoMsgT;
   

   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }

   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if ( pclSockT != (QCanSocketCpFD *) 0L ) 
   {
      ptsFifoT = pclSockT->aptsCanFifoP[ubBufferIdxV];
      
      if (ptsFifoT == Q_NULLPTR)
      {
         tvStatusT = eCP_ERR_FIFO_PARAM;
      }
      else
      {
         if(CpFifoIsEmpty(ptsFifoT) == 1)
         {
            //--------------------------------------------------------------------------------
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


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreFifoRelease()                                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoRelease(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   CpStatus_tv       tvStatusT;
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;

   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }

   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if ( pclSockT != (QCanSocketCpFD *) 0L ) 
   {
      pclSockT->aptsCanFifoP[ubBufferIdxV] = Q_NULLPTR;
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreFifoWrite()                                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoWrite(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV, CpCanMsg_ts * ptsCanMsgV, uint32_t * pulMsgCntV)
{
   QCanFrame         clFrameT;
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   CpStatus_tv       tvStatusT;
   uint32_t          ulMsgCntT;
   CpTrmHandler_Fn   pfnTrmHandlerT;
   
   
   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }

   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if ( pclSockT != (QCanSocketCpFD *) 0L ) 
   {

      //-------------------------------------------------------------------------------------------
      // write CAN frame
      //
      for (ulMsgCntT = 0; ulMsgCntT < *pulMsgCntV; ulMsgCntT++)
      {
         clFrameT = pclSockT->fromCpMsg(ptsCanMsgV);
         if(pclSockT->write(clFrameT) == false)
         {
            tvStatusT = eCP_ERR_TRM_FULL;
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


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreHDI()                                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreHDI(CpPort_ts *ptsPortV, CpHdi_ts *ptsHdiV)
{
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   CpStatus_tv       tvStatusT = eCP_ERR_CHANNEL;
   

   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV 
   //
   tvStatusT = CheckParam(ptsPortV, eCP_BUFFER_1, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }

   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if ( pclSockT != (QCanSocketCpFD *) 0L ) 
   {
      if (ptsHdiV != (CpHdi_ts *) 0L)
      {
         ptsHdiV->ubVersionMajor   = CP_VERSION_MAJOR;
         ptsHdiV->ubVersionMinor   = CP_VERSION_MINOR;
         ptsHdiV->ubCanFeatures    = 3;
         ptsHdiV->ubDriverFeatures = 0;
         ptsHdiV->ubDriverMajor    = CP_DRIVER_MAJOR;
         ptsHdiV->ubDriverMinor    = CP_DRIVER_MINOR;
         ptsHdiV->ubBufferMax      = CP_BUFFER_MAX;
         ptsHdiV->slNomBitRate     = pclSockT->slNomBitRateP;
         ptsHdiV->slDatBitRate     = pclSockT->slDatBitRateP;

         tvStatusT = eCP_ERR_NONE;
      }
   }
   
   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreIntFunctions()                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreIntFunctions( CpPort_ts * ptsPortV,
                        uint8_t (* pfnRcvHandler)(CpCanMsg_ts *, uint8_t),
                        uint8_t (* pfnTrmHandler)(CpCanMsg_ts *, uint8_t),
                        uint8_t (* pfnErrHandler)(CpState_ts *) )
{
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;
   CpStatus_tv       tvStatusT = eCP_ERR_CHANNEL;
   

   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV 
   //
   tvStatusT = CheckParam(ptsPortV, eCP_BUFFER_1, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }


   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if (pclSockT != (QCanSocketCpFD *) 0L)
   {
      //---------------------------------------------------------------------------
      // store the new callbacks
      //
      pclSockT->pfnRcvIntHandlerP = pfnRcvHandler;
      pclSockT->pfnTrmIntHandlerP = pfnTrmHandler;
      pclSockT->pfnErrIntHandlerP = pfnErrHandler;

      tvStatusT = eCP_ERR_NONE;
   }
   
   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreStatistic()                                                                                                  //
// return statistical information                                                                                     //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpCoreStatistic(CpPort_ts * ptsPortV, CpStatistic_ts * ptsStatsV)
{
   CpStatus_tv       tvStatusT = eCP_ERR_CHANNEL;
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;

   //---------------------------------------------------------------------------------------------------
   // test parameter ptsPortV 
   //
   tvStatusT = CheckParam(ptsPortV, eCP_BUFFER_1, eDRV_INFO_INIT);

   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if (tvStatusT == eCP_ERR_NONE)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
      {
         pclSockT  = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
      }
   }


   //---------------------------------------------------------------------------------------------------
   // test if previous operations failed before we proceed
   //
   if (pclSockT != (QCanSocketCpFD *) 0L)
   {
      tvStatusT = eCP_ERR_PARAM;
      if (ptsStatsV != (CpStatistic_ts *) 0L)
      {
         ptsStatsV->ulErrMsgCount = pclSockT->tsStatisticP.ulErrMsgCount;
         ptsStatsV->ulRcvMsgCount = pclSockT->tsStatisticP.ulRcvMsgCount;
         ptsStatsV->ulTrmMsgCount = pclSockT->tsStatisticP.ulTrmMsgCount;
         tvStatusT = eCP_ERR_NONE;
      }
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpSocketConnectSlots()                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpSocketConnectSlots(uint8_t ubPhyIfV, QObject * pclDestObjectV,
                                 const char * pubSockConnectV, const char * pubSockDisconnectV,
                                 const char * pubSockErrorV)
{
   QCanSocketCpFD *  pclSockT = (QCanSocketCpFD *) 0L;

   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpSocketConnectSlots() ...... : " << pclDestObjectV;


   //---------------------------------------------------------------------------------------------------
   // test parameter
   //
   if ((ubPhyIfV > QCAN_NETWORK_MAX) || (ubPhyIfV == eCAN_CHANNEL_NONE))
   {
      return(eCP_ERR_CHANNEL);
   }

   
   //-----------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   pclSockT  = &aclCanSockListS[ubPhyIfV - 1];

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


//--------------------------------------------------------------------------------------------------------------------//
// CpSocketInstance()                                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanSocket * CpSocketInstance(uint8_t ubPhyIfV)
{
   QCanSocket *  pclSockT = (QCanSocket *) 0L;

   //---------------------------------------------------------------------------------------------------
   // test parameter
   //
   if ((ubPhyIfV < QCAN_NETWORK_MAX) && (ubPhyIfV > eCAN_CHANNEL_NONE))
   {
      pclSockT = (QCanSocket *) &aclCanSockListS[ubPhyIfV - 1];
   }

   return (pclSockT);
}

//--------------------------------------------------------------------------------------------------------------------//
// CpSocketProcessEvents()                                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void CpSocketProcessEvents()
{
   if (pclApplicationS != 0L)
   {
      pclApplicationS->processEvents();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// CpSocketSetHostAddress()                                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CpSocketSetHostAddress(uint8_t ubPhyIfV, QHostAddress clHostAddressV)
{
   QCanSocketCpFD *   pclSockT;
   CpStatus_tv        tvStatusT = eCP_ERR_CHANNEL;

   //---------------------------------------------------------------------------------------------------
   // test parameter
   //
   if ( (ubPhyIfV > eCAN_CHANNEL_NONE) && (ubPhyIfV <= QCAN_NETWORK_MAX) )
   {

      //-------------------------------------------------------------------------------------------
      // get pointer to CANpie socket class
      //
      pclSockT  = &aclCanSockListS[ubPhyIfV - 1];

      pclSockT->setHostAddress(clHostAddressV);
      tvStatusT = eCP_ERR_NONE;
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketCpFD::QCanSocketCpFD()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanSocketCpFD::QCanSocketCpFD()
{
   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   qDebug() << "QCanSocketCpFD() ............ : create object";


   //---------------------------------------------------------------------------------------------------
   // clear members
   //
   pfnErrIntHandlerP  = 0;
   pfnRcvIntHandlerP  = 0;
   pfnTrmIntHandlerP  = 0;

   slNomBitRateP      = eCAN_BITRATE_NONE;
   slDatBitRateP      = eCAN_BITRATE_NONE;

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketCpFD::~QCanSocketCpFD()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanSocketCpFD::~QCanSocketCpFD()
{
   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   qDebug() << "~QCanSocketCpFD() ........... : delete object";

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketCpFD::fromCanFrame()                                                                                     //
// message conversion                                                                                                 //
//--------------------------------------------------------------------------------------------------------------------//
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

   for (ubDataCntT = 0; ubDataCntT < CP_DATA_SIZE; ubDataCntT++)
   {
      CpMsgSetData(&tsCanMsgT, ubDataCntT, clCanFrameR.data(ubDataCntT));
   }
   return(tsCanMsgT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketCpFD::fromCpMsg()                                                                                        //
// message conversion                                                                                                 //
//--------------------------------------------------------------------------------------------------------------------//
QCanFrame QCanSocketCpFD::fromCpMsg(CpCanMsg_ts * ptsCanMsgV)
{
   QCanFrame      clCanFrameT;
   uint8_t        ubDataCntT;

   if (CpMsgIsFdFrame(ptsCanMsgV))
   {
      if (CpMsgIsExtended(ptsCanMsgV))
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
      if (CpMsgIsExtended(ptsCanMsgV))
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

   for (ubDataCntT = 0; ubDataCntT < clCanFrameT.dataSize(); ubDataCntT++)
   {
      clCanFrameT.setData(ubDataCntT, CpMsgGetData(ptsCanMsgV, ubDataCntT));
   }

   return (clCanFrameT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketCpFD::fromCpMsg()                                                                                        //
// message conversion                                                                                                 //
//--------------------------------------------------------------------------------------------------------------------//
QCanFrame QCanSocketCpFD::fromCpMsg(uint8_t ubMsgBufferV)
{
   QCanFrame      clCanFrameT;
   CpCanMsg_ts *  ptsCanMsgT;
   uint8_t        ubDataCntT;

   ptsCanMsgT = &(atsCanMsgP[ubMsgBufferV]);

   if(CpMsgIsFdFrame(ptsCanMsgT))
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



//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketCpFD::handleCanFrame()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketCpFD::handleCanFrame(QCanFrame & clCanFrameR)
{
   CpCanMsg_ts    tsCanMsgT;
   CpCanMsg_ts *  ptsCanBufT;
   CpFifo_ts *    ptsFifoT;
   uint32_t       ulAccMaskT;
   uint8_t        ubBufferIdxT;

   tsCanMsgT = fromCanFrame(clCanFrameR);

   //---------------------------------------------------------------------------------------------------
   // run through all possible message buffer
   //
   for (ubBufferIdxT = 0; ubBufferIdxT < CP_BUFFER_MAX; ubBufferIdxT++)
   {
      //-------------------------------------------------------------------------------------------
      // setup pointer to CAN message buffer
      //
      ptsCanBufT = &(this->atsCanMsgP[ubBufferIdxT]);

      //-------------------------------------------------------------------------------------------
      // get acceptance mask
      //
      ulAccMaskT = this->atsAccMaskP[ubBufferIdxT];

      //-------------------------------------------------------------------------------------------
      // test direction flag
      //
      if ( ((ptsCanBufT->ulMsgUser) & CP_USER_FLAG_RCV) == 0) continue;

      //-------------------------------------------------------------------------------------------
      // distinguish frame types
      //
      if (CpMsgIsExtended(ptsCanBufT) == CpMsgIsExtended(&tsCanMsgT))
      {
         //-----------------------------------------------------------------------------------
         // check for identifier
         //
         if( (CpMsgGetIdentifier(ptsCanBufT) & ulAccMaskT) == (CpMsgGetIdentifier(&tsCanMsgT) & ulAccMaskT) )
         {
            //---------------------------------------------------------------------------
            // copy to buffer
            //
            ptsCanBufT->ulIdentifier   = tsCanMsgT.ulIdentifier;
            ptsCanBufT->ubMsgDLC       = tsCanMsgT.ubMsgDLC;
            ptsCanBufT->ubMsgCtrl      = tsCanMsgT.ubMsgCtrl;
            memcpy(&(ptsCanBufT->tuMsgData.aubByte[0]), &(tsCanMsgT.tuMsgData.aubByte[0]), CP_DATA_SIZE );

            //---------------------------------------------------------------------------
            // test for receive callback handler
            //
            if (this->aptsCanFifoP[ubBufferIdxT] == 0L)
            {
               //-------------------------------------------------------------------
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

            //---------------------------------------------------------------------------
            // Update statistic
            //
            tsStatisticP.ulRcvMsgCount++;

            //---------------------------------------------------------------------------
            // Quit loop because there was an identifier match
            //
            break;
         }
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketCpFD::onSocketReceive()                                                                                  //
// receive CAN message                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketCpFD::onSocketReceive()
{
   QCanFrame         clCanFrameT;
   uint32_t          ulFrameCntT;
   uint32_t          ulFrameMaxT;

   //---------------------------------------------------------------------------------------------------
   // Check how many frames are available
   //
   ulFrameMaxT = framesAvailable();

   //---------------------------------------------------------------------------------------------------
   // Read CAN frames and handle them
   //
   for (ulFrameCntT = 0; ulFrameCntT < ulFrameMaxT; ulFrameCntT++)
   {
      if (this->read(clCanFrameT) == true)
      {
         handleCanFrame(clCanFrameT);
      }
   }
}
