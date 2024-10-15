/*****************************************************************************
 IXXAT Automation GmbH
******************************************************************************

 Summary : GUID for the FRC-EP190 device.
 Date    : 2015-09-07

******************************************************************************
 all rights reserved
*****************************************************************************/

#ifndef _VCIGUID_FRC_H_
#define _VCIGUID_FRC_H_

#include <stdtype.h>

/*****************************************************************************
 * Device Class for VCI4FRC user mode driver 
 *****************************************************************************/

// {3E202702-1106-435d-A998-221693513FFB}
DEFINE_GUID(GUID_FRC_EP170_DEVICE, 
            0x3E202702, 0x1106, 0x435d, 0xA9, 0x98, 0x22, 0x16, 0x93, 0x51, 0x3F, 0xFB);

// {AA63AFCF-9D39-4f30-B4D5-AA1ACEE8AB6F}
DEFINE_GUID(GUID_FRC_EP190_DEVICE, 
            0xaa63afcf, 0x9d39, 0x4f30, 0xb4, 0xd5, 0xaa, 0x1a, 0xce, 0xe8, 0xab, 0x6f);

// {93355360-2FE1-40EB-BFA5-B2CBEF4FBFE1}
DEFINE_GUID(GUID_FRC_CANNECTOR_DEVICE, 
            0x93355360, 0x2fe1, 0x40eb, 0xbf, 0xa5, 0xb2, 0xcb, 0xef, 0x4f, 0xbf, 0xe1);


// for compatibility reasons
#define GUID_VCI4FRC_DEVICE GUID_FRC_EP190_DEVICE

#endif //_VCIGUID_FRC_H_
