//============================================================================//
// File:          qcan_plugin_peak.hpp                                        //
// Description:   CAN plugin for PEAK devices                                 //
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

#ifndef QCAN_PLUGIN_PEAK_HPP_
#define QCAN_PLUGIN_PEAK_HPP_

#include <QtCore/QLibrary>
#include <QtCore/QObject>
#include <QtCore/QtPlugin>

#include <QtWidgets/QWidget>

#include "qcan_plugin.hpp"

#include "qcan_pcan_basic.hpp"
#include "qcan_interface_peak.hpp"

//-----------------------------------------------------------------------------
/*!
** \class   QCanPluginPeak
** \brief   Peak PCAN
**
*/

class QCanPluginPeak : public QCanPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QCanPlugin_iid FILE "plugin.json")
    Q_INTERFACES(QCanPlugin)

private:

   /*!
    * \brief auwPCanChannelP
    * Contains all PCAN Channels that should be handled
    * by this plugin.
    */
   QList<uint16_t> auwPCanChannelP;

   /*!
    * \brief Paramter to indentify an interface
    */
   typedef struct {
      uint16_t uwPCanChannel;
      QCanInterfacePeak * pclQCanInterfacePeak;
   } QCanIf_ts;

   /*!
    * \brief atsQCanIfPeakP
    * Contains list of available QCAN Interfaces
    */
   QList<QCanIf_ts> atsQCanIfPeakP;

   /*!
    * \brief pclPcanBasicP
    * Reference to the static PCAN Basic lib
    */
   QCanPcanBasic &pclPcanBasicP = QCanPcanBasic::instance();

public:
   QCanPluginPeak();
   ~QCanPluginPeak() override;

   QCanPluginPeak(const QCanPluginPeak&) = delete;               // no copy constructor
   QCanPluginPeak& operator=(const QCanPluginPeak&) = delete;    // no assignment operator
   QCanPluginPeak(QCanPluginPeak&&) = delete;                    // no move constructor
   QCanPluginPeak& operator=(QCanPluginPeak&&) = delete;         // no move operator

   QIcon          icon(void) override;
   uint8_t        interfaceCount(void) override;
   QCanInterface  *getInterface(uint8_t ubInterfaceV) override;
   QString        name(void) override;
};

#endif /*QCAN_PLUGIN_PEAK_HPP_*/
