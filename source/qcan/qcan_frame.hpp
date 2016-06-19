//============================================================================//
// File:          qcan_frame.hpp                                              //
// Description:   QCAN classes - CAN frame                                    //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53842 Troisdorf - Germany                                                  //
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


#ifndef QCAN_FRAME_HPP_
#define QCAN_FRAME_HPP_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <QByteArray>
#include <QDataStream>
#include <QString>



//-----------------------------------------------------------------------------
/*!
** \file    qcan_frame.hpp
** \brief   CAN frame
**
** This file ...
**
*/

typedef struct QCanTimestamp_s {
   uint32_t ulSeconds;
   uint32_t ulNanoSeconds;
} QCanTimestamp_ts;


#define  CAN_FRAME_DATA_MAX      64



//-----------------------------------------------------------------------------
/*!
** \class   QCanFrame
** \brief   CAN frame
** 
*/
class QCanFrame
{
public:
   
   enum Type_e {

      /*! Classic CAN, Standard frame format                */
      eTYPE_CAN_STD = 0,

      /*! Classic CAN, Extended frame format                */
      eTYPE_CAN_EXT,

      /*! ISO CAN FD, Standard frame format                 */
      eTYPE_FD_STD,

      /*! ISO CAN FD, Extended frame format                 */
      eTYPE_FD_EXT,

      /*! QCan error frame, \see QCanFrameError             */
      eTYPE_QCAN_ERR = 0x10,

      /*! QCan API frame, \see QCanFrameApi                 */
      eTYPE_QCAN_API = 0x20
   };

   QCanFrame();
   
   QCanFrame(const Type_e & ubTypeR, const uint32_t & ulIdentifierR = 0, 
             const uint8_t & ubDlcR = 0);
   
   virtual ~QCanFrame();
   
   uint8_t     data(const uint8_t & ubPosR) const;
   
   QByteArray  data(void) const;
   
   uint8_t     dataSize(void) const;

   uint16_t    dataUInt16(const uint8_t & ubPosR,
                          const bool & btMsbFirstR = 0) const;

   uint32_t    dataUInt32(const uint8_t & ubPosR,
                          const bool & btMsbFirstR = 0) const;
   
   uint8_t     dlc(void) const;
   
   Type_e      frameType(void) const;
   
   bool        fromByteArray(const QByteArray & clByteArrayR);

   /*!
   ** \brief   Identifier value
   ** \return  Identifier of CAN frame
   ** \see     setExtId(), setStdId()
   ** 
   */
   uint32_t    identifier(void) const;

   bool        isExtended(void) const;
   
   bool        isRemote(void) const;
   
   /*!
   ** \brief   Set Data
   ** \param   ubPosR      Zero based index of byte position
   ** \param   ubValueR    Value of data byte in CAN message
   ** \see     data()
   ** 
   ** This function sets the data in a CAN message. The parameter
   ** \c ubPosR must be within the range 0 .. 7 for classic frames and
   ** 0 .. 63 for FD frames.
   */
   void        setData(const uint8_t & ubPosR, const uint8_t & ubValueR);

   void        setData(const QByteArray &clDataR);

   void        setDataSize(uint8_t &ubSizeR);

   void        setDataUInt16(const uint8_t & ubPosR, const uint16_t & uwValueR,
                             const bool & btMsbFirstR = 0);

   void        setDataUInt32(const uint8_t & ubPosR, const uint32_t & ulValueR,
                             const bool & btMsbFirstR = 0);
   
   void        setDlc(uint8_t ubDlcV);
   
   void        setExtId(uint32_t ulIdentifierV);
   

   void        setFrameType(const Type_e &ubTypeR);
   
   void        setMarker(const uint32_t & ulMarkerValueR);

   void        setRemote(const bool & btRtrR = true);
   
   void        setStdId(uint16_t uwIdentifierV);
   
   void        setUser(const uint32_t & ulUserValueR);
   
   QByteArray  toByteArray() const;
   
   virtual QString   toString(const bool & btShowTimeR = false);
   
   bool operator==(const QCanFrame & clCanFrameR);
   
   bool operator!=(const QCanFrame & clCanFrameR);
             
   friend QDataStream & operator<< (QDataStream & clStreamR, 
                                    const QCanFrame & clCanFrameR);
   
   friend QDataStream & operator>> (QDataStream & clStreamR, 
                                    QCanFrame & clCanFrameR);
   
private:
   
   /*!   
   ** The identifier field may have 11 bits for standard frames
   ** (CAN specification 2.0A) or 29 bits for extended frames
   ** (CAN specification 2.0B). 
   */
   uint32_t  ulIdentifierP;

   /*!
   ** The data length code denotes the number of data bytes
   ** which are transmitted by a message.
   ** The possible value range for the data length code is
   ** from 0 to 15.
   */
   uint8_t  ubMsgDlcP;

   /*!   
   ** The structure member \c ubMsgCtrlP defines the
   ** different frame types (2.0A / 2.0B / Classic / FD / RTR).
   ** <ul>
   ** <li>Bit 0: Std. / Ext. Frame
   ** <li>Bit 1: Classic CAN / CAN FD
   ** <li>Bit 2: Remote Frame
   ** <li>Bit 3: Overload Frame
   ** <li>Bit 4: Internal: Error frame
   ** <li>Bit 5: Internal: Function frame
   ** <li>Bit 6: ISO CAN FD: value of EDL bit
   ** <li>Bit 7: ISO CAN FD: value of ESI bit
   ** </ul>
   */
   uint8_t  ubMsgCtrlP;

   /*!   
   ** The data field has up to 64 bytes of message data.
   ** The number of used bytes is described via the structure
   ** member \c ubMsgDlcP.
   */
   uint8_t  aubByteP[CAN_FRAME_DATA_MAX];

   /*!   
   ** The time stamp field defines the time when a CAN message
   ** was received by the CAN controller. This is an optional
   ** field (available if #CP_CAN_MSG_TIME is set to 1).
   */
   QCanTimestamp_ts tsMsgTimeP;

   
   /*!   
   ** The field user data can hold a 32 bit value, which is
   ** defined by the user.
   */
   uint32_t ulMsgUserP;

   /*!   
   ** The field user data can hold a 32 bit value, which is
   ** defined by the user.
   */
   uint32_t ulMsgMarkerP;
   
};





#endif   // QCAN_FRAME_HPP_ 

