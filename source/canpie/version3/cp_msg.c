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

#error Functions are not yet adopted to CAN FD, set CP_CAN_MSG_MACRO=1


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
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return;

   //----------------------------------------------------------------
   // clear all fields of the structure
   //
   ptsCanMsgV->tuMsgId.ulExt        = 0;

   ptsCanMsgV->tuMsgData.aulLong[0] = 0;
   ptsCanMsgV->tuMsgData.aulLong[1] = 0;

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


//----------------------------------------------------------------------------//
// CpMsgClrOverrun()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void CpMsgClrOverrun(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return;

   //----------------------------------------------------------------
   // clear overrun bit
   //
   ptsCanMsgV->ubMsgCtrl &= ~CP_MSG_CTRL_OVR_BIT;
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
   if(ptsCanMsgV == 0L) return;

   //----------------------------------------------------------------
   // clear remote bit
   //
   ptsCanMsgV->ubMsgCtrl &= ~CP_MSG_CTRL_RTR_BIT;
}


//----------------------------------------------------------------------------//
// CpMsgDisable()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
/*
void  CpMsgDisable(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return;


   //----------------------------------------------------------------
   // mark as disabled frame
   //
   ptsCanMsgV->ubMsgCtrl &= ~CP_MASK_EN_BIT;
}
*/

//----------------------------------------------------------------------------//
// CpMsgEnable()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
/*
void  CpMsgEnable(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return;


   //----------------------------------------------------------------
   // mark as extended frame
   //
   ptsCanMsgV->ubMsgCtrl |= CP_MASK_EN_BIT;
}
*/

//----------------------------------------------------------------------------//
// CpMsgGetData()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t  CpMsgGetData(CpCanMsg_ts * ptsCanMsgV, uint8_t ubPosV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return(0);

   //----------------------------------------------------------------
   // test ubPosV, range from 0 to 7
   //
   if(ubPosV > 7) return (0);

   //----------------------------------------------------------------
   // return data
   //
   return (ptsCanMsgV->tuMsgData.aubByte[ubPosV]);
}


//----------------------------------------------------------------------------//
// CpMsgGetDlc()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t  CpMsgGetDlc(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return(0);

   return(ptsCanMsgV->ubMsgDLC);
}


//----------------------------------------------------------------------------//
// CpMsgGetExtId()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
uint32_t  CpMsgGetExtId(CpCanMsg_ts * ptsCanMsgV)
{
   uint32_t  ulExtIdT;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return(0);

   //----------------------------------------------------------------
   // mask the lower 29 bits
   //
   ulExtIdT = ptsCanMsgV->tuMsgId.ulExt & CP_MASK_EXT_FRAME;

   return(ulExtIdT);
}


//----------------------------------------------------------------------------//
// CpMsgGetStdId()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
uint16_t  CpMsgGetStdId(CpCanMsg_ts * ptsCanMsgV)
{
   uint16_t  uwStdIdT;

   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return(0);

   //----------------------------------------------------------------
   // mask the lower 11 bits
   //
   uwStdIdT = ptsCanMsgV->tuMsgId.uwStd & CP_MASK_STD_FRAME;

   return(uwStdIdT);
}

//----------------------------------------------------------------------------//
// CpMsgIsEnabled()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
/*
uint8_t  CpMsgIsEnabled(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return(0);

   if(ptsCanMsgV->ubMsgCtrl & CP_MASK_EN_BIT)
   {
      return(1);
   }

   return(0);
}
*/


//----------------------------------------------------------------------------//
// CpMsgIsExtended()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t  CpMsgIsExtended(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return(0);

   if(ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_EXT_BIT)
   {
      return(1);
   }

   return(0);
}

uint8_t  CpMsgIsFastData(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return(0);

   if(ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_FD_BIT)
   {
      return(1);
   }

   return(0);
}

//----------------------------------------------------------------------------//
// CpMsgIsRemote()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t  CpMsgIsRemote(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return(0);

   if(ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_RTR_BIT)
   {
      return(1);
   }

   return(0);
}

//----------------------------------------------------------------------------//
// CpMsgIsOverrun()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t  CpMsgIsOverrun(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return(0);

   if(ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_OVR_BIT)
   {
      return(1);
   }

   return(0);
}

//----------------------------------------------------------------------------//
// CpMsgIsTxRqst()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
/*
uint8_t  CpMsgIsTxRqst(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return(0);

   if(ptsCanMsgV->ubMsgCtrl & CP_MASK_TXRQ_BIT)
   {
      return(1);
   }

   return(0);
}
*/


//----------------------------------------------------------------------------//
// CpMsgSetData()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetData(CpCanMsg_ts * ptsCanMsgV, uint8_t ubPosV, uint8_t ubValueV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return;

   //----------------------------------------------------------------
   // test ubPosV, range from 0 to 7
   //
   if(ubPosV > 7) return;

   ptsCanMsgV->tuMsgData.aubByte[ubPosV] = ubValueV;
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
   if(ptsCanMsgV == 0L) return;

   //----------------------------------------------------------------
   // make sure the Data Length Code is in range
   //
   if(ubDlcV > 8) return;

   ptsCanMsgV->ubMsgDLC = ubDlcV;
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
   if(ptsCanMsgV == 0L) return;


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


//----------------------------------------------------------------------------//
// CpMsgIsOverrun()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void CpMsgSetOverrun(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return;

   ptsCanMsgV->ubMsgCtrl |= CP_MSG_CTRL_OVR_BIT;
}


//----------------------------------------------------------------------------//
// CpMsgIsRemote()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void  CpMsgSetRemote(CpCanMsg_ts * ptsCanMsgV)
{
   //----------------------------------------------------------------
   // check for valid pointer
   //
   if(ptsCanMsgV == 0L) return;
   ptsCanMsgV->ubMsgCtrl |= CP_MSG_CTRL_RTR_BIT;
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
   if(ptsCanMsgV == 0L) return;


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



#endif   // #if CP_CAN_MSG_MACRO == 0
