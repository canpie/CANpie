//============================================================================//
// File:          qcan_frame_error.hpp                                        //
// Description:   QCAN classes - CAN error frame                              //
//                                                                            //
// Copyright 2017 MicroControl GmbH & Co. KG                                  //
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
// Licensed under the Apache License, Version 2.0 (the "License");            //
// you may not use this file except in compliance with the License.           //
// You may obtain a copy of the License at                                    //
//                                                                            //
//    http://www.apache.org/licenses/LICENSE-2.0                              //
//                                                                            //
// Unless required by applicable law or agreed to in writing, software        //
// distributed under the License is distributed on an "AS IS" BASIS,          //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   //
// See the License for the specific language governing permissions and        //
// limitations under the License.                                             //
//============================================================================//



#ifndef QCAN_FRAME_ERROR_HPP_
#define QCAN_FRAME_ERROR_HPP_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "qcan_data.hpp"

using namespace QCan;

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
class QCanFrameError : private QCanData
{
public:
   
   /*!
   ** \enum ErrorType_e
   ** 
   ** The enumeration ErrorType_e defines CAN error types. 
   */
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
   ** \return     Value of receive error counter
   ** \see        setErrorCounterReceive()
   **
   ** This functions returns the current value of the receive error counter.
   */
   uint8_t  errorCounterReceive(void) const;


   /*!
   ** \return     Value of transmit error counter
   ** \see        setErrorCounterTransmit()
   **
   ** This functions returns the current value of the transmit error counter.
   ** A value of 255 leads to a bus-off condition (CANpie::eCAN_STATE_BUS_OFF).
   */
   uint8_t  errorCounterTransmit(void) const;


   /*!
   ** \return     Current error state
   ** \see        setErrorState()
   **
   ** This functions returns the current error state, defined by the
   ** enumeration CANpie::CAN_State_e.
   */
   CAN_State_e             errorState(void) const;


   /*!
   ** \return     Error type
   ** \see        setErrorType()
   **
   ** This functions returns the error type which caused the last error
   ** condition.
   */
   QCanFrameError::ErrorType_e   errorType(void) const;


   /*!
   ** \param[in]  ubErrorCntR    Receive error counter value
   ** \see        errorCounterReceive()
   **
   ** This functions sets the receive error counter. Passing a value
   ** greater than 127 will set the error state to error passive
   ** (CANpie::eCAN_STATE_BUS_PASSIVE).
   */
   void setErrorCounterReceive(const uint8_t & ubErrorCntR);


   /*!
   ** \param[in]  ubErrorCntR    Transmit error counter value
   ** \see        errorCounterTransmit()
   **
   ** This functions sets the transmit error counter. Passing a value
   ** greater than 127 will set the error state to error passive
   ** (CANpie::eCAN_STATE_BUS_PASSIVE). A value of 255 will set the
   ** error state to bus-off (CANpie::eCAN_STATE_BUS_OFF).
   */
   void setErrorCounterTransmit(const uint8_t & ubErrorCntR);


   /*!
   ** \param[in]  ubStateV       Error state value
   ** \see                       errorState()
   **
   ** This functions sets the current error state, defined by the
   ** enumeration CANpie::CAN_State_e.
   */
   void setErrorState(CAN_State_e ubStateV);


   /*!
   ** \param[in]  ubTypeV        Error type value
   ** \see        errorType()
   **
   ** This functions sets the error type which caused the last error
   ** condition.
   */
   void setErrorType(ErrorType_e ubTypeV);

   /*!
   ** \param[in]  clTimeStampR    Time-stamp
   ** \see        timeStamp()
   **
   ** The function sets the time-stamp of the CAN frame.
   */
   inline void  setTimeStamp(const QCanTimeStamp & clTimeStampR)       \
                     { clMsgTimeP = clTimeStampR; };

   /*!
   ** \return     Time-stamp
   ** \see        setTimeStamp()
   **
   ** The function returns the time-stamp value of the CAN frame.
   */
   inline QCanTimeStamp timeStamp(void) const { return clMsgTimeP; };

   bool       fromByteArray(const QByteArray & clByteArrayR);
   QByteArray toByteArray() const;
   virtual QString   toString(const bool & btShowTimeR = false);
   
private:
   
   
};





#endif   // QCAN_FRAME_HPP_ 

