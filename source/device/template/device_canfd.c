//============================================================================//
// File:          device_can_fd.c                                             //
// Description:   CANpie core functions                                       //
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



#include "cp_core.h"
#include "cp_msg.h"


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


enum DrvInfo_e {
   eDRV_INFO_OFF = 0,
   eDRV_INFO_INIT,
   eDRV_INFO_ACTIVE
};

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
static uint32_t    aulAccMaskS[CP_BUFFER_MAX];

static uint8_t     ubCanModeS;

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


static CpStatus_tv CheckParam(const CpPort_ts * ptsPortV, 
                              const uint8_t ubBufferIdxV,
                              const uint8_t unReqStateV )
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
      if (ptsPortV->ubDrvInfo >= unReqStateV)
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
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBitrate( CpPort_ts * ptsPortV, int32_t slNomBitRateV,
                           int32_t slDatBitRateV)
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
      {
         tvStatusT = eCP_ERR_NONE;
         
         //-----------------------------------------------------
         // test bit-rate
         //
         if ((slNomBitRateV > eCP_BITRATE_1M) || 
             (slNomBitRateV == eCP_BITRATE_NONE) )
         {
            tvStatusT = eCP_ERR_BITRATE;
         }
         if ((slDatBitRateV != eCP_BITRATE_NONE) && 
             (slNomBitRateV > slDatBitRateV) )
         {
            tvStatusT = eCP_ERR_BITRATE;
         }
         
         //-----------------------------------------------------
         // configure the btr register here
         //
         if (tvStatusT == eCP_ERR_NONE)
         {
            
         }
      }
      else
      {
         tvStatusT = eCP_ERR_INIT_MISSING;
      }
   }
   
   return (tvStatusT);
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
   CpStatus_tv tvStatusT;
   
   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
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

      switch(ubDirectionV)
      {
         case eCP_BUFFER_DIR_RCV:
            aulAccMaskS[ubBufferIdxV] = ulAcceptMaskV;
            break;

         case eCP_BUFFER_DIR_TRM:

            break;
      }
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
   CpStatus_tv tvStatusT;
   uint8_t     ubCntT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
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
            *pubDestDataV = CpMsgGetData(&atsCanMsgS[ubBufferIdxV], ubCntT);
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
                                 uint8_t ubBufferIdxV,
                                 uint8_t * pubDlcV)
{
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      //----------------------------------------------------------------
      // read DLC from simulated CAN buffer
      //
      *pubDlcV = atsCanMsgS[ubBufferIdxV].ubMsgDLC;      
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
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      
   }


   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSend()                                                         //
// send message out of the CAN controller                                     //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSend( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      
   }


   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSetData( CpPort_ts * ptsPortV, 
                                 uint8_t ubBufferIdxV,
                                 uint8_t * pubSrcDataV,
                                 uint8_t   ubStartPosV,
                                 uint8_t   ubSizeV)
{
   CpStatus_tv tvStatusT;
   uint8_t     ubCntT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
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
            CpMsgSetData(&atsCanMsgS[ubBufferIdxV], ubCntT, *pubSrcDataV);
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
                                 uint8_t ubBufferIdxV,
                                 uint8_t ubDlcV)
{
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      //--------------------------------------------------------
      // write DLC to simulated CAN buffer
      //
      atsCanMsgS[ubBufferIdxV].ubMsgDLC = ubDlcV;    
   }

   return (tvStatusT);
}



//----------------------------------------------------------------------------//
// CpCoreCanMode()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanMode(CpPort_ts * ptsPortV, uint8_t ubModeV)
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
      {
         tvStatusT = eCP_ERR_NONE;
         
         //-----------------------------------------------------
         // switch CAN controller into mode "ubModeV"
         //
         switch(ubModeV)
         {
            //---------------------------------------------
            // Stop the CAN controller (passive on the bus)
            //
            case eCP_MODE_STOP:
               ubCanModeS = ubModeV;
               break;

            //---------------------------------------------
            // Start the CAN controller (active on the bus)
            //
            case eCP_MODE_START:
               ubCanModeS = ubModeV;
               break;

            //--------------------------------------------------------
            // Start the CAN controller (Listen-Only)
            //
            case eCP_MODE_LISTEN_ONLY:
               ubCanModeS = ubModeV;
               break;

            //--------------------------------------------------------
            // Operate in self-test mode
            //
            case eCP_MODE_SELF_TEST:
               ubCanModeS = ubModeV;
               break;
               
            //--------------------------------------------------------
            // Other modes are not supported
            //
            default:
               tvStatusT = eCP_ERR_NOT_SUPPORTED;
               break;
         }
      }
   }

   return(tvStatusT);
}



