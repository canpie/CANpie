//============================================================================//
// File:          qcan_timestamp.hpp                                          //
// Description:   QCan classes - time-stamp                                   //
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


#ifndef QCAN_TIMESTAMP_HPP_
#define QCAN_TIMESTAMP_HPP_



/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <stdint.h>

//-------------------------------------------------------------------
/*!
** \file qcan_timestamp.hpp
**
*/

//-------------------------------------------------------------------
/*!
** \def  TIME_STAMP_SECS_LIMIT
** 
** The symbol TIME_STAMP_SECS_LIMIT defines the maximum value for 
** the seconds field of the time-stamp.
*/
#define  TIME_STAMP_SECS_LIMIT      ((uint32_t) 4290000000)


//-------------------------------------------------------------------
/*!
** \def  TIME_STAMP_NSEC_LIMIT
** 
** The symbol TIME_STAMP_NSEC_LIMIT defines the maximum value for 
** the nanoseconds field of the time-stamp.
*/
#define  TIME_STAMP_NSEC_LIMIT      ((uint32_t)  999999999)


//-------------------------------------------------------------------
/*!
** \def  TIME_STAMP_INVALID_VALUE
** 
** The symbol TIME_STAMP_INVALID_VALUE defines the invalid value for 
** both data fields. the nanoseconds field of the time-stamp.
*/
#define  TIME_STAMP_INVALID_VALUE   ((uint32_t) 0xFFFFFFEE)

//-----------------------------------------------------------------------------
/*!
** \class   QCanTimeStamp
** \brief   CAN frame time-stamp
**
** The QCanTimeStamp class provides the functionality for setting a time-stamp
** with a 1 ns (nano-seconds) resolution to a CAN frame 
** (refer to #QCanFrame::clMsgTimeP). The time-stamp has two data fields:
** \li A nanoseconds field with a valid value range from 0 to 
**     #TIME_STAMP_NSEC_LIMIT
** \li A seconds field with a valid value range from 0 to 
**     #TIME_STAMP_SECS_LIMIT
** 
** The time-stamp covers a time-period of more than 49652 days (136 years).
** The value of a time-stamp can be set from a counter value by means of
** the functions fromMicroSeconds() or fromMilliSeconds(). 
** 
*/
class QCanTimeStamp
{
public:
   
   /*!
   **
   ** @param ulSecondsV - Set the seconds field
   ** @param ulNanoSecondsV - Set the nano-seconds field
   ** 
   ** Construct a time-stamp with \a ulSecondsV and \a ulNanoSecondsV. If the 
   ** valid value range for the parameters is violated the object will be 
   ** constructed using maximum allowed values. 
   */
   QCanTimeStamp(uint32_t ulSecondsV=0, uint32_t ulNanoSecondsV=0);

   
   /*!
   ** Clear time-stamp by setting both data fields to 0. 
   */
   void  clear(void);

   /*!
   ** \param[in] ulMicroSecondsV - time-value in microseconds [&micro;s]
   ** 
   ** Set the time-stamp value according to the parameter \a ulMicroSecondsV.
   */
   void  fromMicroSeconds(uint32_t ulMicroSecondsV);

   /*!
   ** \param[in] ulMilliSecondsV - time-value in milliseconds [msec]
   ** 
   ** Set the time-stamp value according to the parameter \a ulMilliSecondsV.
   */   
   void  fromMilliSeconds(uint32_t ulMilliSecondsV);

   /*!
   ** \return  \c true if time-stamp value is valid
   ** 
   ** Returns \c true if this time-stamp is within the valid value range.
   ** The limit for the data fields are defined by #TIME_STAMP_SECS_LIMIT
   ** and #TIME_STAMP_NSEC_LIMIT.
   */
   bool  isValid(void);
   
   
   /*!
   ** \return  Nanoseconds part of time-stamp value
   ** \sa      setNanoSeconds()
   ** 
   ** Returns the nanoseconds part of this time-stamp. A return value of 
   ** #TIME_STAMP_INVALID_VALUE marks the time-stamp value as invalid.
   ** The validity of a time-stamp can be tested with isValid().
   */
   inline uint32_t nanoSeconds(void) const   { return(ulNanoSecondsP);  };
   
   
   /*!
   ** \return  Seconds part of time-stamp value
   ** \sa      setSeconds()
   ** 
   ** Returns the seconds part of this time-stamp. A return value of 
   ** #TIME_STAMP_INVALID_VALUE marks the time-stamp value as invalid.
   ** The validity of a time-stamp can be tested with isValid().
   */
   inline uint32_t seconds(void) const       { return(ulSecondsP);      };

   
   /*!
   ** \param[in]  ulNanoSecondsV - Nanosecond value
   ** \sa         nanoSeconds()
   ** 
   ** Set the nanoseconds part of this time-stamp. The value of 
   ** \a ulNanoSecondsV must be in the range from 0 to #TIME_STAMP_NSEC_LIMIT.
   ** If passing a value greater than #TIME_STAMP_NSEC_LIMIT the value is
   ** limited to #TIME_STAMP_NSEC_LIMIT.
   */
   void setNanoSeconds(uint32_t ulNanoSecondsV);
   
