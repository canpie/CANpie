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
   qRegisterMetaType<CpCanMsg_ts>("CpCanMsg_ts");

   //---------------------------------------------------------------------------------------------------
   // throw an error if no access to USART is avaialable
   //
   if (pclCpUsartP->isAvailable() == false)
   {
      qCritical() << "QCanInterfaceUsart::QCanInterfaceUsart(): USART instance is not available!";
   }

   //---------------------------------------------------------------------------------------------------
   // setup interface variables
   //
   clUsartNameP = clNameV;
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
   if (pclCpUsartP->isAvailable() == true)
   {
      if (teConnectedP == ConnectedState)
      {
         teConnectedP = UnconnectedState;
         pclCpUsartP->release();
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


   qDebug(" QCanInterfaceUsart::connect(void) .................. ");
   //---------------------------------------------------------------------------------------------------
   // log the connect event
   //
   emit addLogMessage(tr("Connect ") + clUsartNameP + tr(" from 'QCan USART CANpie Message' plugin"), eLOG_LEVEL_INFO);

   //---------------------------------------------------------------------------------------------------
   // process the connect only if USART instance is available
   //
   if (pclCpUsartP->isAvailable() == true)
   {
      //-------------------------------------------------------------------------------------------
      // initialise USART interface by defined Name and Number
      //
      pclCpUsartP->setDeviceName(clUsartNameP);

      //-------------------------------------------------------------------------------------------
      // reset transmission lists for CAN messags
      //
      atsReceiveMessageListP.clear();

      //-------------------------------------------------------------------------------------------
      // connect the signal from USART to local slot for message reception
      // and connect USART interface
      //
      QObject::connect( pclCpUsartP, &QCanUsart::messageReceive,
                        this,        &QCanInterfaceUsart::onUsartMessageReceive);

      QObject::connect( pclCpUsartP, &QCanUsart::logMessage,
                        this,        &QCanInterfaceUsart::onLogMessage);


      // Wait in a event loop till the USART Thread has been started
      QObject::connect(pclCpUsartP, &QCanUsart::started, &clEventLoopP, &QEventLoop::quit);

      // the USART Thread is started at connect, so trigger it from a nonblocking timer
      QTimer::singleShot(1, pclCpUsartP, SLOT(connect()));

      // wait in an event loop till the start of the thread finishes
      clEventLoopP.exec();

      qDebug() << "Thread has been started!!!";

      //-------------------------------------------------------------------------------------------
      // configure the refresh timer which triggers the reception of can messages
      //
      QTimer::singleShot(10, this, SLOT(onTimerEvent()));

      //-------------------------------------------------------------------------------------------
      // update and report the state of the interface
      //
      teConnectedP = ConnectedState;
//      emit connectionChanged(teConnectedP);

      teReturnT = eERROR_NONE;

   }
   else
   {
      qCritical() << "QCanInterfaceUsart::connect(): USART instance is not available!";
      emit addLogMessage(tr("Connect to ") + clUsartNameP + tr(" is not possible, no USART instance available."), eLOG_LEVEL_FATAL);
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
   qDebug("QCanInterfaceUsart::disconnect()");

   //---------------------------------------------------------------------------------------------------
   // log the disconnect event
   //
   emit addLogMessage(tr("Disonnect ") + clUsartNameP + tr(" from 'QCan USART CANpie Message' plugin"), eLOG_LEVEL_INFO);

   //---------------------------------------------------------------------------------------------------
   // process the disconnect only if USART instance is available
   //
   if (pclCpUsartP->isAvailable() == true)
   {
      //-----------------------------------------------------------------------------------s
      // reset / clear all local resources
      //
      qDebug("QCanInterfaceUsart::disconnect() 1");
      pclCpUsartP->release();
      qDebug("QCanInterfaceUsart::disconnect() 2");
      pclCpUsartP->disconnect();
      qDebug("QCanInterfaceUsart::disconnect() 3");
      atsReceiveMessageListP.clear();
      qDebug("QCanInterfaceUsart::disconnect() 4");

      teConnectedP = UnconnectedState;
      emit connectionChanged(UnconnectedState);

      teReturnT = eERROR_NONE;
   }
   else
   {
      qCritical() << "QCanInterfaceUsart::disconnect(): USART instance is not available!";
      emit addLogMessage(tr("Disconnect from ") + clUsartNameP + tr(" is not possible, no USART instance available."), eLOG_LEVEL_FATAL);
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
   if (pclCpUsartP->isAvailable())
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
   if (pclCpUsartP->isAvailable())
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
   atsReceiveMessageListP.append(tsCanMessageV);

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
   if (pclCpUsartP->isAvailable() == true)
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
   if (pclCpUsartP->isAvailable() == true)
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
         //connect();
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
   if (pclCpUsartP->isAvailable() == true)
   {
      pclCpUsartP->setCanBitrate(slNomBitRateV, slDatBitRateV);
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

   qDebug() << "QCanInterfaceUsart::setMode()";

   //---------------------------------------------------------------------------------------------------
   // process only if USART instance is available
   //
   if (pclCpUsartP->isAvailable() == true)
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
            pclCpUsartP->setCanMode(quint8(teModeV));
            break;

         case eCAN_MODE_STOP :
            teCanModeP = eCAN_MODE_STOP;
            pclCpUsartP->setCanMode(quint8(teModeV));
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
   if(pclCpUsartP->isAvailable() == true)
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
   if(pclCpUsartP->isAvailable() == true)
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
// setActiveUsartInterface()                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanInterfaceUsart::setActiveUsartInterface(QCanUsart *pclCpUsartV)
{
   qDebug() << "setActiveUsartInterface()";

   pclCpUsartP = pclCpUsartV;
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
   if (pclCpUsartP->isAvailable() == true)
   {
      //-------------------------------------------------------------------------------------------
      // Convert the QCanFrame in to the CpCanMsg object and provide it to the USART interface
      //
      CpCanMsg_ts tsCanMessageT;

      if (clFrameR.toCpCanMsg(&tsCanMessageT) != true)
      {
         teReturnT = eERROR_UNKNOWN;
      }
      else if (pclCpUsartP->messageSend(tsCanMessageT) != true)
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
   qDebug() << "value from current connfig, name: "<<pclCpUsartP->currentConfig().clName;
   qDebug() << "value from current connfig, mode: "<<pclCpUsartP->currentConfig().ubMode;
   qDebug() << "value from current connfig, bitrate: "<<pclCpUsartP->currentConfig().slBaud;

   //---------------------------------------------------------------------------------------------------
   // create configuration GUI
   //
   QCanConfig *pclConfigGuiT = new QCanConfig(pclCpUsartP->currentConfig());
   pclConfigGuiT->exec();

   //---------------------------------------------------------------------------------------------------
   // handle the new accepted confiugration parameters
   //
   if (pclConfigGuiT->result() == QDialog::Accepted)
   {
      // Setup the Hardware or other changed parameters
//      pclCpUsartP->setConfig(pclConfigGuiT->currentConfig());
      QMessageBox msgBox;
      msgBox.setText("User configurations are not porivided to COM interface!");
      msgBox.exec();

      btStateT = true;
   }

   pclConfigGuiT->deleteLater();

   return btStateT;
}



