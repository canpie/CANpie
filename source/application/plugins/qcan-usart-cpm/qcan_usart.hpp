//====================================================================================================================//
// File:          qcan_usart.hpp                                                                                      //
// Description:   QCAN USART library handling API                                                                     //
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


#ifndef QCAN_USART_H_
#define QCAN_USART_H_

#include <QtCore/QObject>


#include <QtCore/QLibrary>
#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "cp_msg.h"

//--------------------------------------------------------------------------------------------------------------------//
// QCanUsart                                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
class QCanUsart : public QObject
{
    Q_OBJECT

public:
   typedef struct QCanUsartConfig_s{
      QString clName;
      qint32  slBaud;
      quint8  ubMode;
   } QCanUsartConfig_ts;

   //------------------------------------------------------------------------------
   /*!
   ** \enum USART_MODE_e
   */
   enum USART_MODE_e
   {
      eUSART_MODE_7N1 = 1,
      eUSART_MODE_7E1,
      eUSART_MODE_7O1,
      eUSART_MODE_8N1,
      eUSART_MODE_8E1,
      eUSART_MODE_8O1
   };

   /*!
   ** \brief   Define size of USART frame that transmitts the CpCanMsg_ts
   ** \def     USART_FRAME_SIZE
   */
   #define USART_FRAME_SIZE 86

private:
   QCanUsart();
   QCanUsart(const QCanUsart &);

   QSerialPort    *pclSerialPortP;

   QCanUsartConfig_ts tsConfigP;

   /*!
    * \brief CpCanMsgToByteArray
    * \param tsCanMessageV - CAN message, that should be converted in to a byte array.
    * \return Byte array of CAN message
    */
   QByteArray  CpCanMsgToByteArray(CpCanMsg_ts tsCanMessageV);

   /*!
    * \brief CpCanMsgFromByteArray
    * \param clByteArrayV - Byte array that should be converted in to a CAN message.
    * \param pbtOkV - Optional parameter for the result of the conversion.
    * \return CANpie Message Object
    */
   CpCanMsg_ts CpCanMsgFromByteArray(QByteArray clByteArrayV, bool *pbtOkV = Q_NULLPTR);

   quint32 ulUsartFrameRcvP;

private slots:
   void onError(QSerialPort::SerialPortError error);
   void onReadyRead(void);
   void onBytesWritten(qint64 sqByteCountV);

signals:
   void logMessage(const QString & clMessageR);
   void messageReceive(CpCanMsg_ts);

public:
   ~QCanUsart();

   /*!
    * \brief Get USART device interface configuration.
    * \return Current configuration of USART device interface,
    *         which parameters are provided by QCanUsartConfig_s.
    */
   QCanUsartConfig_ts & currentConfig();

   void setCanBitrate(int32_t slNomBitRateV, int32_t slDatBitRateV);
   void setCanMode(uint8_t ubModeV);

   /*!
    * \brief Provide new/updated configuration for USART device interface.
    * \return Result for new configuration process
    */
   bool setConfig(QCanUsartConfig_ts &);

   /*!
    * \brief Provide name of Serial Interface for initialisation.
    * \attention This Function should be called before connection establishement.
    */
   void setDeviceName(QString);

   /*!
    * \brief isAvailable
    * \return Returns true if interface acces is available and false if not.
    */
   bool isAvailable(void);

   /*!
    * \brief errorString
    * \return Returns a human-readable description of the last error that occurred.
    */
   QString errorString();

   /*!
    * \brief Allow only one instance of PCAN Basic
    */
   static QCanUsart & getInstance()
   {
      static QCanUsart clInstanceS;
      return clInstanceS;
   }

   /*!
    * \brief Connect the USART interface device
    * \return
    */
   bool connect(void);

   /*!
    * \brief Release USART interface device
    */
   void release(void);

   /*!
    * \brief Send a message
    * \return
    */
   bool messageSend(CpCanMsg_ts);
};

#endif /*QCAN_USART_H_*/
