#ifndef QCAN_DEFS_HPP_
#define QCAN_DEFS_HPP_


#define  QCAN_SOCKET_RCV_PORT       5566
#define  QCAN_SOCKET_TRM_PORT       5567

namespace QCan {

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
