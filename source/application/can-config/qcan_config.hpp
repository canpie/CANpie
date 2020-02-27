//====================================================================================================================//
// File:          qcan_config.hpp                                                                                     //
// Description:   Configure CAN interface                                                                             //
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

#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>

#include "qcan_namespace.hpp"

#include <QCanNetworkSettings>
#include <QCanServerSettings>


using namespace QCan;

/*--------------------------------------------------------------------------------------------------------------------*\
** Class definition                                                                                                   **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


class QCanConfig : public QObject
{
   Q_OBJECT

public:
   QCanConfig(QObject *parent = 0);

public slots:
   void  aboutToQuitApp(void);

signals:
   void finished();

private slots:
   void  execCommand(void);

   void  onNetworkObjectReceived(const CAN_Channel_e teChannelV, QJsonObject clNetworkConfigV);
   void  onServerObjectReceived(QJsonObject clServerConfigV);
   void  onServerStateChanged(enum QCanServerSettings::State_e teStateV);
   
   void  runCommandParser(void);

   void  quit(void);

private:

   void  showNetworkSettings(void);
   void  showNetworkStatistics(void);
   void  showServerSettings(void);

   QPointer<QCoreApplication>    pclApplicationP;

   //----------------------------------------------------------------------------------------------
   // Command line parser
   //
   QCommandLineParser            clCommandParserP;

   //----------------------------------------------------------------------------------------------
   // Retrieve information from CANpie FD server and selected network
   //
   QCanNetworkSettings           clNetworkSettingsP;
   QCanServerSettings            clServerSettingsP;

   //----------------------------------------------------------------------------------------------
   // Host address of CANpie FD server, set with -H option
   //
   QHostAddress                  clHostAddressP;


   //----------------------------------------------------------------------------------------------
   // Number of selected CAN channel
   //
   CAN_Channel_e                 teCanChannelP;
   
   int32_t                       slNomBitRateP;
   int32_t                       slDatBitRateP;
   CAN_Mode_e                    teCanModeP;

   uint32_t                      ulCommandFlagsP;

};



