//============================================================================//
// File:          canpie.h                                                    //
// Description:   General CAN driver definitions and structures               //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53844 Troisdorf - Germany                                                  //
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
//----------------------------------------------------------------------------//
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 04.12.1998  Initial version                                                //
// 29.04.1999  Changed structures, new data type definitions                  //
// 15.06.2000  moved definitions from cpmsg.h                                 //
// 06.11.2000  added new error codes, added missing buffer number             //
// 12.06.2007  update HDI and statistic structure                             //
//                                                                            //
//============================================================================//


#ifndef  CANPIE_H_
#define  CANPIE_H_


//-----------------------------------------------------------------------------
/*!
** \file    canpie.h
** \brief   CANpie constants, structures and enumerations
**
** This file holds constants and structures used within CANpie.
** All functions, structures, defines and constants in CANpie have
** the prefix "Cp". The following table shows the used nomenclature:
**
** <table border=0>
** <tr class="memlist">
** <td width=200 class="memItemLeft"> <b>CANpie code</b></td>
** <td width=200 class="memItemRight"><b>Prefix</b></td>
** </tr>
** <tr>
** <td width=200 class="memItemLeft">Core functions</td>
** <td width=200 class="memItemRight">CpCore</td>
** </tr>
** <tr>
** <td width=200 class="memItemLeft">Message access functions</td>
** <td width=200 class="memItemRight">CpMsg</td>
** </tr>
** <tr>
** <td width=200 class="memItemLeft">Structures</td>
** <td width=200 class="memItemRight">CpName_ts</td>
** </tr>
** <tr>
** <td width=200 class="memItemLeft">Constants / Defines</td>
** <td width=200 class="memItemRight">CP</td>
** </tr>
** <tr>
** <td width=200 class="memItemLeft">Error Codes</td>
** <td width=200 class="memItemRight">CpErr</td>
** </tr>
** </table>
*/


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "compiler.h"
#include "cp_platform.h"       // Architecture dependent definitions

/*@ -isoreserved                                                             @*/

/*----------------------------------------------------------------------------*\
** Definitions & Enumerations                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
/*!
** \defgroup CP_CONF  CANpie configuration options
**
** The CANpie driver can be configured during compile time via
** several configuration options. They are typically defined in
** the \c cp_arch.h architecture file. The symbol #CP_TARGET is used
** to select an existing definition scheme from this file.
** <p>
** If symbols are not defined, they get a default value which is
** assigned in the \c canpie.h header file.
*/

/*-------------------------------------------------------------------*/
/*!
** \def  CP_AUTOBAUD
** \ingroup CP_CONF
**
** This symbol enables or disables the autobaud feature of the
** CAN driver (CpCoreAutobaud()).
** - 0 = disable autobaud feature (not supported by hardware)
** - 1 = enable autobaud feature
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
** it is also possible to emulate these. A value of 0 denotes that
** there are no message buffers available. This also means all buffer
** functions (e.g. CpCoreBufferInit(), etc.) return the error code
** #CpErr_NOT_SUPPORTED.
*/
#ifndef  CP_BUFFER_MAX
#define  CP_BUFFER_MAX              0
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
** \def  CP_CAN_MSG_TIME
** \ingroup CP_CONF
**
** This symbol defines if the CAN message structure CpCanMsg_s
** has a timestamp field.
** - 0 = no timestamp field (not supported by hardware / driver)
** - 1 = include timestamp field
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
** error code #CpErr_NOT_SUPPORTED.
** - 0 = no statistic information (not supported by driver)
** - 1 = enable statistic information
*/
#ifndef  CP_STATISTIC
#define  CP_STATISTIC               0
#endif



#define  CP_VERSION_MAJOR           3
#define  CP_VERSION_MINOR           0


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
** the CpCanMsg_s::ulExt.
*/
#define  CP_MASK_STD_FRAME 0x000007FF


/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_EXT_FRAME
** \ingroup CP_MASK
**
** Mask for extended frame (29 bits), used in combination with
** the CpCanMsg_s::ulExt.
*/
#define  CP_MASK_EXT_FRAME 0x1FFFFFFF


/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_CTRL_EXT_BIT
** \ingroup CP_MSG_CTRL
**
** Bit mask for the EXT bit (extended frame) in the \c ubMsgCtrl 
** field of the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MASK_EXT_BIT      0x01


/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_CTRL_RTR_BIT
** \ingroup CP_MSG_CTRL
**
** Bit mask for the RTR bit (remote frame) in the \c ubMsgCtrl 
** field of the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MSG_CTRL_RTR_BIT     0x04


