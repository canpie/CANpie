#ifndef QCAN_SERVER_DIALOG_HPP_
#define QCAN_SERVER_DIALOG_HPP_


#include <QAction>
#include <QDialog>
#include <QMenu>
#include <QSettings>
#include <QSystemTrayIcon>

#include "ui_ServerConfig.h"
#include "qcan_server.hpp"

class QCanServerDialog : public QDialog
{
    Q_OBJECT

public:
    QCanServerDialog(QWidget *parent = 0);
    ~QCanServerDialog();

public slots:

   void onNetworkChange(int slIndexV);
   void onNetworkConfBitrate(int slBitrateV);
   void onNetworkConfEnable(bool btEnableV);
   void onNetworkConfListenOnly(bool btEnableV);
   void onNetworkConfInterface(QMouseEvent *);

   void onNetworkShowLoad(int slLoadV);

private:

   uint8_t  currentNetwork(void);
   void     createActions(void);
   void     createTrayIcon(void);
    void setupNetworks(void);
    void showNetworkConfiguration(void);
    void setIcon(void);
   void     updateUI(uint8_t ubNetworkIdxV);

    Ui_ServerConfig   ui;
    QAction *         pclActionCnfgP;
    QAction *         pclActionQuitP;

    QSystemTrayIcon * pclIconTrayP;
    QMenu *           pclMenuTrayP;

    QCanServer *      pclCanServerP;
    QSettings *       pclSettingsP;


};

#endif // QCAN_SERVER_DIALOG_HPP_
