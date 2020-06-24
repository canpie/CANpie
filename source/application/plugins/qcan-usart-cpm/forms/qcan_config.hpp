//****************************************************************************//
// File:          qcan_config.hpp                                             //
// Description:   Additional Device Configuration QCAN interface              //
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

#ifndef QCAN_CONFIG_HPP_
#define QCAN_CONFIG_HPP_

#include <QDialog>
#include <QString>
#include <QSerialPortInfo>
#include <QDebug>
#include "ui_qcan_config.h"
#include "qcan_usart.hpp"

/*----------------------------------------------------------------------------*\
** Prototypes                                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/
class QCanConfig : public QDialog
{
   Q_OBJECT

   public:
      QCanConfig(QCanUsart::QCanUsartConfig_ts &tsConfigurationR, QWidget *parent = Q_NULLPTR);

   private:
      Ui::QCanCfgGui clQCanCfgGuiP;
      QCanUsart::QCanUsartConfig_ts tsCurrentConfigP;

  public slots:
      QCanUsart::QCanUsartConfig_ts &currentConfig(void);

};

#endif /* QCAN_CONFIG_HPP_ */