/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_CTRL_OVR_BIT
** \ingroup CP_MSG_CTRL
**
** Bit mask for the OVR bit (overrun) in the \c ubMsgCtrl 
** field of the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MSG_CTRL_OVR_BIT     0x08


/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_CTRL_EDL_BIT
** \ingroup CP_MSG_CTRL
**
** Bit mask for the EDL bit (ISO CAN FD, extended data length) in the 
** \c ubMsgCtrl field of the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MSG_CTRL_EDL_BIT     0x40

/*-------------------------------------------------------------------*/
/*!
** \def     CP_MSG_CTRL_ESI_BIT
** \ingroup CP_MSG_CTRL
**
** Bit mask for the ESI bit (ISO CAN FD, error state indicator) in the 
** \c ubMsgCtrl field of the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MSG_CTRL_ESI_BIT     0x80



//-----------------------------------------------------------------------------
/*!
** \defgroup CP_ACC_MASK Mask values for acceptance filter
**
** The following definitions are used in combination with the
** function CpCoreBufferAccMask()
*/

/*-------------------------------------------------------------------*/
/*!
** \def     CP_ACC_MASK_EXT
** \ingroup CP_ACC_MASK
** 
** Bit mask for extended frame format.
*/
#define  CP_ACC_MASK_EXT      0x20000000

/*-------------------------------------------------------------------*/
/*!
** \def     CP_ACC_MASK_RTR
** \ingroup CP_ACC_MASK
** 
** Bit mask for remote frame format.
*/
#define  CP_ACC_MASK_RTR      0x40000000


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
#define  CP_DATA_SIZE      8
#else
#define  CP_DATA_SIZE      64
#endif


/*----------------------------------------------------------------------------*/
/*!
** \enum    CpErr
** \brief   CANpie Error codes
**
** All functions that may cause an error condition will return an
** error code. The CANpie error codes are within the value range from
** 0 to 127. The designer of the core functions might extend the error code
** table with hardware specific error codes, which must be in the range
** from 128 to 255.
*/
enum CpErr {

   /*!   No error (00dec / 00hex)
   */
   CpErr_OK = 0,

   /*!   Error not specified (01dec / 01hex)
   */
   CpErr_GENERIC,

   /*!   Hardware failure (02dec / 02hex)
   */
   CpErr_HARDWARE,

   /*!   Initialisation failure (03dec / 03hex)
   */
   CpErr_INIT_FAIL,

   /*!   Channel is initialised, ready to run (04dec / 04hex)
   */
   CpErr_INIT_READY,

   /*!    CAN channel was not initialised (05dec / 05hex)
   */
   CpErr_INIT_MISSING,

   /*!   Receive buffer is empty (05dec / 05hex)
   */
   CpErr_RCV_EMPTY,

   /*!   Receive buffer overrun (06dec / 06hex)
   */
   CpErr_OVERRUN,

   /*!   Transmit buffer is full (07dec / 07hex)
   */
   CpErr_TRM_FULL,

   /*!   CAN message has wrong format (10dec / 0Ahex)
   */
   CpErr_CAN_MESSAGE = 10,

   /*!   CAN identifier not valid (11dec / 0Bhex)
   */
   CpErr_CAN_ID,

   /*!   CAN data length code not valid (12dec / 0Chex)
   */
   CpErr_CAN_DLC,

   /*!   FIFO is empty (20dec / 14hex)
   */
   CpErr_FIFO_EMPTY = 20,

   /*!   Message is waiting in FIFO (21dec / 15hex)
   */
   CpErr_FIFO_WAIT,

   /*!   FIFO is full (22dec / 16hex)
   */
   CpErr_FIFO_FULL,

   /*!   FIFO size is out of range (23dec / 17hex)
   */
   CpErr_FIFO_SIZE,

   /*!   Parameter of FIFO function is out of range (24dec / 18hex)
   */
   CpErr_FIFO_PARM,

   /*!   Controller is in error passive (30dec / 1Ehex)
   */
   CpErr_BUS_PASSIVE = 30,

   /*!   Controller is in bus off (31dec / 1Fhex)
   */
   CpErr_BUS_OFF,

   /*!   Controller is in warning status (32dec / 20hex)
   */
   CpErr_BUS_WARNING,


   /*!   Channel out of range (40dec / 28hex)
   */
   CpErr_CHANNEL = 40,

   /*!   Register address out of range (41dec / 29hex)
   */
   CpErr_REGISTER,

   /*!   Bitrate out of range (42dec / 2Ahex)
   */
   CpErr_BAUDRATE,

