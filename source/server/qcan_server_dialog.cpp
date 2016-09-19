//============================================================================//
// File:          qcan_network.cpp                                            //
// Description:   QCAN classes - CAN network                                  //
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


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "qcan_server_dialog.hpp"

#include <QCoreApplication>
#include <QDebug>

#include <QDir>


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanServer()                                                               //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanServerDialog::QCanServerDialog(QWidget * parent)
   : QDialog(parent)
{
   uint8_t        ubNetworkIdxT;
   QCanNetwork *  pclNetworkT;
   QString        clNetNameT;

   //----------------------------------------------------------------
   // create CAN networks
   //
   setupNetworks();

   //----------------------------------------------------------------
   // setup the user interface
   //
   ui.setupUi(this);

   //----------------------------------------------------------------
   // connect widgets of dialog
   //
   connect(ui.pclCkbNetEnableM, SIGNAL(clicked(bool)),
           this, SLOT(onNetworkConfEnable(bool)));

   connect(ui.pclTbxNetworkM, SIGNAL(currentChanged(int)),
           this, SLOT(onNetworkChange(int)));

   connect(ui.pclCbbNetBitrateM, SIGNAL(currentIndexChanged(int)),
           this, SLOT(onNetworkConfBitrate(int)));

   //----------------------------------------------------------------
   // connect default signals / slots for statistic
   //
   pclNetworkT = pclCanServerP->network(0);
   connect(pclNetworkT, SIGNAL(showCanFrames(uint32_t)),
           this, SLOT(onNetworkShowCanFrames(uint32_t)) );

   connect(pclNetworkT, SIGNAL(showErrFrames(uint32_t)),
           this, SLOT(onNetworkShowErrFrames(uint32_t)) );

   connect(pclNetworkT, SIGNAL(showLoad(uint8_t, uint32_t)),
            this, SLOT(onNetworkShowLoad(uint8_t, uint32_t)) );

   //----------------------------------------------------------------
   // load settings
   //
   pclSettingsP = new QSettings( QSettings::NativeFormat,
                                 QSettings::UserScope,
                                 "microcontrol.net",
                                 "QCANserver");

   for(ubNetworkIdxT = 0; ubNetworkIdxT < QCAN_NETWORK_MAX; ubNetworkIdxT++)
   {
      pclNetworkT = pclCanServerP->network(ubNetworkIdxT);
      clNetNameT  = "CAN " + QString("%1").arg(ubNetworkIdxT+1);
      pclSettingsP->beginGroup(clNetNameT);
      //pclSettingsP->setValue("interface",  win->isFullScreen());
      pclNetworkT->setBitrate(pclSettingsP->value("bitrate",
                                 QCan::eCAN_BITRATE_500K).toInt(), 0);

      pclNetworkT->setNetworkEnabled(pclSettingsP->value("enable",
                                 0).toBool());

      pclNetworkT->setErrorFramesEnabled(pclSettingsP->value("errorFrame",
                                 0).toBool());

      pclNetworkT->setFastDataEnabled(pclSettingsP->value("canFD",
                                 0).toBool());

      pclNetworkT->setListenOnlyEnabled(pclSettingsP->value("listenOnly",
                                 0).toBool());

      pclSettingsP->endGroup();
   }

   createActions();
   createTrayIcon();

   //->
   setIcon();
   pclIconTrayP->show();




   //----------------------------------------------------------------
   // Intialise interface widgets for CAN interface selection
   //
   QDir pluginsDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
   if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
       pluginsDir.cdUp();
       pluginsDir.setPath(pluginsDir.path() + "/plugins");
#elif defined(Q_OS_MAC)
   if (pluginsDir.dirName() == "MacOS") {
       pluginsDir.cdUp();
       pluginsDir.cdUp();
       pluginsDir.cdUp();
       pluginsDir.setPath(pluginsDir.path() + "/plugins");
   }
#endif


   ui.pclTbxQCanInterfaceWidget1_M->setPluginPath(pluginsDir);
   connect(ui.pclTbxQCanInterfaceWidget1_M, SIGNAL(interfaceChanged(QCanInterface*)),
           this, SLOT(onInterfaceChange(QCanInterface*)) );

   ui.pclTbxQCanInterfaceWidget2_M->setPluginPath(pluginsDir);
   connect(ui.pclTbxQCanInterfaceWidget2_M, SIGNAL(interfaceChanged(QCanInterface*)),
           this, SLOT(onInterfaceChange(QCanInterface*)) );

   ui.pclTbxQCanInterfaceWidget3_M->setPluginPath(pluginsDir);
   connect(ui.pclTbxQCanInterfaceWidget3_M, SIGNAL(interfaceChanged(QCanInterface*)),
           this, SLOT(onInterfaceChange(QCanInterface*)) );

   ui.pclTbxQCanInterfaceWidget4_M->setPluginPath(pluginsDir);
   connect(ui.pclTbxQCanInterfaceWidget4_M, SIGNAL(interfaceChanged(QCanInterface*)),
           this, SLOT(onInterfaceChange(QCanInterface*)) );

   ui.pclTbxQCanInterfaceWidget5_M->setPluginPath(pluginsDir);
   connect(ui.pclTbxQCanInterfaceWidget5_M, SIGNAL(interfaceChanged(QCanInterface*)),
           this, SLOT(onInterfaceChange(QCanInterface*)) );

   ui.pclTbxQCanInterfaceWidget6_M->setPluginPath(pluginsDir);
   connect(ui.pclTbxQCanInterfaceWidget6_M, SIGNAL(interfaceChanged(QCanInterface*)),
           this, SLOT(onInterfaceChange(QCanInterface*)) );

   ui.pclTbxQCanInterfaceWidget7_M->setPluginPath(pluginsDir);
   connect(ui.pclTbxQCanInterfaceWidget7_M, SIGNAL(interfaceChanged(QCanInterface*)),
           this, SLOT(onInterfaceChange(QCanInterface*)) );

   ui.pclTbxQCanInterfaceWidget8_M->setPluginPath(pluginsDir);
   connect(ui.pclTbxQCanInterfaceWidget8_M, SIGNAL(interfaceChanged(QCanInterface*)),
           this, SLOT(onInterfaceChange(QCanInterface*)) );


   //----------------------------------------------------------------
   // show CAN channel 1 as default and update user interface
   //
   slLastNetworkIndexP = 0;
   ui.pclTbxNetworkM->setCurrentIndex(0);
   this->updateUI(0);
}


