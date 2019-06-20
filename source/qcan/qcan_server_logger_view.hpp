//====================================================================================================================//
// File:          qcan_server_logger.hpp                                                                              //
// Description:   QCAN server - logging class                                                                         //
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


#ifndef QCAN_SERVER_LOGGER_VIEW_HPP_
#define QCAN_SERVER_LOGGER_VIEW_HPP_


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextBrowser>

#include "qcan_server_logger.hpp"


//----------------------------------------------------------------------------------------------------------------
/*!
** \class QCanServerLoggerView
**
** This class add a logging capability for a QCanServer.
** <p>
** \todo - Description of file names, default log level
**
*/
class QCanServerLoggerView : public QCanServerLogger
{
   Q_OBJECT

public:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  pclServerV      - Pointer to QCanServer class
   **
   ** Create new QCanServerLogger object. The parameter \a pclServerV defines the QCanServer which
   ** shall be attached for logging.
   */
   QCanServerLoggerView(QCanServer * pclServerV = Q_NULLPTR);

   ~QCanServerLoggerView();

   void hide(void);

   bool isHidden(void);

   void show(void);


public slots:
   virtual void  appendMessage(const CAN_Channel_e ubChannelV, const QString & clLogMessageV,
                               LogLevel_e teLogLevelV = eLOG_LEVEL_INFO);
private slots:
   void onChangeLogLevel(QAction * pclActionV);
   void onClearLog(void);
   void onSetLogFile(void);
   void onShowLogMenu(const QPoint & clPositionR);

private:
   QMainWindow  * pclLogWindowP;
   QTabWidget   * pclLogTabP;
   QTextBrowser * apclLogTextP[QCAN_NETWORK_MAX];

   CAN_Channel_e  teCanChannelP;

};

#endif // QCAN_SERVER_LOGGER_VIEW_HPP_
