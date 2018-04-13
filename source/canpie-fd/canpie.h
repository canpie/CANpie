//============================================================================//
// File:          canpie.h                                                    //
// Description:   General CAN driver definitions and structures               //
//                                                                            //
// Copyright 2017 MicroControl GmbH & Co. KG                                  //
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
// Licensed under the Apache License, Version 2.0 (the "License");            //
// you may not use this file except in compliance with the License.           //
// You may obtain a copy of the License at                                    //
//                                                                            //
//    http://www.apache.org/licenses/LICENSE-2.0                              //
//                                                                            //
// Unless required by applicable law or agreed to in writing, software        //
// distributed under the License is distributed on an "AS IS" BASIS,          //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   //
// See the License for the specific language governing permissions and        //
// limitations under the License.                                             //
//============================================================================//


#ifndef  CANPIE_H_
#define  CANPIE_H_


//-----------------------------------------------------------------------------
/*!
** \file    canpie.h
** \brief   %CANpie constants, structures and enumerations
**
** This file holds constants and structures used within %CANpie FD.
** All functions, structures, defines and constants in %CANpie FD
** have the prefix "Cp". The following table shows the used nomenclature:
**
** <div class="function" style="width:400px">
**<table class="function" style="width:400px">
**<tr class="head">
**<td class="entry" style="width:50%"><b>%CANpie code</b></td>
**<td class="desc"><b>Nomenclature</b></td>
**</tr>
**<tr class="odd">
**<td class="entry" style="width:50%">Core functions</td>
**<td class="desc">CpCore<Name></td>
**</tr>
**<tr>
**<td class="entry" style="width:50%">Message access functions</td>
**<td class="desc">CpMsg<Name></td>
**</tr>
**<tr class="odd">
**<td class="entry" style="width:50%">Structures</td>
**<td class="desc">Cp<Name>_ts</td>
**</tr>
**<tr>
**<td class="entry" style="width:25%">Definitions</td>
**<td class="desc">CP_<NAME></td>
**</tr>
**<tr class="odd">
**<td class="entry" style="width:50%">Enumerations</td>
**<td class="desc">eCP_<NAME></td>
**</tr>
**</table>
**</div>
*/


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "mc_compiler.h"
#include "cp_platform.h"       // Architecture dependent definitions


/*----------------------------------------------------------------------------*\
** Definitions & Enumerations                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
/*!
** \defgroup CP_CONF  CANpie configuration options
**
** The %CANpie FD driver can be configured during compile time
** via several configuration options. They are typically defined in the
** \c cp_platform.h configuration file.
** <p>
** If symbols are not defined, they get a default value which is
** assigned in the \c canpie.h header file.
*/

/*-------------------------------------------------------------------*/
/*!
** \def  CP_AUTOBAUD
** \ingroup CP_CONF
**
** This symbol enables or disables the automatic bit-rate detection
** feature of the CAN driver. The default value is 0 (disabled).
**
** - 0 = disable bit-rate detection (not supported by hardware)
** - 1 = enable bit-rate detection
*/
#ifndef  CP_AUTOBAUD
#define  CP_AUTOBAUD                0
#endif

/*-------------------------------------------------------------------*/
/*!
** \def  CP_BUFFER_MAX
** \ingroup CP_CONF
**
** This symbol defines the number of message buffers (mailboxes)
** of a CAN controller. In case the controller has no message buffers,
** it is also possible to emulate these. A value of 0 is not allowed
** for this symbol.
*/
#ifndef  CP_BUFFER_MAX
#define  CP_BUFFER_MAX              8
#endif

/*-------------------------------------------------------------------*/
/*!
** \def  CP_CAN_FD
** \ingroup CP_CONF
**
** This symbol defines if ISO CAN FD support is enabled.
** - 0 = disable ISO CAN FD support
** - 1 = enable ISO CAN FD support
*/
#ifndef  CP_CAN_FD
#define  CP_CAN_FD                  0
#endif


