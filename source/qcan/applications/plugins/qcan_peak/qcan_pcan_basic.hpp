//============================================================================//
// File:          qcan_pcan_basic.hpp                                         //
// Description:   PCAN Basic library API                                      //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53844 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions         //
// are met:                                                                   //
// 1. Redistributions of source code must retain the above copyright          //
//    notice, this list of conditions, the following disclaimer and           //
//    the referenced file 'LICENSE'.                                          //
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

#ifndef QCAN_PCAN_BASIC_H_
#define QCAN_PCAN_BASIC_H_


#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtCore/QLibrary>

#ifdef   Q_OS_WIN32
#include <windows.h>
#include "PCANBasic.h"
#define  DRV_CALLBACK_TYPE       WINAPI
#define  QCAN_SUPPORT_CAN_FD     1
#define  QCAN_PEAKLIB            "PCANBasic.dll"
#endif

#ifdef   Q_OS_OSX
#include "PCBUSB.h"
#define  DRV_CALLBACK_TYPE
#define  QCAN_SUPPORT_CAN_FD     1
#define  QCAN_PEAKLIB            "libPCBUSB.dylib"
#endif

#ifndef  QCAN_PEAKLIB
#error   Operating system not supported
#endif


//----------------------------------------------------------------------------//
// QCanPeakUsb                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
class QCanPcanBasic
{

private:

   QLibrary clCanLibP;
   bool btLibFuncLoadP;

   QCanPcanBasic();
   QCanPcanBasic(const QCanPcanBasic &);


   //----------------------------------------------------------------
   // Type definitions of PCANBasic.dll taken from PCANBasic.h
   // (Last Change 24.04.2015)
   //
   typedef TPCANStatus (DRV_CALLBACK_TYPE *CAN_Initialize_tf)     (TPCANHandle uwChannelV, TPCANBaudrate uwBtr0Btr1V, TPCANType ubHwTypeV, DWORD ulIOPortV, WORD uwInterruptV);
   #if QCAN_SUPPORT_CAN_FD > 0
   typedef TPCANStatus (DRV_CALLBACK_TYPE *CAN_InitializeFD_tf)   (TPCANHandle uwChannelV, TPCANBitrateFD pszBitrateFDV);
   #endif
   typedef TPCANStatus (DRV_CALLBACK_TYPE *CAN_Uninitialize_tf)   (TPCANHandle uwChannelV);
   typedef TPCANStatus (DRV_CALLBACK_TYPE *CAN_Reset_tf)          (TPCANHandle uwChannelV);
   typedef TPCANStatus (DRV_CALLBACK_TYPE *CAN_GetStatus_tf)      (TPCANHandle uwChannelV);
   typedef TPCANStatus (DRV_CALLBACK_TYPE *CAN_Read_tf)           (TPCANHandle uwChannelV, TPCANMsg *ptsMessageBufferV, TPCANTimestamp *tsTimestampBufferV);
   #if QCAN_SUPPORT_CAN_FD > 0
   typedef TPCANStatus (DRV_CALLBACK_TYPE *CAN_ReadFD_tf)         (TPCANHandle uwChannelV, TPCANMsgFD *ptsMessageBufferV, TPCANTimestampFD *puqTimestampBufferV);
   #endif
   typedef TPCANStatus (DRV_CALLBACK_TYPE *CAN_Write_tf)          (TPCANHandle uwChannelV, TPCANMsg *ptsMessageBufferV);
   #if QCAN_SUPPORT_CAN_FD > 0
   typedef TPCANStatus (DRV_CALLBACK_TYPE *CAN_WriteFD_tf)        (TPCANHandle uwChannelV, TPCANMsgFD *ptsMessageBufferV);
   #endif
   typedef TPCANStatus (DRV_CALLBACK_TYPE *CAN_FilterMessages_tf) (TPCANHandle uwChannelV, DWORD ulFromIDV, DWORD ulToIDV, TPCANMode ubModeV);
   typedef TPCANStatus (DRV_CALLBACK_TYPE *CAN_GetValue_tf)       (TPCANHandle uwChannelV, TPCANParameter ubParameterV, void *pvdBufferV, DWORD ulBufferLengthV);
   typedef TPCANStatus (DRV_CALLBACK_TYPE *CAN_SetValue_tf)       (TPCANHandle uwChannelV, TPCANParameter ubParameterV, void *pvdBufferV, DWORD ulBufferLengthV);
   typedef TPCANStatus (DRV_CALLBACK_TYPE *CAN_GetErrorText_tf)   (TPCANStatus ulErrorV, WORD uwLanguageV, LPSTR pszBufferV);


public:
   ~QCanPcanBasic();

   CAN_Initialize_tf      pfnCAN_InitializeP;
   #if QCAN_SUPPORT_CAN_FD > 0
   CAN_InitializeFD_tf    pfnCAN_InitializeFDP;
   #endif
   CAN_Uninitialize_tf    pfnCAN_UninitializeP;
   CAN_Reset_tf           pfnCAN_ResetP;
   CAN_GetStatus_tf       pfnCAN_GetStatusP;
   CAN_Read_tf            pfnCAN_ReadP;
   #if QCAN_SUPPORT_CAN_FD > 0
   CAN_ReadFD_tf          pfnCAN_ReadFDP;
   #endif
   CAN_Write_tf           pfnCAN_WriteP;
   #if QCAN_SUPPORT_CAN_FD > 0
   CAN_WriteFD_tf         pfnCAN_WriteFDP;
   #endif
   CAN_FilterMessages_tf  pfnCAN_FilterMessagesP;
   CAN_GetValue_tf        pfnCAN_GetValueP;
   CAN_SetValue_tf        pfnCAN_SetValueP;
   CAN_GetErrorText_tf    pfnCAN_GetErrorTextP;

   // allow only one instance of PCAN Basic
   static QCanPcanBasic & getInstance()
   {
      static QCanPcanBasic clInstanceS;
      return clInstanceS;
   }

   // helper functions
   bool isAvailable (void);
   QString formatedError(TPCANStatus tvErrorV);
};

#endif /*QCAN_PCAN_BASIC_H_*/
