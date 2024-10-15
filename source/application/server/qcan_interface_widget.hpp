//====================================================================================================================//
// File:          qcan_interface_widget.hpp                                                                           //
// Description:   Widget class for QCanInterface                                                                      //
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


#ifndef QCAN_INTERFACE_WIDGET_HPP_
#define QCAN_INTERFACE_WIDGET_HPP_

/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QDir>
#include <QtGui/QIcon>
#include <QtWidgets/QMenu>
#include <QtWidgets/QWidget>

#include <QCanInterface>
#include <QCanPlugin>

//-----------------------------------------------------------------------------
/*!
** \class QCanInterfaceWidget
** \brief Widget for QCanInterface
**
** This class is a widget for a CAN interface.
*/
class QCanInterfaceWidget : public QWidget
{
   Q_OBJECT

public:

   #define QCAN_IF_VCAN_NAME "Virtual CAN bus"
   #define QCAN_IF_VCAN_ICON ":images/mc_network_vcan_256.png"

   QCanInterfaceWidget(uint8_t ubIdxV);

   QCanInterfaceWidget(const QCanInterfaceWidget&) = delete;               // no copy constructor
   QCanInterfaceWidget& operator=(const QCanInterfaceWidget&) = delete;    // no assignment operator
   QCanInterfaceWidget(QCanInterfaceWidget&&) = delete;                    // no move constructor
   QCanInterfaceWidget& operator=(QCanInterfaceWidget&&) = delete;         // no move operator

   /*!
    * \brief setIcon
    * \param clIconR - Icon
    */
   void setIcon(QIcon clIconR, QIcon::Mode teModeV = QIcon::Normal);
   void setPluginPath(QDir);

   /*!
    * \brief setInterface
    * \param clNameV - Name of a plugin
    * \return Returns true if given plugin could be set
    */
   bool setInterface(QString clNameV);

   /*!
    * \brief pluginName
    * \return Name of acutaly used plugin
    */
   QString name(void);

//   /*!
//    * \brief pluginChannel
//    * \return Selected channel number of plugin
//    */
//   uint8_t pluginChannel(void);

private:

   uint8_t  ubInterfaceIdxP;
   QIcon    clIconP;
   QDir     clPluginPathP;

   /*!
    * \brief qCanInterfaceP
    * This pointer points to the actual selected interface.
    * Otherwise this value is set to NULL.
    */
   QCanInterface *pclQCanInterfaceP;


   QList<QCanPlugin *> apclQCanPluginP;

protected:
   void mousePressEvent(QMouseEvent *event) override;
   void paintEvent(QPaintEvent *event) override;
   bool loadPlugin(void);

private slots:

public slots:



signals:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubChannelR  CAN channel
   ** \param[in]  clMessageR  Logging message
   ** \param[in]  teLogLevelR Logging level
   **
   ** This signal is emitted by the CAN network to inform the application about status changes or
   ** error conditions.
   */
   void  addLogMessage(const QCan::CAN_Channel_e & ubChannelR,
                       const QString & clMessageR, const QCan::LogLevel_e & teLogLevelR = QCan::eLOG_LEVEL_WARN);

   void clicked(bool checked = false );

   void interfaceChanged(const QCan::CAN_Channel_e & ubChannelR, QCanInterface * pclInterfaceV);

};


#endif // QCAN_INTERFACE_WIDGET_HPP_
