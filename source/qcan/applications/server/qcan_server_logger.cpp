


#include "qcan_server_logger.hpp"

#include <QDebug>

#include <QFont>
#include <QFontDatabase>

QCanServerLogger::QCanServerLogger()
{
   teLogLevelP = eLOG_LEVEL_INFO;
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
   for (uint8_t ubLogTextT = 0; ubLogTextT < QCAN_NETWORK_MAX; ubLogTextT++)
   {
      apclLogTextP[ubLogTextT] = new QTextBrowser();
      apclLogTextP[ubLogTextT]->setFont(clFontT);
      clTabLabelT = QString(" CAN &%1 ").arg(ubLogTextT + 1);
      pclLogTabP->addTab(apclLogTextP[ubLogTextT], clTabLabelT);
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


void QCanServerLogger::appendMessage(const CAN_Channel_e ubChannelV,
                                     const QString & clLogMessageV,
                                     LogLevel_e teLogLevelV)
{
   clLogMessageP.clear();

   if (teLogLevelV >= teLogLevelP)
   {
      clTimeP = QDateTime::currentDateTime();
      clLogMessageP  = clTimeP.toString("hh:mm:ss.zzz - ");
      clLogMessageP += clLogMessageV;
      qDebug() << ubChannelV << clLogMessageV;

      apclLogTextP[ubChannelV - 1]->append(clLogMessageP);
   }
}


void QCanServerLogger::show(void)
{
   pclLogWindowP->show();
}
