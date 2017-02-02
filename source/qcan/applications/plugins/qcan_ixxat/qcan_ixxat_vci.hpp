//============================================================================//
// File:          qcan_ixxat_vci.hpp                                          //
// Description:   Virtual Communication Interface                             //
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


#ifndef QCAN_IXXAT_VCI_H_
#define QCAN_IXXAT_VCI_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include <QObject>

#ifdef Q_OS_WIN32
#include <windows.h>
#include "vcinpl.h"

#define QCAN_IXXATLIB "vcinpl.dll"
#ifndef DRV_CALLBACK_TYPE
#define DRV_CALLBACK_TYPE VCIAPI
#endif

#else
#define DRV_CALLBACK_TYPE
#endif

#include <QLibrary>
#include <QDebug>

//-----------------------------------------------------------------------------
/*!
** \class   QCanIxxatVci
** \brief   IXXAT VCI
** 
*/
class QCanIxxatVci
{

private:

   QLibrary clCanLibP;
   bool btLibFuncLoadP;

   QCanIxxatVci();
   QCanIxxatVci(const QCanIxxatVci &);


public:
   ~QCanIxxatVci();

   // allow only one instance of PCAN Basic
   static QCanIxxatVci & getInstance()
   {
      static QCanIxxatVci clInstanceS;
      return clInstanceS;
   }

   // helper functions
   bool isAvailable (void);
   QString formatedError(HRESULT hrError);



   //----------------------------------------------------------------
   // Type definitions of PCANBasic.dll taken from vcinpl.h
   // API documentation can be found in "VCI V3 C-API Manual 1.5" here:
   // http://www.ixxat.com/de/support/downloads-dateien-dokumente/treiber
   // http://www.ixxat.com/support/file-and-documents-download/drivers/vci-v3-driver-download
   //

   /*!
    * device manager specific functions
    */
   typedef HRESULT (DRV_CALLBACK_TYPE *vciEnumDeviceOpen_tf)     ( OUT PHANDLE hEnum );
   typedef HRESULT (DRV_CALLBACK_TYPE *vciEnumDeviceClose_tf)    ( IN HANDLE hEnum );
   typedef HRESULT (DRV_CALLBACK_TYPE *vciEnumDeviceNext_tf)     ( IN HANDLE hEnum, OUT PVCIDEVICEINFO pInfo );
   typedef HRESULT (DRV_CALLBACK_TYPE *vciEnumDeviceReset_tf)    ( IN HANDLE hEnum );
   typedef HRESULT (DRV_CALLBACK_TYPE *vciEnumDeviceWaitEvent_tf)( IN HANDLE hEnum, IN UINT32 dwMsTimeout );
   typedef HRESULT (DRV_CALLBACK_TYPE *vciFindDeviceByHwid_tf)   ( IN REFGUID rHwid, OUT PVCIID pVciid );
   typedef HRESULT (DRV_CALLBACK_TYPE *vciFindDeviceByClass_tf)  ( IN REFGUID rClass, IN  UINT32 dwInst, OUT PVCIID pVciid );
   typedef HRESULT (DRV_CALLBACK_TYPE *vciSelectDeviceDlg_tf)    ( IN HWND hwndParent, OUT PVCIID pVciid );
   typedef void    (DRV_CALLBACK_TYPE *vciFormatError_tf)        ( IN HRESULT hrError, OUT PCHAR pszText, IN UINT32 dwSize );

   /*!
    * device specific functions
    */
   typedef HRESULT (DRV_CALLBACK_TYPE *vciDeviceOpen_tf)   ( IN REFVCIID rVciid, OUT PHANDLE phDevice );
   typedef HRESULT (DRV_CALLBACK_TYPE *vciDeviceOpenDlg_tf)( IN HWND hwndParent, OUT PHANDLE phDevice );
   typedef HRESULT (DRV_CALLBACK_TYPE *vciDeviceClose_tf)  ( IN HANDLE hDevice );
   typedef HRESULT (DRV_CALLBACK_TYPE *vciDeviceGetInfo_tf)( IN HANDLE hDevice, OUT PVCIDEVICEINFO pInfo );
   typedef HRESULT (DRV_CALLBACK_TYPE *vciDeviceGetCaps_tf)( IN HANDLE hDevice, OUT PVCIDEVICECAPS pCaps );

