//============================================================================//
// File:          canpie_message.hpp                                          //
// Description:   CANpie classes - CANpie message structure                   //
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


#ifndef CANPIE_DATA_HPP_
#define CANPIE_DATA_HPP_


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

#ifndef  CANPIE_QT_SUPPORT
#define  CANPIE_QT_SUPPORT          1
#endif

#define  QCAN_FRAME_ARRAY_SIZE      96


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "canpie_namespace.hpp"
#include "canpie_timestamp.hpp"

#if CANPIE_QT_SUPPORT > 0
#include <QByteArray>
#endif


//-------------------------------------------------------------------
/*!
** \file canpie_data.hpp
**
*/


//-------------------------------------------------------------------
/*!
** \def  CAN_MSG_DATA_MAX
**
** The symbol CAN_MSG_DATA_MAX defines the maximum number of
** bytes in a CAN frame (payload).
*/
#define  CAN_MSG_DATA_MAX        64


//-----------------------------------------------------------------------------
/*!
** \class   CpData
** \brief   CAN frame
** 
** This class defines a <i>CAN frame</i>. It is the base class for
** CpFrameApi and CpFrameError. A CAN frame can have four different
** formats: classic CAN frame format (using standard or extended
** identifier) and ISO CAN FD format (also using standard or extended
** identifier). The possible frame formats are defined by the Type_e
** enumeration. A classic CAN frame can have a maximum payload of 8 byte,
** whereas an ISO CAN FD frame can have a maximum payload of 64 byte.
**
**
*/
class CpData
{
public:
   
   enum Type_e {

      eTYPE_API = 0,
      
      eTYPE_CAN,

      eTYPE_ERROR

   };

   /*!
   ** Constructs an empty classic standard CAN frame (type eTYPE_CAN)
   ** with a DLC value of 0.
   */
   CpData();
   
   CpData(const Type_e & ubTypeR);
   
   virtual ~CpData();
   
   /*!
   ** \return     Unsigned 16-bit data
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  btMsbFirstR    Byte order
   ** \see        setDataUInt16()
   **
   ** The function returns an unsigned 16-bit value (uint16_t) starting at
   ** position \a ubPosR of the CAN payload.
   */
   virtual uint16_t dataUInt16(const uint8_t & ubPosR,
                               const bool & btMsbFirstR = 0) const;


   /*!
   ** \return     Unsigned 32-bit data
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  btMsbFirstR    Byte order
   ** \see        setDataUInt32()
   **
   ** The function returns an unsigned 32-bit value (uint32_t) starting at
   ** position \a ubPosR of the CAN payload.
   */
   virtual uint32_t dataUInt32(const uint8_t & ubPosR,
                               const bool & btMsbFirstR = 0) const;

   
   inline uint32_t   identifier() const   { return (ulIdentifierP);  };
   inline uint8_t    dlc() const          { return (ubMsgDlcP);      };
   inline uint8_t    control() const      { return (ubMsgCtrlP);     };
   inline uint32_t   marker() const       { return (ulMsgMarkerP);   };
   inline uint32_t   user() const         { return (ulMsgUserP);     };   
   inline uint8_t    data(uint8_t ubPosV) const { return (aubByteP[ubPosV]); };
   
   Type_e      frameType(void) const;
   
   bool        isFrameApi() const;

   bool        isFrameCan() const;
   bool        isFrameError() const;
   
   virtual void setData(const uint8_t & ubPosR, const uint8_t & ubValueR);
   
   /*!
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  uwValueR       16-bit value
   ** \param[in]  btMsbFirstR    Byte order
   ** \see        dataUInt16()
   **
   ** The function sets an unsigned 16-bit value (uint16_t) \a uwValueR
   ** starting at position \a ubPosR of the payload.
   */
   virtual void setDataUInt16(const uint8_t & ubPosR, const uint16_t & uwValueR,
                              const bool & btMsbFirstR = 0);


   /*!
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  ulValueR       32-bit value
   ** \param[in]  btMsbFirstR    Byte order
   ** \see        dataUInt32()
   **
   ** The function sets an unsigned 32-bit value (uint32_t) \a ulValueR
   ** starting at position \a ubPosR of the payload.
   */
   virtual void setDataUInt32(const uint8_t & ubPosR, const uint32_t & ulValueR,
                              const bool & btMsbFirstR = 0);

   inline void  setTimeStamp(const CpTimeStamp & clTimeStampR)       \
                     { clMsgTimeP = clTimeStampR; };

   inline CpTimeStamp timeStamp(void) const { return clMsgTimeP; };
   
   #if CANPIE_QT_SUPPORT > 0
   virtual QByteArray toByteArray() const;
   virtual bool       fromByteArray(const QByteArray & clByteArrayR);
   #endif

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
   ** <li>Bit 1: ISO CAN FD: value of FDF bit
   ** <li>Bit 2: Remote Frame
   ** <li>Bit 3: Overload Frame
   ** <li>Bit 4: reserved, always 0
   ** <li>Bit 5: reserved, always 0
   ** <li>Bit 6: ISO CAN FD: value of BRS bit
   ** <li>Bit 7: ISO CAN FD: value of ESI bit
   ** </ul>
   */
   uint8_t  ubMsgCtrlP;

   /*!   
   ** The data field has up to 64 bytes of message data.
   ** The number of used bytes is described via the structure
   ** member \c ubMsgDlcP.
   */
   uint8_t  aubByteP[CAN_MSG_DATA_MAX];

   /*!   
   ** The time stamp field defines the time when a CAN message
   ** was received by the CAN controller. This is an optional
   ** field (available if #CP_CAN_MSG_TIME is set to 1).
   */
   CpTimeStamp clMsgTimeP;

   
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





#endif   // CANPIE_DATA_HPP_ 