/*-------------------------------------------------------------------*/
/*!
** \def  CP_CAN_MSG_MACRO
** \ingroup CP_CONF
**
** This symbol defines if access to the CAN message structure
** CpCanMsg_s is done via macros or via functions.
** - 0 = access via functions
** - 1 = access via macros
*/
#ifndef  CP_CAN_MSG_MACRO
#define  CP_CAN_MSG_MACRO           1
#endif

/*-------------------------------------------------------------------*/
/*!
** \def  CP_CAN_MSG_MARKER
** \ingroup CP_CONF
**
** This symbol defines if the CAN message structure CpCanMsg_s
** has a marker field.
** - 0 = no marker field (not supported by hardware / driver)
** - 1 = include marker field
*/
#ifndef  CP_CAN_MSG_MARKER
#define  CP_CAN_MSG_MARKER          0
#endif

/*-------------------------------------------------------------------*/
/*!
** \def  CP_CAN_MSG_TIME
** \ingroup CP_CONF
**
** This symbol defines if the CAN message structure CpCanMsg_s
** has a time-stamp field.
** - 0 = no time-stamp field (not supported by hardware / driver)
** - 1 = include time-stamp field
*/
#ifndef  CP_CAN_MSG_TIME
#define  CP_CAN_MSG_TIME            0
#endif

/*-------------------------------------------------------------------*/
/*!
** \def  CP_CAN_MSG_USER
** \ingroup CP_CONF
**
** This symbol defines if the CAN message structure CpCanMsg_s
** has a user-defined field.
** - 0 = no user-defined field (not supported by driver)
** - 1 = include user-defined field
*/
#ifndef  CP_CAN_MSG_USER
#define  CP_CAN_MSG_USER            1
#endif

/*-------------------------------------------------------------------*/
/*!
** \def  CP_CHANNEL_MAX
** \ingroup CP_CONF
**
** This symbol defines the total number of physical CAN interfaces
** supported by the driver. For a LPC2294 microcontroller the value
** would be 4 (4 CAN channels).
*/
#ifndef  CP_CHANNEL_MAX
#define  CP_CHANNEL_MAX             1
#endif

/*-------------------------------------------------------------------*/
/*!
** \def  CP_SMALL_CODE
** \ingroup CP_CONF
**
** This symbol is used to control the usage of the \c ptsPortV
** parameter in the core functions during compilation time. For
** microcontrollers with small resources and only one CAN channel
** the port parameter can be omitted.
** - 0 = use \c ptsPortV parameter in core functions
** - 1 = do not use \c ptsPortV parameter in core functions
*/
#ifndef  CP_SMALL_CODE
#define  CP_SMALL_CODE              0
#endif

/*-------------------------------------------------------------------*/
/*!
** \def  CP_STATISTIC
** \ingroup CP_CONF
**
** This symbol defines if the driver support statistic information.
** A value of 0 denotes that no statistic information is available.
** This also means that the function CpCoreStatistic() returns the
** error code #eCP_ERR_NOT_SUPPORTED.
** - 0 = no statistic information (not supported by driver)
** - 1 = enable statistic information
*/
#ifndef  CP_STATISTIC
#define  CP_STATISTIC               0
#endif


//-----------------------------------------------------------------------------
/*!
** \defgroup CP_VERSION  CANpie version information
**
** The %CANpie FD version information read during run-time via the
** function CpCoreHDI().
*/

/*-------------------------------------------------------------------*/
/*!
** \def  CP_VERSION_MAJOR
** \ingroup CP_VERSION
**
** This symbol defines the driver version major value.
*/
#define  CP_VERSION_MAJOR           3

/*-------------------------------------------------------------------*/
/*!
** \def  CP_VERSION_MINOR
** \ingroup CP_VERSION
**
** This symbol defines the driver version minor value.
*/
#define  CP_VERSION_MINOR           2


//-----------------------------------------------------------------------------
/*!
** \defgroup CP_MASK Mask values for CAN messages
**
** The following definitions are used in combination with the
** structure CpCanMsg_s.
*/

/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_STD_FRAME
** \ingroup CP_MASK
**
** Mask for standard frame (11 bits), used in combination with
** the CpCanMsg_s::ulIdentifier.
*/
#define  CP_MASK_STD_FRAME       ((uint32_t) 0x000007FF)


