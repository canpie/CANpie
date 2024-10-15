//////////////////////////////////////////////////////////////////////////
// IXXAT Automation GmbH
//////////////////////////////////////////////////////////////////////////
/**
  VCI driver and device GUIDs.

  @file "vciguid.h"
*/
//////////////////////////////////////////////////////////////////////////
// (C) 2002-2011 IXXAT Automation GmbH,all rights reserved
//////////////////////////////////////////////////////////////////////////

#ifndef VCI4005GUID_H
#define VCI4005GUID_H

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
 * Driver and Device Classes from the CAN passive PCIe driver (VCI005Wx.SYS)
 ****************************************************************************
 * DriverClass = {4D579B48-46CC-4445-AA8D-C0C174A9B40C}
 * DeviceClass = {1448FCFF-73DC-478a-B60D-058E374417C8} // CAN-IB100 Device
 * DeviceClass = {2F7B75C6-A3F0-4f0b-94C7-010B663B7E5A} // CAN-IB300 Device
 * DeviceClass = {84A67EBC-2C6D-4b9b-A75F-768F638A4669} // CAN-IB400 Device
 * DeviceClass = {89545925-7FE7-4353-98F6-65156A47B27A} // CAN-IB700 Device

 ****************************************************************************/
DEFINE_GUID(GUID_CANIB100_PCIE_DRIVER,
       0x4d579b48,0x46cc,0x4445,0xaa,0x8d,0xc0,0xc1,0x74,0xa9,0xb4,0xc);
DEFINE_GUID(GUID_CANIB100_PCIE_DEVICE,
       0x1448fcff,0x73dc,0x478a,0xb6,0x0d,0x05,0x8e,0x37,0x44,0x17,0xc8);
DEFINE_GUID(GUID_CANIB300_PCI_DEVICE,
       0x2f7b75c6,0xa3f0,0x4f0b,0x94,0xc7,0x01,0x0b,0x66,0x3b,0x7e,0x5a);
DEFINE_GUID(GUID_CANIB500_PCIE_DEVICE,
       0x84a67ebc,0x2c6d,0x4b9b,0xa7,0x5f,0x76,0x8f,0x63,0x8a,0x46,0x69);
DEFINE_GUID(GUID_CANIB700_PCI_DEVICE,
       0x89545925,0x7fe7,0x4353,0x98,0xf6,0x65,0x15,0x6a,0x47,0xb2,0x7a);




#endif //_VCI4005GUID_H_
