//====================================================================================================================//
// File:          qcan_server.cpp                                                                                     //
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

#include "qcan_server.hpp"

#include <QtCore/QJsonDocument>


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#define  QCAN_MEMORY_KEY                     "QCAN_SERVER_SHARED_KEY"

#define  MAXIMUM_SERVER_TIME_DIFFERENCE      ((int64_t) 2000)

//------------------------------------------------------------------------------------------------------
// Version information is controller via qmake project file, the following defintions are only
// placeholders
//
#ifndef  VERSION_MAJOR
#define  VERSION_MAJOR                       1
#endif

#ifndef  VERSION_MINOR
#define  VERSION_MINOR                       0
#endif

#ifndef  VERSION_BUILD
#define  VERSION_BUILD                       0
#endif


/*--------------------------------------------------------------------------------------------------------------------*\
** Structures                                                                                                         **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//-----------------------------------------------------------------------------------------------------
// Server signature inside shared memory
//
typedef struct CanServerSharedMemory_s {

   //--------------------------------------------------------------------------
   // Server version: major
   //
   int32_t  slVersionMajor;

   //--------------------------------------------------------------------------
   // Server version: minor
   //
   int32_t  slVersionMinor;

   //--------------------------------------------------------------------------
   // Server version: build
   //
   int32_t  slVersionBuild;

   //--------------------------------------------------------------------------
   // Number of CAN networks supported by the server
   //
   int32_t  slNetworkCount;

   //--------------------------------------------------------------------------
   // Stores the start date / time of the server in milliseconds that have
   // passed since 1970-01-01T00:00:00.000
   //
   int64_t  sqDateTimeStart;

   //--------------------------------------------------------------------------
   // Stores the actual date / time of the server in milliseconds that have
   // passed since 1970-01-01T00:00:00.000
   //
   int64_t  sqDateTimeActual;


} CanServerSharedMemory_ts;


/*--------------------------------------------------------------------------------------------------------------------*\
** Class methods QCanServer                                                                                           **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanServer::QCanServer( QObject * pclParentV, uint16_t uwPortNumberV, uint8_t ubNetworkNumV, bool btClearServerV)
{

   //---------------------------------------------------------------------------------------------------
   // set the parent
   //
   this->setParent(pclParentV);

   //---------------------------------------------------------------------------------------------------
   // limit the number of possible networks to QCAN_NETWORK_MAX
   //
   if (ubNetworkNumV > QCAN_NETWORK_MAX)
   {
      ubNetworkNumV = QCAN_NETWORK_MAX;
   }

   //---------------------------------------------------------------------------------------------------
   // disable all feature changes by default
   //
   btAllowBitrateChangeP = false;
   btAllowBusOffRecoverP = false;
   btAllowCanModeChangeP = false;

   //---------------------------------------------------------------------------------------------------
   // access is only granted from local host by default
   //
   clServerAddressP      = QHostAddress::LocalHost;

   //---------------------------------------------------------------------------------------------------
   // set pointers to NULL
   //
   pclServerConfigurationP = Q_NULLPTR;
   pclTimerP               = Q_NULLPTR;
   
   //---------------------------------------------------------------------------------------------------
   // store the supplied parameters of the constructor 
   //
   uwServerPortP = uwPortNumberV;
   ubNetworkMaxP = ubNetworkNumV;

   //---------------------------------------------------------------------------------------------------
   // assume no error before checking the server configuration
   //
   teErrorP = QCanServer::eERROR_NONE;

   //---------------------------------------------------------------------------------------------------
   // create list for CAN networks
   //
   clNetworkListP.reserve(ubNetworkNumV);


   //---------------------------------------------------------------------------------------------------
   // check if the server is already active
   //
   checkServerSettings(btClearServerV);
   if (teErrorP == eERROR_NONE)
   {
      //-------------------------------------------------------------------------------------------
      // setup a new web socket server 
      //
      pclWebSocketServerP  = new QWebSocketServer("CANpie-Server", QWebSocketServer::NonSecureMode);

      connect( pclWebSocketServerP, &QWebSocketServer::newConnection,
               this, &QCanServer::onWebSocketServerConnect);

      connect( pclWebSocketServerP, &QWebSocketServer::closed,
               this, &QCanServer::onWebSocketServerClose);

      connect( pclWebSocketServerP, &QWebSocketServer::serverError,
               this, &QCanServer::onWebSocketServerError);

      pclWebSocketServerP->setMaxPendingConnections(QCAN_WEB_SOCKET_MAX);
      pclWebSocketServerP->listen(clServerAddressP, uwServerPortP);

      clWebSocketListP.clear();

      //-------------------------------------------------------------------------------------------
      // create CAN networks
      //
      QCanNetwork *  pclCanNetT;

      for(uint8_t ubNetCntT = 0; ubNetCntT < ubNetworkNumV; ubNetCntT++)
      {
         pclCanNetT = new QCanNetwork(pclParentV);
         clNetworkListP.append(pclCanNetT);
      }


      //-------------------------------------------------------------------------------------------
      // start timer that calls onTimerEvent() every second
      //
      pclTimerP = new QTimer();
      connect(pclTimerP, &QTimer::timeout, this, &QCanServer::onTimerEvent);
      pclTimerP->start(1000);
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// ~QCanServer()                                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanServer::~QCanServer()
{
   //---------------------------------------------------------------------------------------------------
   // close web socket server
   //
   if (pclWebSocketServerP != Q_NULLPTR)
   {
      if (pclWebSocketServerP->isListening())
      {
         pclWebSocketServerP->close();
      }
      delete (pclWebSocketServerP);
   }
   

   //---------------------------------------------------------------------------------------------------
   // remove all networks
   //
   for(uint8_t ubNetCntT = 0; ubNetCntT < clNetworkListP.size(); ubNetCntT++)
   {
      delete(clNetworkListP.at(ubNetCntT));
   }
   clNetworkListP.clear();

   //---------------------------------------------------------------------------------------------------
   // stop timer
   //
   if (pclTimerP != Q_NULLPTR)
   {
      pclTimerP->stop();
      delete (pclTimerP);
   }

   //---------------------------------------------------------------------------------------------------
   // Detaches the process from the shared memory segment
   //
   if (pclServerConfigurationP != Q_NULLPTR)
   {
      pclServerConfigurationP->detach();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::checkServerSettings()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::checkServerSettings(bool btClearServerV)
{
   pclServerConfigurationP = new QSharedMemory(QString(QCAN_MEMORY_KEY));
 
   //---------------------------------------------------------------------------------------------------
   // Check if the shared memory could be attached, if not test the flag btClearServerV and try to
   // attach it again.
   //
   if (pclServerConfigurationP->create(sizeof(CanServerSharedMemory_ts), QSharedMemory::ReadWrite) == false)
   {
      if (pclServerConfigurationP->error() == QSharedMemory::AlreadyExists)
      {
         teErrorP = QCanServer::eERROR_ACTIVE;
         if (pclServerConfigurationP->attach() == true)
         {
            CanServerSharedMemory_ts * ptsConfigurationT = (CanServerSharedMemory_ts *)pclServerConfigurationP->data();
            if ((QDateTime::currentMSecsSinceEpoch() - (ptsConfigurationT->sqDateTimeActual)) > MAXIMUM_SERVER_TIME_DIFFERENCE)
            {
               teErrorP = QCanServer::eERROR_CRASHED;
               emit error(teErrorP);
            }
         }
      }
   }

   //---------------------------------------------------------------------------------------------------
   // If the momory has been attached to the process clear it first and then setup server values
   //
   if ( (teErrorP == QCanServer::eERROR_NONE) ||
        (btClearServerV && (teErrorP == QCanServer::eERROR_CRASHED)) )
   {
      if (pclServerConfigurationP->lock() == true)
      {
         //-----------------------------------------------------------------------------------
         // clear all memory initially
         //
         memset(pclServerConfigurationP->data(), 0, sizeof(CanServerSharedMemory_ts));

         //-----------------------------------------------------------------------------------
         // setup the shared memory
         //
         CanServerSharedMemory_ts * ptsConfigurationT = (CanServerSharedMemory_ts *) pclServerConfigurationP->data();

         ptsConfigurationT->slVersionMajor = VERSION_MAJOR;
         ptsConfigurationT->slVersionMinor = VERSION_MINOR;
         ptsConfigurationT->slVersionBuild = VERSION_BUILD;
         ptsConfigurationT->slNetworkCount = ubNetworkMaxP;

         sqDateTimeStartP = QDateTime::currentMSecsSinceEpoch();
         ptsConfigurationT->sqDateTimeStart  = sqDateTimeStartP;
         ptsConfigurationT->sqDateTimeActual = sqDateTimeStartP;
         sqUptimeMillisecondsP = 0;

         pclServerConfigurationP->unlock();
         teErrorP = QCanServer::eERROR_NONE;
      } 
   }

   emit error(teErrorP);

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::allowBitrateChange()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::allowBitrateChange(bool btEnabledV)
{
   qDebug() << " QCanServer::enableBitrateChange()" << btEnabledV;
   btAllowBitrateChangeP = btEnabledV;

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::allowBusOffRecovery()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::allowBusOffRecovery(bool btEnabledV)
{
   btAllowBusOffRecoverP = btEnabledV;

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::allowModeChange()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::allowModeChange(bool btEnabledV)
{
   btAllowCanModeChangeP = btEnabledV;

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::maximumNetwork()                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
uint8_t QCanServer::maximumNetwork(void) const
{
   return (ubNetworkMaxP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::network()                                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanNetwork * QCanServer::network(uint8_t ubNetworkIdxV)
{
   QCanNetwork * pclNetworkT = (QCanNetwork *) 0L;

   if (ubNetworkIdxV < clNetworkListP.size())
   {
      pclNetworkT = clNetworkListP.at(ubNetworkIdxV);
   }

   return(pclNetworkT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::onTimerEvent()                                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::onTimerEvent(void)
{
   if (teErrorP == QCanServer::eERROR_NONE)
   {
      int64_t sqCurrentMSecsT = QDateTime::currentMSecsSinceEpoch();

      pclServerConfigurationP->lock();

      //-------------------------------------------------------------------------------------------
      // update the system time in shared memorys
      //
      CanServerSharedMemory_ts * ptsConfigurationT = (CanServerSharedMemory_ts *) pclServerConfigurationP->data();
      if (ptsConfigurationT != (CanServerSharedMemory_ts *) 0L)
      {
         ptsConfigurationT->sqDateTimeActual = sqCurrentMSecsT;
      }

      pclServerConfigurationP->unlock();

      sqUptimeMillisecondsP = sqCurrentMSecsT - sqDateTimeStartP;
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::onSocketDisconnect()                                                                                   //
// remove socket from list                                                                                            //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::onSocketDisconnect(void)
{
   qDebug() << "QCanServer::onSocketDisconnect()";

   QWebSocket * pclSocketT = qobject_cast<QWebSocket *>(sender());

   //---------------------------------------------------------------------------------------------------
   // search for this socket pointer inside the list
   //
   for (int32_t slCountT = 0; slCountT < clWebSocketListP.size(); slCountT++)
   {
      if (clWebSocketListP.at(slCountT) == pclSocketT)
      {

         //-----------------------------------------------------------------------------------
         // remove all signal / slot connections
         //
         pclSocketT->disconnect();

         //-----------------------------------------------------------------------------------
         // remove entry from list
         //
         clWebSocketListP.removeAt(slCountT);

         pclSocketT->close();
         break;
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::onWebSocketConnect()                                                                                   //
// slot that manages a new local server connection                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::onWebSocketServerConnect(void)
{
   QWebSocket * pclSocketT;
   bool         btCloseSocketT = true;


   //---------------------------------------------------------------------------------------------------
   // Get next pending connection
   //
   pclSocketT = pclWebSocketServerP->nextPendingConnection();


   //---------------------------------------------------------------------------------------------------
   // Get the URL path and test if it is valid:
   // URL = <server>/settings       -> access the QCanServer class, settings
   // URL = <server>/<num>          -> access CAN network with number <num>, data exchange
   // URL = <server>/<num>/settings -> access CAN network with number <num>, settings
   //
   if (pclSocketT->requestUrl().isValid())
   {

      QString clSocketPathT  = pclSocketT->requestUrl().path();

      //-------------------------------------------------------------------------------------------
      // Split path in two sections
      //
      QString clSectionOneT = clSocketPathT.section('/', 0, 0, QString::SectionSkipEmpty);
      QString clSectionTwoT = clSocketPathT.section('/', 1, 1, QString::SectionSkipEmpty);

      if (clSectionOneT.compare("settings") == 0)
      {
         //-----------------------------------------------------------------------------------
         // access the QCanServer class
         //
         btCloseSocketT = false;
         clWebSocketListP.append(pclSocketT);


         connect( pclSocketT, &QWebSocket::disconnected,
                  this,       &QCanServer::onSocketDisconnect);

         sendServerSettings(pclSocketT);

      }
      else
      {
         //-----------------------------------------------------------------------------------
         // Access to network: the first section holds a valid network number > 0. If the
         // conversion fails the method toInt() returns 0.
         //
         int32_t slNetNumberT = clSectionOneT.toInt();

         if (slNetNumberT > 0)
         {
            QCanNetwork * pclNetworkT = network(slNetNumberT - 1);
            if (clSectionTwoT.compare("settings") == 0)
            {
               if (pclNetworkT != Q_NULLPTR)
               {
                  pclNetworkT->attachWebSocket(pclSocketT, QCanNetwork::eSOCKET_TYPE_SETTINGS);
                  btCloseSocketT = false;
               }
            }
            else
            {
               if (pclNetworkT != Q_NULLPTR)
               {
                  pclNetworkT->attachWebSocket(pclSocketT, QCanNetwork::eSOCKET_TYPE_CAN_FRAME);
                  btCloseSocketT = false;
               }
            }
         }
      }
   }

   //---------------------------------------------------------------------------------------------------
   // if the request failed close the socket
   //
   if (btCloseSocketT)
   {
      pclSocketT->close(QWebSocketProtocol::CloseCodeDatatypeNotSupported);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::onWebSocketClose()                                                                                     //
// close WebSocket server                                                                                             //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::onWebSocketServerClose(void)
{
   qDebug() << "QCanServer::onWebSocketServerClose()";
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::onWebSocketServerError()                                                                               //
// react on error from WebSocket server                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::onWebSocketServerError(QWebSocketProtocol::CloseCode teCloseCodeV)
{
   qDebug() << "QCanServer::onWebSocketServerError()" << teCloseCodeV;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::sendServerSettings()                                                                                   //
// send information about server via WebSocket                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::sendServerSettings(QWebSocket * pclSocketV, uint32_t flags)
{
   Q_UNUSED(flags);

   
   //---------------------------------------------------------------------------------------------------
   // Create JSON object containing all information about the server. Structure is
   // {
   //    "apiVersion" : "1.0",
   //     <serverOject>  
   // }
   //
   clJsonServerP["apiVersion"]          = "1.0";

   clJsonServerP["allowBitrateChange"]  = btAllowBitrateChangeP;
   clJsonServerP["allowModeChange"]     = btAllowCanModeChangeP;
   clJsonServerP["allowBusOffRecovery"] = btAllowBusOffRecoverP;

   clJsonServerP["networkCount"]        = (int) ubNetworkMaxP;
   clJsonServerP["serverLocalTime"]     = QDateTime::currentDateTime().toString(Qt::ISODate);
   clJsonServerP["serverUptime"]        = (qint64)sqUptimeMillisecondsP;

   clJsonServerP["versionMajor"]        = (int) VERSION_MAJOR;
   clJsonServerP["versionMinor"]        = (int) VERSION_MINOR;
   clJsonServerP["versionBuild"]        = (int) VERSION_BUILD;


   //---------------------------------------------------------------------------------------------------
   // create JSON document in text format and send it via the socket
   //
   QJsonDocument clJsonDocumentT(clJsonServerP);

   if (pclSocketV != Q_NULLPTR)
   {
         qDebug() << "sendServerSettings";

      pclSocketV->sendTextMessage(clJsonDocumentT.toJson());
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::setServerAddress()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::setServerAddress(const QHostAddress clHostAddressV, const uint16_t uwPortV)
{
   if (pclWebSocketServerP != Q_NULLPTR)
   {
      if (pclWebSocketServerP->isListening())
      {
         pclWebSocketServerP->close();

         clServerAddressP = clHostAddressV;
         uwServerPortP    = uwPortV;
         qDebug() << "QCanServer::setServerAddress(" << clHostAddressV << "," << uwPortV << ")";
         pclWebSocketServerP->listen(clServerAddressP, uwServerPortP);
      }
   }
}
