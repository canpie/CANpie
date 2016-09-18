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


#include "qcan_defs.hpp"
#include "qcan_frame.hpp"


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
class QCanFrameError : private QCanFrame
{
public:
   
   enum ErrorType_e {

      /*! no error                                          */
      eERROR_TYPE_NONE   = 0,

      /*! Bit 0 error occurred                              */
      eERROR_TYPE_BIT0,

      /*! Bit 1 error occurred                              */
      eERROR_TYPE_BIT1,

      /*! Stuff error occurred                              */
      eERROR_TYPE_STUFF,

      /*! Form  error occurred                              */
      eERROR_TYPE_FORM,

      /*! CRC error occurred                                */
      eERROR_TYPE_CRC,

      /*! Acknowledgement error occurred                    */
      eERROR_TYPE_ACK
   };


   /*!
   ** Constructs an empty CAN error frame with a CAN state of
   ** QCan::eCAN_STATE_STOPPED and error counter values of 0.
   */
   QCanFrameError();
   
   
   ~QCanFrameError();

   /*!
   ** \see  setErrorCounterReceive()
   **
   ** This functions returns the current value of the receive error counter.
   */
   uint8_t  errorCounterReceive(void) const;


   /*!
   ** \see  setErrorCounterTransmit()
   **
   ** This functions returns the current value of the transmit error counter.
   ** A value of 255 leads to a bus-off condition (QCan::eCAN_STATE_BUS_OFF).
   */
   uint8_t  errorCounterTransmit(void) const;


   /*!
   ** \see  setErrorState()
   **
   ** This functions returns the current error state, defined by the
   ** enumeration QCan::CAN_State_e.
   */
   QCan::CAN_State_e             errorState(void) const;


   /*!
   ** \see  setErrorType()
   **
   ** This functions returns the error type which caused the last error
   ** condition.
   */
   QCanFrameError::ErrorType_e   errorType(void) const;
   

   /*!
   ** \see  errorCounterReceive()
   **
   ** This functions sets the transmit error counter. Passing a value
   ** greater than 127 will set the error state to error passive
   ** (QCan::eCAN_STATE_BUS_PASSIVE). A value of 255 will set the
   ** error state to bus-off (QCan::eCAN_STATE_BUS_OFF).
   */
   void setErrorCounterReceive(const uint8_t & ubErrorCntR);


   /*!
   ** \see  errorCounterTransmit()
   **
   ** This functions sets the transmit error counter. Passing a value
   ** greater than 127 will set the error state to error passive
   ** (QCan::eCAN_STATE_BUS_PASSIVE). A value of 255 will set the
   ** error state to bus-off (QCan::eCAN_STATE_BUS_OFF).
   */
   void setErrorCounterTransmit(const uint8_t & ubErrorCntR);

   void setErrorState(const QCan::CAN_State_e & ubStateR);

   void setErrorType(const ErrorType_e & ubTypeR);


   virtual QString   toString(const bool & btShowTimeR = false);
   
private:
   
   
};





#endif   // QCAN_FRAME_HPP_ 

