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

#include "qcan_server_memory.hpp"
#include "qcan_server_settings.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#define  MAXIMUM_SERVER_TIME_DIFFERENCE      ((int64_t) 2000)



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
   // create shared memory for data exchange (read-only access)
   //
   pclSettingsP = new QSharedMemory(QString(QCAN_MEMORY_KEY));
   pclSettingsP->attach(QSharedMemory::ReadOnly);

   //---------------------------------------------------------------------------------------------------
   // create a local socket
   //
   pclLocalSockP = new QLocalSocket(this);
   pclLocalSockP->abort();

   //---------------------------------------------------------------------------------------------------
   // make signal / slot connections for local socket
   //
   connect( pclLocalSockP, SIGNAL(connected()),
            this, SLOT(onSocketConnect()));

   connect( pclLocalSockP, SIGNAL(disconnected()),
            this, SLOT(onSocketDisconnect()));

   connect( pclLocalSockP, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this, SLOT(onSocketError(QLocalSocket::LocalSocketError)));

   connect( pclLocalSockP, SIGNAL(readyRead()),
            this, SLOT(onSocketReceive()));

   //---------------------------------------------------------------------------------------------------
   // connect to local server
   //
   pclLocalSockP->connectToServer(QString("CANpieServer"));

   //---------------------------------------------------------------------------------------------------
   // the socket is not connected yet
   //
   btIsConnectedP       = false;

   //---------------------------------------------------------------------------------------------------
   // No socket errors available yet
   //
   slSocketErrorP = 0;

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::~QCanServerSettings()                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanServerSettings::~QCanServerSettings()
{
   pclSettingsP->detach();
   delete (pclSettingsP);
}


