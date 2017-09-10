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


#include <QDebug>
#include <QNetworkInterface>

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanStub()                                                                 //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanSocket::QCanSocket(QObject * pclParentV)
{
   if(pclParentV != Q_NULLPTR)
   {
      this->setParent(pclParentV);
   }

   //----------------------------------------------------------------
   // create new TCP socket which is not connected yet
   //
   pclTcpSockP = new QTcpSocket(this);
   btIsConnectedP = false;

   //----------------------------------------------------------------
   // set default values for host address and port
   //
   clTcpHostAddrP = QHostAddress(QHostAddress::LocalHost);
   uwTcpPortP = QCAN_TCP_DEFAULT_PORT;

   //----------------------------------------------------------------
   // make signal / slot connection for TCP socket
   //
   connect( pclTcpSockP, SIGNAL(connected()),
            this, SLOT(onSocketConnect()));

   connect( pclTcpSockP, SIGNAL(disconnected()),
            this, SLOT(onSocketDisconnect()));

   connect( pclTcpSockP, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onSocketError(QAbstractSocket::SocketError)));

   connect( pclTcpSockP, SIGNAL(readyRead()),
            this, SLOT(onSocketReceive()));
}


QCanSocket::~QCanSocket()
{
   delete(pclTcpSockP);

}


//----------------------------------------------------------------------------//
// connectNetwork()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanSocket::connectNetwork(CAN_Channel_e ubChannelV, 
                                const int32_t slMilliSecsV)
{
   bool btResultT = false;

   //----------------------------------------------------------------
   // create a new local socket
   //
   if(btIsConnectedP == false)
   {
      qDebug() << "QCanSocket::connectNetwork()  :" << ubChannelV;

      pclTcpSockP->abort();
      pclTcpSockP->connectToHost(clTcpHostAddrP, uwTcpPortP + ubChannelV - 1);

      //--------------------------------------------------------
      // if the parameter slMilliSecsV is 0 we do not wait for a
      // connection here
      //
      if (slMilliSecsV == 0)
      {
         btResultT = true;
      }
      else
      {
         //------------------------------------------------
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

   return(btResultT);
}


//----------------------------------------------------------------------------//
// disconnectNetwork()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocket::disconnectNetwork(void)
{
   qDebug() << "QCanSocket::disconnectNetwork() ";
   pclTcpSockP->disconnectFromHost();
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
   return(pclTcpSockP->errorString());
}


//----------------------------------------------------------------------------//
// framesAvailable()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t QCanSocket::framesAvailable(void) const
{
   uint32_t    ulFrameCountT;


   ulFrameCountT = pclTcpSockP->bytesAvailable() / QCAN_FRAME_ARRAY_SIZE;
   return(ulFrameCountT);
}


//----------------------------------------------------------------------------//
// isConnected()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanSocket::isConnected(void)
{
   return(btIsConnectedP);
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
void QCanSocket::onSocketError(QAbstractSocket::SocketError teSocketErrorV)
{
   qDebug() << "QCanSocket::onSocketError() ";

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

   ulFrameCountT = pclTcpSockP->bytesAvailable() / QCAN_FRAME_ARRAY_SIZE;
   framesReceived(ulFrameCountT);
}

bool QCanSocket::read(QByteArray & clFrameDataR, 
                      QCanData::Type_e * pubFrameTypeV)
{
   bool  btResultT = false;

   if(framesAvailable() > 0)
   {
      clFrameDataR = pclTcpSockP->read(QCAN_FRAME_ARRAY_SIZE);
      if (pubFrameTypeV != Q_NULLPTR)
      {
         switch(clFrameDataR.at(0) & 0xE0)
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
   bool  btResultT = false;
   QByteArray  clDatagramT;

   if(framesAvailable() > 0)
   {
      clDatagramT = pclTcpSockP->read(QCAN_FRAME_ARRAY_SIZE);
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
      if(pclTcpSockP->write(clDatagramT) == QCAN_FRAME_ARRAY_SIZE)
      {
         pclTcpSockP->flush();
         btResultT = true;
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

   if(btIsConnectedP == true)
   {
      QByteArray  clDatagramT = clFrameR.toByteArray();
      if(pclTcpSockP->write(clDatagramT) == QCAN_FRAME_ARRAY_SIZE)
      {
         pclTcpSockP->flush();
         btResultT = true;
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

   if(btIsConnectedP == true)
   {
      QByteArray  clDatagramT = clFrameR.toByteArray();
      if(pclTcpSockP->write(clDatagramT) == QCAN_FRAME_ARRAY_SIZE)
      {
         pclTcpSockP->flush();
         btResultT = true;
      }
   }

   return(btResultT);
}

