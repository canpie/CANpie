//====================================================================================================================//
// File:          qcan_interface_template.hpp                                                                         //
// Description:   Template for QCan Interface                                                                         //
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

#ifndef QCAN_INTERFACE_TEMPLATE_HPP_
#define QCAN_INTERFACE_TEMPLATE_HPP_

/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtCore/QTimer>
#include <QtGui/QIcon>

#include <QCanInterface>



//----------------------------------------------------------------------------------------------------------------
/*!
** \class   QCanInterfaceTemplate
**
** The QCanInterfaceTemplate class provides a template for writing an own CAN plug-in.
*/
class QCanInterfaceTemplate : public QCanInterface
{
    Q_OBJECT

public:

   QCanInterfaceTemplate(uint16_t uwPCanChannelV);
   ~QCanInterfaceTemplate() override;

   QCanInterfaceTemplate(const QCanInterfaceTemplate&) = delete;               // no copy constructor
   QCanInterfaceTemplate& operator=(const QCanInterfaceTemplate&) = delete;    // no assignment operator
   QCanInterfaceTemplate(QCanInterfaceTemplate&&) = delete;                    // no move constructor
   QCanInterfaceTemplate& operator=(QCanInterfaceTemplate&&) = delete;         // no move operator

   InterfaceError_e  connect(void) override;

   ConnectionState_e connectionState(void) override;

   InterfaceError_e  disconnect(void) override;

   void              disableFeatures(uint32_t ulFeatureMaskV) override;

   void              enableFeatures(uint32_t ulFeatureMaskV) override;

   QIcon             icon(void) override;

   QString           name(void) override;

   InterfaceError_e  read( QCanFrame &clFrameR) override;

   InterfaceError_e  reset(void) override;

   InterfaceError_e  setBitrate( int32_t slBitrateV,
                                 int32_t slBrsClockV) override;

   InterfaceError_e  setMode( const QCan::CAN_Mode_e teModeV) override;

   QCan::CAN_State_e state(void) override;

   InterfaceError_e  statistic(QCanStatistic_ts &clStatisticR) override;

   uint32_t          supportedFeatures(void) override;

   QString           version(void) override;

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clFrameR CAN data frame
   ** \return     Status code defined by InterfaceError_e
   ** \see        read()
   **
   ** The functions simulates the process of writing a CAN message (data frame) to the CAN interface.
   ** The CAN frame identifier is evaluated first: an identifier value of 0 is treated in the
   ** following way:
   **
   **
   */
   InterfaceError_e  write(const QCanFrame &clFrameR) override;


Q_SIGNALS:

   void  addLogMessage(const QString & clMessageR, const QCan::LogLevel_e & teLogLevelR = QCan::eLOG_LEVEL_WARN);
   void  connectionChanged(const QCanInterface::ConnectionState_e & teConnectionStateR);
   void  readyRead(void);
   void  stateChanged(const QCan::CAN_State_e & teCanStateR);

private slots:
   void  onTimerEvent(void);

private:

   /*! Channel number of CAN interface                */
   uint16_t          uwChannelP;

   /*! Enabled features of CAN interface              */
   uint32_t          ulFeaturesP;

   /*! Current mode of CAN interface                  */
   QCan::CAN_Mode_e  teCanModeP;

   /*! Statistic values                               */
   QCanStatistic_ts  clStatisticP;

   /*! CAN interface connection state                 */
   ConnectionState_e teConnectedP;

   /*! Flag if CAN frame has been received            */
   bool              btHasReceivedFrameP;

   /*! Buffer for received frame                      */
   QCanFrame         clRcvFrameP;

   /*! Buffer for error frame                         */
   QCanFrame         clErrFrameP;

   /*! Error state                                    */
   QCan::CAN_State_e teErrorStateP;

   /*! Command for simulation / testing               */
   uint8_t           ubCommandP;

   /*! Counter for simulation / testing               */
   uint8_t           ubCountP;

};

#endif   /* QCAN_INTERFACE_TEMPLATE_HPP_     */
