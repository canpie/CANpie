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

#include <QtCore/QSharedMemory>

#include "qcan_namespace.hpp"

using namespace QCan;

//----------------------------------------------------------------------------------------------------------------
/*!
** \class QCanNetworkSettings
**
** The QCanNetworkSettings provides the functionality to access parameters of a local QCanNetwork.
**
*/
class QCanNetworkSettings
{

public:
   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelV - CAN channel
   **
   ** Construct a QCanNetworkSettings object for the channel \a teChannelV.
   */
   QCanNetworkSettings(const CAN_Channel_e teChannelV = eCAN_CHANNEL_1);

   ~QCanNetworkSettings();


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
   ** \param[in]  teChannelR - CAN channel
   **
   ** Set the CAN channel for the QCanNetworkSettings object.
   */
   inline void    setChannel(const CAN_Channel_e & teChannelR) { teChannelP = teChannelR;    };


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

private:
   QSharedMemory *   pclSettingsP;
   CAN_Channel_e     teChannelP;
};

#endif // QCAN_NETWORK_SETTINGS_HPP_
