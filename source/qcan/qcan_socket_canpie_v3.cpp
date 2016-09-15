//============================================================================//
// File:          qcan_network.cpp                                            //
// Description:   QCAN classes - CAN network                                  //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53842 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions         //
// are met:                                                                   //
// 1. Redistributions of source code must retain the above copyright          //
//    notice, this list of conditions, the following disclaimer and           //
//    the referenced file 'COPYING'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'COPYING' file.                   //
//                                                                            //
//============================================================================//



/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


#include "qcan_socket.hpp"
#include "cp_core.h"
#include "cp_msg.h"



/*----------------------------------------------------------------------------*\
** Definitions                                                                **
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


//-------------------------------------------------------------------
// simulation of CAN message buffer
//
static CpCanMsg_ts atsCanMsgS[CP_BUFFER_MAX];
static uint32_t    atsAccMaskS[CP_BUFFER_MAX];


//-------------------------------------------------------------------
// these pointers store the callback handlers
//
uint8_t           (* pfnRcvIntHandler) (CpCanMsg_ts *, uint8_t);
uint8_t           (* pfnTrmIntHandler) (CpCanMsg_ts *, uint8_t);
uint8_t           (* pfnErrIntHandler) (CpState_ts *);



/*----------------------------------------------------------------------------*\
** Internal function                                                          **
**                                                                            **
\*----------------------------------------------------------------------------*/
static QCanFrame fromCpMsg(CpCanMsg_ts * ptsCanMsgV);
static void      toCpMsg(CpCanMsg_ts * ptsCanMsgV, QCanFrame & clCanFrameR);

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
   #if CP_SMALL_CODE == 0
   if(ptsPortV == 0L)
   {
      return(CpErr_PARAM);
   }
   #endif

   if(ubBaudSelV > eCP_BITRATE_MAX)
   {
      return(CpErr_BAUDRATE);
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
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);

   atsAccMaskS[ubBufferIdxV - 1] = ulAccMaskV;

   return(CpErr_OK);
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
   uint8_t  ubCntT;


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
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);


   //----------------------------------------------------------------
   // copy data from simulated CAN buffer
   //
   for(ubCntT = 0; ubCntT < 8; ubCntT++)
   {
      *pubDestDataV = CpMsgGetData(&atsCanMsgS[ubBufferIdxV - 1], ubCntT);
      pubDestDataV++;
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
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);

   //----------------------------------------------------------------
   // read DLC from simulated CAN buffer
   //
   *pubDlcV = atsCanMsgS[ubBufferIdxV - 1].ubMsgDLC;

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferInit()                                                         //
// Initialise CAN message buffer                                              //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferInit( CpPort_ts * ptsPortV, CpCanMsg_ts * ptsCanMsgV,
                              uint8_t ubBufferIdxV, uint8_t ubDirectionV)
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
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX ) return(CpErr_BUFFER);

   //----------------------------------------------------------------
   // copy to simulated CAN buffer
   //
   atsCanMsgS[ubBufferIdxV - 1].tuMsgId.ulExt = ptsCanMsgV->tuMsgId.ulExt;
   atsCanMsgS[ubBufferIdxV - 1].ubMsgDLC      = ptsCanMsgV->ubMsgDLC;

   if(ubDirectionV == eCP_BUFFER_DIR_TX)
   {
      atsCanMsgS[ubBufferIdxV - 1].ubMsgCtrl = 0x80;
   }
   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferRelease( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
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


   //-----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);


   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSend()                                                         //
// send message out of the CAN controller                                     //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSend(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   QCanFrame   clFrameT;

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX ) return(CpErr_BUFFER);

   clFrameT = fromCpMsg(&atsCanMsgS[ubBufferIdxV - 1]);
   ptsPortV->writeFrame(clFrameT);

   return (CpErr_OK);
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
   uint8_t  ubCntT;


   //-----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX ) return(CpErr_BUFFER);

   //----------------------------------------------------------------
   // copy data to simulated CAN buffer
   //
   for(ubCntT = 0; ubCntT < 8; ubCntT++)
   {
      CpMsgSetData(&atsCanMsgS[ubBufferIdxV - 1], ubCntT, *pubSrcDataV);
      pubSrcDataV++;
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
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);

   //----------------------------------------------------------------
   // write DLC to simulated CAN buffer
   //
   atsCanMsgS[ubBufferIdxV - 1].ubMsgDLC = ubDlcV;

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferTransmit()                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferTransmit(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 CpCanMsg_ts * ptsCanMsgV)
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
   // check for valid buffer number
   //
   if(ubBufferIdxV < eCP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);

   //----------------------------------------------------------------
   // copy to simulated CAN buffer
   //
   atsCanMsgS[ubBufferIdxV - 1].tuMsgId.ulExt = ptsCanMsgV->tuMsgId.ulExt;
   atsCanMsgS[ubBufferIdxV - 1].ubMsgDLC      = ptsCanMsgV->ubMsgDLC;

   return (CpErr_OK);
}

