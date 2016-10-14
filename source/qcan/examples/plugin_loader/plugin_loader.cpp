//============================================================================//
// File:          plugin_loader.cpp                                           //
// Description:   Example application to load CANpie plugins                  //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53844 Troisdorf - Germany                                                  //
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
// ("LGPL") version 3 as distributed in the 'LICENSE' file.                   //
//                                                                            //
//============================================================================//


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include <QtWidgets>
#include <plugin_loader.hpp>

/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// PluginLoader()                                                             //
// constructor                                                                //
//----------------------------------------------------------------------------//
PluginLoader::PluginLoader()
{
    createGUI();
    setLayout(layoutP);
    setWindowTitle("Example: Plugin Loader");

    if (!loadPlugin()) {
        QMessageBox::information(this, "Error", "Could not load the plugin");
        lineEditP->setEnabled(false);
    }
}

//----------------------------------------------------------------------------//
// parmValue()                                                                //
// helper function to get given value from a string                           //
//----------------------------------------------------------------------------//
uint32_t PluginLoader::parmValue(QStringList clCmdV, uint8_t ubParmNrV)
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

//----------------------------------------------------------------------------//
// triggerCmd()                                                               //
// evaluates entered command calls methods of selected the plugin             //
//----------------------------------------------------------------------------//
void PluginLoader::triggerCmd()
{
   uint8_t  ubChnNrT = 0;
   QString  clStringT;

   QStringList clCmdT = lineEditP->text().split(' ');
   lineEditP->clear();

   uint32_t ulStatusT;
   QCanFrame clFrameT;

   ubChnNrT = (uint8_t) parmValue(clCmdT,1);


   //------------------------------------------------------------------
   // connect
   //
   if (clCmdT.at(0)  == "connect")
   {
      ulStatusT = 0;
      qCanInterfaceP = qCanPluginP->getInterface(parmValue(clCmdT,1));
      if (qCanInterfaceP != NULL)
      {
         ulStatusT = (uint32_t) qCanInterfaceP->connect();
      }
      qInfo() << QString("connect() => " + QString::number(ulStatusT));
   }

   if (clCmdT.at(0) == "name")
   {
      clStringT = "FAIN to get Interface of plugin!";
      qCanInterfaceP = qCanPluginP->getInterface(parmValue(clCmdT,1));
      if (qCanInterfaceP != NULL)
      {
         clStringT = qCanInterfaceP->name();
      }
      qInfo() << QString("name("+ QString::number(parmValue(clCmdT,1)) + ") => " + clStringT);
   }

//   if (clCmdT.at(0) == "channel")
//   {
//      ulStatusT = (uint32_t) qCanInterfaceP->channel();
//      qInfo() << QString("channel() => " + QString::number(ulStatusT));
//   }

   if (clCmdT.at(0) == "interfaceCount")
   {
      ulStatusT = (uint32_t) qCanPluginP->interfaceCount();
      qInfo() << QString("canInterfaces() => " + QString::number(ulStatusT));
   }

//   if (clCmdT.at(0) == "interfaces")
//   {
//      ulStatusT = (uint32_t) qCanPluginP->interfaceCount();
//      qInfo() << QString("canInterfaces() => " + QString::number(ulStatusT));

//      qCanInterfaceP = qCanPluginP->getInterface(1);
//      if (qCanInterfaceP != NULL)
//      {

//         qInfo() << QString("canInterface(1).name() => " + qCanInterfaceP->name());
//      } else
//      {
//         qWarning() << QString("Fail to get interface 1");
//      }

//   }

   if (clCmdT.at(0) == "support")
   {
      ulStatusT = (uint32_t) qCanPluginP->interfaceCount();
      qDebug() << "Number of supported interfaces:" << QString::number(ulStatusT);

      for (uint32_t ulCntrT = 0; ulCntrT < ulStatusT; ulCntrT++)
      {
         qCanInterfaceP = qCanPluginP->getInterface(ulCntrT);

         qCanInterfaceP->connect();
         qInfo() << "Supported Features:" << QString::number(qCanInterfaceP->supportedFeatures(),16) << "[hex]";
         qCanInterfaceP->disconnect();
      }
   }


   if (clCmdT.at(0) == "bitrate")
   {
      int32_t slBitrateT = (int32_t) parmValue(clCmdT,1);
      int32_t slBrsClockV = (int32_t) parmValue(clCmdT,2);

      ubChnNrT = (uint8_t) parmValue(clCmdT,3);

      ulStatusT = qCanInterfaceP->setBitrate((CAN_Bitrate_e) slBitrateT, slBrsClockV);
      qInfo() << QString("bitrate(" + QString::number(slBitrateT) +"," +QString::number(slBrsClockV)+"," + QString::number(ubChnNrT) + ") => " + QString::number(ulStatusT));
   }

   if (clCmdT.at(0) == "runtest")
   {
      ubChnNrT = parmValue(clCmdT,2);

      switch (parmValue(clCmdT,1))
      {
         case 0 :
         if  (0 >= qCanPluginP->interfaceCount())
            return;
         qCanInterfaceP = qCanPluginP->getInterface(0);

         if (qCanInterfaceP->connect() != QCanInterface::eERROR_NONE)
            qWarning() << "Warning: connect() fail!";

         if (qCanInterfaceP->setBitrate(eCAN_BITRATE_500K, 0) != QCanInterface::eERROR_NONE)
            qWarning() << "Warning: setBitrate() fail!";

         if (qCanInterfaceP->setMode(eCAN_MODE_START) != QCanInterface::eERROR_NONE)
            qWarning() << "Warning: setMode() fail!";
         break;

         //------------------------------------------------------------
         // Init interface 1 and transmit a FD message
         //
         case 1 :

         if  (1 >= qCanPluginP->interfaceCount())
            return;

         qCanInterfaceP = qCanPluginP->getInterface(1);

         if (qCanInterfaceP->connect() != QCanInterface::eERROR_NONE)
            qWarning() << "Warning: connect() fail!";

         if (qCanInterfaceP->setBitrate(eCAN_BITRATE_1M, 2000000) != QCanInterface::eERROR_NONE)
            qWarning() << "Warning: setBitrate() fail!";

         if (qCanInterfaceP->setMode(eCAN_MODE_START) != QCanInterface::eERROR_NONE)
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

         int32_t slStatusT = qCanInterfaceP->write(*pclFrameFdT);

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

      int32_t slStatusT = qCanInterfaceP->write(clFrameT);

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

      int32_t slStatusT = qCanInterfaceP->read(clFrameT);

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

      int32_t slStatusT = qCanInterfaceP->write(clFrameT);

      if (slStatusT == QCanInterface::eERROR_NONE)
      {

      }
      else if (slStatusT != QCanInterface::eERROR_FIFO_TRM_FULL)
      {
         qWarning() << "Warning: write() fail!";
      }
   }
}

//----------------------------------------------------------------------------//
// changePlugin()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
void PluginLoader::changePlugin(int slIdxV)
{
   QPluginLoader pluginLoader(aclDirListP.at(slIdxV));
   QObject *plugin = pluginLoader.instance();
   if (plugin)
   {
      qDebug() << "PluginLoader::changePlugin() DEBUG: Load new plugin:" << aclDirListP.at(slIdxV);
      qCanPluginP = qobject_cast<QCanPlugin *>(plugin);
   }
}


//----------------------------------------------------------------------------//
// createGUI()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
void PluginLoader::createGUI()
{
   lineEditP = new QLineEdit;
   comboBoxP = new QComboBox;

   connect(lineEditP, SIGNAL( returnPressed()),
           this, SLOT(triggerCmd()));

   layoutP = new QGridLayout;
   layoutP->addWidget(new QLabel(tr("Plugin:")), 0, 0);
   layoutP->addWidget(comboBoxP, 0, 1);
   layoutP->addWidget(new QLabel(tr("Message:")), 1, 0);
   layoutP->addWidget(lineEditP, 1, 1);
   layoutP->setSizeConstraint(QLayout::SetFixedSize);
}

//----------------------------------------------------------------------------//
// loadPlugin()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
bool PluginLoader::loadPlugin()
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
      qCritical() << "PluginLoader::loadPlugin() CRITICAL: Plugin Path: " << pluginsDir.absolutePath() << " does not exist!";
      return false;
   }

   clSelectItemListT.clear();
   aclDirListP.clear();
   foreach (QString fileName, pluginsDir.entryList(QDir::Files))
   {
      qDebug() << "PluginLoader::loadPlugin() DEBUG: plugin file: " << pluginsDir.absoluteFilePath(fileName);

      if (QLibrary::isLibrary(pluginsDir.absoluteFilePath(fileName)))
      {
         qDebug() << "PluginLoader::loadPlugin() DEBUG: plugin file is a library... ";
      }
      QPluginLoader pluginLoader( pluginsDir.absoluteFilePath(fileName));
      QObject *plugin = pluginLoader.instance();
      if (plugin)
      {
         qCanPluginP = qobject_cast<QCanPlugin *>(plugin);
         if (qCanPluginP)
         {
             clSelectItemListT.append(fileName);
             aclDirListP.append(pluginsDir.absoluteFilePath(fileName));
         }
      } else
      {
         qDebug() << "PluginLoader::loadPlugin() DEBUG: not a plugin file...";
      }
   }

   if (clSelectItemListT.isEmpty())
   {
      return false;
   }

   comboBoxP->addItems(clSelectItemListT);
   comboBoxP->setCurrentIndex(clSelectItemListT.count()-1);

   connect(comboBoxP, SIGNAL(activated(int)),
           this, SLOT(changePlugin(int)));

   return true;
}
