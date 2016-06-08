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



//! [0]
class QCanPeakUsb : public QObject, QCanInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.microcontrol.Qt.qcan.QCanInterface" FILE "plugin.json")
    Q_INTERFACES(QCanInterface)

private:
    typedef uint32_t (*CAN_Initialize_tv) (uint8_t ubChannelV, uint16_t Btr0Btr1, uint8_t HwType, uint32_t IOPort, uint16_t Interrupt);
    typedef uint32_t (*CAN_Uninitialize_tv) ( uint8_t ubChannelV);
    typedef uint32_t (*CAN_Reset_tv) ( uint8_t ubChannelV);
    typedef uint32_t (*CAN_GetStatus_tv) ( uint8_t ubChannelV);
    typedef uint32_t (*CAN_Read_tv) ( uint8_t ubChannelV, TPCANMsg*, TPCANTimestamp*);

    CAN_Initialize_tv pfnCAN_InitializeP;
    CAN_Uninitialize_tv pfnCAN_UninitializeP;
    CAN_GetStatus_tv pfnCAN_GetStatusP;
    CAN_Read_tv pfnCAN_ReadP;

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
