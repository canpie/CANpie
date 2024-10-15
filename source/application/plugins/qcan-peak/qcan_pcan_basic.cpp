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

#include "qcan_pcan_basic.hpp"

#include <QtCore/QDir>
#include <QtCore/QCoreApplication>


QCanPcanBasic * QCanPcanBasic::pclQCanPcanBasicInstanceP = nullptr;


//--------------------------------------------------------------------------------------------------------------------//
// QCanPcanBasic()                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanPcanBasic::QCanPcanBasic()
{
   #if defined(Q_OS_MAC)
   //---------------------------------------------------------------------------------------------------
   // For MacOS the library is copied inside the following directory:
   // 'CANpieServer.app/Contents/Frameworks/'
   // This allows a simple package generation.
   //
   QDir clLibDirectoryT(QCoreApplication::applicationDirPath());
   clLibDirectoryT.cdUp();
   clLibDirectoryT.cd("Frameworks");
   clCanLibP.setFileName(clLibDirectoryT.absolutePath() + "/" + QCAN_PEAKLIB);
   #else
   clCanLibP.setFileName(QCAN_PEAKLIB);
   #endif

   //---------------------------------------------------------------------------------------------------
   // Create logging file for this plugin 
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanPcanBasic::QCanPcanBasic() - init logging" ;
   #endif

   QFile clLogFileT(QCoreApplication::applicationDirPath() + "/qcan_peak_plugin.log");
   bool  btHasLogFileT = clLogFileT.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);

   if (!clCanLibP.load())
   {
       qCritical() << "QCanPcanBasic(): Failed to load the library:" << qPrintable(clCanLibP.fileName());
      if (btHasLogFileT)
      {
         clLogFileT.write(qPrintable("Failed to load PEAK library: " + clCanLibP.fileName() + "\n"));
         clLogFileT.write(qPrintable(clCanLibP.errorString() + "\n"));
      }
   }
   else
   {
      if (btHasLogFileT)
      {
         clLogFileT.write(qPrintable("Loaded PEAK library: " + clCanLibP.fileName() + "\n" ));
      }

      //-------------------------------------------------------------------------------------------
      // Loads API functions
      //
      pfnCAN_InitializeP = reinterpret_cast<QCanPcanBasic::CAN_Initialize_tf>(clCanLibP.resolve("CAN_Initialize"));
      if (btHasLogFileT)
      {
         if (pfnCAN_InitializeP != nullptr)
         {
            clLogFileT.write(qPrintable("Resolved PEAK library function : CAN_Initialize\n"));
         }
         else
         {
            clLogFileT.write(qPrintable("Failed to resolve PEAK library function : CAN_Initialize\n"));
         }
      }

      #if QCAN_SUPPORT_CAN_FD > 0
      pfnCAN_InitializeFDP = reinterpret_cast<QCanPcanBasic::CAN_InitializeFD_tf>(clCanLibP.resolve("CAN_InitializeFD"));
      if (btHasLogFileT)
      {
         if (pfnCAN_InitializeFDP != nullptr)
         {
            clLogFileT.write(qPrintable("Resolved PEAK library function : CAN_InitializeFD\n"));
         }
         else
         {
            clLogFileT.write(qPrintable("Failed to resolve PEAK library function : CAN_InitializeFD\n"));
         }
      }
      #endif

      pfnCAN_UninitializeP = reinterpret_cast<QCanPcanBasic::CAN_Uninitialize_tf>(clCanLibP.resolve("CAN_Uninitialize"));
      if (btHasLogFileT)
      {
         if (pfnCAN_UninitializeP != nullptr)
         {
            clLogFileT.write(qPrintable("Resolved PEAK library function : CAN_Uninitialize\n"));
         }
         else
         {
            clLogFileT.write(qPrintable("Failed to resolve PEAK library function : CAN_Uninitialize\n"));
         }
      }

      pfnCAN_ResetP = reinterpret_cast<QCanPcanBasic::CAN_Reset_tf>(clCanLibP.resolve("CAN_Reset"));
      if (btHasLogFileT)
      {
         if (pfnCAN_ResetP != nullptr)
         {
            clLogFileT.write(qPrintable("Resolved PEAK library function : CAN_Reset\n"));
         }
         else
         {
            clLogFileT.write(qPrintable("Failed to resolve PEAK library function : CAN_Reset\n"));
         }
      }

      pfnCAN_GetStatusP = reinterpret_cast<QCanPcanBasic::CAN_GetStatus_tf>(clCanLibP.resolve("CAN_GetStatus"));
      if (btHasLogFileT)
      {
         if (pfnCAN_GetStatusP != nullptr)
         {
            clLogFileT.write(qPrintable("Resolved PEAK library function : CAN_GetStatus\n"));
         }
         else
         {
            clLogFileT.write(qPrintable("Failed to resolve PEAK library function : CAN_GetStatus\n"));
         }
      }

      pfnCAN_ReadP = reinterpret_cast<QCanPcanBasic::CAN_Read_tf>(clCanLibP.resolve("CAN_Read"));
      if (btHasLogFileT)
      {
         if (pfnCAN_ReadP != nullptr)
         {
            clLogFileT.write(qPrintable("Resolved PEAK library function : CAN_Read\n"));
         }
         else
         {
            clLogFileT.write(qPrintable("Failed to resolve PEAK library function : CAN_Read\n"));
         }
      }

      #if QCAN_SUPPORT_CAN_FD > 0
      pfnCAN_ReadFDP = reinterpret_cast<QCanPcanBasic::CAN_ReadFD_tf>(clCanLibP.resolve("CAN_ReadFD"));
      if (btHasLogFileT)
      {
         if (pfnCAN_ReadFDP != nullptr)
         {
            clLogFileT.write(qPrintable("Resolved PEAK library function : CAN_ReadFD\n"));
         }
         else
         {
            clLogFileT.write(qPrintable("Failed to resolve PEAK library function : CAN_ReadFD\n"));
         }
      }

      #endif

      pfnCAN_WriteP = reinterpret_cast<QCanPcanBasic::CAN_Write_tf>(clCanLibP.resolve("CAN_Write"));
      if (btHasLogFileT)
      {
         if (pfnCAN_WriteP != nullptr)
         {
            clLogFileT.write(qPrintable("Resolved PEAK library function : CAN_Write\n"));
         }
         else
         {
            clLogFileT.write(qPrintable("Failed to resolve PEAK library function : CAN_Write\n"));
         }
      }

      #if QCAN_SUPPORT_CAN_FD > 0
      pfnCAN_WriteFDP = reinterpret_cast<QCanPcanBasic::CAN_WriteFD_tf>(clCanLibP.resolve("CAN_WriteFD"));
      if (btHasLogFileT)
      {
         if (pfnCAN_WriteFDP != nullptr)
         {
            clLogFileT.write(qPrintable("Resolved PEAK library function : CAN_WriteFD\n"));
         }
         else
         {
            clLogFileT.write(qPrintable("Failed to resolve PEAK library function : CAN_WriteFD\n"));
         }
      }

      #endif

      pfnCAN_FilterMessagesP = reinterpret_cast<QCanPcanBasic::CAN_FilterMessages_tf>(clCanLibP.resolve("CAN_FilterMessages"));
      if (btHasLogFileT)
      {
         if (pfnCAN_FilterMessagesP != nullptr)
         {
            clLogFileT.write(qPrintable("Resolved PEAK library function : CAN_FilterMessages\n"));
         }
         else
         {
            clLogFileT.write(qPrintable("Failed to resolve PEAK library function : CAN_FilterMessages\n"));
         }
      }

      pfnCAN_GetValueP = reinterpret_cast<QCanPcanBasic::CAN_GetValue_tf>(clCanLibP.resolve("CAN_GetValue"));
      if (btHasLogFileT)
      {
         if (pfnCAN_GetValueP != nullptr)
         {
            clLogFileT.write(qPrintable("Resolved PEAK library function : CAN_GetValue\n"));
         }
         else
         {
            clLogFileT.write(qPrintable("Failed to resolve PEAK library function : CAN_GetValue\n"));
         }
      }

      pfnCAN_SetValueP = reinterpret_cast<QCanPcanBasic::CAN_SetValue_tf>(clCanLibP.resolve("CAN_SetValue"));
      if (btHasLogFileT)
      {
         if (pfnCAN_SetValueP != nullptr)
         {
            clLogFileT.write(qPrintable("Resolved PEAK library function : CAN_SetValue\n"));
         }
         else
         {
            clLogFileT.write(qPrintable("Failed to resolve PEAK library function : CAN_SetValue\n"));
         }
      }

      pfnCAN_GetErrorTextP = reinterpret_cast<QCanPcanBasic::CAN_GetErrorText_tf>(clCanLibP.resolve("CAN_GetErrorText"));
      if (btHasLogFileT)
      {
         if (pfnCAN_GetErrorTextP != nullptr)
         {
            clLogFileT.write(qPrintable("Resolved PEAK library function : CAN_GetErrorText\n"));
         }
         else
         {
            clLogFileT.write(qPrintable("Failed to resolve PEAK library function : CAN_GetErrorText\n"));
         }
      }


      //----------------------------------------------------------------
      // check for success
      //
      btLibFuncLoadP =  pfnCAN_InitializeP &&
                        #if QCAN_SUPPORT_CAN_FD > 0
                        pfnCAN_InitializeFDP &&
                        #endif
                        pfnCAN_UninitializeP &&
                        pfnCAN_ResetP &&
                        pfnCAN_GetStatusP &&
                        pfnCAN_ReadP  &&
                        pfnCAN_WriteP &&
                        #if QCAN_SUPPORT_CAN_FD > 0
                        pfnCAN_FilterMessagesP &&
                        pfnCAN_ReadFDP &&
                        pfnCAN_WriteFDP &&
                        #endif
                        pfnCAN_GetValueP &&
                        pfnCAN_SetValueP &&
                        pfnCAN_GetErrorTextP;

      //----------------------------------------------------------------
      // If the API was not loaded (Wrong version), an error message is shown.
      //
      if (!btLibFuncLoadP)
      {
         qCritical() << "QCanPcanBasic::QCanPcanBasic() CRITICAL: Failed to load some library functions!";
         if (btHasLogFileT)
         {
            clLogFileT.write(qPrintable("Failed to load some library functions\n"));
            clLogFileT.write(qPrintable(clCanLibP.errorString() + "\n"));
         }
      } 
      else
      {
         qInfo() << "QCanPcanBasic::QCanPcanBasic() INFO: All library functions successfully loaded!";
         if (btHasLogFileT)
         {
            clLogFileT.write(qPrintable("All library functions successfully loaded\n"));
         }
      }
   }

   if (btHasLogFileT)
   {
      clLogFileT.close();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// ~QCanPcanBasic()                                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanPcanBasic::~QCanPcanBasic()
{
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanPcanBasic::~QCanPcanBasic()";
   #endif
   
   //----------------------------------------------------------------
   // Unload library
   //
   if (clCanLibP.isLoaded())
   {
      //--------------------------------------------------------
      // release all devices
      //
      if (isAvailable())
      {
         pfnCAN_UninitializeP(PCAN_NONEBUS);
         btLibFuncLoadP = false;
      }

      //--------------------------------------------------------
      // unload library
      //
      clCanLibP.unload();
   }

   //----------------------------------------------------------------
   // remove all API functions
   //
   pfnCAN_InitializeP   = nullptr;
   pfnCAN_UninitializeP = nullptr;
   pfnCAN_ResetP        = nullptr;
   pfnCAN_GetStatusP    = nullptr;
   pfnCAN_ReadP         = nullptr;
   pfnCAN_WriteP        = nullptr;
   pfnCAN_FilterMessagesP = nullptr;
   pfnCAN_GetValueP     = nullptr;
   pfnCAN_SetValueP     = nullptr;
   pfnCAN_GetErrorTextP = nullptr;

   #if QCAN_SUPPORT_CAN_FD > 0
   pfnCAN_InitializeFDP = nullptr;
   pfnCAN_ReadFDP       = nullptr;
   pfnCAN_WriteFDP      = nullptr;
   #endif
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanPcanBasic::errorString()                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanPcanBasic::errorString(TPCANStatus tvErrorV)
{
   TPCANStatus tvStatusT;
   char aszBufferT[256];
   QString clResultT;

   memset(aszBufferT,'\0',256);

   // Gets the text using the GetErrorText API function
   // If the function success, the translated error is returned. If it fails,
   // a text describing the current error is returned.
   //

   tvStatusT = pfnCAN_GetErrorTextP(tvErrorV,0x00,aszBufferT);
   if(tvStatusT != PCAN_ERROR_OK)
   {
      clResultT = ("An error ocurred. Error-code's text ("+ QString::number(tvErrorV,16).toUpper() + "h) couldn't be retrieved");
   } 
   else
   {
      clResultT = QString::number(tvErrorV,16).toUpper() + "h : " + QLatin1String(aszBufferT);
   }

   return clResultT;
}



//--------------------------------------------------------------------------------------------------------------------//
// QCanPcanBasic::instance()                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanPcanBasic & QCanPcanBasic::instance() 
{
   if (pclQCanPcanBasicInstanceP == nullptr) 
   {
      pclQCanPcanBasicInstanceP = new QCanPcanBasic();
   }
   
   return(*pclQCanPcanBasicInstanceP);
}

//--------------------------------------------------------------------------------------------------------------------//
// QCanPcanBasic::isAvailable()                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanPcanBasic::isAvailable()
{
   return btLibFuncLoadP;

}

