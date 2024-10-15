//====================================================================================================================//
// File:          qcan_socket_dialog.hpp                                                                              //
// Description:   QCAN classes - CAN socket dialog                                                                    //
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

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QTime>

#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>


#include "qcan_server_settings.hpp"
#include "qcan_socket_dialog.hpp"


enum SocketHost_e {
   eSocketHostLocal = 0,
   eSocketHostRemote
};


class QCanSocketDialogPrivate : public QObject
{
   
public:
   QCanSocketDialogPrivate(void) {}

   
   void                 createWidgets(QCanSocketDialog * pclWidgetV);
   void                 connectSlots(QCanSocketDialog * pclWidgetV);
   QCan::CAN_Channel_e  channel(void);
   
   void                 doConnectionWait();
   void                 enableButtonOk(bool btEnableV = true);
   void                 enableConnectionWait(bool btEnableV = true);
   void                 enableHost(bool btEnableV = true);
   void                 setChannel(const QCan::CAN_Channel_e teChannelV);
   void                 setHostAddress(const QHostAddress clHostAddressV);
   void                 showInfoText(const QString clTextV);
   void                 translate(QCanSocketDialog * pclWidgetV);
   

   
private:
   QDialogButtonBox *pclButtonBoxP;
   QGroupBox *       pclGroupBoxP;
   QLabel *          pclLblHostP;
   QComboBox *       pclCbxHostP;
   QLabel *          pclLblHostAddressP;
   QLineEdit *       pclEdtHostAddressP;
   QLabel *          pclLblCanChannelP;
   QComboBox *       pclCbxCanChannelP;
   QLabel *          pclLblCanDetailP;
   QLabel *          pclLblCanInfoP;
   QProgressBar *    pclBarConnectWaitP;
};


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialogPrivate::createWidgets()                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialogPrivate::createWidgets(QCanSocketDialog * pclWidgetV)
{
   if (pclWidgetV->objectName().isEmpty())
   {
      pclWidgetV->setObjectName(QStringLiteral("QCanSocketDialog"));
   }
   
   //---------------------------------------------------------------------------------------------------
   // set size
   //
   pclWidgetV->setFixedSize(380, 251);

   //---------------------------------------------------------------------------------------------------
   // set buttons
   //
   pclButtonBoxP = new QDialogButtonBox(pclWidgetV);
   pclButtonBoxP->setObjectName(QStringLiteral("pclButtonBoxP"));
   pclButtonBoxP->setGeometry(QRect(10, 210, 361, 32));
   pclButtonBoxP->setOrientation(Qt::Horizontal);
   pclButtonBoxP->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

   pclGroupBoxP = new QGroupBox(pclWidgetV);
   pclGroupBoxP->setObjectName(QStringLiteral("pclGroupBoxP"));
   pclGroupBoxP->setGeometry(QRect(10, 10, 361, 191));
   
   //---------------------------------------------------------------------------------------------------
   // set label and combo-box for "Connect to: <remote/local>"
   //
   pclLblHostP = new QLabel(pclGroupBoxP);
   pclLblHostP->setObjectName(QStringLiteral("pclLblHostP"));
   pclLblHostP->setGeometry(QRect(10, 10, 140, 20));
   pclLblHostP->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
 
   pclCbxHostP = new QComboBox(pclGroupBoxP);
   pclCbxHostP->setObjectName(QStringLiteral("pclCbxHostP"));
   pclCbxHostP->setGeometry(QRect(170, 10, 150, 26));

   //---------------------------------------------------------------------------------------------------
   // set label and line-edit for "IPv4-Address: 000.000.000.000"
   //
   pclLblHostAddressP = new QLabel(pclGroupBoxP);
   pclLblHostAddressP->setObjectName(QStringLiteral("pclLblHostAddressP"));
   pclLblHostAddressP->setGeometry(QRect(10, 40, 140, 20));
   pclLblHostAddressP->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
   pclLblHostAddressP->hide();
   
   pclEdtHostAddressP = new QLineEdit(pclGroupBoxP);
   pclEdtHostAddressP->setObjectName(QStringLiteral("pclEdtHostAddressP"));
   pclEdtHostAddressP->setGeometry(QRect(170, 42, 145, 21));
   pclEdtHostAddressP->setInputMask("000.000.000.000");
   pclEdtHostAddressP->hide();

   QFont clFontT;
   clFontT.setFamily(QStringLiteral("Courier"));
   pclEdtHostAddressP->setFont(clFontT);
   pclEdtHostAddressP->setAlignment(Qt::AlignCenter);
   
   
   //---------------------------------------------------------------------------------------------------
   // set label and combo-box for "CAN channel: <CAN 1/ CAN 8>"
   //
   pclLblCanChannelP = new QLabel(pclGroupBoxP);
   pclLblCanChannelP->setObjectName(QStringLiteral("pclLblCanChannelP"));
   pclLblCanChannelP->setGeometry(QRect(10, 70, 140, 20));
   pclLblCanChannelP->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

   pclCbxCanChannelP = new QComboBox(pclGroupBoxP);
   pclCbxCanChannelP->setObjectName(QStringLiteral("pclCbxCanChannelM"));
   pclCbxCanChannelP->setGeometry(QRect(170, 70, 150, 26));
   pclCbxCanChannelP->clear();
   for (uint8_t ubCanChannelT = QCan::eCAN_CHANNEL_1; ubCanChannelT <= QCAN_NETWORK_MAX; ubCanChannelT++)
   {
      QString clNameT = "CAN " + QString("%1").arg(ubCanChannelT);
      pclCbxCanChannelP->addItem(clNameT, ubCanChannelT);
   }

   //---------------------------------------------------------------------------------------------------
   // set label and text field for "CAN details: <..>"
   //
   pclLblCanDetailP = new QLabel(pclGroupBoxP);
   pclLblCanDetailP->setObjectName(QStringLiteral("pclLblCanDetailP"));
   pclLblCanDetailP->setGeometry(QRect(10, 100, 140, 20));
   pclLblCanDetailP->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

   pclLblCanInfoP = new QLabel(pclGroupBoxP);
   pclLblCanInfoP->setObjectName(QStringLiteral("pclLblCanInfoP"));
   pclLblCanInfoP->setGeometry(QRect(170, 102, 148, 60));
   pclLblCanInfoP->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
   
   pclBarConnectWaitP = new QProgressBar(pclGroupBoxP);
   pclBarConnectWaitP->setObjectName(QStringLiteral("pclBarConnectWaitP"));
   pclBarConnectWaitP->setGeometry(QRect(170, 120, 141, 23));
   pclBarConnectWaitP->setRange(0, 100);
   pclBarConnectWaitP->setValue(0);
   pclBarConnectWaitP->setTextVisible(false);
   pclBarConnectWaitP->hide();
   
   //---------------------------------------------------------------------------------------------------
   // set the tab order
   //
   QWidget::setTabOrder(pclCbxHostP, pclEdtHostAddressP);
   QWidget::setTabOrder(pclEdtHostAddressP, pclCbxCanChannelP);


   
   //---------------------------------------------------------------------------------------------------
   // configure signal / slots
   //
   QObject::connect(pclButtonBoxP, SIGNAL(accepted()), pclWidgetV, SLOT(accept()));
   QObject::connect(pclButtonBoxP, SIGNAL(rejected()), pclWidgetV, SLOT(reject()));

   QMetaObject::connectSlotsByName(pclWidgetV);

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialogPrivate::channel()                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCan::CAN_Channel_e QCanSocketDialogPrivate::channel(void)
{
   return (static_cast< QCan::CAN_Channel_e >(pclCbxCanChannelP->currentData().toInt()));
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialogPrivate::connectSlots()                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialogPrivate::connectSlots(QCanSocketDialog * pclWidgetV)
{
   connect( pclCbxHostP, SIGNAL(currentIndexChanged(int)),        pclWidgetV, SLOT(onHostChanged(int)));
   
   connect( pclCbxCanChannelP, SIGNAL(currentIndexChanged(int)),  pclWidgetV, SLOT(onChannelChanged(int)));
   
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialogPrivate::enableButtonOk()                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialogPrivate::enableButtonOk(bool btEnableV)
{
   QPushButton * pclButtonOkT = pclButtonBoxP->button(QDialogButtonBox::Ok);

   pclButtonOkT->setEnabled(btEnableV);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialogPrivate::enableConnectionWait()                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialogPrivate::enableConnectionWait(bool btEnableV)
{
   if (btEnableV == true)
   {
      pclBarConnectWaitP->show();
   }
   else
   {
      pclBarConnectWaitP->hide();
      pclBarConnectWaitP->reset();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialogPrivate::enableHost()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialogPrivate::enableHost(bool btEnableV)
{
   if (btEnableV)
   {
      pclLblHostAddressP->show();
      pclEdtHostAddressP->show();
   }
   else
   {
      pclLblHostAddressP->hide();
      pclEdtHostAddressP->hide();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialogPrivate::setHostAddress()                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialogPrivate::doConnectionWait()
{
   if (pclBarConnectWaitP->value() <  pclBarConnectWaitP->maximum())
   {
      pclBarConnectWaitP->setValue(pclBarConnectWaitP->value() + 1);      
   }
   else
   {
      pclBarConnectWaitP->setValue(pclBarConnectWaitP->minimum());
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialogPrivate::setChannel()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialogPrivate::setChannel(const QCan::CAN_Channel_e teChannelV)
{
   pclCbxCanChannelP->setCurrentIndex(pclCbxCanChannelP->findData(teChannelV));
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialogPrivate::setHostAddress()                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialogPrivate::setHostAddress(const QHostAddress clHostAddressV)
{
   if (clHostAddressV == QHostAddress::LocalHost)
   {
      enableHost(false);
   }
   else
   {
      enableHost(true);
      pclEdtHostAddressP->setText(clHostAddressV.toString());
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialogPrivate::showInfoText()                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialogPrivate::showInfoText(const QString clTextV)
{
   pclLblCanInfoP->setText(clTextV);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialogPrivate::translate()                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialogPrivate::translate(QCanSocketDialog * pclWidgetV)
{
   pclWidgetV->setWindowTitle(tr("Connect to CAN interface"));
   pclLblHostP->setText(tr("Connect to:"));
   pclLblHostAddressP->setText(tr("IPv4-Address:"));
   pclLblCanChannelP->setText(tr("CAN channel:"));
   pclLblCanDetailP->setText(tr("CAN details:"));

   pclCbxHostP->clear();
   pclCbxHostP->addItem(tr("Local host"),    eSocketHostLocal);
   pclCbxHostP->addItem(tr("Remote server"), eSocketHostRemote);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog()                                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanSocketDialog::QCanSocketDialog(QWidget *parent, Qt::WindowFlags f) :
      QDialog(parent, f)
{
   pclSocketP = new QCanSocket();
   pclTimerP  = new QTimer();
   pclWidgetP = new QCanSocketDialogPrivate();

   clHostAddressP = QHostAddress::LocalHost;
   teChannelP     = QCan::eCAN_CHANNEL_1;
   btConnectFailP = true;

   pclWidgetP->createWidgets(this);
   pclWidgetP->connectSlots(this);
   pclWidgetP->translate(this);
   setModal(true);
   connectSlots();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog()                                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanSocketDialog::QCanSocketDialog(QWidget *parent, const QString &caption) :
      QDialog(parent)
{
   pclSocketP = new QCanSocket();
   pclTimerP  = new QTimer();
   pclWidgetP = new QCanSocketDialogPrivate();

   clHostAddressP = QHostAddress::LocalHost;
   teChannelP     = QCan::eCAN_CHANNEL_1;
   btConnectFailP = true;

   pclWidgetP->createWidgets(this);
   pclWidgetP->connectSlots(this);
   pclWidgetP->translate(this);
   this->setWindowTitle(caption);
   setModal(true);
   connectSlots();
}


//--------------------------------------------------------------------------------------------------------------------//
// ~QCanSocketDialog()                                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanSocketDialog::~QCanSocketDialog()
{
   disconnectSlots();
   delete (pclWidgetP);
   delete (pclTimerP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::accept()                                                                                         //
// This method is called when [OK] is clicked. It is called before done().                                            //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialog::accept(void)
{
   QDialog::accept();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::channel()                                                                                        //
// get selected channel number                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCan::CAN_Channel_e QCanSocketDialog::channel() const
{
   return (teChannelP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::connect()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialog::connect()
{
   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanSocketDialog::connect()";
   #endif

   pclSocketP->disconnectNetwork();

   pclWidgetP->showInfoText(tr("Waiting for connection .."));
   pclWidgetP->enableConnectionWait();
   pclTimerP->start(20);

   pclSocketP->connectNetwork(pclWidgetP->channel());
   
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::connectSlots()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialog::connectSlots(void)
{
   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanSocketDialog::connectSlots()";
   #endif

   QObject::connect( pclTimerP, SIGNAL(timeout()), 
                     this, SLOT(onConnectionTimer()));

   //---------------------------------------------------------------------------------------------------
   // connect signals for socket operations
   //
   QObject::connect(pclSocketP, SIGNAL(connected()),
                    this, SLOT(onSocketConnected()));

   QObject::connect(pclSocketP, SIGNAL(disconnected()),
                    this, SLOT(onSocketDisconnected()));
   
   QObject::connect(pclSocketP, SIGNAL(error(QAbstractSocket::SocketError)),
                    this, SLOT(onSocketError(QAbstractSocket::SocketError)));

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::disconnectSlots()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialog::disconnectSlots(void)
{
   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanSocketDialog::disconnectSlots()";
   #endif

   //---------------------------------------------------------------------------------------------------
   // disconnect signals for socket operations
   //
   if (pclSocketP != nullptr)
   {
      QObject::disconnect(pclSocketP, SIGNAL(connected()),
                          this, SLOT(onSocketConnected()));

      QObject::disconnect(pclSocketP, SIGNAL(disconnected()),
                          this, SLOT(onSocketDisconnected()));

      QObject::disconnect(pclSocketP, SIGNAL(error(QAbstractSocket::SocketError)),
                          this, SLOT(onSocketError(QAbstractSocket::SocketError)));
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::done()                                                                                           //
// This method is called when [OK] or [Cancel] is clicked. Possible values for slResultV: QDialog::Accepted or        //
// QDialog::Rejected                                                                                                  //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialog::done(int slResultV)
{
   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanSocketDialog::done()" << slResultV;
   #endif

   //---------------------------------------------------------------------------------------------------
   // First test if the connection to a CANpie server failed
   //
   if (btConnectFailP == true)
   {
      delete (pclSocketP);
      pclSocketP.clear();
      teChannelP = QCan::eCAN_CHANNEL_NONE;
   }
   else
   {
      //-------------------------------------------------------------------------------------------
      // Connection to a CANpie server was successful
      //
      if (slResultV == QDialog::Accepted)
      {
         emit socketSelected(pclSocketP);
      }
      else
      {
         //-----------------------------------------------------------------------------------
         // Cancel has been pressed:
         // - disconnect from network
         // - delete the QCanSocket instance
         // - set channel number to eCAN_CHANNEL_NONE
         //
         pclSocketP->disconnectNetwork();
         delete (pclSocketP);
         pclSocketP.clear();
         teChannelP = QCan::eCAN_CHANNEL_NONE;
      }
   }

   QDialog::done(slResultV);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::onChannelChanged()                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialog::onChannelChanged(int slIndexV)
{
   Q_UNUSED(slIndexV);

   teChannelP = pclWidgetP->channel();
   this->connect();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::onConnectionTimer()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialog::onConnectionTimer()
{
   pclWidgetP->doConnectionWait();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::onHostChanged()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialog::onHostChanged(int slIndexV)
{
   if (slIndexV == eSocketHostLocal)
   {
      pclWidgetP->enableHost(false);
   }
   else
   {
      pclWidgetP->enableHost(true);
   }
   
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::onSocketConnected()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialog::onSocketConnected(void)
{
   QByteArray        clCanDataT;
   QString           clCanInfoT;

   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanSocketDialog::onSocketConnected()";
   #endif

   pclTimerP->stop();
   pclWidgetP->enableConnectionWait(false);

   clCanInfoT  = tr("Connected\n");
   QCanServerSettings   clSettingsT;
   //clCanInfoT += clSettingsT.networkName(teChannelP);
   //clCanInfoT += "\n";
   //clCanInfoT += clSettingsT.networkNominalBitrate(teChannelP);

   pclWidgetP->showInfoText(clCanInfoT);
   pclWidgetP->enableButtonOk(true);

   btConnectFailP = false;


   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanSocketDialog::onSocketConnected() - done";
   #endif
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::onSocketDisconnected()                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialog::onSocketDisconnected(void)
{
   pclWidgetP->showInfoText(tr("Disconnected."));

   btConnectFailP = true;

   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanSocketDialog::onSocketDisconnected()";
   #endif
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::onSocketError()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialog::onSocketError(QAbstractSocket::SocketError teSocketErrorV)
{
   Q_UNUSED(teSocketErrorV);
   pclTimerP->stop();
   pclWidgetP->enableConnectionWait(false);
   pclWidgetP->showInfoText(tr("Failed to connect."));
   pclWidgetP->enableButtonOk(false);

   btConnectFailP = true;

   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanSocketDialog::onSocketError()";
   #endif
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::peerAddress()                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QHostAddress QCanSocketDialog::peerAddress() const
{
   QHostAddress clPeerAddressT = QHostAddress::LocalHost;

   return clPeerAddressT;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::show()                                                                                           //
// Show the dialog and try to connect to the server                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
int QCanSocketDialog::exec()
{
   this->connect();

   return QDialog::exec();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::socket()                                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanSocket * QCanSocketDialog::socket(void) const
{
   return (pclSocketP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::setChannel()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialog::setChannel(const QCan::CAN_Channel_e teChannelV)
{
   pclWidgetP->setChannel(teChannelV);
   teChannelP = teChannelV;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSocketDialog::setHostAddress()                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSocketDialog::setHostAddress(const QHostAddress clHostAddressV)
{
   pclWidgetP->setHostAddress(clHostAddressV);
}