//----------------------------------------------------------------------------//
// QCanServer()                                                               //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanServerDialog::~QCanServerDialog()
{
   uint8_t        ubNetworkIdxT;
   QCanNetwork *  pclNetworkT;
   QString        clNetNameT;

   //----------------------------------------------------------------
   // store settings
   //
   for(ubNetworkIdxT = 0; ubNetworkIdxT < QCAN_NETWORK_MAX; ubNetworkIdxT++)
   {
      pclNetworkT = pclCanServerP->network(ubNetworkIdxT);
      clNetNameT  = "CAN " + QString("%1").arg(ubNetworkIdxT+1);
      pclSettingsP->beginGroup(clNetNameT);
      //pclSettingsP->setValue("interface",  win->isFullScreen());
      pclSettingsP->setValue("bitrate",    pclNetworkT->bitrate());
      pclSettingsP->setValue("enable",     pclNetworkT->isNetworkEnabled());
      pclSettingsP->setValue("errorFrame", pclNetworkT->isErrorFramesEnabled());
      pclSettingsP->setValue("canFD",      pclNetworkT->isFastDataEnabled());
      pclSettingsP->setValue("listenOnly", pclNetworkT->isListenOnlyEnabled());
      pclSettingsP->endGroup();
   }

   delete(pclSettingsP);
}


//----------------------------------------------------------------------------//
// createActions()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::createActions(void)
{
   pclActionCnfgP = new QAction(tr("&Configuration ..."), this);
   connect(pclActionCnfgP, &QAction::triggered, this, &QWidget::showNormal);


   pclActionQuitP = new QAction(tr("&Quit"), this);
   connect(pclActionQuitP, &QAction::triggered, qApp, &QCoreApplication::quit);
}


//----------------------------------------------------------------------------//
// createTrayIcon()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::createTrayIcon(void)
{
   pclMenuTrayP = new QMenu(this);
   pclMenuTrayP->addAction(pclActionCnfgP);
   pclMenuTrayP->addSeparator();
   pclMenuTrayP->addAction(pclActionQuitP);

   pclIconTrayP = new QSystemTrayIcon(this);
   pclIconTrayP->setContextMenu(pclMenuTrayP);
}

QCanInterfaceWidget * QCanServerDialog::currentInterfaceWidget(void)
{
   QCanInterfaceWidget * pclWidgetT = Q_NULLPTR;
   switch(this->currentNetwork())
   {
      case 0:
         pclWidgetT = ui.pclTbxQCanInterfaceWidget1_M;
         break;

      case 1:
         pclWidgetT = ui.pclTbxQCanInterfaceWidget2_M;
         break;

      case 2:
         pclWidgetT = ui.pclTbxQCanInterfaceWidget3_M;
         break;

      case 3:
         pclWidgetT = ui.pclTbxQCanInterfaceWidget4_M;
         break;

      case 4:
         pclWidgetT = ui.pclTbxQCanInterfaceWidget5_M;
         break;

   }


   return(pclWidgetT);
}

//----------------------------------------------------------------------------//
// currentNetwork()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t QCanServerDialog::currentNetwork(void)
{
   return(ui.pclTbxNetworkM->currentIndex());
}

//----------------------------------------------------------------------------//
// onInterfaceChange()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onInterfaceChange(QCanInterface *pclCanIfV)
{
   QCanNetwork *  pclNetworkT;

   qDebug() << "QCanServerDialog:onInterfaceChange()...";


   //----------------------------------------------------------------
   // get selected QCanNetwork
   //
   pclNetworkT = pclCanServerP->network(currentNetwork());

   //----------------------------------------------------------------
   // add / remove physical CAN interface
   //
   if (pclCanIfV == Q_NULLPTR)
   {
      pclNetworkT->removeInterface();
   }
   else
   {
      if(pclNetworkT->addInterface(pclCanIfV) == true)
      {
         currentInterfaceWidget()->setIcon(pclCanIfV->icon());
      }
   }

}