   /*!   Buffer number out of range (43dec / 2Bhex)
   */
   CpErr_BUFFER,

   /*!   Parameter number out of range (44dec / 2Chex)
   */
   CpErr_PARAM,

   /*!   Function is not supported (50dec / 32hex)
   */
   CpErr_NOT_SUPPORTED = 50
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CpFifo_e
** \brief   FIFO Buffer numbers
*/
enum CpFifo_e {
   eCP_FIFO_RCV = 0,
   eCP_FIFO_TRM
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
**    if( CpMsgGetStdId(ptsCanMsgV) < 100)
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
enum CpCallback_e {

   /*!
   ** Message was processed by callback and is not inserted in the FIFO
   */
   eCP_CALLBACK_PROCESSED = 0,

   /*!
   ** Message was processed by callback and is inserted in the FIFO
   */
   eCP_CALLBACK_PUSH_FIFO
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CpBitrate_e
** \brief   Fixed bitrates
**
** The values of the enumeration CpBitrate_e are used as parameter for the
** function CpCoreBitrate().
*/
enum CpBitrate_e {
   /*!
   ** Bitrate 10 kBit/sec
   */
   eCP_BITRATE_10K = 0,

   /*!
   ** Bitrate 20 kBit/sec
   */
   eCP_BITRATE_20K,

   /*!
   ** Bitrate 50 kBit/sec
   */
   eCP_BITRATE_50K,

   /*!
   ** Bitrate 100 kBit/sec
   */
   eCP_BITRATE_100K,

   /*!
   ** Bitrate 125 kBit/sec
   */
   eCP_BITRATE_125K,

   /*!
   ** Bitrate 250 kBit/sec
   */
   eCP_BITRATE_250K,

   /*!
   ** Bitrate 500 kBit/sec
   */
   eCP_BITRATE_500K,

   /*!
   ** Bitrate 800 kBit/sec
   */
   eCP_BITRATE_800K,

   /*!
   ** Bitrate 1 MBit/sec
   */
   eCP_BITRATE_1M,

   /*!
   ** Use automatic Bitrate detection
   */
   eCP_BITRATE_AUTO,

   eCP_BITRATE_MAX = eCP_BITRATE_AUTO
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CpChannel_e
** \brief   Channel definition
**
** The physical CAN interfaces are numbered from 0 .. N-1 (N: total
** number of physical CAN interfaces on the target system). The enumeration
** CpChannel_e lists up to 8 physical interfaces.
*/
enum CpChannel_e {

   /*! CAN interface 1              */
   eCP_CHANNEL_1 = 0,

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
enum CpMode_e {
   /*!   Set controller in Stop mode (no reception / transmission possible)
   */
   eCP_MODE_STOP = 0,

   /*!   Set controller into normal operation
   */
   eCP_MODE_START,

   /*!   Set controller into listen-only mode
   */
   eCP_MODE_LISTEN_ONLY,

   /*!   Set controller into Sleep mode
   */
   eCP_MODE_SLEEP
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CpState_e
** \brief   State of CAN controller
**
** These values are used as return value for the function CpCoreCanState().
*/
enum CpState_e {
   /*!
   ** CAN controller is in stopped mode
   */
   eCP_STATE_STOPPED  = 0,


   /*!
   ** CAN controller is in Sleep mode
   */
   eCP_STATE_SLEEPING,


   /*!
   ** CAN controller is error active
   */
   eCP_STATE_BUS_ACTIVE,


   /*!
   ** CAN controller is active, warning level is reached
   */
   eCP_STATE_BUS_WARN,

   /*!
   ** CAN controller is error passive
   */
   eCP_STATE_BUS_PASSIVE,

   /*!
   ** CAN controller went into Bus Off
   */
   eCP_STATE_BUS_OFF,

   /*!
   ** General failure of physical layer detected (if supported by hardware)
   */
   eCP_STATE_PHY_FAULT = 10,

   /*!
   ** Fault on CAN-H detected (Low Speed CAN)
   */
   eCP_STATE_PHY_H,

   /*!
   ** Fault on CAN-L detected (Low Speed CAN)
   */
   eCP_STATE_PHY_L

};

enum CpErrType_e {
   eCP_ERR_TYPE_NONE   = 0,
   eCP_ERR_TYPE_BIT0,
   eCP_ERR_TYPE_BIT1,
   eCP_ERR_TYPE_STUFF,
   eCP_ERR_TYPE_FORM,
   eCP_ERR_TYPE_CRC,
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
enum CpBuffer_e {
   eCP_BUFFER_1 = 1,
   eCP_BUFFER_2,
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
   eCP_BUFFER_16
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CpBufferDir_e
** \brief   Buffer direction definition
*/
enum CpBufferDir_e {
   /*!
   **    Buffer direction is receive
   */
   eCP_BUFFER_DIR_RX = 0,

