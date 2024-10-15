/*****************************************************************************
 HMS Technology Center Ravensburg GmbH
******************************************************************************

 File    : VCI002.H
 Summary : Driver specific declaration.

 Date    : 2015-04-15

 Compiler: MSVC

******************************************************************************
 all rights reserved
*****************************************************************************/

#ifndef _VCI002_H_
#define _VCI002_H_

#include <stdtype.h>

/*****************************************************************************
 * Driver and Device Classes for PC-I04/PCI driver (VCI002Wx.SYS)
 ****************************************************************************
 * DriverClass = {0971DA4A-D77A-42A2-8E36-9E5B3BD7747D}
 * DeviceClass = {E7752D2E-28A3-4318-AF0C-02C756FBC25A}
 ****************************************************************************/
DEFINE_GUID(GUID_PCI04PCI_DRIVER,
       0x0971DA4A,0xD77A,0x42A2,0x8E,0x36,0x9E,0x5B,0x3B,0xD7,0x74,0x7D);

DEFINE_GUID(GUID_PCI04PCI_DEVICE,
       0xE7752D2E,0x28A3,0x4318,0xAF,0x0C,0x02,0xC7,0x56,0xFB,0xC2,0x5A);

#endif //_VCI002_H_
