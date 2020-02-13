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
#include <QtCore/QObject>
#include <QtCore/QSharedMemory>

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

#include "qcan_network.hpp"


//----------------------------------------------------------------------------------------------------------------
/*!
** \class QCanServer
**
** This class represents a CAN server, which incorporates up to #QCAN_NETWORK_MAX number of CAN networks
** (QCanNetwork). It is only possible to run one instance of a QCanServer on a machine. In order to avoid
** multiple instances, the QCanServer class initialises a shared memory region which can be accessed using
** the QCanServerSettings class.
** <p>
**
**
*/
class QCanServer : public QObject
{
   Q_OBJECT

public:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  pclParentV     Pointer to QObject parent class
   ** \param[in]  uwPortStartV   Port number for TCP access
   ** \param[in]  ubNetworkNumV  Number of supported
   **
   ** Create new QCanServer object. The parameter \a ubNetworkNumV defines the maximum number of
   ** CAN networks (class QCanNetwork).
   */
   QCanServer( QObject * pclParentV = Q_NULLPTR,
               uint16_t  uwPortStartV = QCAN_TCP_DEFAULT_PORT,
               uint8_t   ubNetworkNumV = QCAN_NETWORK_MAX);

   ~QCanServer();

   void           enableBitrateChange(bool btEnabledV = true);

   void           enableBusOffRecovery(bool btEnabledV = true);

   void           enableModeChange(bool btEnabledV = true);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubNetworkIndexV - Network index
   **
   ** \return Pointer to QCanNetwork
   **
   ** The function returns a pointer to a QCanNetwork class stored at index \a ubNetworkIndexV. The
   ** first network is stored at index value 0.
   */
   QCanNetwork * network(uint8_t ubNetworkIndexV);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return Maximum number of networks
   **
   ** The function returns the number of networks.
   */
   uint8_t       maximumNetwork(void) const;

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return Host address of server
   **
   ** The function returns the host address of the server.
   */
   QHostAddress  serverAddress(void)     { return (clServerAddressP);  };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clHostAddressV - Host address
   **
   ** The function configures the host address of the server.
   */
   void          setServerAddress(QHostAddress clHostAddressV);


private slots:

   void          onTimerEvent(void);

private:

   void          initSettings();

   void          releaseSettings(void);

   QHostAddress               clServerAddressP;
   QVector<QCanNetwork *> *   pclListNetsP;
   QTimer *                   pclTimerP;

   uint16_t                   uwPortStartP;
   uint8_t                    ubNetworkMaxP;

   //---------------------------------------------------------------------------------------------------
   // The QCanServer allocates shared memory for data exchange between the server and clients.
   // The shared memory is read-only for the clients, client access is done via QCanServerSettings.
   //
   QSharedMemory *            pclSettingsP;
   bool                       btMemoryAttachedP;

   //---------------------------------------------------------------------------------------------------
   // Management of local sockets:  a QLocalServer (pclLocalSrvP) is
   // used to handle a fixed number of QLocalSockets (pclLocalSockListP)
   //
   QPointer<QLocalServer>     pclLocalSrvP;
   QVector<QLocalSocket*>*    pclLocalSockListP;
   QMutex                     clLocalSockMutexP;

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

private slots:
   /*!
   ** This slot is called upon local socket connection.
   */
   void onLocalSocketConnect(void);

   /*!
   ** This slot is called upon local socket disconnection.
   */
   void onLocalSocketDisconnect(void);

   /*!
   ** This slot is called when new data is available on any open local socket.
   */
   void onLocalSocketNewData(void);
};

#endif // QCAN_SERVER_HPP_
