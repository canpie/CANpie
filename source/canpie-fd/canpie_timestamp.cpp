//============================================================================//
// File:          canpie_timestamp.cpp                                        //
// Description:   CANpie classes - time-stamp                                 //
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
#include "../canpie-fd/canpie_timestamp.hpp"




//----------------------------------------------------------------------------//
// CpTimeStamp()                                                              //
// time-stamp constructor                                                     //
//----------------------------------------------------------------------------//
CpTimeStamp::CpTimeStamp(uint32_t ulSecondsV, uint32_t ulNanoSecondsV)
{
  this->setSeconds(ulSecondsV);
  this->setNanoSeconds(ulNanoSecondsV);
}


//----------------------------------------------------------------------------//
// clear()                                                                    //
// clear time-stamp value                                                     //
//----------------------------------------------------------------------------//
void CpTimeStamp::clear(void)
{
  ulSecondsP     = 0;
  ulNanoSecondsP = 0;
}


//----------------------------------------------------------------------------//
// fromMicroSeconds()                                                         //
// convert micro-seconds value to time-stamp value                            //
//----------------------------------------------------------------------------//
void CpTimeStamp::fromMicroSeconds(uint32_t ulMicroSecondsV)
{
  //----------------------------------------------------------------
  // calculate the seconds
  //
  ulSecondsP     = ulMicroSecondsV / 1000000UL;

  //----------------------------------------------------------------
  // substract the seconds from parameter
  //
  ulMicroSecondsV = ulMicroSecondsV - (ulSecondsP * 1000000UL);

  //----------------------------------------------------------------
  // convert the remaining part to nano-seconds
  //
  ulNanoSecondsP = ulMicroSecondsV * 1000UL;
}


//----------------------------------------------------------------------------//
// fromMilliSeconds()                                                         //
// convert milli-seconds value to time-stamp value                            //
//----------------------------------------------------------------------------//
void CpTimeStamp::fromMilliSeconds(uint32_t ulMilliSecondsV)
{
  //----------------------------------------------------------------
  // calculate the seconds
  //
  ulSecondsP     = ulMilliSecondsV / 1000UL;

  //----------------------------------------------------------------
  // substract the seconds from parameter
  //
  ulMilliSecondsV = ulMilliSecondsV - (ulSecondsP * 1000UL);

  //----------------------------------------------------------------
  // convert the remaining part to nano-seconds
  //
  ulNanoSecondsP = ulMilliSecondsV * 1000000UL;

}


//----------------------------------------------------------------------------//
// isValid()                                                                  //
// test valid value range for data fields                                     //
//----------------------------------------------------------------------------//
bool CpTimeStamp::isValid(void)
{
  bool  btResultT = false;

  if ((ulSecondsP <= TIME_STAMP_SECS_LIMIT) &&
      (ulNanoSecondsP <= TIME_STAMP_NSEC_LIMIT))
  {
    btResultT = true;
  }

  return (btResultT);
}


//----------------------------------------------------------------------------//
// operator -                                                                 //
// substract two time-stamp values                                            //
//----------------------------------------------------------------------------//
CpTimeStamp CpTimeStamp::operator-(const CpTimeStamp &clTimestampR) const
{
  CpTimeStamp clTempT(*this);

  clTempT -= clTimestampR;   // use operator -=
  return (clTempT);
}


//----------------------------------------------------------------------------//
// operator !=                                                                //
// compare two time-stamp values                                              //
//----------------------------------------------------------------------------//
bool CpTimeStamp::operator!=(const CpTimeStamp &clTimestampR) const
{
  return (!(*this == clTimestampR));
}


//----------------------------------------------------------------------------//
// operator +                                                                 //
// add two time-stamp values                                                  //
//----------------------------------------------------------------------------//
CpTimeStamp  CpTimeStamp::operator+(const CpTimeStamp &clTimestampR) const
{
  CpTimeStamp clTempT(*this);

  clTempT += clTimestampR;   // use operator +=
  return (clTempT);
}


//----------------------------------------------------------------------------//
// operator +=                                                                //
// add two time-stamp values                                                  //
//----------------------------------------------------------------------------//
CpTimeStamp &CpTimeStamp::operator+=(const CpTimeStamp &clTimestampR)
{
  uint32_t    ulNanoSecsT = 0;
  uint64_t    uqSecondsT = 0;

  //----------------------------------------------------------------
  // Add nano-second values, this may lead to an overflow, i.e.
  // more than 999.999.999 nano-seconds
  //
  ulNanoSecsT = this->nanoSeconds() + clTimestampR.nanoSeconds();
  if (ulNanoSecsT > TIME_STAMP_NSEC_LIMIT)
  {
    uqSecondsT  = 1;
    ulNanoSecsT = ulNanoSecsT - 1000000000UL;
  }

  //----------------------------------------------------------------
  // calculate seconds value
  //
  uqSecondsT = uqSecondsT + this->seconds();
  uqSecondsT = uqSecondsT + clTimestampR.seconds();

  //----------------------------------------------------------------
  // check for overflow
  //
  if (uqSecondsT > TIME_STAMP_SECS_LIMIT)
  {
    //---------------------------------------------------
    // set invalid value
    //
    this->ulSecondsP     = TIME_STAMP_INVALID_VALUE;
    this->ulNanoSecondsP = TIME_STAMP_INVALID_VALUE;
  }
  else
  {
    this->setSeconds((uint32_t) uqSecondsT);
    this->setNanoSeconds(ulNanoSecsT);
  }

  return (*this);
}


