//============================================================================//
// File:          qcan_usart.cpp                                              //
// Description:   QCAN USART library handling                                 //
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

#include "qcan_usart.hpp"


/*----------------------------------------------------------------------------*\
** handle functins from cp_usart module                                       **
**                                                                            **
\*----------------------------------------------------------------------------*/
CpPort_ts   tsCpPortG;
QByteArray  clUartRcvBufG;
QByteArray  clUartTrmBufG;

//----------------------------------------------------------------------------//
// McUsartInit()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t McUsartInit(uint8_t ubPortV, int32_t slBaudV, uint8_t ubModeV)
{
   Q_UNUSED(ubPortV);

   QCanUsart &pclCanUsartT = QCanUsart::getInstance();
   return pclCanUsartT.DevInit(slBaudV,ubModeV);
}

//----------------------------------------------------------------------------//
// McUsartSetRcvBufferSize()                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t McUsartSetRcvBufferSize(uint8_t ubPortV, uint32_t ulSizeV, uint8_t *pubBufferV)
{
   Q_UNUSED(ubPortV);
   Q_UNUSED(pubBufferV);

   QCanUsart &pclCanUsartT = QCanUsart::getInstance();
   return pclCanUsartT.DevSetRcvBufferSize(ulSizeV);
}

//----------------------------------------------------------------------------//
// McUsartSetDir()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t McUsartSetDir(uint8_t ubPortV, uint8_t ubDirV)
{
   Q_UNUSED(ubPortV);

   QCanUsart &pclCanUsartT = QCanUsart::getInstance();
   return pclCanUsartT.DevSetDir(ubDirV);
}

//----------------------------------------------------------------------------//
// McUsartRelease()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t McUsartRelease(uint8_t ubPortV)
{
   Q_UNUSED(ubPortV);

   QCanUsart &pclCanUsartT = QCanUsart::getInstance();
   return pclCanUsartT.DevRelease();
}

//----------------------------------------------------------------------------//
// McUsartWrite()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t McUsartWrite(uint8_t ubPortV, uint8_t *pubBufferV, uint32_t ulSizeV)
{
   Q_UNUSED(ubPortV);

   // transmit data to a usart Port
   QCanUsart &pclCanUsartT = QCanUsart::getInstance();
   return pclCanUsartT.DevWrite(pubBufferV,ulSizeV);
}

//----------------------------------------------------------------------------//
// McUsartSetRcvHandler()                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t McUsartSetRcvHandler(uint8_t ubPortV, UsartHandler_fn pfnHandler)
{
   Q_UNUSED(ubPortV);

   QCanUsart &pclCanUsartT = QCanUsart::getInstance();
   return pclCanUsartT.DevSetRcvHandler(pfnHandler);
}

//----------------------------------------------------------------------------//
// McUsartSetTrmHandler()                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t McUsartSetTrmHandler(uint8_t ubPortV, UsartHandler_fn pfnHandler)
{
   Q_UNUSED(ubPortV);

   QCanUsart &pclCanUsartT = QCanUsart::getInstance();
   return pclCanUsartT.DevSetTrmHandler(pfnHandler);
}

//----------------------------------------------------------------------------//
// currentConfig()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
QCanUsart::QCanUsartConfig_ts & QCanUsart::currentConfig()
{
   return tsConfigP;
}

//----------------------------------------------------------------------------//
// setConfig()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanUsart::setConfig(QCanUsartConfig_ts & tsNewConfigR)
{
   qDebug() << "QCanUsart::setConfig()";
   bool btStatusT = false;

   tsConfigP = tsNewConfigR;

   //----------------------------------------------------------------
   if (pclSerialPortP != NULL)
   {
      pclSerialPortP->disconnect();
      pclSerialPortP->close();
      delete(pclSerialPortP);
   }

   pclSerialPortP = new QSerialPort();
   pclSerialPortP->setPortName(tsConfigP.clName);

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

   switch (tsConfigP.ubDirection)
   {
   case eUSART_DIR_NONE:
      pclSerialPortP->setBaudRate(tsConfigP.slBaud,QSerialPort::AllDirections);
      break;
   case eUSART_DIR_RX:
      pclSerialPortP->setBaudRate(tsConfigP.slBaud,QSerialPort::Input);
      break;
   case eUSART_DIR_TX:
      pclSerialPortP->setBaudRate(tsConfigP.slBaud,QSerialPort::Output);
      break;
   case eUSART_DIR_RXTX:
      pclSerialPortP->setBaudRate(tsConfigP.slBaud,QSerialPort::AllDirections);
      break;
   case eUSART_DIR_RS485:
      //! todo
      break;
   default:
      tsConfigP.ubDirection = eUSART_DIR_RXTX;
      pclSerialPortP->setBaudRate(tsConfigP.slBaud,QSerialPort::AllDirections);
      break;
   }

   //----------------------------------------------------------------
   // now open COM port with given configuration
   //
   if (pclSerialPortP->open(QIODevice::ReadWrite))
   {
      qDebug() << "   SUCCESS!!!";
      qDebug() << "Serial COM Port: " + pclSerialPortP->portName() + " opened!";


      //------------------------------------------------------------------
      // reset buffer of the UART
      //
      while (pclSerialPortP->bytesAvailable() > 0)
      {
         clUartRcvBufG.clear();
         clUartRcvBufG.resize(pclSerialPortP->bytesAvailable());
         pclSerialPortP->read(&(clUartRcvBufG.data()[0]), pclSerialPortP->bytesAvailable());
      }

      clUartRcvBufG.clear();
      pclSerialPortP->flush();

      clUartTrmBufG.clear();

      //------------------------------------------------------------------
      // (re)initialise serial interface
      //
      pclSerialPortP->connect(pclSerialPortP, SIGNAL(error(QSerialPort::SerialPortError)),
                              this,           SLOT(onError(QSerialPort::SerialPortError)));

      pclSerialPortP->connect(pclSerialPortP, SIGNAL(readyRead()),
                              this,           SLOT(  onReadyRead()));

      pclSerialPortP->connect(pclSerialPortP, SIGNAL(bytesWritten(qint64)),
                              this,           SLOT(  onBytesWritten(qint64)));

      btStatusT = true;
   }

   //----------------------------------------------------------------
   // handle the open error
   //
   else
   {
      qWarning() << tr("Fail to open Interface: ") << pclSerialPortP->portName();
   }

   return btStatusT;
}