//----------------------------------------------------------------------------//
// CpCoreCanMode()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanMode(CpPort_ts * ptsPortV, uint8_t ubModeV)
{
   uint8_t  ubStatusT;

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
   // switch CAN controller into mode "ubModeV"
   //
   switch(ubModeV)
   {
      //--------------------------------------------------------
      // Stop the CAN controller (passive on the bus)
      //
      case eCP_MODE_STOP:
         ubStatusT = CpErr_OK;
         break;

      //--------------------------------------------------------
      // Start the CAN controller (active on the bus)
      //
      case eCP_MODE_START:
         ubStatusT = CpErr_OK;
         break;

      //--------------------------------------------------------
      // Start the CAN controller (Listen-Only)
      //
      case eCP_MODE_LISTEN_ONLY:
         ubStatusT = CpErr_OK;
         break;

      //--------------------------------------------------------
      // Other modes are not supported
      //
      default:
         ubStatusT = CpErr_NOT_SUPPORTED;
         break;
   }


   return(ubStatusT);
}



//----------------------------------------------------------------------------//
// CpCoreCanState()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanState(CpPort_ts * ptsPortV, CpState_ts * ptsStateV)
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
   ptsPortV = new QCanSocket();
   ptsPortV->connectNetwork((QCan::CAN_Channel_e) ubPhyIfV);

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreDriverRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverRelease(CpPort_ts * ptsPortV)
{
   CpStatus_tv tvStatusT;

   tvStatusT = CpCoreCanMode(ptsPortV, eCP_MODE_STOP);
   ptsPortV->disconnectNetwork();

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
   // store the new callbacks
   //
   pfnRcvIntHandler = pfnRcvHandler;
   pfnTrmIntHandler = pfnTrmHandler;
   pfnErrIntHandler = pfnErrHandler;


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
   return (CpErr_OK);
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



//----------------------------------------------------------------------------//
// fromCpMsg()                                                                //
// message conversion                                                         //
//----------------------------------------------------------------------------//
static QCanFrame fromCpMsg(CpCanMsg_ts * ptsCanMsgV)
{
   QCanFrame   clCanFrameT;
   uint8_t     ubDataCntT;

   if(CpMsgIsExtended(ptsCanMsgV))
   {
      clCanFrameT.setExtId(CpMsgGetExtId(ptsCanMsgV));
   }
   else
   {
      clCanFrameT.setStdId(CpMsgGetStdId(ptsCanMsgV));
   }

   clCanFrameT.setDlc(CpMsgGetDlc(ptsCanMsgV));
   for(ubDataCntT = 0; ubDataCntT < CpMsgGetDlc(ptsCanMsgV); ubDataCntT++)
   {
      clCanFrameT.setData(ubDataCntT, CpMsgGetData(ptsCanMsgV, ubDataCntT));
   }

   return(clCanFrameT);
}


//----------------------------------------------------------------------------//
// toCpMsg()                                                                  //
// message conversion                                                         //
//----------------------------------------------------------------------------//
static void toCpMsg(CpCanMsg_ts * ptsCanMsgV, QCanFrame & clCanFrameR)
{
   CpMsgClear(ptsCanMsgV);

   if(clCanFrameR.isExtended() == true)
   {
      CpMsgSetExtId(ptsCanMsgV, clCanFrameR.identifier());
   }
   else
   {
      CpMsgSetStdId(ptsCanMsgV, clCanFrameR.identifier());
   }
}
