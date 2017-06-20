//============================================================================//
// File:          qcan_ixxat_vci.cpp                                          //
// Description:   IXXAT VCI library handling                                  //
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
#include "qcan_ixxat_vci.hpp"


/*----------------------------------------------------------------------------*\
** Function implementation                                                    **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanPcanBasic()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
QCanIxxatVci::QCanIxxatVci()
{
   //-----------------------------------------------------------------------
   // Loads library
   //
   clCanLibP.setFileName(QCAN_IXXATLIB);
   btLibFuncLoadP = false;

   //----------------------------------------------------------------
   // try to load library
   //
   if (!clCanLibP.load()) {
       qCritical() << "QCanIxxatVci::QCanIxxatVci() Failed to load the library";
   }

   //----------------------------------------------------------------
   // Loads API functions
   //
   else
   {

      pfnVciEnumDeviceOpenP = (vciEnumDeviceOpen_tf) clCanLibP.resolve("vciEnumDeviceOpen");
      pfnVciEnumDeviceCloseP = (vciEnumDeviceClose_tf) clCanLibP.resolve("vciEnumDeviceClose");
      pfnVciEnumDeviceNextP = (vciEnumDeviceNext_tf) clCanLibP.resolve("vciEnumDeviceNext");
      pfnVciEnumDeviceResetP = (vciEnumDeviceReset_tf) clCanLibP.resolve("vciEnumDeviceReset");
      pfnVciEnumDeviceWaitEventP = (vciEnumDeviceWaitEvent_tf) clCanLibP.resolve("vciEnumDeviceWaitEvent");
      pfnVciFindDeviceByHwidP = (vciFindDeviceByHwid_tf) clCanLibP.resolve("vciFindDeviceByHwid");
      pfnVciFindDeviceByClassP = (vciFindDeviceByClass_tf) clCanLibP.resolve("vciFindDeviceByClass");
      pfnVciSelectDeviceDlgP = (vciSelectDeviceDlg_tf) clCanLibP.resolve("vciSelectDeviceDlg");
      pfnVciFormatErrorP = (vciFormatError_tf) clCanLibP.resolve("vciFormatError");

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
      btLibFuncLoadP = (pfnVciEnumDeviceOpenP &&
                        pfnVciEnumDeviceCloseP &&
                        pfnVciEnumDeviceNextP &&
                        pfnVciEnumDeviceResetP &&
                        pfnVciEnumDeviceWaitEventP &&
                        pfnVciFindDeviceByHwidP &&
                        pfnVciFindDeviceByClassP &&
                        pfnVciSelectDeviceDlgP &&
                        pfnVciFormatErrorP &&

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
                        pfnCanSchedulerStopMessageP);

      //--------------------------------------------------------
      // If the API was not loaded (wrong version),
      //
      if (!btLibFuncLoadP)
      {
         qCritical() << "ERROR: DLL functions could not be loaded!";
      } else
      {
         qInfo() << "SUCCESS: All DLL functions could be loaded!";
      }
   }
}

//----------------------------------------------------------------------------//
// ~QCanIxxatVci()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
QCanIxxatVci::~QCanIxxatVci()
{
   qDebug() << "QCanIxxatVci::~QCanIxxatVci()";

   btLibFuncLoadP = false;

   //-----------------------------------------------------------------------
   // Unload library and remove all API functions
   //
   if (clCanLibP.isLoaded())
   {
      clCanLibP.unload();
   }

   pfnVciEnumDeviceOpenP = NULL;
   pfnVciEnumDeviceCloseP = NULL;
   pfnVciEnumDeviceNextP = NULL;
   pfnVciEnumDeviceResetP = NULL;
   pfnVciEnumDeviceWaitEventP = NULL;
   pfnVciFindDeviceByHwidP = NULL;
   pfnVciFindDeviceByClassP = NULL;
   pfnVciSelectDeviceDlgP = NULL;

   pfnVciDeviceOpenP = NULL;
   pfnVciDeviceOpenDlgP = NULL;
   pfnVciDeviceCloseP = NULL;
   pfnVciDeviceGetInfoP = NULL;
   pfnVciDeviceGetCapsP = NULL;

   pfnCanControlOpenP = NULL;
   pfnCanControlCloseP = NULL;
   pfnCanControlGetCapsP = NULL;
   pfnCanControlGetStatusP = NULL;
   pfnCanControlDetectBitrateP = NULL;
   pfnCanControlInitializeP = NULL;
   pfnCanControlResetP = NULL;
   pfnCanControlStartP = NULL;
   pfnCanControlSetAccFilterP = NULL;
   pfnCanControlAddFilterIdsP = NULL;
   pfnCanControlRemFilterIdsP = NULL;

   pfnCanChannelOpenP = NULL;
   pfnCanChannelCloseP = NULL;
   pfnCanChannelGetCapsP = NULL;
   pfnCanChannelGetStatusP = NULL;
   pfnCanChannelInitializeP = NULL;
   pfnCanChannelActivateP = NULL;
   pfnCanChannelPeekMessageP = NULL;
   pfnCanChannelPostMessageP = NULL;
   pfnCanChannelWaitRxEventP = NULL;
   pfnCanChannelWaitTxEventP = NULL;
   pfnCanChannelReadMessageP = NULL;
   pfnCanChannelSendMessageP = NULL;
   pfnCanChannelPeekMultipleMessagesP = NULL;
   pfnCanChannelPostMultipleMessagesP = NULL;
   pfnCanChannelReadMultipleMessagesP = NULL;
   pfnCanChannelSendMultipleMessagesP = NULL;

   pfnCanSchedulerOpenP = NULL;
   pfnCanSchedulerCloseP = NULL;
   pfnCanSchedulerGetCapsP = NULL;
   pfnCanSchedulerGetStatusP = NULL;
   pfnCanSchedulerActivateP = NULL;
   pfnCanSchedulerResetP = NULL;
   pfnCanSchedulerAddMessageP = NULL;
   pfnCanSchedulerRemMessageP = NULL;
   pfnCanSchedulerStartMessageP = NULL;
   pfnCanSchedulerStopMessageP = NULL;
}

//----------------------------------------------------------------------------//
// isAvailable()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanIxxatVci::isAvailable()
{
   return btLibFuncLoadP;

}

//----------------------------------------------------------------------------//
// formatedError()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
QString QCanIxxatVci::formatedError(HRESULT hrError)
{
   char aszBufferT[256];
   QString clResultT;

   memset(aszBufferT,'\0',256);

   // Gets the text using the vciFormatError API function
   //

   pfnVciFormatErrorP(hrError,aszBufferT,255);
   clResultT = QLatin1String(aszBufferT);
   if(clResultT.length() == 0)
   {
      clResultT = ("An error ocurred. Error-code's text ("+ QString::number(hrError,16).toUpper() + "h) couldn't be retrieved");
   } else
   {
      clResultT = QString::number(hrError,16).toUpper() + "h : " + QLatin1String(aszBufferT);
   }

   return clResultT;
}