//--------------------------------------------------------------------------------------------------------------------//
// networkCount()                                                                                                     //
// return number of supported CAN networks                                                                            //
//--------------------------------------------------------------------------------------------------------------------//
int QCanServerSettings::networkCount(void)
{
   int32_t  slCountT = 0;

   if (pclSettingsP->isAttached())
   {
      if (pclSettingsP->lock())
      {
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();
         if (ptsSettingsT != (ServerSettings_ts *) 0L)
         {
            slCountT = ptsSettingsT->tsServer.slNetworkCount;
         }
      }

      pclSettingsP->unlock();
   }

   return (slCountT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::networkBusLoad()                                                                               //
// return bus-load of the current CAN interface                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
uint8_t QCanServerSettings::networkBusLoad(const CAN_Channel_e teChannelV)
{
   uint8_t  ubResultT = 0;

   if (pclSettingsP->isAttached())
   {
      if (pclSettingsP->lock())
      {
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         //----------------------------------------------------------------------------------------
         // get error counter value
         //
         if (ptsSettingsT != (ServerSettings_ts *) 0L)
         {
            ubResultT = ptsSettingsT->atsNetwork[teChannelV - 1].ubBusLoad;
         }
      }

      pclSettingsP->unlock();
   }


   return (ubResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::networkConfiguration()                                                                         //
// return configuration of network                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanServerSettings::networkConfiguration(const CAN_Channel_e teChannelV)
{
   int32_t  slResultT = 0;

   if (pclSettingsP->isAttached())
   {
      if (pclSettingsP->lock())
      {
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         //----------------------------------------------------------------------------------------
         // get configuration flags of network
         //
         if (ptsSettingsT != (ServerSettings_ts *) 0L)
         {
            slResultT = ptsSettingsT->atsNetwork[teChannelV - 1].slFlags;
         }
      }

      pclSettingsP->unlock();
   }

   return (slResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::networkConfigurationString()                                                                   //
// return configuration of network                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanServerSettings::networkConfigurationString(const CAN_Channel_e teChannelV)
{
   QString  clResultT;
   int32_t  slConfigurationT;

   slConfigurationT = networkConfiguration(teChannelV);

   if ((slConfigurationT & QCAN_NETWORK_FLAG_CAN_FD) > 0)
   {
      clResultT  = "CAN FD";
   }
   else
   {
      clResultT  = "Classic CAN";
   }

   if ((slConfigurationT & QCAN_NETWORK_FLAG_LISTEN_ONLY) > 0)
   {
      clResultT += ", listen-only mode";
   }

   if ((slConfigurationT & QCAN_NETWORK_FLAG_ERROR_FRAME) > 0)
   {
      clResultT += ", error frames enabled";
   }

   return (clResultT);

}

//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::networkDataBitrate()                                                                           //
// return the data bit-rate of network                                                                                //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanServerSettings::networkDataBitrate(const CAN_Channel_e teChannelV)
{
   int32_t  slResultT = eCAN_BITRATE_NONE;

   if (pclSettingsP->isAttached())
   {
      if (pclSettingsP->lock())
      {
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         //----------------------------------------------------------------------------------------
         // get data bit-rate value
         //
         if (ptsSettingsT != (ServerSettings_ts *) 0L)
         {
            slResultT = ptsSettingsT->atsNetwork[teChannelV - 1].slDatBitRate;
         }
      }

      pclSettingsP->unlock();
   }

   return (slResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::dataBitrateString()                                                                            //
// return the data bit-rate of network                                                                                //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanServerSettings::networkDataBitrateString(const CAN_Channel_e teChannelV)
{
   QString  clResultT;
   int32_t  slBitrateT;

   slBitrateT = networkDataBitrate(teChannelV);

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
// QCanServerSettings::networkErrorCount()                                                                            //
// return error counter of the current CAN interface                                                                  //
//--------------------------------------------------------------------------------------------------------------------//
uint32_t QCanServerSettings::networkErrorCount(const CAN_Channel_e teChannelV)
{
   uint32_t  ulResultT = 0;

   if (pclSettingsP->isAttached())
   {
      if (pclSettingsP->lock())
      {
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         //----------------------------------------------------------------------------------------
         // get error counter value
         //
         if (ptsSettingsT != (ServerSettings_ts *) 0L)
         {
            ulResultT = ptsSettingsT->atsNetwork[teChannelV - 1].ulCntFrameErr;
         }
      }

      pclSettingsP->unlock();
   }


   return (ulResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::networkFrameCount()                                                                            //
// return frame count of the current CAN interface                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
uint32_t QCanServerSettings::networkFrameCount(const CAN_Channel_e teChannelV)
{
   uint32_t  ulResultT = 0;

   if (pclSettingsP->isAttached())
   {
      if (pclSettingsP->lock())
      {
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         //----------------------------------------------------------------------------------------
         // get error counter value
         //
         if (ptsSettingsT != (ServerSettings_ts *) 0L)
         {
            ulResultT = ptsSettingsT->atsNetwork[teChannelV - 1].ulCntFrameCan;
         }
      }

      pclSettingsP->unlock();
   }


   return (ulResultT);
}

//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::name()                                                                                         //
// return the name of the current CAN interface                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanServerSettings::networkName(const CAN_Channel_e teChannelV)
{
   QString clResultT;

   if (pclSettingsP->isAttached())
   {
      if (pclSettingsP->lock())
      {
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         //----------------------------------------------------------------------------------------
         // calculate the size of the interface name and limit it if necessary
         //
         if (ptsSettingsT != (ServerSettings_ts *) 0L)
         {
            int slSizeT = strlen(ptsSettingsT->atsNetwork[teChannelV - 1].szInterfaceName);
            if (slSizeT > QCAN_IF_NAME_LENGTH)
            {
               slSizeT = QCAN_IF_NAME_LENGTH;
            }

            clResultT = QString::fromLatin1(&(ptsSettingsT->atsNetwork[teChannelV - 1].szInterfaceName[0]), slSizeT);
         }
      }

      pclSettingsP->unlock();
   }


   return (clResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::nominalBitrate()                                                                               //
// return the nominal bit-rate of the current CAN interface                                                           //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanServerSettings::networkNominalBitrate(const CAN_Channel_e teChannelV)
{
   int32_t  slResultT = eCAN_BITRATE_NONE;

   if (pclSettingsP->isAttached())
   {
      if (pclSettingsP->lock())
      {
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         //----------------------------------------------------------------------------------------
         // get nominal bit-rate value
         //
         if (ptsSettingsT != (ServerSettings_ts *) 0L)
         {
            slResultT = ptsSettingsT->atsNetwork[teChannelV - 1].slNomBitRate;
         }
      }

      pclSettingsP->unlock();
   }


   return (slResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::nominalBitrateString()                                                                         //
// return the nominal bit-rate of the current CAN interface                                                           //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanServerSettings::networkNominalBitrateString(const CAN_Channel_e teChannelV)
{
   QString  clResultT;
   int32_t  slBitrateT;

   slBitrateT = networkNominalBitrate(teChannelV);

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
// QCanServerSettings::networkState()                                                                                 //
// return CAN network state                                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
CAN_State_e QCanServerSettings::networkState(const CAN_Channel_e teChannelV)
{
   CAN_State_e teResultT = eCAN_STATE_STOPPED;

   if (pclSettingsP->isAttached())
   {
      if (pclSettingsP->lock())
      {
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         //----------------------------------------------------------------------------------------
         // get CAN state value
         //
         if (ptsSettingsT != (ServerSettings_ts *) 0L)
         {
            teResultT = (CAN_State_e)(ptsSettingsT->atsNetwork[teChannelV - 1].slStatus);
         }
      }

      pclSettingsP->unlock();
   }


   return (teResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::networkStateString()                                                                           //
// return CAN network state                                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanServerSettings::networkStateString(const CAN_Channel_e teChannelV)
{
   QString clResultT;

   switch(networkState(teChannelV))
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


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::onSocketConnect()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerSettings::onSocketConnect(void)
{
   qDebug() << "QCanServerSettings::onSocketConnect() ";

   //---------------------------------------------------------------------------------------------------
   // keep connection state in member variable
   //
   btIsConnectedP = true;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::onSocketDisconnect()                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerSettings::onSocketDisconnect(void)
{
   qDebug() << "QCanServerSettings::onSocketDisconnect() ";

   //---------------------------------------------------------------------------------------------------
   // keep connection state in member variable
   //
   btIsConnectedP = false;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::onSocketError()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerSettings::onSocketError(QLocalSocket::LocalSocketError teSocketErrorV)
{
   qDebug() << "QCanServerSettings::onSocketError() ";

   switch(teSocketErrorV)
   {
      //-------------------------------------------------------------------------------------------
      // abort all operations and disconnect socket
      //
      case QLocalSocket::PeerClosedError:
      case QLocalSocket::ConnectionError:
         pclLocalSockP->abort();
         btIsConnectedP = false;
         break;

      default:

         break;

   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::onSocketReceive()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerSettings::onSocketReceive(void)
{
   qDebug() << "QCanServerSettings::onSocketReceive() ";

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::resetNetwork()                                                                                 //
// Reset CAN network                                                                                                  //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanServerSettings::resetNetwork(const CAN_Channel_e teChannelV)
{
   bool  btAllowResetNetworkT = false;
   bool  btResultT = false;

   //---------------------------------------------------------------------------------------------------
   // Check if the the channel parameter is valid and if the CAN servers allows to change the bit-rate
   //
   if (pclSettingsP->isAttached())
   {
      if (pclSettingsP->lock())
      {
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         if (ptsSettingsT != (ServerSettings_ts *) 0L)
         {
            //--------------------------------------------------------------------------------
            // Test the channel value
            //
            if ( (teChannelV > 0) && (teChannelV <= ptsSettingsT->tsServer.slNetworkCount))
            {
               //------------------------------------------------------------------------
               // Test the flag in the server: change mode allowed?
               //
               if ( (ptsSettingsT->tsServer.slFlags) & QCAN_SERVER_FLAG_ALLOW_MODE)
               {
                  btAllowResetNetworkT = true;
               }
            }
         }
      }

      pclSettingsP->unlock();
   }


   //---------------------------------------------------------------------------------------------------
   // Reset network via a local socket command
   //
   if (btAllowResetNetworkT)
   {
      ServerSocketCommand_ts  tsCommandT;
      tsCommandT.ubCommand      = eSERVER_CMD_RESET;
      tsCommandT.ubChannel      = teChannelV;
      tsCommandT.slParameter[0] = 0;
      tsCommandT.slParameter[1] = 0;

      if (pclLocalSockP->write((char *)&(tsCommandT), sizeof(tsCommandT)) == sizeof(tsCommandT))
      {
         pclLocalSockP->flush();
         btResultT = true;
      }
   }

   return (btResultT);

}

//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::setNetworkBitrate()                                                                            //
// change bit-rate of CAN network                                                                                     //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanServerSettings::setNetworkBitrate(const CAN_Channel_e teChannelV,
                                           int32_t slNomBitRateV, int32_t slDatBitRateV)
{
   bool  btAllowBitrateChangeT = false;
   bool  btResultT = false;

   //---------------------------------------------------------------------------------------------------
   // Check if the the channel parameter is valid and if the CAN servers allows to change the bit-rate
   //
   if (pclSettingsP->isAttached())
   {
      if (pclSettingsP->lock())
      {
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         if (ptsSettingsT != (ServerSettings_ts *) 0L)
         {
            //--------------------------------------------------------------------------------
            // Test the channel value
            //
            if ( (teChannelV > 0) && (teChannelV <= ptsSettingsT->tsServer.slNetworkCount))
            {
               //------------------------------------------------------------------------
               // Test the flag in the server: change bit-rate allowed?
               //
               if ( (ptsSettingsT->tsServer.slFlags) & QCAN_SERVER_FLAG_ALLOW_BITRATE)
               {
                  btAllowBitrateChangeT = true;
               }
            }
         }
      }

      pclSettingsP->unlock();
   }


   //---------------------------------------------------------------------------------------------------
   // Change bit-rate setting via a local socket command
   //
   if (btAllowBitrateChangeT)
   {
      ServerSocketCommand_ts  tsCommandT;
      tsCommandT.ubCommand      = eSERVER_CMD_BITRATE;
      tsCommandT.ubChannel      = teChannelV;
      tsCommandT.slParameter[0] = slNomBitRateV;
      tsCommandT.slParameter[1] = slDatBitRateV;

      if (pclLocalSockP->write((char *)&(tsCommandT), sizeof(tsCommandT)) == sizeof(tsCommandT))
      {
         pclLocalSockP->flush();
         btResultT = true;
      }
   }

   return (btResultT);
}

//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::setNetworkMode()                                                                               //
// return CAN network state                                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanServerSettings::setNetworkMode(const CAN_Channel_e teChannelV, const CAN_Mode_e teModeV)
{
   bool  btAllowCanModeChangeT = false;
   bool  btResultT = false;

   //---------------------------------------------------------------------------------------------------
   // Check if the the channel parameter is valid and if the CAN servers allows to change the bit-rate
   //
   if (pclSettingsP->isAttached())
   {
      if (pclSettingsP->lock())
      {
         ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

         if (ptsSettingsT != (ServerSettings_ts *) 0L)
         {
            //--------------------------------------------------------------------------------
            // Test the channel value
            //
            if ( (teChannelV > 0) && (teChannelV <= ptsSettingsT->tsServer.slNetworkCount))
            {
               //------------------------------------------------------------------------
               // Test the flag in the server: change bit-rate allowed?
               //
               if ( (ptsSettingsT->tsServer.slFlags) & QCAN_SERVER_FLAG_ALLOW_MODE)
               {
                  btAllowCanModeChangeT = true;
               }
            }
         }
      }

      pclSettingsP->unlock();
   }


   //---------------------------------------------------------------------------------------------------
   // Change CAN mode via a local socket command
   //
   if (btAllowCanModeChangeT)
   {
      ServerSocketCommand_ts  tsCommandT;
      tsCommandT.ubCommand      = eSERVER_CMD_MODE;
      tsCommandT.ubChannel      = teChannelV;
      tsCommandT.slParameter[0] = teModeV;
      tsCommandT.slParameter[1] = 0;

      if (pclLocalSockP->write((char *)&(tsCommandT), sizeof(tsCommandT)) == sizeof(tsCommandT))
      {
         pclLocalSockP->flush();
         btResultT = true;
      }
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::state()                                                                                        //
// return the current state of the QCan server                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanServerSettings::State_e QCanServerSettings::state(void)
{
   State_e  teServerStateT = eSTATE_UNKNOWN;

   if (pclSettingsP->isAttached())
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

      if ( (QDateTime::currentMSecsSinceEpoch() - (ptsSettingsT->tsServer.sqDateTimeActual)) > MAXIMUM_SERVER_TIME_DIFFERENCE)
      {
         teServerStateT = eSTATE_CRASHED;
      }
      else
      {
         teServerStateT = eSTATE_ACTIVE;
      }
   }
   else
   {
      teServerStateT = eSTATE_INACTIVE;
   }

   return (teServerStateT);
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
      case eSTATE_CRASHED:
         clResultT = QObject::tr("is crashed");
         break;

      case eSTATE_INACTIVE:
         clResultT = QObject::tr("is not active");
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
// versionBuild()                                                                                                     //
// return build version                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
int QCanServerSettings::versionBuild(void)
{
   int32_t  slVersionT = -1;

   if (pclSettingsP->isAttached())
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();
      slVersionT = ptsSettingsT->tsServer.slVersionBuild;
   }

   return (slVersionT);
}


//--------------------------------------------------------------------------------------------------------------------//
// versionMajor()                                                                                                     //
// return major version                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
int QCanServerSettings::versionMajor(void)
{
   int32_t  slVersionT = -1;

   if (pclSettingsP->isAttached())
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();
      slVersionT = ptsSettingsT->tsServer.slVersionMajor;
   }

   return (slVersionT);
}


//--------------------------------------------------------------------------------------------------------------------//
// versionMinor()                                                                                                     //
// return minor version                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanServerSettings::versionMinor(void)
{
   int32_t  slVersionT = -1;

   if (pclSettingsP->isAttached())
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();
      slVersionT = ptsSettingsT->tsServer.slVersionMinor;
   }

   return (slVersionT);
}

