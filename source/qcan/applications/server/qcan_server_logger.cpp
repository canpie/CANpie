


#include "qcan_server_logger.hpp"

#include <QDebug>

#include <QFileDialog>
#include <QFont>
#include <QFontDatabase>
#include <QMenu>


QCanServerLogger::QCanServerLogger()
{
   teCanChannelP = eCAN_CHANNEL_1;
   pclLogWindowP = new QMainWindow();
   pclLogTabP    = new QTabWidget();

   //----------------------------------------------------------------
   // add the Source Code Pro font
   //
   int slFontIdT = QFontDatabase::addApplicationFont(":/fonts/SourceCodePro-Medium.ttf");
   QString clFontFamilyT = QFontDatabase::applicationFontFamilies(slFontIdT).at(0);
   qDebug() << "Font family" << clFontFamilyT;
   QFont clFontT = QFont(clFontFamilyT, 12, 50);

   QString  clTabLabelT;
   for (uint8_t ubLogNumT = 0; ubLogNumT < QCAN_NETWORK_MAX; ubLogNumT++)
   {
      apclLogFileP[ubLogNumT] = Q_NULLPTR;

      apclLogTextP[ubLogNumT] = new QTextBrowser();
      apclLogTextP[ubLogNumT]->setFont(clFontT);
      clTabLabelT = QString(" CAN &%1 ").arg(ubLogNumT + 1);

      apclLogTextP[ubLogNumT]->setContextMenuPolicy(Qt::CustomContextMenu);
      connect( apclLogTextP[ubLogNumT],
               SIGNAL(customContextMenuRequested(const QPoint &)),
               this, SLOT(onShowLogMenu(const QPoint &)));

      pclLogTabP->addTab(apclLogTextP[ubLogNumT], clTabLabelT);
      setLogLevel((CAN_Channel_e) (ubLogNumT + 1), eLOG_LEVEL_NOTICE);

   }
   //----------------------------------------------------------------
   // the tab widget is the central widget, the initial log widget
   // size is 800 x 480 pixel
   //
   pclLogWindowP->setCentralWidget(pclLogTabP);
   pclLogWindowP->setWindowTitle("CANpie Server - Logging Window");
   pclLogWindowP->resize(800, 480);
}

QCanServerLogger::~QCanServerLogger()
{
   for (uint8_t ubLogNumT = 0; ubLogNumT < QCAN_NETWORK_MAX; ubLogNumT++)
   {
      if (apclLogFileP[ubLogNumT] != Q_NULLPTR)
      {
         qDebug() << "close log file " << ubLogNumT;
         apclLogFileP[ubLogNumT]->flush();
         apclLogFileP[ubLogNumT]->close();
      }
   }
   delete pclLogTabP;
   delete pclLogWindowP;
}


void QCanServerLogger::addLoggingSource(QObject *sender)
{

   connect( sender,
            SIGNAL(addLogMessage(const CAN_Channel_e,
                                 const QString &, LogLevel_e)),
            this,
            SLOT(appendMessage(const CAN_Channel_e,
                               const QString &, LogLevel_e)),
            Qt::DirectConnection);
}


//----------------------------------------------------------------------------//
// appendMessage()                                                            //
// append log message to log window for channel 'ubChannelV'                  //
//----------------------------------------------------------------------------//
void QCanServerLogger::appendMessage(const CAN_Channel_e ubChannelV,
                                     const QString & clLogMessageV,
                                     LogLevel_e teLogLevelV)
{
   if ((ubChannelV >= eCAN_CHANNEL_1) && (ubChannelV <= QCAN_NETWORK_MAX))
   {
      clLogMessageP.clear();

      if (teLogLevelV <= ateLogLevelP[ubChannelV - 1])
      {
         clTimeP = QDateTime::currentDateTime();
         clLogMessageP  = clTimeP.toString("hh:mm:ss.zzz - ");
         clLogMessageP += clLogMessageV;

         apclLogTextP[ubChannelV - 1]->append(clLogMessageP);

         if ((apclLogFileP[ubChannelV - 1]) != Q_NULLPTR)
         {
            apclLogFileP[ubChannelV - 1]->write(clLogMessageP.toLatin1());
            qDebug() << "write data to file";

         }
      }
   }
}

void QCanServerLogger::hide(void)
{
   pclLogWindowP->hide();
}

bool QCanServerLogger::isHidden(void)
{
   return (pclLogWindowP->isHidden());
}


LogLevel_e QCanServerLogger::logLevel(const CAN_Channel_e ubChannelV)
{
   LogLevel_e teLevelT = eLOG_LEVEL_INFO;

   if ((ubChannelV >= eCAN_CHANNEL_1) && (ubChannelV <= QCAN_NETWORK_MAX))
   {
      teLevelT = ateLogLevelP[ubChannelV - 1];

      qDebug() << "GET LOG LEVEL" << QString::number(teLevelT,10)<<" of Channel "<< QString::number(ubChannelV,10);
   } else
   {
      qDebug() << "-------------FAIL TO GET LOG LEVEL...";
   }

   return teLevelT;
}