   /*!
   **    Buffer direction is transmit
   */
   eCP_BUFFER_DIR_TX
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
struct CpTime_s {
   uint32_t  ulSec1970;
   uint32_t  ulNanoSec;
};

//-------------------------------------------------------------------
/*!
** \typedef    CpTime_ts
** \brief      CAN time
**
*/
typedef struct CpTime_s  CpTime_ts;



/*----------------------------------------------------------------------------*/
/*!
** \struct  CpCanMsg_s   canpie.h
** \brief   CAN message structure
**
** For transmission and reception of CAN messages a structure which holds
** all necessary informations is used. The structure has the following
** data fields:
*/
struct CpCanMsg_s {

   /*!
   ** The identifier field may have 11 bits for standard frames
   ** (CAN specification 2.0A) or 29 bits for extended frames
   ** (CAN specification 2.0B). The three most significant bits
   ** are reserved (always read 0).
   ** \see CP_MASK
   */
   union {
      /*! identifier for Standard Frame      */
      uint16_t  uwStd;
      /*! identifier for Extended Frame      */
      uint32_t  ulExt;
   } tuMsgId;

   /*!
   ** The data field has up to 8 bytes (64 bit) of message data.
   ** The number of used bytes is described via the structure
   ** member \c ubMsgDLC.
   */
   union {
      /*!   byte access                      */
      uint8_t  aubByte[CP_DATA_SIZE];

      /*!   16 bit access                    */
      uint16_t  auwWord[CP_DATA_SIZE / 2];

      /*!   32 bit access                    */
      uint32_t  aulLong[CP_DATA_SIZE / 4];
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
   ** <li>Bit 0: Frame Format 0
   ** <li>Bit 1: Frame Format 1
   ** <li>Bit 2: Remote Frame
   ** <li>Bit 3: Overload Frame
   ** <li>Bit 4: reserved, always 0
   ** <li>Bit 5: reserved, always 0
   ** <li>Bit 6: ISO CAN FD: value of EDL bit
   ** <li>Bit 7: ISO CAN FD: value of ESI bit
   ** </ul>
   ** \see CP_MASK
   */
   uint8_t  ubMsgCtrl;


#if CP_CAN_MSG_TIME == 1
   /*!   The time stamp field defines the time when a CAN message
   **    was received by the CAN controller. This is an optional
   **    field (available if #CP_CAN_MSG_TIME is set to 1).
   */
   CpTime_ts tsMsgTime;
#endif

#if CP_CAN_MSG_USER == 1

   /*!   The field user data can hold a 32 bit value, which is
   **    defined by the user. This is an optional field
   **    (available if #CP_CAN_MSG_USER is set to 1).
   */
   uint32_t  ulMsgUser;
#endif

};



/*----------------------------------------------------------------------------*/
/*!
** \typedef CpCanMsg_ts
** \brief   CAN message structure
**
** For transmission and reception of CAN messages the structure CpCanMsg_s
** is used.
*/
typedef struct CpCanMsg_s  CpCanMsg_ts;


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
struct CpHdi_s {
   
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
   ** controller.
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
   ** software driver.
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
   ** The element \a ulBitrate defines the actual configured bit-rate 
   ** of the CAN controller in bits-per-second (bps). For ISO CAN FD 
   ** the value defines the bit-rate of the arbitration phase.
   */
   uint32_t ulBitrate;
   
   /*!
   ** The element ulBitrateFD is only valid for ISO CAN FD controller.
   ** The value defines the actual configured bit-rate of the data phase
   ** in bits-per-second (bps).
   */
   uint32_t ulBitrateFD;

};


/*----------------------------------------------------------------------------*/
/*!
** \typedef CpHdi_ts
** \brief   Hardware description interface structure
**
** The structure CpHdi_s provides fields to gather information about
** the CAN hardware.
**
*/
typedef struct CpHdi_s  CpHdi_ts;




/*----------------------------------------------------------------------------*/
/*!
** \struct  CpStatistic_s   canpie.h
** \brief   CAN statistic structure
**
*/
typedef struct CpStatistic_s {

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
typedef struct CpState_s {

   /*!   CAN error state 
   **    Value should correspond to values from #CP_STATE
   */
   uint8_t     ubCanErrState;

   /*!   Last error type occurred
   **    Value should correspond to values from #CP_ERR_TYPE 
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

