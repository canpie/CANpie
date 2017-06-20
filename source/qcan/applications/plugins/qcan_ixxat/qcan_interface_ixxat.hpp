//============================================================================//
// File:          qcan_interface_ixxat.hpp                                    //
// Description:   CAN plugin for IXXAT VCI devices                            //
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


#ifndef QCAN_INTERFACE_IXXAT_H_
#define QCAN_INTERFACE_IXXAT_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include <QObject>
#include <QtPlugin>
#include <QLibrary>
#include <QVector>
#include <QCanInterface>
#include "qcan_ixxat_vci.hpp"


//-----------------------------------------------------------------------------
/*!
** \class   QCanInterfaceIxxat
** \brief   IXXAT interface
** 
*/
class QCanInterfaceIxxat : public QCanInterface
{

private:

   /*!
    * \brief clCanLibP
    */
   QLibrary clCanLibP;


   /*!
    * \brief clCanDevInfoP
    *  This value holds handle value for the device list.
    */
   HANDLE   vdDevListP;

   /*!
    * \brief clCanDevInfoP
    *  This struct holds information about selected device.
    */
//   QVector<VCIDEVICEINFO> aclCanDevInfoP;

   /*!
    * \brief uwCanDeviceP
    */
   uint16_t uwCanDeviceP = 0;

   /*!
    *  This value holds interface handle value of a device.
    */
   HANDLE   vdCanInterfaceP = 0;
   HANDLE   vdCanControlP = 0;
   HANDLE   vdCanChannelP = 0;

   /*!
    * \brief uwCanChannelP
    */
   uint16_t uwCanChannelP = 0;

   CAN_Mode_e teCanModeP;

   struct Bitrate_s {
      uint8_t ubBtr0;
      uint8_t ubBtr1;
   };
   struct Bitrate_s tsBitrateP;

   /*!
    * \brief clStatisticP
    */
   QCanStatistic_ts clStatisticP;

   /*!
    * \brief ubChannelP
    * This value holds channel number of interface
    */
   uint8_t ubChannelP;

   /*!
    * \brief btConnectedP
    */
   bool     btConnectedP = false;

   bool     btFdUsedP;

   bool     btMsgAndErrPendingP;
   VCIDEVICEINFO clDevInfoP;

   //----------------------------------------------------------------
   // Reference to the static PCAN Basic lib
   //
   QCanIxxatVci &pclIxxatVciP = QCanIxxatVci::getInstance();

   void setupErrorFrame(uint8_t ubStatusV, uint8_t ubErrorV, QCanFrameError &clFrameR);

public:

   QCanInterfaceIxxat(VCIDEVICEINFO clDevInfoV);
   ~QCanInterfaceIxxat();

   InterfaceError_e  connect(void) Q_DECL_OVERRIDE;

   bool              connected(void) Q_DECL_OVERRIDE;

   InterfaceError_e  disconnect(void) Q_DECL_OVERRIDE;

   QIcon             icon(void) Q_DECL_OVERRIDE;

   QString           name(void) Q_DECL_OVERRIDE;

   InterfaceError_e  read( QByteArray &clDataR) Q_DECL_OVERRIDE;

   InterfaceError_e  reset(void) Q_DECL_OVERRIDE;

   InterfaceError_e  setBitrate( int32_t slBitrateV,
                                 int32_t slBrsClockV) Q_DECL_OVERRIDE;

   InterfaceError_e  setMode( const CAN_Mode_e teModeV) Q_DECL_OVERRIDE;

   InterfaceError_e  statistic(QCanStatistic_ts &clStatisticR) Q_DECL_OVERRIDE;

   uint32_t          supportedFeatures(void) Q_DECL_OVERRIDE;

   InterfaceError_e  write(const QCanFrame &clFrameR) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void errorOccurred(int32_t slCanBusErrorV);
};

#endif /*QCAN_INTERFACE_IXXAT_H_*/
