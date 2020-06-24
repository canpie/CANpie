//============================================================================//
// File:          qcan_main.cpp                                               //
// Description:   Main of application                                         //
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


/*!
 * \brief Standalone application of QCan plugin
 * This file will be only compiled if 'TEMPLATE = app' is set in project file.
 * Intention of this file is to run QCan plugin as standalone application
 * to test functions and operation of GUI. 
 *
 */

/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include <QApplication>
#include "qcan_interface_usart.hpp"
#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>

//----------------------------------------------------------------------------//
// main                                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
int main(int argc, char *argv[])
{
   QApplication a(argc, argv);

   //----------------------------------------------------------------
   // get all available COM devices
   //
   QStringList clDeviceListT;
   clDeviceListT.clear();
   foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
   {
      qDebug() << "Found Port: " << info.portName();

      clDeviceListT.append(info.portName());
   }

   //----------------------------------------------------------------
   // create GUI for COM device selection
   //
   QDialog *pclDevSelectionGuiT = new QDialog;
   pclDevSelectionGuiT->resize(400, 240);
   QComboBox *pclComboBox_Device;
   pclComboBox_Device = new QComboBox(pclDevSelectionGuiT);
   pclComboBox_Device->setGeometry(QRect(60, 70, 101, 20));
   pclComboBox_Device->insertItems(0,clDeviceListT);
   QDialogButtonBox *pclButtonBoxT;
   pclButtonBoxT = new QDialogButtonBox(pclDevSelectionGuiT);
   pclButtonBoxT->setGeometry(QRect(220, 210, 171, 32));
   pclButtonBoxT->setOrientation(Qt::Horizontal);
   pclButtonBoxT->setStandardButtons(QDialogButtonBox::Ok);
   QObject::connect(pclButtonBoxT, SIGNAL(accepted()), pclDevSelectionGuiT, SLOT(accept()));
   pclDevSelectionGuiT->exec();

   //----------------------------------------------------------------
   // check result of selection
   //
   if (pclDevSelectionGuiT->result() == QDialog::Accepted)
   {
      //--------------------------------------------------------
      // call Device Specific Configuration GUI
      //
      QCanInterfaceUsart *pclQCanInterfaceT = new QCanInterfaceUsart(quint16(pclComboBox_Device->currentIndex()),
                                                                     pclComboBox_Device->currentText());

      // make minimal initialisation of QCAn plugin
      pclQCanInterfaceT->connect();
      pclQCanInterfaceT->setBitrate(0,0);

      // call for new config
      pclQCanInterfaceT->configureDevice();
   }

   return a.exec();
}
