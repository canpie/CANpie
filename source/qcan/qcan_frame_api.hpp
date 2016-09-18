//============================================================================//
// File:          qcan_frame_api.hpp                                          //
// Description:   QCAN classes - CAN API frame                                //
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



#ifndef QCAN_FRAME_API_HPP_
#define QCAN_FRAME_API_HPP_


#include "qcan_frame.hpp"


//-----------------------------------------------------------------------------
/*!
** \class   QCanFrameApi
** \brief   CAN API frame
** 
** This class defines a CAN API frame.
*/
class QCanFrameApi : private QCanFrame
{
public:
   

   enum ApiFunc_e {

      eAPI_FUNC_NONE   = 0,
      eAPI_FUNC_BITRATE,
      eAPI_FUNC_CAN_MODE,
      eAPI_FUNC_DRIVER_INIT,
      eAPI_FUNC_DRIVER_RELEASE,
      eAPI_FUNC_PROCESS_ID
   };
   QCanFrameApi();
   
   
   ~QCanFrameApi();

   int32_t  bitrate(void);
   int32_t  brsClock(void);

   ApiFunc_e function(void);

   void setBitrate(int32_t slBitrateV, int32_t slBrsClockV);

   void setDriverInit();
   void setDriverRelease();

   virtual QString   toString(const bool & btShowTimeR = false);
   
private:
   
   
};





#endif   // QCAN_FRAME_HPP_ 

