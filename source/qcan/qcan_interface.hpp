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
//#include "canpie.h"



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
   ** \enum    Error_e
   ** \brief   Fixed errors
   **
   ** The values of the enumeration are used as return values
   */
   enum Error_e {
      eERROR_UNKNOWN = -1,
      eERROR_OK,
      eERROR_LIBRARY,
      eERROR_CHANNEL,
      eERROR_BITRATE,
      eERROR_DEVICE,
      eERROR_MODE,
      eERROR_FIFO_IN_EMPTY,
      eERROR_FIFO_OUT_FULL
   };

   /*----------------------------------------------------------------------------*/
   /*!
   ** \enum    Mode_e
   ** \brief   Mode of CAN controller
   **
   ** These values are used as parameter for the function setMode() in
   ** order to change the state of the CAN controller.
   */
   typedef enum Mode_e {
      /*!   Set controller in Stop mode (no reception / transmission possible)
      */
      eMODE_STOP = 0,

      /*!   Set controller into normal operation
      */
      eMODE_START,

      /*!   Set controller into listen-only mode
      */
      eMODE_LISTEN_ONLY,

      /*!   Set controller into Sleep mode
      */
      eMODE_SLEEP
   }Mode_te;

//   typedef struct QCanStatistic_e {
//      uint32_t   ulRcvCount;
//      uint32_t   ulTrmCount;
//      uint32_t   ulErrCount;
//   } QCanStatistic_te;

//   QCanInterface();

//   QCanInterface(QString clName);

//   virtual ~QCanInterface() {};
//    explicit QCanBusDevice(QObject *parent = Q_NULLPTR);

   // Demo Function
   virtual QString echo(const QString &message) = 0;

   /*!
   **	\brief	set bitrate
   **
   **	Set bitrate of CAN interface
   */
   virtual int32_t   setBitrate(int32_t slBitrateV, int32_t slBrsClockV) = 0;


    /*!
    **	\brief	Set Mode
    **
    **	Set mode of CAN interface
    */
    virtual int32_t	setMode(const Mode_te teModeV) = 0;


    /*!
       ** \brief   get CAN state
       **
       ** get state  of CAN interface
       */
    virtual int32_t	state(void) = 0;

    virtual void	statistic(QCanStatistic_ts &clStatisticR) = 0;

	
    virtual int32_t	read(QCanFrame &clFrameR) = 0;

    virtual int32_t	write(const QCanFrame &clFrameR) = 0;

    // connect the device
    virtual int32_t connect(void) = 0;

    // disconnect the device
    virtual void disconnect(void) = 0;

    // icon of interface
    virtual QIcon icon(void) = 0;

    // name of interface
    virtual QString name(void) = 0;

Q_SIGNALS:
    void errorOccurred(int32_t slCanBusErrorV);     //  QCanBusDevice::CanBusError
    void framesReceived(uint32_t ulFramesCountV);
    void framesWritten(uint32_t ulFramesCountV);
    void stateChanged(int32_t slCanBusDevStatusV);

private:


};

#define QCanInterface_iid "net.microcontrol.Qt.qcan.QCanInterface"

Q_DECLARE_INTERFACE(QCanInterface, QCanInterface_iid)

#endif /*QCAN_INTERFACE_HPP_*/
