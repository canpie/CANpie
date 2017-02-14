//============================================================================//
// File:          qcan_interface_peak.hpp                                     //
// Description:   CAN plugin for PEAK USB device                              //
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

#ifndef QCAN_INTERFACE_PEAK_H_
#define QCAN_INTERFACE_PEAK_H_

#include <QObject>
#include <QtPlugin>
#include <QLibrary>
#include <QCanInterface>
#include <QIcon>
#include "qcan_frame_error.hpp"
#include "qcan_pcan_basic.hpp"


//----------------------------------------------------------------------------//
// QCanPeakUsb                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
class QCanInterfacePeak : public QCanInterface
{
    Q_OBJECT

private:

   /*!
    * \brief clStatisticP
    */
   QCanStatistic_ts clStatisticP;

   /*!
    * \brief readCAN
    * \param clFrameR
    * \param ubChannelV
    * \return
    *
    * Read CAN message from peak USB device
    */
   InterfaceError_e  readCAN(QCanFrame &clFrameR);

   /*!
    * \brief readFD
    * \param clFrameR
    * \param ubChannelV
    * \return
    *
    * Read CAN FD message from peak USB device
    */
   InterfaceError_e  readFD(QCanFrame &clFrameR);

   /*!
    * \brief ubChannelP
    * This value holds channel number of interface
    */
   uint16_t uwPCanChannelP;

   //----------------------------------------------------------------
   // Reference to the static PCAN Basic lib
   //
   QCanPcanBasic &pclPcanBasicP = QCanPcanBasic::getInstance();

   /*!
    * \brief btConnectedP
    */
   bool btConnectedP;

   bool btFdUsedP;

   void setupErrorFrame(TPCANStatus ulStatusV, CpFrameError &clFrameR);

public:

   QCanInterfacePeak(uint16_t uwPCanChannelV);
   ~QCanInterfacePeak();

   InterfaceError_e  connect(void) Q_DECL_OVERRIDE;

   bool              connected(void) Q_DECL_OVERRIDE;

   InterfaceError_e  disconnect(void) Q_DECL_OVERRIDE;

   QIcon             icon(void) Q_DECL_OVERRIDE;

   QString           name(void) Q_DECL_OVERRIDE;

   InterfaceError_e  read( QByteArray &clDataR) Q_DECL_OVERRIDE;
   
   InterfaceError_e  read( QCanFrame &clFrameR) Q_DECL_OVERRIDE;
   
   InterfaceError_e  setBitrate( int32_t slBitrateV,
                                 int32_t slBrsClockV) Q_DECL_OVERRIDE;

   InterfaceError_e  setMode( const CAN_Mode_e teModeV) Q_DECL_OVERRIDE;

   CAN_State_e state(void) Q_DECL_OVERRIDE;

   InterfaceError_e  statistic(QCanStatistic_ts &clStatisticR) Q_DECL_OVERRIDE;

   uint32_t          supportedFeatures(void) Q_DECL_OVERRIDE;

   InterfaceError_e  write(const QCanFrame &clFrameR) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void errorOccurred(int32_t slCanBusErrorV);
};

#endif /*QCAN_INTERFACE_PEAK_H_*/
