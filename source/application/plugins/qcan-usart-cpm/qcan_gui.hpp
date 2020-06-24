//============================================================================//
// File:          mcg_can.hpp                                                 //
// Description:   MCG plugin for CAN Protocol stacks                          //
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


#ifndef MCG_CAN_H_
#define MCG_CAN_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include <QObject>
#include <QtPlugin>
#include <QLibrary>
#include <QVariant>
#include <QDebug>
//#include <McgPluginCan>

// add support for QML
//#include <QQuickView>
//#include <QQuickItem>
//#include <QQmlContext>
//#include <QQmlProperty>


/*!
 * \brief The McgCan class
 * This class is used as plugin interface to the main
 * Matroska Code Generator application.
 * All required functions for implementation are declared in
 * #McgPluginCan and #McgPlugin.
 * Typically this class does not have any plugin specific implementations.
 * Such functionality is porovided by Cos target implementation classes
 * like #Cos
 */
 
//----------------------------------------------------------------------------//
// McgCan                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
class McgCan : public McgPluginCan
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID McgPluginCan_iid FILE "plugin.json")
    Q_INTERFACES(McgPluginCan)

private:
   /*!
    * \brief clSourceDirP
    * This value holds target root dirctory for source files
    */
   QDir clSourceDirP;

   /*!
    * \brief pclViewP
    * This pointer references to the view provided by main application
    */
   QQuickView *pclViewP;

   qint32 slProfileNumberP;
   QWidget *pclViewWidgedContainerP;

public:
   /*!
    * \brief McgCan
    */
   McgCan();
   
      /*!
    * \brief ~McgCan
    */
   ~McgCan();

   void loadProfiles(QDir) Q_DECL_OVERRIDE;
   qint32 profileNumber(void) Q_DECL_OVERRIDE;

	// function set provided by McgPlugin
   QIcon    icon(void) Q_DECL_OVERRIDE;
   QString  iconUrl(void) Q_DECL_OVERRIDE;
   QString  name(void) Q_DECL_OVERRIDE;
   QString  version(void) Q_DECL_OVERRIDE;
   bool     setSourceDir(QDir) Q_DECL_OVERRIDE;
   void     setView(QQuickView *pclViewV) Q_DECL_OVERRIDE;
   void     setVisible(bool btVisibleV) Q_DECL_OVERRIDE;
   bool     saveCode(const QString &clPathR, bool btEditFilesV) Q_DECL_OVERRIDE;

signals:
	// signals set provided by McgPlugin
   void log(McgLog_e,QString) Q_DECL_OVERRIDE;

public slots:
   // public slots set provided by McgPlugin
   void loadProject(QFile &) Q_DECL_OVERRIDE;
   void saveProject(QFile &) Q_DECL_OVERRIDE;
   void onSelect(bool) Q_DECL_OVERRIDE;
};

#endif /*MCG_CAN_H_*/

