//============================================================================//
// File:          qcan_network.cpp                                            //
// Description:   QCAN classes - CAN network                                  //
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


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "qcan_socket.hpp"


#include <QtCore/QDebug>

#include <QtNetwork/QNetworkInterface>

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanSocket()                                                               //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanSocket::QCanSocket(QObject * pclParentV)
{
   if (pclParentV != Q_NULLPTR)
   {
      this->setParent(pclParentV);
   }

   //----------------------------------------------------------------
   // set default values for host address and port
   //
   clTcpHostAddrP = QHostAddress(QHostAddress::LocalHost);
   uwTcpPortP     = QCAN_TCP_DEFAULT_PORT;

   //----------------------------------------------------------------
   // create a local and a TCP socket by default, the selection
   // which one is used will be done in setHostAddress().
   //
   pclLocalSockP = new QLocalSocket(this);
   pclTcpSockP   = new QTcpSocket(this);

   //----------------------------------------------------------------
   // the socket is not connected yet and the default connection
   // method is "local"
   //
   btIsLocalConnectionP = true;
   btIsConnectedP       = false;

   //----------------------------------------------------------------
   // No socket errors available yet
   //
   slSocketErrorP = 0;

}


//----------------------------------------------------------------------------//
// QCanSocket()                                                               //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanSocket::~QCanSocket()
{
   delete (pclLocalSockP);
   delete (pclTcpSockP);
}


//----------------------------------------------------------------------------//
// connectNetwork()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanSocket::connectNetwork(CAN_Channel_e ubChannelV, 
                                const int32_t slMilliSecsV)
{
   bool btResultT = false;

   //---------------------------------------------------------------------
   // create a new socket
   //
   if (btIsConnectedP == false)
   {

      if (btIsLocalConnectionP == false)
      {
         qDebug() << "QCanSocket::connectNetwork(" << ubChannelV << "," << slMilliSecsV << ") - TCP";

         //----------------------------------------------------------
         // create new TCP socket
         //
         pclTcpSockP->abort();
         pclTcpSockP->connectToHost(clTcpHostAddrP, uwTcpPortP + ubChannelV - 1);

         //----------------------------------------------------------
         // make signal / slot connection for TCP socket
         //
         connect( pclTcpSockP, SIGNAL(connected()),
                  this, SLOT(onSocketConnect()));

         connect( pclTcpSockP, SIGNAL(disconnected()),
                  this, SLOT(onSocketDisconnect()));

         connect( pclTcpSockP, SIGNAL(error(QAbstractSocket::SocketError)),
                  this, SLOT(onSocketErrorTcp(QAbstractSocket::SocketError)));

         connect( pclTcpSockP, SIGNAL(readyRead()),
                  this, SLOT(onSocketReceive()));

         //----------------------------------------------------------
         // if the parameter slMilliSecsV is 0 we do not wait for a
         // connection here
         //
         if (slMilliSecsV == 0)
         {
            btResultT = true;
         }
         else
         {
            //--------------------------------------------------
            // wait for a connection for a time frame of
            // 'slMilliSecsV' milli-seconds. This is a busy
            // wait, so be careful with this function
            //
            if (pclTcpSockP->waitForConnected(slMilliSecsV))
            {
               qDebug() << "QCanSocket::connectNetwork()  : connected";
               btResultT = true;
            }
         }
      }
      else
      {
         qDebug() << "QCanSocket::connectNetwork(" << ubChannelV << "," << slMilliSecsV << ") - Local";

         //----------------------------------------------------------
         // create new local socket
         //
         pclLocalSockP->abort();

         //----------------------------------------------------------
         // make signal / slot connection for local socket
         //
         connect( pclLocalSockP, SIGNAL(connected()),
                  this, SLOT(onSocketConnect()));

         connect( pclLocalSockP, SIGNAL(disconnected()),
                  this, SLOT(onSocketDisconnect()));

         connect( pclLocalSockP, SIGNAL(error(QLocalSocket::LocalSocketError)),
                  this, SLOT(onSocketErrorLocal(QLocalSocket::LocalSocketError)));

         connect( pclLocalSockP, SIGNAL(readyRead()),
                  this, SLOT(onSocketReceive()));

         //----------------------------------------------------------
         // connect to local server
         //
         pclLocalSockP->connectToServer(QString("CANpieServerChannel%1").arg(ubChannelV));
         qDebug() << "QCanSocket::connectNetwork() -" << pclLocalSockP->fullServerName();

         //----------------------------------------------------------
         // if the parameter slMilliSecsV is 0 we do not wait for a
         // connection here
         //
         if (slMilliSecsV == 0)
         {
            btResultT = true;
         }
         else
         {
            //--------------------------------------------------
            // wait for a connection for a time frame of
            // 'slMilliSecsV' milli-seconds. This is a busy
            // wait, so be careful with this function
            //
            if (pclLocalSockP->waitForConnected(slMilliSecsV))
            {
               qDebug() << "QCanSocket::connectNetwork()  : connected";
               btResultT = true;
            }
         }

      }
   }

   return(btResultT);
}


