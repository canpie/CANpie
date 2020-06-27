//====================================================================================================================//
// File:          qcan_server_dialog.cpp                                                                              //
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


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include "qcan_server_dialog.hpp"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QString>

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTreeWidgetItem>

/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


#define  TAB_CONFIG_NETWORK      ((int32_t) 0)
#define  TAB_CONFIG_SERVER       ((int32_t) 1)
#define  TAB_CONFIG_INFO         ((int32_t) 2)
#define  TAB_CONFIG_PLUGIN       ((int32_t) 3)

/*--------------------------------------------------------------------------------------------------------------------*\
** Class methods                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog()                                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanServerDialog::QCanServerDialog(QWidget * parent)
   : QDialog(parent)
{
   uint8_t        ubNetworkIdxT;
   QCanNetwork *  pclNetworkT;
   QString        clNetNameT;


   //---------------------------------------------------------------------------------------------------
   // create CAN networks
   //
   setupNetworks();

   //---------------------------------------------------------------------------------------------------
   // setup the user interface
   //
   ui.setupUi(this);

   createCanInfo();

   //---------------------------------------------------------------------------------------------------
   // update version number in info tab
   //
   QString clVersionT = "CANpie FD Server Version ";
   clVersionT += QString("%1.%2.").arg(VERSION_MAJOR).arg(VERSION_MINOR);
   clVersionT += QString("%1").arg(VERSION_BUILD);
   ui.pclLblInfoVersionM->setText(clVersionT);

   //---------------------------------------------------------------------------------------------------
   // Add logging: this must be done before the QCanInterfaceWidget and QCanNetwork classes are
   // initialised
   //
   pclLoggerP = new QCanServerLoggerView(pclCanServerP);

   //---------------------------------------------------------------------------------------------------
   // Hide some elements:
   // - CAN server port
   // - Tab for plug-ins
   //
   ui.pclLblSrvPortM->hide();
   ui.pclEdtSrvPortM->hide();
   ui.pclChkDisableNetworkM->hide();
   ui.pclTabConfigM->removeTab(TAB_CONFIG_PLUGIN);

   //---------------------------------------------------------------------------------------------------
   // create toolbox for can interfaces and add logging
   //
   pclTbxNetworkP = new QToolBox(ui.pclTabConfigNetworkM);
   pclTbxNetworkP->setGeometry(QRect(0, 0, 101, 361));
   for(ubNetworkIdxT = 0; ubNetworkIdxT < QCAN_NETWORK_MAX; ubNetworkIdxT++)
   {
      apclCanIfWidgetP[ubNetworkIdxT]  = new QCanInterfaceWidget(ubNetworkIdxT);
      apclCanIfWidgetP[ubNetworkIdxT]->setGeometry(QRect(0, 0, 101, 145));
      pclTbxNetworkP->addItem(apclCanIfWidgetP[ubNetworkIdxT], 
                              ("CAN " + QString::number(ubNetworkIdxT+1,10)));

      connect( apclCanIfWidgetP[ubNetworkIdxT],
               &QCanInterfaceWidget::interfaceChanged,
               this,
               &QCanServerDialog::onInterfaceChange);
   }


   //---------------------------------------------------------------------------------------------------
   // create values for nominal bit-rate
   //
   ui.pclCbbNetNomBitrateM->clear();
   ui.pclCbbNetNomBitrateM->addItem("10 kBit/s" , (int32_t)   10000);
   ui.pclCbbNetNomBitrateM->addItem("20 kBit/s" , (int32_t)   20000);
   ui.pclCbbNetNomBitrateM->addItem("50 kBit/s" , (int32_t)   50000);
   ui.pclCbbNetNomBitrateM->addItem("100 kBit/s", (int32_t)  100000);
   ui.pclCbbNetNomBitrateM->addItem("125 kBit/s", (int32_t)  125000);
   ui.pclCbbNetNomBitrateM->addItem("250 kBit/s", (int32_t)  250000);
   ui.pclCbbNetNomBitrateM->addItem("500 kBit/s", (int32_t)  500000);
   ui.pclCbbNetNomBitrateM->addItem("800 kBit/s", (int32_t)  800000);
   ui.pclCbbNetNomBitrateM->addItem("1 MBit/s"  , (int32_t) 1000000);
   
   //---------------------------------------------------------------------------------------------------
   // create values for data bit-rate
   //
   ui.pclCbbNetDatBitrateM->clear();
   ui.pclCbbNetDatBitrateM->addItem("None"      , (int32_t) eCAN_BITRATE_NONE);
   ui.pclCbbNetDatBitrateM->addItem("1 MBit/s"  , (int32_t) 1000000);
   ui.pclCbbNetDatBitrateM->addItem("2 MBit/s"  , (int32_t) 2000000);
   ui.pclCbbNetDatBitrateM->addItem("4 MBit/s"  , (int32_t) 4000000);
   ui.pclCbbNetDatBitrateM->addItem("5 MBit/s"  , (int32_t) 5000000);
   
   //---------------------------------------------------------------------------------------------------
   // load settings
   //
   pclSettingsP = new QSettings( QSettings::NativeFormat,
                                 QSettings::UserScope,
                                 "microcontrol.net",
                                 "CANpieServer");

   //---------------------------------------------------------------------------------------------------
   // settings for server
   //
   pclSettingsP->beginGroup("Server");
   QHostAddress clHostAddrT = QHostAddress(pclSettingsP->value("hostAddress", "127.0.0.1").toString());
   pclCanServerP->setServerAddress(clHostAddrT);

   pclCanServerP->allowBitrateChange(  pclSettingsP->value("allowBitrateChange"  , 0).toBool());
   pclCanServerP->allowBusOffRecovery( pclSettingsP->value("allowBusOffRecovery" , 0).toBool());
   pclCanServerP->allowModeChange(     pclSettingsP->value("allowModeChange"     , 0).toBool());

   pclSettingsP->endGroup();

   //---------------------------------------------------------------------------------------------------
   // settings for network
   //
   for (ubNetworkIdxT = 0; ubNetworkIdxT < pclCanServerP->maximumNetwork(); ubNetworkIdxT++)
   {
      pclNetworkT = pclCanServerP->network(ubNetworkIdxT);
      clNetNameT  = "CAN_" + QString("%1").arg(ubNetworkIdxT+1);
      pclSettingsP->beginGroup(clNetNameT);

      pclLoggerP->setLogLevel((CAN_Channel_e)(ubNetworkIdxT + 1),
                              (LogLevel_e) pclSettingsP->value("loglevel", eLOG_LEVEL_INFO).toInt());

      pclNetworkT->setNetworkEnabled(     pclSettingsP->value("enabled"             , 0).toBool());

      pclNetworkT->setErrorFrameEnabled(  pclSettingsP->value("errorFrameEnabled"   , 0).toBool());

      pclNetworkT->setFlexibleDataEnabled(pclSettingsP->value("flexibleDataEnabled" , 0).toBool());

      pclNetworkT->setListenOnlyEnabled(  pclSettingsP->value("listenOnlyEnabled"   , 0).toBool());

      pclNetworkT->setBitrate(            pclSettingsP->value("bitrateNominal"   , 500000).toInt(),
                                          pclSettingsP->value("bitrateData"      , eCAN_BITRATE_NONE).toInt());

      apclCanIfWidgetP[ubNetworkIdxT]->setInterface(pclSettingsP->value("interfaceName","").toString());

      pclSettingsP->endGroup();

   }


   //---------------------------------------------------------------------------------------------------
   // connect widgets of dialog
   //
   connect(ui.pclCkbNetErrorFramesM, SIGNAL(clicked(bool)),
           this, SLOT(onNetworkConfErrorFrames(bool)));

   connect(ui.pclCkbNetCanFdM, SIGNAL(clicked(bool)),
           this, SLOT(onNetworkConfCanFd(bool)));

   connect(ui.pclCkbNetListenOnlyM, SIGNAL(clicked(bool)),
           this, SLOT(onNetworkConfListenOnly(bool)));
   
   connect(pclTbxNetworkP, SIGNAL(currentChanged(int)),
           this, SLOT(onNetworkChange(int)));

   connect(ui.pclCbbNetNomBitrateM, SIGNAL(currentIndexChanged(int)),
           this, SLOT(onNetworkConfBitrateNom(int)));

   connect(ui.pclCbbNetDatBitrateM, SIGNAL(currentIndexChanged(int)),
           this, SLOT(onNetworkConfBitrateDat(int)));

   connect(ui.pclBtnConfigDefaultM, SIGNAL(clicked(bool)),
           this, SLOT(onServerSetDefault(bool)));

   connect(ui.pclChkServerRemoteAccessM, SIGNAL(stateChanged(int)),
           this, SLOT(onServerAccessRemote(int)));

   connect(ui.pclChkChangeBitrateM, SIGNAL(stateChanged(int)),
           this, SLOT(onServerAccessBitrate(int)));

   connect(ui.pclChkChangeModeM, SIGNAL(stateChanged(int)),
           this, SLOT(onServerAccessMode(int)));

   connect(ui.pclChkBusOffRecoverM, SIGNAL(stateChanged(int)),
           this, SLOT(onServerAccessBusOff(int)));

   connect(ui.pclPbtNetDevSpecificConfigM, SIGNAL(clicked()),
           this, SLOT(onDeviceSpecificConfig()));

   connect(ui.pclBtnStatusResetM, SIGNAL(clicked(bool)),
           this, SLOT(onNetworkReset(bool)));


   //---------------------------------------------------------------------------------------------------
   // Connect signals of the available CAN networks:
   //
   for (ubNetworkIdxT = 0; ubNetworkIdxT < pclCanServerP->maximumNetwork(); ubNetworkIdxT++)
   {
      pclNetworkT = pclCanServerP->network(ubNetworkIdxT);

      connect(pclNetworkT, SIGNAL(showInterfaceState(CAN_Channel_e, QCanInterface::ConnectionState_e)),
            this, SLOT(onInterfaceStateChange(CAN_Channel_e, QCanInterface::ConnectionState_e)) );

      connect(pclNetworkT, SIGNAL(showBitrate(CAN_Channel_e, uint32_t, int32_t)),
            this, SLOT(onNetworkShowBitrate(CAN_Channel_e, uint32_t, int32_t)));

      connect(pclNetworkT, SIGNAL(showCanFrames(CAN_Channel_e, uint32_t)),
            this, SLOT(onNetworkShowCanFrames(CAN_Channel_e, uint32_t)) );

      connect(pclNetworkT, SIGNAL(showErrFrames(CAN_Channel_e, uint32_t)),
            this, SLOT(onNetworkShowErrFrames(CAN_Channel_e, uint32_t)) );

      connect(pclNetworkT, SIGNAL(showLoad(CAN_Channel_e, uint8_t, uint32_t)),
            this, SLOT(onNetworkShowLoad(CAN_Channel_e, uint8_t, uint32_t)) );

      connect(pclNetworkT, SIGNAL(showState(CAN_Channel_e, CAN_State_e)),
            this, SLOT(onNetworkShowState(CAN_Channel_e, CAN_State_e)) );

      connect(pclNetworkT, SIGNAL(showSocketState(CAN_Channel_e, uint32_t, uint32_t)),
            this, SLOT(onNetworkShowSocketState(CAN_Channel_e, uint32_t, uint32_t)) );


   }


   //---------------------------------------------------------------------------------------------------
   // create actions and tray icon in system tray
   //
   createActions();
   createTrayIcon();
   setIcon();
   pclIconTrayP->show();

   //---------------------------------------------------------------------------------------------------
   // set title of dialog window
   //
   setWindowTitle("CANpie Server");
   
   //---------------------------------------------------------------------------------------------------
   // show CAN channel 1 as default and update user interface
   //
   slNetworkTabIndexP = 0;
   ui.pclTabConfigM->setCurrentIndex(0);
   pclTbxNetworkP->setCurrentIndex(0);
   this->updateUI(eCAN_CHANNEL_1);

}


//--------------------------------------------------------------------------------------------------------------------//
// ~QCanServerDialog()                                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanServerDialog::~QCanServerDialog()
{
   uint8_t        ubNetworkIdxT;
   QCanNetwork *  pclNetworkT;
   QString        clNetNameT;

   //---------------------------------------------------------------------------------------------------
   // store network settings
   //
   for(ubNetworkIdxT = 0; ubNetworkIdxT < QCAN_NETWORK_MAX; ubNetworkIdxT++)
   {
      //-------------------------------------------------------------------------------------------
      // settings for network
      //
      pclNetworkT = pclCanServerP->network(ubNetworkIdxT);
      clNetNameT  = "CAN_" + QString("%1").arg(ubNetworkIdxT+1);
      pclSettingsP->beginGroup(clNetNameT);
      pclSettingsP->setValue("bitrateNominal"      , pclNetworkT->nominalBitrate());
      pclSettingsP->setValue("bitrateData"         , pclNetworkT->dataBitrate());
      pclSettingsP->setValue("enabled"             , pclNetworkT->isNetworkEnabled());
      pclSettingsP->setValue("errorFrameEnabled"   , pclNetworkT->isErrorFrameEnabled());
      pclSettingsP->setValue("flexibleDataEnabled" , pclNetworkT->isFlexibleDataEnabled());
      pclSettingsP->setValue("listenOnlyEnabled"   , pclNetworkT->isListenOnlyEnabled());
      pclSettingsP->setValue("loglevel"            , pclLoggerP->logLevel((CAN_Channel_e)(ubNetworkIdxT+1)));
      pclSettingsP->setValue("interfaceName"       , apclCanIfWidgetP[ubNetworkIdxT]->name());

      pclSettingsP->endGroup();
   }

   //-------------------------------------------------------------------------------------------
   // settings for server
   //
   pclSettingsP->beginGroup("Server");

   pclSettingsP->setValue("hostAddress",         pclCanServerP->serverAddress().toString());

   pclSettingsP->setValue("allowBitrateChange",  pclCanServerP->isBitrateChangeAllowed());
   pclSettingsP->setValue("allowBusOffRecovery", pclCanServerP->isBusOffRecoveryAllowed());
   pclSettingsP->setValue("allowModeChange",     pclCanServerP->isModeChangeAllowed());
   pclSettingsP->endGroup();

   delete(pclSettingsP);
   delete(pclLoggerP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::createActions()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::createActions(void)
{
   pclActionConfigP = new QAction(tr("&Configuration ..."), this);
   connect(pclActionConfigP, &QAction::triggered, this, &QWidget::showNormal);

   pclActionLoggingP = new QAction(this);
   pclActionLoggingP->setShortcut(Qt::CTRL | Qt::Key_L);
   connect(pclActionLoggingP, SIGNAL(triggered()), this, SLOT(onLoggingWindow()));
   this->addAction(pclActionLoggingP);

   pclActionQuitP = new QAction(tr("&Quit"), this);
   connect(pclActionQuitP, &QAction::triggered, qApp, &QCoreApplication::quit);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::createCanInfo()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::createCanInfo(void)
{
   uint8_t           ubNetworkCntT;
   QTreeWidgetItem * pclNetworkItemT = 0L;
   QCanNetwork *     pclNetworkT = 0L;

   //---------------------------------------------------------------------------------------------------
   // clear all items in widget
   //
   ui.pclTrwNetworkM->clear();
   ui.pclTrwNetworkM->setColumnWidth(0, 100);
   ui.pclTrwNetworkM->setColumnWidth(1, 100);
   ui.pclTrwNetworkM->setColumnWidth(2, 200);


   //---------------------------------------------------------------------------------------------------
   // setup entries for each CAN network
   //
   for(ubNetworkCntT = 0; ubNetworkCntT < pclCanServerP->maximumNetwork(); ubNetworkCntT++)
   {
      pclNetworkT = pclCanServerP->network(ubNetworkCntT);
      pclNetworkItemT = new QTreeWidgetItem(ui.pclTrwNetworkM , ubNetworkCntT);

      //-------------------------------------------------------------------------------------------
      // name of network
      //
      pclNetworkItemT->setText(0, pclNetworkT->name());

      //-------------------------------------------------------------------------------------------
      // number of connections
      //
      pclNetworkItemT->setText(1, " 0 / 0 ");
      pclNetworkItemT->setToolTip(1, tr("Number of local connections / number of WebSocket connections"));

      //-------------------------------------------------------------------------------------------
      // interface name
      //
      pclNetworkItemT->setText(2, "Virtual CAN");


      //-------------------------------------------------------------------------------------------
      // interface version
      //
      pclNetworkItemT->setText(3, " ");

   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::createTrayIcon()                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::createTrayIcon(void)
{
   pclMenuTrayP = new QMenu(this);
   pclMenuTrayP->addAction(pclActionConfigP);

   pclMenuTrayP->addSeparator();
   pclMenuTrayP->addAction(pclActionQuitP);

   pclIconTrayP = new QSystemTrayIcon(this);
   pclIconTrayP->setContextMenu(pclMenuTrayP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onDeviceSpecificConfig()                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onDeviceSpecificConfig()
{
   qDebug() << "onDeviceSpecificConfig";

   QCanNetwork *  pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if(pclNetworkT != Q_NULLPTR)
   {
      qDebug() << "Call Configuration GUI for " << pclNetworkT->name();
      pclNetworkT->setInterfaceConfiguration();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onInterfaceChange()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onInterfaceChange( const CAN_Channel_e & ubChannelR, QCanInterface * pclInterfaceV)
{
   QCanNetwork *  pclNetworkT;


   //---------------------------------------------------------------------------------------------------
   // The channel enumeration starts at 1 for a physical channel, the network list starts at 0.
   // Setup a variable for access to the network and the widget array.
   //
   uint8_t  ubIndexV = ubChannelR - 1;

   //---------------------------------------------------------------------------------------------------
   // get selected QCanNetwork
   //
   pclNetworkT = pclCanServerP->network(ubIndexV);
   if (pclNetworkT != (QCanNetwork *) 0L)
   {
      //-------------------------------------------------------------------------------------------
      // Add / remove physical CAN interface:
      // First, any existing CAN interface is stopped (disconnect from the network) and then the
      // pointer for the QCanInterface class is removed from the network. These operations never
      // fail, even if there is no physical interface present.
      //
      pclNetworkT->stopInterface();
      pclNetworkT->removeInterface();

      if (pclInterfaceV == Q_NULLPTR)
      {
         //-----------------------------------------------------------------------------------
         // Remove physical CAN interface:
         // We use the internal VCAN again, the icon is updated.
         //
         qDebug() << "QCanServerDialog::onInterfaceChange() --- pclInterfaceV == Q_NULLPTR  -> use VCAN";
         apclCanIfWidgetP[ubIndexV]->setIcon(QIcon(QCAN_IF_VCAN_ICON));

         //-----------------------------------------------------------------------------
         // Update the CAN information tree view:
         // Get the item for the given channel number and set interface name / version
         //
         QTreeWidgetItem * pclNetworkItemT = ui.pclTrwNetworkM->topLevelItem(ubChannelR - 1);

         if (pclNetworkItemT != (QTreeWidgetItem *) 0L)
         {
            pclNetworkItemT->setText(2, "Virtual CAN"   );
            pclNetworkItemT->setText(3, " "             );
         }
      }
      else
      {
         //-----------------------------------------------------------------------------------
         // Add physical CAN interface:
         // We use the icon of the QCanInterface class. The interface is started only
         // if the network is enabled.
         //
         if (pclNetworkT->addInterface(pclInterfaceV) == true)
         {
            qDebug() << "QCanServerDialog::onInterfaceChange() --- start CAN interface";
            if (pclNetworkT->isNetworkEnabled())
            {
               apclCanIfWidgetP[ubIndexV]->setIcon(pclInterfaceV->icon());
               pclNetworkT->startInterface();

               //-----------------------------------------------------------------------------
               // Update the CAN information tree view:
               // Get the item for the given channel number and set interface name / version
               //
               QTreeWidgetItem * pclNetworkItemT = ui.pclTrwNetworkM->topLevelItem(ubChannelR - 1);

               if (pclNetworkItemT != (QTreeWidgetItem *) 0L)
               {
                  pclNetworkItemT->setText(2, pclInterfaceV->name()    );
                  pclNetworkItemT->setText(3, pclInterfaceV->version() );
               }
            }
         }
         else
         {
            //-----------------------------------------------------------------------------------
            // Failed to add the new QCanInterface:
            // Log error and set VCAN icon
            //
            qDebug() << "QCanServerDialog::onInterfaceChange() --- failed to add CAN interface";
            pclLoggerP->appendMessage(ubChannelR, tr("Failed to add CAN interface"), eLOG_LEVEL_ERROR);
            apclCanIfWidgetP[ubIndexV]->setIcon(QIcon(QCAN_IF_VCAN_ICON));
         }
      }

      updateUI(ubChannelR);

   }
}

//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onInterfaceStateChange()                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onInterfaceStateChange(const CAN_Channel_e & ubChannelR,
                                              const QCanInterface::ConnectionState_e & teConnectionStateR)
{
   switch (teConnectionStateR)
   {
      //--------------------------------------------------------------------------------------
      // There is a hardware failure for the CAN interface, this means the CAN interface
      // has already been removed inside the QCanNetwork::onInterfaceConnectionChanged()
      // method. We change the icon here and issue a notification.
      //
      case QCanInterface::FailureState:

         apclCanIfWidgetP[ubChannelR - 1]->setIcon(QIcon(QCAN_IF_VCAN_ICON));

         if ( (pclIconTrayP != 0L) && (ui.pclChkDisableNotificationM->isChecked() == false) )
         {
            pclIconTrayP->showMessage(tr("Error"), tr("CAN interface has been removed"),
                                      QSystemTrayIcon::Critical);
         }

         break;

      default:

         break;
   }


}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onLoggingWindow()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onLoggingWindow(void)
{
   pclLoggerP->show();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onNetworkChange()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onNetworkChange(int slIndexV)
{
   //---------------------------------------------------------------------------------------------------
   // Store the selected tab index:
   // We store the index of the selected tab here because requesting the selected tab via the
   // method ui.pclTabConfigM->currentIndex() fails here: inside this slot still the old tab
   // is the current one, slIndexV is the new one.
   //
   slNetworkTabIndexP = slIndexV;

   //---------------------------------------------------------------------------------------------------
   // update user interface
   //
   this->updateUI(CAN_Channel_e (slIndexV + 1));
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onNetworkShowBitrate()                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onNetworkShowBitrate(CAN_Channel_e ubChannelV, uint32_t slNomBitRateV, int32_t slDatBitRateV)
{
   Q_UNUSED(slNomBitRateV);
   Q_UNUSED(slDatBitRateV);

   updateUI(ubChannelV);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onNetworkConfBitrateDat()                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfBitrateDat(int slBitrateSelV)
{
   QCanNetwork *  pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if(pclNetworkT != Q_NULLPTR)
   {
      slBitrateSelV = ui.pclCbbNetDatBitrateM->currentData().toInt();
      pclNetworkT->setBitrate(pclNetworkT->nominalBitrate(), slBitrateSelV);
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onNetworkConfBitrateNom()                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfBitrateNom(int slBitrateSelV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if(pclNetworkT != Q_NULLPTR)
   {
      slBitrateSelV = ui.pclCbbNetNomBitrateM->currentData().toInt();
      pclNetworkT->setBitrate(slBitrateSelV, pclNetworkT->dataBitrate());
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onNetworkConfCanFd()                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfCanFd(bool btEnableV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if (pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setFlexibleDataEnabled(btEnableV);
   }

   //----------------------------------------------------------------
   // update user interface
   //
   if (btEnableV == true)
   {
      ui.pclLblNetDatBitrateM->setEnabled(true);
      ui.pclCbbNetDatBitrateM->setEnabled(true);
   }
   else
   {
      ui.pclLblNetDatBitrateM->setEnabled(false);
      ui.pclCbbNetDatBitrateM->setEnabled(false);
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onNetworkConfEnable()                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfEnable(bool btEnableV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if (pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setNetworkEnabled(btEnableV);
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onNetworkConfErrorFrames()                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfErrorFrames(bool btEnableV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if (pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setErrorFrameEnabled(btEnableV);
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onNetworkConfListenOnly()                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfListenOnly(bool btEnableV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if (pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setListenOnlyEnabled(btEnableV);
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onNetworkReset()                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onNetworkReset(bool btCheckedV)
{
   Q_UNUSED(btCheckedV);
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if(pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->reset();
   }
   ui.pclBtnStatusResetM->setEnabled(false);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onNetworkShowCanFrames()                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onNetworkShowCanFrames(CAN_Channel_e teChannelV, uint32_t ulFrameCntV)
{
   if (teChannelV == selectedChannel())
   {
      ui.pclCntStatCanM->setText(QString("%1").arg(ulFrameCntV));
      if (ulFrameCntV > 0)
      {
         ui.pclBtnStatusResetM->setEnabled(true);
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onNetworkShowErrFrames()                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onNetworkShowErrFrames(CAN_Channel_e teChannelV, uint32_t ulFrameCntV)
{
   if (teChannelV == selectedChannel())
   {
      ui.pclCntStatErrM->setText(QString("%1").arg(ulFrameCntV));
      if (ulFrameCntV > 0)
      {
         ui.pclBtnStatusResetM->setEnabled(true);
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onNetworkShowLoad()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onNetworkShowLoad(CAN_Channel_e teChannelV, uint8_t ubLoadV, uint32_t ulMsgPerSecV)
{
   if (teChannelV == selectedChannel())
   {
      ui.pclCntStatMsgM->setText(QString("%1").arg(ulMsgPerSecV));
      ui.pclPgbStatLoadM->setValue(ubLoadV);
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onNetworkShowSocketState()                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onNetworkShowSocketState(const CAN_Channel_e & ubChannelR,
                                                const uint32_t & ulLocalSocketsR, const uint32_t & ulTcpSocketsR)
{
   QTreeWidgetItem * pclNetworkItemT = 0L;

   //---------------------------------------------------------------------------------------------------
   // get the item for the given channel number
   //
   pclNetworkItemT = ui.pclTrwNetworkM->topLevelItem(ubChannelR - 1);

   if (pclNetworkItemT != (QTreeWidgetItem *) 0L)
   {
      pclNetworkItemT->setText(1, QString(" %1 / %2").arg(ulLocalSocketsR).arg(ulTcpSocketsR) );
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onNetworkShowState()                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onNetworkShowState(CAN_Channel_e teChannelV, CAN_State_e teStateV)
{

   if (teChannelV == selectedChannel())
   {
      ui.pclTxtStatusBusM->setStyleSheet("QLabel { color : black; }");
      switch (teStateV)
      {
         case eCAN_STATE_STOPPED:
            ui.pclTxtStatusBusM->setText(tr("Stopped"));
            break;

         case eCAN_STATE_SLEEPING:
            ui.pclTxtStatusBusM->setText(tr("Sleeping"));
            break;

         case eCAN_STATE_BUS_ACTIVE:
            ui.pclTxtStatusBusM->setStyleSheet("QLabel { color : green; }");
            ui.pclTxtStatusBusM->setText(tr("OK (error active)"));
            break;

         case eCAN_STATE_BUS_WARN:
            ui.pclTxtStatusBusM->setStyleSheet("QLabel { color : orange; }");
            ui.pclTxtStatusBusM->setText(tr("Warning"));
            break;

         case eCAN_STATE_BUS_PASSIVE:
            ui.pclTxtStatusBusM->setStyleSheet("QLabel { color : orange; }");
            ui.pclTxtStatusBusM->setText(tr("Error passive"));
            break;

         case eCAN_STATE_BUS_OFF:
            ui.pclTxtStatusBusM->setStyleSheet("QLabel { color : red; }");
            ui.pclTxtStatusBusM->setText(tr("Bus-off"));
            break;
      }
   }

   //---------------------------------------------------------------------------------------------------
   // Notify a bus-off state
   //
   if ( (pclIconTrayP != 0L)                                  &&
        (ui.pclChkDisableNotificationM->isChecked() == false) &&
        (teStateV == eCAN_STATE_BUS_OFF)                          )
   {
      pclIconTrayP->showMessage(tr("Error"),
                                tr(QString("CAN %1 interface is bus-off").arg(teChannelV).toLatin1()),
                                QSystemTrayIcon::Information);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onServerAccessBitrate()                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onServerAccessBitrate(int slStateV)
{

   if(slStateV == Qt::Unchecked)
   {
      pclCanServerP->allowBitrateChange(false);
   }
   else
   {
      pclCanServerP->allowBitrateChange(true);
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onServerAccessBusOff()                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onServerAccessBusOff(int slStateV)
{
   if(slStateV == Qt::Unchecked)
   {
      pclCanServerP->allowBusOffRecovery(false);
   }
   else
   {
      pclCanServerP->allowBusOffRecovery(true);
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onServerAccessMode()                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onServerAccessMode(int slStateV)
{
   if(slStateV == Qt::Unchecked)
   {
      pclCanServerP->allowModeChange(false);
   }
   else
   {
      pclCanServerP->allowModeChange(true);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onServerAccessRemote()                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onServerAccessRemote(int slStateV)
{
   QHostAddress clHostAddressT;

   if(slStateV == Qt::Unchecked)
   {
      clHostAddressT = QHostAddress(QHostAddress::LocalHost);
   }
   else
   {
      clHostAddressT = QHostAddress(QHostAddress::Any);
   }
   pclCanServerP->setServerAddress(clHostAddressT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::onServerSetDefault()                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::onServerSetDefault(bool btCheckedV)
{
   Q_UNUSED(btCheckedV);


   //---------------------------------------------------------------------------------------------------
   // set log level of all networks to eLOG_LEVEL_INFO
   //
   for (uint8_t ubChannelCntT = eCAN_CHANNEL_1; ubChannelCntT <= pclCanServerP->maximumNetwork(); ubChannelCntT++)
   {
      pclLoggerP->setLogLevel((CAN_Channel_e) ubChannelCntT, eLOG_LEVEL_INFO);
   }

   //---------------------------------------------------------------------------------------------------
   // server can't be accessed via network
   //
   ui.pclChkServerRemoteAccessM->setChecked(false);

   //---------------------------------------------------------------------------------------------------
   // do not allow bit-rate change
   //
   ui.pclChkChangeBitrateM->setChecked(false);

   //---------------------------------------------------------------------------------------------------
   // do not allow mode change
   //
   ui.pclChkChangeModeM->setChecked(false);

   //---------------------------------------------------------------------------------------------------
   // all networks are enabled
   //
   QCanNetwork *     pclNetworkT = 0L;
   ui.pclChkDisableNetworkM->setChecked(false);
   for(uint8_t ubNetworkCntT = 0; ubNetworkCntT < pclCanServerP->maximumNetwork(); ubNetworkCntT++)
   {
      pclNetworkT = pclCanServerP->network(ubNetworkCntT);

      if (pclNetworkT != (QCanNetwork *) 0L)
      {
         pclNetworkT->setNetworkEnabled(true);
      }
   }

   //---------------------------------------------------------------------------------------------------
   // recover from bus-off is disabled
   //
   ui.pclChkBusOffRecoverM->setChecked(false);

   //---------------------------------------------------------------------------------------------------
   // notification are possible
   //
   ui.pclChkDisableNotificationM->setChecked(false);

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::selectedChannel()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CAN_Channel_e QCanServerDialog::selectedChannel(void)
{
   return ((CAN_Channel_e) (slNetworkTabIndexP + 1));
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::setIcon()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::setIcon(void)
{
    QIcon clIconT = QIcon(":images/canpie_server_512.png");

    pclIconTrayP->setIcon(clIconT);
    pclIconTrayP->setToolTip("CANpie Server");
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::setupNetworks()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::setupNetworks(void)
{
   pclCanServerP = new QCanServer(this, QCAN_WEB_SOCKET_DEFAULT_PORT, QCAN_NETWORK_MAX, true);
   if (pclCanServerP->state() == QCanServer::eERROR_ACTIVE)
   {
      QMessageBox::warning(0L, "CANpie FD Server", "CANpie Server is already running.");
               exit(0);

   }
}



//--------------------------------------------------------------------------------------------------------------------//
// QCanServerDialog::updateUI()                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerDialog::updateUI(const CAN_Channel_e & ubChannelR)
{
   QCanNetwork * pclNetworkT;

   qDebug() << "QCanServerDialog::updateUI(CAN channel" << ubChannelR << ")";
   qDebug() << "Selected channel: " << selectedChannel();
   //---------------------------------------------------------------------------------------------------
   // update network tab
   //
   pclNetworkT = pclCanServerP->network(ubChannelR - 1);
   if(pclNetworkT != Q_NULLPTR)
   {

      //-------------------------------------------------------------------------------------------
      // Does it support error frames?
      //
      if(pclNetworkT->hasErrorFrameSupport())
      {
         ui.pclCkbNetErrorFramesM->setEnabled(true);
         ui.pclCkbNetErrorFramesM->setChecked(pclNetworkT->isErrorFrameEnabled());
      }
      else
      {
         ui.pclCkbNetErrorFramesM->setChecked(false);
         ui.pclCkbNetErrorFramesM->setEnabled(false);
      }
      
      //-------------------------------------------------------------------------------------------
      // Does it support CAN-FD frames?
      //
      if(pclNetworkT->hasFlexibleDataSupport())
      {
         ui.pclCkbNetCanFdM->setEnabled(true);
         ui.pclCkbNetCanFdM->setChecked(pclNetworkT->isFlexibleDataEnabled());
      }
      else
      {
         ui.pclCkbNetCanFdM->setChecked(false);
         ui.pclCkbNetCanFdM->setEnabled(false);
      }


      //-------------------------------------------------------------------------------------------
      // Is CAN-FD activated?
      //
      if(ui.pclCkbNetCanFdM->isChecked())
      {
         ui.pclLblNetDatBitrateM->setEnabled(true);
         ui.pclCbbNetDatBitrateM->setEnabled(true);
      }
      else
      {
         ui.pclLblNetDatBitrateM->setEnabled(false);
         ui.pclCbbNetDatBitrateM->setEnabled(false);
      }


      //-------------------------------------------------------------------------------------------
      // Does it support Listen-Only mode?
      //
      if(pclNetworkT->hasListenOnlySupport())
      {
         ui.pclCkbNetListenOnlyM->setEnabled(true);
         ui.pclCkbNetListenOnlyM->setChecked(pclNetworkT->isListenOnlyEnabled());
      }
      else
      {
         ui.pclCkbNetListenOnlyM->setChecked(false);
         ui.pclCkbNetListenOnlyM->setEnabled(false);
      }

      //-------------------------------------------------------------------------------------------
      // Does it support Device Specific Configuration
      //
      if (pclNetworkT->hasSpecificConfigurationSupport())
      {
         ui.pclPbtNetDevSpecificConfigM->show();
         ui.pclPbtNetDevSpecificConfigM->setEnabled(true);
      }
      else
      {
         ui.pclPbtNetDevSpecificConfigM->setEnabled(false);
         ui.pclPbtNetDevSpecificConfigM->hide();
      }

      //-------------------------------------------------------------------------------------------
      // Show current nominal bit-rate:
      // During update of the widget all signals from the widget are blocked, so a call of
      // onNetworkConfBitrateNom() is suppressed.
      //
      int32_t slNomBitRateSelT = ui.pclCbbNetNomBitrateM->findData(pclNetworkT->nominalBitrate());
      ui.pclCbbNetNomBitrateM->blockSignals(true);
      ui.pclCbbNetNomBitrateM->setCurrentIndex(slNomBitRateSelT);
      ui.pclCbbNetNomBitrateM->blockSignals(false);

      
      //-------------------------------------------------------------------------------------------
      // Show current data bit-rate:
      // During update of the widget all signals from the widget are blocked, so a call of
      // onNetworkConfBitrateDat() is suppressed.
      //
      int32_t slDatBitRateSelT = ui.pclCbbNetDatBitrateM->findData(pclNetworkT->dataBitrate());
      ui.pclCbbNetDatBitrateM->blockSignals(true);
      ui.pclCbbNetDatBitrateM->setCurrentIndex(slDatBitRateSelT);
      ui.pclCbbNetDatBitrateM->blockSignals(false);


      //-------------------------------------------------------------------------------------------
      // Bus load and Messages/s are set to 0 by default
      //
      ui.pclCntStatMsgM->setText("0");
      ui.pclPgbStatLoadM->setValue(0);


      //-------------------------------------------------------------------------------------------
      // The reset button is disabled by default, it is enabled by onNetworkShowCanFrames() or
      // onNetworkShowErrFrames()
      //
      ui.pclBtnStatusResetM->setEnabled(false);

      //-------------------------------------------------------------------------------------------
      // Update value for CAN messages
      //
      onNetworkShowCanFrames(ubChannelR, pclNetworkT->frameCount());

      //-------------------------------------------------------------------------------------------
      // Update value for Error messages
      //
      onNetworkShowErrFrames(ubChannelR, pclNetworkT->frameCountError());

      //-------------------------------------------------------------------------------------------
      // show current CAN bus status
      //
      onNetworkShowState(ubChannelR, pclNetworkT->state());
   }

   //---------------------------------------------------------------------------------------------------
   // update server tab
   //
   //---------------------------------------------------------------------------------------------------

   QHostAddress clHostAddrT = pclCanServerP->serverAddress();
   if (clHostAddrT == QHostAddress("0.0.0.0"))
   {
      ui.pclChkServerRemoteAccessM->setCheckState(Qt::Checked);
   }
   else
   {
      ui.pclChkServerRemoteAccessM->setCheckState(Qt::Unchecked);
   }

   //---------------------------------------------------------------------------------------------------
   // Clear / set check for bitrate change
   //
   if (pclCanServerP->isBitrateChangeAllowed())
   {
      ui.pclChkChangeBitrateM->setCheckState(Qt::Checked);
   }
   else
   {
      ui.pclChkChangeBitrateM->setCheckState(Qt::Unchecked);
   }

   //---------------------------------------------------------------------------------------------------
   // Clear / set check for mode change
   //
   if (pclCanServerP->isModeChangeAllowed())
   {
      ui.pclChkChangeModeM->setCheckState(Qt::Checked);
   }
   else
   {
      ui.pclChkChangeModeM->setCheckState(Qt::Unchecked);
   }

   //---------------------------------------------------------------------------------------------------
   // Clear / set check for bus-off recovery
   //
   if (pclCanServerP->isBusOffRecoveryAllowed())
   {
      ui.pclChkBusOffRecoverM->setCheckState(Qt::Checked);
   }
   else
   {
      ui.pclChkBusOffRecoverM->setCheckState(Qt::Unchecked);
   }
   
}
