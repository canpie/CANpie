//============================================================================//
// File:          device_can_fd.c                                             //
// Description:   CANpie core functions                                       //
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

static CpFifo_ts * aptsFifoS[CP_BUFFER_MAX];


//-------------------------------------------------------------------
// these pointers store the callback handlers
//
static CpRcvHandler_Fn  /*@null@*/  pfnRcvHandlerS = CPP_NULL;
static CpTrmHandler_Fn  /*@null@*/  pfnTrmHandlerS = CPP_NULL;
static CpErrHandler_Fn  /*@null@*/  pfnErrHandlerS = CPP_NULL;


/*----------------------------------------------------------------------------*\
** Function implementation                                                    **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// CpCoreBitrate()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBitrate( CpPort_ts * ptsPortV, int32_t slNomBitRateV,
                           int32_t slDatBitRateV)
{
   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif

   //----------------------------------------------------------------
   // test bit-rate
   //
   if(slNomBitRateV > eCP_BITRATE_1M)
   {
      return (eCP_ERR_BITRATE);
   }
   if(slNomBitRateV > slDatBitRateV)
   {
      return (eCP_ERR_BITRATE);
   }

   return (eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreBufferConfig()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferConfig( CpPort_ts * ptsPortV,
                                uint8_t   ubBufferIdxV,
                                uint32_t  ulIdentifierV,
                                uint32_t  ulAcceptMaskV,
                                uint8_t   ubFormatV,
                                uint8_t   ubDirectionV)
{
   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if((ubBufferIdxV < eCP_BUFFER_1  ) || (ubBufferIdxV > CP_BUFFER_MAX) )
   {
      return(eCP_ERR_BUFFER);
   }

   //----------------------------------------------------------------
   // test message format and mask identifier
   //
   switch(ubFormatV & CP_MSG_FORMAT_MASK)
   {
      case CP_MSG_FORMAT_CBFF:
         ulIdentifierV = ulIdentifierV & CP_MASK_STD_FRAME;
         ulAcceptMaskV = ulAcceptMaskV & CP_MASK_STD_FRAME;
         break;

      case CP_MSG_FORMAT_CEFF:
         ulIdentifierV = ulIdentifierV & CP_MASK_EXT_FRAME;
         ulAcceptMaskV = ulAcceptMaskV & CP_MASK_EXT_FRAME;
         break;


   }

   switch(ubDirectionV)
   {
      case eCP_BUFFER_DIR_RCV:

         break;

      case eCP_BUFFER_DIR_TRM:

         break;
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
   uint8_t  ubCntT;


   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if((ubBufferIdxV < eCP_BUFFER_1  ) || (ubBufferIdxV > CP_BUFFER_MAX) )
   {
      return(eCP_ERR_BUFFER);
   }

   //----------------------------------------------------------------
   // test start position and size
   //
   #if CP_CAN_FD > 0
   #else
   if(ubStartPosV > CP_DATA_SIZE)              return(eCP_ERR_PARAM);
   if(ubSizeV > 8)                  return(eCP_ERR_PARAM);
   if((ubStartPosV + ubSizeV) > 8)  return(eCP_ERR_PARAM);

   #endif

   //----------------------------------------------------------------
   // copy data from simulated CAN buffer
   //
   for(ubCntT = ubStartPosV; ubCntT < ubSizeV; ubCntT++)
   {
      *pubDestDataV = CpMsgGetData(&atsCanMsgS[ubBufferIdxV - 1], ubCntT);
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
   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if((ubBufferIdxV < eCP_BUFFER_1  ) || (ubBufferIdxV > CP_BUFFER_MAX) )
   {
      return(eCP_ERR_BUFFER);
   }

   //----------------------------------------------------------------
   // read DLC from simulated CAN buffer
   //
   *pubDlcV = atsCanMsgS[ubBufferIdxV - 1].ubMsgDLC;

   return (eCP_ERR_NONE);
}



//----------------------------------------------------------------------------//
// CpCoreBufferRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferRelease( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif


   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if((ubBufferIdxV < eCP_BUFFER_1  ) || (ubBufferIdxV > CP_BUFFER_MAX) )
   {
      return(eCP_ERR_BUFFER);
   }


   return (eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSend()                                                         //
// send message out of the CAN controller                                     //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSend(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if((ubBufferIdxV < eCP_BUFFER_1  ) || (ubBufferIdxV > CP_BUFFER_MAX) )
   {
      return(eCP_ERR_BUFFER);
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
   uint8_t  ubCntT;

   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif


   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if((ubBufferIdxV < eCP_BUFFER_1  ) || (ubBufferIdxV > CP_BUFFER_MAX) )
   {
      return(eCP_ERR_BUFFER);
   }

   //----------------------------------------------------------------
   // copy data to simulated CAN buffer
   //
   for(ubCntT = ubStartPosV; ubCntT < ubSizeV; ubCntT++)
   {
      CpMsgSetData(&atsCanMsgS[ubBufferIdxV - 1], ubCntT, *pubSrcDataV);
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
   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif


   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if((ubBufferIdxV < eCP_BUFFER_1  ) || (ubBufferIdxV > CP_BUFFER_MAX) )
   {
      return(eCP_ERR_BUFFER);
   }

   //----------------------------------------------------------------
   // write DLC to simulated CAN buffer
   //
   atsCanMsgS[ubBufferIdxV - 1].ubMsgDLC = ubDlcV;

   return (eCP_ERR_NONE);
}



//----------------------------------------------------------------------------//
// CpCoreCanMode()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanMode(CpPort_ts * ptsPortV, uint8_t ubModeV)
{
   uint8_t  ubStatusT;

   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
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
         ubStatusT = eCP_ERR_NONE;
         break;

      //--------------------------------------------------------
      // Start the CAN controller (active on the bus)
      //
      case eCP_MODE_START:
         ubStatusT = eCP_ERR_NONE;
         break;

      //--------------------------------------------------------
      // Start the CAN controller (Listen-Only)
      //
      case eCP_MODE_LISTEN_ONLY:
         ubStatusT = eCP_ERR_NONE;
         break;

      //--------------------------------------------------------
      // Other modes are not supported
      //
      default:
         ubStatusT = eCP_ERR_NOT_SUPPORTED;
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
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif


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
CpStatus_tv CpCoreDriverInit( uint8_t ubPhyIfV, CpPort_ts * ptsPortV,
                              uint8_t CPP_PARM_UNUSED(ubConfigV) )
{

   if(ubPhyIfV != eCP_CHANNEL_1)
   {
      return(eCP_ERR_CHANNEL);
   }

   if(ptsPortV != (CpPort_ts *) 0L)
   {

   }

   return(eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreDriverRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverRelease(CpPort_ts * ptsPortV)
{
   CpStatus_tv tvStatusT;

   tvStatusT = CpCoreCanMode(ptsPortV, eCP_MODE_STOP);


   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreFifoConfig()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoConfig(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                             CpFifo_ts * ptsFifoV)
{
   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if((ubBufferIdxV < eCP_BUFFER_1  ) || (ubBufferIdxV > CP_BUFFER_MAX) )
   {
      return(eCP_ERR_BUFFER);
   }

   if(ptsFifoV != 0)
   {
      aptsFifoS[ubBufferIdxV - 1] = ptsFifoV;
   }
   return(eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreFifoRead()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoRead(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                           CpCanMsg_ts * ptsCanMsgV,
                           uint32_t * pulBufferSizeV)
{

   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if((ubBufferIdxV < eCP_BUFFER_1  ) || (ubBufferIdxV > CP_BUFFER_MAX) )
   {
      return(eCP_ERR_BUFFER);
   }

   if(pulBufferSizeV != (uint32_t *) 0L)
   {
      if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
      {

      }
   }

   return(eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreFifoRelease()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoRelease(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if((ubBufferIdxV < eCP_BUFFER_1  ) || (ubBufferIdxV > CP_BUFFER_MAX) )
   {
      return(eCP_ERR_BUFFER);
   }

   return(eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreFifoWrite()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoWrite(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                             CpCanMsg_ts * ptsCanMsgV,
                             uint32_t * pulBufferSizeV)
{
   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif

   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if((ubBufferIdxV < eCP_BUFFER_1  ) || (ubBufferIdxV > CP_BUFFER_MAX) )
   {
      return(eCP_ERR_BUFFER);
   }

   if(pulBufferSizeV != (uint32_t *) 0L)
   {
      if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
      {

      }

   }

   return(eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreHDI()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreHDI(CpPort_ts * ptsPortV, CpHdi_ts * ptsHdiV)
{
   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif

   //----------------------------------------------------------------
   // get version number
   //
   ptsHdiV->ubVersionMajor = CP_VERSION_MAJOR;
   ptsHdiV->ubVersionMinor = CP_VERSION_MINOR;

   ptsHdiV->ubCanFeatures = 0;
   ptsHdiV->ubDriverFeatures = 0,

   ptsHdiV->ubBufferMax = CP_BUFFER_MAX;
   return(eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreIntFunctions()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreIntFunctions(CpPort_ts * ptsPortV,
                               CpRcvHandler_Fn pfnRcvHandlerV,
                               CpTrmHandler_Fn pfnTrmHandlerV,
                               CpErrHandler_Fn pfnErrHandlerV )
{
   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif

   //----------------------------------------------------------------
   // store the new callbacks
   //
   pfnRcvHandlerS = pfnRcvHandlerV;
   pfnTrmHandlerS = pfnTrmHandlerV;
   pfnErrHandlerS = pfnErrHandlerV;


   return (eCP_ERR_NONE);
}


//----------------------------------------------------------------------------//
// CpCoreStatistic()                                                          //
// return statistical information                                             //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreStatistic(CpPort_ts * ptsPortV, CpStatistic_ts * ptsStatsV)
{
   //----------------------------------------------------------------
   // test CAN port
   //
   #if CP_SMALL_CODE == 0
   if(ptsPortV == (CpPort_ts *) 0L)
   {
      return(eCP_ERR_CHANNEL);
   }
   #endif

   ptsStatsV->ulErrMsgCount = 0;
   ptsStatsV->ulRcvMsgCount = 0;
   ptsStatsV->ulTrmMsgCount = 0;

   return(eCP_ERR_NONE);
}

