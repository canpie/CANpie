//============================================================================//
// File:          qcan_frame.hpp                                              //
// Description:   QCAN classes - CAN frame                                    //
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


#ifndef QCAN_FRAME_HPP_
#define QCAN_FRAME_HPP_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <QtCore/QDataStream>
#include <QtCore/QString>

#include "qcan_data.hpp"

using namespace QCan;



//-----------------------------------------------------------------------------
/*!
** \class   QCanFrame
** \brief   CAN frame
** 
*/
class QCanFrame : private QCanData
{
public:
   
   /*!
   ** \enum    Format_e
   **
   ** This enumeration defines the CAN frame formats for classical CAN
   ** and CAN FD.
   */
   enum Format_e {

      /*! Classic CAN, Standard frame format                */
      eFORMAT_CAN_STD = 0,

      /*! Classic CAN, Extended frame format                */
      eFORMAT_CAN_EXT,

      /*! ISO CAN FD, Standard frame format                 */
      eFORMAT_FD_STD,

      /*! ISO CAN FD, Extended frame format                 */
      eFORMAT_FD_EXT,

   };

   /*!
   ** Constructs an empty classic standard CAN frame (type eTYPE_CAN_STD)
   ** with a DLC value of 0.
   */
   QCanFrame();
   

   /*!
   ** \param[in]  ubFormatR      CAN frame format
   ** \param[in]  ulIdentifierR  CAN frame identifier
   ** \param[in]  ubDlcR         CAN frame data length code
    **  
   ** Constructs a CAN frame of type \c ubTypeR with an identifier value
   ** of \c ulIdentifierR and a DLC value of \c ubDlcR.
   */
   QCanFrame(const Format_e & ubFormatR, const uint32_t & ulIdentifierR = 0, 
             const uint8_t & ubDlcR = 0);
   
 
   virtual ~QCanFrame();
   
   /*!
   ** \return  \c true if bit-rate switch is set
   ** \see     setBitrateSwitch()
   **
   ** The function returns \c true if the bit-rate switch bit is set
   ** for the CAN frame. For classic CAN frames the function always
   ** returns \c false.
   */
   bool        bitrateSwitch(void) const;


   /*!
   ** \return     Data at payload position \a ubPosR
   ** \param[in]  ubPosR   Index of payload
   **
   ** The function returns the data stored at payload position \a ubPosR.
   ** The first position starts at index 0. If the index position is out
   ** of range, the function will return 0. The maximum index position
   ** for a CAN frame is dataSize() - 1, as depicted in the following code
   ** example.
   ** \code
   ** CpFrame  clCanFrameT;
   ** ..
   ** for(uint8_t ubPosT = 0; ubPosT < clCanFrameT.dataSize(); ubPosT++)
   ** {
   **    //------------------------------------------------
   **    // print payload of CAN frame
   **    //
   **    cout << hex << clCanFrameT.data(ubPosT);
   ** }
   ** \endcode
   */
   uint8_t     data(const uint8_t & ubPosR) const;
   

   /*!
   ** \return     Data of CAN frame
   **
   ** The function returns the data stored in the CAN frame as QByteArray.
   */
   QByteArray  data(void) const;


   /*!
   ** \return     Size of payload in bytes
   ** \see        dlc()
   **
   ** The function returns the number of bytes that are valid for the CAN
   ** frame. The possible value range is 0 to 8 for classic CAN frames and
   ** 0 to 64 for CAN FD frames. In order to obtain the data length code (DLC)
   ** use the dlc() function.
   */
   uint8_t     dataSize(void) const;


