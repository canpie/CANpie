//============================================================================//
// File:          canpie_timestamp.hpp                                        //
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


#ifndef CANPIE_TIMESTAMP_HPP_
#define CANPIE_TIMESTAMP_HPP_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <stdint.h>




//-----------------------------------------------------------------------------
/*!
** \class   CpTimeStamp
** \brief   CAN frame time-stamp
**
** The timestamp ..
** 
*/
class CpTimeStamp
{
public:
   


   CpTimeStamp();
   
   void  clear(void);

   void  fromMicroSeconds(uint64_t uqMicroSecondsV);

   void  fromMilliSeconds(uint64_t uqMicroSecondsV);

   inline uint32_t nanoSeconds(void) const   { return(ulNanoSecondsP);  };
   
   inline uint32_t seconds(void) const       { return(ulSecondsP);      };

   void  setNanoSeconds(uint32_t ulNanoSecondsV);
   
   inline void setSeconds(const uint32_t ulSecondsV)  { ulSecondsP = ulSecondsV; };

   bool operator==(const CpTimeStamp & clTimestampR);

   bool operator!=(const CpTimeStamp & clTimestampR);

   bool operator<(const CpTimeStamp & clTimestampR);

   bool operator>(const CpTimeStamp & clTimestampR);

   CpTimeStamp operator+(const CpTimeStamp & clTimestampR);

private:

   uint32_t ulSecondsP;

   uint32_t ulNanoSecondsP;
   
};





#endif   // CANPIE_TIMESTAMP_HPP_ 

