//============================================================================//
// File:          cp_msg.c                                                    //
// Description:   CANpie message access functions                             //
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
void  CpMsgClear(CpCanMsg_ts * ptsCanMsgV)
{
   uint8_t  ubDataCntT = 0;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // clear all fields of the structure
      //
      ptsCanMsgV->tuMsgId.ulExt        = 0;

      for(ubDataCntT = 0; ubDataCntT < (CP_DATA_SIZE / 4); ubDataCntT++)
      {
         ptsCanMsgV->tuMsgData.aulLong[ubDataCntT] = 0;
      }

      ptsCanMsgV->ubMsgDLC             = 0;

      ptsCanMsgV->ubMsgCtrl            = 0;

      #if CP_CAN_MSG_TIME == 1
      ptsCanMsgV->tsMsgTime.ulSec1970  = 0;
      ptsCanMsgV->tsMsgTime.ulNanoSec  = 0;
      #endif

      #if CP_CAN_MSG_USER == 1
      ptsCanMsgV->ulMsgUser            = 0;
      #endif
   }

}


//----------------------------------------------------------------------------//
// CpMsgClrOverrun()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void CpMsgClrOverrun(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // clear overrun bit
      //
      ptsCanMsgV->ubMsgCtrl &= ~CP_MSG_CTRL_OVR_BIT;
   }
}


//----------------------------------------------------------------------------//
// CpMsgClrRemote()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void CpMsgClrRemote(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // clear remote bit
      //
      ptsCanMsgV->ubMsgCtrl &= ~CP_MSG_CTRL_RTR_BIT;

   }
}


//----------------------------------------------------------------------------//
// CpMsgGetData()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t  CpMsgGetData(CpCanMsg_ts * ptsCanMsgV, uint8_t ubPosV)
{
   uint8_t  ubDataT = 0;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // test ubPosV parameter
      //
      if((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT) > 0)
      {
         if(ubPosV < 64)
         {
            ubDataT = ptsCanMsgV->tuMsgData.aubByte[ubPosV];
         }
      }
      else
      {
         if(ubPosV <  8)
         {
            ubDataT = ptsCanMsgV->tuMsgData.aubByte[ubPosV];
         }
      }
   }


   //----------------------------------------------------------------
   // return data
   //
   return(ubDataT);
}


//----------------------------------------------------------------------------//
// CpMsgGetDlc()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t  CpMsgGetDlc(CpCanMsg_ts * ptsCanMsgV)
{
   uint8_t  ubDlcT = 0;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      ubDlcT = ptsCanMsgV->ubMsgDLC;
   }

   return(ubDlcT);
}


//----------------------------------------------------------------------------//
// CpMsgGetExtId()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
uint32_t  CpMsgGetExtId(CpCanMsg_ts * ptsCanMsgV)
{
   uint32_t  ulExtIdT = 0UL;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // mask the lower 29 bits
      //
      ulExtIdT = ptsCanMsgV->tuMsgId.ulExt & CP_MASK_EXT_FRAME;
   }

   return(ulExtIdT);
}


//----------------------------------------------------------------------------//
// CpMsgGetStdId()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
uint16_t  CpMsgGetStdId(CpCanMsg_ts * ptsCanMsgV)
{
   uint16_t  uwStdIdT = 0;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // mask the lower 11 bits
      //
      uwStdIdT = ptsCanMsgV->tuMsgId.uwStd & CP_MASK_STD_FRAME;
   }

   return(uwStdIdT);
}


//----------------------------------------------------------------------------//
// CpMsgIsBitrateSwitch()                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
bool_t  CpMsgIsBitrateSwitch(CpCanMsg_ts * ptsCanMsgV)
{
   bool_t btResultT = false;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      if((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT) > 0)
      {
         if((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_BRS_BIT) > 0)
         {
            btResultT = true;
         }
      }
   }

   return(btResultT);
}


//----------------------------------------------------------------------------//
// CpMsgIsExtended()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
bool_t  CpMsgIsExtended(CpCanMsg_ts * ptsCanMsgV)
{
   bool_t btResultT = false;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      if((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_EXT_BIT) > 0)
      {
         btResultT = true;
      }
   }

   return(btResultT);
}


