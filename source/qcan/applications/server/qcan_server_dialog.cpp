//============================================================================//
// File:          qcan_server_dialog.cpp                                      //
// Description:   QCAN classes - CAN server dialog                            //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53844 Troisdorf - Germany                                                  //
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


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "qcan_server_dialog.hpp"

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
   // create toolbox for can interfaces
   //
   pclTbxNetworkP = new QToolBox(ui.pclTabConfigNetworkM);
   pclTbxNetworkP->setGeometry(QRect(0, 0, 101, 361));
   for(ubNetworkIdxT = 0; ubNetworkIdxT < QCAN_NETWORK_MAX; ubNetworkIdxT++)
   {
      apclCanIfWidgetP[ubNetworkIdxT]  = new QCanInterfaceWidget(ubNetworkIdxT);
      apclCanIfWidgetP[ubNetworkIdxT]->setGeometry(QRect(0, 0, 101, 145));
      pclTbxNetworkP->addItem(apclCanIfWidgetP[ubNetworkIdxT], 
                              ("CAN " + QString::number(ubNetworkIdxT+1,10)));
   }

   //----------------------------------------------------------------
   // connect widgets of dialog
   //
   connect(ui.pclCkbNetEnableM, SIGNAL(clicked(bool)),
           this, SLOT(onNetworkConfEnable(bool)));

   connect(ui.pclCkbNetErrorFramesM, SIGNAL(clicked(bool)),
           this, SLOT(onNetworkConfErrorFrames(bool)));

   connect(ui.pclCkbNetCanFdM, SIGNAL(clicked(bool)),
           this, SLOT(onNetworkConfCanFd(bool)));

   connect(ui.pclCkbNetListenOnlyM, SIGNAL(clicked(bool)),
           this, SLOT(onNetworkConfListenOnly(bool)));
   
   connect(pclTbxNetworkP, SIGNAL(currentChanged(int)),
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
   // Intialise interface widgets for CAN interface selection
   //
   for(ubNetworkIdxT = 0; ubNetworkIdxT < QCAN_NETWORK_MAX; ubNetworkIdxT++)
   {
      connect( apclCanIfWidgetP[ubNetworkIdxT], 
               &QCanInterfaceWidget::interfaceChanged,
               this,                                       
               &QCanServerDialog::onInterfaceChange  );
   }


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

      pclNetworkT->setBitrate(pclSettingsP->value("bitrate",
                                 eCAN_BITRATE_500K).toInt(), 0);

      pclNetworkT->setNetworkEnabled(pclSettingsP->value("enable",
                                 0).toBool());

      pclNetworkT->setErrorFramesEnabled(pclSettingsP->value("errorFrame",
                                 0).toBool());

      pclNetworkT->setFastDataEnabled(pclSettingsP->value("canFD",
                                 0).toBool());

      pclNetworkT->setListenOnlyEnabled(pclSettingsP->value("listenOnly",
                                 0).toBool());

      apclCanIfWidgetP[ubNetworkIdxT]->setInterface(pclSettingsP->value("interface"+QString::number(ubNetworkIdxT),"").toString());

      pclSettingsP->endGroup();
   }

   //----------------------------------------------------------------
   // create actions and tray icon in system tray
   //
   createActions();
   createTrayIcon();
   setIcon();
   pclIconTrayP->show();

   //----------------------------------------------------------------
   // set title of dialog window
   //
   setWindowTitle("CANpie Server");
   
   //----------------------------------------------------------------
   // show CAN channel 1 as default and update user interface
   //
   slLastNetworkIndexP = 0;
   ui.pclTabConfigM->setCurrentIndex(0);
   pclTbxNetworkP->setCurrentIndex(0);
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
      pclSettingsP->setValue("bitrate",    pclNetworkT->bitrate());
      pclSettingsP->setValue("enable",     pclNetworkT->isNetworkEnabled());
      pclSettingsP->setValue("errorFrame", pclNetworkT->isErrorFramesEnabled());
      pclSettingsP->setValue("canFD",      pclNetworkT->isFastDataEnabled());
      pclSettingsP->setValue("listenOnly", pclNetworkT->isListenOnlyEnabled());

      pclSettingsP->setValue("interface"+QString::number(ubNetworkIdxT), 
                              apclCanIfWidgetP[ubNetworkIdxT]->name());

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


//----------------------------------------------------------------------------//
// onInterfaceChange()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onInterfaceChange( uint8_t ubIdxV, 
                                          QCanInterface *pclCanIfV)
{
   QCanNetwork *  pclNetworkT;

   qDebug() << "QCanServerDialog:onInterfaceChange() ToolBox Index " << QString::number(ubIdxV);


   //----------------------------------------------------------------
   // get selected QCanNetwork
   //
   pclNetworkT = pclCanServerP->network(ubIdxV);

   //----------------------------------------------------------------
   // add / remove physical CAN interface
   //
   if (pclCanIfV == Q_NULLPTR)
   {
      pclNetworkT->removeInterface();
      apclCanIfWidgetP[ubIdxV]->setIcon(QIcon(QCAN_IF_VCAN_ICON));
      qDebug() << " set vCAN icon";
   }
   else
   {
      qDebug() << " set interface icon";
      if(pclNetworkT->addInterface(pclCanIfV) == true)
      {
         apclCanIfWidgetP[ubIdxV]->setIcon(pclCanIfV->icon());
      } else
      {
         qWarning() << "Fail to add interface!";
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
   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if(pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setBitrate(slBitrateV, -1);
   }
}


//----------------------------------------------------------------------------//
// onNetworkConfCanFd()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfCanFd(bool btEnableV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if(pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setFastDataEnabled(btEnableV);
   }
}


//----------------------------------------------------------------------------//
// onNetworkConfEnable()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfEnable(bool btEnableV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if(pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setNetworkEnabled(btEnableV);
   }
}


//----------------------------------------------------------------------------//
// onNetworkConfErrorFrames()                                                 //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfErrorFrames(bool btEnableV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if(pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setErrorFramesEnabled(btEnableV);
   }
}


//----------------------------------------------------------------------------//
// onNetworkConfListenOnly()                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfListenOnly(bool btEnableV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
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
      // Is network enabled?
      //
      ui.pclCkbNetEnableM->setChecked(pclNetworkT->isNetworkEnabled());

      //--------------------------------------------------------
      // Does it support error frames?
      //
      if(pclNetworkT->hasErrorFramesSupport())
      {
         ui.pclCkbNetErrorFramesM->setEnabled(true);
         ui.pclCkbNetErrorFramesM->setChecked(pclNetworkT->isErrorFramesEnabled());
      }
      else
      {
         ui.pclCkbNetErrorFramesM->setChecked(false);
         ui.pclCkbNetErrorFramesM->setEnabled(false);
      }
      
      //--------------------------------------------------------
      // Does it support CAN-FD frames?
      //
      if(pclNetworkT->hasFastDataSupport())
      {
         ui.pclCkbNetCanFdM->setEnabled(true);
         ui.pclCkbNetCanFdM->setChecked(pclNetworkT->isFastDataEnabled());
      }
      else
      {
         ui.pclCkbNetCanFdM->setChecked(false);
         ui.pclCkbNetCanFdM->setEnabled(false);
      }

      //--------------------------------------------------------
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
      

      //--------------------------------------------------------
      // show current bitrate
      //
      ui.pclCbbNetBitrateM->setCurrentIndex(pclNetworkT->bitrate());
      
   }
}
