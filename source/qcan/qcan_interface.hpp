//============================================================================//
// File:          qcan_interface.hpp                                          //
// Description:   Virtual CAN interface class                                 //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53842 Troisdorf - Germany                                                  //
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
#include "qcan_frame.hpp"
#include "canpie.h"


typedef struct QCanStatistic_s {
   uint32_t   ulRcvCount;
   uint32_t   ulTrmCount;
   uint32_t   ulErrCount;
} QCanStatistic_ts;

//-----------------------------------------------------------------------------
/*!
** \class   QCanInterface
** \brief   CAN interface
** 
*/
class QCanInterface
{
public:

//   QCanInterface();

//   QCanInterface(QString clName);

   virtual 			~QCanInterface() {};

   // Demo Function
   virtual QString echo(const QString &message) = 0;

   /*!
   **	\brief	set bitrate
   **
   **	Set bitrate of CAN interface
   */
   virtual int32_t 	setBitrate(uint32_t ulBitrateV, uint32_t ulBrsClockV) = 0;


//    /*!
//    **	\brief	Set Mode
//    **
//    **	Set mode of CAN interface
//    */
//    virtual int32_t	setMode(const uint32_t ulModeV) = 0;


//    /*!
//       ** \brief   get CAN state
//       **
//       ** get state  of CAN interface
//       */
//    virtual int32_t	state(void) = 0;

//    virtual int32_t	statistic(QCanStatistic_ts &clStatisticR) = 0;

	
//    virtual int32_t	read(QCanFrame &clFrameR) = 0;

//    virtual int32_t	write(const QCanFrame &clFrameR) = 0;

//    // connect the device
//    virtual int32_t   connect(void) = 0;

//    // disconnect the device
//   virtual int32_t   disconnect(void) = 0;
Q_SIGNALS:
//       void errorOccurred(QCanBusDevice::CanBusError);
//       void framesReceived();
//       void framesWritten(int32_t ulFramesCountV);
//       void stateChanged(QCanBusDevice::CanBusDeviceState state);

private:


};

QT_BEGIN_NAMESPACE

#define QCanInterface_iid "net.microcontrol.Qt.qcan.QCanInterface"

Q_DECLARE_INTERFACE(QCanInterface, QCanInterface_iid)

//#define EchoInterface_iid "org.qt-project.Qt.Examples.EchoInterface"

//Q_DECLARE_INTERFACE(EchoInterface, EchoInterface_iid)

QT_END_NAMESPACE

#endif /*QCAN_INTERFACE_HPP_*/
