//////////////////////////////////////////////////////////////////////////
// IXXAT Automation GmbH
//////////////////////////////////////////////////////////////////////////
/**
  VCI driver and device GUIDs.

  @file "vci4117guid.h"
*/
//////////////////////////////////////////////////////////////////////////
// (C) 2002-2011 IXXAT Automation GmbH,all rights reserved
//////////////////////////////////////////////////////////////////////////

#ifndef VCI4117GUID_H
#define VCI4117GUID_H

//////////////////////////////////////////////////////////////////////////
//  include files
//////////////////////////////////////////////////////////////////////////
#include <stdtype.h>

//////////////////////////////////////////////////////////////////////////
//  constants and macros
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// data types
//////////////////////////////////////////////////////////////////////////

/*****************************************************************************
 * Driver and Device Classes from the USB-BGI driver (VCI117Wx.SYS)
 ****************************************************************************
 * DriverClass = {72E6137E-7F58-49b4-A8B4-1F8012B5AEC1}
 * DeviceClass = {EDCCFFED-C78C-4242-B617-56320E33A361} CANbridge
 * DeviceClass = {0F254982-BCEE-40ac-8EB8-906F773D00FA} CAN@net NT
 * DeviceClass = {F70A8D3A-CBE0-42b6-8F04-E3E3B46DD3B3} CAN@net NT Ethernet
  ****************************************************************************/
DEFINE_GUID(GUID_USB_CANATNET_NT_DRIVER,
            0x72e6137e, 0x7f58, 0x49b4, 0xa8, 0xb4, 0x1f, 0x80, 0x12, 0xb5, 0xae, 0xc1);

DEFINE_GUID(GUID_USB_CANBRIDGE_DEVICE, 
            0xedccffed, 0xc78c, 0x4242, 0xb6, 0x17, 0x56, 0x32, 0xe, 0x33, 0xa3, 0x61);

DEFINE_GUID(GUID_USB_CANATNET_NT_DEVICE, 
            0xf254982, 0xbcee, 0x40ac, 0x8e, 0xb8, 0x90, 0x6f, 0x77, 0x3d, 0x0, 0xfa);
            
DEFINE_GUID(GUID_ETH_CANATNET_NT_DEVICE, 
            0xf70a8d3a, 0xcbe0, 0x42b6, 0x8f, 0x4, 0xe3, 0xe3, 0xb4, 0x6d, 0xd3, 0xb3);
            


#endif //_VCI4117GUID_H_
