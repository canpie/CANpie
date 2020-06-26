//============================================================================//
// File:          cp_msg.c                                                    //
// Description:   CANpie message access functions                             //
//                                                                            //
// Copyright 2020 MicroControl GmbH & Co. KG                                  //
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
#include "cp_msg.h"


//-------------------------------------------------------------------
// This file can only be compiled if the symbol CP_CAN_MSG_MACRO is
// 0 (or not defined). Otherwise macros are already defined and we
// will get a compiler error.
//
#if CP_CAN_MSG_MACRO == 0


/*----------------------------------------------------------------------------*\
** Functions                                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// CpMsgClear()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgClear(CpCanMsg_ts *ptsCanMsgV)
{
   uint8_t  ubDataCntT = (uint8_t) 0;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // clear all fields of the structure
      //
      ptsCanMsgV->ulIdentifier        = 0UL;

      for (ubDataCntT = 0; ubDataCntT < (uint8_t) CP_DATA_SIZE; ubDataCntT++)
      {
         ptsCanMsgV->tuMsgData.aubByte[ubDataCntT] = (uint8_t) 0;
      }

      ptsCanMsgV->ubMsgDLC             = (uint8_t) 0;

      ptsCanMsgV->ubMsgCtrl            = (uint8_t) 0;

      #if CP_CAN_MSG_TIME == 1
      ptsCanMsgV->tsMsgTime.ulSec1970  = 0UL;
      ptsCanMsgV->tsMsgTime.ulNanoSec  = 0UL;
      #endif

      #if CP_CAN_MSG_USER == 1
      ptsCanMsgV->ulMsgUser            = 0UL;
      #endif

      #if CP_CAN_MSG_MARKER == 1
      ptsCanMsgV->ulMsgMarker          = 0UL;
      #endif

   }

}


//----------------------------------------------------------------------------//
// CpMsgClrOverrun()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void CpMsgClrOverrun(CpCanMsg_ts *ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // clear overrun bit
      //
      ptsCanMsgV->ubMsgCtrl &= ~(CP_MSG_CTRL_OVR_BIT);
   }
}


//----------------------------------------------------------------------------//
// CpMsgClrRemote()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void CpMsgClrRemote(CpCanMsg_ts *ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // clear remote bit
      //
      ptsCanMsgV->ubMsgCtrl &= ~CP_MSG_CTRL_RTR_BIT;

   }
}


//----------------------------------------------------------------------------//
// CpMsgDlcToSize()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
CPP_INLINE uint8_t CpMsgDlcToSize(const uint8_t ubDlcV)
{
   uint8_t  ubSizeT;

   switch(ubDlcV)
   {
      case 0:
      case 1: case 2: case 3: case 4:
      case 5: case 6: case 7: case 8:
         ubSizeT = ubDlcV;
         break;

      case 9:
         ubSizeT = 12;
         break;

      case 10:
         ubSizeT = 16;
         break;

      case 11:
         ubSizeT = 20;
         break;

      case 12:
         ubSizeT = 24;
         break;

      case 13:
         ubSizeT = 32;
         break;

      case 14:
         ubSizeT = 48;
         break;

      case 15:
         ubSizeT = 64;
         break;

      default:
         ubSizeT = 0;
         break;

   }

   return(ubSizeT);
}

//----------------------------------------------------------------------------//
// CpMsgGetData()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t  CpMsgGetData(const CpCanMsg_ts *ptsCanMsgV, uint8_t ubPosV)
{
   uint8_t  ubDataT = (uint8_t) 0;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // test ubPosV parameter
      //
      if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT) > 0)
      {
         if (ubPosV < 64)
         {
            ubDataT = ptsCanMsgV->tuMsgData.aubByte[ubPosV];
         }
      }
      else
      {
         if (ubPosV <  8)
         {
            ubDataT = ptsCanMsgV->tuMsgData.aubByte[ubPosV];
         }
      }
   }


   //----------------------------------------------------------------
   // return data
   //
   return (ubDataT);
}


//----------------------------------------------------------------------------//
// CpMsgGetDlc()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t  CpMsgGetDlc(const CpCanMsg_ts *ptsCanMsgV)
{
   uint8_t  ubDlcT = 0;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      ubDlcT = ptsCanMsgV->ubMsgDLC;
   }

   return (ubDlcT);
}


//----------------------------------------------------------------------------//
// CpMsgGetExtId()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
uint32_t  CpMsgGetExtId(const CpCanMsg_ts *ptsCanMsgV)
{
   uint32_t  ulExtIdT = 0UL;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // mask the lower 29 bits
      //
      ulExtIdT = ptsCanMsgV->ulIdentifier & CP_MASK_EXT_FRAME;
   }

   return (ulExtIdT);
}


//----------------------------------------------------------------------------//
// CpMsgGetIdentifier()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
uint32_t  CpMsgGetIdentifier(const CpCanMsg_ts *ptsCanMsgV)
{
   uint32_t  ulIdentifierT = 0UL;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_EXT_BIT) > 0)
      {
         //-----------------------------------------------------
         // mask the lower 29 bits
         //
         ulIdentifierT = ptsCanMsgV->ulIdentifier & CP_MASK_EXT_FRAME;
      }
      else
      {
         //-----------------------------------------------------
         // mask the lower 11 bits
         //
         ulIdentifierT = ptsCanMsgV->ulIdentifier & CP_MASK_STD_FRAME;
      }
   }

   return (ulIdentifierT);
}


//----------------------------------------------------------------------------//
// CpMsgGetStdId()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
uint16_t  CpMsgGetStdId(const CpCanMsg_ts *ptsCanMsgV)
{
   uint16_t  uwStdIdT = (uint16_t) 0;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // mask the lower 11 bits
      //
      uwStdIdT = (uint16_t)(ptsCanMsgV->ulIdentifier & CP_MASK_STD_FRAME);
   }

   return (uwStdIdT);
}


//----------------------------------------------------------------------------//
// CpMsgGetTime()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
CpTime_ts *CpMsgGetTime(CpCanMsg_ts *ptsCanMsgV)
{
   CpTime_ts *ptsTimeT = CPP_NULL;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      #if CP_CAN_MSG_TIME > 0
      /*@ -immediatetrans @*/
      ptsTimeT = &(ptsCanMsgV->tsMsgTime);
      #endif
   }
   return (ptsTimeT);
}


