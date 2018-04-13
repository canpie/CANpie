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



/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

//----------------------------------------------------------------------------//
// QCanInterfaceWidget()                                                      //
// constructor                                                                //
//----------------------------------------------------------------------------//
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
      clPluginsDirT.setPath(clPluginsDirT.path() + "/Plugins");
   }
   #endif
   clPluginPathP = clPluginsDirT;
   qDebug() << QString("QCanInterfaceWidget::QCanInterfaceWidget("+QString::number(ubIdxV) +","+ clPluginPathP.absolutePath()+")");

   loadPlugin();
}


//----------------------------------------------------------------------------//
// QCanInterfaceWidget::mousePressEvent()                                     //
// handle mouse press events on window areas                                  //
//----------------------------------------------------------------------------//
void QCanInterfaceWidget::mousePressEvent(QMouseEvent * pclEventV)
{
   QAction * pclActionT;
   QPoint pos(this->mapFromParent(QCursor::pos()));
   QMenu clContextMenuT(tr("CAN interface selection"), this);
   QMenu *clPluginMenuT;
   QCanInterface *pclInterfaceT;
   QString clInterfaceNameT;


   if (!loadPlugin())
   {
//       QMessageBox::information(this, "ERROR", "Could not load any plugins!");
      qCritical() << "QCanInterfaceWidget::QCanInterfaceWidget() ERROR: Could not load any plugins!";
   }

   //----------------------------------------------------------------
   // create context menu with all available plugins
   //
   pclActionT = new QAction(QCAN_IF_VCAN_NAME, this);
   pclActionT->setIcon(QIcon(QCAN_IF_VCAN_ICON));
   clContextMenuT.addAction(pclActionT);

   //----------------------------------------------------------------
   // create menues for each plugin with corresponding interfaces
   //
   foreach (QCanPlugin *pclPluginT, apclQCanPluginP)
   {
      clPluginMenuT = clContextMenuT.addMenu(pclPluginT->name());
      clPluginMenuT->setIcon(pclPluginT->icon());

      for (uint8_t ubIfCntT = 0; ubIfCntT < pclPluginT->interfaceCount(); ubIfCntT++)
      {
         pclInterfaceT = pclPluginT->getInterface(ubIfCntT);

         // at this point interface could be used by an other client
         // don't put it in to the list as available
         if (pclInterfaceT->connect() == QCanInterface::eERROR_NONE)
         {
            clInterfaceNameT = pclInterfaceT->name();
            pclActionT = new QAction(clInterfaceNameT, this);
            pclActionT->setIcon(QIcon(pclInterfaceT->icon()));
            pclInterfaceT->disconnect();

            clPluginMenuT->addAction(pclActionT);
         }
      }
   }

   //----------------------------------------------------------------
   // evaluate click operation
   //
   switch(pclEventV->button())
   {
      case Qt::RightButton:

         //-----------------------------------------------------
         // show context menu
         //
         pclActionT = clContextMenuT.exec(pos);

         //-----------------------------------------------------
         // evaluate selection
         //
         if (pclActionT != 0)
         {
            qDebug() << "left mousePressEvent(), process action:" << pclActionT->text();

            setInterface(pclActionT->text());
         }
         break;

      case Qt::LeftButton:
         qDebug() << "Right button pressed";

         break;

      default:

         break;
   }


   //  clicked(0);
}

//----------------------------------------------------------------------------//
// loadPlugin()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanInterfaceWidget::loadPlugin()
{
   QCanPlugin    *pclQCanPluginT;

   //----------------------------------------------------------------
   // reset list of plugins
   //
   apclQCanPluginP.clear();

   //----------------------------------------------------------------
   // check plugin path
   //
   if (!clPluginPathP.exists())
   {
      qWarning() << "QCanInterfaceWidget::loadPlugin() WARNING: plugin path does not exist!";
      return false;
   }

   //----------------------------------------------------------------
   // creat a list of plugins
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
         } else
         {
            qWarning() << "QCanInterfaceWidget::loadPlugin() WARNING: plugin" << clPluginPathP.absoluteFilePath(clFileNameT) << "could NOT be loaded or the root component object could NOT be instantiated!";
         }
      } else
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