//----------------------------------------------------------------------------//
// onChangeLogLevel()                                                         //
// change log level for all CAN chennels                                      //
//----------------------------------------------------------------------------//
void QCanServerLogger::onChangeLogLevel(QAction * pclActionV)
{
   setLogLevel(teCanChannelP, (LogLevel_e) pclActionV->data().toInt());
}


//----------------------------------------------------------------------------//
// onClearLog()                                                               //
// clear log window                                                           //
//----------------------------------------------------------------------------//
void QCanServerLogger::onClearLog(void)
{
   apclLogTextP[teCanChannelP - 1]->clear();
}

//----------------------------------------------------------------------------//
// onSetLogFile()                                                             //
// set log file                                                               //
//----------------------------------------------------------------------------//
void QCanServerLogger::onSetLogFile(void)
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


//----------------------------------------------------------------------------//
// showLogWindowMenu()                                                        //
// show context menu in log window                                            //
//----------------------------------------------------------------------------//
void QCanServerLogger::onShowLogMenu(const QPoint &pos)
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

      pclLevelT = pclSubMenuT->addAction(tr("Info"));
      pclLevelT->setCheckable(true);
      pclLevelT->setData(eLOG_LEVEL_INFO);
      if (ateLogLevelP[slTabIndexT] == eLOG_LEVEL_INFO)
      {
         pclLevelT->setChecked(true);
      }

      pclLevelT = pclSubMenuT->addAction(tr("Notice"));
      pclLevelT->setCheckable(true);
      pclLevelT->setData(eLOG_LEVEL_NOTICE);
      if (ateLogLevelP[slTabIndexT] == eLOG_LEVEL_NOTICE)
      {
         pclLevelT->setChecked(true);
      }

      pclLevelT = pclSubMenuT->addAction(tr("Warning"));
      pclLevelT->setCheckable(true);
      pclLevelT->setData(eLOG_LEVEL_WARN);
      if (ateLogLevelP[slTabIndexT] == eLOG_LEVEL_WARN)
      {
         pclLevelT->setChecked(true);
      }

      pclLevelT = pclSubMenuT->addAction(tr("Error"));
      pclLevelT->setCheckable(true);
      pclLevelT->setData(eLOG_LEVEL_ERROR);
      if (ateLogLevelP[slTabIndexT] == eLOG_LEVEL_ERROR)
      {
         pclLevelT->setChecked(true);
      }

      pclLevelT = pclSubMenuT->addAction(tr("Debug"));
      pclLevelT->setCheckable(true);
      pclLevelT->setData(eLOG_LEVEL_DEBUG);
      if (ateLogLevelP[slTabIndexT] == eLOG_LEVEL_DEBUG)
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

bool QCanServerLogger::setFileName(const CAN_Channel_e ubChannelV,
                                   QString fileName)
{
   QFile *  pclLogFileT;


   if ((ubChannelV >= eCAN_CHANNEL_1) && (ubChannelV <= QCAN_NETWORK_MAX))
   {
      if ((apclLogFileP[teCanChannelP - 1]) == Q_NULLPTR)
      {
         apclLogFileP[teCanChannelP - 1] = new QFile();
      }
      else
      {
         //-----------------------------------------------------
         // make sure the file is closed in case it was used
         // before
         //
         apclLogFileP[teCanChannelP - 1]->close();
      }


      pclLogFileT = apclLogFileP[teCanChannelP - 1];


      //--------------------------------------------------------
      // open the new file in rw mode for text files, existing
      // contents is truncated
      //
      pclLogFileT->setFileName(fileName);
      pclLogFileT->open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);

      //--------------------------------------------------------
      // write existing log data to the file
      //
      pclLogFileT->write(apclLogTextP[teCanChannelP - 1]->toPlainText().toLatin1());
      qDebug() << "write data to file";
      return true;
   }

   return false;
}

void QCanServerLogger::setLogLevel(const CAN_Channel_e ubChannelV,
                                   LogLevel_e teLogLevelV)
{
   qDebug() << "SET LOG LEVEL" << QString::number(teLogLevelV,10)<<" of Channel "<< QString::number(ubChannelV,10);

   if ((ubChannelV >= eCAN_CHANNEL_1) && (ubChannelV <= QCAN_NETWORK_MAX))
   {
      ateLogLevelP[ubChannelV - 1] = teLogLevelV;

      QString  clLogTextP;

      switch (teLogLevelV)
      {
         case eLOG_LEVEL_INFO:
            clLogTextP = tr("Set log level: Info");
            break;

         case eLOG_LEVEL_NOTICE:
            clLogTextP = tr("Set log level: Notice");
            break;

         case eLOG_LEVEL_WARN:
            clLogTextP = tr("Set log level: Warning");
            break;

         case eLOG_LEVEL_ERROR:
            clLogTextP = tr("Set log level: Error");
            break;

         case eLOG_LEVEL_DEBUG:
            clLogTextP = tr("Set log level: Debug");
            break;
      }

      appendMessage(ubChannelV, clLogTextP, teLogLevelV);
   }
}


//----------------------------------------------------------------------------//
// show()                                                                     //
// show the log window and ensure it is placed over all other windows         //
//----------------------------------------------------------------------------//
void QCanServerLogger::show(void)
{
   pclLogWindowP->show();
   pclLogWindowP->raise();
}