   /*!
    * CAN controller specific functions
    */
   typedef HRESULT (DRV_CALLBACK_TYPE *canControlOpen_tf)         ( IN HANDLE   hDevice,
                                                         IN UINT32   dwCanNo,
                                                         OUT PHANDLE phCanCtl );
   typedef HRESULT (DRV_CALLBACK_TYPE *canControlClose_tf)        ( IN HANDLE hCanCtl );
   typedef HRESULT (DRV_CALLBACK_TYPE *canControlGetCaps_tf)      ( IN HANDLE hCanCtl, OUT PCANCAPABILITIES pCanCaps );
   typedef HRESULT (DRV_CALLBACK_TYPE *canControlGetStatus_tf)    ( IN HANDLE hCanCtl, OUT PCANLINESTATUS pStatus );
   typedef HRESULT (DRV_CALLBACK_TYPE *canControlDetectBitrate_tf)( IN HANDLE  hCanCtl,
                                                         IN UINT16  wMsTimeout,
                                                         IN UINT32  dwCount,
                                                         IN PUINT8  pabBtr0,
                                                         IN PUINT8  pabBtr1,
                                                         OUT PINT32 plIndex );
   typedef HRESULT (DRV_CALLBACK_TYPE *canControlInitialize_tf)   ( IN HANDLE hCanCtl,
                                                         IN UINT8  bMode,
                                                         IN UINT8  bBtr0,
                                                         IN UINT8  bBtr1 );
   typedef HRESULT (DRV_CALLBACK_TYPE *canControlReset_tf)        ( IN HANDLE hCanCtl );
   typedef HRESULT (DRV_CALLBACK_TYPE *canControlStart_tf)        ( IN HANDLE hCanCtl,
                                                         IN BOOL   fStart );
   typedef HRESULT (DRV_CALLBACK_TYPE *canControlSetAccFilter_tf) ( IN HANDLE hCanCtl,
                                                         IN BOOL   fExtend,
                                                         IN UINT32 dwCode,
                                                         IN UINT32 dwMask );
   typedef HRESULT (DRV_CALLBACK_TYPE *canControlAddFilterIds_tf) ( IN HANDLE hCanCtl,
                                                         IN BOOL   fExtend,
                                                         IN UINT32 dwCode,
                                                         IN UINT32 dwMask );
   typedef HRESULT (DRV_CALLBACK_TYPE *canControlRemFilterIds_tf) ( IN HANDLE hCanCtl,
                                                         IN BOOL   fExtend,
                                                         IN UINT32 dwCode,
                                                         IN UINT32 dwMask );

