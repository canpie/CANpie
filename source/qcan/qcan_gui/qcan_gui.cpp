//============================================================================//
// File:          qcan_qui.cpp                                                //
// Description:   GUI to handle with plugins and test them                    //
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


#include <QtWidgets>
#include <qcan_gui.hpp>

//! [0]
QCanGui::QCanGui()
{
    createGUI();
    setLayout(layout);
    setWindowTitle("Echo Plugin Example");

    if (!loadPlugin()) {
        QMessageBox::information(this, "Error", "Could not load the plugin");
        lineEdit->setEnabled(false);
    }
}
//! [0]

uint32_t QCanGui::parmValue(QStringList clCmdV, uint8_t ubParmNrV)
{
   bool     btResultT;
   uint32_t ulValueT = 0;

   if (clCmdV.length() > ubParmNrV)
   {
      ulValueT = clCmdV.at(ubParmNrV).toUInt(&btResultT, 10);
      if (!btResultT)
      {
         ulValueT = 0;
      }
   }

   return ulValueT;
}

void QCanGui::triggerCmd()
{
   uint8_t  ubChnNrT = 0;
   QString  clStringT;

   QStringList clCmdT = lineEdit->text().split(' ');
   lineEdit->clear();

   uint32_t ulStatusT;
   QCanFrame clFrameT;

   ubChnNrT = (uint8_t) parmValue(clCmdT,1);


   //------------------------------------------------------------------
   // connect
   //
   if (clCmdT.at(0)  == "connect")
   {
      ulStatusT = (uint32_t) qCanInterface->connect(ubChnNrT);
      qInfo() << QString("connect(" + QString::number(ubChnNrT) + ") => " + QString::number(ulStatusT));
   }

   if (clCmdT.at(0) == "name")
   {
      clStringT = qCanInterface->name(ubChnNrT);
      qInfo() << QString("name(" + QString::number(ubChnNrT) + ") => " + clStringT);
   }

   if (clCmdT.at(0) == "channel")
   {
      ulStatusT = (uint32_t) qCanInterface->channel();
      qInfo() << QString("channel() => " + QString::number(ulStatusT));
   }

   if (clCmdT.at(0) == "bitrate")
   {
      int32_t slBitrateT = (int32_t) parmValue(clCmdT,1);
      int32_t slBrsClockV = (int32_t) parmValue(clCmdT,2);

      ubChnNrT = (uint8_t) parmValue(clCmdT,3);

      ulStatusT = qCanInterface->setBitrate((QCan::CAN_Bitrate_e) slBitrateT, slBrsClockV, ubChnNrT);
      qInfo() << QString("bitrate(" + QString::number(slBitrateT) +"," +QString::number(slBrsClockV)+"," + QString::number(ubChnNrT) + ") => " + QString::number(ulStatusT));
   }

   if (clCmdT.at(0) == "runtest")
   {
      ubChnNrT = parmValue(clCmdT,2);

      switch (parmValue(clCmdT,1))
      {
         case 0 :
         if (qCanInterface->connect(ubChnNrT) != QCanInterface::eERROR_NONE)
            qWarning() << "Warning: connect() fail!";

         if (qCanInterface->setBitrate(QCan::eCAN_BITRATE_500K, 0,ubChnNrT) != QCanInterface::eERROR_NONE)
            qWarning() << "Warning: setBitrate() fail!";

         if (qCanInterface->setMode(QCan::eCAN_MODE_START,ubChnNrT) != QCanInterface::eERROR_NONE)
            qWarning() << "Warning: setMode() fail!";
         break;

         //------------------------------------------------------------
         // Init interface 1 and transmit a FD message
         //
         case 1 :
         if (qCanInterface->connect(ubChnNrT) != QCanInterface::eERROR_NONE)
            qWarning() << "Warning: connect() fail!";

         if (qCanInterface->setBitrate(QCan::eCAN_BITRATE_1M, 2000000, ubChnNrT) != QCanInterface::eERROR_NONE)
            qWarning() << "Warning: setBitrate() fail!";

         if (qCanInterface->setMode(QCan::eCAN_MODE_START,ubChnNrT) != QCanInterface::eERROR_NONE)
            qWarning() << "Warning: setMode() fail!";

         QCanFrame *pclFrameFdT = new QCanFrame(QCanFrame::eTYPE_FD_STD,0x175);
         pclFrameFdT->setDataSize(11);

         pclFrameFdT->setData(0,5);
         pclFrameFdT->setData(1,4);
         pclFrameFdT->setData(2,3);
         pclFrameFdT->setData(3,2);
         pclFrameFdT->setData(4,1);
         pclFrameFdT->setData(5,0xAA);
         pclFrameFdT->setData(6,0x55);
         pclFrameFdT->setData(7,0xAA);
         pclFrameFdT->setData(8,0xAA);
         pclFrameFdT->setData(9,0x77);


         qInfo() << "write a standard FD Message!";

         int32_t slStatusT = qCanInterface->write(*pclFrameFdT, ubChnNrT);

         if (slStatusT ==  QCanInterface::eERROR_NONE)
         {

         }
         else if (slStatusT !=  QCanInterface::eERROR_FIFO_TRM_FULL)
         {
            qWarning() << "Warning: write() fail!";
         }
         break;

      }
   }



   if (clCmdT.at(0) == "write")
   {
      clFrameT.setStdId(175);
      clFrameT.setDlc(5);
      clFrameT.setData(0,5);
      clFrameT.setData(1,4);
      clFrameT.setData(2,3);
      clFrameT.setData(3,2);
      clFrameT.setData(4,1);

      int32_t slStatusT = qCanInterface->write(clFrameT,0);

      if (slStatusT ==  QCanInterface::eERROR_NONE)
      {

      }
      else if (slStatusT !=  QCanInterface::eERROR_FIFO_TRM_FULL)
      {
         qWarning() << "Warning: write() fail!";
      }
   }

   if (clCmdT.at(0) == "read")
   {
      ubChnNrT = parmValue(clCmdT,1);

      int32_t slStatusT = qCanInterface->read(clFrameT,ubChnNrT);

      qDebug() << "read status [hex]:" << QString::number(slStatusT,16);

      if (slStatusT == QCanInterface::eERROR_NONE)
      {



         if (clFrameT.isExtended())
         {
            qDebug() << "Receive Ext: " + QString::number((uint32_t)clFrameT.identifier(),16) + "h " +
                         QString::number((uint8_t)clFrameT.dlc(),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(0),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(1),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(2),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(3),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(4),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(5),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(6),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(7),16) + "h ";


         } else
         {
            qDebug() << "Receive Std: " + QString::number((uint32_t)clFrameT.identifier(),16) + "h " +
                         QString::number((uint8_t)clFrameT.dlc(),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(0),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(1),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(2),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(3),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(4),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(5),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(6),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(7),16) + "h ";
         }
      }
      else if (slStatusT != QCanInterface::eERROR_FIFO_RCV_EMPTY)
      {
         qWarning() << "Warning: read() fail with " << QString::number(slStatusT,10);
      }
   }

   if (clCmdT.at(0) == "write_ext")
   {
      clFrameT.setExtId(175234);
      clFrameT.setDlc(8);
      clFrameT.setData(0,0x55);
      clFrameT.setData(1,0x44);
      clFrameT.setData(2,0x33);
      clFrameT.setData(3,0x22);
      clFrameT.setData(4,0x11);
      clFrameT.setData(5,0x14);
      clFrameT.setData(6,0x15);
      clFrameT.setData(7,0x16);

      int32_t slStatusT = qCanInterface->write(clFrameT,0);

      if (slStatusT == QCanInterface::eERROR_NONE)
      {

      }
      else if (slStatusT != QCanInterface::eERROR_FIFO_TRM_FULL)
      {
         qWarning() << "Warning: write() fail!";
      }
   }
}


