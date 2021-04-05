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
QCanUsart::QCanUsart(QObject *parent)
   : QThread(parent)
{
   //---------------------------------------------------------------------------------------------------
   // setup default USART configuration
   //
   pclSerialPortP = Q_NULLPTR;
   ulUsartFrameRcvP = 0;
   clUsartFrameRcvBufferT.clear();
   clUsartFrameRcvBufferT.resize(USART_FRAME_SIZE);

   aclUsartWriteFramesP.clear();

   //---------------------------------------------------------------------------------------------------
   // setup default USART configuration, that can be ovewritten by the loaded or user configuration
   //
   tsConfigP.clName = "COM1";
//   tsConfigP.slBaud = 115200;
   tsConfigP.slBaud = 3456000;
   tsConfigP.ubMode = eUSART_MODE_8N1;

}

//--------------------------------------------------------------------------------------------------------------------//
// ~QCanUsart()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanUsart::~QCanUsart()
{
   qDebug() << "QCanUsart::~QCanUsart()";

   emit logMessage("~QCanUsart()");

}


//--------------------------------------------------------------------------------------------------------------------//
// connect()                                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanUsart::connect()
{
   //---------------------------------------------------------------------------------------------------
   // at a reconnect release the interface, before performing a new connection
   //
   if (isRunning())
   {
      qDebug() << "Process is runnning release it ...";

      release();
      quit();
      while (isFinished() == true) {};

      qDebug() << ".... release finished!";
   }

   //---------------------------------------------------------------------------------------------------
   // provide new configuration for USART interface
   //
   setConfig(tsConfigP);

   //---------------------------------------------------------------------------------------------------
   // start the thread
   //
   if (!isRunning())
   {
      btQuitThreadP = false;
      start();
   }
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
   QString     clDebugValueT;

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

            emit logMessage("UF1: WRONG CRC: ID: " + QString::number(tsCanMessageT.ulIdentifier,16) + "h");

            for (int slCounterT = 0; slCounterT < clUsartFrameRcvBufferT.length(); slCounterT++)
            {
               clDebugValueT.append(QString::number(clUsartFrameRcvBufferT.at(slCounterT), 16) + " ");
            }
            emit logMessage(clDebugValueT);

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

   return clArrayT;
}


//--------------------------------------------------------------------------------------------------------------------//
// currentConfig()                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanUsart::QCanUsartConfig_ts QCanUsart::currentConfig()
{
   QCanUsartConfig_ts tsConfigurationT;
   tsConfigurationT = tsConfigP;
   return tsConfigurationT;
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
   bool btReturnT = true;

   //---------------------------------------------------------------------------------------------------
   // append new message so it is passed to the target MCU later int the thread
   //
   aclUsartWriteFramesP.append(CpCanMsgToByteArray(tsCanMessageV));

   return btReturnT;
}

//--------------------------------------------------------------------------------------------------------------------//
// release()                                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanUsart::release()
{
   emit logMessage("QCanUsart::release() start");

   //---------------------------------------------------------------------------------------------------
   // now relese the USART interface by quitting the thread
   //
   btQuitThreadP = true;
   quit();

   emit logMessage("QCanUsart::release() stop");
}

//--------------------------------------------------------------------------------------------------------------------//
// setCanBitrate()                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanUsart::setCanBitrate(int32_t slNomBitRateV, int32_t slDatBitRateV)
{
   CpCanMsg_ts tsCanMessageT;

   //---------------------------------------------------------------------------------------------------
   // append new configuration frame so the configuration is passed via RPC to the target MCU
   //
   CpMsgClear(&tsCanMessageT);
   CpMsgRpcSetBitrate(&tsCanMessageT, slNomBitRateV, slDatBitRateV);
   aclUsartWriteConfigFramesP.append(CpCanMsgToByteArray(tsCanMessageT));

   emit logMessage("setCanBitrate(): "+QString::number(slNomBitRateV,10) +", " + QString::number(slDatBitRateV,10));
}

//--------------------------------------------------------------------------------------------------------------------//
// setCanMode()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanUsart::setCanMode(uint8_t ubModeV)
{
   CpCanMsg_ts tsCanMessageT;

   //---------------------------------------------------------------------------------------------------
   // append new configuration frame so the configuration is passed via RPC to the target MCU
   //
   CpMsgClear(&tsCanMessageT);
   CpMsgRpcSetCanMode(&tsCanMessageT, ubModeV);
   aclUsartWriteConfigFramesP.append(CpCanMsgToByteArray(tsCanMessageT));

   emit logMessage("setCanMode(): "+QString::number(ubModeV,10));
}

