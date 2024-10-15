//====================================================================================================================//
// File:          qcan_interface_vci.hpp                                                                              //
// Description:   QCan Interface of VCI library                                                                       //
//                                                                                                                    //
// Copyright (C) MicroControl GmbH & Co. KG                                                                           //
// 53844 Troisdorf - Germany                                                                                          //
// www.microcontrol.net                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the   //
// following conditions are met:                                                                                      //
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions, the following   //
//    disclaimer and the referenced file 'LICENSE'.                                                                   //
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       //
//    following disclaimer in the documentation and/or other materials provided with the distribution.                //
// 3. Neither the name of MicroControl nor the names of its contributors may be used to endorse or promote products   //
//    derived from this software without specific prior written permission.                                           //
//                                                                                                                    //
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance     //
// with the License. You may obtain a copy of the License at                                                          //
//                                                                                                                    //
//    http://www.apache.org/licenses/LICENSE-2.0                                                                      //
//                                                                                                                    //
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed   //
// on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for  //
// the specific language governing permissions and limitations under the License.                                     //
//                                                                                                                    //
//====================================================================================================================//


#ifndef QCAN_INTERFACE_VCI4_HPP_
#define QCAN_INTERFACE_VCI4_HPP_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtCore/QLibrary>
#include <QtCore/QTimer>

#include <QtGui/QIcon>


#include <QCanInterface>

#include "vcisdk.h"

//-----------------------------------------------------------------------------
/*!
** \class   QCanInterfaceVci
** \brief   HMS VCI4 interface
** 
*/
class QCanInterfaceVci : public QCanInterface
{
   Q_OBJECT

public:

   QCanInterfaceVci(VCIDEVICEINFO clDevInfoV);
   ~QCanInterfaceVci();

   QCanInterfaceVci(const QCanInterfaceVci&) = delete;               // no copy constructor
   QCanInterfaceVci& operator=(const QCanInterfaceVci&) = delete;    // no assignment operator
   QCanInterfaceVci(QCanInterfaceVci&&) = delete;                    // no move constructor
   QCanInterfaceVci& operator=(QCanInterfaceVci&&) = delete;         // no move operator

   InterfaceError_e  connect(void) override;

   ConnectionState_e connectionState(void) override;

   InterfaceError_e  disconnect(void) override;

   void              disableFeatures(uint32_t ulFeatureMaskV) override;

   void              enableFeatures(uint32_t ulFeatureMaskV) override;

   QIcon             icon(void) override;

   QString           name(void) override;

   InterfaceError_e  read( QCanFrame &clFrameR) override;

   InterfaceError_e  reset(void) override;

   InterfaceError_e  setBitrate(int32_t slNomBitRateV, int32_t slDatBitRateV = QCan::eCAN_BITRATE_NONE) override;

   InterfaceError_e  setMode( const QCan::CAN_Mode_e teModeV) override;

   QCan::CAN_State_e state(void) override;

   InterfaceError_e  statistic(QCanStatistic_ts &clStatisticR) override;

   uint32_t          supportedFeatures(void) override;

   QString           version(void) override;

   InterfaceError_e  write(const QCanFrame &clFrameR) override;


Q_SIGNALS:

   void  addLogMessage(const QString & clMessageR, const QCan::LogLevel_e & teLogLevelR = QCan::eLOG_LEVEL_WARN);
   void  connectionChanged(const QCanInterface::ConnectionState_e & teConnectionStateR);
   void  readyRead(void);
   void  stateChanged(const QCan::CAN_State_e & teCanStateR);

private slots:
   void  onTimerEvent(void);

private:

   VCIDEVICEINFO  clDevInfoP;

   IBalObject     *ptsBalObjectP;   // bus access object

   uint32_t       ulBusCtlNumP;     // controller number
   uint32_t       ulSocketNumP;     // socket number

   ICanControl2 * ptsCanControlP;   // control interface
   ICanChannel2 * ptsCanChannelP;   // channel interface

   CANINITLINE2      tsLineInitP;      // structure for CAN line initiasation
   PFIFOREADER       ptsRcvFifoP;

   PFIFOWRITER       ptsTrmFifoP;

   HANDLE            ulDeviceEventP;

   /*! Possible features of CAN interface             */
   uint32_t          ulFeaturesDeviceP;

   /*! Enabled features of CAN interface              */
   uint32_t          ulFeaturesActiveP;

   /*! Nominal and data bitrate structures for VCI    */
   CANBTP            tsBitrateNomP;
   CANBTP            tsBitrateDatP;

   /*! Current mode of CAN interface                  */
   QCan::CAN_Mode_e  teCanModeP;

   /*! Statistic values                               */
   QCanStatistic_ts  clStatisticP;

   /*! CAN interface connection state                 */
   ConnectionState_e teConnectedP;

   /*! Flag if CAN FD is used                         */
   bool              btFdUsedP;

   /*! Buffer for error frame                         */
   QCanFrame         clErrFrameP;

   /*! Error state                                    */
   QCan::CAN_State_e teErrorStateP;

   /*! Name of interface                              */
   QString           clNameP;

   /*! Logging information (trace)                    */
   QString           clLogMsgP;


   /*! Icon of interface                              */
   QIcon             clIconP;

   bool  socketInit(void);
   void  socketRelease(void);



};

#endif /*QCAN_INTERFACE_VCI4_HPP_*/
