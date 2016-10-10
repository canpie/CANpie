#include "qcan_interface_widget.hpp"

#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>

#include <QMessageBox>
#include <QDir>
#include <QMenu>
#include <QPluginLoader>
#include <QPoint>


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

   //----------------------------------------------------------------
   // check there are any plugins available
   //
   if (!loadPlugin())
   {
       QMessageBox::information(this, "ERROR", "Could not load any plugins!");
   }

   //----------------------------------------------------------------
   // create context menu with all available plugins
   //
//   pclActionT = new QAction(QCAN_IF_VCAN_NAME, this);
//   pclActionT->setIcon(QIcon(QCAN_IF_VCAN_ICON));
//   clContextMenuT.addAction(pclActionT);

   qint32 slCntrT = 0;
   while (slCntrT < atsChannelListP.count())
   {
      pclActionT = new QAction(atsChannelListP[slCntrT].clName, this);
      pclActionT->setIcon(atsChannelListP[slCntrT].clIcon);
      clContextMenuT.addAction(pclActionT);
      slCntrT++;
   }

   //----------------------------------------------------------------
   // evaluate click operation
   //
   switch(pclEventV->button())
   {
      case Qt::LeftButton:

         qDebug() << "Left button pressed";
         //-----------------------------------------------------
         // show context menu
         //
         pclActionT = clContextMenuT.exec(pos);

         //-----------------------------------------------------
         // evaluate selection
         //
         if (pclActionT != 0)
         {
            QList<QString> aclNameT;
            aclNameT.clear();
            for (uint8_t ubCntrT = 0; ubCntrT < atsChannelListP.length(); ubCntrT++)
            {
               aclNameT.append(atsChannelListP[ubCntrT].clName);
            }

            qint32 slIdxT = aclNameT.indexOf(pclActionT->text());

//            qint32 slIdxT = aclPluginNameListP.indexOf(pclActionT->text());
            qDebug() << "mousePressEvent(): select index" << slIdxT << "for" << pclActionT->text();

            pclQCanInterfaceP = NULL;
            if (slIdxT >= 0)
            {
               ubPluginChannelP = slIdxT;
               QPluginLoader clPluginLoaderT(atsChannelListP[slIdxT].clPlugin);
               qDebug() << "Use plugin:" << atsChannelListP[slIdxT].clPlugin;
               QObject *pclPluginT = clPluginLoaderT.instance();
               if (pclPluginT)
               {
                   pclQCanInterfaceP = qobject_cast<QCanInterface *>(pclPluginT);
               }
            }

            emit interfaceChanged(ubInterfaceIdxP,pclQCanInterfaceP,atsChannelListP[slIdxT].ubChannel);
         } else
         {
            qDebug() << "mousePressEvent(): no index selected";
         }
         break;

      case Qt::RightButton:
         qDebug() << "Right button pressed";

         break;

      default:

         break;
   }
   clicked(0);
}

