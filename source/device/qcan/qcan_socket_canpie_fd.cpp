//============================================================================//
// File:          qcan_socket_canpie_fd.hpp                                   //
// Description:   QCAN classes - CAN socket for CANpie FD                     //
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

#include <QVector>


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
   eDRV_INFO_INIT,
   eDRV_INFO_ACTIVE
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
// Setup bit-rare of CAN controller                                           //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBitrate( CpPort_ts * ptsPortV, int32_t slBitrateV,
                           int32_t slBrsClockV)
{
   QCanFrameApi      clFrameT;
   QCanSocketCpFD *   pclSockT;
   int32_t           slBitrateT;

   //----------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpCoreBitrate() ..... :" << slBitrateV << slBrsClockV;

   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(eCP_ERR_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(eCP_ERR_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);


   //----------------------------------------------------------------
   // test parameter value
   //
   if(slBitrateV > eCP_BITRATE_MAX)
   {
      return(eCP_ERR_BITRATE);
   }

   //----------------------------------------------------------------
   // Make sure that the socket is connected.
   //


   //----------------------------------------------------------------
   // The parameter ubBaudSelV uses the same enumeration values
   // as defined in QCan::CAN_Bitrate_e, so it can be copied.
   //
   slBitrateT = slBitrateV;
   clFrameT.setBitrate(slBitrateT, slBrsClockV);

   if(pclSockT->writeFrame(clFrameT) == false)
   {
      qDebug() << "CpCoreBitrate(): Failed to set bitrate";
      return(eCP_ERR_INIT_FAIL);
   }


   return (eCP_ERR_NONE);
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
      pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);
      
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
      pclSockT->atsCanMsgM[ubBufferIdxV].ulIdentifier  = ulIdentifierV;
      pclSockT->atsCanMsgM[ubBufferIdxV].ubMsgCtrl     = ubFormatV;
      
      //--------------------------------------------------------
      // mark Tx/Rx message
      //
      if(ubDirectionV == eCP_BUFFER_DIR_TRM)
      {
         pclSockT->atsCanMsgM[ubBufferIdxV].ulMsgUser = CP_USER_FLAG_TRM;
      }
      else
      {
         pclSockT->atsCanMsgM[ubBufferIdxV].ulMsgUser = CP_USER_FLAG_RCV;
      }

      //--------------------------------------------------------
      // set acceptance mask to default value
      //
      pclSockT->atsAccMaskM[ubBufferIdxV] = ulAcceptMaskV;   
   }


   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferGetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetData( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
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
      pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);

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
            *pubDestDataV = CpMsgGetData(&(pclSockT->atsCanMsgM[ubBufferIdxV]), 
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
CpStatus_tv CpCoreBufferGetDlc(  CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubDlcV)
{
   QCanSocketCpFD *  pclSockT;
   CpStatus_tv       tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);
      
      //----------------------------------------------------------------
      // read DLC from simulated CAN buffer
      //
      *pubDlcV = pclSockT->atsCanMsgM[ubBufferIdxV].ubMsgDLC;   
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferRelease( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   QCanSocketCpFD * pclSockT;
   CpStatus_tv      tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);

      //--------------------------------------------------------
      // clear simulated CAN buffer
      //
      pclSockT->atsCanMsgM[ubBufferIdxV].ulIdentifier = 0;
      pclSockT->atsCanMsgM[ubBufferIdxV].ubMsgDLC     = 0;
      pclSockT->atsCanMsgM[ubBufferIdxV].ubMsgCtrl    = 0;
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
      pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);
      
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
         if(pclSockT->pfnTrmIntHandlerP != 0)
         {
            pfnTrmHandlerT = pclSockT->pfnTrmIntHandlerP;
            
            tvStatusT = (* pfnTrmHandlerT)(&(pclSockT->atsCanMsgM[ubBufferIdxV]),
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
CpStatus_tv CpCoreBufferSetData( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubSrcDataV,
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
      pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);

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
            CpMsgSetData(&(pclSockT->atsCanMsgM[ubBufferIdxV]), 
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
CpStatus_tv CpCoreBufferSetDlc(  CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t ubDlcV)
{
   QCanSocketCpFD *  pclSockT;
   CpStatus_tv       tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);
      
      //--------------------------------------------------------
      // write DLC to simulated CAN buffer
      //
      pclSockT->atsCanMsgM[ubBufferIdxV].ubMsgDLC = ubDlcV;
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
   QCanSocketCpFD *   pclSockT;

   //----------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpCoreMode() ........ :" << ubModeV;


   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(eCP_ERR_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(eCP_ERR_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);


   //----------------------------------------------------------------
   // check connection state
   //
   if(pclSockT->isConnected() == false)
   {
      if(ubModeV == eCP_MODE_STOP)
      {
         return(eCP_ERR_NONE);
      }
      return(eCP_ERR_GENERIC);
   }


   //----------------------------------------------------------------
   // switch CAN controller into mode "ubModeV"
   //
   switch(ubModeV)
   {
      //--------------------------------------------------------
      // Stop the CAN controller (passive on the bus)
      //
      case eCP_MODE_STOP:
         clFrameT.setMode(eCAN_MODE_STOP);
         break;

      //--------------------------------------------------------
      // Start the CAN controller (active on the bus)
      //
      case eCP_MODE_START:
         clFrameT.setMode(eCAN_MODE_START);
         break;

      //--------------------------------------------------------
      // Start the CAN controller (Listen-Only)
      //
      case eCP_MODE_LISTEN_ONLY:
         clFrameT.setMode(eCAN_MODE_LISTEN_ONLY);
         break;

      //--------------------------------------------------------
      // Other modes are not supported
      //
      default:
         return(eCP_ERR_NOT_SUPPORTED);
         break;
   }


   return(eCP_ERR_NONE);
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
      return(eCP_ERR_PARAM);
   }

   ptsStateV->ubCanErrState = eCP_STATE_BUS_ACTIVE;
   ptsStateV->ubCanErrType  = eCP_ERR_TYPE_NONE;

   //----------------------------------------------------------------
   // get current error counter
   //
   ptsStateV->ubCanTrmErrCnt = 0;
   ptsStateV->ubCanRcvErrCnt = 0;

   return(eCP_ERR_NONE);
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
   qDebug() << "CpCoreDriverInit() .. :" << ubPhyIfV << ubConfigV;

   //----------------------------------------------------------------
   // test parameter
   //
   if(ubPhyIfV >= QCAN_NETWORK_MAX)
   {
      return(eCP_ERR_CHANNEL);
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


   //----------------------------------------------------------------
   // store physical channel information
   //
   ptsPortV->ubPhyIf = ubPhyIfV;
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
   QCanSocketCpFD *   pclSockT;

   //----------------------------------------------------------------
   // get access to socket
   //
   if(ptsPortV == 0L)
   {
      return(eCP_ERR_PARAM);
   }
   if(ptsPortV->ubPhyIf >= QCAN_NETWORK_MAX)
   {
      return(eCP_ERR_PARAM);
   }
   pclSockT = &(aclCanSockListS[ptsPortV->ubPhyIf]);

   //----------------------------------------------------------------
   // store the new callbacks
   //
   pclSockT->pfnRcvIntHandlerP = pfnRcvHandler;
   pclSockT->pfnTrmIntHandlerP = pfnTrmHandler;
   pclSockT->pfnErrIntHandlerP = pfnErrHandler;


   return (eCP_ERR_NONE);
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

QCanSocketCpFD::QCanSocketCpFD()
{
   pfnRcvIntHandlerP = 0;
   pfnTrmIntHandlerP = 0;

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

   ptsCanMsgT = &(atsCanMsgM[ubMsgBufferV]);

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
   
}


//----------------------------------------------------------------------------//
// handleCanFrame()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketCpFD::handleCanFrame(QCanFrame & clCanFrameR)
{
   CpCanMsg_ts    tsCanMsgT;
   CpCanMsg_ts *  ptsCanBufT;
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
      ptsCanBufT = &(this->atsCanMsgM[ubBufferIdxT]);

      //--------------------------------------------------------
      // get acceptance mask
      //
      ulAccMaskT = this->atsAccMaskM[ubBufferIdxT];

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
            ptsCanBufT->ulIdentifier        = tsCanMsgT.ulIdentifier;
            ptsCanBufT->ubMsgDLC             = tsCanMsgT.ubMsgDLC;
            memcpy(&(ptsCanBufT->aubData[0]),
                   &(tsCanMsgT.aubData[0]),
                   CP_DATA_SIZE );
            if(this->pfnRcvIntHandlerP != 0)
            {
               (* this->pfnRcvIntHandlerP)(ptsCanBufT, ubBufferIdxT + 1);
            }

         }
      }
   }
}


//----------------------------------------------------------------------------//
// onSocketConnect()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketCpFD::onSocketConnect(void)
{
   
}


//----------------------------------------------------------------------------//
// onSocketDisconnect()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketCpFD::onSocketDisconnect(void)
{
   
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

            default:

               break;
               
         }
      }
   }
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
