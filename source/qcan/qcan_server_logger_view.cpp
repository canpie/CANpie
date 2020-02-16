//====================================================================================================================//
// File:          qcan_server_logger_view.cpp                                                                         //
// Description:   QCAN server - logging widget                                                                        //
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

#include <QtGui/QFont>
#include <QtGui/QFontDatabase>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

#include "qcan_server_logger_view.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/



//--------------------------------------------------------------------------------------------------------------------//
// QCanServerLoggerView::QCanServerLoggerView()                                                                       //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanServerLoggerView::QCanServerLoggerView(QCanServer * pclServerV)
                     :QCanServerLogger(pclServerV)
{
   teCanChannelP = eCAN_CHANNEL_1;
   pclLogWindowP = new QMainWindow();
   pclLogTabP    = new QTabWidget();

   //---------------------------------------------------------------------------------------------------
   // add the Source Code Pro font
   //
   int slFontIdT = QFontDatabase::addApplicationFont(":/fonts/SourceCodePro-Medium.ttf");
   QString clFontFamilyT = QFontDatabase::applicationFontFamilies(slFontIdT).at(0);
   qDebug() << "Font family" << clFontFamilyT;
   QFont clFontT = QFont(clFontFamilyT, 12, 50);

   QString  clTabLabelT;
   for (uint8_t ubLogNumT = 0; ubLogNumT < QCAN_NETWORK_MAX; ubLogNumT++)
   {
      apclLogTextP[ubLogNumT] = new QTextBrowser();
      apclLogTextP[ubLogNumT]->setFont(clFontT);
      clTabLabelT = QString(" CAN &%1 ").arg(ubLogNumT + 1);

      apclLogTextP[ubLogNumT]->setContextMenuPolicy(Qt::CustomContextMenu);
      connect( apclLogTextP[ubLogNumT],
               SIGNAL(customContextMenuRequested(const QPoint &)),
               this, SLOT(onShowLogMenu(const QPoint &)));

      pclLogTabP->addTab(apclLogTextP[ubLogNumT], clTabLabelT);
   }


   //---------------------------------------------------------------------------------------------------
   // the tab widget is the central widget, the initial log widget
   // size is 800 x 480 pixel
   //
   pclLogWindowP->setCentralWidget(pclLogTabP);
   pclLogWindowP->setWindowTitle("CANpie Server - Logging Window");
   pclLogWindowP->resize(800, 480);

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerLoggerView::~QCanServerLoggerView()                                                                      //
// destructor                                                                                                         //
//--------------------------------------------------------------------------------------------------------------------//
QCanServerLoggerView::~QCanServerLoggerView()
{

}

//--------------------------------------------------------------------------------------------------------------------//
// QCanServerLoggerView::appendMessage()                                                                              //
// append log message to windows and to log file                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerLoggerView::appendMessage(const CAN_Channel_e teChannelV, const QString & clLogMessageV,
                                         LogLevel_e teLogLevelV)
{
   QCanServerLogger::appendMessage(teChannelV, clLogMessageV, teLogLevelV);

   if ((teChannelV >= eCAN_CHANNEL_1) && (teChannelV <= QCAN_NETWORK_MAX))
   {

      if (teLogLevelV <= logLevel(teChannelV))
      {
         apclLogTextP[teChannelV - 1]->append(message());
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// hide()                                                                                                             //
// hide the log window                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerLoggerView::hide(void)
{
   pclLogWindowP->hide();
}


//--------------------------------------------------------------------------------------------------------------------//
// isHidden()                                                                                                         //
// check if log window is hidden                                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanServerLoggerView::isHidden(void)
{
   return (pclLogWindowP->isHidden());
}


//--------------------------------------------------------------------------------------------------------------------//
// onChangeLogLevel()                                                                                                 //
// change log level for all CAN channels                                                                              //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerLoggerView::onChangeLogLevel(QAction * pclActionV)
{
   setLogLevel(teCanChannelP, (LogLevel_e) pclActionV->data().toInt());
}

//--------------------------------------------------------------------------------------------------------------------//
// onClearLog()                                                                                                       //
// clear log window                                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerLoggerView::onClearLog(void)
{
   apclLogTextP[teCanChannelP - 1]->clear();
}


//--------------------------------------------------------------------------------------------------------------------//
// onSetLogFile()                                                                                                     //
// set log file                                                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerLoggerView::onSetLogFile(void)
{
   QString clFileNameT;

   clFileNameT = QFileDialog::getSaveFileName(Q_NULLPTR, tr("Save File"),
                              "~/untitled.log",
                              tr("Log file (*.log)"));

   if(clFileNameT.isEmpty() == false)
   {
      setFileName(teCanChannelP, clFileNameT);
   }
}

//--------------------------------------------------------------------------------------------------------------------//
// showLogWindowMenu()                                                                                                //
// show context menu in log window                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerLoggerView::onShowLogMenu(const QPoint &pos)
{
   QMenu *    pclMenuT;
   QMenu *    pclSubMenuT;
   QAction *  pclClearT;
   QAction *  pclFileT;
   QAction *  pclLevelT;
   int        slTabIndexT;

   slTabIndexT = pclLogTabP->currentIndex();
   if(slTabIndexT >= 0)
   {
      //-------------------------------------------------------
      // store the selected tab
      //
      teCanChannelP = (CAN_Channel_e) (slTabIndexT + 1);

      //-------------------------------------------------------
      // create default menu and add additional entries
      //
      pclMenuT  = apclLogTextP[slTabIndexT]->createStandardContextMenu();


      //-------------------------------------------------------
      // clear all entries
      //
      pclClearT = pclMenuT->addAction(tr("Clear all"),
                                       this, SLOT(onClearLog()));
      if(apclLogTextP[slTabIndexT]->toPlainText().size() == 0)
      {
          pclClearT->setDisabled(true);
      }
      else
      {
          pclClearT->setEnabled(true);
      }

      pclMenuT->addSeparator();

      //-------------------------------------------------------
      // log data to file
      //
      pclFileT  = pclMenuT->addAction(tr("Log to file .."),
                                       this, SLOT(onSetLogFile()));

      //-------------------------------------------------------
      // create a sub-menu for the different log-levels
      //
      pclSubMenuT = new QMenu(tr("Log level"));


      pclLevelT = pclSubMenuT->addAction(tr("Fatal"));
      pclLevelT->setCheckable(true);
      pclLevelT->setData(eLOG_LEVEL_FATAL);
      if (logLevel(teCanChannelP) == eLOG_LEVEL_FATAL)
      {
         pclLevelT->setChecked(true);
      }

      pclLevelT = pclSubMenuT->addAction(tr("Error"));
      pclLevelT->setCheckable(true);
      pclLevelT->setData(eLOG_LEVEL_ERROR);
      if (logLevel(teCanChannelP) == eLOG_LEVEL_ERROR)
      {
         pclLevelT->setChecked(true);
      }

      pclLevelT = pclSubMenuT->addAction(tr("Warning"));
      pclLevelT->setCheckable(true);
      pclLevelT->setData(eLOG_LEVEL_WARN);
      if (logLevel(teCanChannelP) == eLOG_LEVEL_WARN)
      {
         pclLevelT->setChecked(true);
      }

      pclLevelT = pclSubMenuT->addAction(tr("Info"));
      pclLevelT->setCheckable(true);
      pclLevelT->setData(eLOG_LEVEL_INFO);
      if (logLevel(teCanChannelP) == eLOG_LEVEL_INFO)
      {
         pclLevelT->setChecked(true);
      }

      pclLevelT = pclSubMenuT->addAction(tr("Debug"));
      pclLevelT->setCheckable(true);
      pclLevelT->setData(eLOG_LEVEL_DEBUG);
      if (logLevel(teCanChannelP) == eLOG_LEVEL_DEBUG)
      {
         pclLevelT->setChecked(true);
      }

      pclLevelT = pclSubMenuT->addAction(tr("Trace"));
      pclLevelT->setCheckable(true);
      pclLevelT->setData(eLOG_LEVEL_TRACE);
      if (logLevel(teCanChannelP) == eLOG_LEVEL_TRACE)
      {
         pclLevelT->setChecked(true);
      }


      connect( pclSubMenuT, SIGNAL(triggered(QAction*)),
               this, SLOT(onChangeLogLevel(QAction*)));
      pclMenuT->addMenu(pclSubMenuT);

      pclMenuT->exec(apclLogTextP[slTabIndexT]->mapToGlobal(pos));

      delete pclMenuT;
    }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerLoggerView::show()                                                                                       //
// show the log window and ensure it is placed over all other windows                                                 //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerLoggerView::show(void)
{
   pclLogWindowP->show();
   pclLogWindowP->raise();
}
