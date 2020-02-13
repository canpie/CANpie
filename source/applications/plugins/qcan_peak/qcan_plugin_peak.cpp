//============================================================================//
// File:          qcan_plugin_peak.cpp                                        //
// Description:   CAN plugin for PCAN Basic library                           //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53842 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions         //
// are met:                                                                   //
// 1. Redistributions of source code must retain the above copyright          //
//    notice, this list of conditions, the following disclaimer and           //
//    the referenced file 'COPYING'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'COPYING' file.                   //
//                                                                            //
//============================================================================//

#include "qcan_plugin_peak.hpp"

//----------------------------------------------------------------------------//
// QCanPluginPeak()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
QCanPluginPeak::QCanPluginPeak()
{
   qDebug() << "QCanPluginPeak::QCanPluginPeak()";

   // reset number of interfaces
//   apclQCanInterfacePeakP.clear();
   atsQCanIfPeakP.clear();
   auwPCanChannelP.clear();

   //----------------------------------------------------------------
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

//----------------------------------------------------------------------------//
// ~QCanPeakUsb()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
QCanPluginPeak::~QCanPluginPeak()
{
   qDebug() << "QCanPluginPeak::~QCanPluginPeak()";

   //----------------------------------------------------------------
   // disconnect all connected interfaces and delete objects
   //
   foreach (QCanIf_ts tsCanIfEntryT, atsQCanIfPeakP)
   {
      if (tsCanIfEntryT.pclQCanInterfacePeak->connectionState() == QCanInterface::ConnectedState)
      {
         tsCanIfEntryT.pclQCanInterfacePeak->disconnect();
      }
      tsCanIfEntryT.pclQCanInterfacePeak->deleteLater();
      tsCanIfEntryT.pclQCanInterfacePeak = NULL;
   }

   atsQCanIfPeakP.clear();
}

//----------------------------------------------------------------------------//
// interfaceCount()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t QCanPluginPeak::interfaceCount()
{
   //----------------------------------------------------------------
   // check conditions of each channel
   //
   TPCANStatus    tsStatusT;
   uint32_t       ulParmBufferT;
   QCanIf_ts      tsCanIfEntryT;
   bool           btIfIsInListT;

   qDebug() << "QCanPluginPeak::interfaceCount()";

   if (!pclPcanBasicP.isAvailable())
   {
      return 0;
   }

   //----------------------------------------------------------------
   // remove all not connected devices and add new
   //
   for (uint8_t ubPCanChnT = 0; ubPCanChnT < (uint8_t)auwPCanChannelP.length(); ubPCanChnT++)
   {
      tsStatusT = pclPcanBasicP.pfnCAN_GetValueP(auwPCanChannelP[ubPCanChnT], PCAN_CHANNEL_CONDITION,
                                         (void*)&ulParmBufferT, sizeof(ulParmBufferT));
      if (tsStatusT != PCAN_ERROR_OK)
      {
         qDebug() << "QCanPluginPeak::QCanPluginPeak()" << pclPcanBasicP.formatedError(tsStatusT);
      } else if ((ulParmBufferT & PCAN_CHANNEL_AVAILABLE) == PCAN_CHANNEL_AVAILABLE)
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
         for (uint8_t ubCntrT = 0; ubCntrT < (uint8_t)atsQCanIfPeakP.length(); ubCntrT++)
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
   return (uint8_t)atsQCanIfPeakP.length();
}

//----------------------------------------------------------------------------//
// getInterface()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface * QCanPluginPeak::getInterface(uint8_t ubInterfaceV)
{
   qDebug() << "QCanPluginPeak::getInterface(" + QString::number(ubInterfaceV,10) +")";
   if (ubInterfaceV < atsQCanIfPeakP.length())
   {
      qDebug() << "QCanPluginPeak::getInterface() INFO: return pointer to interface" << QString::number(ubInterfaceV);

      return (atsQCanIfPeakP.at(ubInterfaceV).pclQCanInterfacePeak);
   }

   qDebug() << "QCanPluginPeak::getInterface() CRITICAL: interface" << QString::number(ubInterfaceV) << "is not available!";

   return NULL;
}


//----------------------------------------------------------------------------//
// icon()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QIcon QCanPluginPeak::icon()
{
   return QIcon(":/images/pcan_256.png");
}


//----------------------------------------------------------------------------//
// name()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QString QCanPluginPeak::name()
{
   return QString("PEAK PCANBasic");
}