//----------------------------------------------------------------------------//
// CpMsgInit()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgInit(CpCanMsg_ts *ptsCanMsgV, uint8_t ubFormatV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // initialise identifier, DLC and frame format
      //
      ptsCanMsgV->ulIdentifier = 0UL;
      ptsCanMsgV->ubMsgDLC     = (uint8_t) 0;
      ptsCanMsgV->ubMsgCtrl    = ubFormatV & CP_MASK_MSG_FORMAT;
   }
}


//----------------------------------------------------------------------------//
// CpMsgIsBitrateSwitchSet()                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
bool_t  CpMsgIsBitrateSwitchSet(const CpCanMsg_ts *ptsCanMsgV)
{
   bool_t btResultT = false;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT) > 0)
      {
         if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_BRS_BIT) > 0)
         {
            btResultT = true;
         }
      }
   }

   return (btResultT);
}


//----------------------------------------------------------------------------//
// CpMsgIsExtended()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
bool_t  CpMsgIsExtended(const CpCanMsg_ts *ptsCanMsgV)
{
   bool_t btResultT = false;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_EXT_BIT) > 0)
      {
         btResultT = true;
      }
   }

   return (btResultT);
}


//----------------------------------------------------------------------------//
// CpMsgIsFdFrame()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
bool_t  CpMsgIsFdFrame(const CpCanMsg_ts *ptsCanMsgV)
{
   bool_t btResultT = false;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT) > 0)
      {
         btResultT = true;
      }
   }

   return (btResultT);
}


//----------------------------------------------------------------------------//
// CpMsgIsOverrun()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
bool_t  CpMsgIsOverrun(const CpCanMsg_ts *ptsCanMsgV)
{
   bool_t btResultT = false;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_OVR_BIT) > 0)
      {
         btResultT = true;
      }
   }

   return (btResultT);
}


//----------------------------------------------------------------------------//
// CpMsgIsRemote()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
bool_t  CpMsgIsRemote(const CpCanMsg_ts *ptsCanMsgV)
{
   bool_t btResultT = false;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_RTR_BIT) > 0)
      {
         btResultT = true;
      }
   }

   return (btResultT);
}


//----------------------------------------------------------------------------//
// CpMsgIsRpc()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
bool_t  CpMsgIsRpc(const CpCanMsg_ts *ptsCanMsgV)
{
   bool_t btResultT = false;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_RPC_BIT) > 0)
      {
         btResultT = true;
      }
   }

   return (btResultT);
}

