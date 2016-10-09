//============================================================================//
// File:          qcan_frame_error.hpp                                        //
// Description:   QCAN classes - CAN error frame                              //
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



#ifndef QCAN_FRAME_ERROR_HPP_
#define QCAN_FRAME_ERROR_HPP_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include "canpie_frame_error.hpp"
#include "qcan_defs.hpp"
#include "qcan_frame.hpp"

using namespace CANpie;


//-----------------------------------------------------------------------------
/*!
** \class   QCanFrameError
** \brief   CAN error frame
** 
** This class defines a (virtual) <i>CAN error frame</i>. The class is
** used for two scenarios: first for simulation of CAN error frames within
** a CAN network (QCanNetwork) and second for retrieval of the error state
** from a physical CAN interface.
** <p>
** The QCanFrameError class can be used to inform about the actual
** <b>error state</b> (function errorState()) and the current values
** of the error counters (functions errorCounterReceive() and
** errorCounterTransmit()). In addition, it is possible to inform
** about the last error type which caused the error condition
** (function errorType()).
*/
class QCanFrameError : public CpFrameError, private QCanFrame
{
public:
   

   /*!
   ** Constructs an empty CAN error frame with a CAN state of
   ** QCan::eCAN_STATE_STOPPED and error counter values of 0.
   */
   QCanFrameError();
   
   
   ~QCanFrameError();



   virtual QString   toString(const bool & btShowTimeR = false);
   
private:
   
   
};





#endif   // QCAN_FRAME_HPP_ 

