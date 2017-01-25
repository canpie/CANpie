//============================================================================//
// File:          canpie_namespace.hpp                                        //
// Description:   CANpie namespace                                            //
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
// ("LGPL") version 3 as distributed in the 'LICENSE' file.                   //
//                                                                            //
//============================================================================//


#ifndef CANPIE_NAMESPACE_HPP_
#define CANPIE_NAMESPACE_HPP_



/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


//-------------------------------------------------------------------
/*!
** CANpie namespace
**
*/
namespace CANpie
{

enum CAN_Bitrate_e
{

  /*!
  ** No bit-rate defined
  */
  eCAN_BITRATE_NONE = -1,

  /*!
  ** bit-rate 10 kBit/sec
  */
  eCAN_BITRATE_10K = 0,

  /*!
  ** bit-rate 20 kBit/sec
  */
  eCAN_BITRATE_20K,

  /*!
  ** bit-rate 50 kBit/sec
  */
  eCAN_BITRATE_50K,

  /*!
  ** bit-rate 100 kBit/sec
  */
  eCAN_BITRATE_100K,

  /*!
  ** bit-rate 125 kBit/sec
  */
  eCAN_BITRATE_125K,

  /*!
  ** bit-rate 250 kBit/sec
  */
  eCAN_BITRATE_250K,

  /*!
  ** bit-rate 500 kBit/sec
  */
  eCAN_BITRATE_500K,

  /*!
  ** bit-rate 800 kBit/sec
  */
  eCAN_BITRATE_800K,

  /*!
  ** bit-rate 1 MBit/sec
  */
  eCAN_BITRATE_1M,

  /*!
  ** Use automatic bit-rate detection
  */
  eCAN_BITRATE_AUTO
};

enum CAN_Channel_e
{

  /*! CAN interface 1              */
  eCAN_CHANNEL_1 = 0,

  /*! CAN interface 2              */
  eCAN_CHANNEL_2,

  /*! CAN interface 3              */
  eCAN_CHANNEL_3,

  /*! CAN interface 4              */
  eCAN_CHANNEL_4,

  /*! CAN interface 5              */
  eCAN_CHANNEL_5,

  /*! CAN interface 6              */
  eCAN_CHANNEL_6,

  /*! CAN interface 7              */
  eCAN_CHANNEL_7,

  /*! CAN interface 8              */
  eCAN_CHANNEL_8
};

enum CAN_State_e
{
  /*!
  ** CAN controller is in stopped mode
  */
  eCAN_STATE_STOPPED  = 0,


  /*!
  ** CAN controller is in Sleep mode
  */
  eCAN_STATE_SLEEPING,


  /*!
  ** CAN controller is error active
  */
  eCAN_STATE_BUS_ACTIVE,


  /*!
  ** CAN controller is active, warning level is reached
  */
  eCAN_STATE_BUS_WARN,

  /*!
  ** CAN controller is error passive
  */
  eCAN_STATE_BUS_PASSIVE,

  /*!
  ** CAN controller went into Bus Off
  */
  eCAN_STATE_BUS_OFF
};

enum CAN_Mode_e
{
  /*!   Set controller in Stop mode (no reception / transmission possible)
  */
  eCAN_MODE_STOP = 0,

  /*!   Set controller into normal operation
  */
  eCAN_MODE_START,

  /*!   Set controller into listen-only mode
  */
  eCAN_MODE_LISTEN_ONLY,

  /*!   Set controller into Sleep mode
  */
  eCAN_MODE_SLEEP
};
}

#endif // CANPIE_NAMESPACE_HPP_
