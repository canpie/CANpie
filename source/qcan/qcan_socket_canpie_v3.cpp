//****************************************************************************//
// File:          device_can.c                                                //
// Description:   CANpie Core functions template                              //
// Author:        Uwe Koppe                                                   //
// e-mail:        koppe@microcontrol.net                                      //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// Lindlaustr. 2c                                                             //
// 53844 Troisdorf                                                            //
// Germany                                                                    //
// Tel: +49-2241-25659-0                                                      //
// Fax: +49-2241-25659-11                                                     //
//                                                                            //
// The copyright to the computer program(s) herein is the property of         //
// MicroControl GmbH & Co. KG, Germany. The program(s) may be used            //
// and/or copied only with the written permission of MicroControl GmbH &      //
// Co. KG or in accordance with the terms and conditions stipulated in        //
// the agreement/contract under which the program(s) have been supplied.      //
//----------------------------------------------------------------------------//
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 08.02.2013  Initial version                                                //
//                                                                            //
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-12-06 16:42:14 +0100 (Sa, 06 Dez 2014) $
// SVN  $Rev: 6365 $ --- $Author: koppe $
//------------------------------------------------------------------------------


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <QApplication>
#include <QTimer>
#include <QVector>


#include "qcan_frame_api.hpp"
#include "qcan_frame_error.hpp"
#include "qcan_socket_canpie_v3.hpp"


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

static QCanSocketCp3  aclCanSockListS[QCAN_NETWORK_MAX];




/*----------------------------------------------------------------------------*\
** Function implementation                                                    **
**                                                                            **
\*----------------------------------------------------------------------------*/




