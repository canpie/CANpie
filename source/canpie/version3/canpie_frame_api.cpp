//============================================================================//
// File:          canpie_frame_api.cpp                                        //
// Description:   CANpie classes - CAN API frame                              //
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

#include "canpie_frame_api.hpp"


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// CpFrameApi()                                                               //
// constructor                                                                //
//----------------------------------------------------------------------------//
CpFrameApi::CpFrameApi()
{
   this->ulIdentifierP = CAN_FRAME_TYPE_API;
   setFrameType(eTYPE_FD_EXT);
   setDlc(15);
   setData(0, CpFrameApi::eAPI_FUNC_NONE);
}


//----------------------------------------------------------------------------//
// CpFrameApi()                                                               //
// destructor                                                                 //
//----------------------------------------------------------------------------//
CpFrameApi::~CpFrameApi()
{

}

//----------------------------------------------------------------------------//
// bitrate()                                                                  //
// get bit-rate value (byte 2 .. 5)                                           //
//----------------------------------------------------------------------------//
int32_t CpFrameApi::bitrate(void)
{
   int32_t  slBitrateT = -1;

   if(data(0) == CpFrameApi::eAPI_FUNC_BITRATE)
   {
      slBitrateT = dataUInt32(2);
   }

   return(slBitrateT);
}


//----------------------------------------------------------------------------//
// brsClock()                                                                 //
// get bit-rate switch clock value (byte 6 ..9)                               //
//----------------------------------------------------------------------------//
int32_t CpFrameApi::brsClock(void)
{
   int32_t  slBrsClockT = -1;

   if(data(0) == CpFrameApi::eAPI_FUNC_BITRATE)
   {
      slBrsClockT = dataUInt32(6);
   }

   return(slBrsClockT);
}

//----------------------------------------------------------------------------//
// function()                                                                 //
// determine the function code                                                //
//----------------------------------------------------------------------------//
CpFrameApi::ApiFunc_e CpFrameApi::function(void)
{
   return((CpFrameApi::ApiFunc_e) data(0));

}


//----------------------------------------------------------------------------//
// mode()                                                                     //
// get operation mode                                                         //
//----------------------------------------------------------------------------//
CAN_Mode_e CpFrameApi::mode(void)
{
   return((CAN_Mode_e ) data(1));
}


//----------------------------------------------------------------------------//
// setBitrate()                                                               //
// Byte 2 .. 5: slBitrateV, Byte 6 .. 9: slBrsClockV                          //
//----------------------------------------------------------------------------//
void CpFrameApi::setBitrate(int32_t slBitrateV, int32_t slBrsClockV)
{
   setData(0, CpFrameApi::eAPI_FUNC_BITRATE);
   setDataUInt32(2, slBitrateV);
   setDataUInt32(6, slBrsClockV);
}


//----------------------------------------------------------------------------//
// setMode()                                                                  //
// set operation mode                                                         //
//----------------------------------------------------------------------------//
void CpFrameApi::setMode(CAN_Mode_e teModeV)
{
   setData(0, CpFrameApi::eAPI_FUNC_CAN_MODE);
   setData(1, (uint8_t) teModeV);
}


void CpFrameApi::setDriverInit()
{
   setData(0, CpFrameApi::eAPI_FUNC_DRIVER_INIT);

}
void CpFrameApi::setDriverRelease()
{
   setData(0, CpFrameApi::eAPI_FUNC_DRIVER_RELEASE);

}

