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
#include "canpie_frame_api.hpp"
#include "qcan_defs.hpp"
#include "qcan_frame.hpp"

using namespace CANpie;



//-----------------------------------------------------------------------------
/*!
** \class   QCanFrameApi
** \brief   CAN API frame
** 
** This class defines a CAN API frame.
*/
class QCanFrameApi : public CpFrameApi, private QCanFrame
{
public:
  
   QCanFrameApi();
   
   
   ~QCanFrameApi();

   QCanFrame& base(void)   { return static_cast<QCanFrame&>(*this); };

   QByteArray toByteArray() const;
   QString    toString(const bool & btShowTimeR = false);
   
private:
   
   
};





#endif   // QCAN_FRAME_HPP_ 

