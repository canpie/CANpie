//============================================================================//
// File:          canpie.h                                                    //
// Description:   General CAN driver definitions and structures               //
// Author:        Uwe Koppe                                                   //
// e-mail:        koppe@microcontrol.net                                      //
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


//------------------------------------------------------------------------------
// SVN  $Date: 2016-03-24 16:33:54 +0100 (Thu, 24. Mar 2016) $
// SVN  $Rev: 7442 $ --- $Author: tiderko $
//------------------------------------------------------------------------------

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

#include "cp_arch.h"       // Architecture dependent definitions

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
** \def  CP_FILTER_MAX
** \ingroup CP_CONF
**
** This symbol defines the total number of used Software Filter
** in CANpie implementation
** - 0 = no filter functionality is available
** - n = n corresponds to usable filters
*/
#ifndef  CP_FILTER_MAX
#define  CP_FILTER_MAX             10
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



#define  CP_VERSION_MAJOR           2
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


#define  CP_ID_FLAG_EXTENDED 0x20000000

/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_EXT_BIT
** \ingroup CP_MASK
**
** Set the EXT bit (extended frame) in the \c ubMsgCtrl field of
** the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MASK_EXT_BIT   0x01


/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_RTR_BIT
** \ingroup CP_MASK
**
** Set the RTR bit (remote frame) in the ubMsgCtrl field of
** the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MASK_RTR_BIT   0x02


/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_OVR_BIT
** \ingroup CP_MASK
**
** Set the OVR bit (overrun) in the ubMsgCtrl field of
** the CpCanMsg_ts structure (CpCanMsg_s::ubMsgCtrl).
*/
#define  CP_MASK_OVR_BIT   0x04


/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_EN_BIT
**
** Set the EN bit (enable) in the ubMsgCtrl field of
** the CpCanMsg_ts structure.
*/
#define  CP_MASK_EN_BIT    0x08


/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_TXRQ_BIT
**
** Set the TXRQ bit (transmit request) in the ubMsgCtrl field of
** the CpCanMsg_ts structure.
*/
#define  CP_MASK_TXRQ_BIT   0x10


#define  CP_ACC_MASK_EXT   0x20000000
#define  CP_ACC_MASK_RTR   0x40000000

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

   /*!   Baudrate out of range (42dec / 2Ahex)
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
** \enum    CP_FIFO
** \brief   FIFO Buffer numbers
*/
enum CP_FIFO {
   CP_FIFO_RCV = 0,
   CP_FIFO_TRM
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_CALLBACK
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
**       return(CP_CALLBACK_PROCESSED)
**    }
**
**    // Put all other messages into the FIFO
**    return (CP_CALLBACK_PUSH_FIFO);
** }
** \endcode
** <br>
**
*/
enum CP_CALLBACK {

   /*!
   ** Message was processed by callback and is not inserted in the FIFO
   */
   CP_CALLBACK_PROCESSED = 0,

   /*!
   ** Message was processed by callback and is inserted in the FIFO
   */
   CP_CALLBACK_PUSH_FIFO
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_BAUD
** \brief   Fixed baudrates
**
** The values of the enumeration CP_BAUD are used as parameter for the
** function CpCoreBaudrate().
*/
enum CP_BAUD {
   /*!
   ** Baudrate 10 kBit/sec
   */
   CP_BAUD_10K = 0,

   /*!
   ** Baudrate 20 kBit/sec
   */
   CP_BAUD_20K,

   /*!
   ** Baudrate 50 kBit/sec
   */
   CP_BAUD_50K,

   /*!
   ** Baudrate 100 kBit/sec
   */
   CP_BAUD_100K,

   /*!
   ** Baudrate 125 kBit/sec
   */
   CP_BAUD_125K,

   /*!
   ** Baudrate 250 kBit/sec
   */
   CP_BAUD_250K,

   /*!
   ** Baudrate 500 kBit/sec
   */
   CP_BAUD_500K,

   /*!
   ** Baudrate 800 kBit/sec
   */
   CP_BAUD_800K,

   /*!
   ** Baudrate 1 MBit/sec
   */
   CP_BAUD_1M,

   /*!
   ** Use automatic baudrate detection
   */
   CP_BAUD_AUTO,

   CP_BAUD_MAX = CP_BAUD_AUTO
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_CHANNEL
** \brief   Channel definition
**
** The physical CAN interfaces are numbered from 0 .. N-1 (N: total
** number of physical CAN interfaces on the target system). The enumeration
** CP_CHANNEL lists up to 8 physical interfaces.
*/
enum CP_CHANNEL {

   /*! CAN interface 1              */
   CP_CHANNEL_1 = 0,

   /*! CAN interface 2              */
   CP_CHANNEL_2,

