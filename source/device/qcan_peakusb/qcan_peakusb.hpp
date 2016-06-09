//============================================================================//
// File:          qcan_peakusb.hpp                                            //
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

#ifndef QCAN_PEAK_USB_H_
#define QCAN_PEAK_USB_H_

#include <QObject>
#include <QtPlugin>
#include <QLibrary>
#include <..\..\qcan\qcan_interface.hpp>

#ifdef Q_OS_WIN32
#include <windows.h>
#ifdef DRV_CALLBACK_TYPE
#define DRV_CALLBACK_TYPE WINAPI
#endif
#else
#define DRV_CALLBACK_TYPE
#endif


#include "PCANBasic.h"

//static fpCAN_Initialize CAN_Initialize;

//GENERATE_SYMBOL_VARIABLE(TPCANStatus, CAN_Initialize, TPCANHandle, TPCANBaudrate, TPCANType, quint32, quint16)

//GENERATE_SYMBOL_VARIABLE(TPCANStatus, CAN_Initialize, TPCANHandle, TPCANBaudrate, TPCANType, quint32, quint16)
//GENERATE_SYMBOL_VARIABLE(TPCANStatus, CAN_Uninitialize, TPCANHandle)
//GENERATE_SYMBOL_VARIABLE(TPCANStatus, CAN_Reset, TPCANHandle)
//GENERATE_SYMBOL_VARIABLE(TPCANStatus, CAN_GetStatus, TPCANHandle)
//GENERATE_SYMBOL_VARIABLE(TPCANStatus, CAN_Read, TPCANHandle, TPCANMsg*, TPCANTimestamp*)
//GENERATE_SYMBOL_VARIABLE(TPCANStatus, CAN_Write, TPCANHandle, TPCANMsg*)
//GENERATE_SYMBOL_VARIABLE(TPCANStatus, CAN_FilterMessages, TPCANHandle, quint32, quint32, TPCANMode)
//GENERATE_SYMBOL_VARIABLE(TPCANStatus, CAN_GetValue, TPCANHandle, TPCANParameter, void *, quint32)
//GENERATE_SYMBOL_VARIABLE(TPCANStatus, CAN_SetValue, TPCANHandle, TPCANParameter, void *, quint32)
//GENERATE_SYMBOL_VARIABLE(TPCANStatus, CAN_GetErrorText, TPCANStatus, quint16, char *)

//TPCANStatus __stdcall CAN_Initialize(TPCANHandle Channel, TPCANBaudrate Btr0Btr1, TPCANType HwType _DEF_ARG,DWORD IOPort _DEF_ARG, WORD Interrupt _DEF_ARG);
//TPCANStatus __stdcall CAN_InitializeFD(TPCANHandle Channel,	TPCANBitrateFD BitrateFD);
//TPCANStatus __stdcall CAN_Uninitialize(TPCANHandle Channel);

//TPCANStatus __stdcall CAN_Reset(TPCANHandle Channel);
//TPCANStatus __stdcall CAN_GetStatus(TPCANHandle Channel);

//TPCANStatus __stdcall CAN_Read(TPCANHandle Channel, TPCANMsg* MessageBuffer, TPCANTimestamp* TimestampBuffer);
//TPCANStatus __stdcall CAN_ReadFD(TPCANHandle Channel,	TPCANMsgFD* MessageBuffer,	TPCANTimestampFD *TimestampBuffer);

//TPCANStatus __stdcall CAN_Write(TPCANHandle Channel,  TPCANMsg* MessageBuffer);
//TPCANStatus __stdcall CAN_WriteFD(TPCANHandle Channel,	TPCANMsgFD* MessageBuffer);

//TPCANStatus __stdcall CAN_FilterMessages(TPCANHandle Channel, DWORD FromID, DWORD ToID, TPCANMode Mode);
//TPCANStatus __stdcall CAN_GetValue(TPCANHandle Channel, TPCANParameter Parameter,  void* Buffer, DWORD BufferLength);
//TPCANStatus __stdcall CAN_SetValue(TPCANHandle Channel,        TPCANParameter Parameter, void* Buffer,		DWORD BufferLength);
//TPCANStatus __stdcall CAN_GetErrorText(TPCANStatus Error, WORD Language, LPSTR Buffer);