//----------------------------------------------------------------------------//
// operator <                                                                 //
// compare two time-stamp values                                              //
//----------------------------------------------------------------------------//
bool CpTimeStamp::operator<(const CpTimeStamp &clTimestampR) const
{
  bool btResultT = false;

  //----------------------------------------------------------------
  // when seconds part is smaller, then result is true
  //
  if (this->seconds() < clTimestampR.seconds())
  {
    btResultT = true;
  }

  //----------------------------------------------------------------
  // when seconds part is equal, check the nano-seconds
  //
  if (this->seconds() == clTimestampR.seconds())
  {
    if (this->nanoSeconds() < clTimestampR.nanoSeconds())
    {
      btResultT = true;
    }
  }

  return (btResultT);
}


//----------------------------------------------------------------------------//
// operator <=                                                                //
// compare two time-stamp values                                              //
//----------------------------------------------------------------------------//
bool CpTimeStamp::operator<=(const CpTimeStamp &clTimestampR) const
{
  return ((*this == clTimestampR) || (*this < clTimestampR));
}


//----------------------------------------------------------------------------//
// operator -=                                                                //
// substract two time-stamp values                                            //
//----------------------------------------------------------------------------//
CpTimeStamp &CpTimeStamp::operator-=(const CpTimeStamp &clTimeStampR)
{
  uint32_t    ulNanoSecsT = 0;
  uint32_t    ulSecondsT  = 0;

  //----------------------------------------------------------------
  // test is substraction may cause an underflow
  //
  if (*this < clTimeStampR)
  {
    //---------------------------------------------------
    // set invalid value
    //
    this->ulSecondsP     = TIME_STAMP_INVALID_VALUE;
    this->ulNanoSecondsP = TIME_STAMP_INVALID_VALUE;
  }
  else
  {
    if (this->ulNanoSecondsP < clTimeStampR.ulNanoSecondsP)
    {
      ulNanoSecsT = this->ulNanoSecondsP   + 1000000000UL;
      ulNanoSecsT = ulNanoSecsT - clTimeStampR.ulNanoSecondsP;
      ulSecondsT  = (this->ulSecondsP)     - (clTimeStampR.ulSecondsP);
      ulSecondsT  = ulSecondsT - 1;
    }
    else
    {
      ulNanoSecsT = (this->ulNanoSecondsP) - (clTimeStampR.ulNanoSecondsP);
      ulSecondsT  = (this->ulSecondsP)     - (clTimeStampR.ulSecondsP);
    }

    //---------------------------------------------------
    // set value
    //
    this->ulSecondsP     = ulSecondsT;
    this->ulNanoSecondsP = ulNanoSecsT;
  }

  return (*this);
}

//----------------------------------------------------------------------------//
// operator ==                                                                //
// compare two time-stamp values                                               //
//----------------------------------------------------------------------------//
bool CpTimeStamp::operator==(const CpTimeStamp &clTimeStampR) const
{
  bool btResultT = false;
  if (this->nanoSeconds() == clTimeStampR.nanoSeconds())
  {
    if (this->seconds() == clTimeStampR.seconds())
    {
      btResultT = true;
    }
  }
  return (btResultT);
}


//----------------------------------------------------------------------------//
// operator >                                                                 //
// compare two time-stamp values                                               //
//----------------------------------------------------------------------------//
bool CpTimeStamp::operator>(const CpTimeStamp &clTimestampR) const
{
  return (!(*this < clTimestampR) && !(*this == clTimestampR));
}


//----------------------------------------------------------------------------//
// operator >=                                                                 //
// compare two time-stamp values                                               //
//----------------------------------------------------------------------------//
bool CpTimeStamp::operator>=(const CpTimeStamp &clTimestampR) const
{
  return ((*this == clTimestampR) || !(*this < clTimestampR));
}


//----------------------------------------------------------------------------//
// setNanoSeconds()                                                           //
// set nano-seconds value                                                     //
//----------------------------------------------------------------------------//
void CpTimeStamp::setNanoSeconds(uint32_t ulNanoSecondsV)
{
  if (ulNanoSecondsV <= TIME_STAMP_NSEC_LIMIT)
  {
    ulNanoSecondsP = ulNanoSecondsV;
  }
  else
  {
    ulNanoSecondsP = TIME_STAMP_NSEC_LIMIT;
  }
}


//----------------------------------------------------------------------------//
// setSeconds()                                                               //
// set seconds value                                                          //
//----------------------------------------------------------------------------//
void CpTimeStamp::setSeconds(const uint32_t ulSecondsV)
{
  if (ulSecondsV <= TIME_STAMP_SECS_LIMIT)
  {
    ulSecondsP = ulSecondsV;
  }
  else
  {
    ulSecondsP = TIME_STAMP_SECS_LIMIT;
  }
}