//----------------------------------------------------------------------------//
// QCanInterfaceWidget::paintEvent()                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanInterfaceWidget::paintEvent(QPaintEvent * pclEventV)
{
   //qDebug() << "Paint" << pclEventV->rect();

   QPalette clPaletteT(this->palette());
   QPainter clPainterT(this);
   QBrush brush = QBrush(Qt::transparent, Qt::NoBrush);
   //qDebug() << "Brush" << brush;
   clPainterT.setPen((Qt::white));
   clPainterT.setBrush(brush);
   //clPainterT.eraseRect(pclEventV->rect());
   clPainterT.fillRect(pclEventV->rect(), QColor(0xE3, 0xE3, 0xE3)); //Qt::NoBrush);
   //clPainterT.drawLine(1, 1, 80, 80);

   clIconP.paint(&clPainterT, pclEventV->rect(), Qt::AlignCenter);
}

//----------------------------------------------------------------------------//
// pluginName()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
QString QCanInterfaceWidget::name()
{
   QString clNameT = QString(QCAN_IF_VCAN_NAME);
   bool    btIfConnectedT = false;

   if (pclQCanInterfaceP != NULL)
   {
      // store previous state of connection
      if (pclQCanInterfaceP->connected())
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

//----------------------------------------------------------------------------//
// pluginName()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
//uint8_t QCanInterfaceWidget::pluginChannel()
//{
//   if (pclQCanInterfaceP != NULL)
//   {
//      pclQCanInterfaceP->connect()
//      QString clNameT = pclQCanInterfaceP->name();
//      pclQCanInterfaceP->disconnect()

//      return ubPluginChannelP;
//   }

//   return 0;
//}


//----------------------------------------------------------------------------//
// setIcon()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanInterfaceWidget::setIcon(QIcon clIconR)
{
   clIconP = clIconR;
   this->repaint();
}


//----------------------------------------------------------------------------//
// setInterface()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanInterfaceWidget::setInterface(QString clNameV)
{
   QString clInterfaceNameT = NULL;
   QCanInterface *pclInterfaceT;

   qInfo() << QString("QCanInterfaceWidget::setInterface(" + clNameV +")");

   pclQCanInterfaceP = NULL;

   if (clNameV != QString(QCAN_IF_VCAN_NAME))
   {
      foreach (QCanPlugin *pclPluginT, apclQCanPluginP)
      {
         // if an interface have been found quit here
         if (pclQCanInterfaceP != NULL)
         {
            break;
         }

         // check all interces of selected plugin
         for (uint8_t ubIfCntT = 0; ubIfCntT < pclPluginT->interfaceCount(); ubIfCntT++)
         {
            qDebug() << "2te stelle....";
            pclInterfaceT = pclPluginT->getInterface(ubIfCntT);
            pclInterfaceT->connect();
            clInterfaceNameT = pclInterfaceT->name();
            pclInterfaceT->disconnect();

            if (clNameV == clInterfaceNameT)
            {
               qDebug() << "Reqeusted Interface found: " << clInterfaceNameT;
               pclQCanInterfaceP = pclInterfaceT;
               break;
            }
         }
      }
   } else
   {
      qDebug() << "New interface name is 'Virtual CAN bus', so do not change it.";
   }

   qDebug() << "emit interfaceChanged " << QString::number(ubInterfaceIdxP,10);

   emit interfaceChanged(ubInterfaceIdxP, pclQCanInterfaceP);

   if (clInterfaceNameT != NULL)
   {
      qDebug() << "Set new Interface:" << clInterfaceNameT;

      return true;
   }

   return false;
}


////----------------------------------------------------------------------------//
//// setPluginPath()                                                            //
////                                                                            //
////----------------------------------------------------------------------------//
//void QCanInterfaceWidget::setPluginPath(QDir clPluginPathV)
//{
//   clPluginPathP = clPluginPathV;
//   qDebug() << QString("QCanInterfaceWidget::setPluginPath("+clPluginPathP.absolutePath()+")");
//}