/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_EXT_FRAME
** \ingroup CP_MASK
**
** Mask for extended frame (29 bits), used in combination with
** the CpCanMsg_s::ulIdentifier.
*/
#define  CP_MASK_EXT_FRAME       ((uint32_t) 0x1FFFFFFF)

/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_MSG_FORMAT
** \ingroup CP_MASK
**
** Mask for message format, used in combination with \c ubMsgCtrl field
** of the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
** This mask covers #CP_MSG_FORMAT_CBFF, #CP_MSG_FORMAT_CEFF,
** #CP_MSG_FORMAT_FBFF and #CP_MSG_FORMAT_FEFF control flags.
*/
#define  CP_MASK_MSG_FORMAT      ((uint8_t) 0x03)

//-----------------------------------------------------------------------------
/*!
** \defgroup CP_MSG_CTRL Bit mask for message control
**
** The following definitions are used in combination with the
** structure CpCanMsg_s::ubMsgCtrl.
*/
/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_CTRL_EXT_BIT
** \ingroup CP_MSG_CTRL
**
** Bit mask for the EXT bit (extended frame) in the \c ubMsgCtrl
** field of the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MSG_CTRL_EXT_BIT     ((uint8_t) 0x01)


/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_CTRL_FDF_BIT
** \ingroup CP_MSG_CTRL
**
** Bit mask for the FDF bit (ISO CAN FD, Fast Data format) in
** the \c ubMsgCtrl field of the CpCanMsg_ts structure
** (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MSG_CTRL_FDF_BIT      ((uint8_t) 0x02)


/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_CTRL_RTR_BIT
** \ingroup CP_MSG_CTRL
**
** Bit mask for the RTR bit (remote frame) in the \c ubMsgCtrl
** field of the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MSG_CTRL_RTR_BIT     ((uint8_t) 0x04)


/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_CTRL_OVR_BIT
** \ingroup CP_MSG_CTRL
**
** Bit mask for the OVR bit (overrun) in the \c ubMsgCtrl
** field of the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MSG_CTRL_OVR_BIT     ((uint8_t) 0x08)


/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_CTRL_BRS_BIT
** \ingroup CP_MSG_CTRL
**
** Bit mask for the BRS bit (ISO CAN FD, bit-rate switch) in the
** \c ubMsgCtrl field of the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MSG_CTRL_BRS_BIT     ((uint8_t) 0x40)


/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_CTRL_ESI_BIT
** \ingroup CP_MSG_CTRL
**
** Bit mask for the ESI bit (ISO CAN FD, error state indicator) in the
** \c ubMsgCtrl field of the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MSG_CTRL_ESI_BIT     ((uint8_t) 0x80)


/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_FORMAT_CBFF
** \ingroup CP_MSG_CTRL
**
** Bit definition for a Classical CAN frame with Standard Identifier
** (CBFF: Classical base frame format) in the \c ubMsgCtrl field of
** the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MSG_FORMAT_CBFF      ((uint8_t) 0x00)

/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_FORMAT_CEFF
** \ingroup CP_MSG_CTRL
**
** Bit definition for a Classical CAN frame with Extended Identifier
** (CEFF: Classical extended frame format) in the \c ubMsgCtrl field
** of the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MSG_FORMAT_CEFF      (CP_MSG_CTRL_EXT_BIT)

/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_FORMAT_FBFF
** \ingroup CP_MSG_CTRL
**
** Bit definition for an ISO CAN FD frame with Standard Identifier
** (FBFF: FD base frame format) in the \c ubMsgCtrl field of the
** CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MSG_FORMAT_FBFF      (CP_MSG_CTRL_FDF_BIT)

/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_FORMAT_FEFF
** \ingroup CP_MSG_CTRL
**
** Bit definition for an ISO CAN FD frame with Extended Identifier
** (FEFF: FD extended frame format) in the \c ubMsgCtrl field of the
** CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MSG_FORMAT_FEFF      (CP_MSG_CTRL_FDF_BIT | CP_MSG_CTRL_EXT_BIT)


/*-------------------------------------------------------------------*/
/*!
** \def  CP_DATA_SIZE
**
** The symbol CP_DATA_SIZE defines the maximum number of bytes
** which are transferred inside the data field of a CAN message.
** For classic CAN frames the maximum number of bytes is 8, for
** CAN FD frames the maximum number of bytes is 64.
*/
#if CP_CAN_FD == 0
#define  CP_DATA_SIZE            ((uint8_t)  8)
#else
#define  CP_DATA_SIZE            ((uint8_t) 64)
#endif


/*----------------------------------------------------------------------------*/
/*!
** \enum    CpErr_e
** \brief   CANpie Error codes
**
** All functions that may cause an error condition will return an
** error code. The CANpie error codes are within the value range from
** 0 to 127. The designer of the core functions might extend the error code
** table with hardware specific error codes, which must be in the range
** from 128 to 255.
*/
enum CpErr_e
{

   /*!   No error (00dec / 00hex)
   */
   eCP_ERR_NONE = 0,

   /*!   Error not specified (01dec / 01hex)
   */
   eCP_ERR_GENERIC = 1,

   /*!   Hardware failure (02dec / 02hex)
   */
   eCP_ERR_HARDWARE = 2,

   /*!   Initialisation failure (03dec / 03hex)
   */
   eCP_ERR_INIT_FAIL = 3,

   /*!   Channel is initialised, ready to run (04dec / 04hex)
   */
   eCP_ERR_INIT_READY = 4,

   /*!    CAN channel was not initialised (05dec / 05hex)
   */
   eCP_ERR_INIT_MISSING = 5,

   /*!   Receive buffer is empty (06dec / 06hex)
   */
   eCP_ERR_RCV_EMPTY = 6,

   /*!   Receive buffer overrun (07dec / 07hex)
   */
   eCP_ERR_RCV_OVERRUN = 7,

   /*!   Transmit buffer is full (08dec / 08hex)
   */
   eCP_ERR_TRM_FULL = 8,

   /*!   CAN message has wrong format (10dec / 0Ahex)
   */
   eCP_ERR_CAN_MESSAGE = 10,

   /*!   CAN identifier not valid (11dec / 0Bhex)
   */
   eCP_ERR_CAN_ID = 11,

   /*!   CAN data length code not valid (12dec / 0Chex)
   */
   eCP_ERR_CAN_DLC = 12,

   /*!   FIFO is empty (20dec / 14hex)
   */
   eCP_ERR_FIFO_EMPTY = 20,

   /*!   Message is waiting in FIFO (21dec / 15hex)
   */
   eCP_ERR_FIFO_WAIT = 21,

   /*!   FIFO is full (22dec / 16hex)
   */
   eCP_ERR_FIFO_FULL = 22,

   /*!   FIFO size is out of range (23dec / 17hex)
   */
   eCP_ERR_FIFO_SIZE = 23,

   /*!   Parameter of FIFO function is out of range (24dec / 18hex)
   */
   eCP_ERR_FIFO_PARAM = 24,

   /*!   Controller is in error passive (30dec / 1Ehex)
   */
   eCP_ERR_BUS_PASSIVE = 30,

   /*!   Controller is in bus off (31dec / 1Fhex)
   */
   eCP_ERR_BUS_OFF = 31,

   /*!   Controller is in warning status (32dec / 20hex)
   */
   eCP_ERR_BUS_WARNING = 32,


   /*!   Channel out of range (40dec / 28hex)
   */
   eCP_ERR_CHANNEL = 40,

   /*!   Register address out of range (41dec / 29hex)
   */
   eCP_ERR_REGISTER = 41,

   /*!   bit-rate out of range (42dec / 2Ahex)
   */
   eCP_ERR_BITRATE = 42,

   /*!   Buffer number out of range (43dec / 2Bhex)
   */
   eCP_ERR_BUFFER = 43,

   /*!   Parameter number out of range (44dec / 2Chex)
   */
   eCP_ERR_PARAM = 44,

   /*!   Function is not supported (50dec / 32hex)
   */
   eCP_ERR_NOT_SUPPORTED = 50
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CpCallback_e
** \brief   Callback Return Codes
**
** These return values are used by the callback functions that can be
** installed by the function CpCoreIntFunctions(). <p>
** \b Example
** \code
** uint8_t MyCallback(CpCanMsg_ts * ptsCanMsgV, uint8_t ubBufferIdxV)
** {
**    // Do something with IDs < 100
**    if( CpMsgGetIdentifier(ptsCanMsgV) < 100)
**    {
**       //.....
**       return(eCP_CALLBACK_PROCESSED)
**    }
**
**    // Put all other messages into the FIFO
**    return (eCP_CALLBACK_PUSH_FIFO);
** }
** \endcode
** <br>
**
*/
enum CpCallback_e
{

   /*!
   ** Message was processed by callback and should not inserted in the FIFO
   */
   eCP_CALLBACK_PROCESSED = 0,

   /*!
   ** Message was not processed by callback and must be inserted in the FIFO
   */
   eCP_CALLBACK_PUSH_FIFO
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CpBitrate_e
** \brief   Fixed bit-rates
**
** The values of the enumeration CpBitrate_e are used as parameter for the
** function CpCoreBitrate().
*/
enum CpBitrate_e
{

   /*!
   ** No bit-rate defined
   */
   eCP_BITRATE_NONE = -1,

   /*!
   ** bit-rate 10 kBit/s
   */
   eCP_BITRATE_10K = 0,

   /*!
   ** bit-rate 20 kBit/s
   */
   eCP_BITRATE_20K = 1,

   /*!
   ** bit-rate 50 kBit/s
   */
   eCP_BITRATE_50K = 2,

   /*!
   ** bit-rate 100 kBit/s
   */
   eCP_BITRATE_100K = 3,

   /*!
   ** bit-rate 125 kBit/s
   */
   eCP_BITRATE_125K = 4,

   /*!
   ** bit-rate 250 kBit/s
   */
   eCP_BITRATE_250K = 5,

   /*!
   ** bit-rate 500 kBit/s
   */
   eCP_BITRATE_500K = 6,

   /*!
   ** bit-rate 800 kBit/s
   */
   eCP_BITRATE_800K = 7,

   /*!
   ** bit-rate 1 MBit/s
   */
   eCP_BITRATE_1M = 8,

   /*!
   ** bit-rate 2 MBit/s (ISO CAN FD, data phase)
   */
   eCP_BITRATE_2M = 10,

   /*!
   ** bit-rate 4 MBit/s (ISO CAN FD, data phase)
   */
   eCP_BITRATE_4M = 11,

   /*!
   ** bit-rate 5 MBit/s (ISO CAN FD, data phase)
   */
   eCP_BITRATE_5M = 12,

   /*!
   ** Use automatic bit-rate detection
   ** \internal Do not change the value of this enumerator in order
   **           to keep compatibility.
   */
   eCP_BITRATE_AUTO = 9,

   eCP_BITRATE_MAX = eCP_BITRATE_AUTO
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CpChannel_e
** \brief   Channel definition
**
** The physical CAN interfaces are numbered from 1 .. N (N: total
** number of physical CAN interfaces on the target system). The enumeration
** CpChannel_e lists up to 8 physical interfaces. The enumeration value
** eCP_CHANNEL_NONE denotes an invalid channel value.
*/
enum CpChannel_e
{

   /*! CAN interface invalid        */
   eCP_CHANNEL_NONE = 0,

   /*! CAN interface 1              */
   eCP_CHANNEL_1,

   /*! CAN interface 2              */
   eCP_CHANNEL_2,

   /*! CAN interface 3              */
   eCP_CHANNEL_3,

   /*! CAN interface 4              */
   eCP_CHANNEL_4,

   /*! CAN interface 5              */
   eCP_CHANNEL_5,

   /*! CAN interface 6              */
   eCP_CHANNEL_6,

   /*! CAN interface 7              */
   eCP_CHANNEL_7,

   /*! CAN interface 8              */
   eCP_CHANNEL_8
};




/*----------------------------------------------------------------------------*/
/*!
** \enum    CpMode_e
** \brief   Mode of CAN controller
**
** These values are used as parameter for the function CpCoreCanMode() in
** order to change the state of the CAN controller.
*/
enum CpMode_e
{
   /*!   Set controller in 'Initialization' mode (no reception or
   **    transmission possible)
   */
   eCP_MODE_INIT = 0,

   eCP_MODE_STOP = 0,         // deprecated definition


   /*!   Set controller into 'Operation' mode
   */
   eCP_MODE_OPERATION = 1,

   eCP_MODE_START = 1,        // deprecated definition

   /*!   Set controller into 'Listen-only' mode
   */
   eCP_MODE_LISTEN_ONLY = 2,

   /*!   Set controller into 'Sleep' (power-down) mode
   */
   eCP_MODE_SLEEP = 3,

   /*!   Set controller into 'Self-test' mode
   */
   eCP_MODE_SELF_TEST = 4

};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CpState_e
** \brief   State of CAN controller
**
** These values are used as return value for the function CpCoreCanState().
*/
enum CpState_e
{
   /*!
   ** CAN controller is in stopped mode
   */
   eCP_STATE_INIT = 0,

   eCP_STATE_STOPPED  = 0,    // deprecated definition

   /*!
   ** CAN controller is in Sleep mode
   */
   eCP_STATE_SLEEPING = 1,

   /*!
   ** CAN controller is error active
   */
   eCP_STATE_BUS_ACTIVE = 2,

   /*!
   ** CAN controller is active, warning level is reached
   */
   eCP_STATE_BUS_WARN = 3,

   /*!
   ** CAN controller is error passive
   */
   eCP_STATE_BUS_PASSIVE = 4,

   /*!
   ** CAN controller went into Bus Off
   */
   eCP_STATE_BUS_OFF = 5,

   /*!
   ** General failure of physical layer detected (if supported by hardware)
   */
   eCP_STATE_PHY_FAULT = 10,

   /*!
   ** Fault on CAN-H detected (Low Speed CAN)
   */
   eCP_STATE_PHY_H = 11,

   /*!
   ** Fault on CAN-L detected (Low Speed CAN)
   */
   eCP_STATE_PHY_L = 12

};

/*----------------------------------------------------------------------------*/
/*!
** \enum    CpErrType_e
** \brief   Error type
**
** These values are used as return value for the function CpCoreCanState().
*/
enum CpErrType_e
{

   /*! No error                        */
   eCP_ERR_TYPE_NONE   = 0,

   /*! Bit 0 error                     */
   eCP_ERR_TYPE_BIT0,

   /*! Bit 1 error                     */
   eCP_ERR_TYPE_BIT1,

   /*! Stuff error                     */
   eCP_ERR_TYPE_STUFF,

   /*! Format error                    */
   eCP_ERR_TYPE_FORM,

   /*! CRC error                       */
   eCP_ERR_TYPE_CRC,

   /*! Acknowledge error               */
   eCP_ERR_TYPE_ACK
};

/*----------------------------------------------------------------------------*/
/*!
** \enum    CpBuffer_e
** \brief   Buffer definition
**
** The enumeration CpBuffer_e is used to define a message buffer inside a
** FullCAN controller. The index for the first buffer starts at 0.
*/
enum CpBuffer_e
{
   /*! Buffer number 1              */
   eCP_BUFFER_1 = 0,

   /*! Buffer number 2              */
   eCP_BUFFER_2,

   /*! Buffer number 3 ..           */
   eCP_BUFFER_3,
   eCP_BUFFER_4,
   eCP_BUFFER_5,
   eCP_BUFFER_6,
   eCP_BUFFER_7,
   eCP_BUFFER_8,
   eCP_BUFFER_9,
   eCP_BUFFER_10,
   eCP_BUFFER_11,
   eCP_BUFFER_12,
   eCP_BUFFER_13,
   eCP_BUFFER_14,
   eCP_BUFFER_15,

   /*! Buffer number 16             */
   eCP_BUFFER_16
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CpBufferDir_e
** \brief   Buffer direction definition
**
** The enumeration CpBufferDir_e is used to define the direction of
** a message buffer (transmit or receive). The enumeration values are
** used by the function CpCoreBufferConfig().
**
*/
enum CpBufferDir_e
{

   /*! Message buffer direction is receive               */
   eCP_BUFFER_DIR_RCV = 0,

   /*! Message buffer direction is transmit              */
   eCP_BUFFER_DIR_TRM
};

/*----------------------------------------------------------------------------*\
** Structures                                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*!
** \struct  CpTime_s   canpie.h
** \brief   CAN time structure
**
*/
typedef struct CpTime_s
{
   uint32_t  ulSec1970;
   uint32_t  ulNanoSec;
} CpTime_ts;



/*----------------------------------------------------------------------------*/
/*!
** \struct  CpCanMsg_s
** \brief   CAN message structure
**
** For transmission and reception of CAN messages a structure which holds
** all necessary informations is used. The structure has the following
** data fields:
*/
typedef struct CpCanMsg_s
{

   /*!
   ** The identifier field may have 11 bits for standard frames
   ** (CAN specification 2.0A) or 29 bits for extended frames
   ** (CAN specification 2.0B). The three most significant bits
   ** are reserved (always read 0).
   ** \see CP_MASK
   */
   uint32_t  ulIdentifier;

   /*!
   ** The data field has up to 8 bytes (for classic CAN) or
   ** 64 bytes (for ISO CAN FD) of message data.
   ** The number of used bytes is described via the structure
   ** member \c ubMsgDLC.
   */
   union {
      /*!   byte access, array of bytes         */
      uint8_t   aubByte[CP_DATA_SIZE];

      /*!   16 bit access, array of words       */
      uint16_t  auwWord[CP_DATA_SIZE / 2];

      /*!   32 bit access, array of longs       */
      uint32_t  aulLong[CP_DATA_SIZE / 4];

      /*!   64 bit access, array of quadwords   */
      #if CPP_DATA_SIZE > 32
      uint64_t  auqQuad[CP_DATA_SIZE / 8];
      #endif
   } tuMsgData;

   /*!
   ** The data length code denotes the number of data bytes
   ** which are transmitted by a message.
   ** The possible value range for the data length code is
   ** from 0 to 8 for classic CAN and 0 to 15 for ISO CAN FD.
   */
   uint8_t  ubMsgDLC;


   /*!
   ** The structure member \c ubMsgCtrl defines the
   ** different data frames (2.0A / 2.0B) and the RTR frames.
   ** <ul>
   ** <li>Bit 0: Std. / Ext. Frame
   ** <li>Bit 1: ISO CAN FD: value of FDF bit
   ** <li>Bit 2: Remote Frame
   ** <li>Bit 3: Overload Frame
   ** <li>Bit 4: reserved, always 0
   ** <li>Bit 5: reserved, always 0
   ** <li>Bit 6: ISO CAN FD: value of BRS bit
   ** <li>Bit 7: ISO CAN FD: value of ESI bit
   ** </ul>
   ** \see CP_MSG_CTRL
   */
   uint8_t  ubMsgCtrl;


   #if CP_CAN_MSG_TIME == 1
   /*!
   ** The time stamp field defines the time when a CAN message
   ** was received by the CAN controller. This is an optional
   ** field (available if #CP_CAN_MSG_TIME is set to 1).
   */
   CpTime_ts tsMsgTime;
   #endif

   #if CP_CAN_MSG_USER == 1
   /*!
   ** The field user data can hold a 32 bit value, which is
   ** defined by the user. This is an optional field
   ** (available if #CP_CAN_MSG_USER is set to 1).
   */
   uint32_t  ulMsgUser;
   #endif

   #if CP_CAN_MSG_MARKER == 1
   /*!
   ** The field message marker can hold a 32 bit value, which is
   ** defined by the application. This is an optional field
   ** (available if #CP_CAN_MSG_MARKER is set to 1).
   */
   uint32_t  ulMsgMarker;
   #endif

} CpCanMsg_ts;



/*----------------------------------------------------------------------------*/
/*!
** \struct  CpHdi_s   canpie.h
** \brief   Hardware description interface
**
** The Hardware Description Interface provides a method to gather
** information about the CAN hardware and the functionality of the driver.
** The hardware description structure is available for every physical
** CAN channel.
*/
typedef struct CpHdi_s
{

   /*!
   ** Major version number of CANpie driver
   */
   uint8_t  ubVersionMajor;

   /*!
   ** Minor version number of CANpie driver
   */
   uint8_t  ubVersionMinor;

   /*!
   ** The element \a ubCanFeatures defines the capabilities of the CAN
   ** controller. Reserved bit values are read as 0.
   ** <ul>
   ** <li>Bit 0: 0 = 2.0A support, 1 = 2.0B support
   ** <li>Bit 1: 0 = Classic CAN , 1 = ISO CAN FD
   ** <li>Bit 2: reserved
   ** <li>Bit 3: reserved
   ** <li>Bit 4: reserved
   ** <li>Bit 5: reserved
   ** <li>Bit 6: reserved
   ** <li>Bit 7: reserved
   ** </ul>
   */
   uint8_t  ubCanFeatures;

   /*!
   ** The element \a ubDriverFeatures defines the capabilities of the
   ** software driver. Reserved bit values are read as 0.
   ** <ul>
   ** <li>Bit 0: 0 = no time stamp, 1 = time stamp support
   ** <li>Bit 1: 0 = no user data , 1 = user data support
   ** <li>Bit 2: reserved
   ** <li>Bit 3: reserved
   ** <li>Bit 4: reserved
   ** <li>Bit 5: reserved
   ** <li>Bit 6: reserved
   ** <li>Bit 7: reserved
   ** </ul>
   */
   uint8_t  ubDriverFeatures;

   /*!
   ** The element \a ubBufferMax defines the total number of CAN
   ** message buffers (mailboxes).
   */
   uint8_t ubBufferMax;

   /*!
   ** reserved element (alignment)
   */
   uint8_t uwReserved[3];

   /*!
   ** Resolution of optional time stamp in nano-seconds.
   */
   uint32_t ulTimeStampRes;

   /*!
   ** Clock rate of CAN controller in Hertz.
   */
   uint32_t ulCanClock;

   /*!
   ** The element \a ulBitRateMin defines the lowest configurable bit-rate
   ** in bits-per-second (bps). The value is specified through the used CAN
   ** transceiver.
   */
   uint32_t ulBitRateMin;

   /*!
   ** The element \a ulBitRateMax defines the highest configurable bit-rate
   ** in bits-per-second (bps). The value is specified through the used
   ** CAN transceiver.
   */
   uint32_t ulBitRateMax;

   /*!
   ** The element \a slNomBitRate defines the actual configured bit-rate
   ** of the CAN controller in bits-per-second (bps). For ISO CAN FD
   ** the value defines the bit-rate of the arbitration phase.
   */
   int32_t  slNomBitRate;

   /*!
   ** The element slDatBitRate is only valid for ISO CAN FD controller.
   ** The value defines the actual configured bit-rate of the data phase
   ** in bits-per-second (bps).
   */
   int32_t  slDatBitRate;

} CpHdi_ts ;


/*----------------------------------------------------------------------------*/
/*!
** \struct  CpStatistic_s   canpie.h
** \brief   CAN statistic structure
**
*/
typedef struct CpStatistic_s
{

   /*!   Total number of received data & remote frames
   */
   uint32_t     ulRcvMsgCount;

   /*!   Total number of transmitted data & remote frames
   */
   uint32_t     ulTrmMsgCount;

   /*!   Total number of error frames
   */
   uint32_t     ulErrMsgCount;

} CpStatistic_ts;




/*----------------------------------------------------------------------------*/
/*!
** \struct  CpState_s   canpie.h
** \brief   CAN state structure
**
*/
typedef struct CpState_s
{

   /*!   CAN error state
   **    Value should correspond to values from #CpState_e
   */
   uint8_t     ubCanErrState;

   /*!   Last error type occurred
   **    Value should correspond to values from #CpErrType_e
   */
   uint8_t     ubCanErrType;

   /*!   receive error counter
   */
   uint8_t     ubCanRcvErrCnt;

   /*!   transmit error counter
   */
   uint8_t     ubCanTrmErrCnt;

} CpState_ts;







#endif   /* CANPIE_H_   */

