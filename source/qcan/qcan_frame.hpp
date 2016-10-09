//============================================================================//
// File:          qcan_frame.hpp                                              //
// Description:   QCAN classes - CAN frame                                    //
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


#ifndef QCAN_FRAME_HPP_
#define QCAN_FRAME_HPP_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <QByteArray>
#include <QDataStream>
#include <QString>

#include "canpie_frame.hpp"


#define  QCAN_FRAME_ARRAY_SIZE      96


//-----------------------------------------------------------------------------
/*!
** \class   QCanFrame
** \brief   CAN frame
** 
*/
class QCanFrame : public CpFrame
{
public:
   
   /*!
   ** Constructs an empty classic standard CAN frame (type eTYPE_CAN_STD)
   ** with a DLC value of 0.
   */
   QCanFrame();
   

   /*!
   ** Constructs a CAN frame of type \c ubTypeR with an identifier value
   ** of \c ulIdentifierR and a DLC value of \c ubDlcR.
   */
   QCanFrame(const Type_e & ubTypeR, const uint32_t & ulIdentifierR = 0, 
             const uint8_t & ubDlcR = 0);
   
   virtual ~QCanFrame();
   
   
   QByteArray  data(void) const;
   uint8_t     data(const uint8_t & ubPosR) const;

   
   bool        fromByteArray(const QByteArray & clByteArrayR);

   void        setData(const QByteArray &clDataR);
   void        setData(const uint8_t & ubPosR, const uint8_t & ubValueR);
   
   virtual QByteArray toByteArray() const;
   
   virtual QString    toString(const bool & btShowTimeR = false);
   
             
   friend QDataStream & operator<< (QDataStream & clStreamR, 
                                    const QCanFrame & clCanFrameR);
   
   friend QDataStream & operator>> (QDataStream & clStreamR, 
                                    QCanFrame & clCanFrameR);
   
private:
   
   
};





#endif   // QCAN_FRAME_HPP_ 

