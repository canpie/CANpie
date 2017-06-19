#ifndef QCAN_SERVER_DIALOG_HPP_
#define QCAN_SERVER_DIALOG_HPP_


#include <QObject>
#include <QAction>
#include <QDialog>
#include <QMenu>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QToolBox>

#include "ui_ServerConfig.h"
#include "qcan_server.hpp"
#include "qcan_server_logger.hpp"
#include "qcan_interface_widget.hpp"

using namespace QCan;


class QCanServerDialog : public QDialog
{
    Q_OBJECT

public:
    QCanServerDialog(QWidget *parent = 0);
    ~QCanServerDialog();


public slots:

   //void onNetworkConfInterface(QMouseEvent *);

   /*!
   ** This slot is triggered on change of the physical CAN interface.
   ** The parameter \c pclCanInterfaceV is a pointer to a new interface. If
   ** \c pclCanInterfaceV is NULL, the physical interface shall be removed.
   */
   void onInterfaceChange(uint8_t ubIdxV, QCanInterface * pclCanIfV);

   void onNetworkShowCanFrames(uint32_t ulFrameCntV);
   void onNetworkShowErrFrames(uint32_t ulFrameCntV);
   void onNetworkShowLoad(uint8_t ubLoadV, uint32_t ulMsgPerSecV);
   void onNetworkShowState(CAN_State_e teStateV);


private slots:
   void onNetworkChange(int slIndexV);
   void onNetworkConfBitrateDat(int slBitrateV);
   void onNetworkConfBitrateNom(int slBitrateV);
   void onNetworkConfCanFd(bool btEnableV);
   void onNetworkConfEnable(bool btEnableV);
   void onNetworkConfErrorFrames(bool btEnableV);
   void onNetworkConfListenOnly(bool btEnableV);
   void onNetworkReset(bool btCheckedV);
   
   void onServerConfAddress(int slIndexV);
   void onServerConfTime(int slValueV);

   void onLoggingWindow(void);

private:
   void     createActions(void);
   void     createTrayIcon(void);
   void     setupNetworks(void);
   void     showNetworkConfiguration(void);
   void     setIcon(void);
   void     updateUI(uint8_t ubNetworkIdxV);

   Ui_ServerConfig         ui;
   QAction *               pclActionConfigP;
   QAction *               pclActionLoggingP;
   QAction *               pclActionQuitP;

   QSystemTrayIcon *       pclIconTrayP;
   QMenu *                 pclMenuTrayP;

   QCanServer *            pclCanServerP;
   QSettings *             pclSettingsP;

   int32_t                 slLastNetworkIndexP;

   QToolBox *              pclTbxNetworkP;
   QCanInterfaceWidget *   apclCanIfWidgetP[QCAN_NETWORK_MAX];
   QCanServerLogger *      pclLoggerP;

};

#endif // QCAN_SERVER_DIALOG_HPP_
