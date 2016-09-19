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
QCanInterfaceWidget::QCanInterfaceWidget()
 : QWidget()
{
   clIconP = QIcon(":images/network-icon.png");
   pclQCanInterfaceP = NULL;
}


//----------------------------------------------------------------------------//
// QCanInterfaceWidget::mousePressEvent()                                     //
// handle mouse press events on window areas                                  //
//----------------------------------------------------------------------------//
void QCanInterfaceWidget::mousePressEvent(QMouseEvent * pclEventV)
{
   QList<QAction *> apclListActionsT;
   QAction * pclActionT;
   QPoint pos(this->mapFromParent(QCursor::pos()));
   QMenu clContextMenuT(tr("CAN interface selection"), this);

   //----------------------------------------------------------------
   // check there are any plugins available
   //
   if (!loadPlugin())
   {
       QMessageBox::information(this, "ERROR", "Could not load the any plugins!");
   }

   //----------------------------------------------------------------
   // create context menu with all available plugins
   //
   pclActionT = new QAction("Virtual", this);
   pclActionT->setIcon(QIcon(":images/network-icon.png"));
   apclListActionsT.append(pclActionT);
   clContextMenuT.addAction(apclListActionsT.at(0));

   quint32 ulCntrT = 0;
   while (ulCntrT < aclPluginNameListP.count())
   {
      pclActionT = new QAction(aclPluginNameListP.at(ulCntrT), this);
      pclActionT->setIcon(aclIconListP.at(ulCntrT));
      apclListActionsT.append(pclActionT);
      clContextMenuT.addAction(apclListActionsT.at(ulCntrT+1));
      ulCntrT++;
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
            qint32 slIdxT = aclPluginNameListP.indexOf(pclActionT->text());
            qDebug() << "mousePressEvent(): select index ..." << slIdxT;

            pclQCanInterfaceP = NULL;
            if (slIdxT > 0)
            {
               QPluginLoader clPluginLoaderT(aclPluginListP.at(slIdxT));
               QObject *pclPluginT = clPluginLoaderT.instance();
               if (pclPluginT)
               {
                   pclQCanInterfaceP = qobject_cast<QCanInterface *>(pclPluginT);
               }
            }

            emit interfaceChanged(pclQCanInterfaceP);
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
   aclPluginNameListP.clear();
   aclPluginListP.clear();
   aclIconListP.clear();
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
               qInfo() << "loadPlugin(): found" << clPluginPathP.absoluteFilePath(clFileNameT) << "plugin.";

               //-------------------------------------
               // collect all information of valid plugin
               // \todo Get the PluginName and Icon
               //
               aclPluginNameListP.append(pclQCanIfT->name());
               aclIconListP.append(pclQCanIfT->icon());
               aclPluginListP.append(clPluginPathP.absoluteFilePath(clFileNameT));
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

   if (aclPluginNameListP.isEmpty())
   {
      qWarning() << "loadPlugin(): NONE plugins have been found!";
      return false;
   }

   qInfo() << "loadPlugin(): found" << QString::number(aclPluginNameListP.count(),10) << "available plugins.";

   return true;
}


//----------------------------------------------------------------------------//
// QCanInterfaceWidget::paintEvent()                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanInterfaceWidget::paintEvent(QPaintEvent * pclEventV)
{
   qDebug() << "Paint" << pclEventV->rect();

   QPalette clPaletteT(this->palette());
   QPainter clPainterT(this);
   QBrush brush = QBrush(Qt::transparent, Qt::NoBrush);
   qDebug() << "Brush" << brush;
   clPainterT.setPen((Qt::white));
   clPainterT.setBrush(brush);
   //clPainterT.eraseRect(pclEventV->rect());
   clPainterT.fillRect(pclEventV->rect(), QColor(0xE3, 0xE3, 0xE3)); //Qt::NoBrush);
   //clPainterT.drawLine(1, 1, 80, 80);

   clIconP.paint(&clPainterT, pclEventV->rect(), Qt::AlignCenter);
}

//----------------------------------------------------------------------------//
// setPluginPath()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanInterfaceWidget::setPluginPath(QDir clPluginPathV)
{
   clPluginPathP = clPluginPathV;
   qDebug() << "INFO: Plugin Path is set to" << clPluginPathP.absolutePath();
}