//--------------------------------------------------------------------------------------------------------------------//
// setConfig()                                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanUsart::setConfig(QCanUsartConfig_ts & tsNewConfigR)
{
   bool btStatusT = true;

   //---------------------------------------------------------------------------------------------------
   // store provided configuration
   //
   tsConfigP = tsNewConfigR;

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


//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanUsart::run()
{
   QTimer     clTimerT;
   QEventLoop clDelayT;
   bool btOkV;
   CpCanMsg_ts tsCanMessageT;
   QString clMessageT;


   qDebug() << "QCanUsart::run() >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ";

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

   //-------------------------------------------------------------------------------------------
   // now open COM port with given configuration
   //
   if (pclSerialPortP->open(QIODevice::ReadWrite))
   {
      qDebug() << "QCanUsart::setConfig() +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ ";
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
      emit logMessage("Successfully open " + pclSerialPortP->portName());
   }

   //-------------------------------------------------------------------------------------------
   // handle the open error
   //
   else
   {
      clMessageT.clear();
      clMessageT.append(tr("Error on USART interface: ") + pclSerialPortP->portName());
      clMessageT.append(tr("             Error value: ") + QString::number(pclSerialPortP->error(), 16) + "h");
      clMessageT.append(tr("            Error string: ") + pclSerialPortP->errorString());

      emit logMessage(clMessageT);
   }

   //---------------------------------------------------------------------------------------------------
   // make initialisation for a non blocking delay in the thread
   //
   clTimerT.setInterval(1);
   clTimerT.connect(&clTimerT, &QTimer::timeout, &clDelayT, &QEventLoop::quit);
   clTimerT.start();

   qDebug() << "QCanUsart::run() --------------------------------------------------------------- ";

   //---------------------------------------------------------------------------------------------------
   // perform processing of receive data, while
   // - thread is running
   //          or
   // - some configuration data for USART transmission is pending
   //
   while ((btQuitThreadP == false) || (aclUsartWriteConfigFramesP.isEmpty() == false))
   {
      //-------------------------------------------------------------------------------------------
      // perform a non blocking delay, so event loop in background is also processed
      //
      clDelayT.exec();

      //-------------------------------------------------------------------------------------------
      // process received USART data
      //
      while (pclSerialPortP->bytesAvailable() >= USART_FRAME_SIZE)
      {
         ulUsartFrameRcvP++;
         //emit logMessage("UF: "+QString::number(ulUsartFrameRcvP,10));

         //-------------------------------------------------------------------------------------------
         // read received byte array
         //
         pclSerialPortP->read(clUsartFrameRcvBufferT.data(), clUsartFrameRcvBufferT.size());

         //-------------------------------------------------------------------------------------------
         // convert it to the CANpie Message
         //
         tsCanMessageT = CpCanMsgFromByteArray(clUsartFrameRcvBufferT, &btOkV);

         //-------------------------------------------------------------------------------------------
         // provide the message to the higher level application only if it was parsed successfully
         //
         if (btOkV == true)
         {
            emit messageReceive(tsCanMessageT);
         }
      }

      //-------------------------------------------------------------------------------------------
      // Write CANpie Configuration messages to USART, if some pending
      //
      if ((aclUsartWriteConfigFramesP.isEmpty() == false) && (pclSerialPortP->bytesToWrite() == 0))
      {
         if (pclSerialPortP->write(aclUsartWriteConfigFramesP.first()) == USART_FRAME_SIZE)
         {
            btUsartIsWritingP = true;
         }
         pclSerialPortP->flush();
         aclUsartWriteConfigFramesP.removeFirst();
      }

      //-------------------------------------------------------------------------------------------
      // Write CANpie messages to USART, if some pending
      //
      if ((aclUsartWriteFramesP.isEmpty() == false) && (pclSerialPortP->bytesToWrite() == 0))
      {
         qDebug() << "USART Frames to write: " << QString::number(aclUsartWriteFramesP.size(),10);
         if (pclSerialPortP->write(aclUsartWriteFramesP.first()) == USART_FRAME_SIZE)
         {
            qDebug() << "QCanUsart::run() USART write OK";
            btUsartIsWritingP = true;
         }
         pclSerialPortP->flush();
         aclUsartWriteFramesP.removeFirst();
      }

   }

   //---------------------------------------------------------------------------------------------------
   // insert here a delay of 10ms to ensure the whole USART frame is transmitted, befor closing that
   // interface
   //
   clTimerT.stop();
   clTimerT.singleShot(10, &clDelayT, &QEventLoop::quit);
   clDelayT.exec();

   //---------------------------------------------------------------------------------------------------
   // reset USART write buffer
   //
   aclUsartWriteConfigFramesP.clear();
   aclUsartWriteFramesP.clear();
   btUsartIsWritingP = false;

   //---------------------------------------------------------------------------------------------------
   // release serial port
   //
   if (pclSerialPortP != Q_NULLPTR)
   {
      pclSerialPortP->disconnect();
      pclSerialPortP->close();
      delete(pclSerialPortP);

      pclSerialPortP = Q_NULLPTR;
   }


   qDebug() << "QCanUsart::run() <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  ";
}














