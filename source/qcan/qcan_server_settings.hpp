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

#include <QtCore/QPointer>
#include <QtCore/QSharedMemory>

#include <QtNetwork/QLocalSocket>

#include "qcan_namespace.hpp"

using namespace QCan;

//----------------------------------------------------------------------------------------------------------------
/*!
** \class QCanServerSettings
**
** The QCanServerSettings provides the functionality to test the current state of a local QCanServer and
** each individual QCanNetwork within this server.
** <p>
** The following code shows an example how to test for a local QCanServer:
** \code
** QCanServerSettings   clServerSettingsT;
** if (clServerSettingsT.state() < QCanServerSettings::eSTATE_ACTIVE)
** {
**    fprintf(stdout, "CANpie FD server %s \n", qPrintable(clServerSettingsT.stateString()));
**    exit(0);
** }
**
** \endcode
*/
class QCanServerSettings : public QObject
{
   Q_OBJECT

public:
   //---------------------------------------------------------------------------------------------------
   /*!
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
      /*! Server is crashed                           */
      eSTATE_CRASHED = -2,

      /*! Server is in unknown state.                 */
      eSTATE_UNKNOWN = -1,

      /*! Server is not running.                      */
      eSTATE_INACTIVE = 0,

      /*! Server is active                            */
      eSTATE_ACTIVE   = 1
   };


   uint8_t        networkBusLoad(const CAN_Channel_e teChannelV = eCAN_CHANNEL_1);

   int32_t        networkConfiguration(const CAN_Channel_e teChannelV = eCAN_CHANNEL_1);

   QString        networkConfigurationString(const CAN_Channel_e teChannelV = eCAN_CHANNEL_1);

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
   ** \return  Data bit-rate
   ** \see     networkDataBitrateString()
   **
   ** Return the current data bit-rate of the selected network in [bits/s]. If the bit-rate value
   ** is not valid (not configured) the function returns QCan::eCAN_BITRATE_NONE. The result of this
   ** function is equivalent to QCanNetwork::dataBitrate().
   */
   int32_t        networkDataBitrate(const CAN_Channel_e teChannelV = eCAN_CHANNEL_1);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Data bit-rate
   ** \see     networkDataBitrate()
   **
   ** Return the current data bit-rate of the selected network as string object. If the bit-rate value
   ** is not valid (not configured) the function returns "None".
   */
   QString        networkDataBitrateString(const CAN_Channel_e teChannelV = eCAN_CHANNEL_1);

   uint32_t       networkErrorCount(const CAN_Channel_e teChannelV = eCAN_CHANNEL_1);

   uint32_t       networkFrameCount(const CAN_Channel_e teChannelV = eCAN_CHANNEL_1);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelV     CAN channel
   **
   ** \return     Network name
   **
   ** Return the name of the selected network as string object. If a CAN interface is attached to the
   ** network the function also returns the name of the CAN interface.
   */
   QString        networkName(const CAN_Channel_e teChannelV = eCAN_CHANNEL_1);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelV     CAN channel
   **
   ** \return     Nominal bit-rate
   ** \see        networkNominalBitrateString()
   **
   ** Return the current nominal bit-rate of the selected network in [bits/s]. If the bit-rate value
   ** is not valid (not configured) the function returns QCan::eCAN_BITRATE_NONE.
   */
   int32_t        networkNominalBitrate(const CAN_Channel_e teChannelV = eCAN_CHANNEL_1);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelV     CAN channel
   ** \return     Nominal bit-rate
   ** \see        networkNominalBitrate()
   **
   ** Return the current nominal bit-rate of the selected network as string object. If the bit-rate value
   ** is not valid (not configured) the function returns "None".
   */
   QString        networkNominalBitrateString(const CAN_Channel_e teChannelV = eCAN_CHANNEL_1);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  CAN state
   ** \see     networkStateString()
   **
   ** Return the current CAN state of the selected network.
   */
   CAN_State_e    networkState(const CAN_Channel_e teChannelV = eCAN_CHANNEL_1);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  CAN state
   ** \see     networkState()
   **
   ** Return the current CAN state of the selected network as string object.
   */
   QString        networkStateString(const CAN_Channel_e teChannelV = eCAN_CHANNEL_1);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelV     CAN channel
   **
   ** Set the CAN mode of the network denoted by parameter \a teChannelV.
   */
   bool           resetNetwork(const CAN_Channel_e teChannelV);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelV     CAN channel
   **
   ** Set the bit-rate of the network denoted by parameter \a teChannelV.
   */
   bool           setNetworkBitrate(const CAN_Channel_e teChannelV,
                                    int32_t slNomBitRateV, int32_t slDatBitRateV = eCAN_BITRATE_NONE);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelV     CAN channel
   **
   ** Set the CAN mode of the network denoted by parameter \a teChannelV.
   */
   bool           setNetworkMode(const CAN_Channel_e teChannelV, const CAN_Mode_e teModeV);

   bool           startServer(void);

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
   **
   ** Return the build version of the active QCanServer.
   */
   int32_t        versionBuild(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Major version
   **
   ** Return the major version of the active QCanServer.
   */
   int32_t        versionMajor(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Minor version
   **
   ** Return the minor version of the active QCanServer.
   */
   int32_t        versionMinor(void);

private:
   QSharedMemory *         pclSettingsP;

   QPointer<QLocalSocket>  pclLocalSockP;
   bool                    btIsConnectedP;
   int32_t                 slSocketErrorP;

private slots:
   void           onSocketConnect(void);
   void           onSocketDisconnect(void);
   void           onSocketError(QLocalSocket::LocalSocketError teSocketErrorV);
   void           onSocketReceive(void);

};

#endif // QCAN_SERVER_SETTINGS_HPP_