   /*!
   ** \return     Unsigned 16-bit data
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  btMsbFirstR    Byte order
   ** \see        setDataUInt16()
   **
   ** The function returns an unsigned 16-bit value (uint16_t) starting at
   ** position \a ubPosR of the CAN payload.
   */
   uint16_t    dataUInt16(const uint8_t & ubPosR,
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
   uint32_t    dataUInt32(const uint8_t & ubPosR,
                          const bool & btMsbFirstR = 0) const;


   /*!
   ** \return     Data length code (DLC)
   ** \see        dataSize()
   **
   ** The function returns the DLC value (data length code) for the CAN
   ** frame. The possible value range is 0 to 8 for classic CAN frames
   ** and 0 to 15 for CAN FD frames. In order to obtain the number of
   ** bytes inside the payload use dataSize().
   */
   uint8_t     dlc(void) const;


   /*!
   ** \return  Frame type
   ** \see     setFrameType()
   **
   ** The function returns the CAN frame type, defined by the #Type_e
   ** enumeration.
   */
   Format_e    frameFormat(void) const;

   bool        fromByteArray(const QByteArray & clByteArrayR);


   /*!
   ** \return  \c true if error state indicator is set
   ** \see     setErrorStateIndicator()
   **
   ** The function returns \c true if the error state indicator bit is set
   ** for the CAN frame. For classic CAN frames the function always
   ** returns \c false.
   */
   bool        errorStateIndicator(void) const;


   /*!
   ** \return  Identifier of CAN frame
   ** \see     setIdentifier()
   **
   ** The function returns the identifier value. In order to distinguish
   ** between standard (11 bit) and extended (29 bit) frame formats use
   ** frameFormat() or isExtended().
   */
   uint32_t    identifier(void) const;


   /*!
   ** \return  \c true if Extended CAN frame
   ** \see     frameType()
   **
   ** The function returns \c true if the frame type is either #eFORMAT_CAN_EXT
   ** or #eFORMAT_FD_EXT, otherwise it will return \c false;
   */
   bool        isExtended(void) const;


   /*!
   ** \return  \c true if Remote frame
   ** \see     setRemote()
   **
   ** The function returns \c true if the CAN frame is a Remote frame.
   ** Please note that only classic CAN frames have this attribute, i.e.
   ** a ISO CAN FD frame will always return \c false.
   */
   bool        isRemote(void) const;

   /*!
   ** \return  Marker of CAN frame
   ** \see     setMarker()
   **
   ** The function returns the message marker of the frame.
   */
   uint32_t    marker(void) const;

   /*!
   ** \param[in]  btBrsR         Value of BRS bit
   ** \see        bitrateSwitch()
   **
   ** The function sets the value of the bit-rate switch (BRS)
   ** bit of a ISO CAN FD frame. The function has no impact on a classic
   ** CAN frame.
   */
   void        setBitrateSwitch(const bool & btBrsR = true);


   /*!
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  ubValueR       Data of payload
   ** \see        data()
   **
   ** This function sets the data in a CAN message. The parameter
   ** \c ubPosR must be within the range 0 .. 7 for classic frames and
   ** 0 .. 63 for FD frames.
   */
   void        setData(const uint8_t & ubPosR, const uint8_t & ubValueR);

   void        setData(const QByteArray &clDataR);

   /*!
   ** \param[in]  ubSizeV        Size of payload
   ** \see        setDlc()
   **
   ** The function sets the number of data bytes that are valid for the CAN
   ** frame. The possible value range is 0 to 8 for classic CAN frames and
   ** 0 to 64 for CAN FD frames.
   */
   void        setDataSize(uint8_t ubSizeV);

   /*!
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  uwValueR       16-bit value
   ** \param[in]  btMsbFirstR    Byte order
   ** \see        dataUInt16()
   **
   ** The function sets an unsigned 16-bit value (uint16_t) \a uwValueR
   ** starting at position \a ubPosR of the CAN payload.
   */
   void        setDataUInt16(const uint8_t & ubPosR, const uint16_t & uwValueR,
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
   void        setDataUInt32(const uint8_t & ubPosR, const uint32_t & ulValueR,
                             const bool & btMsbFirstR = 0);

   /*!
   ** \param[in]  ubDlcV         DLC value
   ** \see                       setDataSize()
   **
   ** The function sets the DLC value (data length code)
   ** for the CAN frame. The possible value range is 0 to 8 for
   ** classic CAN frames and 0 to 15 for CAN FD frames.
   */
   void        setDlc(uint8_t ubDlcV);


   /*!
   ** \param[in]  btEsiR         Value of ESI bit
   ** \see        errorStateIndicator()
   **
   ** The function sets the value of the Error State Indicator (ESI)
   ** bit of a ISO CAN FD frame. The function has no impact on a classic
   ** CAN frame.
   */
   void        setErrorStateIndicator(const bool & btEsiR = true);


   /*!
   ** \param[in]  ubFormatR  CAN frame format
   ** \see        frameType()
   **
   ** The function sets the CAN frame format, defined by the #Format_e
   ** enumeration. Please note that a change of frame format has
   ** an impact on several internal CAN frame bits:
   **
   ** <b>Changing from Classic CAN to ISO CAN FD</b><br>
   ** In this case the RTR bit is cleared and the FDF bit is set.
   ** Both ESI and BRS are cleared by default.
   **
   ** <b>Changing from ISO CAN FD to Classic CAN</b><br>
   ** In this case the the FDF bit is cleared, also the ESI and BRS
   ** bits are cleared. The RTR bit is cleared by default. If the
   ** current DLC value exceeds the value 8, the DLC value will be
   ** set to 8.
   */
   void        setFrameFormat(const Format_e &ubFormatR);


   /*!
   ** \param[in]  ulIdentifierV  CAN frame type
   ** \see        identifier()
   **
   ** The function sets the identifier value of a CAN frame. Depending
   ** on the CAN frame type (Standard or Extended) the value is truncated
   ** to a 11-bit value (for Standard frames) or a 29-bit value (for
   ** Extended frames).
   */
   void        setIdentifier(uint32_t ulIdentifierV);

   /*!
   ** \param[in]  ulMarkerValueR    Marker value
   ** \see        marker()
   **
   ** The function sets a message marker for the CAN frame. The purpose
   ** of the message marker is application specific.
   */
   void        setMarker(const uint32_t & ulMarkerValueR);

   /*!
   ** \param[in]  btRtrR         Value of RTR bit
   ** \see        isRemote()
   **
   ** The function sets the value of the Remote Transmission Request (RTR)
   ** bit of a classic CAN frame.
   */
   void        setRemote(const bool & btRtrR = true);

   void        setUser(const uint32_t & ulUserValueR);

   /*!
   ** \param[in]  clTimeStampR    Time-stamp
   ** \see        timeStamp()
   **
   ** The function sets the time-stamp of the CAN frame.
   */
   inline void  setTimeStamp(const QCanTimeStamp & clTimeStampR)       \
                     { clMsgTimeP = clTimeStampR; };

   /*!
   ** \return     Time-stamp
   ** \see        setTimeStamp()
   **
   ** The function returns the time-stamp value of the CAN frame.
   */
   inline QCanTimeStamp timeStamp(void) const { return clMsgTimeP; };


   
   QByteArray toByteArray() const;
   
   /*!
   ** \return     CAN frame as QString object
   ** \param[in]  btShowTimeR    Print time-stamp
   **
   ** The function returns the identifier, frame format, DLC value
   ** and payload of a CAN frame as QString object.
   **
   */
   virtual QString    toString(const bool & btShowTimeR = false);
   
   uint32_t    user(void) const;
             
   friend QDataStream & operator<< (QDataStream & clStreamR, 
                                    const QCanFrame & clCanFrameR);
   
   friend QDataStream & operator>> (QDataStream & clStreamR, 
                                    QCanFrame & clCanFrameR);
   
private:
   
   
};





#endif   // QCAN_FRAME_HPP_ 