//----------------------------------------------------------------------------//
// onNetworkChange()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkChange(int slIndexV)
{
   QCanNetwork *  pclNetworkT;

   qDebug() << "onNetworkChange()" << slIndexV;

   //----------------------------------------------------------------
   // reconnect the signals / slots for statistic
   //
   pclNetworkT = pclCanServerP->network(slLastNetworkIndexP);
   disconnect(pclNetworkT, SIGNAL(showCanFrames(uint32_t)),
           this, SLOT(onNetworkShowCanFrames(uint32_t)) );

   disconnect(pclNetworkT, SIGNAL(showErrFrames(uint32_t)),
           this, SLOT(onNetworkShowErrFrames(uint32_t)) );

   disconnect(pclNetworkT, SIGNAL(showLoad(uint8_t, uint32_t)),
            this, SLOT(onNetworkShowLoad(uint8_t, uint32_t)) );


   pclNetworkT = pclCanServerP->network(slIndexV);
   connect(pclNetworkT, SIGNAL(showCanFrames(uint32_t)),
           this, SLOT(onNetworkShowCanFrames(uint32_t)) );

   connect(pclNetworkT, SIGNAL(showErrFrames(uint32_t)),
           this, SLOT(onNetworkShowErrFrames(uint32_t)) );

   connect(pclNetworkT, SIGNAL(showLoad(uint8_t, uint32_t)),
            this, SLOT(onNetworkShowLoad(uint8_t, uint32_t)) );

   //----------------------------------------------------------------
   // update user interface
   //
   this->updateUI(slIndexV);

   //----------------------------------------------------------------
   // store index for future access
   //
   slLastNetworkIndexP = slIndexV;
}

//----------------------------------------------------------------------------//
// onNetworkConfBitrate()                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfBitrate(int slBitrateV)
{
   QCanNetwork * pclNetworkT;

   qDebug() << "Change bit-rate" << slBitrateV;
   pclNetworkT = pclCanServerP->network(currentNetwork());
   if(pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setBitrate(slBitrateV, -1);
   }
}


//----------------------------------------------------------------------------//
// onNetworkConfEnable()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfEnable(bool btEnableV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(currentNetwork());
   if(pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setNetworkEnabled(btEnableV);
   }
}


//void QCanServerDialog::onNetworkConfInterface(QMouseEvent * event)
//{
//   qDebug() << "Event!";
//}

//----------------------------------------------------------------------------//
// onNetworkConfListenOnly()                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfListenOnly(bool btEnableV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(currentNetwork());
   if(pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setListenOnlyEnabled(btEnableV);
   }
}


//----------------------------------------------------------------------------//
// onNetworkShowCanFrames()                                                   //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkShowCanFrames(uint32_t ulFrameCntV)
{
   ui.pclCntStatCanM->setText(QString("%1").arg(ulFrameCntV));
}

//----------------------------------------------------------------------------//
// onNetworkShowErrFrames()                                                   //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkShowErrFrames(uint32_t ulFrameCntV)
{
   ui.pclCntStatErrM->setText(QString("%1").arg(ulFrameCntV));
}


//----------------------------------------------------------------------------//
// onNetworkShowLoad()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkShowLoad(uint8_t ubLoadV, uint32_t ulMsgPerSecV)
{
   ui.pclCntStatMsgM->setText(QString("%1").arg(ulMsgPerSecV));
   ui.pclPgbStatLoadM->setValue(ubLoadV);
}


//----------------------------------------------------------------------------//
// setupNetworks()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::setupNetworks(void)
{
   pclCanServerP = new QCanServer(  this,
                                    QCAN_TCP_DEFAULT_PORT,
                                    QCAN_NETWORK_MAX);
}


//----------------------------------------------------------------------------//
// setIcon()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::setIcon(void)
{
    QIcon clIconT = QIcon(":images/network-icon.png");

    pclIconTrayP->setIcon(clIconT);
    pclIconTrayP->setToolTip("CAN network configuration");

    //this->setWindowIcon(clIconT);

}


//----------------------------------------------------------------------------//
// updateUI()                                                                 //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::updateUI(uint8_t ubNetworkIdxV)
{
   QCanNetwork * pclNetworkT;


   pclNetworkT = pclCanServerP->network(ubNetworkIdxV);
   if(pclNetworkT != Q_NULLPTR)
   {
      //--------------------------------------------------------
      // is network enabled?
      //
      ui.pclCkbNetEnableM->setChecked(pclNetworkT->isNetworkEnabled());

      //--------------------------------------------------------
      // show current bitrate
      //
      ui.pclCbbNetBitrateM->setCurrentIndex(pclNetworkT->bitrate());
   }
}
