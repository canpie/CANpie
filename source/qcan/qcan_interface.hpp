//============================================================================//
// File:          qcan_interface.hpp                                          //
// Description:   Virtual CAN interface class                                 //
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




//-----------------------------------------------------------------------------
/*!
** \file    qcan_can_interface.hpp
** \brief   CAN interface class
**
*/


#ifndef QCAN_INTERFACE_HPP_
#define QCAN_INTERFACE_HPP_

#include <stdint.h>
#include "qcan_defs.hpp"
#include "qcan_frame.hpp"




//-----------------------------------------------------------------------------
/*!
** \class   QCanInterface
** \brief   CAN interface
** 
*/
class QCanInterface : public QObject
{

public:

    typedef struct QCanStatistic_s {
       uint32_t   ulRcvCount;
       uint32_t   ulTrmCount;
       uint32_t   ulErrCount;
    } QCanStatistic_ts;



   /*!
   ** \enum    InterfaceError_e
   **
   ** This enum describes possible error conditions.
   */
   enum InterfaceError_e {
      /*! An unknown error occurred.                  */
      eERROR_UNKNOWN = -1,

      /*! No error occurred.                          */
      eERROR_NONE,

      /*! Access to library / plugin failed           */
      eERROR_LIBRARY,

      eERROR_CHANNEL,

      /*! Bit-rate not supported                      */
      eERROR_BITRATE,

      eERROR_DEVICE,

      eERROR_MODE,

      /*! No message in receive FIFO available        */
      eERROR_FIFO_RCV_EMPTY,

      /*! Transmit FIFO is full                       */
      eERROR_FIFO_TRM_FULL
   };


   /*!
   ** \see channel()
   **
   ** Gives nbumber of available channels.
   */
   virtual uint8_t channel(void) = 0;

   /*!
   ** \see disconnect()
   **
   ** Connect to the physical channel \c ubChannelV of the CAN interface.
   ** The first CAN channel starts at index 0.
   */
   virtual InterfaceError_e connect(uint8_t ubChannelV = 0) = 0;

   /*!
   ** \see connect()
   **
   ** Disconnect from the physical channel \c ubChannelV of the CAN interface.
   ** The first CAN channel starts at index 0.
   */
   virtual InterfaceError_e disconnect(uint8_t ubChannelV = 0) = 0;


   /*!
   ** Get icon from the physical channel \c ubChannelV of the CAN interface.
   ** The first CAN channel starts at index 0.
   */
   virtual QIcon icon(uint8_t ubChannelV = 0) = 0;

   /*!
   ** Get name from the physical channel \c ubChannelV of the CAN interface.
   ** The first CAN channel starts at index 0.
   */
   virtual QString name(uint8_t ubChannelV = 0) = 0;

   /*!
   ** \see  write()
   **
   ** The functions reads a CAN message from the physical channel
   ** \c ubChannelV of the CAN interface. The first CAN channel starts at
   ** index 0.
   ** If no message is available, the function will return the value
   ** eERROR_FIFO_RCV_EMPTY. On success the function returns eERROR_NONE.
   **
   */
   virtual InterfaceError_e   read( QCanFrame &clFrameR, uint8_t ubChannelV = 0) = 0;

   /*!
   **	This function sets the bit-rate of the CAN interface. If the physical
   **	CAN interface does not support CAN FD, the parameter \c slBrsClockV
   **	is not evaluated.
   */
   virtual InterfaceError_e   setBitrate(int32_t slBitrateV,
                                         int32_t slBrsClockV = QCan::eCAN_BITRATE_NONE,
                                         uint8_t ubChannelV = 0) = 0;


   /*!
   ** Set mode of the physical channel \c ubChannelV.
   ** The first CAN channel starts at index 0.
   */
   virtual InterfaceError_e	setMode(const QCan::CAN_Mode_e teModeV,
                                      uint8_t ubChannelV = 0) = 0;


   /*!
   ** Get state of the physical channel \c ubChannelV.
   ** The first CAN channel starts at index 0.
   */
   virtual QCan::CAN_State_e	state(uint8_t ubChannelV = 0) = 0;


   virtual InterfaceError_e   statistic(QCanStatistic_ts &clStatisticR,
                                        uint8_t ubChannelV = 0) = 0;

   /*!
   ** The function returns the supported features of the physical
   ** channel \c ubChannelV. The first CAN channel starts at index 0.
   ** The return value is a bit-mask using values defined in the header
   ** file qcan_defs.hpp.
   **
   */
   virtual uint32_t           supportedFeatures(uint8_t ubChannelV = 0) = 0;
	

   /*!
   ** \see  read()
   **
   ** The functions writes a CAN message to the physical channel
   ** \c ubChannelV of the CAN interface. The first CAN channel starts at
   ** index 0. On success the function returns eERROR_NONE.
   **
   */
   virtual InterfaceError_e	write(const QCanFrame &clFrameR,
                                    uint8_t ubChannelV = 0) = 0;



Q_SIGNALS:
    void errorOccurred(int32_t slCanBusErrorV, uint8_t ubChannelV = 0);     //  QCanBusDevice::CanBusError
    void framesReceived(uint32_t ulFramesCountV, uint8_t ubChannelV = 0);
    void framesWritten(uint32_t ulFramesCountV, uint8_t ubChannelV = 0);
    void stateChanged(int32_t slCanBusDevStatusV, uint8_t ubChannelV = 0);

private:


};

#define QCanInterface_iid "net.microcontrol.Qt.qcan.QCanInterface"

Q_DECLARE_INTERFACE(QCanInterface, QCanInterface_iid)

#endif /*QCAN_INTERFACE_HPP_*/
