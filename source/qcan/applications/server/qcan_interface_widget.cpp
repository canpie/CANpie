//====================================================================================================================//
// File:          qcan_interface_widget.cpp                                                                           //
// Description:   QCAN server - Widget for QCanInterface                                                              //
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


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include "qcan_interface_widget.hpp"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QPluginLoader>
#include <QtCore/QPoint>

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QPalette>

#include <QtWidgets/QMessageBox>


/*--------------------------------------------------------------------------------------------------------------------*\
** Class methods                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceWidget()                                                                                              //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterfaceWidget::QCanInterfaceWidget(uint8_t ubIdxV)
 : QWidget()
{
   ubInterfaceIdxP = ubIdxV;
   clIconP = QIcon(QCAN_IF_VCAN_ICON);
   pclQCanInterfaceP = NULL;

   //----------------------------------------------------------------
   // check there are any plugins available
   //
   QDir clPluginsDirT(qApp->applicationDirPath());
   #if defined(Q_OS_WIN)
   clPluginsDirT.setPath(clPluginsDirT.path() + "/plugins");
   #elif defined(Q_OS_MAC)
   if(clPluginsDirT.dirName() == "MacOS")
   {
      clPluginsDirT.cdUp();
      clPluginsDirT.setPath(clPluginsDirT.path() + "/PlugIns");
   }
   #endif
   clPluginPathP = clPluginsDirT;
   qDebug() << QString("QCanInterfaceWidget::QCanInterfaceWidget("+QString::number(ubIdxV) +","+ clPluginPathP.absolutePath()+")");

   loadPlugin();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceWidget::mousePressEvent()                                                                             //
// handle mouse press events on window areas                                                                          //
//--------------------------------------------------------------------------------------------------------------------//
void QCanInterfaceWidget::mousePressEvent(QMouseEvent * pclEventV)
{
   QAction *      pclActionT;
   QPoint         clPosT(this->mapFromParent(QCursor::pos()));
   QMenu          clContextMenuT(tr("CAN interface selection"), this);
   QMenu *        pclPluginMenuT;
   QCanInterface *pclInterfaceT;

   if (!loadPlugin())
   {
      qCritical() << "QCanInterfaceWidget::QCanInterfaceWidget() ERROR: Could not load any plugins!";
   }

   //---------------------------------------------------------------------------------------------------
   // create context menu with all available plugins
   //
   pclActionT = new QAction(QCAN_IF_VCAN_NAME, this);
   pclActionT->setIcon(QIcon(QCAN_IF_VCAN_ICON));
   clContextMenuT.addAction(pclActionT);

   //---------------------------------------------------------------------------------------------------
   // create menus for each plugin with corresponding interfaces
   //
   foreach (QCanPlugin *pclPluginT, apclQCanPluginP)
   {
      pclPluginMenuT = clContextMenuT.addMenu(pclPluginT->name());
      pclPluginMenuT->setIcon(pclPluginT->icon());

      for (uint8_t ubIfCntT = 0; ubIfCntT < pclPluginT->interfaceCount(); ubIfCntT++)
      {
         pclInterfaceT = pclPluginT->getInterface(ubIfCntT);

         //-----------------------------------------------------------------------------------
         // at this point interface could be used by an other client
         // don't put it in to the list as available
         //
         pclActionT = new QAction(pclInterfaceT->name(), this);
         pclActionT->setIcon(QIcon(pclInterfaceT->icon()));
         pclPluginMenuT->addAction(pclActionT);
      }
   }

   //---------------------------------------------------------------------------------------------------
   // evaluate click operation
   //
   switch(pclEventV->button())
   {
      case Qt::RightButton:

         //-----------------------------------------------------
         // show context menu
         //
         pclActionT = clContextMenuT.exec(clPosT);

         //-----------------------------------------------------
         // evaluate selection
         //
         if (pclActionT != 0)
         {
            qDebug() << "QCanInterfaceWidget::mousePressEvent(), process action:" << pclActionT->text();

            setInterface(pclActionT->text());
         }
         break;

      case Qt::LeftButton:
         qDebug() << "QCanInterfaceWidget::mousePressEvent(): left button pressed";

         break;

      default:

         break;
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceWidget::loadPlugin()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanInterfaceWidget::loadPlugin()
{
   QCanPlugin    *pclQCanPluginT;

   //----------------------------------------------------------------
   // reset list of plug-ins
   //
   apclQCanPluginP.clear();

   //----------------------------------------------------------------
   // check plug-in path
   //
   if (!clPluginPathP.exists())
   {
      qWarning() << "QCanInterfaceWidget::loadPlugin() WARNING: plugin path does not exist!";
      return false;
   }

   //----------------------------------------------------------------
   // create a list of plug-ins
   //
   foreach (QString clFileNameT, clPluginPathP.entryList(QDir::Files))
   {
      if (QLibrary::isLibrary(clPluginPathP.absoluteFilePath(clFileNameT)))
      {
         QPluginLoader clPluginLoaderT(clPluginPathP.absoluteFilePath(clFileNameT));
         QObject *pclPluginT = clPluginLoaderT.instance();
         if (pclPluginT)
         {
            pclQCanPluginT = qobject_cast<QCanPlugin *>(pclPluginT);
            if (pclQCanPluginT)
            {
               qInfo() << "QCanInterfaceWidget::loadPlugin() INFO: found" << clPluginPathP.absoluteFilePath(clFileNameT) << "plugin, which contains" << QString::number(pclQCanPluginT->interfaceCount()) << " interfaces.";

               apclQCanPluginP.append(pclQCanPluginT);
            }
         }
         else
         {
            qWarning() << "QCanInterfaceWidget::loadPlugin() WARNING: plugin" << clPluginPathP.absoluteFilePath(clFileNameT) << "could NOT be loaded or the root component object could NOT be instantiated!";
         }
      }
      else
      {
         qWarning() << "QCanInterfaceWidget::loadPlugin() WARNING: plugin" << clPluginPathP.absoluteFilePath(clFileNameT) << "is NOT a library!";
      }
   }

   //----------------------------------------------------------------
   // check list of plugins
   //
   if (apclQCanPluginP.isEmpty())
   {
      qWarning() << "QCanInterfaceWidget::loadPlugin() WARNING: No plugins have been found!";
      return false;
   }

   qInfo() << "QCanInterfaceWidget::loadPlugin() INFO: found" << QString::number(apclQCanPluginP.length(),10) << "available plugins.";

   return true;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceWidget::paintEvent()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanInterfaceWidget::paintEvent(QPaintEvent * pclEventV)
{
   QPalette clPaletteT(this->palette());
   QPainter clPainterT(this);
   QBrush brush = QBrush(Qt::transparent, Qt::NoBrush);
   clPainterT.setPen((Qt::white));
   clPainterT.setBrush(brush);
   clPainterT.fillRect(pclEventV->rect(), QColor(0xE3, 0xE3, 0xE3)); //Qt::NoBrush);

   clIconP.paint(&clPainterT, pclEventV->rect(), Qt::AlignCenter);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceWidget::name()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanInterfaceWidget::name()
{
   QString clNameT = QString(QCAN_IF_VCAN_NAME);
   bool    btIfConnectedT = false;

   if (pclQCanInterfaceP != NULL)
   {
      // store previous state of connection
      if (pclQCanInterfaceP->connectionState() == QCanInterface::ConnectedState)
      {
         btIfConnectedT = true;
      }

      if (!btIfConnectedT)
      {
         pclQCanInterfaceP->connect();
      }

      clNameT = pclQCanInterfaceP->name();

      if (!btIfConnectedT)
      {
         pclQCanInterfaceP->disconnect();
      }
   }

   return clNameT;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceWidget::setIcon()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanInterfaceWidget::setIcon(QIcon clIconR, QIcon::Mode teModeV)
{
   Q_UNUSED(teModeV);

   clIconP = clIconR;
   this->repaint();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceWidget::setInterface()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanInterfaceWidget::setInterface(QString clNameV)
{
   QString clInterfaceNameT = NULL;
   QCanInterface *pclInterfaceT;


   pclQCanInterfaceP = NULL;

   qDebug() << "QCanInterfaceWidget::setInterface(" << clNameV << ")";

   if (clNameV != QString(QCAN_IF_VCAN_NAME))
   {
      emit addLogMessage(CAN_Channel_e (eCAN_CHANNEL_1 + ubInterfaceIdxP),
                         "Search CAN interface ... : " + clNameV, eLOG_LEVEL_DEBUG);

      foreach (QCanPlugin * pclPluginT, apclQCanPluginP)
      {
         //-----------------------------------------------------------------------------------
         // if an interface has been found quit here
         //
         if (pclQCanInterfaceP != NULL)
         {
            break;
         }

         //-----------------------------------------------------------------------------------
         // check all interfaces of selected plug-in
         //
         for (uint8_t ubIfCntT = 0; ubIfCntT < pclPluginT->interfaceCount(); ubIfCntT++)
         {
            pclInterfaceT = pclPluginT->getInterface(ubIfCntT);
            clInterfaceNameT = pclInterfaceT->name();
            emit addLogMessage(CAN_Channel_e (eCAN_CHANNEL_1 + ubInterfaceIdxP),
                               "Found CAN interface .... : " + clInterfaceNameT, eLOG_LEVEL_DEBUG);
            if (clNameV == clInterfaceNameT)
            {
               emit addLogMessage(CAN_Channel_e (eCAN_CHANNEL_1 + ubInterfaceIdxP),
                                  "Connect CAN interface .. : " + clInterfaceNameT);
               pclQCanInterfaceP = pclInterfaceT;
               break;
            }
         }
      }

      if (pclQCanInterfaceP == NULL)
      {
         emit addLogMessage(CAN_Channel_e (eCAN_CHANNEL_1 + ubInterfaceIdxP),
                            "Search CAN interface ... : Not found");

         emit addLogMessage(CAN_Channel_e (eCAN_CHANNEL_1 + ubInterfaceIdxP),
                            "Use 'Virtual CAN bus' interface");
      }

   }
   else
   {
      emit addLogMessage(CAN_Channel_e (eCAN_CHANNEL_1 + ubInterfaceIdxP),
                         "Use 'Virtual CAN bus' interface");
   }

   qDebug() << "QCanInterfaceWidget::setInterface() ----- emit interfaceChanged(" << ubInterfaceIdxP + 1 << ")";

   emit interfaceChanged(CAN_Channel_e (eCAN_CHANNEL_1 + ubInterfaceIdxP), pclQCanInterfaceP);

   if (clInterfaceNameT != NULL)
   {
      qDebug() << "Set new Interface:" << clInterfaceNameT;

      return true;
   }

   return false;
}

