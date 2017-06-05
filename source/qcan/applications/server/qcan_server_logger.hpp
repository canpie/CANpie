#ifndef QCAN_SERVER_LOGGER_HPP_
#define QCAN_SERVER_LOGGER_HPP_


#include <QDateTime>
#include <QObject>
#include <QMainWindow>
#include <QTextBrowser>

#include "qcan_defs.hpp"
#include "qcan_namespace.hpp"


using namespace QCan;

class QCanServerLogger : public QObject
{
    Q_OBJECT

public:
    QCanServerLogger();
    ~QCanServerLogger();

    /** register class for logging
    ** allow to send log messages */
   void addLoggingSource(QObject *sender);

   bool isHidden(void);

   /** set the name of the log file */
   bool setFileName(const CAN_Channel_e ubChannelV, QString fileName);

   /** set minimum log level that goes to a file */
   void setLogLevel(const CAN_Channel_e ubChannelV, LogLevel_e teLogLevelV);

   void show(void);

   void hide(void);

public slots:
   void appendMessage(const CAN_Channel_e ubChannelV,
                      const QString & clLogMessageV,
                      LogLevel_e teLogLevelV = eLOG_LEVEL_INFO);

private slots:
   void onChangeLogLevel(QAction * pclActionV);
   void onClearLog(void);
   void onSetLogFile(void);
   void onShowLogMenu(const QPoint &pos);

private:
   QDateTime      clTimeP;
   QString        clLogMessageP;
   QString        aclLogFileP[QCAN_NETWORK_MAX];
   LogLevel_e     ateLogLevelP[QCAN_NETWORK_MAX];
   CAN_Channel_e  teCanChannelP;
   QMainWindow  * pclLogWindowP;
   QTabWidget   * pclLogTabP;
   QTextBrowser * apclLogTextP[QCAN_NETWORK_MAX];
};

#endif // QCAN_SERVER_LOGGER_HPP_
