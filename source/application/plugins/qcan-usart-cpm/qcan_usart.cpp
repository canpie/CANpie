//====================================================================================================================//
// File:          qcan_usart.cpp                                                                                      //
// Description:   QCAN USART library handling                                                                         //
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


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/
#include "qcan_usart.hpp"


//--------------------------------------------------------------------------------------------------------------------//
// QCanUsart()                                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanUsart::QCanUsart()
{
   //---------------------------------------------------------------------------------------------------
   // setup default USART configuration
   //
   pclSerialPortP = Q_NULLPTR;
   ulUsartFrameRcvP = 0;

   //---------------------------------------------------------------------------------------------------
   // setup default USART configuration, that can be ovewritten by the loaded or user configuration
   //
   tsConfigP.clName = "COM1";
   tsConfigP.slBaud = 3000000;
   tsConfigP.ubMode = eUSART_MODE_8N1;
}

//--------------------------------------------------------------------------------------------------------------------//
// ~QCanUsart()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanUsart::~QCanUsart()
{
   qDebug() << "QCanUsart::~QCanUsart()";
}


//--------------------------------------------------------------------------------------------------------------------//
// connect()                                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanUsart::connect()
{
   return setConfig(tsConfigP);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpCanMsgToByteArray()                                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CpCanMsg_ts QCanUsart::CpCanMsgFromByteArray(QByteArray clArrayV, bool *pbtOkV)
{
   CpCanMsg_ts tsCanMessageT;
   quint32     ulCrcT = 0x5A5A5A5A;
   quint32     ulIndexT = 0;

   //---------------------------------------------------------------------------------------------------
   // convert theByte Array to CANpie CAN Message (Message to Value)
   //
   if (clArrayV.size() == USART_FRAME_SIZE) // size of CpCanMsg_ts is 86 byte
   {
      // identifier field
      tsCanMessageT.ulIdentifier   = (0xFF & static_cast<quint32>(clArrayV.data()[3]));
      tsCanMessageT.ulIdentifier <<= 8;
      tsCanMessageT.ulIdentifier  |= (0xFF & static_cast<quint32>(clArrayV.data()[2]));
      tsCanMessageT.ulIdentifier <<= 8;
      tsCanMessageT.ulIdentifier  |= (0xFF & static_cast<quint32>(clArrayV.data()[1]));
      tsCanMessageT.ulIdentifier <<= 8;
      tsCanMessageT.ulIdentifier  |= (0xFF & static_cast<quint32>(clArrayV.data()[0]));

      // data field
      for (int index = 0; index < 64; index++) {
         tsCanMessageT.tuMsgData.aubByte[index] = static_cast<quint8>(clArrayV.data()[4+index]);
      }

      // data length code
      tsCanMessageT.ubMsgDLC = (0xFF & static_cast<quint8>(clArrayV.data()[68]));

      // message control field
      tsCanMessageT.ubMsgCtrl = (0xFF & static_cast<quint8>(clArrayV.data()[69]));

      //time stamp field
      tsCanMessageT.tsMsgTime.ulSec1970   = (0xFF & static_cast<quint32>(clArrayV.data()[73]));
      tsCanMessageT.tsMsgTime.ulSec1970 <<= 8;
      tsCanMessageT.tsMsgTime.ulSec1970  |= (0xFF & static_cast<quint32>(clArrayV.data()[72]));
      tsCanMessageT.tsMsgTime.ulSec1970 <<= 8;
      tsCanMessageT.tsMsgTime.ulSec1970  |= (0xFF & static_cast<quint32>(clArrayV.data()[71]));
      tsCanMessageT.tsMsgTime.ulSec1970 <<= 8;
      tsCanMessageT.tsMsgTime.ulSec1970  |= (0xFF & static_cast<quint32>(clArrayV.data()[70]));

      tsCanMessageT.tsMsgTime.ulNanoSec   = (0xFF & static_cast<quint32>(clArrayV.data()[77]));
      tsCanMessageT.tsMsgTime.ulNanoSec <<= 8;
      tsCanMessageT.tsMsgTime.ulNanoSec  |= (0xFF & static_cast<quint32>(clArrayV.data()[76]));
      tsCanMessageT.tsMsgTime.ulNanoSec <<= 8;
      tsCanMessageT.tsMsgTime.ulNanoSec  |= (0xFF & static_cast<quint32>(clArrayV.data()[75]));
      tsCanMessageT.tsMsgTime.ulNanoSec <<= 8;
      tsCanMessageT.tsMsgTime.ulNanoSec  |= (0xFF & static_cast<quint32>(clArrayV.data()[74]));

      // user data field
      tsCanMessageT.ulMsgUser   = (0xFF & static_cast<quint32>(clArrayV.data()[81]));
      tsCanMessageT.ulMsgUser <<= 8;
      tsCanMessageT.ulMsgUser  |= (0xFF & static_cast<quint32>(clArrayV.data()[80]));
      tsCanMessageT.ulMsgUser <<= 8;
      tsCanMessageT.ulMsgUser  |= (0xFF & static_cast<quint32>(clArrayV.data()[79]));
      tsCanMessageT.ulMsgUser <<= 8;
      tsCanMessageT.ulMsgUser  |= (0xFF & static_cast<quint32>(clArrayV.data()[78]));

      // message marker field
      tsCanMessageT.ulMsgMarker   = (0xFF & static_cast<quint32>(clArrayV.data()[85]));
      tsCanMessageT.ulMsgMarker <<= 8;
      tsCanMessageT.ulMsgMarker  |= (0xFF & static_cast<quint32>(clArrayV.data()[84]));
      tsCanMessageT.ulMsgMarker <<= 8;
      tsCanMessageT.ulMsgMarker  |= (0xFF & static_cast<quint32>(clArrayV.data()[83]));
      tsCanMessageT.ulMsgMarker <<= 8;
      tsCanMessageT.ulMsgMarker  |= (0xFF & static_cast<quint32>(clArrayV.data()[82]));


      //-------------------------------------------------------------------------------------------
      // build simple CRC of received byte data
      //
      if (pbtOkV != Q_NULLPTR)
      {
         while (ulIndexT < USART_FRAME_SIZE-4)
         {
            ulCrcT += (0xFF & static_cast<quint32>(clArrayV.data()[ulIndexT]));
            ulIndexT++;
         }

         if (ulCrcT != tsCanMessageT.ulMsgMarker)
         {
            *pbtOkV = false;
         } else
         {
            *pbtOkV = true;
         }
      }
   } else
   {
      if (pbtOkV != Q_NULLPTR)
      {
         *pbtOkV = false;
      }
   }

   return tsCanMessageT;
}

//--------------------------------------------------------------------------------------------------------------------//
// CpCanMsgToByteArray()                                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QByteArray  QCanUsart::CpCanMsgToByteArray(CpCanMsg_ts tsCanMessageV)
{
   QByteArray clArrayT;
   quint32    ulCrcT = 0x5A5A5A5A;
   quint32    ulIndexT = 0;

   //---------------------------------------------------------------------------------------------------
   // convert the CANpie CAN Message to Byte Array (Value to Message)
   //
   clArrayT.resize(USART_FRAME_SIZE); // size of CpCanMsg_ts is 86 byte

   // identifier field
   clArrayT.data()[0] = static_cast<char>(tsCanMessageV.ulIdentifier);
   clArrayT.data()[1] = static_cast<char>(tsCanMessageV.ulIdentifier >> 8);
   clArrayT.data()[2] = static_cast<char>(tsCanMessageV.ulIdentifier >> 16);
   clArrayT.data()[3] = static_cast<char>(tsCanMessageV.ulIdentifier >> 24);

   // data field
   for (int index = 0; index < 64; index++) {
      clArrayT.data()[4+index] = static_cast<char>(tsCanMessageV.tuMsgData.aubByte[index]);
   }

   // data length code
   clArrayT.data()[68] = static_cast<char>(tsCanMessageV.ubMsgDLC);

   // message control field
   clArrayT.data()[69] = static_cast<char>(tsCanMessageV.ubMsgCtrl);

   //time stamp field
   clArrayT.data()[70] = static_cast<char>(tsCanMessageV.tsMsgTime.ulSec1970);
   clArrayT.data()[71] = static_cast<char>(tsCanMessageV.tsMsgTime.ulSec1970 >> 8);
   clArrayT.data()[72] = static_cast<char>(tsCanMessageV.tsMsgTime.ulSec1970 >> 16);
   clArrayT.data()[73] = static_cast<char>(tsCanMessageV.tsMsgTime.ulSec1970 >> 24);

   clArrayT.data()[74] = static_cast<char>(tsCanMessageV.tsMsgTime.ulNanoSec);
   clArrayT.data()[75] = static_cast<char>(tsCanMessageV.tsMsgTime.ulNanoSec >> 8);
   clArrayT.data()[76] = static_cast<char>(tsCanMessageV.tsMsgTime.ulNanoSec >> 16);
   clArrayT.data()[77] = static_cast<char>(tsCanMessageV.tsMsgTime.ulNanoSec >> 24);

   // user data field
   clArrayT.data()[78] = static_cast<char>(tsCanMessageV.ulMsgUser);
   clArrayT.data()[79] = static_cast<char>(tsCanMessageV.ulMsgUser >> 8);
   clArrayT.data()[80] = static_cast<char>(tsCanMessageV.ulMsgUser >> 16);
   clArrayT.data()[81] = static_cast<char>(tsCanMessageV.ulMsgUser >> 24);

   // message marker field is used as simple CRC
   while (ulIndexT < USART_FRAME_SIZE-4)
   {
      ulCrcT += (0xFF & static_cast<quint32>(clArrayT.data()[ulIndexT]));
      ulIndexT++;
   }

   clArrayT.data()[82] = static_cast<char>(ulCrcT);
   clArrayT.data()[83] = static_cast<char>(ulCrcT >> 8);
   clArrayT.data()[84] = static_cast<char>(ulCrcT >> 16);
   clArrayT.data()[85] = static_cast<char>(ulCrcT >> 24);

   qDebug() << "............................CpCanMsgToByteArray:" << clArrayT.toHex();
   qDebug() << "Expected size (CpCanMsg_ts) is: " + QString::number(sizeof(CpCanMsg_ts),10);

   return clArrayT;
}


//--------------------------------------------------------------------------------------------------------------------//
// currentConfig()                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanUsart::QCanUsartConfig_ts & QCanUsart::currentConfig()
{
   return tsConfigP;
}


//--------------------------------------------------------------------------------------------------------------------//
// errorString()                                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanUsart::errorString()
{
   return "No erro String supported yet.";
}


//--------------------------------------------------------------------------------------------------------------------//
// isAvailable()                                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanUsart::isAvailable()
{
   return true;
}


//--------------------------------------------------------------------------------------------------------------------//
// messageSend()                                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanUsart::messageSend(CpCanMsg_ts tsCanMessageV)
{
   bool btReturnT = false;

   //---------------------------------------------------------------------------------------------------
   // convert the provided CANpie Message in to byte arra and send it
   //
   if (qint32(pclSerialPortP->write(CpCanMsgToByteArray(tsCanMessageV))) == USART_FRAME_SIZE)
   {
      btReturnT = true;
   } else
   {
      logMessage("messageSend(): WRONG number of bytes has been written");
   }

   pclSerialPortP->flush();

   return btReturnT;
}


//--------------------------------------------------------------------------------------------------------------------//
// onBytesWritten()                                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanUsart::onBytesWritten(qint64 sqByteCountV)
{
   //---------------------------------------------------------------------------------------------------
   // We could check here number of bytes that has been written against vaue that should be written.
   //
   if (sqByteCountV != 0)
   {

   }
}


//--------------------------------------------------------------------------------------------------------------------//
// onError()                                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanUsart::onError(QSerialPort::SerialPortError teErrorV)
{
   qWarning() << tr("Error on USART interface: ") << pclSerialPortP->portName();
   qWarning() << tr("             Error value: ") << QString::number(teErrorV,16)+"h";
   qWarning() << tr("            Error string: ") << pclSerialPortP->errorString();
}


//--------------------------------------------------------------------------------------------------------------------//
// onReadyRead()                                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanUsart::onReadyRead()
{
//   qDebug() << "............................onReadyRead("+QString::number(pclSerialPortP->bytesAvailable(),10)+")";

   CpCanMsg_ts tsCanMessageT;
   bool btOkV;

   //---------------------------------------------------------------------------------------------------
   // handle data only if whole frame with expected data size has been received
   //
   while (quint64(pclSerialPortP->bytesAvailable()) >= USART_FRAME_SIZE)
   {
//      logMessage("UF: "+QString::number(ulUsartFrameRcvP++,10));

      //-------------------------------------------------------------------------------------------
      // read received byte array
      //
      QByteArray clUartRcvBufT;
      clUartRcvBufT.clear();
      clUartRcvBufT.resize(USART_FRAME_SIZE);
      pclSerialPortP->read(clUartRcvBufT.data(), clUartRcvBufT.size());

      //-------------------------------------------------------------------------------------------
      // convert it to the CANpie Message
      //
      tsCanMessageT = CpCanMsgFromByteArray(clUartRcvBufT, &btOkV);

      //emit logMessage("ID: " + QString::number(tsCanMessageT.ulIdentifier,16) + "h");

      //-------------------------------------------------------------------------------------------
      // provide the message to the higher level application only if it was parsed successfully
      //
      if (btOkV == true)
      {
         emit messageReceive(tsCanMessageT);
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// release()                                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanUsart::release()
{
   QCanUsart &pclCanUsartT = QCanUsart::getInstance();

   if (pclSerialPortP != Q_NULLPTR)
   {
      pclCanUsartT.pclSerialPortP->close();
      delete(pclCanUsartT.pclSerialPortP);

      pclCanUsartT.pclSerialPortP = Q_NULLPTR;
   }
}

//--------------------------------------------------------------------------------------------------------------------//
// setCanBitrate()                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanUsart::setCanBitrate(int32_t slNomBitRateV, int32_t slDatBitRateV)
{
   CpCanMsg_ts tsCanMessageT;

   logMessage("setCanBitrate(): "+QString::number(slNomBitRateV,10) +", " + QString::number(slDatBitRateV,10));

   //---------------------------------------------------------------------------------------------------
   // pass new Bitrate configuration via RPC to the target MCU
   //
   CpMsgClear(&tsCanMessageT);
   CpMsgRpcSetBitrate(&tsCanMessageT, slNomBitRateV, slDatBitRateV);

   if (messageSend(tsCanMessageT) != true)
   {
      logMessage("setCanBitrate(): FAIL to send message!");
   }
}

//--------------------------------------------------------------------------------------------------------------------//
// setCanMode()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanUsart::setCanMode(uint8_t ubModeV)
{
   CpCanMsg_ts tsCanMessageT;

   logMessage("setCanMode(): "+QString::number(ubModeV,10));

   //---------------------------------------------------------------------------------------------------
   // pass new Bitrate configuration via RPC to the target MCU
   //
   CpMsgClear(&tsCanMessageT);
   CpMsgRpcSetCanMode(&tsCanMessageT, ubModeV);

   if (messageSend(tsCanMessageT) != true)
   {
      logMessage("setCanMode(): FAIL to send message!");
   }
}

//--------------------------------------------------------------------------------------------------------------------//
// setConfig()                                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanUsart::setConfig(QCanUsartConfig_ts & tsNewConfigR)
{
   qDebug() << "QCanUsart::setConfig()";
   bool btStatusT = false;

   //---------------------------------------------------------------------------------------------------
   // store provided configuration
   //
   tsConfigP = tsNewConfigR;

   //---------------------------------------------------------------------------------------------------
   // if USART port is already intialized, so release it
   //
   if (pclSerialPortP != Q_NULLPTR)
   {
      pclSerialPortP->disconnect();
      pclSerialPortP->close();
      delete(pclSerialPortP);
   }

   //---------------------------------------------------------------------------------------------------
   // perform new initialisation of USART Port for given device
   //
   pclSerialPortP = new QSerialPort();
   pclSerialPortP->setPortName(tsConfigP.clName);

   //---------------------------------------------------------------------------------------------------
   // setup requested USART mode
   //
   switch (tsConfigP.ubMode)
   {
   case eUSART_MODE_8N1:
      pclSerialPortP->setDataBits(QSerialPort::Data8);
      pclSerialPortP->setParity(QSerialPort::NoParity);
      pclSerialPortP->setStopBits(QSerialPort::OneStop);
      pclSerialPortP->setFlowControl(QSerialPort::NoFlowControl);
      break;
   case eUSART_MODE_8E1:
      pclSerialPortP->setDataBits(QSerialPort::Data8);
      pclSerialPortP->setParity(QSerialPort::EvenParity);
      pclSerialPortP->setStopBits(QSerialPort::OneStop);
      pclSerialPortP->setFlowControl(QSerialPort::NoFlowControl);
      break;
   case eUSART_MODE_8O1:
      pclSerialPortP->setDataBits(QSerialPort::Data8);
      pclSerialPortP->setParity(QSerialPort::OddParity);
      pclSerialPortP->setStopBits(QSerialPort::OneStop);
      pclSerialPortP->setFlowControl(QSerialPort::NoFlowControl);
      break;
   default:
      tsConfigP.ubMode = eUSART_MODE_8E1;
      pclSerialPortP->setDataBits(QSerialPort::Data8);
      pclSerialPortP->setParity(QSerialPort::EvenParity);
      pclSerialPortP->setStopBits(QSerialPort::OneStop);
      pclSerialPortP->setFlowControl(QSerialPort::NoFlowControl);
      break;
   }

   pclSerialPortP->setBaudRate(tsConfigP.slBaud, QSerialPort::AllDirections);

   //---------------------------------------------------------------------------------------------------
   // now open COM port with given configuration
   //
   if (pclSerialPortP->open(QIODevice::ReadWrite))
   {
      qDebug() << "   SUCCESS!!!";
      qDebug() << "Serial COM Port: " + pclSerialPortP->portName() + " opened!";

      //-------------------------------------------------------------------------------------------
      // reset reception buffer of the USART
      //
      QByteArray  clUartRcvBufT;
      while (pclSerialPortP->bytesAvailable() > 0)
      {
         clUartRcvBufT.clear();
         clUartRcvBufT.resize(qint32(pclSerialPortP->bytesAvailable()));
         pclSerialPortP->read(&(clUartRcvBufT.data()[0]), pclSerialPortP->bytesAvailable());
      }
      clUartRcvBufT.clear();

      pclSerialPortP->flush();

      //-------------------------------------------------------------------------------------------
      // (re)initialise serial interface signal slot connections
      //
      pclSerialPortP->connect(pclSerialPortP, SIGNAL(error(QSerialPort::SerialPortError)),
                              this,           SLOT(onError(QSerialPort::SerialPortError)));

      pclSerialPortP->connect(pclSerialPortP, SIGNAL(readyRead()),
                              this,           SLOT(  onReadyRead()));

      pclSerialPortP->connect(pclSerialPortP, SIGNAL(bytesWritten(qint64)),
                              this,           SLOT(  onBytesWritten(qint64)));

      btStatusT = true;
   }

   //---------------------------------------------------------------------------------------------------
   // handle the open error
   //
   else
   {
      qWarning() << tr("Fail to open USART interface: ") << pclSerialPortP->portName();
      qWarning() << tr("                 Error value: ") << pclSerialPortP->error();
      qWarning() << tr("                Error string: ") << pclSerialPortP->errorString();
   }

   return btStatusT;
}

//--------------------------------------------------------------------------------------------------------------------//
// setDeviceName()                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanUsart::setDeviceName(QString clNameV)
{
   tsConfigP.clName = clNameV;
}


















