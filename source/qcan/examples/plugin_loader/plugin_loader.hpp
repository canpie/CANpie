//============================================================================//
// File:          plugin_loader.hpp                                           //
// Description:   Example application to load CANpie plugins                  //
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


#ifndef PLUGIN_LOADER_H_
#define PLUGIN_LOADER_H_


//-----------------------------------------------------------------------------
/*!
** \file    plugin_loader.hpp
** \brief   Plugin Loader class
**
** This example plugin loader should show how to load a CANpie plugin.
** Also it provide a simple possibility to execute some functions of
** loaded plugin. The output is done via qDebug() api.
*/


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <QWidget>
#include <QCanInterface>
#include <QCanPlugin>
#include <QList>

QT_BEGIN_NAMESPACE
class QString;
class QLineEdit;
class QLabel;
class QComboBox;
class QPushButton;
class QGridLayout;
QT_END_NAMESPACE


//-----------------------------------------------------------------------------
/*!
** \class   PluginLoader
*/
class PluginLoader : public QWidget
{
    Q_OBJECT

public:
    PluginLoader();

public slots:


private slots:
    uint32_t parmValue(QStringList clCmdV, uint8_t ubParmNrV);
    void triggerCmd();
    void changePlugin(int slIdxV);

private:
    void createGUI();
    bool loadPlugin();

    QCanInterface *qCanInterfaceP;
    QCanPlugin *qCanPluginP;
    QList<QString> aclDirListP;
    QLineEdit *lineEditP;
    QComboBox *comboBoxP;
    QGridLayout *layoutP;
};

#endif /*PLUGIN_LOADER_H_*/
