//====================================================================================================================//
// File:          qcan_server_settings.cpp                                                                            //
// Description:   QCAN classes - CAN server                                                                           //
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

#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QObject>

#include "qcan_server_settings.hpp"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/




//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::QCanServerSettings()                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanServerSettings::QCanServerSettings(QObject * pclParentV)
{
   if (pclParentV != Q_NULLPTR)
   {
      this->setParent(pclParentV);
   }


   //---------------------------------------------------------------------------------------------------
   // the socket is not connected yet
   //
   pclWebSocketP.clear();
   teServerStateP = QCanServerSettings::eSTATE_UNKNOWN;

   ulUpdateRateP  = 0;

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::~QCanServerSettings()                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanServerSettings::~QCanServerSettings()
{
   closeConnection();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::closeConnection()                                                                              //
// disconnect from CANpie FD server                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerSettings::closeConnection(void)
{
   if (pclWebSocketP.isNull() == false)
   {
      if (teServerStateP == QCanServerSettings::eSTATE_ACTIVE)
      {
         pclWebSocketP->close(QWebSocketProtocol::CloseCodeNormal, "Quit connection");
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::connectToServer()                                                                              //
// connect to CANpie server                                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerSettings::connectToServer(const QHostAddress clServerAddressV, const uint16_t uwPortV, 
                                         const QString & clOriginR, uint32_t flags)
{
   Q_UNUSED(flags);

   pclWebSocketP = new QWebSocket(clOriginR);

   QString clSocketUrlT = QString("ws://") + clServerAddressV.toString() + QString(":%1").arg(uwPortV);
   clSocketUrlT += QString("/settings");
   
   QObject::connect( pclWebSocketP, &QWebSocket::connected,           
                     this, &QCanServerSettings::onSocketConnect);

   QObject::connect( pclWebSocketP, &QWebSocket::disconnected,        
                     this, &QCanServerSettings::onSocketDisconnect);

   QObject::connect( pclWebSocketP, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error),  
                     this, &QCanServerSettings::onSocketError);

   QObject::connect( pclWebSocketP, &QWebSocket::textFrameReceived,        
                     this, &QCanServerSettings::onSocketMessageReceived);

   pclWebSocketP->open(QUrl(clSocketUrlT));

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::isValid()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanServerSettings::isValid(void)
{
   bool  btResultT = false;

   if (teServerStateP == QCanServerSettings::eSTATE_ACTIVE)
   {
      if (clJsonServerP.isEmpty() == false)
      {
         if (clJsonServerP.contains("networkCount"))
         {
            btResultT = true;
         }
      }
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::networkCount()                                                                                 //
// return number of supported CAN networks                                                                            //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanServerSettings::networkCount(void)
{
   int32_t slNetworkCountT = 0;

   if (clJsonServerP.isEmpty() == false)
   {
      if (clJsonServerP.contains("networkCount"))
      {
         slNetworkCountT = clJsonServerP.value("networkCount").toInt();
      }
   }
   return (slNetworkCountT);
}




//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::onSocketConnect()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerSettings::onSocketConnect(void)
{
   qDebug() << "QCanServerSettings::onSocketConnect() ";

   //---------------------------------------------------------------------------------------------------
   // keep connection state in member variable and signal it 
   //
   if (teServerStateP != QCanServerSettings::eSTATE_ACTIVE)
   {
         teServerStateP = QCanServerSettings::eSTATE_ACTIVE;
         emit stateChanged(teServerStateP);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::onSocketDisconnect()                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerSettings::onSocketDisconnect(void)
{
   qDebug() << "QCanServerSettings::onSocketDisconnect() ";

   //---------------------------------------------------------------------------------------------------
   // keep connection state in member variable and signal it 
   //
   if (teServerStateP != QCanServerSettings::eSTATE_INACTIVE)
   {
         teServerStateP = QCanServerSettings::eSTATE_INACTIVE;
         emit stateChanged(teServerStateP);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::onSocketError()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerSettings::onSocketError(QAbstractSocket::SocketError teSocketErrorV)
{
   State_e  teNewStateV = QCanServerSettings::eSTATE_UNKNOWN;

   qDebug() << "QCanServerSettings::onSocketError()" << teSocketErrorV;

   //---------------------------------------------------------------------------------------------------
   // connection state is modified depending on the SocketError value
   //
   switch(teSocketErrorV)
   {
      case QAbstractSocket::RemoteHostClosedError:
         teNewStateV = QCanServerSettings::eSTATE_CLOSED;
         break;
      
      case QAbstractSocket::ConnectionRefusedError:
         teNewStateV = QCanServerSettings::eSTATE_INACTIVE;
         break;

      default:
         teNewStateV = QCanServerSettings::eSTATE_INACTIVE;
         break;

   }

   //---------------------------------------------------------------------------------------------------
   // signal new connection state
   //
   if (teServerStateP != teNewStateV)
   {
         teServerStateP = teNewStateV;
         emit stateChanged(teServerStateP);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::onSocketReceive()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerSettings::onSocketMessageReceived(const QString & clMessageR)
{
   qDebug() << "QCanServerSettings::onSocketMessageReceived()";

   //---------------------------------------------------------------------------------------------------
   // The message that we receive here is a JSON object, convert it into a JSON document first
   // and then check the validity.
   //
   QJsonParseError clJsonErrorT;
   QJsonDocument clJsonDocumentT = QJsonDocument::fromJson(clMessageR.toUtf8(), &clJsonErrorT);
   if ( (clJsonErrorT.error == QJsonParseError::NoError) && clJsonDocumentT.isObject())
   {
      //-------------------------------------------------------------------------------------------
      // Check versionMajor and versionMinor of JSON object
      //
      if (clJsonDocumentT.object().contains("versionMajor") && clJsonDocumentT.object().contains("versionMinor"))
      {
         clJsonServerP = clJsonDocumentT.object();
         emit objectReceived(clJsonServerP);
      }
   }

}



//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::state()                                                                                        //
// return the current state of the QCan server                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanServerSettings::State_e QCanServerSettings::state(void)
{
   return (teServerStateP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::stateString()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanServerSettings::stateString(void)
{
   QString  clResultT;

   switch (state())
   {
      case eSTATE_INACTIVE:
         clResultT = QObject::tr("is not active");
         break;

      case eSTATE_CLOSED:
         clResultT = QObject::tr("has been closed");
         break;

      case eSTATE_ACTIVE:
         clResultT = QObject::tr("is active");
         break;

      default:
         clResultT = QObject::tr("state is unknown");
         break;

   }

   return (clResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::versionBuild()                                                                                 //
// return build version                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanServerSettings::versionBuild(void)
{
   int32_t slVersionBuildT = -1;

   if (clJsonServerP.isEmpty() == false)
   {
      if (clJsonServerP.contains("versionBuild"))
      {
         slVersionBuildT = clJsonServerP.value("versionBuild").toInt();
      }
   }

   return (slVersionBuildT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::versionMajor()                                                                                 //
// return major version                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanServerSettings::versionMajor(void)
{
   int32_t slVersionMajorT = -1;

   if (clJsonServerP.isEmpty() == false)
   {
      if (clJsonServerP.contains("versionMajor"))
      {
         slVersionMajorT = clJsonServerP.value("versionMajor").toInt();
      }
   }

   return (slVersionMajorT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::versionMinor()                                                                                 //
// return minor version                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanServerSettings::versionMinor(void)
{
   int32_t slVersionMinorT = -1;

   if (clJsonServerP.isEmpty() == false)
   {
      if (clJsonServerP.contains("versionMinor"))
      {
         slVersionMinorT = clJsonServerP.value("versionMinor").toInt();
      }
   }

   return (slVersionMinorT);
}
