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


   /** set the name of the log file */
   bool setFileName(QString fileName);

   /** set minimum log level that goes to a file */
   void setLogLevel( LogLevel_e teLogLevelV);

   void show(void);

   void hide(void);

signals:
   void  showLogMessage(const QString & clLogMessageV);

public slots:
   void appendMessage(const CAN_Channel_e ubChannelV,
                      const QString & clLogMessageV,
                      LogLevel_e teLogLevelV = eLOG_LEVEL_INFO);

private slots:

private:
   QDateTime      clTimeP;
   QString        clLogMessageP;
   LogLevel_e     teLogLevelP;
   QMainWindow  * pclLogWindowP;
   QTabWidget   * pclLogTabP;
   QTextBrowser * apclLogTextP[QCAN_NETWORK_MAX];
};

#endif // QCAN_SERVER_LOGGER_HPP_
