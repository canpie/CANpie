//============================================================================//
// File:          canpie_frame.hpp                                            //
// Description:   CANpie classes - CAN frame                                  //
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


#ifndef CANPIE_FRAME_HPP_
#define CANPIE_FRAME_HPP_


#ifndef  CANPIE_QT_SUPPORT
#define  CANPIE_QT_SUPPORT          1
#endif


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
** \file canpie_frame.hpp
**
*/


//-------------------------------------------------------------------
/*!
** \def  CAN_MSG_DATA_MAX
**
** The symbol CAN_MSG_DATA_MAX defines the maximum number of
** bytes in a CAN frame (payload).
*/
#define  CAN_MSG_DATA_MAX           64


#define  CAN_FRAME_ID_MASK_STD      ((uint32_t) 0x000007FF)

#define  CAN_FRAME_ID_MASK_EXT      ((uint32_t) 0x1FFFFFFF)

#define  QCAN_FRAME_ARRAY_SIZE      96


//-----------------------------------------------------------------------------
/*!
** \class   CpFrame
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
class CpFrame
{
public:
   
   enum Type_e {

      eTYPE_API = 0,
      
      eTYPE_CAN,

      eTYPE_ERROR

   };
   
   
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
   ** Constructs an empty classic standard CAN frame (type eFORMAT_CAN_STD)
   ** with a DLC value of 0.
   */
   CpFrame();
   

   /*!
   ** \param[in] ubFormatR       CAN frame format
   ** \param[in] ulIdentifierR   Identifier value
   ** \param[in] ubDlcR          DLC value
   **
   ** Constructs a CAN frame of type \c ubTypeR with an identifier value
   ** of \c ulIdentifierR and a DLC value of \c ubDlcR.
   */
   CpFrame(const Format_e & ubFormatR, const uint32_t & ulIdentifierR = 0, 
           const uint8_t & ubDlcR = 0);
   
   virtual ~CpFrame();
   

   /*!
   ** \return  \c true if bit-rate switch is set
   ** \see     setBitrateSwitch()
   **
   ** The function returns \c true if the bit-rate switch bit is set
   ** for the CAN frame. For classic CAN frames the function always
   ** returns \c false.
   */
   bool        bitrateSwitch(void) const;

   // inline uint8_t    controlField(void) const   { return (ubMsgCtrlP); };

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
   
   Type_e      frameType(void) const;
   

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

   // inline uint32_t    idField(void) const  { return (ulIdentifierP); };
   
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
   inline uint32_t marker(void) const  { return ulMsgMarkerP;  };
   
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
   ** \param[in]  ubTypeR  CAN frame type
   ** \see        frameType()
   **
   ** The function sets the CAN frame type, defined by the #Type_e
   ** enumeration. Please note that a change of frame type has
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
   **
   **
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
   inline void  setTimeStamp(const CpTimeStamp & clTimeStampR)       \
                     { clMsgTimeP = clTimeStampR; };

   /*!
   ** \return     Time-stamp
   ** \see        setTimeStamp()
   **
   ** The function returns the time-stamp value of the CAN frame.
   */
   inline CpTimeStamp timeStamp(void) const { return clMsgTimeP; };

   bool operator==(const CpFrame & clCanFrameR);
   
   bool operator!=(const CpFrame & clCanFrameR);
   
protected:
   void               setFrameType(const Type_e &ubTypeR);

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





#endif   // CANPIE_FRAME_HPP_ 

