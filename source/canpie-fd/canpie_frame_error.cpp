//============================================================================//
// File:          canpie_frame_error.cpp                                      //
// Description:   CANpie classes - CAN error frame                            //
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

#include "canpie_frame_error.hpp"


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// CpFrameError()                                                             //
// constructor                                                                //
//----------------------------------------------------------------------------//
CpFrameError::CpFrameError() : CpData(eTYPE_ERROR)
{

}


//----------------------------------------------------------------------------//
// QCanFrame()                                                                //
// destructor                                                                 //
//----------------------------------------------------------------------------//
CpFrameError::~CpFrameError()
{
   
}


//----------------------------------------------------------------------------//
// errorCounterReceive()                                                      //
// use data byte 2 for storage                                                //
//----------------------------------------------------------------------------//
uint8_t CpFrameError::errorCounterReceive(void) const
{
   return(aubByteP[2]);
}


//----------------------------------------------------------------------------//
// errorCounterTransmit()                                                     //
// use data byte 3 for storage                                                //
//----------------------------------------------------------------------------//
uint8_t CpFrameError::errorCounterTransmit(void) const
{
   return(aubByteP[3]);
}


//----------------------------------------------------------------------------//
// errorState()                                                               //
// use data byte 0 for storage                                                //
//----------------------------------------------------------------------------//
CAN_State_e CpFrameError::errorState(void) const
{
   return((CAN_State_e) aubByteP[0]);
}


//----------------------------------------------------------------------------//
// errorType()                                                                //
// use data byte 1 for storage                                                //
//----------------------------------------------------------------------------//
CpFrameError::ErrorType_e CpFrameError::errorType(void) const
{
   return((CpFrameError::ErrorType_e) aubByteP[1]);
}


//----------------------------------------------------------------------------//
// setErrorCounterReceive()                                                   //
// use data byte 2 for storage                                                //
//----------------------------------------------------------------------------//
void CpFrameError::setErrorCounterReceive(const uint8_t & ubErrorCntR)
{
   aubByteP[2] = ubErrorCntR;
}


//----------------------------------------------------------------------------//
// setErrorCounterTransmit()                                                  //
// use data byte 3 for storage                                                //
//----------------------------------------------------------------------------//
void CpFrameError::setErrorCounterTransmit(const uint8_t & ubErrorCntR)
{
   aubByteP[3] = ubErrorCntR;
}


//----------------------------------------------------------------------------//
// setErrorStat()                                                             //
// use data byte 0 for storage                                                //
//----------------------------------------------------------------------------//
void CpFrameError::setErrorState(CAN_State_e ubStateV)
{
   aubByteP[0] = ubStateV;
}


//----------------------------------------------------------------------------//
// setErrorType()                                                             //
// use data byte 1 for storage                                                //
//----------------------------------------------------------------------------//
void CpFrameError::setErrorType(ErrorType_e ubTypeV)
{
   aubByteP[1] = ubTypeV;
}

      
