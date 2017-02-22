//============================================================================//
// File:          qcan_frame_api.hpp                                          //
// Description:   QCAN classes - CAN API frame                                //
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



#ifndef QCAN_FRAME_API_HPP_
#define QCAN_FRAME_API_HPP_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "qcan_data.hpp"

using namespace QCan;

//-----------------------------------------------------------------------------
/*!
** \class   QCanFrameApi
** \brief   CAN API frame
** 
** This class defines a CAN API frame.
*/
class QCanFrameApi :  private QCanData
{
public:
  
   enum ApiFunc_e {

      eAPI_FUNC_NONE   = 0,

      /*! Set bit-rate                                   */
      eAPI_FUNC_BITRATE,

      /*! Set CAN mode                                   */
      eAPI_FUNC_CAN_MODE,

      eAPI_FUNC_DRIVER_INIT,

      eAPI_FUNC_DRIVER_RELEASE,

      eAPI_FUNC_HDI,

      eAPI_FUNC_NAME,

      eAPI_FUNC_STATE

   };


   QCanFrameApi();
   
   
   ~QCanFrameApi();

   bool  bitrate(int32_t & slNomBitRateV, int32_t & slDatBitRateV);

   int32_t  bitrateData(void);

   int32_t  bitrateNominal(void);
   
   //bool  hdi(CpHdi_ts & tsHdiR);

   ApiFunc_e function(void);

   bool  name(QString & clNameR);

   CAN_Mode_e  mode(void);

   void setBitrate(int32_t slBitrateV, int32_t slBrsClockV);

   void  setDriverInit();

   void  setDriverRelease();

   void  setMode(CAN_Mode_e teModeV);

   void  setName(QString clNameV);

   //void  setHdi(CpHdi_ts * tsHdiV);
   
   bool       fromByteArray(const QByteArray & clByteArrayR);
   QByteArray toByteArray() const;
   virtual QString   toString(const bool & btShowTimeR = false);

private:
   
   
};





#endif   // QCAN_FRAME_API_HPP_ 

