//============================================================================//
// File:          qcan_frame_error.cpp                                        //
// Description:   QCan classes - CAN error frame                              //
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



/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <QDebug>
#include "qcan_frame_error.hpp"

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanFrameError()                                                           //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanFrameError::QCanFrameError() : QCanData(eTYPE_ERROR)
{
   setErrorState(eCAN_STATE_BUS_ACTIVE);
   setErrorCounterReceive(0);
   setErrorCounterTransmit(0);
   setErrorType(eERROR_TYPE_NONE);
}


//----------------------------------------------------------------------------//
// QCanFrame()                                                                //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanFrameError::~QCanFrameError()
{
   
}



//----------------------------------------------------------------------------//
// errorCounterReceive()                                                      //
// use data byte 2 for storage                                                //
//----------------------------------------------------------------------------//
uint8_t QCanFrameError::errorCounterReceive(void) const
{
   return (aubByteP[2]);
}


//----------------------------------------------------------------------------//
// errorCounterTransmit()                                                     //
// use data byte 3 for storage                                                //
//----------------------------------------------------------------------------//
uint8_t QCanFrameError::errorCounterTransmit(void) const
{
   return (aubByteP[3]);
}


//----------------------------------------------------------------------------//
// errorState()                                                               //
// use data byte 0 for storage                                                //
//----------------------------------------------------------------------------//
CAN_State_e QCanFrameError::errorState(void) const
{
   return ((CAN_State_e) aubByteP[0]);
}


//----------------------------------------------------------------------------//
// errorType()                                                                //
// use data byte 1 for storage                                                //
//----------------------------------------------------------------------------//
QCanFrameError::ErrorType_e QCanFrameError::errorType(void) const
{
   return ((QCanFrameError::ErrorType_e) aubByteP[1]);
}


//----------------------------------------------------------------------------//
// setErrorCounterReceive()                                                   //
// use data byte 2 for storage                                                //
//----------------------------------------------------------------------------//
void QCanFrameError::setErrorCounterReceive(const uint8_t & ubErrorCntR)
{
   aubByteP[2] = ubErrorCntR;
}


//----------------------------------------------------------------------------//
// setErrorCounterTransmit()                                                  //
// use data byte 3 for storage                                                //
//----------------------------------------------------------------------------//
void QCanFrameError::setErrorCounterTransmit(const uint8_t & ubErrorCntR)
{
   aubByteP[3] = ubErrorCntR;
}


//----------------------------------------------------------------------------//
// setErrorStat()                                                             //
// use data byte 0 for storage                                                //
//----------------------------------------------------------------------------//
void QCanFrameError::setErrorState(CAN_State_e ubStateV)
{
   aubByteP[0] = ubStateV;
}


//----------------------------------------------------------------------------//
// setErrorType()                                                             //
// use data byte 1 for storage                                                //
//----------------------------------------------------------------------------//
void QCanFrameError::setErrorType(ErrorType_e ubTypeV)
{
   aubByteP[1] = ubTypeV;
}




//----------------------------------------------------------------------------//
// fromByteArray()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanFrameError::fromByteArray(const QByteArray & clByteArrayR)
{
   bool  btResultT = false;
   
   //----------------------------------------------------------------
   // an API frame must have the value 0x80 in the first byte,
   // refer to QCanData class implementation
   //
   if ((clByteArrayR.at(0) & 0x80) > 0)
   {
      btResultT = (QCanData::fromByteArray(clByteArrayR)); 
   }
   
   return (btResultT);
}


//----------------------------------------------------------------------------//
// toByteArray()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
QByteArray QCanFrameError::toByteArray() const
{
   return QCanData::toByteArray();
}

//----------------------------------------------------------------------------//
// toString()                                                                 //
// print CAN frame                                                            //
//----------------------------------------------------------------------------//
QString QCanFrameError::toString(const bool & btShowTimeR)
{
   //----------------------------------------------------------------
   // setup a string object
   //
   QString clStringT;
   
   uint32_t ulSecondsT = 0;
   uint32_t ulNanoSecT = 0;


   if(btShowTimeR == true)
   {
      ulSecondsT = this->timeStamp().seconds();
      ulNanoSecT = this->timeStamp().nanoSeconds();
      ulNanoSecT = ulNanoSecT / 10000;
      clStringT = QString("%1.%2 ").arg(ulSecondsT, 5, 10).arg(ulNanoSecT, 5, 10, QChar('0'));
   }
   

   //----------------------------------------------------------------
   // print frame format
   //
   clStringT += "   Error frame  :  ";

   switch(this->errorState())
   {
      case eCAN_STATE_BUS_ACTIVE:
         clStringT += "Error active";
         break;

      case eCAN_STATE_BUS_WARN:
         clStringT += "Warning level reached";
         break;

      case eCAN_STATE_BUS_PASSIVE:
         clStringT += "Error passive";
         break;

      case eCAN_STATE_BUS_OFF:
         clStringT += "Bus off";
         break;

      default:

         break;
   }
   
   clStringT += QString(" (Rx=%1, Tx=%2) ").arg(errorCounterReceive()).arg(errorCounterTransmit());

   switch(this->errorType())
   {
      case eERROR_TYPE_NONE:

         break;

      case eERROR_TYPE_BIT0:
      case eERROR_TYPE_BIT1:
         clStringT += ", bit error";
         break;

      case eERROR_TYPE_STUFF:
         clStringT += ", stuff error";
         break;

      case eERROR_TYPE_FORM:
         clStringT += ", form error";
         break;

      case eERROR_TYPE_CRC:
         clStringT += ", CRC error";
         break;

      case eERROR_TYPE_ACK:
         clStringT += ", acknowledgement error";
         break;

   }
   return(clStringT);
}
      
