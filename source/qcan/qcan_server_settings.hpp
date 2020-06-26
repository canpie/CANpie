//====================================================================================================================//
// File:          qcan_server_settings.hpp                                                                            //
// Description:   QCAN classes - CAN server settings                                                                  //
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


#ifndef QCAN_SERVER_SETTINGS_HPP_
#define QCAN_SERVER_SETTINGS_HPP_


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QJsonObject>
#include <QtCore/QPointer>

#include <QtWebSockets/QWebSocket>

#include "qcan_defs.hpp"
#include "qcan_namespace.hpp"

using namespace QCan;

/*--------------------------------------------------------------------------------------------------------------------*\
** Structures                                                                                                         **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//----------------------------------------------------------------------------------------------------------------
/*!
** \class QCanServerSettings
**
** The QCanServerSettings provides the functionality to test the current state of a QCanServer, its version
** number and the number of supported CAN networks. Communication between an instance of the QCanServer class
** and the QCanServerSettings class is done via WebSockets using JSON.
** <p>
** The following code shows an example how to establish a connection to a local QCanServer:
** \code
** QCanServerSettings   clServerSettingsT;
** clServerSettingsT.connectToServer();      // connect to local server
** \endcode
** <p>
** Connection to a QCanServer running on a different machine is only possible if remote access was granted
** by calling QCanServer::setServerAddress().
** \code
** QCanServerSettings   clServerSettingsT;
** clServerSettingsT.connectToServer(QHostAddress("192.168.2.12"));      // connect to remote server
** \endcode
** <p>
** A state change of the QCanServer class is signalled via stateChanged(). Data from the QCanServer class
** is in JSON format, new data can be inspected via objectReceived().

*/
class QCanServerSettings : public QObject
{
   Q_OBJECT

public:
   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  pclParentV - Pointer to QObject parent class
   **
   ** Construct a QCanServerSettings object
   */
   QCanServerSettings(QObject * pclParentV = Q_NULLPTR);

   ~QCanServerSettings();

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \enum    State_e
   **
   ** This enumeration describes possible states of the server.
   */
   enum State_e {
      
      /*! Server is in unknown state.                 */
      eSTATE_UNKNOWN  = -1,

      /*! Server is not running.                      */
      eSTATE_INACTIVE = 0,

      /*! Server has been closed                      */
      eSTATE_CLOSED   = 1,

      /*! Server is active                            */
      eSTATE_ACTIVE   = 2
   };

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in] clServerAddressV - IPv4 or IPv6 address of QCanServer class
   ** \param[in] uwPortV          - port number of QCanServer class
   ** \param[in] clOriginR        - origin of the client
   ** \param[in] flags            - reserved
   **
   ** The method opens a WebSocket connection to a QCanServer class defined by \a  clServerAddressV and 
   ** \a uwPortV.
   ** <p>
   ** The origin of the client defined by \a clOriginR is as specified in RFC 6454. The origin is not 
   ** required for non-web browser clients (see RFC 6455). The origin may not contain new line 
   ** characters, otherwise the connection will be aborted immediately during the handshake phase.
   */
   void           connectToServer(const QHostAddress clServerAddressV = QHostAddress::LocalHost, 
                                  const uint16_t uwPortV = QCAN_WEB_SOCKET_DEFAULT_PORT, 
                                  const QString & clOriginR = "", const uint32_t flags = 0);

   void           closeConnection(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   **
   ** \return  \c True if valid data available
   **
   ** The function returns \c True if a connection has been established to a QCanServer and the
   ** received JSON data has a vaild format.
   */
   bool           isValid(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   **
   ** \return  Number of CAN networks
   **
   ** Return the number of networks (i.e. QCanNetwork) managed by the QCanServer class.
   */
   int32_t        networkCount(void);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  State of local QCanServer
   ** \see     stateString()
   **
   ** Return the state of a local QCanServer, possible values are listed by QCanServerSettings::State_e.
   */
   State_e        state(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  State of local QCanServer
   ** \see     state()
   **
   ** Return the state of a local QCanServer as a QString object. 
   */
   QString        stateString(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Build version
   ** \see     isValid()
   **
   ** Return the build version of the active QCanServer. A return value of -1 denotes that either no 
   ** connection could be establed to a QCanServer or the received JSON object was not valid.
   */
   int32_t        versionBuild(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Major version
   ** \see     isValid()
   **
   ** Return the major version of the active QCanServer. A return value of -1 denotes that either no 
   ** connection could be establed to a QCanServer or the received JSON object was not valid.
   */
   int32_t        versionMajor(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Minor version
   ** \see     isValid()
   **
   ** Return the minor version of the active QCanServer. A return value of -1 denotes that either no 
   ** connection could be establed to a QCanServer or the received JSON object was not valid.
   */
   int32_t        versionMinor(void);

signals:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teStateV - Server state
   ** \see        state()
   **
   ** This signal is emitted after a state change from a QCanServer. 
   */
   void           stateChanged(enum QCanServerSettings::State_e teStateV);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clServerSettingsV - JSON data
   **
   ** This signal is emitted after a new object was received from an active QCanServer. The JSON data
   ** has the following format:
   ** \code
   ** {
   **    "allowBitrateChange": true,
   **    "allowBusOffRecovery": true,
   **    "allowModeChange": true,
   **    "apiVersion": "1.0",
   **    "networkCount": 8,
   **    "serverLocalTime": "2020-01-21T16:30:36",
   **    "serverUptime": 15010,
   **    "versionBuild": 0,
   **    "versionMajor": 1,
   **    "versionMinor": 0
   ** }
   ** \endcode
   */
   void           objectReceived(QJsonObject clServerSettingsV);

private:

   QPointer<QWebSocket>    pclWebSocketP;
   State_e                 teServerStateP;
   uint32_t                ulUpdateRateP;

   QJsonObject             clJsonServerP;

private slots:
   void           onSocketConnect(void);
   void           onSocketDisconnect(void);
   void           onSocketError(QAbstractSocket::SocketError clErrorV);
   void           onSocketMessageReceived(const QString & clMessageR);

};

#endif // QCAN_SERVER_SETTINGS_HPP_
