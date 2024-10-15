//====================================================================================================================//
// File:          qcan_plugin_peak.cpp                                                                                //
// Description:   CAN plugin for PCAN Basic library                                                                   //
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

#include "qcan_plugin_peak.hpp"


//--------------------------------------------------------------------------------------------------------------------//
// QCanPluginPeak()                                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanPluginPeak::QCanPluginPeak()
{
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanPluginPeak::QCanPluginPeak()";
   #endif

   // reset number of interfaces
   atsQCanIfPeakP.clear();
   auwPCanChannelP.clear();


   //---------------------------------------------------------------------------------------------------
   // check PCAN Basic lib is available
   //
   if (!pclPcanBasicP.isAvailable())
   {
      qCritical() << "QCanPluginPeak::QCanPluginPeak() CRITICAL: PEAKBasic library is not available or not all functions could be loaded!";
   }
   else
   {
      //----------------------------------------------------------------
      // add in actual version only USB channels
      //
      auwPCanChannelP.append(PCAN_USBBUS1);
      auwPCanChannelP.append(PCAN_USBBUS2);
      auwPCanChannelP.append(PCAN_USBBUS3);
      auwPCanChannelP.append(PCAN_USBBUS4);
      auwPCanChannelP.append(PCAN_USBBUS5);
      auwPCanChannelP.append(PCAN_USBBUS6);
      auwPCanChannelP.append(PCAN_USBBUS7);
      auwPCanChannelP.append(PCAN_USBBUS8);

      //----------------------------------------------------------------
      // more than 8 USB channels only suppoted by Windows DLL
      //
      #ifdef   Q_OS_WIN32
      auwPCanChannelP.append(PCAN_USBBUS9);
      auwPCanChannelP.append(PCAN_USBBUS10);
      auwPCanChannelP.append(PCAN_USBBUS11);
      auwPCanChannelP.append(PCAN_USBBUS12);
      auwPCanChannelP.append(PCAN_USBBUS13);
      auwPCanChannelP.append(PCAN_USBBUS14);
      auwPCanChannelP.append(PCAN_USBBUS15);
      auwPCanChannelP.append(PCAN_USBBUS16);
      #endif
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// ~QCanPeakUsb()                                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanPluginPeak::~QCanPluginPeak()
{
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanPluginPeak::~QCanPluginPeak()";
   #endif

   //---------------------------------------------------------------------------------------------------
   // disconnect all connected interfaces and delete objects
   //
   foreach (QCanIf_ts tsCanIfEntryT, atsQCanIfPeakP)
   {
      if (tsCanIfEntryT.pclQCanInterfacePeak->connectionState() == QCanInterface::ConnectedState)
      {
         tsCanIfEntryT.pclQCanInterfacePeak->disconnect();
      }
      tsCanIfEntryT.pclQCanInterfacePeak->deleteLater();
      tsCanIfEntryT.pclQCanInterfacePeak = nullptr;
   }

   atsQCanIfPeakP.clear();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanPluginPeak::interfaceCount()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
uint8_t QCanPluginPeak::interfaceCount()
{
   //---------------------------------------------------------------------------------------------------
   // check conditions of each channel
   //
   TPCANStatus    tsStatusT;
   uint32_t       ulParmBufferT;
   QCanIf_ts      tsCanIfEntryT;
   bool           btIfIsInListT;

   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanPluginPeak::interfaceCount()";
   #endif

   if (!pclPcanBasicP.isAvailable())
   {
      return 0;
   }

   //---------------------------------------------------------------------------------------------------
   // remove all not connected devices and add new
   //
   for (uint8_t ubPCanChnT = 0; ubPCanChnT < static_cast< uint8_t >(auwPCanChannelP.length()); ubPCanChnT++)
   {
      tsStatusT = pclPcanBasicP.pfnCAN_GetValueP( auwPCanChannelP[ubPCanChnT], PCAN_CHANNEL_CONDITION,
                                                  static_cast< void* >(&ulParmBufferT), sizeof(ulParmBufferT));
      if (tsStatusT != PCAN_ERROR_OK)
      {
         #ifndef QT_NO_DEBUG_OUTPUT
         qDebug() << "QCanPluginPeak::QCanPluginPeak()" << pclPcanBasicP.errorString(tsStatusT);
         #endif
      } 
      else if ((ulParmBufferT & PCAN_CHANNEL_AVAILABLE) == PCAN_CHANNEL_AVAILABLE)
      {
         // -----------------------------------------------
         // check this channel is already in the atsQCanIfPeakP list
         // if so, than do nothing
         // if not, than append this channel and interface to atsQCanIfPeakP
         //
         btIfIsInListT = false;
         foreach (tsCanIfEntryT, atsQCanIfPeakP)
         {
            if (tsCanIfEntryT.uwPCanChannel == auwPCanChannelP[ubPCanChnT])
            {
               btIfIsInListT = true;
               break;
            }
         }

         //------------------------------------------------
         // if new interface is not in the list,
         // than append new entry
         if (btIfIsInListT == false)
         {
            tsCanIfEntryT.uwPCanChannel = auwPCanChannelP[ubPCanChnT];
            tsCanIfEntryT.pclQCanInterfacePeak = new QCanInterfacePeak(auwPCanChannelP[ubPCanChnT]);
            atsQCanIfPeakP.append(tsCanIfEntryT);

            qInfo() << "QCanPluginPeak::QCanPluginPeak() INFO: Add Interface" <<
                       QString::number(atsQCanIfPeakP.length()) <<
                       "with channel value" <<
                       QString::number(auwPCanChannelP[ubPCanChnT],16) <<
                       "[hex].";
         }

      } else
      {
         // Device with given PCAN Channel value is not available or is ocupied

         //------------------------------------------------
         // if this channel is in atsQCanIfPeakP
         //
         for (uint8_t ubCntrT = 0; ubCntrT < static_cast< uint8_t >(atsQCanIfPeakP.length()); ubCntrT++)
         {
            if (atsQCanIfPeakP.at(ubCntrT).uwPCanChannel == auwPCanChannelP[ubPCanChnT])
            {

               qInfo() << "QCanPluginPeak::QCanPluginPeak() INFO: Remove Interface" <<
                          QString::number(ubCntrT) <<
                          "with channel value" <<
                          QString::number(atsQCanIfPeakP.at(ubCntrT).uwPCanChannel,16) <<
                          "[hex] from available list.";

               atsQCanIfPeakP.removeAt(ubCntrT);
               break;
            }
         }
      }
   }

   //----------------------------------------------------------------
   // Length of apclQCanInterfacePeakP corresponds always to
   // the actual available interface number
   //
   return static_cast< uint8_t >(atsQCanIfPeakP.length());
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanPluginPeak::getInterface()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface * QCanPluginPeak::getInterface(uint8_t ubInterfaceV)
{
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanPluginPeak::getInterface(" + QString::number(ubInterfaceV,10) +")";
   #endif

   if (ubInterfaceV < atsQCanIfPeakP.length())
   {
      #ifndef QT_NO_DEBUG_OUTPUT
      qDebug() << "QCanPluginPeak::getInterface() : return pointer to interface" << QString::number(ubInterfaceV);
      #endif

      return (atsQCanIfPeakP.at(ubInterfaceV).pclQCanInterfacePeak);
   }

   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanPluginPeak::getInterface() : interface" << QString::number(ubInterfaceV) << "is not available!";
   #endif

   return nullptr;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanPluginPeak::icon()                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QIcon QCanPluginPeak::icon()
{
   return QIcon(":/images/pcan_256.png");
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanPluginPeak::name()                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanPluginPeak::name()
{
   return QString("PEAK PCANBasic");
}


