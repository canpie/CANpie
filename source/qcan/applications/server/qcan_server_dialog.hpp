//====================================================================================================================//
// File:          qcan_server_dialog.hpp                                                                              //
// Description:   QCAN server - configuration dialog                                                                  //
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


#ifndef QCAN_SERVER_DIALOG_HPP_
#define QCAN_SERVER_DIALOG_HPP_


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


#include <QtCore/QSettings>
#include <QtWidgets/QAction>
#include <QtWidgets/QDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QToolBox>

#include "ui_ServerConfig.h"
#include "qcan_server.hpp"
#include "qcan_server_logger.hpp"
#include "qcan_interface_widget.hpp"

using namespace QCan;


//-----------------------------------------------------------------------------
/*!
** \class QCanServerDialog
** \brief CANpie Server dialog
**
**
*/
class QCanServerDialog : public QDialog
{
    Q_OBJECT

public:
    QCanServerDialog(QWidget *parent = 0);
    ~QCanServerDialog();


public slots:

   //void onNetworkConfInterface(QMouseEvent *);

   /*!
   ** This slot is triggered on change of the physical CAN interface.
   ** The parameter \c pclCanInterfaceV is a pointer to a new interface. If
   ** \c pclCanInterfaceV is NULL, the physical interface shall be removed.
   */
   void onInterfaceChange(uint8_t ubIdxV, QCanInterface * pclCanIfV);

   void onNetworkChangeBitrate(CAN_Channel_e ubChannelV, uint32_t slNomBitRateV, int32_t slDatBitRateV);

   void onNetworkShowCanFrames(CAN_Channel_e ubChannelV, uint32_t ulFrameCntV);
   void onNetworkShowErrFrames(CAN_Channel_e ubChannelV, uint32_t ulFrameCntV);
   void onNetworkShowLoad(CAN_Channel_e ubChannelV, uint8_t ubLoadV, uint32_t ulMsgPerSecV);
   void onNetworkShowState(CAN_Channel_e ubChannelV, CAN_State_e teStateV);


private slots:
   void onNetworkChange(int slIndexV);
   void onNetworkConfBitrateDat(int slBitrateV);
   void onNetworkConfBitrateNom(int slBitrateV);
   void onNetworkConfCanFd(bool btEnableV);
   void onNetworkConfEnable(bool btEnableV);
   void onNetworkConfErrorFrames(bool btEnableV);
   void onNetworkConfListenOnly(bool btEnableV);
   void onNetworkReset(bool btCheckedV);
   
   void onDeviceSpecificConfig();

   void onServerBitrateAccess(int slStateV);
   void onServerRemoteAccess(int slStateV);
   void onServerDispatchTimeLimit(int slValueV);

   void onLoggingWindow(void);

private:
   void     createActions(void);
   void     createTrayIcon(void);
   void     setupNetworks(void);
   void     showNetworkConfiguration(void);
   void     setIcon(void);
   void     updateUI(const CAN_Channel_e & ubChannelR);

   Ui_ServerConfig         ui;
   QAction *               pclActionConfigP;
   QAction *               pclActionLoggingP;
   QAction *               pclActionQuitP;

   QSystemTrayIcon *       pclIconTrayP;
   QMenu *                 pclMenuTrayP;

   QCanServer *            pclCanServerP;
   QSettings *             pclSettingsP;

   int32_t                 slLastNetworkIndexP;

   QToolBox *              pclTbxNetworkP;
   QCanInterfaceWidget *   apclCanIfWidgetP[QCAN_NETWORK_MAX];
   QCanServerLogger *      pclLoggerP;

};

#endif // QCAN_SERVER_DIALOG_HPP_
