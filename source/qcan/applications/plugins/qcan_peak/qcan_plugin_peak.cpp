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
// QCanPeakUsb()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
QCanPluginPeak::QCanPluginPeak()
{
   qDebug() << "QCanPluginPeak::QCanPluginPeak()";

   // reset number of interfaces
   apclQCanInterfacePeakP.clear();

   //----------------------------------------------------------------
   // check PCAN Basic lib is available
   //
   if (!pclPcanBasicP.isAvailable())
   {
      qCritical() << "QCanPluginPeak::QCanPluginPeak() CRITICAL: PEAKBasic library is not available or not all functions could be loaded!";
   }

   //----------------------------------------------------------------
   // add in actual version only USB channels
   //
   auwPCanChannelP.clear();
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

//----------------------------------------------------------------------------//
// ~QCanPeakUsb()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
QCanPluginPeak::~QCanPluginPeak()
{
   qDebug() << "QCanPluginPeak::~QCanPluginPeak()";
   apclQCanInterfacePeakP.clear();
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
   TPCANStatus tsStatusT;
   uint32_t    ulParmBufferT;
   apclQCanInterfacePeakP.clear();
   for (uint8_t ubCntrT = 0; ubCntrT < auwPCanChannelP.length(); ubCntrT++)
   {
      tsStatusT = pclPcanBasicP.getValue(auwPCanChannelP[ubCntrT], PCAN_CHANNEL_CONDITION,
                                         (void*)&ulParmBufferT, sizeof(ulParmBufferT));
      if (tsStatusT != PCAN_ERROR_OK)
      {
         qDebug() << "QCanPluginPeak::QCanPluginPeak()" << pclPcanBasicP.formatedError(tsStatusT);
      } else if ((ulParmBufferT & PCAN_CHANNEL_AVAILABLE) == PCAN_CHANNEL_AVAILABLE)
      {
         apclQCanInterfacePeakP.append(new QCanInterfacePeak(auwPCanChannelP[ubCntrT]));

         qInfo() << "QCanPluginPeak::QCanPluginPeak() SUCCESS: Interface" <<
                    QString::number(apclQCanInterfacePeakP.length()) <<
                    "with channel value" <<
                    QString::number(auwPCanChannelP[ubCntrT],16) <<
                    "[hex] is available!";
      } else
      {
         // Device with given PCAN Channel value is not available
      }
   }

   return (uint8_t)apclQCanInterfacePeakP.length();
}

//----------------------------------------------------------------------------//
// getInterface()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface * QCanPluginPeak::getInterface(uint8_t ubInterfaceV)
{
   if (ubInterfaceV < apclQCanInterfacePeakP.length())
   {
      qInfo() << "QCanPluginPeak::getInterface() INFO: return pointer to interface" << QString::number(ubInterfaceV);

      return (apclQCanInterfacePeakP.at(ubInterfaceV));
   }

   qCritical() << "QCanPluginPeak::getInterface() CRITICAL: interface" << QString::number(ubInterfaceV) << "is not available!";

   return NULL;
}


//----------------------------------------------------------------------------//
// icon()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QIcon QCanPluginPeak::icon()
{
   return QIcon(":/images/pcan.png");
}


//----------------------------------------------------------------------------//
// name()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QString QCanPluginPeak::name()
{
   return QString("PEAK PCANBasic");
}


