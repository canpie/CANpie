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
#include "qcan_server_memory.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/




/*--------------------------------------------------------------------------------------------------------------------*\
** Class methods QCanServer                                                                                           **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanServer::QCanServer( QObject * pclParentV, uint16_t  uwPortStartV, uint8_t ubNetworkNumV)
{
   QCanNetwork *  pclCanNetT;

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
   //
   btAllowBitrateChangeP = false;
   btAllowBusOffRecoverP = false;
   btAllowCanModeChangeP = false;

   //------------------------------------------------------------------------------------
   // save the supplied parameters
   //
   uwPortStartP  = uwPortStartV;
   ubNetworkMaxP = ubNetworkNumV;

   //---------------------------------------------------------------------------------------------------
   // setup a new local server which is listening to the
   //
   pclLocalSrvP = new QLocalServer();

   //---------------------------------------------------------------------------------------------------
   // create initial local socket list
   //
   pclLocalSockListP = new QVector<QLocalSocket*>;
   pclLocalSockListP->reserve(QCAN_LOCAL_SOCKET_MAX);

   //-------------------------------------------------------------------------------------------
   // limit the number of connections for local server
   //
   pclLocalSrvP->setMaxPendingConnections(QCAN_LOCAL_SOCKET_MAX);
   pclLocalSrvP->setSocketOptions(QLocalServer::WorldAccessOption);
   if(pclLocalSrvP->listen(QString("CANpieServer")))
   {
      //-------------------------------------------------------------------------------------------
      // a new connection is handled by the onLocalSocketConnect() method
      //
      connect( pclLocalSrvP, SIGNAL(newConnection()),
               this, SLOT(onLocalSocketConnect()));
   }

   //------------------------------------------------------------------------------------
   // setup shared memory for data exchange
   //
   initSettings();

   //------------------------------------------------------------------------------------
   // create CAN networks
   //
   pclListNetsP = new QVector<QCanNetwork *>;
   pclListNetsP->reserve(ubNetworkNumV);

   for(uint8_t ubNetCntT = 0; ubNetCntT < ubNetworkNumV; ubNetCntT++)
   {
      pclCanNetT = new QCanNetwork(pclParentV, uwPortStartV + ubNetCntT, pclSettingsP);
      pclListNetsP->append(pclCanNetT);
   }

   //------------------------------------------------------------------------------------
   // start timer that calls onTimerEvent() every second
   //
   pclTimerP = new QTimer();
   connect(pclTimerP, SIGNAL(timeout()), this, SLOT(onTimerEvent()));
   pclTimerP->start(1000);
}


//--------------------------------------------------------------------------------------------------------------------//
// ~QCanServer()                                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanServer::~QCanServer()
{
   //---------------------------------------------------------------------------------------------------
   // close local server
   //
   if (pclLocalSrvP->isListening())
   {
      pclLocalSrvP->close();
   }
   delete (pclLocalSrvP);

   //------------------------------------------------------------------------------------
   // stop timer
   //
   pclTimerP->stop();
   delete (pclTimerP);

   releaseSettings();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::enableBitrateChange()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::enableBitrateChange(bool btEnabledV)
{
   btAllowBitrateChangeP = btEnabledV;

   if (btMemoryAttachedP)
   {
      if (pclSettingsP->lock())
      {

         //-----------------------------------------------------------------------------------
         // get access to data of shared memory
         //
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();


         //-----------------------------------------------------------------------------------
         // modify server flags
         //
         if (btAllowBitrateChangeP)
         {
            ptsSettingsT->tsServer.slFlags |= QCAN_SERVER_FLAG_ALLOW_BITRATE;
         }
         else
         {
            ptsSettingsT->tsServer.slFlags &= ~QCAN_SERVER_FLAG_ALLOW_BITRATE;
         }
      }

      pclSettingsP->unlock();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::enableBusOffRecovery()                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::enableBusOffRecovery(bool btEnabledV)
{
   btAllowBusOffRecoverP = btEnabledV;

   if (btMemoryAttachedP)
   {
      if (pclSettingsP->lock())
      {

         //-----------------------------------------------------------------------------------
         // get access to data of shared memory
         //
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();


         //-----------------------------------------------------------------------------------
         // modify server flags
         //
         if (btAllowBusOffRecoverP)
         {
            ptsSettingsT->tsServer.slFlags |= QCAN_SERVER_FLAG_ALLOW_RECOVERY;
         }
         else
         {
            ptsSettingsT->tsServer.slFlags &= ~QCAN_SERVER_FLAG_ALLOW_RECOVERY;
         }
      }

      pclSettingsP->unlock();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::enableModeChange()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::enableModeChange(bool btEnabledV)
{
   btAllowCanModeChangeP = btEnabledV;

   if (btMemoryAttachedP)
   {
      if (pclSettingsP->lock())
      {

         //-----------------------------------------------------------------------------------
         // get access to data of shared memory
         //
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();


         //-----------------------------------------------------------------------------------
         // modify server flags
         //
         if (btAllowCanModeChangeP)
         {
            ptsSettingsT->tsServer.slFlags |= QCAN_SERVER_FLAG_ALLOW_MODE;
         }
         else
         {
            ptsSettingsT->tsServer.slFlags &= ~QCAN_SERVER_FLAG_ALLOW_MODE;
         }
      }

      pclSettingsP->unlock();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::initSettings()                                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::initSettings(void)
{

   pclSettingsP = new QSharedMemory(QString(QCAN_MEMORY_KEY));
   btMemoryAttachedP = pclSettingsP->create(sizeof(ServerSettings_ts), QSharedMemory::ReadWrite);

   if (btMemoryAttachedP)
   {
      if (pclSettingsP->lock())
      {
         //-----------------------------------------------------------------------------------
         // clear all memory initially
         //
         memset(pclSettingsP->data(), 0, sizeof(ServerSettings_ts));

         //-----------------------------------------------------------------------------------
         // setup the shared memory
         //
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         ptsSettingsT->tsServer.slVersionMajor = VERSION_MAJOR;
         ptsSettingsT->tsServer.slVersionMinor = VERSION_MINOR;
         ptsSettingsT->tsServer.slVersionBuild = VERSION_BUILD;
         ptsSettingsT->tsServer.slNetworkCount = ubNetworkMaxP;

         ptsSettingsT->tsServer.sqDateTimeStart  = QDateTime::currentMSecsSinceEpoch();
         ptsSettingsT->tsServer.sqDateTimeActual = QDateTime::currentMSecsSinceEpoch();

         ptsSettingsT->tsServer.slFlags        = 0;
      }

      pclSettingsP->unlock();
   }
   else
   {
      qDebug() << pclSettingsP->errorString();
      btMemoryAttachedP = pclSettingsP->attach();
   }

   qDebug() << "QCanServer::initSettings() - memory attached =" << btMemoryAttachedP;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::maximumNetwork()                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
uint8_t QCanServer::maximumNetwork(void) const
{
   return(ubNetworkMaxP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::network()                                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanNetwork * QCanServer::network(uint8_t ubNetworkIdxV)
{
   return(pclListNetsP->at(ubNetworkIdxV));
}

//--------------------------------------------------------------------------------------------------------------------//
// onLocalSocketConnect()                                                                                             //
// slot that manages a new local server connection                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::onLocalSocketConnect(void)
{
   QLocalSocket * pclSocketT;

   //---------------------------------------------------------------------------------------------------
   // Get next pending connect and add this socket to the the socket list
   //
   pclSocketT =  pclLocalSrvP->nextPendingConnection();
   clLocalSockMutexP.lock();
   pclLocalSockListP->append(pclSocketT);
   clLocalSockMutexP.unlock();

   //---------------------------------------------------------------------------------------------------
   // Add a slot that handles the disconnection of the socket from the local server
   //
   connect( pclSocketT, SIGNAL(disconnected()),
            this,       SLOT(onLocalSocketDisconnect())   );

   //---------------------------------------------------------------------------------------------------
   // Add a slot that handles when new data is available
   //
   connect( pclSocketT, SIGNAL(readyRead()),
            this,       SLOT(onLocalSocketNewData())   );

}


//--------------------------------------------------------------------------------------------------------------------//
// onLocalSocketDisconnect()                                                                                          //
// remove local socket from list                                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::onLocalSocketDisconnect(void)
{
   int32_t        slSockIdxT;
   QLocalSocket * pclSockT;
   QLocalSocket * pclSenderT;


   //---------------------------------------------------------------------------------------------------
   // get sender of signal
   //
   pclSenderT = (QLocalSocket* ) QObject::sender();

   //---------------------------------------------------------------------------------------------------
   // Disconnect everything connected to the sender
   //
   disconnect(pclSenderT, 0, 0, 0);

   //---------------------------------------------------------------------------------------------------
   // remove sender from socket list
   //
   clLocalSockMutexP.lock();
   for(slSockIdxT = 0; slSockIdxT < pclLocalSockListP->size(); slSockIdxT++)
   {
      pclSockT = pclLocalSockListP->at(slSockIdxT);
      if (pclSockT == pclSenderT)
      {
         pclLocalSockListP->remove(slSockIdxT);
         break;
      }
   }
   clLocalSockMutexP.unlock();

}


//--------------------------------------------------------------------------------------------------------------------//
// onLocalSocketNewData()                                                                                             //
// handle reception of new data on local socket                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::onLocalSocketNewData(void)
{
   QLocalSocket *    pclLocalSockT;
   QCanNetwork *     pclNetworkT;
   int32_t           slSockIdxT;
   int32_t           slListSizeT;


   //---------------------------------------------------------------------------------------------------
   // lock socket list
   //
   clLocalSockMutexP.lock();


   //---------------------------------------------------------------------------------------------------
   // check all open local sockets and read messages
   //
   slListSizeT = pclLocalSockListP->size();
   for(slSockIdxT = 0; slSockIdxT < slListSizeT; slSockIdxT++)
   {
      pclLocalSockT = pclLocalSockListP->at(slSockIdxT);
      uint32_t ulCmdMaxT = (pclLocalSockT->bytesAvailable()) / sizeof(ServerSocketCommand_ts);
      while (ulCmdMaxT > 0)
      {
         ServerSocketCommand_ts  tsCommandT;
         pclLocalSockT->read((char *) &tsCommandT, sizeof(ServerSocketCommand_ts));
         pclNetworkT = network(tsCommandT.ubChannel - 1);
         if (pclNetworkT != (QCanNetwork *) 0L)
         {
            switch(tsCommandT.ubCommand)
            {
               case eSERVER_CMD_BITRATE:
                  pclNetworkT->setBitrate(tsCommandT.slParameter[0], tsCommandT.slParameter[1]);
                  break;

               case eSERVER_CMD_MODE:
                  if (tsCommandT.slParameter[0] == eCAN_MODE_INIT)
                  {
                     pclNetworkT->stopInterface();
                  }
                  if (tsCommandT.slParameter[0] == eCAN_MODE_OPERATION)
                  {
                     pclNetworkT->startInterface();
                  }
                  if (tsCommandT.slParameter[0] == eCAN_MODE_LISTEN_ONLY)
                  {
                     pclNetworkT->setListenOnlyEnabled(true);
                     pclNetworkT->startInterface();
                  }
                  break;

               case eSERVER_CMD_RESET:
                  pclNetworkT->reset();
                  break;
            }
         }
         ulCmdMaxT = (pclLocalSockT->bytesAvailable()) / sizeof(ServerSocketCommand_ts);
      }
   }

   //---------------------------------------------------------------------------------------------------
   // unlock mutex
   //
   clLocalSockMutexP.unlock();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::onTimerEvent()                                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::onTimerEvent(void)
{
   if (btMemoryAttachedP)
   {
      pclSettingsP->lock();

      //-------------------------------------------------------------------------------------------
      // update the system time
      //
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();
      if (ptsSettingsT != (ServerSettings_ts *) 0L)
      {
         ptsSettingsT->tsServer.sqDateTimeActual = QDateTime::currentMSecsSinceEpoch();
      }

      pclSettingsP->unlock();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::releaseSettings()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::releaseSettings(void)
{
   pclSettingsP->detach();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::setServerAddress()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::setServerAddress(QHostAddress clHostAddressV)
{
   QCanNetwork *  pclNetworkT;

   if (!clHostAddressV.isEqual(clServerAddressP))
   {
      clServerAddressP = clHostAddressV;

      qDebug() << "QCanServer::setServerAddress()" << clHostAddressV;

      for(uint8_t ubNetCntT = 0; ubNetCntT < maximumNetwork(); ubNetCntT++)
      {
         pclNetworkT = network(ubNetCntT);

         //-------------------------------------------------------------
         // check current state of network
         //
         if(pclNetworkT->isNetworkEnabled() == true)
         {
            //-----------------------------------------------------
            // assign new host address
            //
            pclNetworkT->setNetworkEnabled(false);
            pclNetworkT->setServerAddress(clHostAddressV);
            pclNetworkT->setNetworkEnabled(true);
         }
         else
         {
            //-----------------------------------------------------
            // assign new host address
            //
            pclNetworkT->setServerAddress(clHostAddressV);
         }
      }
   }
}