//----------------------------------------------------------------------------//
// disconnectNetwork()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocket::disconnectNetwork(void)
{
   qDebug() << "QCanSocket::disconnectNetwork() ";
   if (btIsLocalConnectionP == false)
   {
      pclTcpSockP->disconnectFromHost();
   }
   else
   {
      disconnect(pclLocalSockP, 0, 0, 0);
      pclLocalSockP->disconnectFromServer();
   }
}


//----------------------------------------------------------------------------//
// error()                                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
QAbstractSocket::SocketError QCanSocket::error() const
{
   return((QAbstractSocket::SocketError) slSocketErrorP);
}


//----------------------------------------------------------------------------//
// errorString()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
QString QCanSocket::errorString() const
{
   if (btIsLocalConnectionP == false)
   {
      return (pclTcpSockP->errorString());
   }
   return (pclLocalSockP->errorString());

}


//----------------------------------------------------------------------------//
// framesAvailable()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t QCanSocket::framesAvailable(void) const
{
   uint32_t    ulFrameCountT;

   if (btIsLocalConnectionP == false)
   {
      ulFrameCountT = pclTcpSockP->bytesAvailable() / QCAN_FRAME_ARRAY_SIZE;
   }
   else
   {
      ulFrameCountT = pclLocalSockP->bytesAvailable() / QCAN_FRAME_ARRAY_SIZE;
   }

   return (ulFrameCountT);
}


//----------------------------------------------------------------------------//
// isConnected()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanSocket::isConnected(void)
{
   return (btIsConnectedP);
}


//----------------------------------------------------------------------------//
// onSocketConnect()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocket::onSocketConnect(void)
{
   qDebug() << "QCanSocket::onSocketConnect() ";

   //----------------------------------------------------------------
   // send signal about connection state and keep it in local
   // variable
   //
   btIsConnectedP = true;
   emit connected();
}


//----------------------------------------------------------------------------//
// onSocketDisconnect()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocket::onSocketDisconnect(void)
{
   qDebug() << "QCanSocket::onSocketDisconnect() ";

   //----------------------------------------------------------------
   // send signal about connection state and keep it in local
   // variable
   //
   btIsConnectedP = false;
   emit disconnected();
}



//----------------------------------------------------------------------------//
// onSocketError()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocket::onSocketErrorLocal(QLocalSocket::LocalSocketError teSocketErrorV)
{

   qDebug() << "QCanSocket::onSocketErrorLocal() " << teSocketErrorV;
   qDebug() << pclLocalSockP->errorString();

   switch(teSocketErrorV)
   {
      //-------------------------------------------------------------
      // abort all operations and disconnect socket
      //
      case QLocalSocket::PeerClosedError:
      case QLocalSocket::ConnectionError:
         pclLocalSockP->abort();
         btIsConnectedP = false;
         emit disconnected();
         break;

      default:

         break;

   }

   //----------------------------------------------------------------
   // Store socket error and send signal:
   // Since the enumeration values for LocalSocketError are derived
   // from QAbstractSocket::SocketError inside the header file
   // QtNetwork/qlocalsocket.h we can simply cast it.
   //
   slSocketErrorP = teSocketErrorV;
   emit error( (QAbstractSocket::SocketError) teSocketErrorV);
}


