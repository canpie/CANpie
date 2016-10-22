//============================================================================//
// File:          canpie_timestamp.cpp                                        //
// Description:   CANpie classes - timestamp                                  //
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

#include "canpie_timestamp.hpp"




//----------------------------------------------------------------------------//
// CpTimeStamp()                                                              //
// timestamp constructor                                                      //
//----------------------------------------------------------------------------//
CpTimeStamp::CpTimeStamp()
{
   ulSecondsP     = 0;
   ulNanoSecondsP = 0;
}


//----------------------------------------------------------------------------//
// clear()                                                                    //
// clear timestamp value                                                      //
//----------------------------------------------------------------------------//
void CpTimeStamp::clear(void)
{
   ulSecondsP     = 0;
   ulNanoSecondsP = 0;
}


//----------------------------------------------------------------------------//
// fromMicroSeconds()                                                         //
// convert micro-seconds value to timestamp value                             //
//----------------------------------------------------------------------------//
void CpTimeStamp::fromMicroSeconds(uint64_t uqMicroSecondsV)
{

}

//----------------------------------------------------------------------------//
// fromMilliSeconds()                                                         //
// convert milli-seconds value to timestamp value                             //
//----------------------------------------------------------------------------//
void CpTimeStamp::fromMilliSeconds(uint64_t uqMilliSecondsV)
{

}

//----------------------------------------------------------------------------//
// setNanoSeconds()                                                           //
// set nano-seconds value                                                     //
//----------------------------------------------------------------------------//
void CpTimeStamp::setNanoSeconds(uint32_t ulNanoSecondsV)
{
   if(ulNanoSecondsV < 1000000000UL)
   {
      ulNanoSecondsP = ulNanoSecondsV;
   }
   else
   {
      ulNanoSecondsP = 0;
   }
}


//----------------------------------------------------------------------------//
// operator ==                                                                //
// compare two timestamp values                                               //
//----------------------------------------------------------------------------//
bool CpTimeStamp::operator==(const CpTimeStamp & clTimestampR)
{
   bool btResultT = false;

   if(this->nanoSeconds() == clTimestampR.nanoSeconds())
   {
      if(this->seconds() == clTimestampR.seconds())
      {
         btResultT = true;
      }
   }

   return(btResultT);
}


//----------------------------------------------------------------------------//
// operator ==                                                                //
// compare two timestamp values                                               //
//----------------------------------------------------------------------------//
bool CpTimeStamp::operator!=(const CpTimeStamp & clTimestampR)
{
   return(!(*this == clTimestampR));
}


//----------------------------------------------------------------------------//
// operator <                                                                 //
// compare two timestamp values                                               //
//----------------------------------------------------------------------------//
bool CpTimeStamp::operator<(const CpTimeStamp & clTimestampR)
{
   bool btResultT = false;


   return(btResultT);

}


//----------------------------------------------------------------------------//
// operator <                                                                 //
// compare two timestamp values                                               //
//----------------------------------------------------------------------------//
bool CpTimeStamp::operator>(const CpTimeStamp & clTimestampR)
{
   bool btResultT = false;


   return(btResultT);

}


//----------------------------------------------------------------------------//
// operator +                                                                 //
// add two timestamp values                                                   //
//----------------------------------------------------------------------------//
CpTimeStamp CpTimeStamp::operator+(const CpTimeStamp & clTimestampR)
{
   CpTimeStamp clResultT;
   uint32_t    ulNanoSecsT = 0;
   uint32_t    ulSecondsT = 0;

   //----------------------------------------------------------------
   // Add nano-second values, this may lead to an overflow, i.e.
   // more than 999.999.999 nano-seconds
   //
   ulNanoSecsT = this->nanoSeconds() + clTimestampR.nanoSeconds();
   if(ulNanoSecsT > 999999999)
   {
      ulSecondsT = 1;
      clResultT.setNanoSeconds(ulNanoSecsT - 1000000000UL);
   }

   //----------------------------------------------------------------
   // calculate seconds value
   //
   ulSecondsT = ulSecondsT + this->seconds();
   ulSecondsT = ulSecondsT + clTimestampR.seconds();

   return(clResultT);

}
