//====================================================================================================================//
// File:          qcan_server.hpp                                                                                     //
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


#ifndef QCAN_SERVER_HPP_
#define QCAN_SERVER_HPP_


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QSharedMemory>

#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>


#include "qcan_network.hpp"


//----------------------------------------------------------------------------------------------------------------
/*!
** \class QCanServer
**
** This class represents a CAN server, which incorporates up to #QCAN_NETWORK_MAX number of CAN networks
** (QCanNetwork). It is only possible to run one instance of a QCanServer on a machine. In order to avoid
** multiple instances, the QCanServer class initialises a shared memory region.
** <p>
** <h2>Remote access</h2>
** Access to the CAN server is granted via a WebSocket interface running on the default port
** #QCAN_WEB_SOCKET_DEFAULT_PORT. By default, access is granted only to processes running
** on the local machine. Remote access can be granted by calling setServerAddress(QHostAddress::AnyIPv4).
**
*/
class QCanServer : public QObject
{
   Q_OBJECT

public:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  pclParentV     Pointer to QObject parent class
   ** \param[in]  uwPortNumberV  Port number for WebSocket access
   ** \param[in]  ubNetworkNumV  Number of supported CAN networks
   **
   ** Create new QCanServer object. The parameter \a ubNetworkNumV defines the maximum number of
   ** CAN networks (class QCanNetwork).
   */
   QCanServer( QObject * pclParentV = Q_NULLPTR, uint16_t  uwPortNumberV = QCAN_WEB_SOCKET_DEFAULT_PORT,
               uint8_t   ubNetworkNumV = QCAN_NETWORK_MAX, bool btClearServerV = false);

   ~QCanServer();

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \enum    Error_e
   **
   ** This enumeration describes possible error conditions of the server.
   */
   enum Error_e {

      /*! No error.                                   */
      eERROR_NONE = 0,

      /*! Server is crashed                           */
      eERROR_CRASHED,

      /*! Server is active                            */
      eERROR_ACTIVE
   };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  btEnabledV - Enable / disable bit-rate change via application
   **
   ** This function enables the setting of bit-rate via any application if \a btEnable 
   ** is \c true, it is disabled on \c false.
   */
   void           allowBitrateChange(bool btEnabledV = true);

   void           allowBusOffRecovery(bool btEnabledV = true);

   void           allowModeChange(bool btEnabledV = true);

   bool           isBitrateChangeAllowed(void)     { return (btAllowBitrateChangeP);   };

   bool           isBusOffRecoveryAllowed(void)    { return (btAllowBusOffRecoverP);   };

   bool           isModeChangeAllowed(void)        { return (btAllowCanModeChangeP);   };

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubNetworkIndexV - Network index
   **
   ** \return Pointer to QCanNetwork
   **
   ** The function returns a pointer to a QCanNetwork class stored at index \a ubNetworkIndexV. The
   ** first network is stored at index value 0.
   */
   QCanNetwork *  network(uint8_t ubNetworkIndexV);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return Maximum number of networks
   **
   ** The function returns the number of networks.
   */
   uint8_t        maximumNetwork(void) const;

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return Host address of server
   **
   ** The function returns the host address of the server.
   */
   QHostAddress   serverAddress(void)     { return (clServerAddressP);  };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clHostAddressV - Host address
   ** \param[in]  uwPortV        - Port number for WebSocket access
   **
   ** The function configures the host address of the server.
   */
   void           setServerAddress(const QHostAddress clHostAddressV, 
                                   const uint16_t uwPortV = QCAN_WEB_SOCKET_DEFAULT_PORT);

   Error_e        state(void)    { return (teErrorP); };

signals:

   void           error(enum QCanServer::Error_e teErrorV);

private slots:

   void           onTimerEvent(void);

private:

   void           checkServerSettings(bool btClearServerV);
   void           sendServerSettings(QWebSocket * pclSocketV, uint32_t flags = 0);

   Error_e                    teErrorP;

   QHostAddress               clServerAddressP;
   uint16_t                   uwServerPortP;
   QVector<QCanNetwork *>     clNetworkListP;
   QTimer *                   pclTimerP;
   uint8_t                    ubNetworkMaxP;


   //---------------------------------------------------------------------------------------------------
   // Management of web sockets:  a QWebSocketServer (pclSocketServerP) is used to handle a fixed 
   // number of QWebSockets (pclWebSocketListP) for configuration and data inspection
   //
   QPointer<QWebSocketServer> pclWebSocketServerP;
   QVector<QWebSocket*>       clWebSocketListP;
   QMutex                     clWebSocketMutexP;

   //---------------------------------------------------------------------------------------------------
   // This flag keeps the information if bit-rate change is possible via a local client, default
   // value is 'false';
   //
   bool                       btAllowBitrateChangeP;

   //---------------------------------------------------------------------------------------------------
   // This flag keeps the information if a network can recover from bus-off state autonomously.
   //
   bool                       btAllowBusOffRecoverP;

   //---------------------------------------------------------------------------------------------------
   // This flag keeps the information if CAN mode change is possible via a local client, default
   // value is 'false';
   //
   bool                       btAllowCanModeChangeP;

   //---------------------------------------------------------------------------------------------------
   // The QCanServer allocates shared memory to make sure it is initialised only once
   //
   QSharedMemory *            pclServerConfigurationP;

   int64_t                    sqDateTimeStartP;

   int64_t                    sqUptimeMillisecondsP;
   
   QJsonObject                clJsonServerP;
   
private slots:

   void           onSocketDisconnect(void);

   /*!
   ** This slot is called upon WebSocket disconnection.
   */
   void           onWebSocketServerClose(void);

   /*!
   ** This slot is called upon a new WebSocket connection.
   */
   void           onWebSocketServerConnect(void);

   /*!
   ** This slot is called when an error is emitted on the WebSocket.
   */
   void           onWebSocketServerError(QWebSocketProtocol::CloseCode teCloseCodeV);

};

#endif // QCAN_SERVER_HPP_
