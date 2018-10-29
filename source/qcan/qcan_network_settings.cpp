//====================================================================================================================//
// File:          qcan_network_settings.cpp                                                                           //
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


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QDebug>
#include <QtCore/QDateTime>

#include "qcan_server_memory.hpp"
#include "qcan_network_settings.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/




//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::QCanNetworkSettings()                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanNetworkSettings::QCanNetworkSettings(const CAN_Channel_e teChannelV)
{
   pclSettingsP = new QSharedMemory(QString(QCAN_MEMORY_KEY));
   pclSettingsP->attach(QSharedMemory::ReadOnly);

   teChannelP = teChannelV;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::~QCanNetworkSettings()                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanNetworkSettings::~QCanNetworkSettings()
{
   pclSettingsP->detach();
   delete (pclSettingsP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::dataBitrate()                                                                                 //
// return the data bit-rate of the current CAN interface                                                              //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanNetworkSettings::dataBitrate(void)
{
   int32_t  slResultT = eCAN_BITRATE_NONE;

   if (pclSettingsP->isAttached())
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

      //-------------------------------------------------------------------------------------------
      // get data bit-rate value
      //
      slResultT = ptsSettingsT->atsNetwork[teChannelP - 1].slDatBitRate;
   }

   return (slResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::dataBitrateString()                                                                           //
// return the data bit-rate of the current CAN interface                                                              //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanNetworkSettings::dataBitrateString(void)
{
   QString  clResultT;
   int32_t  slBitrateT;

   slBitrateT = dataBitrate();

   //---------------------------------------------------------------------------------------------------
   // Test for unknown bit-rate first
   //
   if (slBitrateT == eCAN_BITRATE_NONE)
   {
      clResultT = "None";
   }
   else
   {
      if (slBitrateT < 1000000)
      {
         //-----------------------------------------------------------------------------------
         // print values < 1000000 in kBit/s
         //
         clResultT = QString("%1 kBit/s").arg(slBitrateT / 1000);
      }
      else
      {
         //------------------------------------------------
         // print values >= 1000000 in MBit/s
         //
         clResultT = QString("%1 MBit/s").arg(slBitrateT / 1000000);
      }

   }
   return (clResultT);
}

//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::name()                                                                                        //
// return the name of the current CAN interface                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanNetworkSettings::name(void)
{
   QString clResultT;

   if (pclSettingsP->isAttached())
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

      //-------------------------------------------------------------------------------------------
      // calculate the size of the interface name, a name
      //
      int slSizeT = strlen(ptsSettingsT->atsNetwork[teChannelP - 1].szInterfaceName);
      if (slSizeT > QCAN_IF_NAME_LENGTH)
      {
         slSizeT = QCAN_IF_NAME_LENGTH;
      }

      clResultT = QString::fromLatin1(&(ptsSettingsT->atsNetwork[teChannelP - 1].szInterfaceName[0]), slSizeT);

   }

   return (clResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::nominalBitrate()                                                                              //
// return the nominal bit-rate of the current CAN interface                                                           //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanNetworkSettings::nominalBitrate(void)
{
   int32_t  slResultT = eCAN_BITRATE_NONE;

   if (pclSettingsP->isAttached())
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

      //-------------------------------------------------------------------------------------------
      // get nominal bit-rate value
      //
      slResultT = ptsSettingsT->atsNetwork[teChannelP - 1].slNomBitRate;
   }

   return (slResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::nominalBitrateString()                                                                        //
// return the nominal bit-rate of the current CAN interface                                                           //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanNetworkSettings::nominalBitrateString(void)
{
   QString  clResultT;
   int32_t  slBitrateT;

   slBitrateT = nominalBitrate();

   //---------------------------------------------------------------------------------------------------
   // Test for unknown bit-rate first
   //
   if (slBitrateT == eCAN_BITRATE_NONE)
   {
      clResultT = "None";
   }
   else
   {
      if (slBitrateT < 1000000)
      {
         //-----------------------------------------------------------------------------------
         // print values < 1000000 in kBit/s
         //
         clResultT = QString("%1 kBit/s").arg(slBitrateT / 1000);
      }
      else
      {
         //------------------------------------------------
         // print values >= 1000000 in MBit/s
         //
         clResultT = QString("%1 MBit/s").arg(slBitrateT / 1000000);
      }

   }
   return (clResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::state()                                                                                       //
// return CAN network state                                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
CAN_State_e QCanNetworkSettings::state(void)
{
   CAN_State_e teResultT = eCAN_STATE_STOPPED;

   if (pclSettingsP->isAttached())
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

      //-------------------------------------------------------------------------------------------
      // get CAN state value
      //
      teResultT = (CAN_State_e)(ptsSettingsT->atsNetwork[teChannelP - 1].slStatus);
   }

   return (teResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetworkSettings::stateString()                                                                                 //
// return CAN network state                                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanNetworkSettings::stateString(void)
{
   QString clResultT;

   switch(state())
   {
      case eCAN_STATE_STOPPED:
         clResultT = "Stopped";
         break;

      case eCAN_STATE_SLEEPING:
         clResultT = "Sleeping";
         break;

      case eCAN_STATE_BUS_ACTIVE:
         clResultT = "Error active";
         break;

      case eCAN_STATE_BUS_WARN:
         clResultT = "Warning";
         break;

      case eCAN_STATE_BUS_PASSIVE:
         clResultT = "Error passive";
         break;

      case eCAN_STATE_BUS_OFF:
         clResultT = "Bus-off";
         break;

      default:
         clResultT = "Unknown";
         break;

   }

   return (clResultT);
}