//----------------------------------------------------------------------------//
// CpCoreCanState()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanState(CpPort_ts * ptsPortV, CpState_ts * ptsStateV)
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
      {
         tvStatusT = eCP_ERR_NONE;

         //-----------------------------------------------------
         // get current error counter
         //
         ptsStateV->ubCanTrmErrCnt = 0;
         ptsStateV->ubCanRcvErrCnt = 0;

      }
   }

   return(tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreDriverInit()                                                         //
// init CAN controller                                                        //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverInit( uint8_t ubPhyIfV, CpPort_ts * ptsPortV,
                              uint8_t CPP_PARM_UNUSED(ubConfigV) )
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;
   
   //----------------------------------------------------------------
   // test physical CAN channel 
   //
   if (ubPhyIfV == eCP_CHANNEL_1)
   {
   
      //--------------------------------------------------------
      // test CAN port
      //
      if (ptsPortV != (CpPort_ts *) 0L)
      {
         if (ptsPortV->ubDrvInfo == eDRV_INFO_OFF)
         {
            ptsPortV->ubPhyIf   = eCP_CHANNEL_1;
            ptsPortV->ubDrvInfo = eDRV_INFO_INIT;
            
            //----------------------------------------------
            // todo: hardware initialisation
            //
            
            tvStatusT = eCP_ERR_NONE;
         }
         else
         {
            //---------------------------------------------
            // already initialised
            //
            tvStatusT = eCP_ERR_INIT_FAIL;
         }
      }
      else
      {
         //-----------------------------------------------------
         // parameter ptsPortV is not correct
         //
         tvStatusT = eCP_ERR_PARAM;
      }
      
   }

   return(tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreDriverRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverRelease(CpPort_ts * ptsPortV)
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
      {
         tvStatusT = CpCoreCanMode(ptsPortV, eCP_MODE_STOP);
         ptsPortV->ubDrvInfo = eDRV_INFO_OFF;
      }
      else
      {
         tvStatusT = eCP_ERR_INIT_MISSING;
      }
   }
   
   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreFifoConfig()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoConfig(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                             CpFifo_ts * ptsFifoV)
{
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      if(ptsFifoV != (CpFifo_ts *) 0)
      {
         aptsFifoS[ubBufferIdxV] = ptsFifoV;
      }
      else
      {
         tvStatusT = eCP_ERR_FIFO_PARAM;
      }
   }

   return(tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreFifoRead()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoRead(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                           CpCanMsg_ts * ptsCanMsgV,
                           uint32_t * pulBufferSizeV)
{
   CpFifo_ts * ptsFifoT;
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      if(pulBufferSizeV != (uint32_t *) 0L)
      {
         if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
         {
            ptsFifoT = aptsFifoS[ubBufferIdxV];
            if (CpFifoIsEmpty(ptsFifoT))
            {
               
            }
         }
      }
   }

   return(tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreFifoRelease()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoRelease(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      
   }

   return(tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreFifoWrite()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoWrite( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                             CpCanMsg_ts * ptsCanMsgV,
                             uint32_t * pulBufferSizeV)
{
   CpFifo_ts * ptsFifoT;
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      if(pulBufferSizeV != (uint32_t *) 0L)
       {
          if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
          {
             ptsFifoT = aptsFifoS[ubBufferIdxV];
             if (CpFifoIsFull(ptsFifoT))
             {
                
             }
          }
       }      
   }

   return(tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreHDI()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreHDI(CpPort_ts * ptsPortV, CpHdi_ts * ptsHdiV)
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
      {
         tvStatusT = eCP_ERR_NONE;
         
         if(ptsHdiV != (CpHdi_ts *) 0)
         {
            //--------------------------------------------------
            // get version number
            //
            ptsHdiV->ubVersionMajor = CP_VERSION_MAJOR;
            ptsHdiV->ubVersionMinor = CP_VERSION_MINOR;

            ptsHdiV->ubCanFeatures    = 0;
            ptsHdiV->ubDriverFeatures = 0;

            ptsHdiV->ubBufferMax = CP_BUFFER_MAX;
            
         }
         else
         {
            tvStatusT = eCP_ERR_PARAM;
         }
      }
   }
   

   return(tvStatusT);
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
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
      {
         tvStatusT = eCP_ERR_NONE;
         
         //-----------------------------------------------------
         // store the new callbacks
         //
         pfnRcvHandlerS = pfnRcvHandlerV;
         pfnTrmHandlerS = pfnTrmHandlerV;
         pfnErrHandlerS = pfnErrHandlerV;
      }
   }



   return(tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreStatistic()                                                          //
// return statistical information                                             //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreStatistic(CpPort_ts * ptsPortV, 
                            CpStatistic_ts * ptsStatsV)
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
      {
         tvStatusT = eCP_ERR_NONE;

         ptsStatsV->ulErrMsgCount = 0;
         ptsStatsV->ulRcvMsgCount = 0;
         ptsStatsV->ulTrmMsgCount = 0;

      }
      
   }


   return(tvStatusT);
}

