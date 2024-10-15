//====================================================================================================================//
// File:          qcan_interface_usart.hpp                                                                            //
// Description:   CAN plugin for USART interface                                                                      //
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

#ifndef QCAN_INTERFACE_USART_HPP_
#define QCAN_INTERFACE_USART_HPP_


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/
#include <QtCore/QLibrary>
#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtCore/QTimer>

#include <QtGui/QIcon>

#include <QtWidgets/QDialog>
#include <QtWidgets/QWidget>

#include <QCanInterface>

#include "qcan_config.hpp"

#include "qcan_usart.hpp"

/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief The maximal number of QCAN Frames for atsWriteMessageListG before they will be discarded.
 */
#define QCAN_FRAME_LIST_MAX   32

/*--------------------------------------------------------------------------------------------------------------------*\
** Declaration of global variables that are used by C and C++ code                                                    **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief btWrtieIsPendingG is set to true when transmission of a CANpie Message is pending.
 * So that no collisions occur when sending the messages.
 */
extern bool btWrtieIsPendingG;
extern QVector<CpCanMsg_ts> atsReadMessageListG;
extern QVector<QCanFrame> atsWriteMessageListG;

/*!
 * \brief tsPortP holds the CANpie Port information that is used to access the CpCore functions of CANpie.
 */
extern CpPort_ts tsPortP;

/*!
 * \brief ulStatisticTrmCountG counts number of transmit messages
 */
extern quint32 ulStatisticTrmCountG;

/*--------------------------------------------------------------------------------------------------------------------*\
** Declaration of global function that are triggered by C code                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/
/*!
 * \brief AppCanErrHandler handles the CAN error messages
 * \param ptsErrV
 * \return
 */
uint8_t AppCanErrHandler(CpState_ts  *ptsErrV);

/*!
 * \brief AppCanRcvHandler handles received CAN messages
 * \param ptsMsgV
 * \param ubBufferV
 * \return
 */
uint8_t AppCanRcvHandler(CpCanMsg_ts *ptsMsgV, uint8_t ubBufferV);

/*!
 * \brief AppCanTrmHandler handles CAN messages after the have been send
 * \param ptsMsgV
 * \param ubBufferV
 * \return
 */
uint8_t AppCanTrmHandler(CpCanMsg_ts *ptsMsgV, uint8_t ubBufferV);


//----------------------------------------------------------------------------------------------------------------
/*!
** \class   QCanInterfaceUsart
**
** The QCanInterfaceUsart class provides a implementaion for USART CAN plug-in to exchange CANpie messages
** using serial interfaces.
*/
class QCanInterfaceUsart : public QCanInterface
{
    Q_OBJECT

public:

   QCanInterfaceUsart(uint16_t uwDevNrV, QString clNameV);
   ~QCanInterfaceUsart(void) override;

   QCanInterfaceUsart(const QCanInterfaceUsart&) = delete;               // no copy constructor
   QCanInterfaceUsart& operator=(const QCanInterfaceUsart&) = delete;    // no assignment operator
   QCanInterfaceUsart(QCanInterfaceUsart&&) = delete;                    // no move constructor
   QCanInterfaceUsart& operator=(QCanInterfaceUsart&&) = delete;         // no move operator


   InterfaceError_e  connect(void) override;

   ConnectionState_e connectionState(void) override;

   bool              configureDevice(void) override;

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

   InterfaceError_e  write(const QCanFrame &clFrameR) override;


Q_SIGNALS:
   void  addLogMessage(const QString & clMessageR, const QCan::LogLevel_e & teLogLevelR = QCan::eLOG_LEVEL_WARN);
   void  connectionChanged(const QCanInterface::ConnectionState_e & teConnectionStateR);
   void  readyRead(void);
   void  stateChanged(const QCan::CAN_State_e & teCanStateR);

private:

   /*!
   * \brief Reference to the static instance of USART interfaces
   */
   QCanUsart &clCpUsartP = QCanUsart::getInstance();

   /*!
    * \brief clStatisticP - Statistic information for CAN communication
    */
   QCanStatistic_s tsStatisticP;

   /*!
    * \brief teCanModeP - Current mode of CAN interface
    */
   QCan::CAN_Mode_e  teCanModeP;

   /*!
    * \brief teErrorStateP - CAN error state
    */
   QCan::CAN_State_e teErrorStateP;

   /*!
    * \brief teConnectedP - CAN interface connection state
    */
   ConnectionState_e teConnectedP;

   /*!
    * \brief slUsartBitrateP - Bitrate used for USART communication
    */
   int32_t slUsartBitrateP;

   /*!
    * \brief ulUsartModeP - Mode used for USART communication
    */
   uint32_t ulUsartModeP;

   /*!
    * \brief clUsartNameP - Name of USART interface
    */
   QString  clUsartNameP;

   /*!
    * \brief uwUsartNumberP - Number of USART interface
    */
   uint16_t uwUsartNumberP;

private slots:
   /*!
    * \brief onTimerEvent checks for pending CAN messages and triggeres readyRead()
    */
   void  onTimerEvent(void);

};

#endif /*QCAN_INTERFACE_USART_H_*/
