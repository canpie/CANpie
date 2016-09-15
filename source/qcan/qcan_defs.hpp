#ifndef QCAN_DEFS_HPP_
#define QCAN_DEFS_HPP_


#define  QCAN_TCP_DEFAULT_PORT      55660

#define  QCAN_TCP_SOCKET_MAX       16

#define  QCAN_NETWORK_MAX              8


namespace QCan {

   enum CAN_Bitrate_e {
      /*!
       ** Bitrate 10 kBit/sec
       */
      eCAN_BITRATE_10K = 0,

      /*!
       ** Bitrate 20 kBit/sec
       */
      eCAN_BITRATE_20K,

      /*!
       ** Bitrate 50 kBit/sec
       */
      eCAN_BITRATE_50K,

      /*!
       ** Bitrate 100 kBit/sec
       */
      eCAN_BITRATE_100K,

      /*!
       ** Bitrate 125 kBit/sec
       */
      eCAN_BITRATE_125K,

      /*!
       ** Bitrate 250 kBit/sec
       */
      eCAN_BITRATE_250K,

      /*!
       ** Bitrate 500 kBit/sec
       */
      eCAN_BITRATE_500K,

      /*!
       ** Bitrate 800 kBit/sec
       */
      eCAN_BITRATE_800K,

      /*!
       ** Bitrate 1 MBit/sec
       */
      eCAN_BITRATE_1M,

      /*!
       ** Use automatic Bitrate detection
       */
      eCAN_BITRATE_AUTO
   };

   enum CAN_Channel_e {

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

   enum CAN_Mode_e {
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

#endif // QCAN_DEFS_HPP_
