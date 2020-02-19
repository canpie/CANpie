//====================================================================================================================//
// File:          qcan_frame.hpp                                                                                      //
// Description:   QCAN classes - CAN frame                                                                            //
//                                                                                                                    //
// Copyright (C) MicroControl GmbH & Co. KG                                                                           //
// 53844 Troisdorf - Germany                                                                                          //
// www.microcontrol.net                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the   //
// following conditions are met:                                                                                      //
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions, the following   //
//    disclaimer and the referenced file 'LICENSE'.                                                                   //
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       //
//    following disclaimer in the documentation and/or other materials provided with the distribution.                //
// 3. Neither the name of MicroControl nor the names of its contributors may be used to endorse or promote products   //
//    derived from this software without specific prior written permission.                                           //
//                                                                                                                    //
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance     //
// with the License. You may obtain a copy of the License at                                                          //
//                                                                                                                    //
//    http://www.apache.org/licenses/LICENSE-2.0                                                                      //
//                                                                                                                    //
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed   //
// on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for  //
// the specific language governing permissions and limitations under the License.                                     //
//                                                                                                                    //
//====================================================================================================================//


#ifndef QCAN_FRAME_HPP_
#define QCAN_FRAME_HPP_


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QByteArray>
#include <QtCore/QDataStream>
#include <QtCore/QString>

#include "qcan_defs.hpp"
#include "qcan_namespace.hpp"
#include "qcan_timestamp.hpp"

using namespace QCan;


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//----------------------------------------------------------------------------------------------------------------
/*!
** \def  QCAN_MSG_DATA_MAX
**
** The symbol CAN_MSG_DATA_MAX defines the maximum number of
** bytes in a CAN frame (payload).
*/
#define  QCAN_MSG_DATA_MAX           64

//----------------------------------------------------------------------------------------------------------------
/*!
** \defgroup QCAN_FRAME QCan frame definitions
**
*/

//----------------------------------------------------------------------------------------------------------------
/*!
** \def     QCAN_FRAME_ID_MASK_STD
** \ingroup QCAN_FRAME
**
** This symbol defines an identifier mask for standard frames.
*/
#define  QCAN_FRAME_ID_MASK_STD      ((uint32_t) 0x000007FF)

//----------------------------------------------------------------------------------------------------------------
/*!
** \def     QCAN_FRAME_ID_MASK_EXT
** \ingroup QCAN_FRAME
**
** This symbol defines an identifier mask for extended frames.
*/
#define  QCAN_FRAME_ID_MASK_EXT      ((uint32_t) 0x1FFFFFFF)

//----------------------------------------------------------------------------------------------------------------
/*!
** \def     QCAN_FRAME_ARRAY_SIZE
** \ingroup QCAN_FRAME
**
** This symbol defines the size of a byte array for a QCanData object (see QCanData::fromByteArray() and
** QCanData::toByteArray() for details).
*/
#define  QCAN_FRAME_ARRAY_SIZE       96

#define  QCAN_FRAME_TIME_STAMP_POS   70

//----------------------------------------------------------------------------------------------------------------
// Define a forward reference to the structure CpCanMsg_s, which is defined inside the header canpie.h
//
struct CpCanMsg_s;


//----------------------------------------------------------------------------------------------------------------
/*!
** \class   QCanFrame
**
** The QCanFrame class supports CAN data frames and CAN error frames. The method frameType() is used to
** distinguish between these formats. The frame type can be selected either by the constructor
** QCanFrame(const FrameType_e &) or by using setFrameType().
** <p>
** <h2>Data Frame</h2>
** The QCanFrame class distinguishes the four message frame formats via the method frameFormat(), possible values
** are listed by the enumeration QCanFrame::FrameFormat_e.
** <div class="function" style="width:800px">
** <table class="function" style="width:800px">
**
** <tr class="odd">
**   <td class="entry" style="width:50%">Classical base frame format (CBFF)</td>
**   <td class="desc">QCanFrame::eFORMAT_CAN_STD</td>
** </tr>
** <tr>
**   <td class="entry" style="width:50%">Classical extended frame format (CEFF)</td>
**   <td class="desc">QCanFrame::eFORMAT_CAN_EXT</td>
** </tr>
** <tr class="odd">
**   <td class="entry" style="width:50%">FD base frame format (FBFF)</td>
**   <td class="desc">QCanFrame::eFORMAT_FD_STD</td>
** </tr>
** <tr>
**   <td class="entry" style="width:50%">FD extended frame format (FEFF)</td>
**   <td class="desc">QCanFrame::eFORMAT_FD_EXT</td>
** </tr>
** </table>
** </div>
**
** The class provides two application specific fields with a size of 32-bit which are compliant are compliant to
** the #CpCanMsg_s structure They can be accessed by the setMarker() and setUser() methods. An optional time-stamp
** is provided by the setTimeStamp() method.
** <p>
** <h2>Error Frame</h2>
**
** A frame of type QCanFrame::eFRAME_TYPE_ERROR is used to inform about the actual <b>error state</b> (function
** errorState()) and the current values of the error counters (functions errorCounterReceive() and
** errorCounterTransmit()). In addition, it is possible to inform about the last error type which caused the
** error condition (function errorType()).
** \code
** QCanFrame   clErrorFrameT;
**
** //--------------------------------------------------------------------------
** // set frame type to error frame
** //
** clErrorFrameT.setFrameType(QCanFrame::eFRAME_TYPE_ERROR);
**
** clErrorFrameT.setErrorCounterReceive(32);
** clErrorFrameT.setErrorCounterTransmit(64);
** \endcode
** <p>
*/
class QCanFrame
{
public:


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \enum ErrorType_e
   **
   ** The enumeration ErrorType_e defines CAN error types.
   */
   enum ErrorType_e {