   /*! CAN interface 3              */
   CP_CHANNEL_3,

   /*! CAN interface 4              */
   CP_CHANNEL_4,

   /*! CAN interface 5              */
   CP_CHANNEL_5,

   /*! CAN interface 6              */
   CP_CHANNEL_6,

   /*! CAN interface 7              */
   CP_CHANNEL_7,

   /*! CAN interface 8              */
   CP_CHANNEL_8
};




/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_MODE
** \brief   Mode of CAN controller
**
** These values are used as parameter for the function CpCoreCanMode() in
** order to change the state of the CAN controller.
*/
enum CP_MODE {
   /*!   Set controller in Stop mode (no reception / transmission possible)
   */
   CP_MODE_STOP = 0,

   /*!   Set controller into normal operation
   */
   CP_MODE_START,

   /*!   Set controller into listen-only mode
   */
   CP_MODE_LISTEN_ONLY,

   /*!   Set controller into Sleep mode
   */
   CP_MODE_SLEEP
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_STATE
** \brief   State of CAN controller
**
** These values are used as return value for the function CpCoreCanState().
*/
enum CP_STATE {
   /*!
   ** CAN controller is in stopped mode
   */
   CP_STATE_STOPPED  = 0,


   /*!
   ** CAN controller is in Sleep mode
   */
   CP_STATE_SLEEPING,


   /*!
   ** CAN controller is error active
   */
   CP_STATE_BUS_ACTIVE,


   /*!
   ** CAN controller is active, warning level is reached
   */
   CP_STATE_BUS_WARN,

   /*!
   ** CAN controller is error passive
   */
   CP_STATE_BUS_PASSIVE,

   /*!
   ** CAN controller went into Bus Off
   */
   CP_STATE_BUS_OFF,

   /*!
   ** General failure of physical layer detected (if supported by hardware)
   */
   CP_STATE_PHY_FAULT = 10,

   /*!
   ** Fault on CAN-H detected (Low Speed CAN)
   */
   CP_STATE_PHY_H,

   /*!
   ** Fault on CAN-L detected (Low Speed CAN)
   */
   CP_STATE_PHY_L

};

enum CP_ERR_TYPE {
   CP_ERR_TYPE_NONE   = 0,
   CP_ERR_TYPE_BIT0,
   CP_ERR_TYPE_BIT1,
   CP_ERR_TYPE_STUFF,
   CP_ERR_TYPE_FORM,
   CP_ERR_TYPE_CRC,
   CP_ERR_TYPE_ACK
};

/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_BUFFER
** \brief   Buffer definition
**
** The enumeration CP_BUFFER is used to define a message buffer inside a
** FullCAN controller. The index for the first buffer starts at 1.
*/
enum CP_BUFFER {
   CP_BUFFER_1 = 1,
   CP_BUFFER_2,
   CP_BUFFER_3,
   CP_BUFFER_4,
   CP_BUFFER_5,
   CP_BUFFER_6,
   CP_BUFFER_7,
   CP_BUFFER_8,
   CP_BUFFER_9,
   CP_BUFFER_10,
   CP_BUFFER_11,
   CP_BUFFER_12,
   CP_BUFFER_13,
   CP_BUFFER_14,
   CP_BUFFER_15
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_BUFFER_DIR
** \brief   Buffer direction definition
*/
enum CP_BUFFER_DIR {
   /*!
   **    Buffer direction is receive
   */
   CP_BUFFER_DIR_RX = 0,

   /*!
   **    Buffer direction is transmit
   */
   CP_BUFFER_DIR_TX
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

/*!
** \typedef    CpTime_ts
** \brief      CAN time
**
*/
typedef struct CpTime_s  CpTime_ts;
/*!
** \typedef    _TsCpTime
** \deprecated Data type was renamed
** \see        CpTime_ts
*/
typedef struct CpTime_s  _TsCpTime;


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
      /*!   byte access, array of 8 byte     */
      uint8_t  aubByte[8];

      /*!   16 bit access, array of 4 words  */
      uint16_t  auwWord[4];

      /*!   32 bit access, array of 2 longs  */
      uint32_t  aulLong[2];
   } tuMsgData;


   /*!
   ** The data length code denotes the number of data bytes
   ** which are transmitted by a message.
   ** The possible value range for the data length code is
   ** from 0 to 8 (bytes).
   */
   uint8_t  ubMsgDLC;


