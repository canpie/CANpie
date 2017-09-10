//============================================================================//
// File:          qcan_namespace.hpp                                          //
// Description:   QCan namespace                                              //
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


#ifndef QCAN_NAMESPACE_HPP_
#define QCAN_NAMESPACE_HPP_



/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


//-------------------------------------------------------------------
/*!
** QCan namespace
**
*/
namespace QCan {

   /*!
   ** \enum CAN_Bitrate_e
   ** 
   ** The enumeration CAN_Bitrate_e defines commonly used
   ** bit-rates. The pre-defined value range is limited to
   ** a value of eCAN_BITRATE_MAX. 
   */
   enum CAN_Bitrate_e {

      /*!
      ** No bit-rate defined
      */
      eCAN_BITRATE_NONE = -1,//!< eCAN_BITRATE_NONE

      /*!
      ** bit-rate 10 kBit/sec
      */
      eCAN_BITRATE_10K = 0,  //!< eCAN_BITRATE_10K

      /*!
      ** bit-rate 20 kBit/sec
      */
      eCAN_BITRATE_20K = 1,  //!< eCAN_BITRATE_20K

      /*!
      ** bit-rate 50 kBit/sec
      */
      eCAN_BITRATE_50K = 2,  //!< eCAN_BITRATE_50K

      /*!
      ** bit-rate 100 kBit/sec
      */
      eCAN_BITRATE_100K = 3, //!< eCAN_BITRATE_100K

      /*!
      ** bit-rate 125 kBit/sec
      */
      eCAN_BITRATE_125K = 4, //!< eCAN_BITRATE_125K

      /*!
      ** bit-rate 250 kBit/sec
      */
      eCAN_BITRATE_250K = 5, //!< eCAN_BITRATE_250K

      /*!
      ** bit-rate 500 kBit/sec
      */
      eCAN_BITRATE_500K = 6, //!< eCAN_BITRATE_500K

      /*!
      ** bit-rate 800 kBit/sec
      */
      eCAN_BITRATE_800K = 7, //!< eCAN_BITRATE_800K

      /*!
      ** bit-rate 1 MBit/sec
      */
      eCAN_BITRATE_1M = 8,   //!< eCAN_BITRATE_1M

      /*!
      ** bit-rate 2 MBit/s (ISO CAN FD, data phase)
      */
      eCAN_BITRATE_2M = 10,  //!< eCAN_BITRATE_2M

      /*!
      ** bit-rate 4 MBit/s (ISO CAN FD, data phase)
      */
      eCAN_BITRATE_4M = 11,  //!< eCAN_BITRATE_4M

      /*!
      ** bit-rate 5 MBit/s (ISO CAN FD, data phase)
      */
      eCAN_BITRATE_5M = 12,   //!< eCAN_BITRATE_5M

      /*!
      ** Use automatic bit-rate detection
      */
      eCAN_BITRATE_AUTO = 9,  //!< eCAN_BITRATE_AUTO
      
      /*!
      ** Pre-defined bit-rate maximum
      */
      eCAN_BITRATE_MAX = 9999 //!< eCAN_BITRATE_MAX
   };

   
   /*!
   ** \enum CAN_Channel_e
   ** 
   ** The enumeration CAN_Channel_e defines up to 8 CAN channels.
   */
   enum CAN_Channel_e {

      /*! CAN interface invalid        */
      eCAN_CHANNEL_NONE = 0,

      /*! CAN interface 1              */
      eCAN_CHANNEL_1,

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

   /*!
   ** \enum CAN_State_e
   ** 
   ** The enumeration CAN_State_e defines possible states of the CAN
   ** protocol engine.
   */
   enum CAN_State_e {
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

   /*!
   ** \enum CAN_Mode_e
   ** 
   ** The enumeration CAN_Mode_e defines possible modes of the CAN
   ** protocol engine.
   */
   enum CAN_Mode_e {

      /*!   Set controller into 'Initialization' mode
      */
      eCAN_MODE_INIT = 0,

      eCAN_MODE_STOP = 0,     // deprecated definition


      /*!   Set controller into Operation' mode
      */
      eCAN_MODE_OPERATION = 1,

      eCAN_MODE_START = 1,    // deprecated definition

      /*!   Set controller into 'Listen-only' mode
      */
      eCAN_MODE_LISTEN_ONLY = 2,

      /*!   Set controller into 'Sleep' (power-down) mode
      */
      eCAN_MODE_SLEEP = 3,

      /*!   Set controller into 'Self-test' mode
      */
      eCAN_MODE_SELF_TEST = 4
   };

   enum LogLevel_e {
      eLOG_LEVEL_INFO = 0,
      eLOG_LEVEL_NOTICE,
      eLOG_LEVEL_WARN,
      eLOG_LEVEL_ERROR,
      eLOG_LEVEL_DEBUG
   };
}

#endif // QCAN_NAMESPACE_HPP_