      /*! no error                                          */
      eERROR_TYPE_NONE   = 0,

      /*! Bit 0 error occurred                              */
      eERROR_TYPE_BIT0,

      /*! Bit 1 error occurred                              */
      eERROR_TYPE_BIT1,

      /*! Stuff error occurred                              */
      eERROR_TYPE_STUFF,

      /*! Form  error occurred                              */
      eERROR_TYPE_FORM,

      /*! CRC error occurred                                */
      eERROR_TYPE_CRC,

      /*! Acknowledgement error occurred                    */
      eERROR_TYPE_ACK
   };

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \enum    FrameFormat_e
   **
   ** This enumeration defines the CAN frame formats for classical CAN and CAN FD.
   */
   enum FrameFormat_e {

      /*! Classic CAN, Standard frame format                */
      eFORMAT_CAN_STD = 0,

      /*! Classic CAN, Extended frame format                */
      eFORMAT_CAN_EXT,

      /*! ISO CAN FD, Standard frame format                 */
      eFORMAT_FD_STD,

      /*! ISO CAN FD, Extended frame format                 */
      eFORMAT_FD_EXT,

   };

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \enum FrameType_e
   **
   ** The enumeration FrameType_e defines the different CAN frame types.
   */
   enum FrameType_e {

      /*! Unknown frame type                                */
      eFRAME_TYPE_UNKNOWN = 0,

      /*! Data frame                                        */
      eFRAME_TYPE_DATA,

      /*! Error frame                                       */
      eFRAME_TYPE_ERROR

   };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** Constructs an empty classic standard CAN frame (frame format QCanFrame::eFORMAT_CAN_STD) with a
   ** DLC value of 0.
   */
   QCanFrame();


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubTypeR  Frame type
   **
   ** Constructs an empty data structure with a frame type of QCanFrame::FrameType_e.
   */
   QCanFrame(const FrameType_e & ubTypeR);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubFormatR      CAN frame format
   ** \param[in]  ulIdentifierR  CAN frame identifier
   ** \param[in]  ubDlcR         CAN frame data length code
   **
   ** Constructs a CAN frame of type \c ubTypeR with an identifier value of \c ulIdentifierR and a DLC
   ** value of \c ubDlcR.
   */
   QCanFrame(const FrameFormat_e & ubFormatR, const uint32_t & ulIdentifierR = 0, const uint8_t & ubDlcR = 0);


