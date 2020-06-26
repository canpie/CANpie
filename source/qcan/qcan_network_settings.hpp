//====================================================================================================================//
// File:          qcan_network_settings.hpp                                                                           //
// Description:   QCAN classes - CAN network settings                                                                 //
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


#ifndef QCAN_NETWORK_SETTINGS_HPP_
#define QCAN_NETWORK_SETTINGS_HPP_


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

//----------------------------------------------------------------------------------------------------------------
/*!
** \class QCanNetworkSettings
**
** The QCanNetworkSettings provides the functionality to access parameters of a QCanNetwork.
**
*/
class QCanNetworkSettings : public QObject
{
   Q_OBJECT

public:
   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelV - CAN channel
   ** \param[in]  pclParentV - Pointer to QObject parent class
   **
   ** Construct a QCanNetworkSettings object for the channel \a teChannelV.
   */
   QCanNetworkSettings(const CAN_Channel_e teChannelV = eCAN_CHANNEL_1, QObject * pclParentV = Q_NULLPTR);

   ~QCanNetworkSettings();


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clServerAddressV - IPv4 or IPv6 address of QCanNetwork class
   ** \param[in]  uwPortV          - port number of QCanNetwork class
   ** \param[in]  clOriginR        - origin of the client
   ** \param[in]  flags            - reserved
   ** \see        closeConnection()
   **
   ** The method opens a WebSocket connection to a QCanNetwork class defined by \a  clServerAddressV and 
   ** \a uwPortV.
   ** <p>
   ** The origin of the client defined by \a clOriginR is as specified in RFC 6454. The origin is not 
   ** required for non-web browser clients (see RFC 6455). The origin may not contain new line 
   ** characters, otherwise the connection will be aborted immediately during the handshake phase.
   */
   void           connectToServer(const QHostAddress clServerAddressV = QHostAddress::LocalHost, 
                                  const uint16_t uwPortV = QCAN_WEB_SOCKET_DEFAULT_PORT, 
                                  const QString & clOriginR = "", const uint32_t flags = 0);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \see        connectToServer()
   **
   ** The method closes a WebSocket connection to a QCanNetwork class.
   */
   void           closeConnection(void);
   
   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Data bit-rate
   ** \see     dataBitrateString()
   **
   ** Return the current data bit-rate of the selected network in [bits/s]. If the bit-rate value
   ** is not valid (not configured) the function returns QCan::eCAN_BITRATE_NONE. The result of this
   ** function is equivalent to QCanNetwork::dataBitrate().
   */
   int32_t        dataBitrate(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Data bit-rate
   ** \see     dataBitrate()
   **
   ** Return the current data bit-rate of the selected network as string object. If the bit-rate value
   ** is not valid (not configured) the function returns "None".
   */
   QString        dataBitrateString(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Data bit-rate
   ** \see     dataBitrate()
   **
   ** Return the current data bit-rate of the selected network as string object. If the bit-rate value
   ** is not valid (not configured) the function returns "None".
   */
   int32_t        errorCount(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Data bit-rate
   ** \see     dataBitrate()
   **
   ** Return the current data bit-rate of the selected network as string object. If the bit-rate value
   ** is not valid (not configured) the function returns "None".
   */
   int32_t        frameCount(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   **
   ** \return  \c True if valid data available
   **
   ** The function returns \c True if a connection has been established to a QCanNetwork and the
   ** received JSON data has a vaild format.
   */
   bool           isValid(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Network name
   **
   ** Return the name of the selected network as string object. If a CAN interface is attached to the
   ** network the function also returns the name of the CAN interface.
   */
   QString        name(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Nominal bit-rate
   ** \see     nominalBitrateString()
   **
   ** Return the current nominal bit-rate of the selected network in [bits/s]. If the bit-rate value
   ** is not valid (not configured) the function returns QCan::eCAN_BITRATE_NONE.
   */
   int32_t        nominalBitrate(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Nominal bit-rate
   ** \see     nominalBitrate()
   **
   ** Return the current nominal bit-rate of the selected network as string object. If the bit-rate value
   ** is not valid (not configured) the function returns "None".
   */
   QString        nominalBitrateString(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   **
   ** Reset the network.
   */
   void           reset(void);

   void           requestUpdate(void);

   bool           send(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  slNomBitRateV     Nominal bit-rate
   ** \param[in]  slDatBitRateV     Data bit-rate
   **
   ** Set the bit-rate of the network.
   */
   void           setBitrate(int32_t slNomBitRateV, int32_t slDatBitRateV = eCAN_BITRATE_NONE);

   void           setChannel(const CAN_Channel_e teChannelV);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teModeV     Requested CAN mode
   **
   ** Set the CAN mode of the network.
   */
   void           setMode(const CAN_Mode_e teModeV);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  CAN state
   ** \see     stateString()
   **
   ** Return the current CAN state of the selected network.
   */
   CAN_State_e    state(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  CAN state
   ** \see     state()
   **
   ** Return the current CAN state of the selected network as string object.
   */
   QString        stateString(void);

signals:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelV - CAN channel
   ** \param[in]  clNetworkSettingsV - JSON data
   **
   ** This signal is emitted after a new object was received from a QCanNetwork instance. The JSON data
   ** has the following format:
   ** \code
   ** {
   **    "apiVersion": "1.0",
   **    "bitrateData": 1000000,
   **    "bitrateNominal": 500000,
   **    "channel": 8,
   **    "enabled": true,
   **    "errorFrameEnabled": false,
   **    "errorFrameSupport": true,
   **    "flexibleDataEnabled": true,
   **    "flexibleDataSupport": true,
   **    "frameCount": 0,
   **    "frameCountError": 0,
   **    "listenOnlyEnabled": false,
   **    "listenOnlySupport": false,
   **    "name": "CAN 8",
   **    "state": 2
   ** }
   ** \endcode
   */
   void           objectReceived(const CAN_Channel_e teChannelV, QJsonObject clNetworkSettingsV);

private:

   enum State_e {
      /*! Server is crashed                           */
      eSTATE_CRASHED = -2,

      /*! Server is in unknown state.                 */
      eSTATE_UNKNOWN = -1,

      /*! Server is not running.                      */
      eSTATE_INACTIVE = 0,

      /*! Server is active                            */
      eSTATE_ACTIVE   = 1
   };

   QPointer<QWebSocket>    pclWebSocketP;
   State_e                 teServerStateP;
   uint32_t                ulUpdateRateP;

   /* Stores the QCanNetwork settings                 */
   QJsonObject             clJsonNetworkP;

   /* Stores commands send to the QCanNetwork class   */
   QJsonObject             clJsonCommandP;

   CAN_Channel_e           teChannelP;

private slots:
   void           onSocketConnect(void);
   void           onSocketDisconnect(void);
   void           onSocketError(QAbstractSocket::SocketError clErrorV);
   void           onSocketMessageReceived(const QString & clMessageR);

};

#endif // QCAN_NETWORK_SETTINGS_HPP_