//----------------------------------------------------------------------------//
// setDeviceName()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanUsart::setDeviceName(QString clNameV)
{
   tsConfigP.clName = clNameV;
}

//----------------------------------------------------------------------------//
// DevInit()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
qint32 QCanUsart::DevInit(qint32 slBaudV, quint8 ubModeV)
{
   Q_UNUSED(ubModeV);

   tsConfigP.slBaud = slBaudV;
   tsConfigP.ubDirection = eUSART_DIR_RXTX;
   if (setConfig(tsConfigP) == true)
   {
      return eUSART_ERROR_OK;
   }
   return eUSART_ERROR_MODE;
}


//----------------------------------------------------------------------------//
// DevRelease()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
qint32 QCanUsart::DevRelease()
{
   QCanUsart &pclCanUsartT = QCanUsart::getInstance();


   if (pclSerialPortP != NULL)
   {
      pclCanUsartT.pclSerialPortP->close();
      delete(pclCanUsartT.pclSerialPortP);

      pclCanUsartT.pclSerialPortP = NULL;
   }

   return eUSART_ERROR_OK;
}

//----------------------------------------------------------------------------//
// DevSetDir()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
qint32 QCanUsart::DevSetDir(quint8 ubDirV)
{
   ubUsartDirectionP = ubDirV;
   return eUSART_ERROR_OK;
}

//----------------------------------------------------------------------------//
// DevSetRcvBufferSize()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
qint32 QCanUsart::DevSetRcvBufferSize(quint32 ulSizeV)
{
   ulUsartRcvBufferSizeP = ulSizeV;
   return eUSART_ERROR_OK;
}

//----------------------------------------------------------------------------//
// DevSetRcvHandler()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
qint32 QCanUsart::DevSetRcvHandler(UsartHandler_fn pfnHandler)
{
   pfnUsartRcvHandlerP = pfnHandler;
   return eUSART_ERROR_OK;
}

//----------------------------------------------------------------------------//
// DevSetTrmHandler()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
qint32 QCanUsart::DevSetTrmHandler(UsartHandler_fn pfnHandler)
{
   pfnUsartTrmHandlerP = pfnHandler;
   return eUSART_ERROR_OK;
}

//----------------------------------------------------------------------------//
// DevWrite()                                                                 //
//                                                                            //
//----------------------------------------------------------------------------//
qint32 QCanUsart::DevWrite(quint8 *pubDataV, quint32 ulSizeV)
{
   clUartTrmBufG.clear();
   clUartTrmBufG.append((const char *)pubDataV,ulSizeV);
   ulUsartTrmBufferSizeP = ulSizeV;
   return pclSerialPortP->write(clUartTrmBufG);
}

//----------------------------------------------------------------------------//
// onBytesWritten()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanUsart::onBytesWritten(qint64 sqByteCountV)
{
//   qDebug() << "............................onBytesWritten("+QString::number(sqByteCountV,10)+")";
   if (sqByteCountV != ulUsartTrmBufferSizeP)
   {
      // something goes wrong....
   }
   pfnUsartTrmHandlerP((uint8_t *)clUartTrmBufG.data(),ulUsartTrmBufferSizeP);
}

//----------------------------------------------------------------------------//
// onError()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanUsart::onError(QSerialPort::SerialPortError teErrorV)
{
   qDebug() << "............................onError("+QString::number(teErrorV,16)+"h)";
}

//----------------------------------------------------------------------------//
// onReadyRead()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanUsart::onReadyRead()
{
   qDebug() << "............................onReadyRead("+QString::number(pclSerialPortP->bytesAvailable(),10)+")";
   if (pclSerialPortP->bytesAvailable() >= ulUsartRcvBufferSizeP)
   {
      //--------------------------------------------------------
      // pass received USART Data to the CANpie Receive Handler
      //
      clUartRcvBufG.clear();
      clUartRcvBufG.resize(ulUsartRcvBufferSizeP);

      pclSerialPortP->read(clUartRcvBufG.data(), ulUsartRcvBufferSizeP);

      pfnUsartRcvHandlerP((uint8_t *)clUartRcvBufG.data(),ulUsartRcvBufferSizeP);
   }
}

//----------------------------------------------------------------------------//
// QCanUsart()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
QCanUsart::QCanUsart()
{
   btLibFuncLoadP = true;
   pclSerialPortP = NULL;
}

//----------------------------------------------------------------------------//
// ~QCanUsart()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
QCanUsart::~QCanUsart()
{
   qDebug() << "QCanUsart::~QCanUsart()";
}

//----------------------------------------------------------------------------//
// isAvailable()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanUsart::isAvailable()
{
   return btLibFuncLoadP;

}

//----------------------------------------------------------------------------//
// formatedError()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
QString QCanUsart::formatedError(CpStatus_tv tvErrorV)
{
   QString clResultT;

   clResultT = ("An error ocurred. Error-code's text ("+ QString::number(tvErrorV,16).toUpper() + "h) couldn't be retrieved");

   return clResultT;
}