   virtual ~QCanFrame();

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  \c true if bit-rate switch is set
   ** \see     setBitrateSwitch()
   **
   ** The function returns \c true if the bit-rate switch bit is set for the CAN frame. For classic CAN
   ** frames the function always returns \c false.
   */
   bool        bitrateSwitch(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Data at payload position \a ubPosR
   ** \param[in]  ubPosR   Index of payload
   **
   ** The function returns the data stored at payload position \a ubPosR. The first position starts at
   ** index 0. If the index position is out of range, the function will return 0. The maximum index
   ** position for a CAN frame is dataSize() - 1, as depicted in the following code
   ** example.
   ** \code
   ** QCanFrame  clCanFrameT;
   ** ..
   ** for (uint8_t ubPosT = 0; ubPosT < clCanFrameT.dataSize(); ubPosT++)
   ** {
   **    //------------------------------------------------
   **    // print payload of CAN frame
   **    //
   **    cout << hex << clCanFrameT.data(ubPosT);
   ** }
   ** \endcode
   */
   uint8_t     data(const uint8_t & ubPosR) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Data of CAN frame
   **
   ** The function returns the data stored in the CAN frame as QByteArray.
   */
   QByteArray  data(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Size of payload in bytes
   ** \see        dlc()
   **
   ** The function returns the number of bytes that are valid for the CAN frame. The possible value
   ** range is 0 to 8 for classic CAN frames and 0 to 64 for CAN FD frames. In order to obtain the data
   ** length code (DLC) use the dlc() function.
   */
   uint8_t     dataSize(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Unsigned 16-bit data
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  btMsbFirstR    Byte order
   ** \see        setDataUInt16()
   **
   ** The function returns an unsigned 16-bit value (uint16_t) starting at position \a ubPosR of the
   ** CAN payload.
   */
   uint16_t    dataUInt16(const uint8_t & ubPosR, const bool & btMsbFirstR = false) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Unsigned 32-bit data
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  btMsbFirstR    Byte order
   ** \see        setDataUInt32()
   **
   ** The function returns an unsigned 32-bit value (uint32_t) starting at position \a ubPosR of the
   ** CAN payload.
   */
   uint32_t    dataUInt32(const uint8_t & ubPosR, const bool & btMsbFirstR = false) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Data length code (DLC)
   ** \see        dataSize()
   **
   ** The function returns the DLC value (data length code) for the CAN frame. The possible value range
   ** is 0 to 8 for classic CAN frames and 0 to 15 for CAN FD frames. In order to obtain the number of
   ** bytes inside the payload use dataSize().
   */
   uint8_t     dlc(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Value of receive error counter
   ** \see        setErrorCounterReceive()
   **
   ** The functions returns the current value of the receive error counter for an error frame (frame
   ** type QCanFrame::eFRAME_TYPE_ERROR). For a data frame the function returns 0.
   */
   uint8_t     errorCounterReceive(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Value of transmit error counter
   ** \see        setErrorCounterTransmit()
   **
   ** This functions returns the current value of the transmit error counter for an error frame (frame
   ** type QCanFrame::eFRAME_TYPE_ERROR). For a data frame the function returns 0.
   ** A value of 255 leads to a bus-off condition (QCan::eCAN_STATE_BUS_OFF).
   */
   uint8_t     errorCounterTransmit(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Current error state
   ** \see        setErrorState()
   **
   ** This functions returns the current error state, defined by the enumeration QCan::CAN_State_e.
   */
   CAN_State_e errorState(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  \c true if error state indicator is set
   ** \see     setErrorStateIndicator()
   **
   ** The function returns \c true if the error state indicator bit is set inside a CAN FD frame. For
   ** classic CAN frames the function always returns \c false.
   */
   bool        errorStateIndicator(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Error type
   ** \see        setErrorType()
   **
   ** This functions returns the error type which caused the last error condition.
   */
   ErrorType_e errorType(void) const;

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Frame format
   ** \see     setFrameFormat()
   **
   ** The function returns the CAN frame format, defined by the #FrameFormat_e enumeration.
   */
   FrameFormat_e  frameFormat(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Frame type
   ** \see     setFrameType()
   **
   ** The function returns the frame type defined by #FrameType_e.
   */
   FrameType_e frameType(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clByteArrayR   Byte array containing CAN frame data
   ** \return     Conversion result
   ** \see        toByteArray()
   **
   ** The function converts a QByteArray object to a QCanFrame object. On success, the function returns
   ** \c true, otherwise \c false.
   */
   bool        fromByteArray(const QByteArray & clByteArrayR);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ptsCanMsgV   Pointer to CANpie message structure
   ** \return     Conversion result
   ** \see        toCpCanMsg()
   **
   ** The function converts a structure of type CpCanMsg_ts to a QCanFrame object. On success, the
   ** function returns \c true, otherwise \c false.
   */
   bool        fromCpCanMsg(const struct CpCanMsg_s * ptsCanMsgV);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Identifier of CAN frame
   ** \see     setIdentifier()
   **
   ** The function returns the identifier value. In order to distinguish between standard (11 bit)
   ** and extended (29 bit) frame formats use either frameFormat() or isExtended().
   */
   uint32_t    identifier(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  \c true if Extended CAN frame
   ** \see     frameType()
   **
   ** The function returns \c true if the frame type is either #eFORMAT_CAN_EXT or #eFORMAT_FD_EXT,
   ** otherwise it will return \c false;
   */
   bool        isExtended(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  \c true if Remote frame
   ** \see     setRemote()
   **
   ** The function returns \c true if the CAN frame is a Remote frame. Please note that only classic
   ** CAN frames have this attribute, i.e. a ISO CAN FD frame will always return \c false.
   */
   bool        isRemote(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Marker of CAN frame
   ** \see     setMarker()
   **
   ** The function returns the message marker of the frame.
   */
   uint32_t    marker(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  btBrsR         Value of BRS bit
   ** \see        bitrateSwitch()
   **
   ** The function sets the value of the bit-rate switch (BRS)
   ** bit of a ISO CAN FD frame. The function has no impact on a classic
   ** CAN frame.
   */
   void        setBitrateSwitch(const bool & btBrsR = true);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  ubValueR       Data of payload
   ** \see        data()
   **
   ** This function sets the data in a CAN message. The parameter \c ubPosR must be within the range
   ** 0 .. 7 for classic frames and 0 .. 63 for FD frames.
   */
   void        setData(const uint8_t & ubPosR, const uint8_t & ubValueR);

   void        setData(const QByteArray &clDataR);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubSizeV        Size of payload
   ** \see        setDlc()
   **
   ** The function sets the number of data bytes that are valid for the CAN frame. The possible value
   ** range is 0 to 8 for classic CAN frames and 0 to 64 for CAN FD frames.
   */
   void        setDataSize(uint8_t ubSizeV);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  uwValueR       16-bit value
   ** \param[in]  btMsbFirstR    Byte order
   ** \see        dataUInt16()
   **
   ** The function sets an unsigned 16-bit value (uint16_t) \a uwValueR starting at position \a ubPosR
   ** of the CAN payload.
   */
   void        setDataUInt16(const uint8_t & ubPosR, const uint16_t & uwValueR,
                             const bool & btMsbFirstR = 0);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubPosR         Index of payload
   ** \param[in]  ulValueR       32-bit value
   ** \param[in]  btMsbFirstR    Byte order
   ** \see        dataUInt32()
   **
   ** The function sets an unsigned 32-bit value (uint32_t) \a ulValueR starting at position \a ubPosR
   ** of the CAN payload.
   */
   void        setDataUInt32(const uint8_t & ubPosR, const uint32_t & ulValueR,
                             const bool & btMsbFirstR = 0);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubDlcV         DLC value
   ** \see                       setDataSize()
   **
   ** The function sets the DLC value (data length code) for a CAN data frame. The possible value range
   ** is 0 to 8 for classic CAN frames and 0 to 15 for CAN FD frames. If the value is out of range it
   ** is not stored in the frame object.
   */
   void        setDlc(uint8_t ubDlcV);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubErrorCntR    Receive error counter value
   ** \see        errorCounterReceive()
   **
   ** The functions sets the receive error counter for an error frame (frame type
   ** QCanFrame::eFRAME_TYPE_ERROR). For a data frame the function has no impact. Passing a value
   ** greater than 127 will set the error state to error passive (QCan::eCAN_STATE_BUS_PASSIVE).
   */
   void        setErrorCounterReceive(const uint8_t & ubErrorCntR);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubErrorCntR    Transmit error counter value
   ** \see        errorCounterTransmit()
   **
   ** The functions sets the transmit error counter for an error frame (frame type
   ** QCanFrame::eFRAME_TYPE_ERROR). For a data frame the function has no impact. Passing a value
   ** greater than 127 will set the error state to error passive (QCan::eCAN_STATE_BUS_PASSIVE). A value
   ** of 255 will set the error state to bus-off (QCan::eCAN_STATE_BUS_OFF).
   */
   void        setErrorCounterTransmit(const uint8_t & ubErrorCntR);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubStateV       Error state value
   ** \see                       errorState()
   **
   ** This functions sets the current error state, defined by the enumeration QCan::CAN_State_e.
   */
   void        setErrorState(CAN_State_e ubStateV);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubTypeV        Error type value
   ** \see        errorType()
   **
   ** This functions sets the error type which caused the last error condition.
   */
   void        setErrorType(ErrorType_e ubTypeV);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  btEsiR         Value of ESI bit
   ** \see        errorStateIndicator()
   **
   ** The function sets the value of the Error State Indicator (ESI) bit of a ISO CAN FD frame. The
   ** function has no impact on a classic CAN frame.
   */
   void        setErrorStateIndicator(const bool & btEsiR = true);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubFormatR  CAN frame format
   ** \see        frameType()
   **
   ** The function sets the CAN frame format, defined by the #FrameFormat_e enumeration. Please note
   ** that a change of frame format has an impact on several internal CAN frame bits:
   **
   ** <b>Changing from Classic CAN to ISO CAN FD</b><br>
   ** In this case the RTR bit is cleared and the FDF bit is set. Both ESI and BRS are cleared by
   ** default.
   **
   ** <b>Changing from ISO CAN FD to Classic CAN</b><br>
   ** In this case the the FDF bit is cleared, also the ESI and BRS bits are cleared. The RTR bit is
   ** cleared by default. If the current DLC value exceeds the value 8, the DLC value will be set to 8.
   */
   void        setFrameFormat(const FrameFormat_e &ubFormatR);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubTypeR  CAN frame type
   ** \see        frameType()
   **
   ** The function sets the CAN frame format, defined by the #FrameType_e enumeration. Please note that
   ** changing the frame type will set all data bytes to 0.
   */
   void        setFrameType(const FrameType_e &ubTypeR);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ulIdentifierV  CAN frame type
   ** \see        identifier()
   **
   ** The function sets the identifier value of a CAN frame. Depending on the CAN frame type (Standard
   ** or Extended) the value is truncated to a 11-bit value (for Standard frames) or a 29-bit value
   ** (for Extended frames).
   */
   void        setIdentifier(uint32_t ulIdentifierV);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ulMarkerValueR    Marker value
   ** \see        marker()
   **
   ** The function sets a message marker inside the CAN frame. The purpose of the message marker is
   ** application specific.
   */
   void        setMarker(const uint32_t & ulMarkerValueR);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  btRtrR         Value of RTR bit
   ** \see        isRemote()
   **
   ** The function sets the value of the Remote Transmission Request (RTR) bit of a classic CAN frame.
   */
   void        setRemote(const bool & btRtrR = true);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ulUserValueR    User value
   ** \see        user()
   **
   ** The function sets a user-defined value inside the CAN frame. The purpose of the field is
   ** application specific.
   */
   void        setUser(const uint32_t & ulUserValueR);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clTimeStampR    Time-stamp
   ** \see        timeStamp()
   **
   ** The function sets the time-stamp of the CAN frame.
   */
   inline void setTimeStamp(const QCanTimeStamp & clTimeStampR)     { clMsgTimeP = clTimeStampR; };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Time-stamp
   ** \see        setTimeStamp()
   **
   ** The function returns the time-stamp value of the CAN frame.
   */
   inline QCanTimeStamp timeStamp(void) const { return clMsgTimeP; };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     QByteArray
   ** \see        fromByteArray()
   **
   ** The function converts a QCanFrame object to a QByteArray. The size of the array is defined by
   ** #QCAN_FRAME_ARRAY_SIZE.
   */
   QByteArray  toByteArray(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ptsCanMsgV   Pointer to CANpie message structure
   ** \return     Conversion result
   ** \see        fromCpCanMsg()
   **
   ** The function converts a QCanFrame to a structure of type CpCanMsg_ts. On success, the
   ** function returns \c true, otherwise \c false.
   */
   bool        toCpCanMsg(struct CpCanMsg_s * ptsCanMsgV) const;

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     CAN frame as QString object
   ** \param[in]  btShowTimeR       Print time-stamp
   ** \param[in]  btShowDataSizeR   Print data size instead of DLC value
   **
   ** The function converts a QCanFrame object to a QString. The output depends on the frame type and
   ** frame format. Frame formats are denoted with the abbreviations CBFF, CEFF, FBFF and FEFF. For
   ** CAN FD frames the fields bit-rate switch (BRS) and error state indicator (ESI) are shown if
   ** set. The parameter \a btShowDataSizeR defines if the DLC value (dlc()) is shown or the data
   ** size (dataSize()).
   **
   ** \code
   **      100  CBFF          2  11 22
   ** 18EEFF00  CEFF          3  45 AB 12
   **      200  FBFF BRS,ESI  9  11 22 00 00 00 00 00 00 00 00 00 00
   **   2AFF33  FEFF BRS     10  88 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
   **    Error frame ----------  Error passive, Error counter: Rcv=128, Trm=32
   ** \endcode
   **
   */
   virtual QString    toString(const bool & btShowTimeR = false, const bool & btShowDataSizeR = false) const;

   uint32_t    user(void) const;

   friend QDataStream & operator<< (QDataStream & clStreamR, const QCanFrame & clCanFrameR);

   friend QDataStream & operator>> (QDataStream & clStreamR, QCanFrame & clCanFrameR);

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





#endif   // QCAN_FRAME_HPP_