//----------------------------------------------------------------------------//
// onSocketError()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocket::onSocketErrorTcp(QAbstractSocket::SocketError teSocketErrorV)
{
   qDebug() << "QCanSocket::onSocketErrorTcp() " << teSocketErrorV;

   switch(teSocketErrorV)
   {
      //-------------------------------------------------------------
      // abort all operations and disconnect socket
      //
      case QAbstractSocket::RemoteHostClosedError:
      case QAbstractSocket::NetworkError:
         pclTcpSockP->abort();
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


//----------------------------------------------------------------------------//
// onSocketReceive()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocket::onSocketReceive(void)
{
   uint32_t    ulFrameCountT;

   if (btIsLocalConnectionP == false)
   {
      ulFrameCountT = pclTcpSockP->bytesAvailable() / QCAN_FRAME_ARRAY_SIZE;
   }
   else
   {
      ulFrameCountT = pclLocalSockP->bytesAvailable() / QCAN_FRAME_ARRAY_SIZE;
   }
   framesReceived(ulFrameCountT);
}


//----------------------------------------------------------------------------//
// read()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanSocket::read(QByteArray & clFrameDataR, 
                      QCanData::Type_e * pubFrameTypeV)
{
   bool  btResultT = false;

   if (framesAvailable() > 0)
   {
      if (btIsLocalConnectionP == false)
      {
         clFrameDataR = pclTcpSockP->read(QCAN_FRAME_ARRAY_SIZE);
      }
      else
      {
         clFrameDataR = pclLocalSockP->read(QCAN_FRAME_ARRAY_SIZE);
      }

      if (pubFrameTypeV != Q_NULLPTR)
      {
         switch (clFrameDataR.at(0) & 0xE0)
         {
            case 0x00:
               *pubFrameTypeV = QCanData::eTYPE_CAN;
               break;
            
            case 0x40:
               *pubFrameTypeV = QCanData::eTYPE_API;
               break;
            
            case 0x80:
               *pubFrameTypeV = QCanData::eTYPE_ERROR;
               break;
               
            default:
               *pubFrameTypeV = QCanData::eTYPE_UNKNOWN;
               break;
         }
      }
      btResultT = true;
   }
   
   return (btResultT);
   
}

//----------------------------------------------------------------------------//
// readFrame()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanSocket::readFrame(QCanFrame & clFrameR)
{
   bool        btResultT = false;
   QByteArray  clDatagramT;

   if (framesAvailable() > 0)
   {
      if (btIsLocalConnectionP == false)
      {
         clDatagramT = pclTcpSockP->read(QCAN_FRAME_ARRAY_SIZE);
      }
      else
      {
         clDatagramT = pclLocalSockP->read(QCAN_FRAME_ARRAY_SIZE);
      }

      btResultT = clFrameR.fromByteArray(clDatagramT);
   }

   return(btResultT);
}


//----------------------------------------------------------------------------//
// setHostAddress()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocket::setHostAddress(QHostAddress clHostAddressV)
{
   if(btIsConnectedP == false)
   {
      clTcpHostAddrP = clHostAddressV;

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

//----------------------------------------------------------------------------//
// writeFrame()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanSocket::writeFrame(const QCanFrame & clFrameR)
{
   bool  btResultT = false;

   if(btIsConnectedP == true)
   {
      QByteArray  clDatagramT = clFrameR.toByteArray();

      if (btIsLocalConnectionP == false)
      {
         if (pclTcpSockP->write(clDatagramT) == QCAN_FRAME_ARRAY_SIZE)
         {
            pclTcpSockP->flush();
            btResultT = true;
         }
      }
      else
      {
         if (pclLocalSockP->write(clDatagramT) == QCAN_FRAME_ARRAY_SIZE)
         {
            btResultT = true;
         }
      }
   }

   return(btResultT);
}

//----------------------------------------------------------------------------//
// writeFrame()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanSocket::writeFrame(const QCanFrameApi & clFrameR)
{
   bool  btResultT = false;

   qDebug() << "QCanSocket::writeFrame(const QCanFrameApi)";

   if (btIsConnectedP == true)
   {
      QByteArray  clDatagramT = clFrameR.toByteArray();

      if (btIsLocalConnectionP == false)
      {
         if (pclTcpSockP->write(clDatagramT) == QCAN_FRAME_ARRAY_SIZE)
         {
            pclTcpSockP->flush();
            btResultT = true;
         }
      }
      else
      {
         qDebug() << "QCanSocket::writeFrame(const QCanFrameApi) - local";
         if (pclLocalSockP->write(clDatagramT) == QCAN_FRAME_ARRAY_SIZE)
         {
            btResultT = true;
         }
      }
   }

   return(btResultT);
}

//----------------------------------------------------------------------------//
// writeFrame()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanSocket::writeFrame(const QCanFrameError & clFrameR)
{
   bool  btResultT = false;

   if (btIsConnectedP == true)
   {
      QByteArray  clDatagramT = clFrameR.toByteArray();
      if (btIsLocalConnectionP == false)
      {
         if (pclTcpSockP->write(clDatagramT) == QCAN_FRAME_ARRAY_SIZE)
         {
            pclTcpSockP->flush();
            btResultT = true;
         }
      }
      else
      {
         if (pclLocalSockP->write(clDatagramT) == QCAN_FRAME_ARRAY_SIZE)
         {
            btResultT = true;
         }
      }
   }

   return(btResultT);
}