   /*!
   ** \param[in]  ulSecondsV - Second value
   ** \sa         seconds()
   ** 
   ** Set the seconds part of this time-stamp. The value of 
   ** \a ulSecondsV must be in the range from 0 to #TIME_STAMP_SECS_LIMIT.
   ** If passing a value greater than #TIME_STAMP_SECS_LIMIT the value is
   ** limited to #TIME_STAMP_SECS_LIMIT.
   */
   void setSeconds(const uint32_t ulSecondsV);

   /*!
   ** \param   clTimeStampR - Refence to other time-stamp
   ** \return  \c true on equal time-stamps
   ** 
   ** Returns \c true if this time-stamp is equal to time-stamp \a clTimeStampR,
   ** otherwise returns \c false.
   */
   bool operator==( const QCanTimeStamp & clTimeStampR) const;

   /*!
   ** \param   clTimeStampR - Refence to other time-stamp
   ** \return  \c true on not equal time-stamps
   ** 
   ** Returns \c true if this time-stamp is not equal to time-stamp 
   ** \a clTimeStampR, otherwise returns \c false.
   */
   bool operator!=( const QCanTimeStamp & clTimeStampR) const;
   
   /*!
   ** \param   clTimeStampR - Refence to other time-stamp
   ** \return  \c true on smaller time-stamp
   **
   ** Returns true if this time-stamp is less than time-stamp \a clTimeStampR,
   ** otherwise returns false.
   */
   bool operator<(const QCanTimeStamp & clTimeStampR) const;

   /*!
   ** \param   clTimeStampR - Refence to other time-stamp
   ** \return  \c true on smaller or equal time-stamp
   **
   ** Returns true if this time-stamp is less than or equal to time-stamp 
   ** \a clTimeStampR, otherwise returns false.
   */   
   bool operator<=(const QCanTimeStamp & clTimeStampR) const;
   
   /*!
   ** \param   clTimeStampR - Refence to other time-stamp
   ** \return  \c true on greater time-stamp
   ** 
   ** Returns true if this time-stamp is greater than time-stamp 
   ** \a clTimeStampR, otherwise returns false.
   */
   bool operator>(const QCanTimeStamp & clTimeStampR) const;

   /*!
   ** \param   clTimeStampR - Refence to other time-stamp
   ** \return  \c true on greater or equal time-stamp
   ** 
   ** Returns true if this time-stamp is greater or equal than time-stamp 
   ** \a clTimeStampR, otherwise returns false.
   */
   bool operator>=(const QCanTimeStamp & clTimeStampR) const;
   
   /*!
   ** \param   clTimeStampR - Refence to other time-stamp
   ** \return  Sum of two time-stamp values
   **  
   ** Add the time-stamp \a clTimeStampR to this time-stamp value 
   ** and returns a reference to this time-stamp.
   */
   QCanTimeStamp   operator+(const QCanTimeStamp & clTimeStampR) const;
   
   /*!
   ** \param   clTimeStampR - Refence to other time-stamp
   ** \return  Sum of two time-stamp values
   ** 
   ** Add the time-stamp \a clTimeStampR to this time-stamp value 
   ** and returns a reference to this time-stamp.
   */
   QCanTimeStamp & operator+=(const QCanTimeStamp & clTimeStampR);
   
   /*!
   ** \param   clTimeStampR - Refence to other time-stamp
   ** \return  Difference between two time-stamp values
   **  
   ** Substract the time-stamp \a clTimeStampR from this time-stamp value 
   ** and returns a reference to this time-stamp.
   */
   QCanTimeStamp operator-(const QCanTimeStamp & clTimeStampR) const;

   /*!
   ** \param   clTimeStampR - Refence to other time-stamp
   ** \return  Difference between two time-stamp values
   **  
   ** Substract the time-stamp \a clTimeStampR from this time-stamp value 
   ** and returns a reference to this time-stamp.
   */
   QCanTimeStamp & operator-=(const QCanTimeStamp & clTimeStampR);
   
private:

   /*!
   ** Seconds, valid value range is 0 to #TIME_STAMP_SECS_LIMIT  
   */
   uint32_t ulSecondsP;

   /*!
   ** Nanoseconds, valid value range is 0 to TIME_STAMP_NSEC_LIMIT 
   */
   uint32_t ulNanoSecondsP;
   
};





#endif   // QCAN_TIMESTAMP_HPP_ 

