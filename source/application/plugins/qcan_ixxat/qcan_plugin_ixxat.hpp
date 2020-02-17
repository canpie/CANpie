//============================================================================//
// File:          qcan_plugin_ixxat.hpp                                       //
// Description:   CAN plugin for IXXAT devices                                //
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
// ("LGPL") version 3 as distributed in the 'COPYING' file.                   //
//                                                                            //
//============================================================================//


#ifndef QCAN_PLUGIN_IXXAT_H_
#define QCAN_PLUGIN_IXXAT_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include <QObject>
#include <QtPlugin>
#include <QLibrary>
#include <QCanPlugin>
#include <QtWidgets>

#include "qcan_ixxat_vci.hpp"
#include "qcan_interface_ixxat.hpp"


//-----------------------------------------------------------------------------
/*!
** \class   QCanPluginIxxat
** \brief   IXXAT VCI
** 
*/
class QCanPluginIxxat : public QCanPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QCanPlugin_iid FILE "plugin.json")
    Q_INTERFACES(QCanPlugin)

private:

   /*!
    * \brief clCanDevInfoP
    *  This value holds handle value for the device list.
    */
   HANDLE   vdDevListP;

   /*!
    * \brief clCanDevInfoP
    *  This struct holds information about selected device.
    */
   QVector<VCIDEVICEINFO> aclCanDevInfoP;

//   typedef struct {
//      HANDLE        clDev;
//      VCIDEVICEINFO clDevInfo;
//   };
   //----------------------------------------------------------------
   // List of available QCAN Interfaces
   //
//   QList<QCanInterfaceIxxat *> apclQCanInterfaceP;

   //----------------------------------------------------------------
   // Reference to the static PCAN Basic lib
   //
   QCanIxxatVci &pclIxxatVciP = QCanIxxatVci::getInstance();

public:
   QCanPluginIxxat();
   ~QCanPluginIxxat();

   QIcon           icon(void) Q_DECL_OVERRIDE;
   uint8_t         interfaceCount(void) Q_DECL_OVERRIDE;
   QCanInterface * getInterface(uint8_t ubInterfaceV) Q_DECL_OVERRIDE;
   QString         name(void) Q_DECL_OVERRIDE;
};

#endif /*QCAN_PLUGIN_IXXAT_H_*/
