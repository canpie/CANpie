//============================================================================//
// File:          qcan_plugin_ixxat.cpp                                       //
// Description:   CAN plugin for IXXAT library                                //
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


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include "qcan_plugin_ixxat.hpp"


/*----------------------------------------------------------------------------*\
** Function implementation                                                    **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanPluginIxxat()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
QCanPluginIxxat::QCanPluginIxxat()
{
   qDebug() << "QCanPluginIxxat::QCanPluginIxxat()";

   //----------------------------------------------------------------
   // check PCAN Basic lib is available
   //
   if (!pclIxxatVciP.isAvailable())
   {
      qCritical() << "QCanPluginIxxat::QCanPluginIxxat() CRITICAL: IXXAT VCI library is not available or not all functions could be loaded!";
   }
}

//----------------------------------------------------------------------------//
// ~QCanPluginIxxat()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
QCanPluginIxxat::~QCanPluginIxxat()
{
   qDebug() << "QCanPluginIxxat::~QCanPluginIxxat()";
}

//----------------------------------------------------------------------------//
// interfaceCount()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t QCanPluginIxxat::interfaceCount()
{
   //----------------------------------------------------------------
   // check PCAN Basic lib is available
   //
   if (!pclIxxatVciP.isAvailable())
   {
      qWarning() << tr("WARNING: No lib available!");
      return 0;
   }

   //----------------------------------------------------------------
   // Try to open device list
   //
   if (pclIxxatVciP.pfnVciEnumDeviceOpenP(&vdDevListP) != VCI_OK)
   {
      qWarning() << tr("WARNING: Can't open device list!") + QString::number(pclIxxatVciP.pfnVciEnumDeviceOpenP(&vdDevListP),16);
   }

   //----------------------------------------------------------------
   // Count number of connected devices
   //
   VCIDEVICEINFO clCanDevT;
   uint8_t ubDevCntrT = 0;
   while (pclIxxatVciP.pfnVciEnumDeviceNextP(vdDevListP,&clCanDevT) == VCI_OK)
   {
      ubDevCntrT++;
   }

   if (!ubDevCntrT)
   {
      qWarning() << tr("WARNING: No devices are connected!");
      return 0;
   }

   // create a device list for easy access
   pclIxxatVciP.pfnVciEnumDeviceResetP(vdDevListP);
   aclCanDevInfoP.resize(ubDevCntrT);
   while (ubDevCntrT)
   {
      ubDevCntrT--;

      pclIxxatVciP.pfnVciEnumDeviceNextP(vdDevListP,&aclCanDevInfoP[ubDevCntrT]);
      qInfo() << tr("INFO: Device '") + aclCanDevInfoP[ubDevCntrT].Description + tr("' found");
   }

   qInfo() << tr("INFO: Total number of found devices is ") + QString::number(aclCanDevInfoP.size(),10);


   return (uint8_t)aclCanDevInfoP.size();
}

//----------------------------------------------------------------------------//
// getInterface()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface * QCanPluginIxxat::getInterface(uint8_t ubInterfaceV)
{
   if (ubInterfaceV < aclCanDevInfoP.length())
   {
      qInfo() << "QCanPluginIxxat::getInterface() INFO: return pointer to interface" << QString::number(ubInterfaceV);

      return new QCanInterfaceIxxat(aclCanDevInfoP[ubInterfaceV]);
   }

   qCritical() << "QCanPluginIxxat::getInterface() CRITICAL: interface" << QString::number(ubInterfaceV) << "is not available!";


   return NULL; ;
}


//----------------------------------------------------------------------------//
// icon()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QIcon QCanPluginIxxat::icon()
{
   return QIcon(":/images/ixxat.ico");
}


//----------------------------------------------------------------------------//
// name()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QString QCanPluginIxxat::name()
{
   return QString("IXXAT VCI 3.5");
}