//----------------------------------------------------------------------------//
// CpCoreBitrate()                                                            //
// Setup baudrate of CAN controller                                           //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBitrate( CpPort_ts * ptsPortV, int32_t slBitrateV,
                           int32_t slBrsClockV)
{
   QCanFrameApi      clFrameT;
   QCanSocketCp3 *   pclSockT;
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
   QTimer clTimeOutT;
   clTimeOutT.start(SOCKET_CONNECT_WAIT);
   while(pclSockT->isConnected() == false)
   {
      QApplication::processEvents();
      if(clTimeOutT.isActive() == false)
      {
         return(eCP_ERR_INIT_FAIL);
      }
   }

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
// CpCoreBufferGetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetData( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubDestDataV,
                                 uint8_t   ubStartPosV,
                                 uint8_t   ubSizeV)
{
   uint8_t           ubCntT;
   QCanSocketCp3 *   pclSockT;

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
   // check for valid data range
   //
   if( (ubStartPosV + ubSizeV) > CP_DATA_SIZE)
   {
      return(eCP_ERR_PARAM);
   }

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(eCP_ERR_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(eCP_ERR_BUFFER);


   //----------------------------------------------------------------
   // copy data from simulated CAN buffer
   //
   for(ubCntT = ubStartPosV; ubCntT < (ubStartPosV + ubSizeV); ubCntT++)
   {
      *pubDestDataV = CpMsgGetData(&(pclSockT->atsCanMsgM[ubBufferIdxV - 1]),
                               ubCntT);
      pubDestDataV++;
   }

   return (eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreBufferGetDlc()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetDlc(  CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubDlcV)
{
   QCanSocketCp3 *   pclSockT;

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
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(eCP_ERR_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX)  return(eCP_ERR_BUFFER);


   //----------------------------------------------------------------
   // read DLC from simulated CAN buffer
   //
   *pubDlcV = pclSockT->atsCanMsgM[ubBufferIdxV - 1].ubMsgDLC;

   return (eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreBufferInit()                                                         //
// Initialise CAN message buffer                                              //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferConfig( CpPort_ts * ptsPortV,
                                uint8_t   ubBufferIdxV,
                                uint32_t  ulIdentifierV,
                                uint32_t  ulAcceptMaskV,
                                uint8_t   ubControlV,
                                uint8_t   ubDirectionV)
{
   QCanSocketCp3 *   pclSockT;

   //----------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpCoreBufferConfig()  :" 
            << qPrintable(QString("BufIdx=%1,").arg(ubBufferIdxV, 2, 10))
            << qPrintable(QString("ID=%1,").arg(ulIdentifierV, 4, 16))
            << qPrintable(QString("Ac=%1").arg(ulAcceptMaskV, 4, 16));
            
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
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(eCP_ERR_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(eCP_ERR_BUFFER);

   //----------------------------------------------------------------
   // align buffer index
   //
   ubBufferIdxV = ubBufferIdxV - 1;

   //----------------------------------------------------------------
   // copy to simulated CAN buffer
   //
   pclSockT->atsCanMsgM[ubBufferIdxV].tuMsgId.ulExt = ulIdentifierV;
   pclSockT->atsCanMsgM[ubBufferIdxV].ubMsgCtrl     = ubControlV;


   //----------------------------------------------------------------
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


   //----------------------------------------------------------------
   // set acceptance mask to default value
   //
   pclSockT->atsAccMaskM[ubBufferIdxV] = ulAcceptMaskV;

   return (eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreBufferRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferRelease( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   QCanSocketCp3 *   pclSockT;

   //----------------------------------------------------------------
   // debug information
   //
   qDebug() << "CpCoreBufferRelease() :" 
            << qPrintable(QString("BufIdx=%1").arg(ubBufferIdxV, 2, 10));
   
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
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(eCP_ERR_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(eCP_ERR_BUFFER);

   //----------------------------------------------------------------
   // clear simulated CAN buffer
   //
   pclSockT->atsCanMsgM[ubBufferIdxV - 1].tuMsgId.ulExt = 0;
   pclSockT->atsCanMsgM[ubBufferIdxV - 1].ubMsgDLC      = 0;
   pclSockT->atsCanMsgM[ubBufferIdxV - 1].ubMsgCtrl     = 0;

   return (eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSend()                                                         //
// send message out of the CAN controller                                     //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSend(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   QCanFrame         clFrameT;
   QCanSocketCp3 *   pclSockT;

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
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(eCP_ERR_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(eCP_ERR_BUFFER);

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

   return (eCP_ERR_NONE);
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
   uint8_t           ubCntT;
   QCanSocketCp3 *   pclSockT;

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
   // check for valid data range
   //
   if( (ubStartPosV + ubSizeV) > CP_DATA_SIZE)
   {
      return(eCP_ERR_PARAM);
   }

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(eCP_ERR_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(eCP_ERR_BUFFER);

   //----------------------------------------------------------------
   // copy data to simulated CAN buffer
   //
   for(ubCntT = ubStartPosV; ubCntT < (ubStartPosV + ubSizeV); ubCntT++)
   {
      CpMsgSetData(&(pclSockT->atsCanMsgM[ubBufferIdxV - 1]),
                   ubCntT, *pubSrcDataV);
      pubSrcDataV++;
   }

   return (eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSetDlc()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSetDlc(  CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t ubDlcV)
{
   QCanSocketCp3 *   pclSockT;

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
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(eCP_ERR_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(eCP_ERR_BUFFER);

   //----------------------------------------------------------------
   // write DLC to simulated CAN buffer
   //
   pclSockT->atsCanMsgM[ubBufferIdxV - 1].ubMsgDLC = ubDlcV;

   return (eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreCanMode()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanMode(CpPort_ts * ptsPortV, uint8_t ubModeV)
{
   QCanFrameApi      clFrameT;
   QCanSocketCp3 *   pclSockT;

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
   QCanSocketCp3 *   pclSockT;

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
   QTimer clTimeOutT;
   clTimeOutT.start(SOCKET_CONNECT_WAIT);
   while(pclSockT->isConnected() == false)
   {
      QApplication::processEvents();
      if(clTimeOutT.isActive() == false)
      {
         ptsPortV->ubPhyIf = 255;   // make channel out of range
         return(eCP_ERR_INIT_FAIL);
      }
   }

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
   QCanSocketCp3 *   pclSockT;

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
// CpCoreMsgRead()                                                            //
// dummy implementation, use CPP_PARM_UNUSED to avoid compiler warning        //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreMsgRead( CpPort_ts *   CPP_PARM_UNUSED(ptsPortV), 
                           CpCanMsg_ts * CPP_PARM_UNUSED(ptsBufferV),
                           uint32_t *    CPP_PARM_UNUSED(pulBufferSizeV))
{
   return (eCP_ERR_RCV_EMPTY);
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

QCanSocketCp3::QCanSocketCp3()
{
   pfnRcvIntHandlerP = 0;
   pfnTrmIntHandlerP = 0;

}

//----------------------------------------------------------------------------//
// fromCpMsg()                                                                //
// message conversion                                                         //
//----------------------------------------------------------------------------//
QCanFrame QCanSocketCp3::fromCpMsg(uint8_t ubMsgBufferV)
{
   QCanFrame      clCanFrameT;
   CpCanMsg_ts *  ptsCanMsgT;
   uint8_t        ubDataCntT;

   ptsCanMsgT = &(atsCanMsgM[ubMsgBufferV]);

   if(CpMsgIsFastData(ptsCanMsgT))
   {
      if(CpMsgIsExtended(ptsCanMsgT))
      {
         clCanFrameT.setFrameType(QCanFrame::eTYPE_FD_EXT);
         clCanFrameT.setIdentifier(CpMsgGetExtId(ptsCanMsgT));
      }
      else
      {
         clCanFrameT.setFrameType(QCanFrame::eTYPE_FD_STD);
         clCanFrameT.setIdentifier(CpMsgGetStdId(ptsCanMsgT));
      }
   }
   else
   {
      if(CpMsgIsExtended(ptsCanMsgT))
      {
         clCanFrameT.setFrameType(QCanFrame::eTYPE_CAN_EXT);
         clCanFrameT.setIdentifier(CpMsgGetExtId(ptsCanMsgT));
      }
      else
      {
         clCanFrameT.setFrameType(QCanFrame::eTYPE_CAN_STD);
         clCanFrameT.setIdentifier(CpMsgGetStdId(ptsCanMsgT));
      }
   }

   clCanFrameT.setDlc(CpMsgGetDlc(ptsCanMsgT));

   for(ubDataCntT = 0; ubDataCntT < clCanFrameT.dataSize(); ubDataCntT++)
   {
      clCanFrameT.setData(ubDataCntT, CpMsgGetData(ptsCanMsgT, ubDataCntT));
   }

   return(clCanFrameT);
}


//----------------------------------------------------------------------------//
// onSocketReceive()                                                          //
// receive CAN message                                                        //
//----------------------------------------------------------------------------//
void QCanSocketCp3::onSocketReceive()
{
   QCanFrame      clFrameT;
   CpCanMsg_ts    tsCanMsgT;
   CpCanMsg_ts *  ptsCanBufT;
   uint32_t       ulFrameCntT;
   uint32_t       ulFrameMaxT;
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
               memcpy(&(ptsCanBufT->tuMsgData.aubByte[0]),
                      &(tsCanMsgT.tuMsgData.aulLong[0]),
                      CP_DATA_SIZE );
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
                memcpy(&(ptsCanBufT->tuMsgData.aubByte[0]),
                       &(tsCanMsgT.tuMsgData.aulLong[0]),
                       CP_DATA_SIZE );
                #if CP_CAN_FD > 0

                #endif
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
CpCanMsg_ts QCanSocketCp3::fromCanFrame(QCanFrame & clCanFrameR)
{
   CpCanMsg_ts    tsCanMsgT;
   uint8_t        ubDataCntT;


   CpMsgClear(&tsCanMsgT);

   switch(clCanFrameR.frameFormat())
   {
      case eTYPE_CAN_STD:
         CpMsgSetStdId(&tsCanMsgT, clCanFrameR.identifier());
         break;

      case eTYPE_CAN_EXT:
         CpMsgSetExtId(&tsCanMsgT, clCanFrameR.identifier());
         break;
         
      case eTYPE_FD_STD:
         CpMsgSetFastData(tsCanMsgT);
         CpMsgSetStdId(&tsCanMsgT, clCanFrameR.identifier());
         break;
         
      case eTYPE_FD_EXT:
         CpMsgSetFastData(tsCanMsgT);
         CpMsgSetExtId(&tsCanMsgT, clCanFrameR.identifier());
         break;
   }
   
   CpMsgSetDlc(&tsCanMsgT, clCanFrameR.dlc());

   for(ubDataCntT = 0; ubDataCntT < 64; ubDataCntT++)
   {
      CpMsgSetData(&tsCanMsgT, ubDataCntT, clCanFrameR.data(ubDataCntT));
   }
   return(tsCanMsgT);
}
