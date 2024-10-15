//====================================================================================================================//
// File:          qcan_error.hpp                                                                                      //
// Description:   Send CAN error messages                                                                             //
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


#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>

#include <QCanSocket>

class QCanError : public QObject
{
   Q_OBJECT

public:
   QCanError(QObject *parent = nullptr);

   QCanError(const QCanError&) = delete;               // no copy constructor
   QCanError& operator=(const QCanError&) = delete;    // no assignment operator
   QCanError(QCanError&&) = delete;                    // no move constructor
   QCanError& operator=(QCanError&&) = delete;         // no move operator

signals:
   void finished();

public slots:
   void aboutToQuitApp(void);

   void runCmdParser(void);

   void sendErrorFrame(void);
   void socketConnected();
   void socketDisconnected();
   void socketError(QAbstractSocket::SocketError teSocketErrorV);
   void quit();
   
private:

   QCoreApplication        *pclAppP;

   QCommandLineParser      clCmdParserP;
   QCanSocket              clCanSocketP;

   //----------------------------------------------------------------------------------------------
   // Number of selected CAN channel
   //
   QCan::CAN_Channel_e     teCanChannelP;
   
   //----------------------------------------------------------------------------------------------
   // values of receive and transmit error counter
   //
   uint8_t                 ubRcvErrorCntP;
   uint8_t                 ubTrmErrorCntP;

   //----------------------------------------------------------------------------------------------
   // flags for decrementing the receive and transmit error counter
   //
   bool                    btDecRcvErrorP;
   bool                    btDecTrmErrorP;

   //----------------------------------------------------------------------------------------------
   // flags for incrementing the receive and transmit error counter
   //
   bool                    btIncRcvErrorP;
   bool                    btIncTrmErrorP;

   QCanFrame::ErrorType_e  teErrorTypeP;

   QCanFrame               clErrorFrameP;
   uint32_t                ulFrameGapP;
   uint32_t                ulFrameCountP;
};



