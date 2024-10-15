//////////////////////////////////////////////////////////////////////////
// IXXAT Automation GmbH
//////////////////////////////////////////////////////////////////////////
/**
  VCI driver and device GUIDs.

  @file "vci4111guid.h"
*/
//////////////////////////////////////////////////////////////////////////
// (C) 2002-2011 IXXAT Automation GmbH,all rights reserved
//////////////////////////////////////////////////////////////////////////

#ifndef VCI4111GUID_H
#define VCI4111GUID_H

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
 * Driver and Device Classes from the CAN passive PCIe driver (VCI111Wx.SYS)
 ****************************************************************************
 * DriverClass = {D90EEF50-D52A-4ffa-A5BC-1D8D33E28B27}
 * DeviceClass = {3624894F-2F83-49d4-A874-8DB120AB5913} // CAN-IB 200
 * DeviceClass = {6082B7B5-B305-45fc-A3A2-7F97C9F92865} // CAN-IB 400
 * DeviceClass = {07797568-2F25-491a-862D-9254BFEBF2CD} // CAN-IB 600
 * DeviceClass = {CCD3C223-435F-4975-8317-0D29B0102B83} // CAN-IB 800
 * DeviceClass = {FF24FDC9-D59C-4beb-8B20-E17EDA06F7F1} // TSPSYS
 ****************************************************************************/
DEFINE_GUID(GUID_CANIB200_PCIE_DRIVER,
       0xd90eef50,0xd52a,0x4ffa,0xa5,0xbc,0x1d,0x8d,0x33,0xe2,0x8b,0x27);

DEFINE_GUID(GUID_CANIB200_PCIE_DEVICE,
       0x3624894f,0x2f83,0x49d4,0xa8,0x74,0x8d,0xb1,0x20,0xab,0x59,0x13);
DEFINE_GUID(GUID_CANIB400_PCI_DEVICE,
       0x6082b7b5,0xb305,0x45fc,0xa3,0xa2,0x7f,0x97,0xc9,0xf9,0x28,0x65);
DEFINE_GUID(GUID_CANIB600_PCIE_DEVICE,
       0x07797568,0x2f25,0x491a,0x86,0x2d,0x92,0x54,0xbf,0xeb,0xf2,0xcd);
DEFINE_GUID(GUID_CANIB800_PCI_DEVICE,
       0xccd3c223,0x435f,0x4975,0x83,0x17,0x0d,0x29,0xb0,0x10,0x2b,0x83);
DEFINE_GUID(GUID_TSPSYS_PCIE_DEVICE,
       0xff24fdc9,0xd59c,0x4beb,0x8b,0x20,0xe1,0x7e,0xda,0x06,0xf7,0xf1);





#endif //_VCI4111GUID_H_