void QCanGui::changePlugin(int slIdxV)
{
    QPluginLoader pluginLoader(aclDirListP.at(slIdxV));
    QObject *plugin = pluginLoader.instance();
    if (plugin) {
        qCanInterface = qobject_cast<QCanInterface *>(plugin);
    }
}


void QCanGui::createGUI()
{
    lineEdit = new QLineEdit;
//    label = new QLabel;
    comboBox = new QComboBox;
//    label->setFrameStyle(QFrame::Box | QFrame::Plain);
//    button = new QPushButton(tr("Send Message"));

    connect(lineEdit, SIGNAL( returnPressed()),
            this, SLOT(triggerCmd()));

    layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Plugin:")), 0, 0);
    layout->addWidget(comboBox, 0, 1);
    layout->addWidget(new QLabel(tr("Message:")), 1, 0);
    layout->addWidget(lineEdit, 1, 1);
//    layout->addWidget(new QLabel(tr("Answer:")), 2, 0);
//    layout->addWidget(label, 2, 1);
//    layout->addWidget(button, 3, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
}
//! [2]

//! [3]
bool QCanGui::loadPlugin()
{
    QStringList clSelectItemListT;
    QDir pluginsDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");


    if (!pluginsDir.exists())
    {
      qDebug() << "ERROR: Plugin Path: " << pluginsDir.absolutePath() << " does not exist!";
      return false;
    }


    clSelectItemListT.clear();
    aclDirListP.clear();
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
       qDebug() << "DEBUG: plugin file: " << pluginsDir.absoluteFilePath(fileName);

       if (QLibrary::isLibrary(pluginsDir.absoluteFilePath(fileName)))
       {
           qDebug() << "DEBUG: plugin file is a library... ";
       }
        QPluginLoader pluginLoader( pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            qCanInterface = qobject_cast<QCanInterface *>(plugin);
            if (qCanInterface)
            {
                clSelectItemListT.append(fileName);
                aclDirListP.append(pluginsDir.absoluteFilePath(fileName));
            }
        } else

                  {
                     qDebug() << "DEBUG: not a plugin file...";
                  }
    }

    if (clSelectItemListT.isEmpty())
    {
        return false;
    }

    comboBox->addItems(clSelectItemListT);
    comboBox->setCurrentIndex(clSelectItemListT.count()-1);

    connect(comboBox, SIGNAL(activated(int)),
            this, SLOT(changePlugin(int)));

    return true;
}
//! [3]
