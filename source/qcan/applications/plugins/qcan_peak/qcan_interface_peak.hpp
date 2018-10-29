//====================================================================================================================//
// File:          qcan_interface_peak.hpp                                                                             //
// Description:   QCan Interface of PCAN Basic library                                                                //
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


#ifndef QCAN_INTERFACE_PEAK_H_
#define QCAN_INTERFACE_PEAK_H_

#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtCore/QLibrary>
#include <QtCore/QTimer>

#include <QtGui/QIcon>


#include <QCanInterface>
#include "qcan_pcan_basic.hpp"


//----------------------------------------------------------------------------------------------------------------
/*!
** \class   QCanInterfacePeak
**
** The QCanInterfacePeak class provides access to CAN interfaces from PEAK.
*/
class QCanInterfacePeak : public QCanInterface
{
    Q_OBJECT

public:

   QCanInterfacePeak(uint16_t uwPCanChannelV);
   ~QCanInterfacePeak();

   InterfaceError_e  connect(void) Q_DECL_OVERRIDE;

   ConnectionState_e connectionState(void) Q_DECL_OVERRIDE;

   InterfaceError_e  disconnect(void) Q_DECL_OVERRIDE;

   void              disableFeatures(uint32_t ulFeatureMaskV) Q_DECL_OVERRIDE;

   void              enableFeatures(uint32_t ulFeatureMaskV) Q_DECL_OVERRIDE;

   QIcon             icon(void) Q_DECL_OVERRIDE;

   QString           name(void) Q_DECL_OVERRIDE;

   InterfaceError_e  read( QCanFrame &clFrameR) Q_DECL_OVERRIDE;

   InterfaceError_e  reset(void) Q_DECL_OVERRIDE;

   InterfaceError_e  setBitrate( int32_t slBitrateV,
                                 int32_t slBrsClockV) Q_DECL_OVERRIDE;

   InterfaceError_e  setMode( const CAN_Mode_e teModeV) Q_DECL_OVERRIDE;

   CAN_State_e       state(void) Q_DECL_OVERRIDE;

   InterfaceError_e  statistic(QCanStatistic_ts &clStatisticR) Q_DECL_OVERRIDE;

   uint32_t          supportedFeatures(void) Q_DECL_OVERRIDE;

   QString           version(void) Q_DECL_OVERRIDE;

   InterfaceError_e  write(const QCanFrame &clFrameR) Q_DECL_OVERRIDE;


Q_SIGNALS:

   void  addLogMessage(const QString & clMessageR, const LogLevel_e & teLogLevelR = eLOG_LEVEL_WARN);
   void  connectionChanged(const QCanInterface::ConnectionState_e & teConnectionStateR);
   void  readyRead(void);
   void  stateChanged(const CAN_State_e & teCanStateR);

private slots:
   void  onTimerEvent(void);

private:


   /*!
    * \brief ubChannelP
    * This value holds channel number of interface
    */
   uint16_t          uwPCanChannelP;

   uint16_t          uwPCanBitrateP;

   //----------------------------------------------------------------
   // Reference to the static PCAN Basic lib
   //
   QCanPcanBasic &   pclPcanBasicP = QCanPcanBasic::getInstance();


   /*! Enabled features of CAN interface              */
   uint32_t          ulFeaturesP;

   /*! Current mode of CAN interface                  */
   CAN_Mode_e        teCanModeP;

   /*! Statistic values                               */
   QCanStatistic_ts  clStatisticP;

   /*! CAN interface connection state                 */
   ConnectionState_e teConnectedP;

   /*! Flag if CAN FD is used                         */
   bool        btFdUsedP;

   /*! Flag if CAN frame has been received            */
   bool        btHasReceivedFrameP;

   /*! Buffer for error frame                         */
   QCanFrame   clErrFrameP;

   /*! Buffer for received frame                      */
   QCanFrame   clRcvFrameP;

   /*! Error state                                    */
   CAN_State_e teErrorStateP;

   /*! Name of interface                              */
   QString  clNameP;

   /*! Icon of interface                              */
   QIcon    clIconP;

   void  checkStatus(void);

   /*!
    * \brief readFrame
    * \param clFrameR
    *
    * Read CAN message from peak USB device
    */
   InterfaceError_e  readFrame(QCanFrame &clFrameR);

   /*!
    * \brief readFrameFD
    * \param clFrameR
    * \param ubChannelV
    * \return
    *
    * Read CAN FD message from peak USB device
    */
   InterfaceError_e  readFrameFD(QCanFrame &clDataR);


   void  setupErrorFrame(TPCANStatus ulStatusV);


};

#endif /*QCAN_INTERFACE_PEAK_H_*/