//----------------------------------------------------------------------------//
// CpMsgRpcGetCanMode()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t CpMsgRpcGetCanMode(const CpCanMsg_ts *ptsCanMsgV)
{
   uint8_t ubModeT = eCP_MODE_INIT;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      ubModeT = ptsCanMsgV->ubMsgDLC;
   }

   return ubModeT;
}

//----------------------------------------------------------------------------//
// CpMsgRpcGetDataBitrate()                                                   //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t CpMsgRpcGetDataBitrate(const CpCanMsg_ts *ptsCanMsgV)
{
   int32_t slBitrateT = eCP_BITRATE_NONE;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      slBitrateT = (int32_t)(ptsCanMsgV->tuMsgData.aulLong[1]);
   }

   return slBitrateT;
}

//----------------------------------------------------------------------------//
// CpMsgRpcGetNominalBitrate()                                                //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t CpMsgRpcGetNominalBitrate(const CpCanMsg_ts *ptsCanMsgV)
{
   int32_t slBitrateT = eCP_BITRATE_NONE;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      slBitrateT = (int32_t)(ptsCanMsgV->tuMsgData.aulLong[0]);
   }

   return slBitrateT;
}

//----------------------------------------------------------------------------//
// CpMsgRpcSetCanMode()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgRpcSetCanMode(CpCanMsg_ts *ptsCanMsgV, uint8_t ubModeV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      ptsCanMsgV->ubMsgDLC = ubModeV;

      ptsCanMsgV->ulIdentifier = (uint32_t)(eCP_RPC_MODE);
      ptsCanMsgV->ubMsgCtrl = (uint8_t)(CP_MSG_CTRL_RPC_BIT);
   }
}


//----------------------------------------------------------------------------//
// CpMsgRpcType()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
uint32_t  CpMsgRpcType(const CpCanMsg_ts *ptsCanMsgV)
{
   uint32_t  ulRpcT = eCP_RPC_NOOP;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      ulRpcT = ptsCanMsgV->ulIdentifier;
      if (ulRpcT >= eCP_RPC_MAX)
      {
          ulRpcT = eCP_RPC_NOOP;
      }
   }

   return (ulRpcT);
}  


//----------------------------------------------------------------------------//
// CpMsgRpcSetBitrate()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgRpcSetBitrate(CpCanMsg_ts *ptsCanMsgV, int32_t slNomBitRateV,
                         int32_t slDatBitRateV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      ptsCanMsgV->tuMsgData.aulLong[0] = (uint32_t)(slNomBitRateV);
      ptsCanMsgV->tuMsgData.aulLong[1] = (uint32_t)(slDatBitRateV);

      ptsCanMsgV->ulIdentifier = (uint32_t)(eCP_RPC_BITRATE);
      ptsCanMsgV->ubMsgCtrl = (uint8_t)(CP_MSG_CTRL_RPC_BIT);
   }
}

//----------------------------------------------------------------------------//
// CpMsgSetBitrateSwitch()                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetBitrateSwitch(CpCanMsg_ts *ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      ptsCanMsgV->ubMsgCtrl |= CP_MSG_CTRL_BRS_BIT;
   }
}


//----------------------------------------------------------------------------//
// CpMsgSetData()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetData(CpCanMsg_ts *ptsCanMsgV, uint8_t ubPosV, uint8_t ubValueV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // test frame type
      //
      if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT) > 0)
      {
         if (ubPosV < 64)
         {
            ptsCanMsgV->tuMsgData.aubByte[ubPosV] = ubValueV;
         }
      }
      else
      {
         if (ubPosV <  8)
         {
            ptsCanMsgV->tuMsgData.aubByte[ubPosV] = ubValueV;
         }
      }
   }
}


//----------------------------------------------------------------------------//
// CpMsgSetDlc()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetDlc(CpCanMsg_ts *ptsCanMsgV, uint8_t ubDlcV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // make sure the Data Length Code is in range
      //
      if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT) > 0)
      {
         if (ubDlcV < 16)
         {
            ptsCanMsgV->ubMsgDLC = ubDlcV;
         }
      }
      else
      {
         if (ubDlcV <  9)
         {
            ptsCanMsgV->ubMsgDLC = ubDlcV;
         }
      }
   }
}


