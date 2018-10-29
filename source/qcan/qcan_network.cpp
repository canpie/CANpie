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

#include "qcan_defs.hpp"
#include "qcan_interface.hpp"
#include "qcan_network.hpp"

#include "qcan_server_memory.hpp"

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define  REFRESH_TIMER_CYCLE_PERIOD          500



/*----------------------------------------------------------------------------*\
** Static variables                                                           **
**                                                                            **
\*----------------------------------------------------------------------------*/
uint8_t  QCanNetwork::ubNetIdP = 0;

static uint32_t   aulDlc2Bitlength[] = { 0,  8,  16,  24,  32,  40,  48,  56, 
                                        64, 96, 128, 160, 192, 256, 384, 512  };

/*----------------------------------------------------------------------------*\
** Static functions                                                           **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// getBitrate()                                                               //
// The functions converts the enumeration value CAN_Bitrate_e to a value      //                                                                 
// using the unit [bit/s]                                                     //                      
//----------------------------------------------------------------------------//
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

/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork()                                                                                                      //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanNetwork::QCanNetwork(QObject * pclParentV, uint16_t  uwPortV, QSharedMemory * pclSettingsV)
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
   // create initial local socket list
   //
   pclLocalSockListP = new QVector<QLocalSocket*>;
   pclLocalSockListP->reserve(QCAN_LOCAL_SOCKET_MAX);


   //---------------------------------------------------------------------------------------------------
   // setup a new TCP server which is listening to the default network name
   //
   pclTcpSrvP = new QTcpServer();
   clTcpHostAddrP = QHostAddress(QHostAddress::LocalHost);
   uwTcpPortP = uwPortV;

   //---------------------------------------------------------------------------------------------------
   // create initial TCP socket list
   //
   pclTcpSockListP = new QVector<QTcpSocket *>;
   pclTcpSockListP->reserve(QCAN_TCP_SOCKET_MAX);


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
   btBitrateFrameEnabledP  = false;

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


   //---------------------------------------------------------------------------------------------------
   // store initial values in shared memory
   //
   pclSettingsP = pclSettingsV;
   if ((pclSettingsP != 0L) && pclSettingsP->isAttached() )
   {
      qDebug() << "QCanNetwork(" << channel() << ") -- Set default values in shared memory";
      pclSettingsP->lock();
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

      ptsSettingsT->atsNetwork[ubIdP - 1].slNomBitRate = slNomBitRateP;
      ptsSettingsT->atsNetwork[ubIdP - 1].slDatBitRate = slDatBitRateP;
      ptsSettingsT->atsNetwork[ubIdP - 1].slStatus     = teCanStateP;
      strcpy(&(ptsSettingsT->atsNetwork[ubIdP - 1].szInterfaceName[0]), "Virtual CAN");
      pclSettingsP->unlock();
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// ~QCanNetwork()                                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanNetwork::~QCanNetwork()
{

   //---------------------------------------------------------------------------------------------------
   // close local server
   //
   if (pclLocalSrvP->isListening())
   {
      pclLocalSrvP->close();
   }
   delete (pclLocalSrvP);

   //---------------------------------------------------------------------------------------------------
   // close TCP server
   //
   if (pclTcpSrvP->isListening())
   {
      pclTcpSrvP->close();
   }
   delete (pclTcpSrvP);

   ubNetIdP--;
}


//--------------------------------------------------------------------------------------------------------------------//
// addInterface()                                                                                                     //
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

      //---------------------------------------------------------------------------------------------------
      // store interface name in shared memory
      //
      if ((pclSettingsP != 0L) && pclSettingsP->isAttached() )
      {
         pclSettingsP->lock();
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         int32_t slStringSizeT = pclCanIfV->name().size();
         if (slStringSizeT > QCAN_IF_NAME_LENGTH)
         {
            slStringSizeT = QCAN_IF_NAME_LENGTH;
         }

         for (int32_t slCntT = 0; slCntT < slStringSizeT; slCntT++)
         {
            ptsSettingsT->atsNetwork[ubIdP - 1].szInterfaceName[slCntT] = pclCanIfV->name().at(slCntT).toLatin1();
         }

         pclSettingsP->unlock();
      }

      btResultT = true;
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// dataBitrateString()                                                                                                //
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

//----------------------------------------------------------------------------//
// hasErrorFrameSupport()                                                    //
// Check if the CAN interface has error frame support                         //
//----------------------------------------------------------------------------//
bool QCanNetwork::hasErrorFrameSupport(void)
{
   bool btResultT;
   
   //----------------------------------------------------------------
   // If no physical CAN interface is connected, the virtual CAN 
   // network can support error frames. Hence the default is TRUE.
   //
   btResultT = true;  
   
   //----------------------------------------------------------------
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


//----------------------------------------------------------------------------//
// hasFlexibleDataSupport()                                                   //
// Check if the CAN interface has CAN FD support                              //
//----------------------------------------------------------------------------//
bool QCanNetwork::hasFlexibleDataSupport(void)
{
   bool btResultT;
   
   //----------------------------------------------------------------
   // If no physical CAN interface is connected, the virtual CAN 
   // network can support FD frames. Hence the default is TRUE.
   //
   btResultT = true;  
   
   //----------------------------------------------------------------
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


//----------------------------------------------------------------------------//
// hasListenOnlySupport()                                                     //
// Check if the CAN interface has Listen-Only support                         //
//----------------------------------------------------------------------------//
bool QCanNetwork::hasListenOnlySupport(void)
{
   bool btResultT;
   
   //----------------------------------------------------------------
   // If no physical CAN interface is connected, the virtual CAN 
   // network can not support Listen-Only. Hence the default is FALSE.
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

//----------------------------------------------------------------------------//
// hasSpecificConfigurationSupport()                                          //
// Check if the CAN interface has Devuce Specific Configuration support       //
//----------------------------------------------------------------------------//
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


//----------------------------------------------------------------------------//
// frameSize()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
uint32_t QCanNetwork::frameSize(const QByteArray & clSockDataR)
{
   uint32_t ulBitCountT = 0;
   
   //----------------------------------------------------------------
   // test for CAN data frame
   //
   if ((clSockDataR.at(0) & 0xE0) == 0x00)
   {
      //--------------------------------------------------------
      // check the DLC value and convert to the number of
      // data bits inside this frame
      //
      ulBitCountT = aulDlc2Bitlength[(clSockDataR.at(4) & 0x0F)];

      //--------------------------------------------------------
      // add the number of bits for the protocol header, 
      // including possible stuff bits
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
bool  QCanNetwork::handleCanFrame(enum FrameSource_e teFrameSrcV, const int32_t slSockSrcV, const QByteArray clSockDataV)
{
   int32_t        slSockIdxT;
   bool           btResultT = false;
   QLocalSocket * pclLocalSockS;
   QTcpSocket *   pclTcpSockS;

   //---------------------------------------------------------------------------------------------------
   // If a CAN interface is present and the source of this data is not the CAN interface: convert to a
   // QCanFrame and write it to the interface
   //
   if ((pclInterfaceP.isNull() == false) && (teFrameSrcV != eFRAME_SOURCE_CAN_IF))
   {
      clCanFrameOutP.fromByteArray(clSockDataV);
      pclInterfaceP->write(clCanFrameOutP);
   }


   //---------------------------------------------------------------------------------------------------
   // check all open local sockets and write CAN frame
   //
   for (slSockIdxT = 0; slSockIdxT < pclLocalSockListP->size(); slSockIdxT++)
   {
      //-------------------------------------------------------------------------------------------
      // If the frame source is a local socket AND the socket source value is equal to the socket
      // index: don't copy message
      //
      if ( (teFrameSrcV == eFRAME_SOURCE_SOCKET_LOCAL) && (slSockIdxT == slSockSrcV))
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
         pclLocalSockS = pclLocalSockListP->at(slSockIdxT);
         pclLocalSockS->write(clSockDataV);
         btResultT = true;
      }
   }

   //---------------------------------------------------------------------------------------------------
   // check all open TCP sockets and write CAN frame
   //
   for (slSockIdxT = 0; slSockIdxT < pclTcpSockListP->size(); slSockIdxT++)
   {
      //-------------------------------------------------------------------------------------------
      // If the frame source is a TCP socket AND the socket source value is equal to the socket
      // index: don't copy message
      //
      if ( (teFrameSrcV == eFRAME_SOURCE_SOCKET_TCP) && (slSockIdxT == slSockSrcV))
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
         pclTcpSockS = pclTcpSockListP->at(slSockIdxT);
         pclTcpSockS->write(clSockDataV);
         pclTcpSockS->flush();
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

   return(btResultT);
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
// QCanNetwork::serverAddress()                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QHostAddress QCanNetwork::serverAddress(void)
{
   qDebug() << "QCanNetwork::serverAddress" << pclTcpSrvP->serverAddress();
   return (pclTcpSrvP->serverAddress());
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
   pclLocalSockListP->append(pclSocketT);
   clLocalSockMutexP.unlock();

   //---------------------------------------------------------------------------------------------------
   // Prepare log message and send it
   //
   emit showSocketState(channel(), pclLocalSockListP->size(), pclTcpSockListP->size());
   QString clSockOpenT = QString("total open: %1").arg(pclLocalSockListP->size() + pclTcpSockListP->size());
   clSockOpenT += QString(" (Local: %1,").arg(pclLocalSockListP->size());
   clSockOpenT += QString(" TCP: %1)   ").arg(pclTcpSockListP->size());
   emit addLogMessage(channel(),
                      "Local socket connected, " + clSockOpenT,
                      eLOG_LEVEL_DEBUG);

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

   //---------------------------------------------------------------------------------------------------
   // Prepare log message and send it
   //
   emit showSocketState(channel(), pclLocalSockListP->size(), pclTcpSockListP->size());
   QString clSockOpenT = QString("total open: %1").arg(pclLocalSockListP->size() + pclTcpSockListP->size());
   clSockOpenT += QString(" (Local: %1,").arg(pclLocalSockListP->size());
   clSockOpenT += QString(" TCP: %1)   ").arg(pclTcpSockListP->size());
   emit addLogMessage(channel(),
                      "Local socket disconnected, " + clSockOpenT,
                      eLOG_LEVEL_DEBUG);
}


//--------------------------------------------------------------------------------------------------------------------//
// onLocalSocketNewData()                                                                                             //
// handle reception of new data on local socket                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onLocalSocketNewData(void)
{
   QLocalSocket *    pclLocalSockT;
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
   slListSizeT = pclLocalSockListP->size();
   for(slSockIdxT = 0; slSockIdxT < slListSizeT; slSockIdxT++)
   {
      pclLocalSockT = pclLocalSockListP->at(slSockIdxT);
      ulFrameMaxT = (pclLocalSockT->bytesAvailable()) / QCAN_FRAME_ARRAY_SIZE;
      while (ulFrameMaxT > 0)
      {
         clSockDataT = pclLocalSockT->read(QCAN_FRAME_ARRAY_SIZE);
         handleCanFrame(eFRAME_SOURCE_SOCKET_LOCAL, slSockIdxT, clSockDataT);

         ulFrameMaxT = (pclLocalSockT->bytesAvailable()) / QCAN_FRAME_ARRAY_SIZE;
      }
   }

   //---------------------------------------------------------------------------------------------------
   // unlock mutex
   //
   clLocalSockMutexP.unlock();
}


//--------------------------------------------------------------------------------------------------------------------//
// onTcpSocketConnect()                                                                                               //
// slot that manages a new TCP server connection                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onTcpSocketConnect(void)
{
   QTcpSocket *   pclSocketT;
   
   //----------------------------------------------------------------
   // Get next pending connect and add this socket to the
   // the socket list
   //
   pclSocketT =  pclTcpSrvP->nextPendingConnection();
   clTcpSockMutexP.lock();
   pclTcpSockListP->append(pclSocketT);
   clTcpSockMutexP.unlock();

   //----------------------------------------------------------------
   // Prepare log message and send it
   //
   QString clSockOpenT = QString("total open: %1").arg(pclLocalSockListP->size() + pclTcpSockListP->size());
   clSockOpenT += QString(" (Local: %1),").arg(pclLocalSockListP->size());
   clSockOpenT += QString(" (TCP: %1)   ").arg(pclTcpSockListP->size());
   emit addLogMessage(CAN_Channel_e (id()),
                      "Socket connected, " + clSockOpenT,
                      eLOG_LEVEL_DEBUG);

   //----------------------------------------------------------------
   // Add a slot that handles the disconnection of the socket
   // from the local server
   //
   connect( pclSocketT,
            SIGNAL(disconnected()),
            this,
            SLOT(onTcpSocketDisconnect())   );
   
   //----------------------------------------------------------------
   // Todo: Send information to client:
}


//----------------------------------------------------------------------------//
// onTcpSocketDisconnect()                                                    //
// remove local socket from list                                              //
//----------------------------------------------------------------------------//
void QCanNetwork::onTcpSocketDisconnect(void)
{
   int32_t      slSockIdxT;
   QTcpSocket * pclSockT;
   QTcpSocket * pclSenderT;


   //----------------------------------------------------------------
   // get sender of signal
   //
   pclSenderT = (QTcpSocket* ) QObject::sender();

   clTcpSockMutexP.lock();
   for(slSockIdxT = 0; slSockIdxT < pclTcpSockListP->size(); slSockIdxT++)
   {
      pclSockT = pclTcpSockListP->at(slSockIdxT);
      if(pclSockT == pclSenderT)
      {
         pclTcpSockListP->remove(slSockIdxT);
         break;
      }
   }
   clTcpSockMutexP.unlock();

   //----------------------------------------------------------------
   // Prepare log message and send it
   //
   QString clSockOpenT = QString("total open: %1").arg(pclTcpSockListP->size());
   emit addLogMessage(CAN_Channel_e (id()),
                      "Socket disconnect, " + clSockOpenT,
                      eLOG_LEVEL_DEBUG);
}


//--------------------------------------------------------------------------------------------------------------------//
// onTcpSocketNewData()                                                                                               //
// handle reception of new data on TCP socket                                                                         //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onTcpSocketNewData()
{
   QTcpSocket *   pclTcpSockT;
   int32_t        slSockIdxT;
   int32_t        slListSizeT;
   uint32_t       ulFrameMaxT;
   QByteArray     clSockDataT;


   //---------------------------------------------------------------------------------------------------
   // lock socket list
   //
   clTcpSockMutexP.lock();


   //---------------------------------------------------------------------------------------------------
   // check all open TCP sockets and read messages
   //
   slListSizeT = pclTcpSockListP->size();
   for(slSockIdxT = 0; slSockIdxT < slListSizeT; slSockIdxT++)
   {
      pclTcpSockT = pclTcpSockListP->at(slSockIdxT);
      ulFrameMaxT = (pclTcpSockT->bytesAvailable()) / QCAN_FRAME_ARRAY_SIZE;
      while (ulFrameMaxT > 0)
      {
         clSockDataT = pclTcpSockT->read(QCAN_FRAME_ARRAY_SIZE);
         handleCanFrame(eFRAME_SOURCE_SOCKET_TCP, slSockIdxT, clSockDataT);
         ulFrameMaxT = (pclTcpSockT->bytesAvailable()) / QCAN_FRAME_ARRAY_SIZE;

      }
   }


   //---------------------------------------------------------------------------------------------------
   // unlock mutex
   //
   clTcpSockMutexP.unlock();

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

      //-------------------------------------------------------------------------------------------
      // store interface name in shared memory
      //
      if ((pclSettingsP != 0L) && pclSettingsP->isAttached() )
      {
         pclSettingsP->lock();
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         strcpy(&(ptsSettingsT->atsNetwork[ubIdP - 1].szInterfaceName[0]), "Virtual CAN");

         pclSettingsP->unlock();
      }

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
      // store bit-rate values in shared memory
      //
      if ((pclSettingsP != 0L) && pclSettingsP->isAttached() )
      {
         pclSettingsP->lock();
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         ptsSettingsT->atsNetwork[ubIdP - 1].slNomBitRate = slNomBitRateP;
         ptsSettingsT->atsNetwork[ubIdP - 1].slDatBitRate = slDatBitRateP;
         pclSettingsP->unlock();
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

   //---------------------------------------------------------------------------------------------------
   // store value in shared memory
   //
   if ((pclSettingsP != 0L) && pclSettingsP->isAttached() )
   {
      pclSettingsP->lock();
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

      ptsSettingsT->atsNetwork[ubIdP - 1].slStatus = teCanStateP;
      pclSettingsP->unlock();
   }

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
      pclLocalSrvP->setMaxPendingConnections(QCAN_TCP_SOCKET_MAX);
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
      // limit the number of connections for TCP server
      //
      pclTcpSrvP->setMaxPendingConnections(QCAN_TCP_SOCKET_MAX);

      if(!pclTcpSrvP->listen(clTcpHostAddrP, uwTcpPortP))
      {
         btNetworkEnabledP =  false;

         addLogMessage(CAN_Channel_e (id()),
                       "Failed to open TCP server ", eLOG_LEVEL_ERROR);

         return;
      }

      //-------------------------------------------------------------------------------------------
      // a new connection is handled by the onTcpSrvNewConnection() method
      //
      connect( pclTcpSrvP, SIGNAL(newConnection()),
               this, SLOT(onTcpSocketConnect()));


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

      disconnect( pclTcpSrvP, SIGNAL(newConnection()),
                  this, SLOT(onTcpSocketConnect()));


      //-------------------------------------------------------------------------------------------
      // close local and TCP server
      //
      pclLocalSrvP->close();
      pclTcpSrvP->close();

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
// setServerAddress()                                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanNetwork::setServerAddress(QHostAddress clHostAddressV)
{
   bool  btResultT = false;

   addLogMessage(CAN_Channel_e (id()),
                 QString("Set server address to " + clHostAddressV.toString()), eLOG_LEVEL_INFO);
   //----------------------------------------------------------------
   // host address can only be changed when network is disabled
   //
   if(btNetworkEnabledP == false)
   {
      clTcpHostAddrP = clHostAddressV;
      btResultT = true;
   }

   return(btResultT);
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
