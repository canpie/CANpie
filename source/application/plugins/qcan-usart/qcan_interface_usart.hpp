//============================================================================//
// File:          qcan_interface_usart.hpp                                    //
// Description:   CAN plugin for USART device                                 //
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

#ifndef QCAN_INTERFACE_USART_HPP_
#define QCAN_INTERFACE_USART_HPP_

#include <QtCore/QLibrary>
#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtCore/QTimer>

#include <QtGui/QIcon>

#include <QtWidgets/QDialog>
#include <QtWidgets/QWidget>

#include <QCanInterface>

//#include "canpie.h"

#include "qcan_usart.hpp"


#include "qcan_config.hpp"

//----------------------------------------------------------------------------//
// QCanInterfaceUsart                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
class QCanInterfaceUsart : public QCanInterface
{
    Q_OBJECT

private:

    //----------------------------------------------------------------
    // Reference to the static PCAN Basic lib
    //
    QCanUsart &clCpUsartP = QCanUsart::getInstance();

   /*!
    * \brief clStatisticP
    */
   QCanStatistic_ts clStatisticP;

   /*!
    * \brief ulUsartBitrateP
    *  Holds bitrate of USART interface
    */
   uint32_t ulUsartBitrateP;
   uint32_t ulUsartModeP;
   CAN_Mode_e teCanModeP;

   QString  clDeviceNameP;
   uint16_t uwDeviceNumberP;

   /*! Error state                                    */
   CAN_State_e       teErrorStateP;


   /*! CAN interface connection state                 */
   ConnectionState_e teConnectedP;

   QWidget *         pclViewWidgedContainerP;

   QTimer *          pclEventTimerP;

private slots:


public:

   QCanInterfaceUsart(uint16_t uwDevNrV, QString clNameV);
   ~QCanInterfaceUsart();

   InterfaceError_e  connect(void) Q_DECL_OVERRIDE;

   ConnectionState_e connectionState(void) Q_DECL_OVERRIDE;

   bool              configureDevice(void) Q_DECL_OVERRIDE;

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
   void  connected(void);
   void  disconnected(void);
   void  readyRead(void);
   void  stateChanged(const CAN_State_e & teCanStateR);

};

#endif /*QCAN_INTERFACE_USART_H_*/