   /*!
   * CAN message channel specific functions
   */
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelOpen_tf) ( IN  HANDLE  hDevice,
                                                 IN  UINT32  dwCanNo,
                                                 IN  BOOL    fExclusive,
                                                 OUT PHANDLE phCanChn );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelClose_tf) ( IN HANDLE hCanChn );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelGetCaps_tf) ( IN  HANDLE           hCanChn,
                                                    OUT PCANCAPABILITIES pCanCaps );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelGetStatus_tf)  ( IN  HANDLE         hCanChn,
                                                       OUT PCANCHANSTATUS pStatus );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelInitialize_tf) ( IN HANDLE hCanChn,
                                                       IN UINT16 wRxFifoSize,
                                                       IN UINT16 wRxThreshold,
                                                       IN UINT16 wTxFifoSize,
                                                       IN UINT16 wTxThreshold );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelActivate_tf)   ( IN HANDLE hCanChn,
                                                       IN BOOL   fEnable );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelPeekMessage_tf) ( IN  HANDLE  hCanChn,
                                                        OUT PCANMSG pCanMsg );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelPostMessage_tf) ( IN HANDLE  hCanChn,
                                                        IN PCANMSG pCanMsg );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelWaitRxEvent_tf) ( IN HANDLE hCanChn,
                                                        IN UINT32 dwMsTimeout );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelWaitTxEvent_tf) ( IN HANDLE hCanChn,
                                                        IN UINT32 dwMsTimeout );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelReadMessage_tf) ( IN  HANDLE  hCanChn,
                                                        IN  UINT32  dwMsTimeout,
                                                        OUT PCANMSG pCanMsg );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelSendMessage_tf) ( IN HANDLE  hCanChn,
                                                        IN UINT32  dwMsTimeout,
                                                        IN PCANMSG pCanMsg );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelPeekMultipleMessages_tf)
                                    ( IN     HANDLE  hCanChn,
                                      IN OUT UINT32 * pdwNum,
                                      OUT    CANMSG aCanMsg[] );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelPostMultipleMessages_tf)
                                    ( IN     HANDLE  hCanChn,
                                      IN OUT UINT32 * pdwNum,
                                      IN     CANMSG aCanMsg[] );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelReadMultipleMessages_tf)
                                    ( IN     HANDLE  hCanChn,
                                      IN     UINT32  dwMsTimeout,
                                      IN OUT UINT32 * pdwNum,
                                      OUT    CANMSG aCanMsg[] );
   typedef HRESULT (DRV_CALLBACK_TYPE *canChannelSendMultipleMessages_tf)
                                    ( IN     HANDLE  hCanChn,
                                      IN     UINT32  dwMsTimeout,
                                      IN OUT UINT32 * pdwNum,
                                      IN     CANMSG aCanMsg[] );

   /*!
    * cyclic CAN message scheduler specific functions
    */
   typedef HRESULT (DRV_CALLBACK_TYPE *canSchedulerOpen_tf)         ( IN  HANDLE  hDevice,
                                                           IN  UINT32  dwCanNo,
                                                           OUT PHANDLE phCanShd );
   typedef HRESULT (DRV_CALLBACK_TYPE *canSchedulerClose_tf)        ( IN HANDLE hCanShd );
   typedef HRESULT (DRV_CALLBACK_TYPE *canSchedulerGetCaps_tf)      ( IN  HANDLE           hCanShd,
                                                           OUT PCANCAPABILITIES pCanCaps );
   typedef HRESULT (DRV_CALLBACK_TYPE *canSchedulerGetStatus_tf)    ( IN  HANDLE              hCanShd,
                                                           OUT PCANSCHEDULERSTATUS pStatus );
   typedef HRESULT (DRV_CALLBACK_TYPE *canSchedulerActivate_tf)     ( IN HANDLE hCanShd,
                                                           IN BOOL   fEnable );
   typedef HRESULT (DRV_CALLBACK_TYPE *canSchedulerReset_tf)        ( IN HANDLE hCanShd );
   typedef HRESULT (DRV_CALLBACK_TYPE *canSchedulerAddMessage_tf)   ( IN  HANDLE          hCanShd,
                                                           IN  PCANCYCLICTXMSG pMessage,
                                                           OUT PUINT32         pdwIndex );
   typedef HRESULT (DRV_CALLBACK_TYPE *canSchedulerRemMessage_tf)   ( IN HANDLE hCanShd,
                                                           IN UINT32 dwIndex );
   typedef HRESULT (DRV_CALLBACK_TYPE *canSchedulerStartMessage_tf) ( IN HANDLE hCanShd,
                                                           IN UINT32 dwIndex,
                                                           IN UINT16 wRepeat );
   typedef HRESULT (DRV_CALLBACK_TYPE *canSchedulerStopMessage_tf)  ( IN HANDLE hCanShd,
                                                           IN UINT32 dwIndex );


   /*!
    * device manager specific functions
    */
   vciEnumDeviceOpen_tf       pfnVciEnumDeviceOpenP;
   vciEnumDeviceClose_tf      pfnVciEnumDeviceCloseP;
   vciEnumDeviceNext_tf       pfnVciEnumDeviceNextP;
   vciEnumDeviceReset_tf      pfnVciEnumDeviceResetP;
   vciEnumDeviceWaitEvent_tf  pfnVciEnumDeviceWaitEventP;
   vciFindDeviceByHwid_tf     pfnVciFindDeviceByHwidP;
   vciFindDeviceByClass_tf    pfnVciFindDeviceByClassP;
   vciSelectDeviceDlg_tf      pfnVciSelectDeviceDlgP;
   vciFormatError_tf          pfnVciFormatErrorP;

   /*!
    * device specific functions
    */
   vciDeviceOpen_tf           pfnVciDeviceOpenP;
   vciDeviceOpenDlg_tf        pfnVciDeviceOpenDlgP;
   vciDeviceClose_tf          pfnVciDeviceCloseP;
   vciDeviceGetInfo_tf        pfnVciDeviceGetInfoP;
   vciDeviceGetCaps_tf        pfnVciDeviceGetCapsP;

   /*!
    * CAN controller specific functions
    */
   canControlOpen_tf          pfnCanControlOpenP;
   canControlClose_tf         pfnCanControlCloseP;
   canControlGetCaps_tf       pfnCanControlGetCapsP;
   canControlGetStatus_tf     pfnCanControlGetStatusP;
   canControlDetectBitrate_tf pfnCanControlDetectBitrateP;
   canControlInitialize_tf    pfnCanControlInitializeP;
   canControlReset_tf         pfnCanControlResetP;
   canControlStart_tf         pfnCanControlStartP;
   canControlSetAccFilter_tf  pfnCanControlSetAccFilterP;
   canControlAddFilterIds_tf  pfnCanControlAddFilterIdsP;
   canControlRemFilterIds_tf  pfnCanControlRemFilterIdsP;

   /*!
    * CAN message channel specific functions
    */
   canChannelOpen_tf          pfnCanChannelOpenP;
   canChannelClose_tf         pfnCanChannelCloseP;
   canChannelGetCaps_tf       pfnCanChannelGetCapsP;
   canChannelGetStatus_tf     pfnCanChannelGetStatusP;
   canChannelInitialize_tf    pfnCanChannelInitializeP;
   canChannelActivate_tf      pfnCanChannelActivateP;
   canChannelPeekMessage_tf   pfnCanChannelPeekMessageP;
   canChannelPostMessage_tf   pfnCanChannelPostMessageP;
   canChannelWaitRxEvent_tf   pfnCanChannelWaitRxEventP;
   canChannelWaitTxEvent_tf   pfnCanChannelWaitTxEventP;
   canChannelReadMessage_tf   pfnCanChannelReadMessageP;
   canChannelSendMessage_tf   pfnCanChannelSendMessageP;
   canChannelPeekMultipleMessages_tf pfnCanChannelPeekMultipleMessagesP;
   canChannelPostMultipleMessages_tf pfnCanChannelPostMultipleMessagesP;
   canChannelReadMultipleMessages_tf pfnCanChannelReadMultipleMessagesP;
   canChannelSendMultipleMessages_tf pfnCanChannelSendMultipleMessagesP;

   /*!
    * cyclic CAN message scheduler specific functions
    */
   canSchedulerOpen_tf        pfnCanSchedulerOpenP;
   canSchedulerClose_tf       pfnCanSchedulerCloseP;
   canSchedulerGetCaps_tf     pfnCanSchedulerGetCapsP;
   canSchedulerGetStatus_tf   pfnCanSchedulerGetStatusP;
   canSchedulerActivate_tf    pfnCanSchedulerActivateP;
   canSchedulerReset_tf       pfnCanSchedulerResetP;
   canSchedulerAddMessage_tf  pfnCanSchedulerAddMessageP;
   canSchedulerRemMessage_tf  pfnCanSchedulerRemMessageP;
   canSchedulerStartMessage_tf pfnCanSchedulerStartMessageP;
   canSchedulerStopMessage_tf pfnCanSchedulerStopMessageP;


};

#endif /*QCAN_IXXAT_VCI_H_*/
