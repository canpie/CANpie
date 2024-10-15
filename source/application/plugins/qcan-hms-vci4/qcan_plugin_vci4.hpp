//====================================================================================================================//
// File:          qcan_plugin_vci4.hpp                                                                                //
// Description:   CAN plugin for VCI4 devices                                                                         //
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


#ifndef QCAN_PLUGIN_VCI4_HPP_
#define QCAN_PLUGIN_VCI4_HPP_


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QFile>
#include <QtCore/QLibrary>
#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtCore/QString>
#include <QtCore/QDebug>

#include <QtWidgets/QWidget>

#include "qcan_plugin.hpp"

#include "qcan_interface_vci4.hpp"
#include "vcisdk.h"

//----------------------------------------------------------------------------------------------------------------------
/*!
** \class   QCanPluginVci4
** \brief   HMS VCI
** 
*/
class QCanPluginVci4 : public QCanPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QCanPlugin_iid FILE "plugin.json")
    Q_INTERFACES(QCanPlugin)

private:

   /*!
    * \brief Hold information about actual connected devices.
    */
   QVector<VCIDEVICEINFO> aclCanDevInfoP;

   /*!
    * \brief Log file for low level inforations about loaded and used devices
    */
   QFile *pclLogFileP;

private slots:
   void log(QString);

public:
   QCanPluginVci4();
   ~QCanPluginVci4();

   QCanPluginVci4(const QCanPluginVci4&) = delete;               // no copy constructor
   QCanPluginVci4& operator=(const QCanPluginVci4&) = delete;    // no assignment operator
   QCanPluginVci4(QCanPluginVci4&&) = delete;                    // no move constructor
   QCanPluginVci4& operator=(QCanPluginVci4&&) = delete;         // no move operator

   QIcon           icon(void) Q_DECL_OVERRIDE;
   uint8_t         interfaceCount(void) Q_DECL_OVERRIDE;
   QCanInterface * getInterface(uint8_t ubInterfaceV) Q_DECL_OVERRIDE;
   QString         name(void) Q_DECL_OVERRIDE;
};

#endif /*QCAN_PLUGIN_VCI4_HPP_*/
