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
//    the referenced file 'COPYING'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'COPYING' file.                   //
//                                                                            //
//============================================================================//



#ifndef QCAN_FRAME_ERROR_HPP_
#define QCAN_FRAME_ERROR_HPP_


#include "qcan_defs.hpp"
#include "qcan_frame.hpp"


//-----------------------------------------------------------------------------
/*!
** \class   QCanFrameError
** \brief   CAN error frame
** 
** This class defines a CAN error frame.
*/
class QCanFrameError : private QCanFrame
{
public:
   
   enum ErrorType_e {

      eERROR_TYPE_NONE   = 0,
      eERROR_TYPE_BIT0,
      eERROR_TYPE_BIT1,
      eERROR_TYPE_STUFF,
      eERROR_TYPE_FORM,
      eERROR_TYPE_CRC,
      eERROR_TYPE_ACK
   };

   QCanFrameError();
   
   
   ~QCanFrameError();

   QCan::CAN_State_e state(void);
   
   void setErrorState(const QCan::CAN_State_e & ubStateR);
   void setErrorType(const ErrorType_e & ubTypeR);
   void setRcvErrorCounter(const uint8_t & ubErrorCntR);
   void setTrmErrorCounter(const uint8_t & ubErrorCntR);


   virtual QString   toString(const bool & btShowTimeR = false);
   
private:
   
   
};





#endif   // QCAN_FRAME_HPP_ 

