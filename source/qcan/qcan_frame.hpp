//============================================================================//
// File:          qcan_frame.cpp                                              //
// Description:   QCAN classes - CAN frame                                    //
// Author:        Uwe Koppe                                                   //
// e-mail:        koppe@microcontrol.net                                      //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// Junkersring 23                                                             //
// 53844 Troisdorf                                                            //
// Germany                                                                    //
// Tel: +49-2241-25659-0                                                      //
// Fax: +49-2241-25659-11                                                     //
//                                                                            //
// The copyright to the computer program(s) herein is the property of         //
// MicroControl GmbH & Co. KG, Germany. The program(s) may be used            //
// and/or copied only with the written permission of MicroControl GmbH &      //
// Co. KG or in accordance with the terms and conditions stipulated in        //
// the agreement/contract under which the program(s) have been supplied.      //
//----------------------------------------------------------------------------//
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 20.05.2015  Initial version                                                //
//                                                                            //
//============================================================================//



#ifndef QCAN_FRAME_HPP_
#define QCAN_FRAME_HPP_


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
      eTYPE_CAN_STD = 0,
      eTYPE_CAN_EXT,
      eTYPE_FD_STD,
      eTYPE_FD_EXT,
      eTYPE_ERROR = 0x10,
      eTYPE_FUNCTION = 0x20
   };

   QCanFrame();
   
   QCanFrame(const Type_e & ubTypeR, const uint32_t & ulIdentifierR = 0, 
             const uint8_t & ubDlcR = 0);
   
   ~QCanFrame();

   
   uint8_t     data(uint8_t ubPosV) const;
   
   QByteArray  data(void) const;
   
   uint8_t     dataSize(void) const;
   
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
   void        setData(const uint8_t &ubPosR, const uint8_t ubValueR);
   
   void        setData(const QByteArray &clDataR);
   
   void        setDataSize(uint8_t &ubSizeR);
   
   void        setDlc(uint8_t ubDlcV);
   
   void        setExtId(uint32_t ulIdentifierV);
   
   void        setFrameType(const Type_e &ubTypeR);
   
   void        setMarker(const uint32_t & ulMarkerValueR);

   void        setRemoteFrame(const bool & btRtrR = true);
   
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

