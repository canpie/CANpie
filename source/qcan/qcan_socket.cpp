//============================================================================//
// File:          qcan_network.cpp                                            //
// Description:   QCAN classes - CAN network                                  //
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
bool QCanSocket::connectNetwork(QCan::CAN_Channel_e ubChannelV)
{
   bool btResultT = false;

   //----------------------------------------------------------------
   // create a new local socket
   //
   if(btIsConnectedP == false)
   {
      qDebug() << "QCanSocket::connectNetwork() " << ubChannelV;

      pclTcpSockP->abort();
      pclTcpSockP->connectToHost(clTcpHostAddrP, uwTcpPortP + ubChannelV);
      btResultT = true;
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

int32_t QCanSocket::framesAvailable(void) const
{
   uint32_t    ulFrameCountT;


   ulFrameCountT = pclTcpSockP->bytesAvailable() / QCAN_FRAME_ARRAY_SIZE;
   return(ulFrameCountT);
}

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
   btIsConnectedP = true;
   qDebug() << "QCanSocket::onSocketConnect() - " << btIsConnectedP;
}


//----------------------------------------------------------------------------//
// onSocketDisconnect()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocket::onSocketDisconnect(void)
{
   qDebug() << "QCanSocket::onSocketDisconnect() ";
   btIsConnectedP = false;

}


//----------------------------------------------------------------------------//
// onSocketError()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocket::onSocketError(QAbstractSocket::SocketError eSocketErrorV)
{
   qDebug() << "QCanSocket::onSocketError() ";

}


//----------------------------------------------------------------------------//
// onSocketReceive()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocket::onSocketReceive(void)
{
   uint32_t    ulFrameCountT;

   qDebug() << "QCanSocket::onSocketReceive() ";

   ulFrameCountT = pclTcpSockP->bytesAvailable() / QCAN_FRAME_ARRAY_SIZE;
   framesReceived(ulFrameCountT);
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
      clFrameR.fromByteArray(clDatagramT);
      btResultT = true;
   }
   return(btResultT);
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



