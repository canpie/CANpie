//====================================================================================================================//
// File:          qcan_interface_usart.cpp                                                                            //
// Description:   QCan Inteface for USART                                                                             //
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

#include "qcan_interface_usart.hpp"

/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

//----------------------------------------------------------------------------------------------------------------------
// nice macros to make a string from a definition, used in version()
//
#define STRINGIFY(x)    #x
#define TOSTRING(x)     STRINGIFY(x)


/*--------------------------------------------------------------------------------------------------------------------*\
** Class methods                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceUsart()                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterfaceUsart::QCanInterfaceUsart(uint16_t uwDeviceNrV, QString clNameV)
{
   //---------------------------------------------------------------------------------------------------
   // throw an error if no access to USART is avaialable
   //
   if (clCpUsartP.isAvailable() == false)
   {
      qCritical() << "QCanInterfaceUsart::QCanInterfaceUsart(): USART instance is not available!";
   }

   //---------------------------------------------------------------------------------------------------
   // setup interface variables
   //
   clUsartNameP = clNameV;
   uwUsartNumberP = uwDeviceNrV;
   teConnectedP = UnconnectedState;

   //---------------------------------------------------------------------------------------------------
   // clear error state
   //
   teErrorStateP = eCAN_STATE_BUS_ACTIVE;
   teCanModeP = eCAN_MODE_STOP;

   atsReceiveMessageListP.clear();
}


//--------------------------------------------------------------------------------------------------------------------//
// ~QCanInterfaceUsart()                                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterfaceUsart::~QCanInterfaceUsart()
{
   qDebug() << "QCanInterfaceUsart::~QCanInterfaceUsart()";

   //---------------------------------------------------------------------------------------------------
   // Relese USART driver
   //
   if (clCpUsartP.isAvailable() == true)
   {
      if (teConnectedP == ConnectedState)
      {
         teConnectedP = UnconnectedState;
         clCpUsartP.release();
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// connect()                                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceUsart::connect(void)
{
   InterfaceError_e teReturnT = eERROR_LIBRARY;

   //---------------------------------------------------------------------------------------------------
   // log the connect event
   //
   emit addLogMessage(tr("Connect ") + clUsartNameP +QString::number(uwUsartNumberP+1,10) + tr(" from 'QCan USART' plugin"), eLOG_LEVEL_INFO);

   //---------------------------------------------------------------------------------------------------
   // process the connect only if USART instance is available
   //
   if (clCpUsartP.isAvailable() == true)
   {
      //-------------------------------------------------------------------------------------------
      // initialise USART interface by defined Name and Number
      //
      clCpUsartP.setDeviceName(clUsartNameP);

      //-------------------------------------------------------------------------------------------
      // reset transmission lists for CAN messags
      //
      atsReceiveMessageListP.clear();

      //-------------------------------------------------------------------------------------------
      // connect the signal from USART to local slot for message reception
      // and connect USART interface
      //
      QObject::connect( &clCpUsartP, &QCanUsart::messageReceive,
                        this,        &QCanInterfaceUsart::onUsartMessageReceive);

      QObject::connect( &clCpUsartP, &QCanUsart::logMessage,
                        this,        &QCanInterfaceUsart::onLogMessage);

      clCpUsartP.connect();

      //-------------------------------------------------------------------------------------------
      // configure the refresh timer which triggers the reception of can messages
      //
      QTimer::singleShot(10, this, SLOT(onTimerEvent()));

      //-------------------------------------------------------------------------------------------
      // update and report the state of the interface
      //
      teConnectedP = ConnectedState;
      emit connectionChanged(teConnectedP);

      teReturnT = eERROR_NONE;

   }
   else
   {
      qCritical() << "QCanInterfaceUsart::connect(): USART instance is not available!";
      emit addLogMessage(tr("Connect to ") + clUsartNameP + QString::number(uwUsartNumberP+1,10) + tr(" is not possible, no USART instance available."), eLOG_LEVEL_FATAL);
   }

   return teReturnT;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceUsart::connectionState()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::ConnectionState_e QCanInterfaceUsart::connectionState(void)
{
   return teConnectedP;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceUsart::disableFeatures()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanInterfaceUsart::disableFeatures(uint32_t ulFeatureMaskV)
{
   Q_UNUSED(ulFeatureMaskV)
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceUsart::disconnect()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceUsart::disconnect()
{
   InterfaceError_e teReturnT = eERROR_LIBRARY;

   //---------------------------------------------------------------------------------------------------
   // log the disconnect event
   //
   emit addLogMessage(tr("Disonnect ") + clUsartNameP +QString::number(uwUsartNumberP+1,10) + tr(" from 'QCan USART' plugin"), eLOG_LEVEL_INFO);

   //---------------------------------------------------------------------------------------------------
   // process the disconnect only if USART instance is available
   //
   if (clCpUsartP.isAvailable() == true)
   {
      //-----------------------------------------------------------------------------------s
      // reset / clear all local resources
      //
      clCpUsartP.release();
      clCpUsartP.disconnect();
      atsReceiveMessageListP.clear();
      teConnectedP = UnconnectedState;
      emit connectionChanged(UnconnectedState);

      teReturnT = eERROR_NONE;

//      else
//      {
//         qWarning() << QString("QCanInterfaceUsart::disconnect(0x" +QString::number(tsPortP.ubPhyIf,16)+", "+clUsartNameP+")") << "fail with error:" << clCpUsartP.errorString();
//         emit addLogMessage(tr("Disconnect from ") + clUsartNameP + QString::number(uwUsartNumberP+1,10) + tr(" fails"), eLOG_LEVEL_FATAL);
//      }
   }
   else
   {
      qCritical() << "QCanInterfaceUsart::disconnect(): USART instance is not available!";
      emit addLogMessage(tr("Disconnect from ") + clUsartNameP + QString::number(uwUsartNumberP+1,10) + tr(" is not possible, no USART instance available."), eLOG_LEVEL_FATAL);
   }

   return teReturnT;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceUsart::disableFeatures()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanInterfaceUsart::enableFeatures(uint32_t ulFeatureMaskV)
{
   Q_UNUSED(ulFeatureMaskV)
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceUsart::icon()                                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QIcon QCanInterfaceUsart::icon(void)
{
   if (clCpUsartP.isAvailable())
   {
      return QIcon(":/images/usart_plugin_256.png");
   }

   return QIcon(":/images/usart_plugin_fail_256.png");
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceUsart::name()                                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanInterfaceUsart::name()
{
   qDebug() << "QCanInterfaceUsart::name()";
   if (clCpUsartP.isAvailable())
   {
      return clUsartNameP;
   }

   return QString("PCAN Basic library is not available");
}

//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceTemplate::onTimerEven()                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanInterfaceUsart::onTimerEvent(void)
{
   //---------------------------------------------------------------------------------------------------
   // check the reception list, if it is not empty, than emit a signal so the data is ready to read
   //
   if (atsReceiveMessageListP.isEmpty() == false)
   {
      emit readyRead();
   }

   //---------------------------------------------------------------------------------------------------
   // in other case retrigger the event timer
   //
   else
   {
      QTimer::singleShot(50, this, SLOT(onTimerEvent()));
   }
}


void QCanInterfaceUsart::onLogMessage(const QString & clMessageR)
{
   emit addLogMessage("USART-CPM: " + clMessageR, eLOG_LEVEL_DEBUG);
}


void QCanInterfaceUsart::onUsartMessageReceive(CpCanMsg_ts tsCanMessageV)
{
   qDebug() << "onUsartMessageReceive ................... ";

   atsReceiveMessageListP.append(tsCanMessageV);
   // convert the clMessageR into CpCanMsg_ts *ptsMsgV
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceUsart::read()                                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfaceUsart::read( QCanFrame &clFrameR)
{
   InterfaceError_e  clRetValueT = eERROR_NONE;
   QCanFrame         clCanFrameT;
   CpCanMsg_ts       tsCanMessageT;

   //---------------------------------------------------------------------------------------------------
   // process the read only if USART instance is available
   //
   if (clCpUsartP.isAvailable() == true)
   {
      //-------------------------------------------------------------------------------------------
      // check channel is connected
      //
      if (teConnectedP == ConnectedState)
      {

         //-----------------------------------------------------------------------------------
         // check fifo contains some messages
         //
         if (atsReceiveMessageListP.isEmpty() == true)
         {
            //---------------------------------------------------------------------------
            // if fifo is empty, than quit here an restart the timer envet for next check
            //
            clRetValueT = eERROR_FIFO_RCV_EMPTY;

            QTimer::singleShot(50, this, SLOT(onTimerEvent()));
         }

         //-----------------------------------------------------------------------------------
         // get next message from FIFO
         //
         else
         {
            //---------------------------------------------------------------------------
            // get message
            //
            tsCanMessageT = atsReceiveMessageListP.at(0);
            atsReceiveMessageListP.removeAt(0);

            //---------------------------------------------------------------------------
            // store the message in QCanFrame object
            //
            clCanFrameT.fromCpCanMsg(&tsCanMessageT);

            //---------------------------------------------------------------------------
            // increase statistic counter
            //
            tsStatisticP.ulRcvCount++;

            //---------------------------------------------------------------------------
            // copy the CAN frame
            //
            clFrameR = clCanFrameT;
         }
      }
      else
      {
         clRetValueT = eERROR_DEVICE;
      }
   }
   else
   {
      clRetValueT = eERROR_LIBRARY;
   }

   return (clRetValueT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceUsart::reset()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceUsart::reset()
{
   InterfaceError_e  clRetValueT = eERROR_NONE;
   qDebug() << "QCanInterfaceUsart::reset()";

   //---------------------------------------------------------------------------------------------------
   // process only if USART instance is available
   //
   if (clCpUsartP.isAvailable() == true)
   {
      //-------------------------------------------------------------------------------------------
      // reset statistic values
      //
      tsStatisticP.ulErrCount = 0;
      tsStatisticP.ulRcvCount = 0;
      tsStatisticP.ulTrmCount = 0;

      //-------------------------------------------------------------------------------------------
      // perform a hardware reset only if it has been initialised before
      //
      if (teConnectedP == ConnectedState)
      {
         disconnect();
         connect();
         setBitrate(slUsartBitrateP, eCP_BITRATE_NONE);
      }

      //-------------------------------------------------------------------------------------------
      // set the interface to the previous mode before reset
      //
      clRetValueT = setMode(teCanModeP);

   } else {
      clRetValueT = eERROR_LIBRARY;
   }

   return clRetValueT;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceUsart::setBitrate()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceUsart::setBitrate(int32_t slNomBitRateV, int32_t slDatBitRateV)
{
   InterfaceError_e  teReturnT = eERROR_NONE;

   qDebug() << "QCanInterfaceUsart::setBitrate()";

   //---------------------------------------------------------------------------------------------------
   // process only if USART instance is available
   //
   if (clCpUsartP.isAvailable() == true)
   {

      clCpUsartP.setCanBitrate(slNomBitRateV, slDatBitRateV);

      //emit addLogMessage(tr("Set CAN bit-rate is actually not supported for USART plugin only 125kBaud are used. Also the changes in 'Device Configuration' are not considered for USART baud-rate."), eLOG_LEVEL_WARN);

      //-------------------------------------------------------------------------------------------
      // \todo the setBitrate function is not supported
      //
//      slNomBitRateV = eCP_BITRATE_125K;
//      slDatBitRateV = eCP_BITRATE_NONE;

      //-------------------------------------------------------------------------------------------
      // provide new bit-rate to the CANpie Core implementation
      //
      //! \ToDo Bitrate übernehmen
//      tvStatusT = clCpUsartP.connect(slUsartBitrateP); // clCpUsartP.CpUsartBitrate(&tsPortP, slUsartBitrateP, slDatBitRateV);
//      if (tvStatusT != eCP_ERR_NONE)
//      {
//         emit addLogMessage(tr("Set CAN bit-rate for ") + clUsartNameP + QString::number(uwUsartNumberP+1,10) + tr(" fails with error 0x") + QString::number(tvStatusT,16), eLOG_LEVEL_FATAL);
//      }
   } else
   {
      teReturnT = eERROR_LIBRARY;
   }

   return teReturnT;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceUsart::setMode()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e	QCanInterfaceUsart::setMode(const CAN_Mode_e teModeV)
{
   InterfaceError_e  teReturnT = eERROR_NONE;
   CpStatus_tv tvStatusT;

   qDebug() << "QCanInterfaceUsart::setMode()";

   //---------------------------------------------------------------------------------------------------
   // process only if USART instance is available
   //
   if (clCpUsartP.isAvailable() == true)
   {
      //-------------------------------------------------------------------------------------------
      // select mode
      //
      switch (teModeV)
      {
         case eCAN_MODE_START :

            //---------------------------------------------------------------------------
            // reset statistic values
            //
            tsStatisticP.ulErrCount = 0;
            tsStatisticP.ulRcvCount = 0;
            tsStatisticP.ulTrmCount = 0;

            teCanModeP = eCAN_MODE_START;
            clCpUsartP.setCanMode(quint8(teModeV));
            break;

         case eCAN_MODE_STOP :
            teCanModeP = eCAN_MODE_STOP;
            clCpUsartP.setCanMode(quint8(teModeV));
            break;

         default :
            teReturnT = eERROR_MODE;
            break;
      }
   }
   else
   {
      teReturnT = eERROR_LIBRARY;
   }

   return teReturnT;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceUsart::state()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CAN_State_e QCanInterfaceUsart::state(void)
{
   return (teErrorStateP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceUsart::statistic()                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e	QCanInterfaceUsart::statistic(QCanStatistic_ts &clStatisticR)
{
   //---------------------------------------------------------------------------------------------------
   // process only if USART instance is available
   //
   if(clCpUsartP.isAvailable() == true)
   {
      clStatisticR.ulErrCount = tsStatisticP.ulErrCount;
      clStatisticR.ulRcvCount = tsStatisticP.ulRcvCount;
      clStatisticR.ulTrmCount = tsStatisticP.ulTrmCount;
   }
   else
   {
      return(eERROR_LIBRARY);
   }


   return(eERROR_NONE);
}


//--------------------------------------------------------------------------------------------------------------------//
// supportedFeatures()                                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
uint32_t QCanInterfaceUsart::supportedFeatures()
{
   uint32_t ulFeaturesT = 0;

   qDebug() << "QCanInterfaceUsart::supportedFeatures()";

   //---------------------------------------------------------------------------------------------------
   // process only if USART instance is available
   //
   if(clCpUsartP.isAvailable() == true)
   {
      ulFeaturesT  = QCAN_IF_SUPPORT_CAN_FD;
      ulFeaturesT |= QCAN_IF_SUPPORT_SPECIFIC_CONFIG;
   }

   return(ulFeaturesT);
}


//--------------------------------------------------------------------------------------------------------------------//
// version()                                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanInterfaceUsart::version(void)
{
   QString clVersionT;
   const char szPluginC[]   = TOSTRING(TARGET_NAME);

   clVersionT  = QString("Plugin ");
   clVersionT.append(szPluginC);
   clVersionT += QString(" version %1.%2,").arg(VERSION_MAJOR).arg(VERSION_MINOR);
   clVersionT += QString(" build %1,").arg(VERSION_BUILD);

   return (clVersionT);
}


//--------------------------------------------------------------------------------------------------------------------//
// write()                                                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceUsart::write(const QCanFrame &clFrameR)
{
   InterfaceError_e  teReturnT = eERROR_NONE;

   //---------------------------------------------------------------------------------------------------
   // process only if USART instance is available
   //
   if (clCpUsartP.isAvailable() == true)
   {
      //-------------------------------------------------------------------------------------------
      // Convert the QCanFrame in to the CpCanMsg object and provide it to the USART interface
      //
      CpCanMsg_ts tsCanMessageT;

      if (clFrameR.toCpCanMsg(&tsCanMessageT) != true)
      {
         teReturnT = eERROR_UNKNOWN;
      }
      else if (clCpUsartP.messageSend(tsCanMessageT) != true)
      {
         teReturnT = eERROR_UNKNOWN;
      }

   } else
   {
      teReturnT = eERROR_LIBRARY;
   }

   return teReturnT;
}

//--------------------------------------------------------------------------------------------------------------------//
// configureDevice()                                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanInterfaceUsart::configureDevice(void)
{
   bool btStateT = false; // if true than new initialisation shoud be done

   qDebug() << "QCanInterfaceUsart::configureDevice";
   qDebug() << "value from current connfig, name: "<<clCpUsartP.currentConfig().clName;
   qDebug() << "value from current connfig, mode: "<<clCpUsartP.currentConfig().ubMode;
   qDebug() << "value from current connfig, bitrate: "<<clCpUsartP.currentConfig().slBaud;

   //---------------------------------------------------------------------------------------------------
   // create configuration GUI
   //
   QCanConfig *pclConfigGuiT = new QCanConfig(clCpUsartP.currentConfig());
   pclConfigGuiT->exec();

   //---------------------------------------------------------------------------------------------------
   // handle the new accepted confiugration parameters
   //
   if (pclConfigGuiT->result() == QDialog::Accepted)
   {
      // Setup the Hardware or other changed parameters
      clCpUsartP.setConfig(pclConfigGuiT->currentConfig());

      btStateT = true;
   }

   pclConfigGuiT->deleteLater();

   return btStateT;
}



