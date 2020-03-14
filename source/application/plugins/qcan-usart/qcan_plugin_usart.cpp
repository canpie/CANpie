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

#include "qcan_plugin_usart.hpp"

//----------------------------------------------------------------------------//
// QCanPluginUsart()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
QCanPluginUsart::QCanPluginUsart()
{
   qDebug() << "QCanPluginUsart::QCanPluginUsart()";

   // reset number of interfaces
   apclQCanIfUsartP.clear();

   //----------------------------------------------------------------
   // check PCAN Basic lib is available
   //
   if (!pclCpUsartP.isAvailable())
   {
      qCritical() << "QCanPluginUsart::QCanPluginUsart() CRITICAL: PEAKBasic library is not available or not all functions could be loaded!";
   }
}

//----------------------------------------------------------------------------//
// ~QCanPeakUsb()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
QCanPluginUsart::~QCanPluginUsart()
{
   qDebug() << "QCanPluginUsart::~QCanPluginUsart()";

   //----------------------------------------------------------------
   // disconnect all connected interfaces and delete objects
   //
   foreach (QCanInterfaceUsart *pclCanIfEntryT, apclQCanIfUsartP)
   {
      if (pclCanIfEntryT->connectionState() == QCanInterface::ConnectedState)
      {
         pclCanIfEntryT->disconnect();
      }
      pclCanIfEntryT->deleteLater();
      pclCanIfEntryT = NULL;
   }

   apclQCanIfUsartP.clear();
}

//----------------------------------------------------------------------------//
// interfaceCount()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t QCanPluginUsart::interfaceCount()
{
   //----------------------------------------------------------------
   // check conditions of each channel
   //
   bool           btIfIsInListT;

   qDebug() << "QCanPluginUsart::interfaceCount()";

   if (!pclCpUsartP.isAvailable())
   {
      return 0;
   }

   QSerialPort *pclSerialPort;
   uint16_t uwDeviceNrT = 0;
   foreach (const QSerialPortInfo &clSerialIfInfoT, QSerialPortInfo::availablePorts())
   {
      qDebug() << "Found USART interface: " << clSerialIfInfoT.portName();

      pclSerialPort = new QSerialPort(clSerialIfInfoT);
      if (pclSerialPort->error() != 0)
      {
         qDebug() << "QCanPluginUsart::QCanPluginUsart(): " << QString::number(pclSerialPort->error(),16);
         qDebug() << "Error: " << pclSerialPort->errorString();
      } else
      {
         qDebug() << "Search list for existing name " << clSerialIfInfoT.portName() << ", list contains " << apclQCanIfUsartP.length() << " COM interfaces";

         // -----------------------------------------------
         // check this channel is already in the apclQCanIfUsartP list
         // if so, than do nothing
         // if not, than append this channel and interface to apclQCanIfUsartP
         //
         btIfIsInListT = false;
         if (apclQCanIfUsartP.length() > 0)
         {
            foreach (QCanInterfaceUsart *pclCanIfEntryT, apclQCanIfUsartP)
            {
               if (pclCanIfEntryT->name() == clSerialIfInfoT.portName())
               {
                  btIfIsInListT = true;
                  qDebug() << "Device is already in list...";
                  break;
               }
            }
         }

         //------------------------------------------------
         // if new interface is not in the list, than append new entry
         if (btIfIsInListT == false)
         {
            qDebug() << "Device exists not in list, add it... ";
            apclQCanIfUsartP.append(new QCanInterfaceUsart(uwDeviceNrT, clSerialIfInfoT.portName()));

            qInfo() << "QCanPluginUsart::QCanPluginUsart() INFO: Add Interface" << QString::number(apclQCanIfUsartP.length());
         }
      }
   }

   //----------------------------------------------------------------
   // Length of apclQCanInterfaceUsartP corresponds always to
   // the actual available interface number
   //
   qDebug() << "return number of interfaces" << QString::number((uint8_t)apclQCanIfUsartP.length(),10);
   return (uint8_t)apclQCanIfUsartP.length();
}

//----------------------------------------------------------------------------//
// getInterface()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface * QCanPluginUsart::getInterface(uint8_t ubInterfaceV)
{
   qDebug() << "QCanPluginUsart::getInterface(" + QString::number(ubInterfaceV,10) +")";
   if (ubInterfaceV < apclQCanIfUsartP.length())
   {
      qDebug() << "QCanPluginUsart::getInterface() INFO: return pointer to interface" << QString::number(ubInterfaceV);

      return (apclQCanIfUsartP.at(ubInterfaceV));
   }

   qDebug() << "QCanPluginUsart::getInterface() CRITICAL: interface" << QString::number(ubInterfaceV) << "is not available!";

   return NULL;
}


//----------------------------------------------------------------------------//
// icon()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QIcon QCanPluginUsart::icon()
{
   return QIcon(":/images/usart_plugin_256.png");
}


//----------------------------------------------------------------------------//
// name()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QString QCanPluginUsart::name()
{
   return QString("USART CANpie");
}