//----------------------------------------------------------------------------//
// CpMsgSetExtId()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetExtId(CpCanMsg_ts *ptsCanMsgV, uint32_t ulExtIdV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //----------------------------------------------------------------
      // use only lower 29 bits
      //
      ulExtIdV = ulExtIdV & CP_MASK_EXT_FRAME;

      //----------------------------------------------------------------
      // mark as extended frame
      //
      ptsCanMsgV->ubMsgCtrl |= CP_MSG_CTRL_EXT_BIT;

      ptsCanMsgV->ulIdentifier = ulExtIdV;
   }
}


//----------------------------------------------------------------------------//
// CpMsgSetIdentifier()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetIdentifier(CpCanMsg_ts *ptsCanMsgV, uint32_t ulIdentifierV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_EXT_BIT) > 0)
      {
         //-----------------------------------------------------
         // mask the lower 29 bits
         //
         ptsCanMsgV->ulIdentifier = ulIdentifierV & CP_MASK_EXT_FRAME;
      }
      else
      {
         //-----------------------------------------------------
         // mask the lower 11 bits
         //
         ptsCanMsgV->ulIdentifier = ulIdentifierV & CP_MASK_STD_FRAME;
      }
   }
}


//----------------------------------------------------------------------------//
// CpMsgSetOverrun()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void CpMsgSetOverrun(CpCanMsg_ts *ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      ptsCanMsgV->ubMsgCtrl |= CP_MSG_CTRL_OVR_BIT;
   }
}


//----------------------------------------------------------------------------//
// CpMsgSetRemote()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetRemote(CpCanMsg_ts *ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT) == 0)
      {
         ptsCanMsgV->ubMsgCtrl |= CP_MSG_CTRL_RTR_BIT;
      }
   }
}


//----------------------------------------------------------------------------//
// CpMsgSetStdId()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetStdId(CpCanMsg_ts *ptsCanMsgV, uint16_t uwStdIdV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //----------------------------------------------------------------
      // use only lower 11 bits
      //
      uwStdIdV = uwStdIdV & CP_MASK_STD_FRAME;

      //----------------------------------------------------------------
      // mark as standard frame
      //
      ptsCanMsgV->ubMsgCtrl &= ~CP_MSG_CTRL_EXT_BIT;

      ptsCanMsgV->ulIdentifier = (uint32_t) uwStdIdV;
   }

}


//----------------------------------------------------------------------------//
// CpMsgSetTime()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
#if CP_CAN_MSG_TIME > 0
void  CpMsgSetTime(CpCanMsg_ts *ptsCanMsgV, const CpTime_ts *ptsTimeV)
#else
void  CpMsgSetTime(CpCanMsg_ts *ptsCanMsgV, const CpTime_ts * CPP_PARM_UNUSED(ptsTimeV))
#endif
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      #if CP_CAN_MSG_TIME > 0
      ptsCanMsgV->tsMsgTime.ulSec1970 = (ptsTimeV->ulSec1970);
      ptsCanMsgV->tsMsgTime.ulNanoSec = (ptsTimeV->ulNanoSec);
      #endif
   }
}


//----------------------------------------------------------------------------//
// CpMsgSizeToDlc()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
CPP_INLINE uint8_t CpMsgSizeToDlc(uint8_t ubSizeV)
{
   uint8_t  ubDlcT;
   //--------------------------------------------------------
   // set DLC value to maximum requested size value
   //
   if(ubSizeV <= 8)
   {
      ubDlcT = ubSizeV;
   }
   else if((ubSizeV > 8) && (ubSizeV <= 12))
   {
      ubDlcT = 9;
   }
   else if((ubSizeV > 12) && (ubSizeV <= 16))
   {
      ubDlcT = 10;
   }
   else if((ubSizeV > 16) && (ubSizeV <= 20))
   {
      ubDlcT = 11;
   }
   else if((ubSizeV > 20) && (ubSizeV <= 24))
   {
      ubDlcT = 12;
   }
   else if((ubSizeV > 24) && (ubSizeV <= 32))
   {
      ubDlcT = 13;
   }
   else if((ubSizeV > 32) && (ubSizeV <= 48))
   {
      ubDlcT = 14;
   }
   else
   {
      ubDlcT = 15;
   }

   return (ubDlcT);
}

#endif   // #if CP_CAN_MSG_MACRO == 0
