//====================================================================================================================//
// File:          qcan_dump.hpp                                                                                       //
// Description:   Dump CAN messages                                                                                   //
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
#include <QtCore/QFile>
#include <QtCore/QPointer>
#include <QtCore/QTimer>

#include "qcan_namespace.hpp"

#include <QCanNetworkSettings>
#include <QCanServerSettings>
#include <QCanSocket>


//------------------------------------------------------------------------------------------------------
/*!
** \anchor can-dump
** \class QCanDump
** \brief Command line tool - dump CAN messages
**
**
*/
class QCanDump : public QObject
{
   Q_OBJECT

public:
   QCanDump(QObject *parent = 0);

public slots:
   void  aboutToQuitApp(void);

private slots:
   void  onNetworkObjectReceived(const CAN_Channel_e teChannelV, QJsonObject clNetworkConfigV);
   void  onServerObjectReceived(QJsonObject clServerConfigV);
   void  onServerStateChanged(enum QCanServerSettings::State_e teStateV);

   void  onSocketConnected(void);
   void  onSocketDisconnected(void);
   void  onSocketError(QAbstractSocket::SocketError teSocketErrorV);
   void  onSocketReceive(void);

   //----------------------------------------------------------------------------------------------
   /*!
   ** The function evaluates the command parameters
   */
   void  runCommandParser(void);

   void  quit();

signals:
   void finished();
   
private:

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
   // Socket to CANpie FD server
   //
   QCanSocket                    clCanSocketP;
   
   //----------------------------------------------------------------------------------------------
   // Number of selected CAN channel
   //
   CAN_Channel_e                 teCanChannelP;
   
   //----------------------------------------------------------------------------------------------
   // CAN frame filter list
   //
   QCanFilterList                clFilterListP;

   QTimer                        clActivityTimerP;
   bool                          btTimeStampP;
   bool                          btErrorFramesP;
   bool                          btQuitNeverP;

   uint32_t                      ulQuitTimeP;
   uint32_t                      ulQuitCountP;
};



