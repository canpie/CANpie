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
}


//----------------------------------------------------------------------------//
// QCanInterfaceWidget::mousePressEvent()                                     //
// handle mouse press events on window areas                                  //
//----------------------------------------------------------------------------//
void QCanInterfaceWidget::mousePressEvent(QMouseEvent * pclEventV)
{
   QList<QAction *> aclListActionsT;
   QAction * pclActionT;

   quint32  ulCntrT;

   quint8 ubCntrT = 0;
   QPoint pos(this->mapFromParent(QCursor::pos()));

   QMenu contextMenu(tr("Context menu"), this);

   if (!loadPlugin())
   {
       QMessageBox::information(this, "ERROR", "Could not load the any plugins!");
       //return;
   }

   //qDebug() << clPluginItemListP.at(0);

   pclActionT = new QAction("Virtual", this);
   pclActionT->setIcon(QIcon(":images/network-icon.png"));
   aclListActionsT.append(pclActionT);
   contextMenu.addAction((aclListActionsT.at(0)));

//   ulCntrT = 0;
//   while (ulCntrT < clPluginItemListP.count())
//   {
//      pclActionT = new QAction(clPluginItemListP.at(ulCntrT), this);
//      aclListActionsT.append(pclActionT);
//      ulCntrT++;
//   }

//   clActionT =pclActionT;//aclListActionsT[0];
//   QAction clActionT("Virtual", this);
//   clActionT.setIcon(QIcon(":images/network-icon.png"));
//   contextMenu.addAction(&clActionT);
//   contextMenu.addAction(&aclListActionsT[1]);
//   contextMenu.addAction(&aclListActionsT[2]);

   switch(pclEventV->button())
   {
      case Qt::LeftButton:
         qDebug() << "Left button pressed";

         //----------------------------------------------------------------
         // create CAN networks
         //
//         if (!loadPlugin())
//         {
//             QMessageBox::information(this, "ERROR", "Could not load the any plugins!");
      //       lineEdit->setEnabled(false);
      //       button->setEnabled(false);
//         }

//         while (ubCntrT < clPluginItemListP.count())
//         {
//            contextMenu.addAction(&aclActionElementT[0]);
//         }
//           //    connect(&action1, SIGNAL(triggered()), this, SLOT(removeDataPoint()));
//            contextMenu.addAction(&aclAction1T);
//            contextMenu.addAction(&aclAction2T);
//            contextMenu.addAction(&aclAction3T);

//            contextMenu.addAction(&aclListActionsT);
            contextMenu.exec(pos);


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
    qWarning() << "loadPlugin(): used plugin path" << clPluginPathP.absolutePath();

    if (!clPluginPathP.exists())
    {
      qWarning() << "loadPlugin(): plugin path does not exist!";
      return false;
    }

    clPluginItemListP.clear();
    aclPluginListP.clear();
    foreach (QString fileName, clPluginPathP.entryList(QDir::Files))
    {
        if (QLibrary::isLibrary(clPluginPathP.absoluteFilePath(fileName)))
        {
           QPluginLoader pluginLoader(clPluginPathP.absoluteFilePath(fileName));
           QObject *plugin = pluginLoader.instance();
           if (plugin)
           {
               qCanInterface = qobject_cast<QCanInterface *>(plugin);
               if (qCanInterface)
               {
                  qInfo() << "loadPlugin(): found" << clPluginPathP.absoluteFilePath(fileName) << "plugin.";
                  clPluginItemListP.append(fileName);
                  aclPluginListP.append(clPluginPathP.absoluteFilePath(fileName));
               }
           } else
           {
              qWarning() << "loadPlugin(): plugin" << clPluginPathP.absoluteFilePath(fileName) << "could NOT be loaded or the root component object could NOT be instantiated!";
           }
        } else
        {
            qWarning() << "loadPlugin(): plugin" << clPluginPathP.absoluteFilePath(fileName) << "is NOT a library!";
        }
    }

    if (clPluginItemListP.isEmpty())
    {
        qWarning() << "loadPlugin(): NONE plugins have been found!";
        return false;
    }

    qInfo() << "loadPlugin(): found" << QString::number(clPluginItemListP.count(),10) << "available plugins.";
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
