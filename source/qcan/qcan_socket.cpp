//====================================================================================================================//
// File:          qcan_socket.cpp                                                                                     //
// Description:   CAN socket class                                                                                    //
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

#include "qcan_socket.hpp"


#include <QtCore/QDebug>

#include <QtNetwork/QNetworkInterface>

/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*\
** Class methods                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanSocket::QCanSocket(QObject * pclParentV)
{
   if (pclParentV != Q_NULLPTR)
   {
      this->setParent(pclParentV);
   }

   //---------------------------------------------------------------------------------------------------
   // set default values for host address and port
   //
   clServerHostAddrP = QHostAddress(QHostAddress::LocalHost);
   uwServerPortP     = QCAN_WEB_SOCKET_DEFAULT_PORT;

   //---------------------------------------------------------------------------------------------------
   // create a local and a WebSocket by default, the selection which one is used will be done in 
   // setHostAddress().
   //
   pclLocalSocketP = new QLocalSocket(this);
   pclWebSocketP   = new QWebSocket("", QWebSocketProtocol::VersionLatest, this);

   //---------------------------------------------------------------------------------------------------
   // the socket is not connected yet and the default connection method is "local"
   //
   btIsLocalConnectionP = true;
   btIsConnectedP       = false;

   //---------------------------------------------------------------------------------------------------
   // No socket errors available yet
   //
   slSocketErrorP = 0;

   clUuidP = QUuid::createUuid();

   teCanStateP = eCAN_STATE_BUS_ACTIVE;

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket()                                                                                                       //
// destructor                                                                                                         //
//--------------------------------------------------------------------------------------------------------------------//
QCanSocket::~QCanSocket()
{
   delete (pclLocalSocketP);
   delete (pclWebSocketP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket::connectNetwork()                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanSocket::connectNetwork(const CAN_Channel_e & teChannelR)
{
   bool btResultT = false;

   //---------------------------------------------------------------------------------------------------
   // create a new socket
   //
   if (btIsConnectedP == false)
   {

      if (btIsLocalConnectionP == false)
      {
         qDebug() << "QCanSocket::connectNetwork(" << teChannelR << "- WebSocket";

         //----------------------------------------------------------------------------------------
         // create new WebSocket
         //
         pclWebSocketP->abort();
         QString clSocketUrlT = QString("ws://") + clServerHostAddrP.toString() + QString(":%1").arg(uwServerPortP);
         clSocketUrlT += QString("/%1").arg(teChannelR);
         qDebug() << "QCanSocket::connectNetwork(" << teChannelR << ") -" << clSocketUrlT;
         pclWebSocketP->open(clSocketUrlT);

         //----------------------------------------------------------------------------------------
         // make signal / slot connection for WebSocket
         //
         connect( pclWebSocketP, &QWebSocket::connected,             this, &QCanSocket::onSocketConnect);

         connect( pclWebSocketP, &QWebSocket::disconnected,          this, &QCanSocket::onSocketDisconnect);

         connect( pclWebSocketP, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
                  this, &QCanSocket::onSocketErrorWeb);

         connect( pclWebSocketP, &QWebSocket::binaryMessageReceived, this, &QCanSocket::onSocketReceiveWeb);

         btResultT = true;
      }
      else
      {
         qDebug() << "QCanSocket::connectNetwork(" << teChannelR << ") - LocalSocket";

         //----------------------------------------------------------------------------------------
         // create new local socket
         //
         pclLocalSocketP->abort();

         //----------------------------------------------------------------------------------------
         // make signal / slot connection for local socket
         //
         connect( pclLocalSocketP, &QLocalSocket::connected,      this, &QCanSocket::onSocketConnect);

         connect( pclLocalSocketP, &QLocalSocket::disconnected,   this, &QCanSocket::onSocketDisconnect);

         connect( pclLocalSocketP, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error), 
                  this, &QCanSocket::onSocketErrorLocal);

         connect( pclLocalSocketP, &QLocalSocket::readyRead,      this, &QCanSocket::onSocketReceiveLocal);

         //----------------------------------------------------------------------------------------
         // connect to local server
         //
         pclLocalSocketP->connectToServer(QString("CANpieServerChannel%1").arg(teChannelR));
         qDebug() << "QCanSocket::connectNetwork() -" << pclLocalSocketP->fullServerName();

         btResultT = true;
      }
   }

   return(btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket::disconnectNetwork()                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocket::disconnectNetwork(void)
{
   qDebug() << "QCanSocket::disconnectNetwork() ";

   if (btIsLocalConnectionP == false)
   {
      pclWebSocketP->disconnect();
   }
   else
   {
      disconnect(pclLocalSocketP, 0, 0, 0);
      pclLocalSocketP->disconnectFromServer();
   }

   btIsConnectedP = false;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket::error()                                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QAbstractSocket::SocketError QCanSocket::error() const
{
   return((QAbstractSocket::SocketError) slSocketErrorP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket::errorString()                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanSocket::errorString() const
{
   QString  clErrorT;

   switch (slSocketErrorP)
   {
      case QAbstractSocket::ConnectionRefusedError:
         clErrorT = qPrintable(tr("connection refused or timed out.")                           );
         break;

      case QAbstractSocket::RemoteHostClosedError:
         clErrorT = qPrintable(tr("server closed the connection.")                              );
         break;

      case QAbstractSocket::HostNotFoundError:
         clErrorT = qPrintable(tr("server address was not found.")                              );
         break;

      case QAbstractSocket::SocketAccessError:
         clErrorT = qPrintable(tr("application lacked the required privileges.")                );
         break;

      case QAbstractSocket::SocketResourceError:
         clErrorT = qPrintable(tr("local system ran out of resources.")                         );
         break;

      case QAbstractSocket::SocketTimeoutError:
         clErrorT = qPrintable(tr("operation timed out.")                                       );
         break;

      case QAbstractSocket::NetworkError:
         clErrorT = qPrintable(tr("error occurred with the network (e.g. cable plugged out.")   );
         break;

      case QAbstractSocket::AddressInUseError:
         clErrorT = qPrintable(tr("address is already in use.")                                 );
         break;

      default:
         clErrorT = qPrintable(tr("unknown reason")                                             );
         break;
   }

   return (clErrorT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket::framesAvailable()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanSocket::framesAvailable(void) const
{
   return (clReceiveFifoP.size());
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket::onSocketConnect()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocket::onSocketConnect(void)
{
   qDebug() << "QCanSocket::onSocketConnect() ";

   //---------------------------------------------------------------------------------------------------
   // send signal about connection state and keep it in local variable
   //
   btIsConnectedP = true;
   emit connected();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket::onSocketDisconnect()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocket::onSocketDisconnect(void)
{
   qDebug() << "QCanSocket::onSocketDisconnect() ";

   //---------------------------------------------------------------------------------------------------
   // send signal about connection state and keep it in local variable
   //
   btIsConnectedP = false;
   emit disconnected();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket::onSocketErrorLocal()                                                                                   //
// handle error conditions of local socket                                                                            //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocket::onSocketErrorLocal(QLocalSocket::LocalSocketError teSocketErrorV)
{

   qDebug() << "QCanSocket::onSocketErrorLocal() " << teSocketErrorV;
   qDebug() << pclLocalSocketP->errorString();

   switch(teSocketErrorV)
   {
      //-------------------------------------------------------------------------------------------
      // abort all operations and disconnect socket
      //
      case QLocalSocket::PeerClosedError:
      case QLocalSocket::ConnectionError:
         pclLocalSocketP->abort();
         btIsConnectedP = false;
         emit disconnected();
         break;

      default:

         break;

   }

   //---------------------------------------------------------------------------------------------------
   // Store socket error and send signal:
   // Since the enumeration values for LocalSocketError are derived from QAbstractSocket::SocketError 
   // inside the header file QtNetwork/qlocalsocket.h we can simply cast it.
   //
   slSocketErrorP = teSocketErrorV;
   emit error( (QAbstractSocket::SocketError) teSocketErrorV);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket::onSocketErrorWeb()                                                                                     //
//  handle error conditions of WebSocket                                                                              //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocket::onSocketErrorWeb(QAbstractSocket::SocketError teSocketErrorV)
{
   qDebug() << "QCanSocket::onSocketErrorWeb()   -" << teSocketErrorV;

   switch(teSocketErrorV)
   {
      //-------------------------------------------------------------
      // abort all operations and disconnect socket
      //
      case QAbstractSocket::RemoteHostClosedError:
      case QAbstractSocket::NetworkError:
         pclWebSocketP->abort();
         btIsConnectedP = false;
         emit disconnected();
         break;

      default:

         break;

   }

   //----------------------------------------------------------------
   // store socket error and send signal
   //
   slSocketErrorP = teSocketErrorV;
   emit error(teSocketErrorV);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket::onSocketReceiveLocal()                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocket::onSocketReceiveLocal(void)
{
   uint32_t    ulFrameCountT;
   bool        btValidFrameT     = false;
   bool        btSignalNewFrameT = false;

   ulFrameCountT = pclLocalSocketP->bytesAvailable() / QCAN_FRAME_ARRAY_SIZE;
   while (ulFrameCountT)
   {
      clReceiveDataP = pclLocalSocketP->read(QCAN_FRAME_ARRAY_SIZE);
      btValidFrameT  = clReceiveFrameP.fromByteArray(clReceiveDataP);
      if (btValidFrameT)
      {
         //-----------------------------------------------------------------------------------
         // Store frame in FIFO
         //
         clReceiveMutexP.lock();
         clReceiveFifoP.enqueue(clReceiveFrameP);
         btSignalNewFrameT = true;
         clReceiveMutexP.unlock();

         //-----------------------------------------------------------------------------------
         // If the frame type is an error frame, store the for the actual CAN state
         //
         if (clReceiveFrameP.frameType() == QCanFrame::eFRAME_TYPE_ERROR)
         {
            teCanStateP = clReceiveFrameP.errorState();
         }
      }

      ulFrameCountT--;
   }


   //---------------------------------------------------------------------------------------------------
   // Emit signal only once when new data arrived. The flag 'btReceiveSignalSendP' is cleared inside
   // the read() method.
   //
   if (btSignalNewFrameT == true)
   {
      emit readyRead();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket::onSocketReceiveWeb()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocket::onSocketReceiveWeb(const QByteArray &clMessageR)
{
   bool        btValidFrameT     = false;
   bool        btSignalNewFrameT = false;

   btValidFrameT  = clReceiveFrameP.fromByteArray(clMessageR);
   if (btValidFrameT)
   {
      //-----------------------------------------------------------------------------------
      // Store frame in FIFO
      //
      clReceiveMutexP.lock();
      clReceiveFifoP.enqueue(clReceiveFrameP);
      btSignalNewFrameT = true;
      clReceiveMutexP.unlock();

      //-----------------------------------------------------------------------------------
      // If the frame type is an error frame, store the for the actual CAN state
      //
      if (clReceiveFrameP.frameType() == QCanFrame::eFRAME_TYPE_ERROR)
      {
         teCanStateP = clReceiveFrameP.errorState();
      }
   }

   //---------------------------------------------------------------------------------------------------
   // Emit signal only once when new data arrived. The flag 'btReceiveSignalSendP' is cleared inside
   // the read() method.
   //
   if (btSignalNewFrameT == true) 
   {
      emit readyRead();
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket::read()                                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanSocket::read(QCanFrame & clFrameR)
{
   bool btResultT = false;

   clReceiveMutexP.lock();
   if (clReceiveFifoP.size() > 0)
   {
      clFrameR = clReceiveFifoP.dequeue();         // read message from queue
      btResultT = true;                            // message has been updated
   }
   clReceiveMutexP.unlock();

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket::setHostAddress()                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocket::setHostAddress(QHostAddress clHostAddressV, uint16_t uwPortV)
{
   if(btIsConnectedP == false)
   {
      clServerHostAddrP = clHostAddressV;
      uwServerPortP     = uwPortV;

      if (clHostAddressV == QHostAddress::LocalHost)
      {
         btIsLocalConnectionP = true;
      }
      else
      {
         btIsLocalConnectionP = false;
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocket::write()                                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanSocket::write(const QCanFrame & clFrameR)
{
   bool  btResultT = false;

   if (btIsConnectedP == true)
   {
      QByteArray  clDatagramT = clFrameR.toByteArray();

      if (btIsLocalConnectionP == false)
      {
         if (pclWebSocketP->sendBinaryMessage(clDatagramT) == QCAN_FRAME_ARRAY_SIZE)
         {
            pclWebSocketP->flush();
            btResultT = true;
         }
      }
      else
      {
         if (pclLocalSocketP->write(clDatagramT) == QCAN_FRAME_ARRAY_SIZE)
         {
            pclLocalSocketP->flush();
            btResultT = true;
         }
      }
   }

   return(btResultT);
}