   /*!
   ** The structure member \c ubMsgCtrl defines the
   ** different data frames (2.0A / 2.0B) and the RTR frames.
   ** <ul>
   ** <li>Bit 0: Extended Frame if set to 1, else Standard Frame
   ** <li>Bit 1: Remote Frame if set to 1, else Data Frame
   ** <li>Bit 2: Receiver Overrun if set to 1, else normal reception
   ** <li>Bit 3: Message enable
   ** <li>Bit 4: Transmit Request
   ** <li>Bit 5: reserved, always 0
   ** <li>Bit 6: reserved, always 0
   ** <li>Bit 7: reserved, always 0
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
/*!
** \typedef    _TsCpCanMsg
** \deprecated Data type was renamed
** \see        CpCanMsg_ts
*/
typedef struct CpCanMsg_s  _TsCpCanMsg;

/*----------------------------------------------------------------------------*/
/*!
** \struct  CpStruct_HDI   canpie.h
** \brief   Hardware description interface
**
** The Hardware Description Interface provides a method to gather
** information about the CAN hardware and the functionality of the driver.
** All items in the structure CpStruct_HDI are constant and must be
** supplied by the designer of the CAN driver. The hardware description
** structure is available for every physical CAN channel.
*/
struct CpHdi_s {

  uint16_t uwVersionNumber;

   /*!   Bit coded value that describes the features of the CAN driver.
   **    <ul>
   **    <li>Bit 0/1: 0 = Standard Frame, 1 = Extended Frame passive,
   **        2 = Extended Frame active
   **    <li>Bit 2: 0 = BasicCAN, 1 = FullCAN
   **    <li>Bit 3: 0 = No IRQ Handler, 1 = Has IRQ Handler
   **    <li>Bit 4: 0 = No identifier filter, 1 = software identifier filter
   **    <li>Bit 5: 0 = No timestamp, 1 = has timestamp
   **    <li>Bit 6: 0 = No user data field, 1 = has user data field
   **    <li>Bit 7: reserved
   **    </ul>
   */
  uint16_t uwSupportFlags;

   /*!   Constant value that identifies the used CAN controller
   **    chip. Possible values for this member are listed
   **    in the header file cp_cc.h
   */
  uint16_t uwControllerType;

   /*!   Defines the number of the interrupt which is used.
   **    If the flag IRQHandler is set to 0, the value of
   **    IRQNumber will be undefined.
   */
  uint16_t uwIRQNumber;


  uint16_t uwBufferMax;
  uint16_t uwRes;

  uint32_t ulTimeStampRes;
  uint32_t ulCanClock;
  uint32_t ulBitrate;


};


/*----------------------------------------------------------------------------*/
/*
** The structure CpHdi_s provides fields to gather information about
** the CAN hardware. This typedef makes it compatible to older versions
** of CANpie. Please use CpHdi_ts for new applications.
*/
typedef struct CpHdi_s  CpStruct_HDI;


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

/*!
** \typedef    _TsCpHdi
** \deprecated Data type was renamed
** \see        CpHdi_ts
*/
typedef struct CpHdi_s  _TsCpHdi;



/*----------------------------------------------------------------------------*/
/*!
** \struct  CpBitTiming_s   canpie.h
** \brief   Bit timing structure
**
*/
struct CpBitTiming_s {

   /*!   Bit timing register 0
   */
   uint8_t     ubBtr0;

   /*!   Bit timing register 1
   */
   uint8_t     ubBtr1;

   /*!   Synchronisation jump width
   */
   uint8_t     ubSjw;


   /*!   Baudrate prescaler
   */
   uint8_t     ubBrp;
};

/*!
** \typedef CpBitTiming_ts
** \brief   Bit timing
**
*/
typedef struct CpBitTiming_s  CpBitTiming_ts;

/*!
** \typedef    _TsCpBitTiming
** \deprecated Data type was renamed
** \see        CpBitTiming_ts
*/
typedef struct CpBitTiming_s  _TsCpBitTiming;



/*----------------------------------------------------------------------------*/
/*!
** \struct  CpStatistic_s   canpie.h
** \brief   CAN statistic structure
**
*/
struct CpStatistic_s {

   /*!   Total number of received data & remote frames
   */
   uint32_t     ulRcvMsgCount;

   /*!   Total number of transmitted data & remote frames
   */
   uint32_t     ulTrmMsgCount;

   /*!   Total number of error frames
   */
   uint32_t     ulErrMsgCount;

};

typedef struct CpStatistic_s CpStatistic_ts;  // new style to be C99 conform
typedef struct CpStatistic_s _TsCpStatistic; // old style to be compatible



/*----------------------------------------------------------------------------*/
/*!
** \struct  CpState_s   canpie.h
** \brief   CAN state structure
**
*/
struct CpState_s {

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

};

/*!
** \typedef    CpState_ts
** \brief      CAN state
*/
typedef struct CpState_s CpState_ts;

/*!
** \typedef    _TsCpState
** \deprecated Data type was renamed
** \see        CpState_ts
*/
typedef struct CpState_s _TsCpState;



#endif   /* CANPIE_H_   */

