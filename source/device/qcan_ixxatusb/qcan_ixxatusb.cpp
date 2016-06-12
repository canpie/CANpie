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
   pfnVciEnumDeviceCloseP = (vciEnumDeviceClose_tf) clCanLibP.resolve("vciEnumDeviceClose");
   pfnVciEnumDeviceNextP = (vciEnumDeviceNext_tf) clCanLibP.resolve("vciEnumDeviceNext");
   pfnVciEnumDeviceResetP = (vciEnumDeviceReset_tf) clCanLibP.resolve("vciEnumDeviceReset");
   pfnVciEnumDeviceWaitEventP = (vciEnumDeviceWaitEvent_tf) clCanLibP.resolve("vciEnumDeviceWaitEvent");
   pfnVciFindDeviceByHwidP = (vciFindDeviceByHwid_tf) clCanLibP.resolve("vciFindDeviceByHwid");
   pfnVciFindDeviceByClassP = (vciFindDeviceByClass_tf) clCanLibP.resolve("vciFindDeviceByClass");
   pfnVciSelectDeviceDlgP = (vciSelectDeviceDlg_tf) clCanLibP.resolve("vciSelectDeviceDlg");

   pfnVciDeviceOpenP = (vciDeviceOpen_tf) clCanLibP.resolve("vciDeviceOpen");
   pfnVciDeviceOpenDlgP = (vciDeviceOpenDlg_tf) clCanLibP.resolve("vciDeviceOpenDlg");
   pfnVciDeviceCloseP = (vciDeviceClose_tf) clCanLibP.resolve("vciDeviceClose");
   pfnVciDeviceGetInfoP = (vciDeviceGetInfo_tf) clCanLibP.resolve("vciDeviceGetInfo");
   pfnVciDeviceGetCapsP = (vciDeviceGetCaps_tf) clCanLibP.resolve("vciDeviceGetCaps");

   pfnCanControlOpenP = (canControlOpen_tf) clCanLibP.resolve("canControlOpen");
   pfnCanControlCloseP = (canControlClose_tf) clCanLibP.resolve("canControlClose");
   pfnCanControlGetCapsP = (canControlGetCaps_tf) clCanLibP.resolve("canControlGetCaps");
   pfnCanControlGetStatusP = (canControlGetStatus_tf) clCanLibP.resolve("canControlGetStatus");
   pfnCanControlDetectBitrateP = (canControlDetectBitrate_tf) clCanLibP.resolve("canControlDetectBitrate");
   pfnCanControlInitializeP = (canControlInitialize_tf) clCanLibP.resolve("canControlInitialize");
   pfnCanControlResetP = (canControlReset_tf) clCanLibP.resolve("canControlReset");
   pfnCanControlStartP = (canControlStart_tf) clCanLibP.resolve("canControlStart");
   pfnCanControlSetAccFilterP = (canControlSetAccFilter_tf) clCanLibP.resolve("canControlSetAccFilter");
   pfnCanControlAddFilterIdsP = (canControlAddFilterIds_tf) clCanLibP.resolve("canControlAddFilterIds");
   pfnCanControlRemFilterIdsP = (canControlRemFilterIds_tf) clCanLibP.resolve("canControlRemFilterIds");

   pfnCanChannelOpenP = (canChannelOpen_tf) clCanLibP.resolve("canChannelOpen");
   pfnCanChannelCloseP = (canChannelClose_tf) clCanLibP.resolve("canChannelClose");
   pfnCanChannelGetCapsP = (canChannelGetCaps_tf) clCanLibP.resolve("canChannelGetCaps");
   pfnCanChannelGetStatusP = (canChannelGetStatus_tf) clCanLibP.resolve("canChannelGetStatus");
   pfnCanChannelInitializeP = (canChannelInitialize_tf) clCanLibP.resolve("canChannelInitialize");
   pfnCanChannelActivateP = (canChannelActivate_tf) clCanLibP.resolve("canChannelActivate");
   pfnCanChannelPeekMessageP = (canChannelPeekMessage_tf) clCanLibP.resolve("canChannelPeekMessage");
   pfnCanChannelPostMessageP = (canChannelPostMessage_tf) clCanLibP.resolve("canChannelPostMessage");
   pfnCanChannelWaitRxEventP = (canChannelWaitRxEvent_tf) clCanLibP.resolve("canChannelWaitRxEvent");
   pfnCanChannelWaitTxEventP = (canChannelWaitTxEvent_tf) clCanLibP.resolve("canChannelWaitTxEvent");
   pfnCanChannelReadMessageP = (canChannelReadMessage_tf) clCanLibP.resolve("canChannelReadMessage");
   pfnCanChannelSendMessageP = (canChannelSendMessage_tf) clCanLibP.resolve("canChannelSendMessage");
   pfnCanChannelPeekMultipleMessagesP = (canChannelPeekMultipleMessages_tf) clCanLibP.resolve("canChannelPeekMultipleMessages");
   pfnCanChannelPostMultipleMessagesP = (canChannelPostMultipleMessages_tf) clCanLibP.resolve("canChannelPostMultipleMessages");
   pfnCanChannelReadMultipleMessagesP = (canChannelReadMultipleMessages_tf) clCanLibP.resolve("canChannelReadMultipleMessages");
   pfnCanChannelSendMultipleMessagesP = (canChannelSendMultipleMessages_tf) clCanLibP.resolve("canChannelSendMultipleMessages");

   pfnCanSchedulerOpenP = (canSchedulerOpen_tf) clCanLibP.resolve("canSchedulerOpen");
   pfnCanSchedulerCloseP = (canSchedulerClose_tf) clCanLibP.resolve("canSchedulerClose");
   pfnCanSchedulerGetCapsP = (canSchedulerGetCaps_tf) clCanLibP.resolve("canSchedulerGetCaps");
   pfnCanSchedulerGetStatusP = (canSchedulerGetStatus_tf) clCanLibP.resolve("canSchedulerGetStatus");
   pfnCanSchedulerActivateP = (canSchedulerActivate_tf) clCanLibP.resolve("canSchedulerActivate");
   pfnCanSchedulerResetP = (canSchedulerReset_tf) clCanLibP.resolve("canSchedulerReset");
   pfnCanSchedulerAddMessageP = (canSchedulerAddMessage_tf) clCanLibP.resolve("canSchedulerAddMessage");
   pfnCanSchedulerRemMessageP = (canSchedulerRemMessage_tf) clCanLibP.resolve("canSchedulerRemMessage");
   pfnCanSchedulerStartMessageP = (canSchedulerStartMessage_tf) clCanLibP.resolve("canSchedulerStartMessage");
   pfnCanSchedulerStopMessageP = (canSchedulerStopMessage_tf) clCanLibP.resolve("canSchedulerStopMessage");

   //----------------------------------------------------------------
   // check for success
   //
   btLibFuncLoadP = pfnVciEnumDeviceOpenP &&
         pfnVciEnumDeviceCloseP &&
         pfnVciEnumDeviceNextP &&
         pfnVciEnumDeviceResetP &&
         pfnVciEnumDeviceWaitEventP &&
         pfnVciFindDeviceByHwidP &&
         pfnVciFindDeviceByClassP &&
         pfnVciSelectDeviceDlgP &&

         pfnVciDeviceOpenP &&
         pfnVciDeviceOpenDlgP &&
         pfnVciDeviceCloseP &&
         pfnVciDeviceGetInfoP &&
         pfnVciDeviceGetCapsP &&

         pfnCanControlOpenP &&
         pfnCanControlCloseP &&
         pfnCanControlGetCapsP &&
         pfnCanControlGetStatusP &&
         pfnCanControlDetectBitrateP &&
         pfnCanControlInitializeP &&
         pfnCanControlResetP &&
         pfnCanControlStartP &&
         pfnCanControlSetAccFilterP &&
         pfnCanControlAddFilterIdsP &&
         pfnCanControlRemFilterIdsP &&

         pfnCanChannelOpenP &&
         pfnCanChannelCloseP &&
         pfnCanChannelGetCapsP &&
         pfnCanChannelGetStatusP &&
         pfnCanChannelInitializeP &&
         pfnCanChannelActivateP &&
         pfnCanChannelPeekMessageP &&
         pfnCanChannelPostMessageP &&
         pfnCanChannelWaitRxEventP &&
         pfnCanChannelWaitTxEventP &&
         pfnCanChannelReadMessageP &&
         pfnCanChannelSendMessageP &&
         pfnCanChannelPeekMultipleMessagesP &&
         pfnCanChannelPostMultipleMessagesP &&
         pfnCanChannelReadMultipleMessagesP &&
         pfnCanChannelSendMultipleMessagesP &&

         pfnCanSchedulerOpenP &&
         pfnCanSchedulerCloseP &&
         pfnCanSchedulerGetCapsP &&
         pfnCanSchedulerGetStatusP &&
         pfnCanSchedulerActivateP &&
         pfnCanSchedulerResetP &&
         pfnCanSchedulerAddMessageP &&
         pfnCanSchedulerRemMessageP &&
         pfnCanSchedulerStartMessageP &&
         pfnCanSchedulerStopMessageP;

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
