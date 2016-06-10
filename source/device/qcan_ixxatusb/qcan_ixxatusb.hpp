//============================================================================//
// File:          qcan_ixxatusb.hpp                                           //
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

#ifndef QCAN_IXXAT_USB_H_
#define QCAN_IXXAT_USB_H_

#include <QObject>
#include <QtPlugin>
#include <QLibrary>
#include <qcan_interface.hpp>


#ifdef Q_OS_WIN32
#include <windows.h>
#include "vcinpl.h"
#define QCAN_IXXATLIB "vcinpl.dll"
#ifdef DRV_CALLBACK_TYPE
#define DRV_CALLBACK_TYPE WINAPI
#endif
#else
#define DRV_CALLBACK_TYPE
#endif



class QCanIxxatUsb : public QObject, QCanInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.microcontrol.Qt.qcan.QCanInterface" FILE "plugin.json")
    Q_INTERFACES(QCanInterface)
private:
   //----------------------------------------------------------------
   // Type definitions of PCANBasic.dll taken from vcinpl.h
   // API documentation can be found in "VCI V3 C-API Manual 1.5" here:
   // http://www.ixxat.com/de/support/downloads-dateien-dokumente/treiber
   // http://www.ixxat.com/support/file-and-documents-download/drivers/vci-v3-driver-download
   //
   //////////////////////////////////////////////////////////////////////////
   // device manager specific functions
   //////////////////////////////////////////////////////////////////////////

   typedef HRESULT VCIAPI (*vciEnumDeviceOpen_tf)     ( OUT PHANDLE hEnum );
   typedef HRESULT VCIAPI (*vciEnumDeviceClose_tf)    ( IN HANDLE hEnum );
   typedef HRESULT VCIAPI (*vciEnumDeviceNext_tf)     ( IN  HANDLE hEnum, OUT PVCIDEVICEINFO pInfo );
   typedef HRESULT VCIAPI (*vciEnumDeviceReset_tf)    ( IN HANDLE hEnum );
   typedef HRESULT VCIAPI (*vciEnumDeviceWaitEvent_tf)( IN HANDLE hEnum, IN UINT32 dwMsTimeout );
   typedef HRESULT VCIAPI (*vciFindDeviceByHwid_tf)   ( IN REFGUID rHwid, OUT PVCIID pVciid );
   typedef HRESULT VCIAPI (*vciFindDeviceByClass_tf)  ( IN REFGUID rClass, IN  UINT32 dwInst, OUT PVCIID  pVciid );
   typedef HRESULT VCIAPI (*vciSelectDeviceDlg_tf)    ( IN HWND hwndParent, OUT PVCIID pVciid );


   //////////////////////////////////////////////////////////////////////////
   // device specific functions
   //////////////////////////////////////////////////////////////////////////

   typedef HRESULT VCIAPI (*vciDeviceOpen_tf) ( IN  REFVCIID rVciid,
                                          OUT PHANDLE  phDevice );
   typedef HRESULT VCIAPI (*vciDeviceOpenDlg_tf) ( IN  HWND    hwndParent,
                                             OUT PHANDLE phDevice );
   typedef HRESULT VCIAPI (*vciDeviceClose_tf) ( IN HANDLE hDevice );
   typedef HRESULT VCIAPI (*vciDeviceGetInfo_tf) ( IN  HANDLE         hDevice,
                                             OUT PVCIDEVICEINFO pInfo );
   typedef HRESULT VCIAPI (*vciDeviceGetCaps_tf) ( IN  HANDLE         hDevice,
                                             OUT PVCIDEVICECAPS pCaps );


   //////////////////////////////////////////////////////////////////////////
   // CAN controller specific functions
   //////////////////////////////////////////////////////////////////////////

   typedef HRESULT VCIAPI (*canControlOpen_tf) ( IN  HANDLE  hDevice,
                                           IN  UINT32  dwCanNo,
                                           OUT PHANDLE phCanCtl );
   typedef HRESULT VCIAPI (*canControlClose_tf) ( IN HANDLE hCanCtl );
   typedef HRESULT VCIAPI (*canControlGetCaps_tf) ( IN  HANDLE           hCanCtl,
                                              OUT PCANCAPABILITIES pCanCaps );
   typedef HRESULT VCIAPI (*canControlGetStatus_tf) ( IN  HANDLE         hCanCtl,
                                                OUT PCANLINESTATUS pStatus );
   typedef HRESULT VCIAPI (*canControlDetectBitrate_tf) ( IN  HANDLE  hCanCtl,
                                                    IN  UINT16  wMsTimeout,
                                                    IN  UINT32  dwCount,
                                                    IN  PUINT8  pabBtr0,
                                                    IN  PUINT8  pabBtr1,
                                                    OUT PINT32  plIndex );
   typedef HRESULT VCIAPI (*canControlInitialize_tf) ( IN HANDLE hCanCtl,
                                                 IN UINT8  bMode,
                                                 IN UINT8  bBtr0,
                                                 IN UINT8  bBtr1 );
   typedef HRESULT VCIAPI (*canControlReset_tf) ( IN HANDLE hCanCtl );
   typedef HRESULT VCIAPI (*canControlStart_tf) ( IN HANDLE hCanCtl,
                                             IN BOOL   fStart );

   typedef HRESULT VCIAPI (*canControlSetAccFilter_tf) ( IN HANDLE hCanCtl,
                                                   IN BOOL   fExtend,
                                                   IN UINT32 dwCode,
                                                   IN UINT32 dwMask );
   typedef HRESULT VCIAPI (*canControlAddFilterIds_tf) ( IN HANDLE hCanCtl,
                                                   IN BOOL   fExtend,
                                                   IN UINT32 dwCode,
                                                   IN UINT32 dwMask );
   typedef HRESULT VCIAPI (*canControlRemFilterIds_tf) ( IN HANDLE hCanCtl,
                                                   IN BOOL   fExtend,
                                                   IN UINT32 dwCode,
                                                   IN UINT32 dwMask );


   //////////////////////////////////////////////////////////////////////////
   // CAN message channel specific functions
   //////////////////////////////////////////////////////////////////////////
   typedef HRESULT VCIAPI (*canChannelOpen_tf) ( IN  HANDLE  hDevice,
                                           IN  UINT32  dwCanNo,
                                           IN  BOOL    fExclusive,
                                           OUT PHANDLE phCanChn );
   typedef HRESULT VCIAPI (*canChannelClose_tf) ( IN HANDLE hCanChn );

   typedef HRESULT VCIAPI (*canChannelGetCaps_tf) ( IN  HANDLE           hCanChn,
                                              OUT PCANCAPABILITIES pCanCaps );

   typedef HRESULT VCIAPI (*canChannelGetStatus_tf) ( IN  HANDLE         hCanChn,
                                                OUT PCANCHANSTATUS pStatus );

   typedef HRESULT VCIAPI (*canChannelInitialize_tf) ( IN HANDLE hCanChn,
                                                 IN UINT16 wRxFifoSize,
                                                 IN UINT16 wRxThreshold,
                                                 IN UINT16 wTxFifoSize,
                                                 IN UINT16 wTxThreshold );

   typedef HRESULT VCIAPI (*canChannelActivate_tf) ( IN HANDLE hCanChn,
                                               IN BOOL   fEnable );

   typedef HRESULT VCIAPI (*canChannelPeekMessage_tf) ( IN  HANDLE  hCanChn,
                                                  OUT PCANMSG pCanMsg );

   typedef HRESULT VCIAPI (*canChannelPostMessage_tf) ( IN HANDLE  hCanChn,
                                                  IN PCANMSG pCanMsg );

   typedef HRESULT VCIAPI (*canChannelWaitRxEvent_tf) ( IN HANDLE hCanChn,
                                                  IN UINT32 dwMsTimeout );

   typedef HRESULT VCIAPI (*canChannelWaitTxEvent_tf) ( IN HANDLE hCanChn,
                                                  IN UINT32 dwMsTimeout );

   typedef HRESULT VCIAPI (*canChannelReadMessage_tf) ( IN  HANDLE  hCanChn,
                                                  IN  UINT32  dwMsTimeout,
                                                  OUT PCANMSG pCanMsg );

   typedef HRESULT VCIAPI (*canChannelSendMessage_tf) ( IN HANDLE  hCanChn,
                                                  IN UINT32  dwMsTimeout,
                                                  IN PCANMSG pCanMsg );

   typedef HRESULT VCIAPI (*
     canChannelPeekMultipleMessages_tf) ( IN     HANDLE  hCanChn,
                                      IN OUT UINT32 * pdwNum,
                                      OUT    CANMSG aCanMsg[] );

   typedef HRESULT VCIAPI (*
     canChannelPostMultipleMessages_tf) ( IN     HANDLE  hCanChn,
                                      IN OUT UINT32 * pdwNum,
                                      IN     CANMSG aCanMsg[] );

   typedef HRESULT VCIAPI (*
     canChannelReadMultipleMessages_tf) ( IN     HANDLE  hCanChn,
                                      IN     UINT32  dwMsTimeout,
                                      IN OUT UINT32 * pdwNum,
                                      OUT    CANMSG aCanMsg[] );

   typedef HRESULT VCIAPI (*     canChannelSendMultipleMessages_tf) ( IN     HANDLE  hCanChn,
                                      IN     UINT32  dwMsTimeout,
                                      IN OUT UINT32 * pdwNum,
                                      IN     CANMSG aCanMsg[] );

   //////////////////////////////////////////////////////////////////////////
   // cyclic CAN message scheduler specific functions
   //////////////////////////////////////////////////////////////////////////


   typedef HRESULT VCIAPI (*canSchedulerOpen_tf) ( IN  HANDLE  hDevice,
                                             IN  UINT32  dwCanNo,
                                             OUT PHANDLE phCanShd );


   typedef HRESULT VCIAPI (*canSchedulerClose_tf) ( IN HANDLE hCanShd );


   typedef HRESULT VCIAPI (*canSchedulerGetCaps_tf) ( IN  HANDLE           hCanShd,
                                                OUT PCANCAPABILITIES pCanCaps );


   typedef HRESULT VCIAPI (*canSchedulerGetStatus_tf) ( IN  HANDLE              hCanShd,
                                                  OUT PCANSCHEDULERSTATUS pStatus );


   typedef HRESULT VCIAPI (*canSchedulerActivate_tf) ( IN HANDLE hCanShd,
                                                 IN BOOL   fEnable );


   typedef HRESULT VCIAPI (*canSchedulerReset_tf) ( IN HANDLE hCanShd );


   typedef HRESULT VCIAPI (*canSchedulerAddMessage_tf) ( IN  HANDLE          hCanShd,
                                                   IN  PCANCYCLICTXMSG pMessage,
                                                   OUT PUINT32         pdwIndex );

   typedef HRESULT VCIAPI (*canSchedulerRemMessage_tf) ( IN HANDLE hCanShd,
                                                   IN UINT32 dwIndex );

   typedef HRESULT VCIAPI (*canSchedulerStartMessage_tf) ( IN HANDLE hCanShd,
                                                     IN UINT32 dwIndex,
                                                     IN UINT16 wRepeat );

   typedef HRESULT VCIAPI (*canSchedulerStopMessage_tf) ( IN HANDLE hCanShd,
                                                    IN UINT32 dwIndex );



   vciEnumDeviceOpen_tf  pfnVciEnumDeviceOpenP;
