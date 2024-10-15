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
#define  CP_DRIVER_MINOR          6


constexpr uint32_t   CP_USER_FLAG_RCV = 0x00000001;
constexpr uint32_t   CP_USER_FLAG_TRM = 0x00000002;


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
static QCoreApplication *  pclApplicationS = nullptr;


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
   if (ptsPortV != nullptr)
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
CpStatus_tv CP_API(CpCoreBitrate)( CpPort_ts * ptsPortV, int32_t slNomBitRateV, int32_t slDatBitRateV)
{
   CpStatus_tv       tvStatusT = eCP_ERR_CHANNEL;
   
   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "CpCoreBitrate() ............. :" << slNomBitRateV << slDatBitRateV;
   #endif

   //---------------------------------------------------------------------------------------------------
   // Parameters are not used in this function, since the bitrate is configured inside the CANpie
   // server. Silence compiler warnings.
   //
   (void) slNomBitRateV;
   (void) slDatBitRateV;

   //---------------------------------------------------------------------------------------------------
   // get access to socket
   //
   if (ptsPortV != nullptr)
   {
      if ( ((ptsPortV->ubPhyIf) > eCP_CHANNEL_NONE) && ((ptsPortV->ubPhyIf) <= QCAN_NETWORK_MAX) )
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
CpStatus_tv CP_API(CpCoreBufferConfig)(CpPort_ts * ptsPortV, uint8_t   ubBufferIdxV,
                                       uint32_t  ulIdentifierV, uint32_t  ulAcceptMaskV,
                                       uint8_t   ubFormatV, uint8_t   ubDirectionV)
{
   QCanSocketCpFD *  pclSockT = nullptr;
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
   if ( pclSockT != nullptr ) 
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
CpStatus_tv CP_API(CpCoreBufferGetData)( CpPort_ts * ptsPortV, uint8_t   ubBufferIdxV,
                                         uint8_t * pubDestDataV, uint8_t   ubStartPosV, uint8_t   ubSizeV)
{
   QCanSocketCpFD *  pclSockT = nullptr;
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
   if ( pclSockT != nullptr ) 
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
CpStatus_tv CP_API(CpCoreBufferGetDlc)(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV, uint8_t * pubDlcV)
{
   QCanSocketCpFD *  pclSockT = nullptr;
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
   if ( pclSockT != nullptr ) 
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
CpStatus_tv CP_API(CpCoreBufferRelease)( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   QCanSocketCpFD *  pclSockT = nullptr;
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
   if ( pclSockT != nullptr ) 
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
CpStatus_tv CP_API(CpCoreBufferSend)(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   QCanFrame         clFrameT;
   QCanSocketCpFD *  pclSockT = nullptr;
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
   if ( pclSockT != nullptr ) 
   {
      //-------------------------------------------------------------------------------------------
      // write CAN frame
      //
      clFrameT = pclSockT->fromCpMsg(ubBufferIdxV);
      #if CP_NRL_SUPPORT > 0
      if (pclSockT->hasFault() == false)
      {
         if (pclSockT->write(clFrameT) == false)
         {
            tvStatusT = eCP_ERR_TRM_FULL;
         }
         else
         {
            pclSockT->tsStatisticP.ulTrmMsgCount++;

            //---------------------------------------------------------------------------
            // NRL: Increment mailbox counter
            //
            CpNrlMboxCounterEvent(pclSockT->ubInterfaceP, ubBufferIdxV);
   
            if (pclSockT->pfnTrmIntHandlerP != nullptr)
            {
               pfnTrmHandlerT = pclSockT->pfnTrmIntHandlerP;
               tvStatusT = (* pfnTrmHandlerT)(&(pclSockT->atsCanMsgP[ubBufferIdxV]), ubBufferIdxV);
            }
         }
      }
      else
      {
         tvStatusT = eCP_ERR_TRM_FULL;
      }
      #else
      if (pclSockT->write(clFrameT) == false)
      {
         tvStatusT = eCP_ERR_TRM_FULL;
      }
      else
      {
         pclSockT->tsStatisticP.ulTrmMsgCount++;

         if (pclSockT->pfnTrmIntHandlerP != nullptr)
         {
            pfnTrmHandlerT = pclSockT->pfnTrmIntHandlerP;
            tvStatusT = (* pfnTrmHandlerT)(&(pclSockT->atsCanMsgP[ubBufferIdxV]), ubBufferIdxV);
         }
      }
      #endif
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreBufferSetData()                                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CP_API(CpCoreBufferSetData)( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,  uint8_t * pubSrcDataV,
                                         uint8_t   ubStartPosV, uint8_t   ubSizeV)
{
   QCanSocketCpFD *  pclSockT = nullptr;
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
   if ( pclSockT != nullptr ) 
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
CpStatus_tv CP_API(CpCoreBufferSetDlc)( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV, uint8_t ubDlcV)
{
   QCanSocketCpFD *  pclSockT = nullptr;
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
   if ( pclSockT != nullptr ) 
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
CpStatus_tv CP_API(CpCoreCanMode)(CpPort_ts * ptsPortV, uint8_t ubModeV)
{
   QCanSocketCpFD *  pclSockT = nullptr;
   CpStatus_tv       tvStatusT = eCP_ERR_CHANNEL;

   
   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "CpCoreCanMode() ............. :" << ubModeV;
   #endif

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
   if ( pclSockT != nullptr ) 
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
CpStatus_tv CP_API(CpCoreCanState)(CpPort_ts * ptsPortV, CpState_ts * ptsStateV)
{
   QCanSocketCpFD *  pclSockT = nullptr;
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
   if ( pclSockT != nullptr ) 
   {
      tvStatusT = eCP_ERR_PARAM;

      if ( ptsStateV != nullptr )
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
CpStatus_tv CP_API(CpCoreDriverInit)(uint8_t ubPhyIfV, CpPort_ts * ptsPortV, uint8_t ubConfigV)
{
   QCanSocketCpFD *  pclSockT = nullptr;
   CpStatus_tv       tvStatusT = eCP_ERR_GENERIC;

   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "CpCoreDriverInit() .......... :" << ubPhyIfV << ubConfigV;
   #endif

   //---------------------------------------------------------------------------------------------------
   // Parameter 'ubConfigV' is not evaluated, silence compiler warning
   //
   (void) ubConfigV;
   
   //---------------------------------------------------------------------------------------------------
   // Check if an instance of QCoreApplication is active. In case there is no QCoreApplication there
   // is no QEventLoop and we need a thread that polls data from the CANpie server.
   //
   if (pclApplicationS == nullptr)
   {
      if (QCoreApplication::instance() == nullptr)
      {
         int32_t slAppArgcT = 0;
         char ** szAppArgvT = nullptr;
         pclApplicationS = new QCoreApplication(slAppArgcT, szAppArgvT);
      
         #ifndef QT_NO_DEBUG_OUTPUT
         qDebug() << "QCanSocketCpFD() ............ : create new QCoreApplication";
         #endif
      }
   }


   //---------------------------------------------------------------------------------------------------
   // test parameter
   //
   if ((ubPhyIfV > QCAN_NETWORK_MAX) || (ubPhyIfV == QCan::eCAN_CHANNEL_NONE))
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
         pclSockT->connectNetwork((QCan::CAN_Channel_e) ubPhyIfV);

         //-----------------------------------------------------------------------------------
         // Wait here until the connection is established
         //
         QTimer clConnectionTimerT;
         clConnectionTimerT.setSingleShot(true);
         clConnectionTimerT.start(100);
         bool_t btConnectionEstablishedT = false;
         while ( (btConnectionEstablishedT == false) && (clConnectionTimerT.isActive()) )
         {
            QCoreApplication::instance()->processEvents();
            btConnectionEstablishedT = pclSockT->isConnected();
         }
      }

      //-------------------------------------------------------------------------------------------
      // initialise members if socket is connected
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
            pclSockT->aptsCanFifoP[ubBufferCntT] = nullptr;
         }

         //---------------------------------------------------------------------------
         // store physical channel information
         //
         ptsPortV->ubPhyIf   = ubPhyIfV;
         ptsPortV->ubDrvInfo = eDRV_INFO_INIT;

         pclSockT->ubInterfaceP = ubPhyIfV;

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

         //---------------------------------------------------------------------------
         // Configure pointer to NRL core data 
         //
         #if CP_NRL_CHANNEL_MAX > 0
         pclSockT->ptsNrlCoreDataM = CpNrlCoreData(ubPhyIfV);
         #endif

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
CpStatus_tv CP_API(CpCoreDriverRelease)(CpPort_ts * ptsPortV)
{
   CpStatus_tv       tvStatusT;
   QCanSocketCpFD *  pclSockT = nullptr;

   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "CpCoreDriverRelease() ....... :" << (ptsPortV->ubPhyIf);
   #endif

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
   if (pclSockT != nullptr)
   {
      //-------------------------------------------------------------------------------------------
      // release connections
      //
      pclSockT->disconnect(pclSockT, &QCanSocketCpFD::readyRead, pclSockT, &QCanSocketCpFD::onSocketReceive);
      pclSockT->disconnectNetwork();
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreFifoConfig()                                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CP_API(CpCoreFifoConfig)(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV, CpFifo_ts * ptsFifoV)
{
   CpStatus_tv       tvStatusT;
   QCanSocketCpFD *  pclSockT = nullptr;
   
   
   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "CpCoreFifoConfig()........... :" << ubBufferIdxV;
   #endif

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
   if ( pclSockT != nullptr ) 
   {
      pclSockT->aptsCanFifoP[ubBufferIdxV] = ptsFifoV;
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreFifoRead()                                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CP_API(CpCoreFifoRead)(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV, CpCanMsg_ts * ptsCanMsgV, 
                                   uint32_t * pulMsgCntV)
{
   CpStatus_tv       tvStatusT;
   QCanSocketCpFD *  pclSockT = nullptr;
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
   if ( pclSockT != nullptr ) 
   {
      ptsFifoT = pclSockT->aptsCanFifoP[ubBufferIdxV];
      
      if (ptsFifoT == nullptr)
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
CpStatus_tv CP_API(CpCoreFifoRelease)(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   CpStatus_tv       tvStatusT;
   QCanSocketCpFD *  pclSockT = nullptr;

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
   if ( pclSockT != nullptr ) 
   {
      pclSockT->aptsCanFifoP[ubBufferIdxV] = nullptr;
   }

   return (tvStatusT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCoreFifoWrite()                                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CP_API(CpCoreFifoWrite)(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV, CpCanMsg_ts * ptsCanMsgV, 
                                    uint32_t * pulMsgCntV)
{
   QCanFrame         clFrameT;
   QCanSocketCpFD *  pclSockT = nullptr;
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
   if ( pclSockT != nullptr ) 
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

         if (pclSockT->pfnTrmIntHandlerP != nullptr)
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
CpStatus_tv CP_API(CpCoreHDI)(CpPort_ts *ptsPortV, CpHdi_ts *ptsHdiV)
{
   QCanSocketCpFD *  pclSockT = nullptr;
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
   if ( pclSockT != nullptr ) 
   {
      if (ptsHdiV != nullptr)
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
CpStatus_tv CP_API(CpCoreIntFunctions)(CpPort_ts * ptsPortV,
                                       uint8_t (* pfnRcvHandler)(CpCanMsg_ts *, uint8_t),
                                       uint8_t (* pfnTrmHandler)(CpCanMsg_ts *, uint8_t),
                                       uint8_t (* pfnErrHandler)(CpState_ts *) )
{
   QCanSocketCpFD *  pclSockT = nullptr;
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
   if (pclSockT != nullptr)
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
CpStatus_tv CP_API(CpCoreStatistic)(CpPort_ts * ptsPortV, CpStatistic_ts * ptsStatsV)
{
   CpStatus_tv       tvStatusT = eCP_ERR_CHANNEL;
   QCanSocketCpFD *  pclSockT = nullptr;

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
   if (pclSockT != nullptr)
   {
      tvStatusT = eCP_ERR_PARAM;
      if (ptsStatsV != nullptr)
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
// CpCoreStatisticClear()                                                                                             //
// clear statistical information                                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
CpStatus_tv CP_API(CpCoreStatisticClear)(CpPort_ts *ptsPortV)
{
   CpStatus_tv       tvStatusT;
   QCanSocketCpFD *  pclSockT = nullptr;

   //---------------------------------------------------------------------------------------------------
   // check parameter
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
   if (pclSockT != nullptr)
   {
      pclSockT->tsStatisticP.ulErrMsgCount = 0;
      pclSockT->tsStatisticP.ulRcvMsgCount = 0;
      pclSockT->tsStatisticP.ulTrmMsgCount = 0;
      tvStatusT = eCP_ERR_NONE;
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
   QCanSocketCpFD *  pclSockT = nullptr;

   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "CpSocketConnectSlots() ...... : " << pclDestObjectV;
   #endif

   //---------------------------------------------------------------------------------------------------
   // test parameter
   //
   if ((ubPhyIfV > QCAN_NETWORK_MAX) || (ubPhyIfV == QCan::eCAN_CHANNEL_NONE))
   {
      return(eCP_ERR_CHANNEL);
   }

   
   //-----------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   pclSockT  = &aclCanSockListS[ubPhyIfV - 1];

   if(pubSockConnectV != nullptr)
   {
      QObject::connect( pclSockT, SIGNAL(connected()),
                        pclDestObjectV, pubSockConnectV);
   }

   if(pubSockDisconnectV != nullptr)
   {
      QObject::connect( pclSockT, SIGNAL(disconnected()),
                        pclDestObjectV, pubSockDisconnectV);
   }
   
   if(pubSockErrorV != nullptr)
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
   QCanSocket *  pclSockT = nullptr;

   //---------------------------------------------------------------------------------------------------
   // test parameter
   //
   if ((ubPhyIfV < QCAN_NETWORK_MAX) && (ubPhyIfV > QCan::eCAN_CHANNEL_NONE))
   {
      pclSockT = static_cast< QCanSocket * >(&aclCanSockListS[ubPhyIfV - 1]);
   }

   return (pclSockT);
}

//--------------------------------------------------------------------------------------------------------------------//
// CpSocketProcessEvents()                                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void CpSocketProcessEvents()
{
   if (pclApplicationS != nullptr)
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
   if ( (ubPhyIfV > QCan::eCAN_CHANNEL_NONE) && (ubPhyIfV <= QCAN_NETWORK_MAX) )
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
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanSocketCpFD() ............ : create object";
   #endif

   //---------------------------------------------------------------------------------------------------
   // clear members
   //
   pfnErrIntHandlerP  = nullptr;
   pfnRcvIntHandlerP  = nullptr;
   pfnTrmIntHandlerP  = nullptr;

   slNomBitRateP      = QCan::eCAN_BITRATE_NONE;
   slDatBitRateP      = QCan::eCAN_BITRATE_NONE;

   #if CP_NRL_SUPPORT > 0
   ptsNrlCoreDataM    = nullptr;
   btFaultOnLineP     = false;
   #endif

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
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "~QCanSocketCpFD() ........... : delete object";
   #endif
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketCpFD::flushReceiveMailbox()                                                                              //
// helper for NRL                                                                                                     //
//--------------------------------------------------------------------------------------------------------------------//
#if CP_NRL_SUPPORT > 0
void QCanSocketCpFD::flushReceiveMailbox(CpNrlCoreData_ts * ptsCoreDataV)
{
   CpCanMsg_ts *  ptsCanBufT;
   uint8_t        ubBufferIdxT;

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
      // test direction flag
      //
      if ( ((ptsCanBufT->ulMsgUser) & CP_USER_FLAG_RCV) > 0) 
      {
         //--------------------------------------------------------------------------------
         // Counter not balanced, so flush this buffer
         //
         if ((ptsCoreDataV->aswMboxCounter[ubBufferIdxT]) != 0)
         {
            //-----------------------------------------------------------------------------------
            // test for receive callback handler
            //
            if (this->aptsCanFifoP[ubBufferIdxT] == nullptr)
            {
               //-------------------------------------------------------------------
               // no FIFO available
               //
               if (this->pfnRcvIntHandlerP != nullptr)
               {
                  (* this->pfnRcvIntHandlerP)(ptsCanBufT, ubBufferIdxT);
               }
            }
         }
      }
   }

}
#endif

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
            // Reload mailbox timeout and increment mailbox counter for NRL
            //
            #if CP_NRL_SUPPORT > 0
            CpNrlMboxTickLoad(ubInterfaceP, ubBufferIdxT);
            CpNrlMboxCounterEvent(ubInterfaceP, ubBufferIdxT);
            #endif

            //---------------------------------------------------------------------------
            // test for receive callback handler
            //
            if (this->aptsCanFifoP[ubBufferIdxT] == nullptr)
            {
               //-------------------------------------------------------------------
               // no FIFO available
               //
               if (this->pfnRcvIntHandlerP != nullptr)
               {
                  (* this->pfnRcvIntHandlerP)(ptsCanBufT, ubBufferIdxT);
               }
            }
            else
            {
               ptsFifoT = this->aptsCanFifoP[ubBufferIdxT];
               if (CpFifoIsFull(ptsFifoT) == true)
               {
                  // drop one message
                  CpFifoIncOut(ptsFifoT);
               }
               ptsCanBufT = CpFifoDataInPtr(ptsFifoT);
               memcpy(ptsCanBufT, &tsCanMsgT, sizeof(CpCanMsg_ts));
               CpFifoIncIn(ptsFifoT);
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
      #if CP_NRL_SUPPORT > 0
      if (btFaultOnLineP == false)
      {
         if (this->read(clCanFrameT) == true)
         {
            handleCanFrame(clCanFrameT);
         }
      }
      #else
      if (this->read(clCanFrameT) == true)
      {
         handleCanFrame(clCanFrameT);
      }
      #endif

   }
}



/*--------------------------------------------------------------------------------------------------------------------*\
** Network Redundancy Layer support                                                                                   **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*\
** CANpie driver structure for NRL                                                                                    **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/
#if CP_NRL_CHANNEL_MAX > 0

CpNrlDriver_ts tsNrlDriverG = {
   CpCoreBitrate_Nrl,
   CpCoreBufferConfig_Nrl,
   CpCoreBufferGetData_Nrl,
   CpCoreBufferGetDlc_Nrl,
   CpCoreBufferRelease_Nrl,
   CpCoreBufferSend_Nrl,
   CpCoreBufferSetData_Nrl,
   CpCoreBufferSetDlc_Nrl,
   CpCoreCanMode_Nrl,
   CpCoreCanState_Nrl,
   CpCoreDriverInit_Nrl,
   CpCoreDriverRelease_Nrl,
   CpCoreFifoConfig_Nrl,
   CpCoreFifoRead_Nrl,
   CpCoreFifoRelease_Nrl,
   CpCoreFifoWrite_Nrl,
   CpCoreHDI_Nrl,
   CpCoreIntFunctions_Nrl,
   CpCoreStatistic_Nrl,
   CpCoreStatisticClear_Nrl,
   nullptr
};


//--------------------------------------------------------------------------------------------------------------------//
// CpNrlFaultSimulation()                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void CpNrlFaultSimulation(uint8_t ubNrlChannelV, enum CpNrlFault_e teFaultV)
{
   QCanSocketCpFD *  pclSockT = nullptr;
   
   //---------------------------------------------------------------------------------------------------
   // get pointer to CANpie socket class
   //
   if ( (ubNrlChannelV > eCP_NRL_CHANNEL_NONE) && (ubNrlChannelV <= eCP_NRL_CHANNEL_LIMIT) )
   {
      switch (teFaultV)
      {
         case eCP_NRL_FAULT_NONE:
            pclSockT  = &aclCanSockListS[ubNrlChannelV - 1];
            if (pclSockT != nullptr)
            {
               pclSockT->simulateFault(false);
            }
            pclSockT  = &aclCanSockListS[ubNrlChannelV];
            if (pclSockT != nullptr)
            {
               pclSockT->simulateFault(false);
            }
            break;

         case eCP_NRL_FAULT_DISCONNECT_DCL:
            pclSockT  = &aclCanSockListS[ubNrlChannelV - 1];
            if (pclSockT != nullptr)
            {
               pclSockT->simulateFault(true);
            }
            pclSockT  = &aclCanSockListS[ubNrlChannelV];
            if (pclSockT != nullptr)
            {
               pclSockT->simulateFault(false);
            }
            break;

         case eCP_NRL_FAULT_DISCONNECT_RCL:
            pclSockT  = &aclCanSockListS[ubNrlChannelV - 1];
            if (pclSockT != nullptr)
            {
               pclSockT->simulateFault(false);
            }
            pclSockT  = &aclCanSockListS[ubNrlChannelV];
            if (pclSockT != nullptr)
            {
               pclSockT->simulateFault(true);
            }
            break;

         case eCP_NRL_FAULT_DISCONNECT_ALL:
            pclSockT  = &aclCanSockListS[ubNrlChannelV - 1];
            if (pclSockT != nullptr)
            {
               pclSockT->simulateFault(true);
            }
            pclSockT  = &aclCanSockListS[ubNrlChannelV];
            if (pclSockT != nullptr)
            {
               pclSockT->simulateFault(true);
            }
            break;

      }

   }

}


//--------------------------------------------------------------------------------------------------------------------//
// CpNrlMboxCounterEvent()                                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool_t CpNrlMboxCounterEvent(uint8_t ubPhyIfV, uint8_t ubBufferIdxV)
{
   uint8_t              ubLineIndexT  = eCP_NRL_LINE_NONE;
   CpNrlCoreData_ts *   ptsNrlCoreDataT;

   ptsNrlCoreDataT = aclCanSockListS[ubPhyIfV - 1].ptsNrlCoreDataM;

   if (ptsNrlCoreDataT != (CpNrlCoreData_ts *) nullptr)
   {
      //-------------------------------------------------------------------------------------------
      // Find the physical CAN interface inside the CpNrlCoreData_ts structure
      //
      if (ubPhyIfV == ptsNrlCoreDataT->ubInterfaceDCL)
      {
         ubLineIndexT = eCP_NRL_LINE_DCL;
      }

      if (ubPhyIfV == ptsNrlCoreDataT->ubInterfaceRCL)
      {
         ubLineIndexT = eCP_NRL_LINE_RCL;
      }

      if (ubLineIndexT < eCP_NRL_LINE_NONE) 
      {
         //-----------------------------------------------------------------------------------
         // Read the current value
         //
         int16_t swCounterValueT = ptsNrlCoreDataT->aswMboxCounter[ubBufferIdxV];
 
         //-----------------------------------------------------------------------------------
         // An event on the DCL increases the counter by 1, an event on the RCL decreases the
         // counter by 1.
         //
         if (ubLineIndexT == eCP_NRL_LINE_DCL)
         {
            swCounterValueT++;
         }
         else
         {
            swCounterValueT--;
         }

         //-----------------------------------------------------------------------------------
         // Write new counter value
         //
         ptsNrlCoreDataT->aswMboxCounter[ubBufferIdxV] = swCounterValueT;
      }
   }

   return true;
}


//--------------------------------------------------------------------------------------------------------------------//
// CpNrlMboxFlush()                                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool_t CpNrlMboxFlush(CpPort_ts * ptsPortV)
{
   QCanSocketCpFD *     pclSockT = nullptr;
   CpNrlCoreData_ts *   ptsNrlCoreDataT = (CpNrlCoreData_ts *) nullptr;


   pclSockT = &aclCanSockListS[(ptsPortV->ubPhyIf) - 1];
   ptsNrlCoreDataT = aclCanSockListS[(ptsPortV->ubPhyIf) - 1].ptsNrlCoreDataM;

   if (pclSockT != nullptr)
   {
      pclSockT->flushReceiveMailbox(ptsNrlCoreDataT);
   }


   return true;
}


//--------------------------------------------------------------------------------------------------------------------//
// CpNrlMboxTickDecrement()                                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
static void CpNrlMboxTickDecrement(CpNrlCoreData_ts * ptsNrlCoreDataV)
{
   uint16_t uwTickValueT;

   for (uint8_t ubBufferT = 0; ubBufferT < CP_BUFFER_MAX; ubBufferT++)
   {
      //-------------------------------------------------------------------------------------------
      // Read the current value.
      //
      uwTickValueT = ptsNrlCoreDataV->auwMboxTick[0][ubBufferT];
 
      //-------------------------------------------------------------------------------------------
      // Decrement value and write it back
      //
      if (uwTickValueT > 0)
      {
         uwTickValueT--;
      }
      ptsNrlCoreDataV->auwMboxTick[0][ubBufferT] = uwTickValueT;

      //-------------------------------------------------------------------------------------------
      // Read the current value.
      //
      uwTickValueT = ptsNrlCoreDataV->auwMboxTick[1][ubBufferT];
 
      //-------------------------------------------------------------------------------------------
      // Decrement value and write it back
      //
      if (uwTickValueT > 0)
      {
         uwTickValueT--;
      }
      ptsNrlCoreDataV->auwMboxTick[1][ubBufferT] = uwTickValueT;

   }
 
}


//--------------------------------------------------------------------------------------------------------------------//
// CpNrlMboxTickLoad()                                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool_t CpNrlMboxTickLoad(uint8_t ubPhyIfV, uint8_t ubBufferIdxV)
{
   uint8_t              ubLineIndexT  = eCP_NRL_LINE_NONE;
   CpNrlCoreData_ts *   ptsNrlCoreDataT;

   ptsNrlCoreDataT = aclCanSockListS[ubPhyIfV - 1].ptsNrlCoreDataM;

   if (ptsNrlCoreDataT != (CpNrlCoreData_ts *) nullptr)
   {
      //-------------------------------------------------------------------------------------------
      // Find the physical CAN interface inside the CpNrlCoreData_ts structure
      //
      if (ubPhyIfV == ptsNrlCoreDataT->ubInterfaceDCL)
      {
         ubLineIndexT = eCP_NRL_LINE_DCL;
      }

      if (ubPhyIfV == ptsNrlCoreDataT->ubInterfaceRCL)
      {
         ubLineIndexT = eCP_NRL_LINE_RCL;
      }

      if (ubLineIndexT < eCP_NRL_LINE_NONE) 
      {
         ptsNrlCoreDataT->auwMboxTick[ubLineIndexT][ubBufferIdxV] = ptsNrlCoreDataT->uwMboxTickReload;
      }
   }

   return true;
}


//--------------------------------------------------------------------------------------------------------------------//
// CpNrlMboxTickEvent()                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void CpNrlMboxTickEvent(void)
{
   if (aclCanSockListS[0].ptsNrlCoreDataM != (CpNrlCoreData_ts *) nullptr)
   {
      CpNrlMboxTickDecrement(aclCanSockListS[0].ptsNrlCoreDataM);
   }
}

#endif
