//====================================================================================================================//
// File:          qcan_network.cpp                                                                                    //
// Description:   QCAN classes - CAN network                                                                          //
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

#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

#include "qcan_defs.hpp"
#include "qcan_interface.hpp"
#include "qcan_network.hpp"



/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//------------------------------------------------------------------------------------------------------
// Defines the cycle time of the onTimerEvent() method
//
#define  REFRESH_TIMER_CYCLE_PERIOD          500



/*--------------------------------------------------------------------------------------------------------------------*\
** Static variables                                                                                                   **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/
uint8_t  QCanNetwork::ubNetIdP = 0;

static uint32_t   aulDlc2Bitlength[] = { 0,  8,  16,  24,  32,  40,  48,  56,
                                        64, 96, 128, 160, 192, 256, 384, 512  };

/*--------------------------------------------------------------------------------------------------------------------*\
** Static functions                                                                                                   **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// getBitrate()                                                                                                       //
// The functions converts the enumeration value CAN_Bitrate_e to a value using the unit [bit/s]                       //
//--------------------------------------------------------------------------------------------------------------------//

static int32_t getBitrate(int32_t slPreDefValueV)
{
   int32_t slBitrateT;

   switch (slPreDefValueV)
   {
      case eCAN_BITRATE_10K:
         slBitrateT = 10000;
         break;

      case eCAN_BITRATE_20K:
         slBitrateT = 20000;
         break;

      case eCAN_BITRATE_50K:
         slBitrateT = 50000;
         break;

      case eCAN_BITRATE_100K:
         slBitrateT = 100000;
         break;

      case eCAN_BITRATE_125K:
         slBitrateT = 125000;
         break;

      case eCAN_BITRATE_250K:
         slBitrateT = 250000;
         break;

      case eCAN_BITRATE_500K:
         slBitrateT = 500000;
         break;

      case eCAN_BITRATE_800K:
         slBitrateT = 800000;
         break;

      case eCAN_BITRATE_1M:
         slBitrateT = 1000000;
         break;

      case eCAN_BITRATE_2M:
         slBitrateT = 2000000;
         break;

      case eCAN_BITRATE_4M:
         slBitrateT = 4000000;
         break;

      case eCAN_BITRATE_5M:
         slBitrateT = 5000000;
         break;

      default:
         slBitrateT = eCAN_BITRATE_NONE;
         break;
   }

   return (slBitrateT);
}


/*--------------------------------------------------------------------------------------------------------------------*\
** Class methods                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork()                                                                                                      //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanNetwork::QCanNetwork(QObject * pclParentV)
{
   //---------------------------------------------------------------------------------------------------
   // set the parent
   //
   this->setParent(pclParentV);

   //---------------------------------------------------------------------------------------------------
   // each network has a unique network number, starting with 1
   //
   ubNetIdP++;
   ubIdP = ubNetIdP;

   pclInterfaceP.clear();

   qDebug() << "QCanNetwork(" << channel() << ") -----";

   //---------------------------------------------------------------------------------------------------
   // set default network name
   //
   clNetNameP = "CAN " + QString("%1").arg(ubNetIdP);

   //---------------------------------------------------------------------------------------------------
   // setup a new local server which is listening to the
   //
   pclLocalSrvP = new QLocalServer();

   //---------------------------------------------------------------------------------------------------
   // configure initial local socket list
   //
   clLocalSockListP.reserve(QCAN_LOCAL_SOCKET_MAX);


   //---------------------------------------------------------------------------------------------------
   // configure initial WebSocket list for CAN frames and settings
   //
   clWebSockListP.reserve(QCAN_WEB_SOCKET_MAX);
   clSettingsListP.reserve(QCAN_WEB_SOCKET_MAX);


   //---------------------------------------------------------------------------------------------------
   // clear statistic
   //
   ulCntFrameCanP = 0;
   ulCntFrameErrP = 0;
   ulCntBitCurP   = 0;

   ulFramePerSecMaxP = 0;
   ulFrameCntSaveP   = 0;

   ubBusLoadP = 0;

   //---------------------------------------------------------------------------------------------------
   // setup timing values
   //
   ulStatisticTimeP = 1000;

   btNetworkEnabledP       = false;
   btErrorFrameEnabledP    = false;
   btListenOnlyEnabledP    = false;
   btFlexibleDataEnabledP  = false;
   btBitrateChangeEnabledP = false;
   btTimeStampEnabledP     = true;

   //---------------------------------------------------------------------------------------------------
   // setup default bit-rate
   //
   slNomBitRateP = eCAN_BITRATE_NONE;
   slDatBitRateP = eCAN_BITRATE_NONE;

   //---------------------------------------------------------------------------------------------------
   // Set initial error state of an interface
   // it is in "Stopped" state because it is not enabled
   //
   teCanStateP   = eCAN_STATE_STOPPED;


   //---------------------------------------------------------------------------------------------------
   // configure the refresh timer which updates all statistic information and sends some signals
   //
   connect(&clRefreshTimerP, SIGNAL(timeout()), this, SLOT(onTimerEvent()));
   clRefreshTimerP.start(REFRESH_TIMER_CYCLE_PERIOD);

}


//--------------------------------------------------------------------------------------------------------------------//
// ~QCanNetwork()                                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanNetwork::~QCanNetwork()
{

   //---------------------------------------------------------------------------------------------------
   // clear list for local socket and web socket
   //
   clLocalSockListP.clear();
   clWebSockListP.clear();
   clSettingsListP.clear();

   //---------------------------------------------------------------------------------------------------
   // close local server
   //
   if (pclLocalSrvP->isListening())
   {
      pclLocalSrvP->close();
   }
   delete (pclLocalSrvP);


   //---------------------------------------------------------------------------------------------------
   // decrement internal network number
   //
   ubNetIdP--;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::addInterface()                                                                                        //
// add physical CAN interface (plug-in)                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanNetwork::addInterface(QCanInterface * pclCanIfV)
{
   bool  btResultT = false;

   if (pclInterfaceP.isNull())
   {
      pclInterfaceP = pclCanIfV;

      //---------------------------------------------------------------------------------------------------
      // connect message logging from CAN interface
      //
      connect(pclInterfaceP, SIGNAL(addLogMessage(QString, LogLevel_e)),
              this,          SLOT(onInterfaceLogMessage(QString, LogLevel_e) ));

      //---------------------------------------------------------------------------------------------------
      // connect signal for QCanInterface::connectionChanged from CAN interface
      //
      connect(pclInterfaceP, SIGNAL(connectionChanged(QCanInterface::ConnectionState_e)),
              this,          SLOT(onInterfaceConnectionChanged(QCanInterface::ConnectionState_e) ));

      //---------------------------------------------------------------------------------------------------
      // connect new data signal from CAN interface
      //
      connect(pclInterfaceP, SIGNAL(readyRead()),
              this,          SLOT(onInterfaceNewData() ));

      //---------------------------------------------------------------------------------------------------
      // connect state change from CAN interface
      //
      connect(pclInterfaceP, SIGNAL(stateChanged(CAN_State_e)),
              this,          SLOT(onInterfaceStateChange(CAN_State_e) ));


      btResultT = true;
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::attachWebSocket()                                                                                     //
// attach web socket to list                                                                                          //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::attachWebSocket(QWebSocket * pclSocketV, enum SocketType_e teSocketTypeV)
{

   if (teSocketTypeV == eSOCKET_TYPE_CAN_FRAME)
   {
      //-------------------------------------------------------------------------------------------
      // add this socket to the the socket list
      //
      clWebSockMutexP.lock();
      clWebSockListP.append(pclSocketV);
      clWebSockMutexP.unlock();

      //-------------------------------------------------------------------------------------------
      // Prepare log message and send it
      //
      emit showSocketState(channel(), clLocalSockListP.size(), clWebSockListP.size());
      logSocketState("Open WebSocket    -");

      //-------------------------------------------------------------------------------------------
      // Add slots that handle data reception and disconnection of the socket from the server
      //
      connect(pclSocketV, &QWebSocket::binaryMessageReceived,  this, &QCanNetwork::onWebSocketBinaryData);
      connect(pclSocketV, &QWebSocket::disconnected,           this, &QCanNetwork::onWebSocketDisconnect);

   }
   else
   {
      //-------------------------------------------------------------------------------------------
      // add this socket to the the socket list
      //
      clSettingsListP.append(pclSocketV);

      //-------------------------------------------------------------------------------------------
      // Prepare log message and send it
      //
      QString clSockOpenT = QString("total open: %1").arg(clSettingsListP.size());
      emit addLogMessage(CAN_Channel_e (id()), "Open WebSocket for settings - " + clSockOpenT, eLOG_LEVEL_DEBUG);

      //-------------------------------------------------------------------------------------------
      // Add slots that handle data reception and disconnection of the socket from the server
      //
      connect(pclSocketV, &QWebSocket::textMessageReceived,    this, &QCanNetwork::onWebSocketTextData);
      connect(pclSocketV, &QWebSocket::disconnected,           this, &QCanNetwork::onWebSocketDisconnect);
      sendNetworkSettings();
   }
   

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::dataBitrateString()                                                                                   //
// return QString value for data bit-rate                                                                             //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanNetwork::dataBitrateString(void)
{
   QString  clDatBitRateT;

   if (slDatBitRateP == eCAN_BITRATE_NONE)
   {
      clDatBitRateT = "None";
   }
   else
   {
      if (slDatBitRateP < 1000000)
      {
         //------------------------------------------------
         // print values < 1000000 in kBit/s
         //
         clDatBitRateT = QString("%1 kBit/s").arg(slDatBitRateP / 1000);
      }
      else
      {
         //------------------------------------------------
         // print values >= 1000000 in MBit/s
         //
         clDatBitRateT = QString("%1 MBit/s").arg(slDatBitRateP / 1000000);
      }
   }


   return (clDatBitRateT);

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::hasErrorFrameSupport()                                                                                //
// Check if the CAN interface has error frame support                                                                 //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanNetwork::hasErrorFrameSupport(void)
{
   bool btResultT;

   //---------------------------------------------------------------------------------------------------
   // If no physical CAN interface is connected, the virtual CAN network can support error frames.
   // Hence the default is TRUE.
   //
   btResultT = true;

   //---------------------------------------------------------------------------------------------------
   // Check supported features of physical CAN interface.
   //
   if (!pclInterfaceP.isNull())
   {
      if (pclInterfaceP->supportedFeatures() & QCAN_IF_SUPPORT_ERROR_FRAMES)
      {
         btResultT = true;
      }
      else
      {
         btResultT = false;
      }
   }

   return(btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::hasFlexibleDataSupport()                                                                              //
// Check if the CAN interface has CAN FD support                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanNetwork::hasFlexibleDataSupport(void)
{
   bool btResultT;

   //---------------------------------------------------------------------------------------------------
   // If no physical CAN interface is connected, the virtual CAN
   // network can support FD frames. Hence the default is TRUE.
   //
   btResultT = true;

   //---------------------------------------------------------------------------------------------------
   // Check supported features of physical CAN interface.
   //
   if (!pclInterfaceP.isNull())
   {
      if (pclInterfaceP->supportedFeatures() & QCAN_IF_SUPPORT_CAN_FD)
      {
         btResultT = true;
      }
      else
      {
         btResultT = false;
      }
   }

   return(btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::hasListenOnlySupport()                                                                                //
// Check if the CAN interface has Listen-Only support                                                                 //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanNetwork::hasListenOnlySupport(void)
{
   bool btResultT;

   //---------------------------------------------------------------------------------------------------
   // If no physical CAN interface is connected, the virtual CAN network can not support Listen-Only.
   // Hence the default is FALSE.
   //
   btResultT = false;


   if (!pclInterfaceP.isNull())
   {
      if (pclInterfaceP->supportedFeatures() & QCAN_IF_SUPPORT_LISTEN_ONLY)
      {
         btResultT = true;
      }
      else
      {
         btResultT = false;
      }
   }

   return(btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::hasSpecificConfigurationSupport()                                                                     //
// Check if the CAN interface has Devuce Specific Configuration support                                               //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanNetwork::hasSpecificConfigurationSupport(void)
{
   bool btResultT = false;

   if (!pclInterfaceP.isNull())
   {
      if (pclInterfaceP->supportedFeatures() & QCAN_IF_SUPPORT_SPECIFIC_CONFIG)
      {
         btResultT = true;
      }
   }

   return(btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::frameSize()                                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
uint32_t QCanNetwork::frameSize(const QByteArray & clSockDataR)
{
   uint32_t ulBitCountT = 0;

   //---------------------------------------------------------------------------------------------------
   // test for CAN data frame
   //
   if ((clSockDataR.at(0) & 0xE0) == 0x00)
   {
      //-------------------------------------------------------------------------------------------
      // check the DLC value and convert to the number of data bits inside this frame
      //
      ulBitCountT = aulDlc2Bitlength[(clSockDataR.at(4) & 0x0F)];

      //-------------------------------------------------------------------------------------------
      // add the number of bits for the protocol header, including possible stuff bits
      //
      switch (clSockDataR.at(5) & 0x03)
      {
         //------------------------------------------------
         // classical CAN, Standard Frame
         //
         case 0:
            ulBitCountT = ulBitCountT + 66;
            break;

         //------------------------------------------------
         // classical CAN, Extended Frame
         //
         case 1:
            ulBitCountT = ulBitCountT + 90;
            break;

         //------------------------------------------------
         // CAN FD, Standard Frame
         //
         case 2:
            ulBitCountT = ulBitCountT + 66;
            break;

         //------------------------------------------------
         // CAN FD, Extended Frame
         //
         case 3:
            ulBitCountT = ulBitCountT + 90;
            break;
      }
   }

   return (ulBitCountT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::handleCanFrame()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool  QCanNetwork::handleCanFrame(enum FrameSource_e teFrameSrcV, const int32_t slSockSrcV, QByteArray clSockDataV)
{
   int32_t        slSockIdxT;
   bool           btResultT = false;
   QLocalSocket * pclLocalSockT;
   QWebSocket *   pclWebSockT;

   //---------------------------------------------------------------------------------------------------
   // If a local time-stamp shall be set, do this here
   //
   if (btTimeStampEnabledP)
   {
      QByteArray clLocalTimeStampT = QCanTimeStamp::now().toByteArray();
      clSockDataV.replace(QCAN_FRAME_TIME_STAMP_POS, 8, clLocalTimeStampT);
   }  

   //---------------------------------------------------------------------------------------------------
   // If a CAN interface is present and the source of this data is not the CAN interface: convert to a
   // QCanFrame and write it to the interface
   //
   if ((pclInterfaceP.isNull() == false) && (teFrameSrcV != eFRAME_SOURCE_CAN_IF))
   {
      clCanFrameOutP.fromByteArray(clSockDataV);

      //-------------------------------------------------------------------------------------------
      // Check if it was possible to write the CAN message, if not we return immediately here.
      //
      if (pclInterfaceP->write(clCanFrameOutP) != QCanInterface::eERROR_NONE)
      {
         return (false);
      }

   }


   //---------------------------------------------------------------------------------------------------
   // check all open local sockets and write CAN frame
   //
   for (slSockIdxT = 0; slSockIdxT < clLocalSockListP.size(); slSockIdxT++)
   {
      //-------------------------------------------------------------------------------------------
      // If the frame source is a local socket AND the socket source value is equal to the socket
      // index: don't copy message
      //
      if ( (teFrameSrcV == eFRAME_SOURCE_LOCAL_SOCKET) && (slSockIdxT == slSockSrcV))
      {
         //-----------------------------------------------------------------------------------
         // do not copy data back to source
         //
      }
      else
      {
         //-----------------------------------------------------------------------------------
         // copy data to socket
         //
         pclLocalSockT = clLocalSockListP.at(slSockIdxT);
         pclLocalSockT->write(clSockDataV);
         btResultT = true;
      }
   }

   //---------------------------------------------------------------------------------------------------
   // check all open web sockets and write CAN frame
   //
   for (slSockIdxT = 0; slSockIdxT < clWebSockListP.size(); slSockIdxT++)
   {
      //-------------------------------------------------------------------------------------------
      // If the frame source is a web socket AND the socket source value is equal to the socket
      // index: don't copy message
      //
      if ( (teFrameSrcV == eFRAME_SOURCE_WEB_SOCKET) && (slSockIdxT == slSockSrcV))
      {
         //-----------------------------------------------------------------------------------
         // do not copy data back to source
         //
      }
      else
      {
         //-----------------------------------------------------------------------------------
         // copy data to socket
         //
         pclWebSockT = clWebSockListP.at(slSockIdxT);
         pclWebSockT->sendBinaryMessage(clSockDataV);
         pclWebSockT->flush();
         btResultT = true;
      }
   }


   //---------------------------------------------------------------------------------------------------
   // count frame
   //
   if ((clSockDataV.at(0) & 0x20) > 0)
   {
      ulCntFrameErrP++;
   }
   else
   {
      ulCntFrameCanP++;
   }
   ulCntBitCurP = ulCntBitCurP + frameSize(clSockDataV);

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::reset()                                                                                               //
// set all values to default / reset CAN interface                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::reset(void)
{

   //--------------------------------------------------------------------------------------
   // clear all counters
   //
   ulCntFrameCanP = 0;
   ulCntFrameErrP = 0;
   ulCntBitCurP   = 0;

   ulFramePerSecMaxP = 0;
   ulFrameCntSaveP   = 0;


   //--------------------------------------------------------------------------------------
   // the initial network state depends if it is enabled or not
   //
   if (btNetworkEnabledP)
   {
      setCanState(eCAN_STATE_BUS_ACTIVE);
   }
   else
   {
      setCanState(eCAN_STATE_STOPPED);
   }


   //--------------------------------------------------------------------------------------
   // If there is a physical CAN interface attached: reset it also
   //
   if (pclInterfaceP.isNull() == false)
   {
      pclInterfaceP->reset();
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::logSocketState()                                                                                      //
//return nominal bit-rate as QString value                                                                            //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::logSocketState(const QString & clInfoR)
{
   int32_t slLocalSocketNumT = clLocalSockListP.size();
   int32_t slWebSocketNumT   = clWebSockListP.size();

   QString clSockOpenT = QString(" total open: %1").arg(slLocalSocketNumT + slWebSocketNumT, 2);
   clSockOpenT += QString(" -  Local socket: %1").arg(slLocalSocketNumT, 2);
   clSockOpenT += QString(" -  WebSocket: %1").arg(slWebSocketNumT, 2);
   emit addLogMessage(channel(), clInfoR + clSockOpenT, eLOG_LEVEL_DEBUG);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::nominalBitrateString()                                                                                //
//return nominal bit-rate as QString value                                                                            //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanNetwork::nominalBitrateString(void)
{
   QString  clNomBitRateT;

   if (slNomBitRateP == eCAN_BITRATE_NONE)
   {
      clNomBitRateT = "None";
   }
   else
   {
      if (slNomBitRateP < 1000000)
      {
         //------------------------------------------------
         // print values < 1000000 in kBit/s
         //
         clNomBitRateT = QString("%1 kBit/s").arg(slNomBitRateP / 1000);
      }
      else
      {
         //------------------------------------------------
         // print values >= 1000000 in MBit/s
         //
         clNomBitRateT = QString("%1 MBit/s").arg(slNomBitRateP / 1000000);
      }
   }

   return (clNomBitRateT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::onInterfaceConnectionChanged()                                                                        //
// handle connection states of a physical CAN interface                                                               //
//--------------------------------------------------------------------------------------------------------------------//
void  QCanNetwork::onInterfaceConnectionChanged(const QCanInterface::ConnectionState_e & teConnectionStateR)
{
   switch(teConnectionStateR)
   {
      case QCanInterface::FailureState:
         stopInterface();
         removeInterface();
         break;

      default:

         break;
   }

   emit showInterfaceState(CAN_Channel_e (id()), teConnectionStateR);
}


//--------------------------------------------------------------------------------------------------------------------//
// onInterfaceLogMessage()                                                                                            //
// pass log messages from CAN interface to destination                                                                //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onInterfaceLogMessage(QString clMessageV, LogLevel_e teLogLevelV)
{
   emit addLogMessage(CAN_Channel_e (id()), clMessageV, teLogLevelV);
}


//--------------------------------------------------------------------------------------------------------------------//
// onInterfaceNewData()                                                                                               //
// handle new data from physical CAN interface                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onInterfaceNewData(void)
{
   QCanFrame      clCanFrameT;
   QByteArray     clSockDataT;

   //---------------------------------------------------------------------------------------------------
   // read messages from active CAN interface
   //
   if (pclInterfaceP.isNull() == false)
   {
      QCanInterface::InterfaceError_e teInterfaceStatusT = pclInterfaceP->read(clCanFrameT);
      while (teInterfaceStatusT == QCanInterface::eERROR_NONE)
      {
         //----------------------------------------------------------------------------------------
         // Convert QCanFrame to a byte array and pass this to the central message handler.
         // Make sure that the frame source is marked as "CAN interface", the parameter
         // "socket source" does not matter in this case, so we set it to 0 here.
         //
         clSockDataT = clCanFrameT.toByteArray();
         handleCanFrame(eFRAME_SOURCE_CAN_IF, 0, clSockDataT);

         teInterfaceStatusT = pclInterfaceP->read(clCanFrameT);
      }

      //-------------------------------------------------------------------------------------------
      // Test interface return value: a value less than QCanInterface::eERROR_NONE denotes a
      // hardware issue. The interface is removed here.
      //
      if (teInterfaceStatusT < QCanInterface::eERROR_NONE)
      {
         // onInterfaceDisconnect();
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// onInterfaceStateChange()                                                                                           //
// handle state change from physical CAN interface                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onInterfaceStateChange(CAN_State_e teStateV)
{
   QString  clLogMessageT;

   //---------------------------------------------------------------------------------------------------
   // Two state changes are critical:
   // - An interface in stopped state means a problem with the hardware
   // - An interface in bus-off state means a problem with the network
   //
   switch (teStateV)
   {
      case eCAN_STATE_STOPPED:
         clLogMessageT = "CAN interface stopped, check the hardware.";
         break;

      case eCAN_STATE_SLEEPING:
         clLogMessageT = "State changed to SLEEPING";
         break;

      case eCAN_STATE_BUS_ACTIVE :
         clLogMessageT = "State changed to ERROR ACTIVE";
         break;

      case eCAN_STATE_BUS_WARN :
         clLogMessageT = "State changed to BUS WARNING";
         break;

      case eCAN_STATE_BUS_PASSIVE :
         clLogMessageT = "State changed to BUS PASSIVE";
         break;

      case eCAN_STATE_BUS_OFF:
         clLogMessageT = "CAN interface is in bus-off condition.";
         break;

      default:

         break;
   }

   //---------------------------------------------------------------------------------------------------
   // store the new state
   //
   if (teCanStateP != teStateV)
   {
      addLogMessage(CAN_Channel_e (id()),  clLogMessageT, eLOG_LEVEL_INFO);

      setCanState(teStateV);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// onLocalSocketConnect()                                                                                             //
// slot that manages a new local server connection                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onLocalSocketConnect(void)
{
   QLocalSocket * pclSocketT;

   //---------------------------------------------------------------------------------------------------
   // Get next pending connect and add this socket to the the socket list
   //
   pclSocketT =  pclLocalSrvP->nextPendingConnection();
   clLocalSockMutexP.lock();
   clLocalSockListP.append(pclSocketT);
   clLocalSockMutexP.unlock();

   //---------------------------------------------------------------------------------------------------
   // Prepare log message and send it
   //
  emit showSocketState(channel(), clLocalSockListP.size(), clWebSockListP.size()); 
   logSocketState("Open LocalSocket  -");

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
void QCanNetwork::onLocalSocketDisconnect(void)
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
   for(slSockIdxT = 0; slSockIdxT < clLocalSockListP.size(); slSockIdxT++)
   {
      pclSockT = clLocalSockListP.at(slSockIdxT);
      if (pclSockT == pclSenderT)
      {
         clLocalSockListP.remove(slSockIdxT);
         break;
      }
   }
   clLocalSockMutexP.unlock();

   //---------------------------------------------------------------------------------------------------
   // Prepare log message and send it
   //
   emit showSocketState(channel(), clLocalSockListP.size(), clWebSockListP.size());
   logSocketState("Close LocalSocket -");
}


//--------------------------------------------------------------------------------------------------------------------//
// onLocalSocketNewData()                                                                                             //
// handle reception of new data on local socket                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onLocalSocketNewData(void)
{
   QLocalSocket *    pclLocalSockT = qobject_cast<QLocalSocket *>(sender());
   int32_t           slSockIdxT;
   int32_t           slListSizeT;
   uint32_t          ulFrameMaxT;
   static QByteArray clSockDataT;


   //---------------------------------------------------------------------------------------------------
   // lock socket list
   //
   clLocalSockMutexP.lock();


   //---------------------------------------------------------------------------------------------------
   // check all open local sockets and read messages
   //
   slListSizeT = clLocalSockListP.size();
   for(slSockIdxT = 0; slSockIdxT < slListSizeT; slSockIdxT++)
   {
      if (pclLocalSockT == clLocalSockListP.at(slSockIdxT))
      {
         ulFrameMaxT = (pclLocalSockT->bytesAvailable()) / QCAN_FRAME_ARRAY_SIZE;
         while (ulFrameMaxT > 0)
         {
            clSockDataT = pclLocalSockT->read(QCAN_FRAME_ARRAY_SIZE);
            handleCanFrame(eFRAME_SOURCE_LOCAL_SOCKET, slSockIdxT, clSockDataT);

            ulFrameMaxT = (pclLocalSockT->bytesAvailable()) / QCAN_FRAME_ARRAY_SIZE;
         }
      }
   }

   //---------------------------------------------------------------------------------------------------
   // unlock mutex
   //
   clLocalSockMutexP.unlock();
}



//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::onWebSocketBinaryData()                                                                               //
// handle reception of new data on web socket                                                                         //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onWebSocketBinaryData(const QByteArray &clMessageR)
{

   QWebSocket *   pclSocketT = qobject_cast<QWebSocket *>(sender());
   int32_t        slListSizeT;

   //---------------------------------------------------------------------------------------------------
   // lock web socket mutex
   //
   clWebSockMutexP.lock();

   //---------------------------------------------------------------------------------------------------
   // check all open web sockets and get socket index
   //
   slListSizeT = clWebSockListP.size();
   for(int32_t slSockIdxT = 0; slSockIdxT < slListSizeT; slSockIdxT++)
   {
      if (clWebSockListP.at(slSockIdxT) == pclSocketT)
      {
         handleCanFrame(eFRAME_SOURCE_WEB_SOCKET, slSockIdxT, clMessageR);
         break;
      }
   }

   //---------------------------------------------------------------------------------------------------
   // unlock web socket mutex
   //
   clWebSockMutexP.unlock();

}

//--------------------------------------------------------------------------------------------------------------------//
// onWebSocketDisconnect()                                                                                            //
// remove web socket from list                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onWebSocketDisconnect(void)
{
   int32_t      slSockIdxT;
   QWebSocket * pclSockT;
   QWebSocket * pclSenderT;


   //---------------------------------------------------------------------------------------------------
   // get sender of signal
   //
   pclSenderT = (QWebSocket* ) QObject::sender();

   clWebSockMutexP.lock();
   for(slSockIdxT = 0; slSockIdxT < clWebSockListP.size(); slSockIdxT++)
   {
      pclSockT = clWebSockListP.at(slSockIdxT);
      if(pclSockT == pclSenderT)
      {
         clWebSockListP.remove(slSockIdxT);
         break;
      }
   }
   clWebSockMutexP.unlock();

   //---------------------------------------------------------------------------------------------------
   // Prepare log message and send it
   //
   emit showSocketState(channel(), clLocalSockListP.size(), clWebSockListP.size());
   logSocketState("Close WebSocket   -");
}


//--------------------------------------------------------------------------------------------------------------------//
// onWebSocketTextData()                                                                                              //
// handle settings message (JSON)                                                                                     //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onWebSocketTextData(const QString &clMessageR)
{
   qDebug() << "QCanNetwork::onWebSocketTextData()" << clMessageR;
   bool btValidDataT = false;

   //---------------------------------------------------------------------------------------------------
   // The message that we receive here is a JSON object, convert it into a JSON document first
   // and then check the validity.
   //
   QJsonParseError clJsonErrorT;
   QJsonDocument clJsonDocumentT = QJsonDocument::fromJson(clMessageR.toUtf8(), &clJsonErrorT);
   if ( (clJsonErrorT.error == QJsonParseError::NoError) && clJsonDocumentT.isObject())
   {
      //-------------------------------------------------------------------------------------------
      // Check for "channel" inside JSON object, this must have the same value of this network
      // instance.
      //
      if (clJsonDocumentT.object().contains("channel"))
      {
         if (clJsonDocumentT.object().value("channel").toInt() == this->id())
         {
            btValidDataT = true;
         }
      }
   }

   //---------------------------------------------------------------------------------------------------
   // Execute command only if the data structure is valid
   //
   if (btValidDataT)
   {
      //-------------------------------------------------------------------------------------------
      // Check for "bitrateData" and "bitrateNominal" inside JSON object
      //
      if (clJsonDocumentT.object().contains("bitrateData") && clJsonDocumentT.object().contains("bitrateNominal"))
      {
         setBitrate( clJsonDocumentT.object().value("bitrateNominal").toInt(), 
                     clJsonDocumentT.object().value("bitrateData").toInt()    );
      }

      //-------------------------------------------------------------------------------------------
      // Check for "mode" inside JSON object
      //
      if (clJsonDocumentT.object().contains("mode"))
      {
         CAN_Mode_e  teModeT = (CAN_Mode_e) clJsonDocumentT.object().value("mode").toInt();
         switch(teModeT)
         {
            case eCAN_MODE_INIT:
               setNetworkEnabled(false);
               break;

            case eCAN_MODE_OPERATION:
               setNetworkEnabled(true);
               setListenOnlyEnabled(false);
               break;

            case eCAN_MODE_LISTEN_ONLY:
               setNetworkEnabled(true);
               setListenOnlyEnabled(true);
               break;

            default:

               break;

         }
      }

      //-------------------------------------------------------------------------------------------
      // Check for "reset" inside JSON object
      //
      if (clJsonDocumentT.object().contains("reset"))
      {
         if (clJsonDocumentT.object().value("reset").toBool())
         {
            reset();
         }
      }

   }
}



//--------------------------------------------------------------------------------------------------------------------//
// onTimerEvent()                                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onTimerEvent(void)
{
   uint32_t       ulMsgPerSecT;
   uint32_t       ulElapsedTimeT;


   //---------------------------------------------------------------------------------------------------
   // start statistic timer, if it is not already running
   //
   ulElapsedTimeT = (uint32_t) clStatisticTimeP.elapsed();
   if (ulElapsedTimeT > ulStatisticTimeP)
   {
      //--------------------------------------------------------------------------------------
      // signal current counter values
      //
      showCanFrames(CAN_Channel_e (id()), ulCntFrameCanP);
      showErrFrames(CAN_Channel_e (id()), ulCntFrameErrP);

      //--------------------------------------------------------------------------------------
      // calculate messages per second
      //
      ulMsgPerSecT = ulCntFrameCanP - ulFrameCntSaveP;
      ulMsgPerSecT = ulMsgPerSecT * 1000;
      ulMsgPerSecT = ulMsgPerSecT / ulElapsedTimeT;

      //--------------------------------------------------------------------------------------
      // calculate bus load
      //
      ulCntBitCurP = ulCntBitCurP * 100;
      ulCntBitCurP = ulCntBitCurP / slNomBitRateP;
      if(ulCntBitCurP > 100)
      {
         ulCntBitCurP = 100;
      }

      //--------------------------------------------------------------------------------------
      // signal bus load and msg/sec
      //
      ubBusLoadP = (uint8_t) ulCntBitCurP;
      showLoad(CAN_Channel_e (id()), ubBusLoadP, ulMsgPerSecT);
      ulCntBitCurP = 0;

      //--------------------------------------------------------------------------------------
      // store actual frame counter value
      //
      ulFrameCntSaveP = ulCntFrameCanP;

      //--------------------------------------------------------------------------------------
      // set new value for start time
      //
      clStatisticTimeP.restart();
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// removeInterface()                                                                                                  //
// remove physical CAN interface (plug-in)                                                                            //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::removeInterface(void)
{
   //---------------------------------------------------------------------------------------------------
   // disconnect all signals from CAN interface to network
   //
   if (!pclInterfaceP.isNull())
   {
      disconnect(pclInterfaceP, 0, 0, 0);

      pclInterfaceP.clear();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::sendNetworkSettings                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void  QCanNetwork::sendNetworkSettings(uint32_t flags)
{
   Q_UNUSED(flags);

   //---------------------------------------------------------------------------------------------------
   // Create JSON object containing all information about the network
   //
   QJsonObject clJsonNetworkT;
   
   clJsonNetworkT["apiVersion"]           = "1.0";
   clJsonNetworkT["channel"]              = (int32_t) this->channel();
   clJsonNetworkT["bitrateData"]          = (int32_t) this->dataBitrate();
   clJsonNetworkT["bitrateNominal"]       = (int32_t) this->nominalBitrate();
   clJsonNetworkT["enabled"]              = (bool)    this->isNetworkEnabled();
   clJsonNetworkT["errorFrameEnabled"]    = (bool)    this->isErrorFrameEnabled();
   clJsonNetworkT["errorFrameSupport"]    = (bool)    this->hasErrorFrameSupport();
   clJsonNetworkT["flexibleDataEnabled"]  = (bool)    this->isFlexibleDataEnabled();
   clJsonNetworkT["flexibleDataSupport"]  = (bool)    this->hasFlexibleDataSupport();
   clJsonNetworkT["frameCount"]           = (int32_t) this->frameCount();
   clJsonNetworkT["frameCountError"]      = (int32_t) this->frameCountError();
   clJsonNetworkT["listenOnlyEnabled"]    = (bool)    this->isErrorFrameEnabled();
   clJsonNetworkT["listenOnlySupport"]    = (bool)    this->isListenOnlyEnabled();
   clJsonNetworkT["name"]                 = (QString) this->name();
   clJsonNetworkT["state"]                = (int32_t) this->state();

   if (pclInterfaceP.isNull() == false)
   {
      clJsonNetworkT["interfaceName"]     = (QString) pclInterfaceP->name();
      clJsonNetworkT["interfaceVersion"]  = (QString) pclInterfaceP->version();
   }
   else
   {
      clJsonNetworkT["interfaceName"]     = "Virtual CAN";
   }
   
   
   //---------------------------------------------------------------------------------------------------
   // create JSON document in text format and send it via all open sockets
   //
   QJsonDocument clJsonDocumentT(clJsonNetworkT);

   for (int32_t slListIndexT = 0; slListIndexT < clSettingsListP.size(); slListIndexT++)
   {
      clSettingsListP.at(slListIndexT)->sendTextMessage(clJsonDocumentT.toJson());
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::setBitrate                                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::setBitrate(int32_t slNomBitRateV, int32_t slDatBitRateV)
{
   //---------------------------------------------------------------------------------------------------
   // Test for pre-defined values from enumeration CAN_Bitrate_e first and convert them in "real"
   // bit-rate values
   //
   if (slNomBitRateV < eCAN_BITRATE_MAX)
   {
      slNomBitRateV = getBitrate(slNomBitRateV);
   }

   if (slDatBitRateV < eCAN_BITRATE_MAX)
   {
      slDatBitRateV = getBitrate(slDatBitRateV);
   }

   qDebug() << "QCanNetwork::setBitrate()" << slNomBitRateV << slDatBitRateV;

   //---------------------------------------------------------------------------------------------------
   // test if the new values differ from the currently stored values
   //
   if ( (slNomBitRateV != slNomBitRateP) || (slDatBitRateP != slDatBitRateV) )
   {
      //-------------------------------------------------------------------------------------------
      // Store new bit-rates:
      // If there is no CAN FD support, the data bit-rate will be set to eCAN_BITRATE_NONE
      //
      slNomBitRateP  = slNomBitRateV;
      if(btFlexibleDataEnabledP)
      {
         slDatBitRateP  = slDatBitRateV;
      }
      else
      {
         slDatBitRateP  = eCAN_BITRATE_NONE;
      }

      //-------------------------------------------------------------------------------------------
      // If there is an active CAN interface, configure the new bit-rate
      //
      if (!pclInterfaceP.isNull())
      {
         pclInterfaceP->setMode(eCAN_MODE_INIT);
         pclInterfaceP->setBitrate(slNomBitRateP, slDatBitRateP);
         pclInterfaceP->setMode(eCAN_MODE_OPERATION);
      }

      //-------------------------------------------------------------------------------------------
      // show log message
      //
      if (btFlexibleDataEnabledP)
      {
         addLogMessage(CAN_Channel_e (id()),
                       "Set nominal bit-rate " + nominalBitrateString() +
                       " and data bit-rate " + dataBitrateString(),
                       eLOG_LEVEL_INFO);
      }
      else
      {
         addLogMessage(CAN_Channel_e (id()),
                       "Set bit-rate " + nominalBitrateString(),
                       eLOG_LEVEL_INFO);
      }

      //-------------------------------------------------------------------------------------------
      // signal that bit-rate has been changed
      //
      emit showBitrate(CAN_Channel_e (id()), slNomBitRateP, slDatBitRateP);
   }

}



//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::setCanState()                                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::setCanState(CAN_State_e teStateV)
{
   teCanStateP = teStateV;

   qDebug() << "QCanNetwork::setCanState(" << teStateV << ")";

   //---------------------------------------------------------------------------------------------------
   // signal the new state to destination
   //
   emit showState(CAN_Channel_e (id()), teCanStateP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::setErrorFrameEnabled()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::setErrorFrameEnabled(bool btEnableV)
{
   qDebug() << "QCanNetwork::setErrorFrameEnabled(" << btEnableV << ")";


   //---------------------------------------------------------------------------------------------------
   // Test if error frame support is available before setting the private member
   //
   if (hasErrorFrameSupport() == true)
   {
      btErrorFrameEnabledP = btEnableV;
   }
   else
   {
      btErrorFrameEnabledP = false;
   }

   //---------------------------------------------------------------------------------------------------
   // If a CAN interface is attached to the network perform the setup
   //
   if (btErrorFrameEnabledP)
   {
      if (!pclInterfaceP.isNull())
      {
         pclInterfaceP->enableFeatures(QCAN_IF_SUPPORT_ERROR_FRAMES);
      }
      addLogMessage(CAN_Channel_e (id()), "CAN error frame support enabled",  eLOG_LEVEL_INFO);
   }
   else
   {
      if (!pclInterfaceP.isNull())
      {
         pclInterfaceP->disableFeatures(QCAN_IF_SUPPORT_ERROR_FRAMES);
      }
      addLogMessage(CAN_Channel_e (id()), "CAN error frame support disabled", eLOG_LEVEL_INFO);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::setFlexibleDataEnabled()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::setFlexibleDataEnabled(bool btEnableV)
{
   //---------------------------------------------------------------------------------------------------
   // Test if FD support is available before setting the private member
   //
   if (hasFlexibleDataSupport() == true)
   {
      btFlexibleDataEnabledP = btEnableV;
   }
   else
   {
      btFlexibleDataEnabledP = false;
   }

   //---------------------------------------------------------------------------------------------------
   // If a CAN interface is attached to the network perform the setup
   //
   if (btFlexibleDataEnabledP)
   {
      if (!pclInterfaceP.isNull())
      {
         pclInterfaceP->enableFeatures(QCAN_IF_SUPPORT_CAN_FD);
      }
      addLogMessage(CAN_Channel_e (id()), "CAN FD support enabled", eLOG_LEVEL_INFO);
   }
   else
   {
      if (!pclInterfaceP.isNull())
      {
         pclInterfaceP->disableFeatures(QCAN_IF_SUPPORT_CAN_FD);
      }
      addLogMessage(CAN_Channel_e (id()), "CAN FD support disabled", eLOG_LEVEL_INFO);
   }


}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::setInterfaceConfiguration()                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::setInterfaceConfiguration()
{
   //---------------------------------------------------------------------------------------------------
   // If there is an active CAN interface, call additional configuration
   //
   if (!pclInterfaceP.isNull())
   {
      pclInterfaceP->configureDevice();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::setListenOnlyEnabled()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::setListenOnlyEnabled(bool btEnableV)
{
   //---------------------------------------------------------------------------------------------------
   // Test if listen-only support is available before setting the private member
   //
   if (hasListenOnlySupport() == true)
   {
      btListenOnlyEnabledP = btEnableV;
   }
   else
   {
      btListenOnlyEnabledP = false;
   }

   //---------------------------------------------------------------------------------------------------
   // If a CAN interface is attached to the network perform the setup
   //
   if (btListenOnlyEnabledP)
   {
      if (!pclInterfaceP.isNull())
      {
         pclInterfaceP->setMode(eCAN_MODE_LISTEN_ONLY);
      }
      addLogMessage(CAN_Channel_e (id()), "Listen-only support enabled", eLOG_LEVEL_INFO);
   }
   else
   {
      if (!pclInterfaceP.isNull())
      {
         pclInterfaceP->setMode(eCAN_MODE_OPERATION);
      }
      addLogMessage(CAN_Channel_e (id()), "Listen-only support disabled", eLOG_LEVEL_INFO);
   }


}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork::setNetworkEnabled()                                                                                   //
// start / stop the server                                                                                            //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::setNetworkEnabled(bool btEnableV)
{

   if ((btEnableV == true) && (btNetworkEnabledP == false))
   {
      //-------------------------------------------------------------------------------------------
      // limit the number of connections for local server
      //
      pclLocalSrvP->setMaxPendingConnections(QCAN_LOCAL_SOCKET_MAX);
      pclLocalSrvP->setSocketOptions(QLocalServer::WorldAccessOption);
      if(!pclLocalSrvP->listen(QString("CANpieServerChannel%1").arg(ubIdP)))
      {
         btNetworkEnabledP =  false;

         addLogMessage(CAN_Channel_e (id()),
                       "Failed to open local server ", eLOG_LEVEL_ERROR);

         return;
      }
      addLogMessage(CAN_Channel_e (id()),
                    pclLocalSrvP->fullServerName(), eLOG_LEVEL_DEBUG);

      //-------------------------------------------------------------------------------------------
      // a new connection is handled by the onLocalSocketConnect() method
      //
      connect( pclLocalSrvP, SIGNAL(newConnection()),
               this, SLOT(onLocalSocketConnect()));



      //-------------------------------------------------------------------------------------------
      // set flag for further operations
      //
      btNetworkEnabledP =  true;

      //-------------------------------------------------------------------------------------------
      // set initial error state of an interface
      //
      setCanState(eCAN_STATE_BUS_ACTIVE);

      clStatisticTimeP.start();

      //-------------------------------------------------------------------------------------------
      // send log message
      //
      addLogMessage(CAN_Channel_e (id()), "Network enabled", eLOG_LEVEL_INFO);


   }

   if ((btEnableV == false) && (btNetworkEnabledP == true))
   {
      //-------------------------------------------------------------------------------------------
      // remove signal / slot connections
      //
      disconnect( pclLocalSrvP, SIGNAL(newConnection()),
                  this, SLOT(onLocalSocketConnect()));



      //-------------------------------------------------------------------------------------------
      // close local server
      //
      pclLocalSrvP->close();

      //-------------------------------------------------------------------------------------------
      // set flag for further operations
      //
      btNetworkEnabledP =  false;

      //-------------------------------------------------------------------------------------------
      // set error state of an interface
      //
      setCanState(eCAN_STATE_STOPPED);


      //-------------------------------------------------------------------------------------------
      // send log message
      //
      addLogMessage(CAN_Channel_e (id()), "Network disabled", eLOG_LEVEL_INFO);

   }


}


//--------------------------------------------------------------------------------------------------------------------//
// startInterface()                                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanNetwork::startInterface(void)
{
   bool  btResultT = false;

   if (!pclInterfaceP.isNull())
   {
      //-------------------------------------------------------------------------------------------
      // connect the interface
      //
      if (pclInterfaceP->connect() == QCanInterface::eERROR_NONE)
      {
         if (pclInterfaceP->setBitrate(slNomBitRateP, slDatBitRateP) == QCanInterface::eERROR_NONE)
         {
            if (pclInterfaceP->setMode(eCAN_MODE_OPERATION) == QCanInterface::eERROR_NONE)
            {

               addLogMessage(CAN_Channel_e (id()),
                             "Start CAN interface .... : success",
                             eLOG_LEVEL_INFO);

               btResultT = true;
            }
            else
            {
               addLogMessage(CAN_Channel_e (id()),
                             "Start CAN interface .... : failed",
                             eLOG_LEVEL_ERROR);
            }
         }
         else
         {
            addLogMessage(CAN_Channel_e (id()),
                          "Start CAN interface .... : failed to configure bit-rate",
                          eLOG_LEVEL_ERROR);
         }
      }

   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// stopInterface()                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanNetwork::stopInterface(void)
{
   bool  btResultT = false;

   if (pclInterfaceP.isNull() == false)
   {
      pclInterfaceP->setMode(eCAN_MODE_INIT);
      addLogMessage(CAN_Channel_e (id()),
                    "Remove CAN interface ... : " + pclInterfaceP->name(),
                    eLOG_LEVEL_INFO);


      if (pclInterfaceP->connectionState() != QCanInterface::UnconnectedState)
      {
         pclInterfaceP->disconnect();
         btResultT = true;
      }
   }

   return (btResultT);
}