//   VciInitialize_tf      pfnVciInitializeP;
//   CAN_InitializeFD_tf    pfnCAN_InitializeFDP;
//   CAN_Uninitialize_tf    pfnCAN_UninitializeP;
//   CAN_Reset_tf           pfnCAN_ResetP;
//   CAN_GetStatus_tf       pfnCAN_GetStatusP;
//   CAN_Read_tf            pfnCAN_ReadP;
//   CAN_ReadFD_tf          pfnCAN_ReadFDP;
//   CAN_Write_tf           pfnCAN_WriteP;
//   CAN_WriteFD_tf         pfnCAN_WriteFDP;
//   CAN_FilterMessages_tf  pfnCAN_FilterMessagesP;
//   CAN_GetValue_tf        pfnCAN_GetValueP;
//   CAN_SetValue_tf        pfnCAN_SetValueP;
//   CAN_GetErrorText_tf    pfnCAN_GetErrorTextP;

   /*!
    * \brief clCanLibP
    */
   QLibrary clCanLibP;


   /*!
    * \brief uwCanChannelP
    */
   uint16_t uwCanChannelP = 0;

   bool     btLibFuncLoadP = false;
public:
    QString echo(const QString &message) Q_DECL_OVERRIDE;
    int32_t setBitrate(uint32_t ulBitrateV, uint32_t ulBrsClockV) Q_DECL_OVERRIDE;
    int32_t	setMode(const uint32_t ulModeV) Q_DECL_OVERRIDE;
    int32_t	state(void) Q_DECL_OVERRIDE;
    int32_t	statistic(QCanStatistic_ts &clStatisticR) Q_DECL_OVERRIDE;
    int32_t	read(QCanFrame &clFrameR) Q_DECL_OVERRIDE;
    int32_t	write(const QCanFrame &clFrameR) Q_DECL_OVERRIDE;
    int32_t connect(void) Q_DECL_OVERRIDE;
    void disconnect(void) Q_DECL_OVERRIDE;

signals:
    void errorOccurred(int32_t slCanBusErrorV);
};

#endif