//----------------------------------------------------------------------------//
// CpMsgIsFastData()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
bool_t  CpMsgIsFastData(CpCanMsg_ts * ptsCanMsgV)
{
   bool_t btResultT = false;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      if((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT) > 0)
      {
         btResultT = true;
      }
   }

   return(btResultT);
}


//----------------------------------------------------------------------------//
// CpMsgIsOverrun()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
bool_t  CpMsgIsOverrun(CpCanMsg_ts * ptsCanMsgV)
{
   bool_t btResultT = false;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      if((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_OVR_BIT) > 0)
      {
         btResultT = true;
      }
   }

   return(btResultT);
}


//----------------------------------------------------------------------------//
// CpMsgIsRemote()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
bool_t  CpMsgIsRemote(CpCanMsg_ts * ptsCanMsgV)
{
   bool_t btResultT = false;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      if((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_RTR_BIT) > 0)
      {
         btResultT = true;
      }
   }

   return(btResultT);
}


//----------------------------------------------------------------------------//
// CpMsgSetBitrateSwitch()                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetBitrateSwitch(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      ptsCanMsgV->ubMsgCtrl |= CP_MSG_CTRL_BRS_BIT;
   }
}


//----------------------------------------------------------------------------//
// CpMsgSetData()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetData(CpCanMsg_ts * ptsCanMsgV, uint8_t ubPosV, uint8_t ubValueV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // test frame type
      //
      if((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT) > 0)
      {
         if(ubPosV < 64)
         {
            ptsCanMsgV->tuMsgData.aubByte[ubPosV] = ubValueV;
         }
      }
      else
      {
         if(ubPosV <  8)
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
void  CpMsgSetDlc(CpCanMsg_ts * ptsCanMsgV, uint8_t ubDlcV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //--------------------------------------------------------
      // make sure the Data Length Code is in range
      //
      if((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT) > 0)
      {
         if(ubDlcV < 16)
         {
            ptsCanMsgV->ubMsgDLC = ubDlcV;
         }
      }
      else
      {
         if(ubDlcV <  9)
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
void  CpMsgSetExtId(CpCanMsg_ts * ptsCanMsgV, uint32_t ulExtIdV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //----------------------------------------------------------------
      // use only lower 29 bits
      //
      ulExtIdV = ulExtIdV & CP_MASK_EXT_FRAME;

      //----------------------------------------------------------------
      // mark as extended frame
      //
      ptsCanMsgV->ubMsgCtrl |= CP_MSG_CTRL_EXT_BIT;

      ptsCanMsgV->tuMsgId.ulExt = ulExtIdV;
   }
}


//----------------------------------------------------------------------------//
// CpMsgSetFastData()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetFastData(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      ptsCanMsgV->ubMsgCtrl |= CP_MSG_CTRL_FDF_BIT;
   }
}


//----------------------------------------------------------------------------//
// CpMsgSetOverrun()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void CpMsgSetOverrun(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      ptsCanMsgV->ubMsgCtrl |= CP_MSG_CTRL_OVR_BIT;
   }
}


//----------------------------------------------------------------------------//
// CpMsgSetRemote()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetRemote(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      ptsCanMsgV->ubMsgCtrl |= CP_MSG_CTRL_RTR_BIT;
   }
}


//----------------------------------------------------------------------------//
// CpMsgSetStdId()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetStdId(CpCanMsg_ts * ptsCanMsgV, uint16_t uwStdIdV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      //----------------------------------------------------------------
      // use only lower 11 bits
      //
      uwStdIdV = uwStdIdV & CP_MASK_STD_FRAME;

      //----------------------------------------------------------------
      // mark as standard frame
      //
      ptsCanMsgV->ubMsgCtrl &= ~CP_MSG_CTRL_EXT_BIT;

      ptsCanMsgV->tuMsgId.uwStd = uwStdIdV;
   }

}


//----------------------------------------------------------------------------//
// CpMsgSetTime()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetTime(CpCanMsg_ts * ptsCanMsgV, CpTime_ts * ptsTimeV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV != (CpCanMsg_ts *) 0L)
   {
      #if CP_CAN_MSG_TIME > 0
      ptsCanMsgV->tsMsgTime.ulSec1970 = ptsTimeV->ulSec1970;
      ptsCanMsgV->tsMsgTime.ulNanoSec = ptsTimeV->ulNanoSec;
      #endif
   }
}

#endif   // #if CP_CAN_MSG_MACRO == 0
