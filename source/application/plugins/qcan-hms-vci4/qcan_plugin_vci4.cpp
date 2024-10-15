//====================================================================================================================//
// File:          qcan_plugin_vci4.cpp                                                                                //
// Description:   Access to the VCI4 library                                                                          //
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

#include "qcan_plugin_vci4.hpp"

#include <QtCore/QCoreApplication>


/*--------------------------------------------------------------------------------------------------------------------*\
** Function implementation                                                                                            **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// QCanPluginVci4::QCanPluginVci4()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanPluginVci4::QCanPluginVci4()
{
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanPluginVci4::QCanPluginVci4()";
   #endif

   //---------------------------------------------------------------------------------------------------
   // create/open logging file
   //
   pclLogFileP = new QFile(QCoreApplication::applicationDirPath() + "/qcan_hms-vci4_plugin.log");
   pclLogFileP->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanPluginVci4::~QCanPluginVci4()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanPluginVci4::~QCanPluginVci4()
{
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanPluginVci4::~QCanPluginVci4()";
   #endif

   //---------------------------------------------------------------------------------------------------
   // close the previously opened log file
   //
   if(pclLogFileP->isOpen())
   {
      pclLogFileP->close();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanPluginVci4::interfaceCount()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
uint8_t QCanPluginVci4::interfaceCount()
{
   HRESULT              clResultT; // error code
   IVciDeviceManager *  pclDevMgrT      = nullptr;    // device manager
   IVciEnumDevice *     pclEnumeratorT  = nullptr;    // enumerator handle

   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanPluginVci4::interfaceCount()";
   #endif

   //---------------------------------------------------------------------------------------------------
   // use device manager to get access to the connected devices
   //
   clResultT = VciGetDeviceManager(&pclDevMgrT);
   if (clResultT == VCI_OK)
   {
      clResultT = pclDevMgrT->EnumDevices(&pclEnumeratorT);
   }

   //---------------------------------------------------------------------------------------------------
   // Get actual Number of devices by providing pointer to ulActualDeviceNumberT and store all
   // device information in to aclCanDevInfoP
   //
   uint32_t ulActualDeviceNumberT = 0;
   aclCanDevInfoP.resize(10); // accept maximal 10 entries
   clResultT = pclEnumeratorT->Next(10, &aclCanDevInfoP[0], &ulActualDeviceNumberT);
   if (clResultT != VCI_OK)
   {
      qCritical() << "Fail to evaluate the number of availbale devices";
   }

   // adjust the size of the list to the actual
   aclCanDevInfoP.resize(static_cast<int>(ulActualDeviceNumberT));

   //---------------------------------------------------------------------------------------------------
   // release device enumerator and device manager
   //
   pclEnumeratorT->Release();
   pclDevMgrT->Release();

   log("Number of available HMS VCI4 devices: " + QString::number(ulActualDeviceNumberT));

   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << tr("QCanPluginVci4::interfaceCount()  INFO: Total number of found devices is ") +
              QString::number(aclCanDevInfoP.size(),10);
   #endif

   return static_cast<uint8_t>(aclCanDevInfoP.size());
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanPluginVci4::getInterface()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface * QCanPluginVci4::getInterface(uint8_t ubInterfaceV)
{
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanPluginVci4::getInterface()";
   #endif
   
   if (ubInterfaceV < aclCanDevInfoP.length())
   {
      qInfo() << "QCanPluginVci4::getInterface() INFO: return pointer to interface" << QString::number(ubInterfaceV);

      log("Use HMS VCI4 interface: " + QString::fromLocal8Bit(aclCanDevInfoP[ubInterfaceV].Description) + 
          " " + QString::fromLocal8Bit(aclCanDevInfoP[ubInterfaceV].UniqueHardwareId.AsChar));

      return new QCanInterfaceVci(aclCanDevInfoP[ubInterfaceV]);
   }

   qCritical() << "QCanPluginVci4::getInterface() CRITICAL: interface" << QString::number(ubInterfaceV) << "is not available!";

   return nullptr;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanPluginVci4::icon()                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QIcon QCanPluginVci4::icon()
{
   return QIcon(":/images/mc_can_hms.png");
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanPluginVci4::name()                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanPluginVci4::name()
{
   return QString("HMS VCI 4");
}

//--------------------------------------------------------------------------------------------------------------------//
// QCanPluginVci4::log()                                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanPluginVci4::log(QString clValueV)
{
   if(pclLogFileP->isOpen())
   {
      pclLogFileP->write(qPrintable(clValueV + "\n"));
      if (pclLogFileP->flush() != true)
      {
         qCritical() << "Fail to flush log file " << pclLogFileP->fileName();
      }
   }
}



