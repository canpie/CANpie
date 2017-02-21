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
   setFrameType(eTYPE_API);
   setMarker(CpFrameApi::eAPI_FUNC_NONE);
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
// get bit-rate value (byte 0 .. 3)                                           //
//----------------------------------------------------------------------------//
bool CpFrameApi::bitrate(int32_t & slNomBitRateV, int32_t & slDatBitRateV)
{
   bool  btResultT = false;

   if(marker() == CpFrameApi::eAPI_FUNC_BITRATE)
   {
      slNomBitRateV = dataUInt32(0);
      slDatBitRateV = dataUInt32(4);
      btResultT = true;
   }

   return(btResultT);
}


//----------------------------------------------------------------------------//
// function()                                                                 //
// determine the function code                                                //
//----------------------------------------------------------------------------//
CpFrameApi::ApiFunc_e CpFrameApi::function(void)
{
   return((CpFrameApi::ApiFunc_e) marker());

}


//----------------------------------------------------------------------------//
// mode()                                                                     //
// get operation mode                                                         //
//----------------------------------------------------------------------------//
CAN_Mode_e CpFrameApi::mode(void)
{
   return((CAN_Mode_e ) data(0));
}

bool CpFrameApi::name(QString & clNameR)
{
   bool     btResultT = false;
   uint8_t  ubPosT;

   if(marker() == CpFrameApi::eAPI_FUNC_NAME)
   {
      clNameR.clear();
      for(ubPosT = 0; ubPosT < data(0); ubPosT++)
      {
         clNameR.append(data(ubPosT + 1));
      }
      btResultT = true;
   }

   return (btResultT);
}


//----------------------------------------------------------------------------//
// setBitrate()                                                               //
// Byte 0 .. 3: slBitrateV, Byte 4 .. 7: slBrsClockV                          //
//----------------------------------------------------------------------------//
void CpFrameApi::setBitrate(int32_t slBitrateV, int32_t slBrsClockV)
{
   setMarker(CpFrameApi::eAPI_FUNC_BITRATE);
   setDataUInt32(0, slBitrateV);
   setDataUInt32(4, slBrsClockV);
}


//----------------------------------------------------------------------------//
// setMode()                                                                  //
// set operation mode                                                         //
//----------------------------------------------------------------------------//
void CpFrameApi::setMode(CAN_Mode_e teModeV)
{
   setMarker(CpFrameApi::eAPI_FUNC_CAN_MODE);
   setData(0, (uint8_t) teModeV);
}


void CpFrameApi::setDriverInit()
{
   setMarker(CpFrameApi::eAPI_FUNC_DRIVER_INIT);

}
void CpFrameApi::setDriverRelease()
{
   setMarker(CpFrameApi::eAPI_FUNC_DRIVER_RELEASE);

}

void CpFrameApi::setName(QString clNameV)
{
   int32_t  slSizeT;
   int32_t  slPosT;

   setMarker(CpFrameApi::eAPI_FUNC_NAME);

   //----------------------------------------------------------------
   // the maximum number of characters is limited to 63
   //
   slSizeT = clNameV.size();
   if(slSizeT > CAN_MSG_DATA_MAX - 1)
   {
      slSizeT = CAN_MSG_DATA_MAX - 1;
   }

   //----------------------------------------------------------------
   // copy size in byte 0 of payload
   //
   setData(0, (uint8_t) slSizeT);

   //----------------------------------------------------------------
   // copy string to payload
   //
   for (slPosT = 0; slPosT < slSizeT; slPosT++)
   {
      setData(slPosT + 1, clNameV.at(slPosT).toLatin1());
   }
}
