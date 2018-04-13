//============================================================================//
// File:          qcan_data.hpp                                               //
// Description:   QCan classes - CAN data structure                           //
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


#ifndef QCAN_DATA_HPP_
#define QCAN_DATA_HPP_



/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <QtCore/QByteArray>

#include "qcan_defs.hpp"
#include "qcan_namespace.hpp"
#include "qcan_timestamp.hpp"


//-------------------------------------------------------------------
/*!
** \file qcan_data.hpp
**
*/


//-------------------------------------------------------------------
/*!
** \def  QCAN_MSG_DATA_MAX
**
** The symbol CAN_MSG_DATA_MAX defines the maximum number of
** bytes in a CAN frame (payload).
*/
#define  QCAN_MSG_DATA_MAX           64


#define  QCAN_FRAME_ID_MASK_STD      ((uint32_t) 0x000007FF)

#define  QCAN_FRAME_ID_MASK_EXT      ((uint32_t) 0x1FFFFFFF)

#define  QCAN_FRAME_ARRAY_SIZE       96


//-----------------------------------------------------------------------------
/*!
** \class   QCanData
** \brief   CAN data
** 
** This class defines a <i>CAN data sructure</i>. It is the base class for
** QCanFrame, QCanFrameApi and QCanFrameError. 
**
**
*/
class QCanData
{
public:
   
   /*!
   ** \enum Type_e
   ** 
   ** The enumeration Type_e defines frame types. 
   */
   enum Type_e {

      /*! API frame, refer to QCanFrameApi               */
      eTYPE_API = 0,
      
      /*! CAN frame, refer to QCanFrame                  */
      eTYPE_CAN,

      /*! Error frame, refer to QCanFrameError           */
      eTYPE_ERROR,
      
      /*! Unknown frame type                             */
      eTYPE_UNKNOWN

   };
   
   
   /*!
   ** Constructs an empty data structure of type Type_e.
   */
   QCanData(const Type_e & ubTypeR);
   

   
   virtual ~QCanData();
   

   /*!
   ** \return     Unsigned 16-bit data
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  btMsbFirstR    Byte order
   ** \see        setDataUInt16()
   **
   ** The function returns an unsigned 16-bit value (uint16_t) starting at
   ** position \a ubPosR of the CAN payload.
   */
   virtual uint16_t    dataUInt16(const uint8_t & ubPosR,
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
   virtual uint32_t    dataUInt32(const uint8_t & ubPosR,
                          const bool & btMsbFirstR = 0) const;


   Type_e      frameType(void) const;
   
   virtual bool       fromByteArray(const QByteArray & clByteArrayR);


   /*!
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  uwValueR       16-bit value
   ** \param[in]  btMsbFirstR    Byte order
   ** \see        dataUInt16()
   **
   ** The function sets an unsigned 16-bit value (uint16_t) \a uwValueR
   ** starting at position \a ubPosR of the CAN payload.
   */
   virtual void        setDataUInt16(const uint8_t & ubPosR, const uint16_t & uwValueR,
                             const bool & btMsbFirstR = 0);


   /*!
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  ulValueR       32-bit value
   ** \param[in]  btMsbFirstR    Byte order
   ** \see        dataUInt32()
   **
   ** The function sets an unsigned 32-bit value (uint32_t) \a ulValueR
   ** starting at position \a ubPosR of the CAN payload.
   */
   virtual void        setDataUInt32(const uint8_t & ubPosR, const uint32_t & ulValueR,
                             const bool & btMsbFirstR = 0);
   
   virtual QByteArray toByteArray() const;


protected:
   
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
   uint8_t  aubByteP[QCAN_MSG_DATA_MAX];

   /*!   
   ** The time stamp field defines the time when a CAN message
   ** was received by the CAN controller. This is an optional
   ** field (available if #CP_CAN_MSG_TIME is set to 1).
   */
   QCanTimeStamp clMsgTimeP;

   
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



#endif   // QCAN_DATA_HPP_ 

