//****************************************************************************//
// File:          device_can.c                                                //
// Description:   CANpie Core functions template                              //
// Author:        Uwe Koppe                                                   //
// e-mail:        koppe@microcontrol.net                                      //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// Lindlaustr. 2c                                                             //
// 53842 Troisdorf                                                            //
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
#include <QVector>

#include "qcan_socket_canpie_v2.hpp"
#include "cp_core.h"
#include "cp_msg.h"


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define  CP_USER_FLAG_RCV     (uint32_t)(0x00000001)
#define  CP_USER_FLAG_TRM     (uint32_t)(0x00000002)

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

static QCanSocketCp2  aclCanSockListS[QCAN_NETWORK_MAX];




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
   //----------------------------------------------------------------
   // avoid compiler warning
   //
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   printf("ptsPortV - %p \n", ptsPortV);

   qDebug() << "CpCoreBaudrate() ....." << ptsPortV;

   while(aclCanSockListS[ptsPortV->ubPhyIf].isConnected() == false)
   {
      QApplication::processEvents();
   }

   if(ubBaudSelV > CP_BAUD_MAX)
   {
      return(CpErr_BAUDRATE);
   }

   printf("OK\n");

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferAccMask()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferAccMask( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint32_t ulAccMaskV)
{
   QCanSocketCp2 *   pclSockT;

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
   QCanSocketCp2 *   pclSockT;

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
   QCanSocketCp2 *   pclSockT;

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
   QCanSocketCp2 *   pclSockT;

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
   // copy to simulated CAN buffer
   //
   pclSockT->atsCanMsgM[ubBufferIdxV - 1].tuMsgId.ulExt = ptsCanMsgV->tuMsgId.ulExt;
   pclSockT->atsCanMsgM[ubBufferIdxV - 1].ubMsgDLC      = ptsCanMsgV->ubMsgDLC;

   //----------------------------------------------------------------
   // mark Tx/Rx message
   //
   if(ubDirectionV == CP_BUFFER_DIR_TX)
   {
      pclSockT->atsCanMsgM[ubBufferIdxV - 1].ulMsgUser = CP_USER_FLAG_TRM;
   }
   else
   {
      pclSockT->atsCanMsgM[ubBufferIdxV - 1].ulMsgUser = CP_USER_FLAG_RCV;
   }


   //----------------------------------------------------------------
   // set acceptance mask to default value
   //
   pclSockT->atsAccMaskM[ubBufferIdxV - 1] = 0x1FFFFFFF;

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferRelease( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   QCanSocketCp2 *   pclSockT;

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
   QCanSocketCp2 *   pclSockT;

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
   QCanSocketCp2 *   pclSockT;

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
   QCanSocketCp2 *   pclSockT;

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
   QCanSocketCp2 *   pclSockT;
   QCanFrame         clFrameT;


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
   pclSockT->atsCanMsgM[ubBufferIdxV].tuMsgData.aulLong[0] = ptsCanMsgV->tuMsgData.aulLong[0];
   pclSockT->atsCanMsgM[ubBufferIdxV].tuMsgData.aulLong[1] = ptsCanMsgV->tuMsgData.aulLong[1];


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
   uint8_t           ubStatusT;
   QCanSocketCp2 *   pclSockT;

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

   printf("CpCoreCanMode() ......");

   while(pclSockT->isConnected() == false)
   {
      QApplication::processEvents();
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
         ubStatusT = CpErr_OK;
         break;

      //--------------------------------------------------------
      // Start the CAN controller (active on the bus)
      //
      case CP_MODE_START:
         ubStatusT = CpErr_OK;
         break;

      //--------------------------------------------------------
      // Start the CAN controller (Listen-Only)
      //
      case CP_MODE_LISTEN_ONLY:
         ubStatusT = CpErr_OK;
         break;

      //--------------------------------------------------------
      // Other modes are not supported
      //
      default:
         ubStatusT = CpErr_NOT_SUPPORTED;
         break;
   }

   printf("OK\n");
   return(ubStatusT);
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
   aclCanSockListS[ubPhyIfV].connectNetwork((QCan::CAN_Channel_e) ubPhyIfV);

   while(aclCanSockListS[ubPhyIfV].isConnected() == false)
   {
      QApplication::processEvents();
   }

   ptsPortV->ubPhyIf = ubPhyIfV;
   qDebug() << "CpCoreDriverInit() ..." << ptsPortV;
   printf("ptsPortV - %p\n", ptsPortV);
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
   QCanSocketCp2 *   pclSockT;

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

QCanSocketCp2::QCanSocketCp2()
{
   pfnRcvIntHandlerP = 0;
   pfnTrmIntHandlerP = 0;

}

//----------------------------------------------------------------------------//
// fromCpMsg()                                                                //
// message conversion                                                         //
//----------------------------------------------------------------------------//
QCanFrame QCanSocketCp2::fromCpMsg(uint8_t ubMsgBufferV)
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
void QCanSocketCp2::onSocketReceive()
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

   qDebug() << "Bingo baby .. :-)";


}


//----------------------------------------------------------------------------//
// fromCanFrame()                                                             //
// message conversion                                                         //
//----------------------------------------------------------------------------//
CpCanMsg_ts QCanSocketCp2::fromCanFrame(QCanFrame & clCanFrameR)
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
