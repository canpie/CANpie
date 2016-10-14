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


using namespace CANpie;

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

      /*! Interface is already used, and can't
       *  be occupied                                 */
      eERROR_USED,

      /*! No message in receive FIFO available        */
      eERROR_FIFO_RCV_EMPTY,

      /*! Transmit FIFO is full                       */
      eERROR_FIFO_TRM_FULL
   };


   /*!
   ** \see disconnect()
   **
   ** Connect to the physical CAN interface.
   */
   virtual InterfaceError_e connect(void) = 0;

   /*!
   ** \see connected()
   **
   ** Returns true if interface is connected.
   */
   virtual bool connected(void) = 0;

   /*!
   ** \see connect()
   **
   ** Disconnect from the physical CAN interface.
   */
   virtual InterfaceError_e disconnect(void) = 0;


   /*!
   ** Get icon from the physical CAN interface.
   */
   virtual QIcon icon(void) = 0;

   /*!
   ** Get name from the physical CAN interface.
   */
   virtual QString name(void) = 0;

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
   virtual InterfaceError_e   read( QCanFrame &clFrameR) = 0;

   /*!
   **	This function sets the bit-rate of the CAN interface. If the physical
   **	CAN interface does not support CAN FD, the parameter \c slBrsClockV
   **	is not evaluated.
   */
   virtual InterfaceError_e   setBitrate(int32_t slBitrateV,
                                         int32_t slBrsClockV = eCAN_BITRATE_NONE) = 0;


   /*!
   ** Set mode of the physical interface.
   */
   virtual InterfaceError_e	setMode(const CAN_Mode_e teModeV) = 0;


   /*!
   ** Get state of the physical CAN interface.
   */
   virtual CAN_State_e	state(void) = 0;


   virtual InterfaceError_e   statistic(QCanStatistic_ts &clStatisticR) = 0;

   /*!
   ** The function returns the supported features of the physical
   ** channel \c ubChannelV. The first CAN channel starts at index 0.
   ** The return value is a bit-mask using values defined in the header
   ** file qcan_defs.hpp.
   **
   */
   virtual uint32_t           supportedFeatures(void) = 0;
	

   /*!
   ** \see  read()
   **
   ** The functions writes a CAN message to the physical CAN interface
   ** \c ubChannelV of the CAN interface. The first CAN channel starts at
   ** index 0. On success the function returns eERROR_NONE.
   **
   */
   virtual InterfaceError_e	write(const QCanFrame &clFrameR) = 0;



Q_SIGNALS:
    void errorOccurred(int32_t slCanBusErrorV);     //  QCanBusDevice::CanBusError
    void framesReceived(uint32_t ulFramesCountV);
    void framesWritten(uint32_t ulFramesCountV);
    void stateChanged(int32_t slCanBusDevStatusV);

private:


};

#endif /*QCAN_INTERFACE_HPP_*/