//----------------------------------------------------------------------------//
// loadPlugin()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanInterfaceWidget::loadPlugin()
{
   QCanInterface *pclQCanIfT;
   Channel_ts     tsChannelParmT;
   qInfo() << "loadPlugin(): used plugin path" << clPluginPathP.absolutePath();

   //----------------------------------------------------------------
   // check plugin path
   //
   if (!clPluginPathP.exists())
   {
      qWarning() << "loadPlugin(): plugin path does not exist!";
      return false;
   }

   //----------------------------------------------------------------
   // check plugins and create a list with valid plugins
   //
//   aclPluginNameListP.clear();
//   aclPluginListP.clear();
//   aclIconListP.clear();

   atsChannelListP.clear();

   // add virtual plugin
   tsChannelParmT.clPlugin = QCAN_IF_VCAN_NAME;
   tsChannelParmT.clName = QCAN_IF_VCAN_NAME;
   tsChannelParmT.clIcon = QIcon(QCAN_IF_VCAN_ICON);
   tsChannelParmT.ubChannel = 0;

   atsChannelListP.clear();
   atsChannelListP.append(tsChannelParmT);
//   aclPluginNameListP.append(QCAN_IF_VCAN_NAME);
//   aclIconListP.append(QIcon(QCAN_IF_VCAN_ICON));
//   aclPluginListP.append("Dummy");

   // add real connected plugins
   foreach (QString clFileNameT, clPluginPathP.entryList(QDir::Files))
   {
      if (QLibrary::isLibrary(clPluginPathP.absoluteFilePath(clFileNameT)))
      {
         QPluginLoader clPluginLoaderT(clPluginPathP.absoluteFilePath(clFileNameT));
         QObject *pclPluginT = clPluginLoaderT.instance();
         if (pclPluginT)
         {
            pclQCanIfT = qobject_cast<QCanInterface *>(pclPluginT);
            if (pclQCanIfT)
            {
               qInfo() << "loadPlugin(): found" << clPluginPathP.absoluteFilePath(clFileNameT) << "plugin, which contains" << QString::number(pclQCanIfT->channel()) << "channels.";

               //-------------------------------------
               // collect all information of valid plugin
               // \todo Get the PluginName and Icon
               //
               for (uint8_t ubChnT = 0; ubChnT < pclQCanIfT->channel(); ubChnT++)
               {
                  tsChannelParmT.clPlugin = clPluginPathP.absoluteFilePath(clFileNameT);
                  tsChannelParmT.clName = pclQCanIfT->name(ubChnT);
                  tsChannelParmT.clIcon = pclQCanIfT->icon(ubChnT);
                  tsChannelParmT.ubChannel = ubChnT;

                  qInfo() << "loadPlugin(): add plugin: " << tsChannelParmT.clPlugin;
                  qInfo() << "loadPlugin(): add name: " << tsChannelParmT.clName;
                  qInfo() << "loadPlugin(): add icon: " << tsChannelParmT.clIcon;
                  qInfo() << "loadPlugin(): add channel: " << QString::number(tsChannelParmT.ubChannel,10);

//                  aclPluginNameListP.append(pclQCanIfT->name(ubChnT));
//                  aclIconListP.append(pclQCanIfT->icon(ubChnT));
//                  aclPluginListP.append(clPluginPathP.absoluteFilePath(clFileNameT));

                  atsChannelListP.append(tsChannelParmT);
               }
            }
         } else
         {
            qWarning() << "loadPlugin(): plugin" << clPluginPathP.absoluteFilePath(clFileNameT) << "could NOT be loaded or the root component object could NOT be instantiated!";
         }
      } else
      {
         qWarning() << "loadPlugin(): plugin" << clPluginPathP.absoluteFilePath(clFileNameT) << "is NOT a library!";
      }
   }

   if (atsChannelListP.isEmpty())
   {
      qWarning() << "loadPlugin(): NONE plugins have been found!";
      return false;
   }

   qInfo() << "loadPlugin(): found" << QString::number(atsChannelListP.count(),10) << "available plugins.";

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
QString QCanInterfaceWidget::pluginName()
{
   if (pclQCanInterfaceP != NULL)
   {
      return pclQCanInterfaceP->name(atsChannelListP[ubPluginChannelP].ubChannel);
   }

   return "Virtual CAN bus";
}

//----------------------------------------------------------------------------//
// pluginName()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t QCanInterfaceWidget::pluginChannel()
{
   if (pclQCanInterfaceP != NULL)
   {
      return ubPluginChannelP;
   }

   return 0;
}


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
// setPlugin()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanInterfaceWidget::setPlugin(QString clNameV, quint8 ubChannelV)
{
   QList<QString> aclNameT;

   qInfo() << "Set plugin "  << clNameV << QString::number(ubChannelV,10);

   //----------------------------------------------------------------
   // check there are any plugins available
   //
   if (loadPlugin())
   {
      aclNameT.clear();
      qInfo() << "Number of Plugins: "  << QString::number(atsChannelListP.length(),10);

      for (uint8_t ubCntrT = 0; ubCntrT < atsChannelListP.length(); ubCntrT++)
      {
         aclNameT.append(atsChannelListP[ubCntrT].clName);
      }

      qint32 slIdxT = aclNameT.indexOf(clNameV);
      qDebug() << "setPlugin(): select index" << slIdxT << "for" << clNameV;

      if (slIdxT >= 0)
      {
         ubPluginChannelP = slIdxT;

         pclQCanInterfaceP = NULL;
         QPluginLoader clPluginLoaderT(atsChannelListP[slIdxT].clPlugin);
         qDebug() << "Use plugin:" << atsChannelListP[slIdxT].clPlugin;
         QObject *pclPluginT = clPluginLoaderT.instance();
         if (pclPluginT)
         {
             pclQCanInterfaceP = qobject_cast<QCanInterface *>(pclPluginT);
         }

         emit interfaceChanged(ubInterfaceIdxP,pclQCanInterfaceP, atsChannelListP[slIdxT].ubChannel);

         return true;
      }
   }

   return false;
}


//----------------------------------------------------------------------------//
// setPluginPath()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanInterfaceWidget::setPluginPath(QDir clPluginPathV)
{
   clPluginPathP = clPluginPathV;
   qInfo() << "Plugin Path is set to" << clPluginPathP.absolutePath();
}


