//====================================================================================================================//
// File:          qcan_plugin.hpp                                                                                     //
// Description:   QCAN classes - CAN plug-in class                                                                    //
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




#ifndef QCAN_PLUGIN_HPP_
#define QCAN_PLUGIN_HPP_

/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QObject>
#include <QtGui/QIcon>


#include "qcan_interface.hpp"



//----------------------------------------------------------------------------------------------------------------
/*!
** \class QCanPlugin
** \brief CAN plug-in representation
**
** The QCanPlugin class is the base class for CAN interface plug-ins, which can be used to add any kind of
** CAN interface cards to the \ref canpie_server.
** <p>
** For an example of the QCanPlugin class refer to the QCanPluginTemplate class located inside the
** \c source/applications/plugins/qcan_template directory.
** 
*/
class QCanPlugin : public QObject
{
   Q_OBJECT

public:

   QCanPlugin();
   
   QCanPlugin(const QCanPlugin &) = delete;            
   QCanPlugin& operator=(const QCanPlugin &) = delete;

   QCanPlugin(QCanPlugin&&) = delete;                    // no move constructor
   QCanPlugin& operator=(QCanPlugin&&) = delete;         // no move operator

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubInterfaceV   Number of CAN interface
   **
   ** \return     CAN interface class
   **
   ** The function returns a pointer to a QCanInterface class defined by the parameter \a  ubInterfaceV.
   ** The first interface starts at index 0. If the number of available interfaces is exceeded the
   ** function returns NULL.
   */
   virtual QCanInterface * getInterface(uint8_t ubInterfaceV = 0) = 0;

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Icon of plug-in
   **
   ** The function returns the icon of the plug-in.
   */
   virtual QIcon           icon(void) = 0;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Number of CAN interfaces
   **
   ** The function returns the number of CAN interfaces that are available.
   */
   virtual uint8_t         interfaceCount(void) = 0;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Name of plug-in
   **
   ** The function returns the name of the plug-in.
   */
   virtual QString         name(void) = 0;

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clFileNameV    Log file name
   ** \return     \c true if file could be opened, otherwise \c false
   **
   ** The function changes the default file path and file name for the log attached to the CAN
   ** channel defined by the parameter \a teChannelV.
   */
   bool                    setFileName(const QString clFileNameV);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teLogLevelV       Log level
   ** \see        logLevel()
   **
   ** The function changes the default log level for the plugin.
   */
   inline void             setLogLevel(QCan::LogLevel_e teLogLevelV)    {  teLogLevelP = teLogLevelV; }

public slots:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clLogMessageV     Log message
   ** \param[in]  teLogLevelV       Log level
   **
   ** The function function appends a log message to the log file. The message will be prepended
   ** with a time-stamp. The parameter \c teLogLevelV defines the log level of the message.
   ** Only messages with a log level smaller or equal to the actual defined log level will be appended
   ** (see setLogLevel()).
   */
   virtual void            appendMessage(const QString & clLogMessageV,
                                         const QCan::LogLevel_e teLogLevelV = QCan::eLOG_LEVEL_INFO);

private:

   QDateTime               clTimeP;
   QString                 clLogMessageP;
   std::unique_ptr<QFile>  pclLogFileP;
   QCan::LogLevel_e        teLogLevelP;

};

#define QCanPlugin_iid "net.microcontrol.Qt.qcan.QCanPlugin"
Q_DECLARE_INTERFACE(QCanPlugin, QCanPlugin_iid)


#endif /*QCAN_PLUGIN_HPP_*/
