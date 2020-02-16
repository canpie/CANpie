//====================================================================================================================//
// File:          qcan_network_settings.cpp                                                                           //
// Description:   QCAN classes - CAN network settings                                                                 //
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
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "qcan_network_settings.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/




//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::QCanNetworkSettings()                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanNetworkSettings::QCanNetworkSettings(const CAN_Channel_e teChannelV, QObject * pclParentV)
{
   if (pclParentV != Q_NULLPTR)
   {
      this->setParent(pclParentV);
   }


   //---------------------------------------------------------------------------------------------------
   // the socket is not connected yet
   //
   pclWebSocketP.clear();
   teServerStateP = QCanNetworkSettings::eSTATE_UNKNOWN;

   ulUpdateRateP  = 0;

   teChannelP     = teChannelV;

   //---------------------------------------------------------------------------------------------------
   // Initialize JSON object for commands to server
   //
   clJsonCommandP["apiVersion"]           = "1.0";
   clJsonCommandP["channel"]              = (int32_t) teChannelP;

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::~QCanNetworkSettings()                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanNetworkSettings::~QCanNetworkSettings()
{
   closeConnection();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::connectToServer()                                                                             //
// connect to CANpie FD server                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetworkSettings::connectToServer(const QHostAddress clServerAddressV, const uint16_t uwPortV, 
                                          const QString & clOriginR, uint32_t flags)
{
   Q_UNUSED(flags);

   pclWebSocketP = new QWebSocket(clOriginR);

   QString clSocketUrlT = QString("ws://") + clServerAddressV.toString() + QString(":%1").arg(uwPortV);
   clSocketUrlT += QString("/%1/settings").arg(teChannelP);
   
   QObject::connect( pclWebSocketP, &QWebSocket::connected,           
                     this, &QCanNetworkSettings::onSocketConnect);

   QObject::connect( pclWebSocketP, &QWebSocket::disconnected,        
                     this, &QCanNetworkSettings::onSocketDisconnect);

   QObject::connect( pclWebSocketP, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error),  
                     this, &QCanNetworkSettings::onSocketError);

   QObject::connect( pclWebSocketP, &QWebSocket::textFrameReceived,        
                     this, &QCanNetworkSettings::onSocketMessageReceived);

   pclWebSocketP->open(QUrl(clSocketUrlT));

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::closeConnection()                                                                             //
// disconnect from CANpie FD server                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetworkSettings::closeConnection(void)
{
   if (pclWebSocketP.isNull() == false)
   {
      if (teServerStateP == QCanNetworkSettings::eSTATE_ACTIVE)
      {
         pclWebSocketP->close(QWebSocketProtocol::CloseCodeNormal, "Quit connection");
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::dataBitrate()                                                                                 //
// return the data bit-rate of the current CAN interface                                                              //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanNetworkSettings::dataBitrate(void)
{
   int32_t  slResultT = eCAN_BITRATE_NONE;

   if (teServerStateP == QCanNetworkSettings::eSTATE_ACTIVE)
   {
      if (clJsonNetworkP.isEmpty() == false)
      {
         if (clJsonNetworkP.contains("bitrateData"))
         {
            slResultT = clJsonNetworkP.value("bitrateData").toInt();
         }
      }
   }

   return (slResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::dataBitrateString()                                                                           //
// return the data bit-rate of the current CAN interface                                                              //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanNetworkSettings::dataBitrateString(void)
{
   QString  clResultT;
   int32_t  slBitrateT;

   slBitrateT = dataBitrate();

   //---------------------------------------------------------------------------------------------------
   // Test for unknown bit-rate first
   //
   if (slBitrateT == eCAN_BITRATE_NONE)
   {
      clResultT = "None";
   }
   else
   {
      if (slBitrateT < 1000000)
      {
         //-----------------------------------------------------------------------------------
         // print values < 1000000 in kBit/s
         //
         clResultT = QString("%1 kBit/s").arg(slBitrateT / 1000);
      }
      else
      {
         //------------------------------------------------
         // print values >= 1000000 in MBit/s
         //
         clResultT = QString("%1 MBit/s").arg(slBitrateT / 1000000);
      }

   }
   return (clResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::errorCount()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanNetworkSettings::errorCount(void)
{
   int32_t teResultT = -1;

   if (teServerStateP == QCanNetworkSettings::eSTATE_ACTIVE)
   {
      if (clJsonNetworkP.isEmpty() == false)
      {
         if (clJsonNetworkP.contains("frameCountError"))
         {
            teResultT = clJsonNetworkP.value("frameCountError").toInt();
         }
      }
   }

   return (teResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::frameCount()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanNetworkSettings::frameCount(void)
{
   int32_t teResultT = -1;

   if (teServerStateP == QCanNetworkSettings::eSTATE_ACTIVE)
   {
      if (clJsonNetworkP.isEmpty() == false)
      {
         if (clJsonNetworkP.contains("frameCount"))
         {
            teResultT = clJsonNetworkP.value("frameCount").toInt();
         }
      }
   }

   return (teResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::isValid()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanNetworkSettings::isValid(void)
{
   bool  btResultT = false;

   if (teServerStateP == QCanNetworkSettings::eSTATE_ACTIVE)
   {
      if (clJsonNetworkP.isEmpty() == false)
      {
         if (clJsonNetworkP.contains("channel"))
         {
            btResultT = true;
         }
      }
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::name()                                                                                        //
// return the name of the current CAN interface                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanNetworkSettings::name(void)
{
   QString clResultT;

   if (teServerStateP == QCanNetworkSettings::eSTATE_ACTIVE)
   {
      if (clJsonNetworkP.isEmpty() == false)
      {
         if (clJsonNetworkP.contains("name"))
         {
            clResultT = clJsonNetworkP.value("name").toString();
         }

         if (clJsonNetworkP.contains("interfaceName"))
         {
            clResultT += " - ";
            clResultT += clJsonNetworkP.value("interfaceName").toString();
         }

         if (clJsonNetworkP.contains("interfaceVersion"))
         {
            clResultT += " - ";
            clResultT += clJsonNetworkP.value("interfaceVersion").toString();
         }

      }
   }

   return (clResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::nominalBitrate()                                                                              //
// return the nominal bit-rate of the current CAN interface                                                           //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanNetworkSettings::nominalBitrate(void)
{
   int32_t  slResultT = eCAN_BITRATE_NONE;

   if (teServerStateP == QCanNetworkSettings::eSTATE_ACTIVE)
   {
      if (clJsonNetworkP.isEmpty() == false)
      {
         if (clJsonNetworkP.contains("bitrateNominal"))
         {
            slResultT = clJsonNetworkP.value("bitrateNominal").toInt();
         }
      }
   }

   return (slResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::nominalBitrateString()                                                                        //
// return the nominal bit-rate of the current CAN interface                                                           //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanNetworkSettings::nominalBitrateString(void)
{
   QString  clResultT;
   int32_t  slBitrateT;

   slBitrateT = nominalBitrate();

   //---------------------------------------------------------------------------------------------------
   // Test for unknown bit-rate first
   //
   if (slBitrateT == eCAN_BITRATE_NONE)
   {
      clResultT = "None";
   }
   else
   {
      if (slBitrateT < 1000000)
      {
         //-----------------------------------------------------------------------------------
         // print values < 1000000 in kBit/s
         //
         clResultT = QString("%1 kBit/s").arg(slBitrateT / 1000);
      }
      else
      {
         //------------------------------------------------
         // print values >= 1000000 in MBit/s
         //
         clResultT = QString("%1 MBit/s").arg(slBitrateT / 1000000);
      }

   }
   return (clResultT);
}



//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::onSocketConnect()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetworkSettings::onSocketConnect(void)
{
   qDebug() << "QCanNetworkSettings::onSocketConnect() ";

   //---------------------------------------------------------------------------------------------------
   // keep connection state in member variable and signal it 
   //
   if (teServerStateP != QCanNetworkSettings::eSTATE_ACTIVE)
   {
         teServerStateP = QCanNetworkSettings::eSTATE_ACTIVE;
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::onSocketDisconnect()                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetworkSettings::onSocketDisconnect(void)
{
   qDebug() << "QCanNetworkSettings::onSocketDisconnect() ";

   //---------------------------------------------------------------------------------------------------
   // keep connection state in member variable and signal it 
   //
   if (teServerStateP != QCanNetworkSettings::eSTATE_INACTIVE)
   {
         teServerStateP = QCanNetworkSettings::eSTATE_INACTIVE;
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::onSocketError()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetworkSettings::onSocketError(QAbstractSocket::SocketError teSocketErrorV)
{
   State_e  teNewStateV = QCanNetworkSettings::eSTATE_UNKNOWN;

   qDebug() << "QCanNetworkSettings::onSocketError()" << teSocketErrorV;

   //---------------------------------------------------------------------------------------------------
   // connection state is modified depending on the SocketError value
   //
   switch(teSocketErrorV)
   {
      case QAbstractSocket::RemoteHostClosedError:
         teNewStateV = QCanNetworkSettings::eSTATE_CRASHED;
         break;

      case QAbstractSocket::ConnectionRefusedError:
         teNewStateV = QCanNetworkSettings::eSTATE_INACTIVE;
         break;

      default:
         teNewStateV = QCanNetworkSettings::eSTATE_INACTIVE;
         break;

   }

   //---------------------------------------------------------------------------------------------------
   // signal new connection state
   //
   if (teServerStateP != teNewStateV)
   {
         teServerStateP = teNewStateV;
   }

   pclWebSocketP->close(); // this might fail if the server is crashed
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::onSocketReceive()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetworkSettings::onSocketMessageReceived(const QString & clMessageR)
{
   qDebug() << "QCanNetworkSettings::onSocketMessageReceived()";

   //---------------------------------------------------------------------------------------------------
   // The message that we receive here is a JSON object, convert it into a JSON document first
   // and then check the validity.
   //
   QJsonParseError clJsonErrorT;
   QJsonDocument clJsonDocumentT = QJsonDocument::fromJson(clMessageR.toUtf8(), &clJsonErrorT);
   if ( (clJsonErrorT.error == QJsonParseError::NoError) && clJsonDocumentT.isObject())
   {
      //-------------------------------------------------------------------------------------------
      // Check bitrateData and bitrateNominal of JSON object
      //
      if (clJsonDocumentT.object().contains("bitrateData") && clJsonDocumentT.object().contains("bitrateNominal"))
      {
         clJsonNetworkP = clJsonDocumentT.object();
         emit objectReceived(teChannelP ,clJsonNetworkP);
      }
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::reset()                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetworkSettings::reset(void)
{
   //---------------------------------------------------------------------------------------------------
   // Update JSON object for commands to server
   //
   clJsonCommandP["reset"]                = (bool) true;

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::requestUpdate()                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetworkSettings::requestUpdate(void)
{

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::send()                                                                                        //
// send command as JSON object to connected QCanNetwork class                                                         //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanNetworkSettings::send(void)
{
   bool btResultT = false;
   
   if (teServerStateP == QCanNetworkSettings::eSTATE_ACTIVE)
   {
      //-------------------------------------------------------------------------------------------
      // create JSON document in text format and send it 
      //
      QJsonDocument clJsonDocumentT(clJsonCommandP);

      if (pclWebSocketP.isNull() == false)
      {
         pclWebSocketP->sendTextMessage(clJsonDocumentT.toJson());
      }
      btResultT = true;

      //-------------------------------------------------------------------------------------------
      // Remove keys from JSON document, this ensures that the same values are send again
      //
      clJsonCommandP.remove("bitrateData");
      clJsonCommandP.remove("bitrateNominal");
      clJsonCommandP.remove("mode");
      clJsonCommandP.remove("reset");


   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::setBitrate()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetworkSettings::setBitrate(int32_t slNomBitRateV, int32_t slDatBitRateV)
{
   //---------------------------------------------------------------------------------------------------
   // Update JSON object for commands to server
   //
   clJsonCommandP["bitrateData"]          = (int32_t) slDatBitRateV;
   clJsonCommandP["bitrateNominal"]       = (int32_t) slNomBitRateV;

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::setChannel()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetworkSettings::setChannel(const CAN_Channel_e teChannelV)
{ 
   teChannelP = teChannelV; 

   //---------------------------------------------------------------------------------------------------
   // Update JSON object for commands to server
   //
   clJsonCommandP["apiVersion"]           = "1.0";
   clJsonCommandP["channel"]              = (int32_t) teChannelP;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::setMode()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetworkSettings::setMode(const CAN_Mode_e teModeV)
{
   //---------------------------------------------------------------------------------------------------
   // Update JSON object for commands to server
   //
   clJsonCommandP["mode"]                 = (int32_t) teModeV;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::state()                                                                                       //
// return CAN network state                                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
CAN_State_e QCanNetworkSettings::state(void)
{
   CAN_State_e teResultT = eCAN_STATE_STOPPED;

   if (teServerStateP == QCanNetworkSettings::eSTATE_ACTIVE)
   {
      if (clJsonNetworkP.isEmpty() == false)
      {
         if (clJsonNetworkP.contains("state"))
         {
            teResultT = (CAN_State_e) clJsonNetworkP.value("state").toInt();
         }
      }
   }

   return (teResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::stateString()                                                                                 //
// return CAN network state                                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanNetworkSettings::stateString(void)
{
   QString clResultT;

   switch(state())
   {
      case eCAN_STATE_STOPPED:
         clResultT = "Stopped";
         break;

      case eCAN_STATE_SLEEPING:
         clResultT = "Sleeping";
         break;

      case eCAN_STATE_BUS_ACTIVE:
         clResultT = "Error active";
         break;

      case eCAN_STATE_BUS_WARN:
         clResultT = "Warning";
         break;

      case eCAN_STATE_BUS_PASSIVE:
         clResultT = "Error passive";
         break;

      case eCAN_STATE_BUS_OFF:
         clResultT = "Bus-off";
         break;

      default:
         clResultT = "Unknown";
         break;

   }

   return (clResultT);
}

