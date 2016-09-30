//============================================================================//
// File:          qcan_qui.hpp                                                //
// Description:   GUI to handle with plugins and test them                    //
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


#ifndef QCAN_GUI_H_
#define QCAN_GUI_H_

#include <QWidget>
#include <QCanInterface>

QT_BEGIN_NAMESPACE
class QString;
class QLineEdit;
class QLabel;
class QComboBox;
class QPushButton;
class QGridLayout;
QT_END_NAMESPACE

//! [0]
class QCanGui : public QWidget
{
    Q_OBJECT

public:
    QCanGui();

public slots:


private slots:
    uint32_t parmValue(QStringList clCmdV, uint8_t ubParmNrV);
    void triggerCmd();
    void changePlugin(int slIdxV);

private:
    void createGUI();
    bool loadPlugin();

    QCanInterface *qCanInterface;
    QList<QString> aclDirListP;
    QLineEdit *lineEdit;
    QComboBox *comboBox;
    QGridLayout *layout;
};
//! [0]

#endif