//! [0]
class QCanPeakUsb : public QObject, QCanInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.microcontrol.Qt.qcan.QCanInterface" FILE "plugin.json")
    Q_INTERFACES(QCanInterface)

private:

   //----------------------------------------------------------------
   // Type definitions of PCANBasic.dll taken from PCANBasic.h
   // (Last Change 24.04.2015)
   //
   typedef TPCANStatus (*CAN_Initialize_tf)     (TPCANHandle uwChannelV, TPCANBaudrate uwBtr0Btr1V, TPCANType ubHwTypeV, DWORD ulIOPortV, WORD uwInterruptV);
   typedef TPCANStatus (*CAN_InitializeFD_tf)   (TPCANHandle uwChannelV, TPCANBitrateFD pszBitrateFDV);
   typedef TPCANStatus (*CAN_Uninitialize_tf)   (TPCANHandle uwChannelV);
   typedef TPCANStatus (*CAN_Reset_tf)          (TPCANHandle uwChannelV);
   typedef TPCANStatus (*CAN_GetStatus_tf)      (TPCANHandle uwChannelV);
   typedef TPCANStatus (*CAN_Read_tf)           (TPCANHandle uwChannelV, TPCANMsg *ptsMessageBufferV, TPCANTimestamp *tsTimestampBufferV);
   typedef TPCANStatus (*CAN_ReadFD_tf)         (TPCANHandle uwChannelV, TPCANMsgFD *ptsMessageBufferV, TPCANTimestampFD *puqTimestampBufferV);
   typedef TPCANStatus (*CAN_Write_tf)          (TPCANHandle uwChannelV, TPCANMsg *ptsMessageBufferV);
   typedef TPCANStatus (*CAN_WriteFD_tf)        (TPCANHandle uwChannelV, TPCANMsgFD *ptsMessageBufferV);
   typedef TPCANStatus (*CAN_FilterMessages_tf) (TPCANHandle uwChannelV, DWORD ulFromIDV, DWORD ulToIDV, TPCANMode ubModeV);
   typedef TPCANStatus (*CAN_GetValue_tf)       (TPCANHandle uwChannelV, TPCANParameter ubParameterV, void *pvdBufferV, DWORD ulBufferLengthV);
   typedef TPCANStatus (*CAN_SetValue_tf)       (TPCANHandle uwChannelV, TPCANParameter ubParameterV, void *pvdBufferV, DWORD ulBufferLengthV);
   typedef TPCANStatus (*CAN_GetErrorText_tf)   (TPCANStatus ulErrorV, WORD uwLanguageV, LPSTR pszBufferV);


   CAN_Initialize_tf      pfnCAN_InitializeP;
   CAN_InitializeFD_tf    pfnCAN_InitializeFDP;
   CAN_Uninitialize_tf    pfnCAN_UninitializeP;
   CAN_Reset_tf           pfnCAN_ResetP;
   CAN_GetStatus_tf       pfnCAN_GetStatusP;
   CAN_Read_tf            pfnCAN_ReadP;
   CAN_ReadFD_tf          pfnCAN_ReadFDP;
   CAN_Write_tf           pfnCAN_WriteP;
   CAN_WriteFD_tf         pfnCAN_WriteFDP;
   CAN_FilterMessages_tf  pfnCAN_FilterMessagesP;
   CAN_GetValue_tf        pfnCAN_GetValueP;
   CAN_SetValue_tf        pfnCAN_SetValueP;
   CAN_GetErrorText_tf    pfnCAN_GetErrorTextP;


   bool btLibFuncLoadP = false;

public:
    QString echo(const QString &message) Q_DECL_OVERRIDE;
    int32_t setBitrate(uint32_t ulBitrateV, uint32_t ulBrsClockV) Q_DECL_OVERRIDE;
    int32_t	setMode(const uint32_t ulModeV) Q_DECL_OVERRIDE;
    int32_t	state(void) Q_DECL_OVERRIDE;
    int32_t	statistic(QCanStatistic_ts &clStatisticR) Q_DECL_OVERRIDE;
    int32_t	read(QCanFrame &clFrameR) Q_DECL_OVERRIDE;
    int32_t	write(const QCanFrame &clFrameR) Q_DECL_OVERRIDE;
    int32_t connect(void) Q_DECL_OVERRIDE;
    int32_t disconnect(void) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void errorOccurred(int32_t slCanBusErrorV);
};
//! [0]




#endif
