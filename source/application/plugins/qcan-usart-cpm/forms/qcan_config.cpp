//****************************************************************************//
// File:          qcan_config.cpp                                             //
// Description:   Additional Device Configuration QCAN interface               //
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

#include "qcan_config.hpp"

//----------------------------------------------------------------------------//
// QCanConfig                                                                 //
//                                                                            //
//----------------------------------------------------------------------------//
QCanConfig::QCanConfig(QCanUsart::QCanUsartConfig_ts &tsConfigurationR, QWidget *parent)
   : QDialog(parent)
{
   //-----------------------------------------------------------------
   // setup GUI
   //
   clQCanCfgGuiP.setupUi(this);

   tsCurrentConfigP = tsConfigurationR;

   //-----------------------------------------------------------------
   // initialise parameter of CAN Hardware
   //
   clQCanCfgGuiP.groupBox->setTitle(tsCurrentConfigP.clName);

   //----------------------------------------------------------------
   // check requested COM Port is available
   //
   QStringList clStringListT;
   clStringListT.clear();
   qint32 slCurrentIndexT = 0;
   foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
   {
      if (info.portName() == tsCurrentConfigP.clName)
      {
         foreach (const qint32 &infoBaud, QSerialPortInfo::standardBaudRates())
         {
            clStringListT.append(QString::number(infoBaud,10));

            if (tsCurrentConfigP.slBaud == qint32(infoBaud))
            {
               slCurrentIndexT = clStringListT.count()-1;
            }
         }
      }
   }

   //----------------------------------------------------------------
   // insert default baud rates and select current one
   //
   clStringListT.append(QString::number(1000000,10));
   if (tsCurrentConfigP.slBaud == qint32(1000000))
   {
      slCurrentIndexT = clStringListT.count()-1;
   }
   clStringListT.append(QString::number(2000000,10));
   if (tsCurrentConfigP.slBaud == qint32(2000000))
   {
      slCurrentIndexT = clStringListT.count()-1;
   }
   clStringListT.append(QString::number(3000000,10));
   if (tsCurrentConfigP.slBaud == qint32(3000000))
   {
      slCurrentIndexT = clStringListT.count()-1;
   }
   clQCanCfgGuiP.clComboBoxCOMBaud->insertItems(0,clStringListT);
   clQCanCfgGuiP.clComboBoxCOMBaud->setCurrentIndex(slCurrentIndexT);

   //----------------------------------------------------------------
   // insert default baud rates and select current one
   //
   clStringListT.clear();
   clStringListT << "8N1" << "8E1" << "8O1";
   clQCanCfgGuiP.clComboBoxCOMMode->insertItems(0,clStringListT);
   clQCanCfgGuiP.clComboBoxCOMMode->setCurrentIndex(tsCurrentConfigP.ubMode-4);
}

//----------------------------------------------------------------------------//
// currentConfig                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
QCanUsart::QCanUsartConfig_ts & QCanConfig::currentConfig()
{
   tsCurrentConfigP.ubMode = (clQCanCfgGuiP.clComboBoxCOMMode->currentIndex()+4);
   tsCurrentConfigP.slBaud = (clQCanCfgGuiP.clComboBoxCOMBaud->currentText().toInt());

   return tsCurrentConfigP;
}

