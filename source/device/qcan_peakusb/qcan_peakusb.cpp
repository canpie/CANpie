//============================================================================//
// File:          qcan_peakusb.cpp                                            //
// Description:   CAN plugin for PEAK USB device                              //
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
#include <qcan_peakusb.hpp>


//----------------------------------------------------------------------------//
// echo()                                                                     //
// dummy implementation of a function to evaluate string                      //
//----------------------------------------------------------------------------//
QString QCanPeakUsb::echo(const QString &message)
{
   uint32_t ulStatusT;
   QString clRetStringT;
   CHAR data[64];

   emit errorOccurred(17);
   if (message == "load")
   {
      connect();
      clRetStringT = ";)";
   }

   if (message == "init")
   {
      ulStatusT = pfnCAN_InitializeP(PCAN_USBBUS1, PCAN_USBBUS1, PCAN_USBBUS1, 0, 0);
      clRetStringT = (" " + QString::number(ulStatusT,16));
      if (ulStatusT != PCAN_ERROR_OK)
      {
         pfnCAN_GetErrorTextP(ulStatusT,0x07,(char*)&data[0]);
      }
   }

   QString clRetString2T(data);
   if (message == "status")
   {
      ulStatusT = pfnCAN_GetStatusP(0);
      clRetStringT = (" " + QString::number(ulStatusT,16) + " ("+ clRetString2T + ")");
   }


//   clRetStringT.prepend(message + " ");
   return ("peak: " + message + clRetStringT +  " ("+ clRetString2T + ")");
}
//! [0]

//! [1]
int32_t QCanPeakUsb::setBitrate(uint32_t ulBitrateV, uint32_t ulBrsClockV)
{
   return 0;
}
//! [1]

//! [2]
int32_t	QCanPeakUsb::setMode(const uint32_t ulModeV)
{
   return 0;
}
//! [2]

int32_t	QCanPeakUsb::state(void)
{
   return 0;
}

int32_t	QCanPeakUsb::statistic(QCanStatistic_ts &clStatisticR)
{
   return 0;
}

int32_t	QCanPeakUsb::read(QCanFrame &clFrameR)
{
   return 0;

}

int32_t	QCanPeakUsb::write(const QCanFrame &clFrameR)
{
   return 0;
}
//----------------------------------------------------------------------------//
// connect()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t QCanPeakUsb::connect(void)
{
   QLibrary pcanLibrary("PCANBasic.dll");

   //-----------------------------------------------------------------------
   // Loads library functions
   //
   if (!pcanLibrary.load()) {
       qCritical() << tr("Failed to load the library: ") + qPrintable(pcanLibrary.fileName());
       return -1;
   }

   //----------------------------------------------------------------
   // Loads API functions
   //
   pfnCAN_InitializeP = (CAN_Initialize_tf) pcanLibrary.resolve("CAN_Initialize");
   pfnCAN_InitializeFDP = (CAN_InitializeFD_tf) pcanLibrary.resolve("CAN_InitializeFD");
   pfnCAN_UninitializeP = (CAN_Uninitialize_tf)pcanLibrary.resolve("CAN_Uninitialize");
   pfnCAN_ResetP = (CAN_Reset_tf)pcanLibrary.resolve("CAN_Reset");
   pfnCAN_GetStatusP = (CAN_GetStatus_tf)pcanLibrary.resolve("CAN_GetStatus");
   pfnCAN_ReadP = (CAN_Read_tf)pcanLibrary.resolve("CAN_Read");
   pfnCAN_ReadFDP = (CAN_ReadFD_tf)pcanLibrary.resolve("CAN_ReadFD");
   pfnCAN_WriteP = (CAN_Write_tf)pcanLibrary.resolve("CAN_Write");
   pfnCAN_WriteFDP = (CAN_WriteFD_tf)pcanLibrary.resolve("CAN_WriteFD");
   pfnCAN_FilterMessagesP = (CAN_FilterMessages_tf)pcanLibrary.resolve("CAN_FilterMessages");
   pfnCAN_GetValueP = (CAN_GetValue_tf)pcanLibrary.resolve("CAN_GetValue");
   pfnCAN_SetValueP = (CAN_SetValue_tf)pcanLibrary.resolve("CAN_SetValue");
   pfnCAN_GetErrorTextP = (CAN_GetErrorText_tf)pcanLibrary.resolve("CAN_GetErrorText");


   //----------------------------------------------------------------
   // check for success
   //
   btLibFuncLoadP =  pfnCAN_InitializeP && pfnCAN_InitializeFDP &&
                     pfnCAN_UninitializeP && pfnCAN_ResetP &&
                     pfnCAN_GetStatusP && pfnCAN_ReadP  &&
                     pfnCAN_ReadFDP && pfnCAN_WriteP &&
                     pfnCAN_WriteFDP && pfnCAN_FilterMessagesP &&
                     pfnCAN_GetValueP && pfnCAN_SetValueP &&
                     pfnCAN_GetErrorTextP;

   //----------------------------------------------------------------
   // If the API was not loaded (Wrong version), an error message is shown.
   //
   if (!btLibFuncLoadP)
   {
      qCritical() << tr("ERROR: DLL functions could not be loaded!");
      return -2;
   }

   qInfo() << tr("SUCCESS: All DLL functions could be loaded!");
   return 0;
}

int32_t QCanPeakUsb::disconnect(void)
{
   return 0;
}
