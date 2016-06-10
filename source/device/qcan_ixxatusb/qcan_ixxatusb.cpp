//============================================================================//
// File:          qcan_ixxatusb.cpp                                           //
// Description:   CAN plugin for IXXAT USB device                             //
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

#include <QtWidgets>
#include <qcan_ixxatusb.hpp>

/*!
** \var aszErrTextG
** Zero terminated string buffer to print error descriptions
*/
char aszErrTextG[128];

QString QCanIxxatUsb::echo(const QString &message)
{
   uint32_t ulStatusT;
   QString clRetStringT;


   aszErrTextG[0] = 0;
   aszErrTextG[1] = 0;
   aszErrTextG[2] = 0;

   emit errorOccurred(17);
   if (message == "load")
   {
      connect();
      clRetStringT = ";)";
   }

   if (message == "parm")
   {
//      ulStatusT = pfnCAN_GetValueP(uwCanChannelP, PCAN_DEVICE_NUMBER, (void *)aszErrTextG, 100);
//      clRetStringT = (" " + QString::number(ulStatusT,16));
//      if (ulStatusT != PCAN_ERROR_OK)
//      {
//         // get default description string of error code
////         pfnCAN_GetErrorTextP(ulStatusT,0x00,aszErrTextG);
//         qCritical() << "ERROR: " << QString(QLatin1String(aszErrTextG)) << " Number: " << QString::number(aszErrTextG[0],16);
//      }
//      qDebug() << "Number: " << QString::number(aszErrTextG[0],16);
   }

   if (message == "status")
   {
//      ulStatusT = pfnCAN_GetStatusP(uwCanChannelP);
//      if (ulStatusT != PCAN_ERROR_OK)
//      {
//         // get default description string of error code
//         pfnCAN_GetErrorTextP(ulStatusT,0x00,aszErrTextG);
//      }

   }

   if (message == "bitrate")
   {
      ulStatusT = setBitrate(eBITRATE_500K, 0);
   }

   clRetStringT = (" " + QString::number(ulStatusT,16) + "h");
  QString clRetString2T(aszErrTextG);

//   clRetStringT.prepend(message + " ");
   return ("ixxat: " + message + clRetStringT +  " ("+ clRetString2T + ")");
}

int32_t QCanIxxatUsb::setBitrate(uint32_t ulBitrateV, uint32_t ulBrsClockV)
{
    return 0;
}

int32_t	QCanIxxatUsb::setMode(const uint32_t ulModeV)
{
   return 0;
}

int32_t	QCanIxxatUsb::state(void)
{
   return 0;
}

int32_t	QCanIxxatUsb::statistic(QCanStatistic_ts &clStatisticR)
{
   return 0;
}

int32_t	QCanIxxatUsb::read(QCanFrame &clFrameR)
{
   return 0;
}

int32_t	QCanIxxatUsb::write(const QCanFrame &clFrameR)
{
   return 0;
}

int32_t QCanIxxatUsb::connect(void)
{
   //-----------------------------------------------------------------------
   // Loads library
   //
   clCanLibP.setFileName(QCAN_IXXATLIB);

   if (!clCanLibP.load()) {
       qCritical() << tr("Failed to load the library: ") + qPrintable(clCanLibP.fileName());
       return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // Loads API functions
   //
   pfnVciEnumDeviceOpenP = (vciEnumDeviceOpen_tf) clCanLibP.resolve("vciEnumDeviceOpen");
//   pfnVciInitializeP = (VciInitialize_tf) clCanLibP.resolve("vciEnumDeviceOpen");
 //   pfnCAN_InitializeFDP = (CAN_InitializeFD_tf) clCanLibP.resolve("CAN_InitializeFD");
//   pfnCAN_UninitializeP = (CAN_Uninitialize_tf)clCanLibP.resolve("CAN_Uninitialize");
//   pfnCAN_ResetP = (CAN_Reset_tf)clCanLibP.resolve("CAN_Reset");
//   pfnCAN_GetStatusP = (CAN_GetStatus_tf)clCanLibP.resolve("CAN_GetStatus");
//   pfnCAN_ReadP = (CAN_Read_tf)clCanLibP.resolve("CAN_Read");
//   pfnCAN_ReadFDP = (CAN_ReadFD_tf)clCanLibP.resolve("CAN_ReadFD");
//   pfnCAN_WriteP = (CAN_Write_tf)clCanLibP.resolve("CAN_Write");
//   pfnCAN_WriteFDP = (CAN_WriteFD_tf)clCanLibP.resolve("CAN_WriteFD");
//   pfnCAN_FilterMessagesP = (CAN_FilterMessages_tf)clCanLibP.resolve("CAN_FilterMessages");
//   pfnCAN_GetValueP = (CAN_GetValue_tf)clCanLibP.resolve("CAN_GetValue");
//   pfnCAN_SetValueP = (CAN_SetValue_tf)clCanLibP.resolve("CAN_SetValue");
//   pfnCAN_GetErrorTextP = (CAN_GetErrorText_tf)clCanLibP.resolve("CAN_GetErrorText");


   //----------------------------------------------------------------
   // check for success
   //
   btLibFuncLoadP = pfnVciEnumDeviceOpenP;
//         && pfnCAN_InitializeFDP &&
//                     pfnCAN_UninitializeP && pfnCAN_ResetP &&
//                     pfnCAN_GetStatusP && pfnCAN_ReadP  &&
//                     pfnCAN_ReadFDP && pfnCAN_WriteP &&
//                     pfnCAN_WriteFDP && pfnCAN_FilterMessagesP &&
//                     pfnCAN_GetValueP && pfnCAN_SetValueP &&
//                     pfnCAN_GetErrorTextP;

   //----------------------------------------------------------------
   // If the API was not loaded (Wrong version), an error message is shown.
   //
   if (!btLibFuncLoadP)
   {
      qCritical() << tr("ERROR: DLL functions could not be loaded!");
      return eERROR_LIBRARY;
   }

   qInfo() << tr("SUCCESS: All DLL functions could be loaded!");
   return eERROR_OK;
}

void QCanIxxatUsb::disconnect(void)
{
}
