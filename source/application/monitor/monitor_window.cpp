#include "monitor_window.hpp"
#include "qcan_socket_dialog.hpp"

MonitorWindow::MonitorWindow(QWidget *parent)
    : QMainWindow(parent)
{
   ui.setupUi(this);
   
   /*
   connect(ui.pclBtnConnectM, SIGNAL(pressed()),
           this, SLOT(onClientConnect()));
   
   connect(ui.pclBtnDisconnectM, SIGNAL(pressed()),
              this, SLOT(onClientDisconnect()));

   connect(ui.pclBtnNmtStartM, SIGNAL(pressed()),
              this, SLOT(onClientNmtStart()));

   connect(ui.pclBtnNmtStopM, SIGNAL(pressed()),
              this, SLOT(onClientNmtStop()));

   connect(ui.pclBtnNmtSyncM, SIGNAL(pressed()),
              this, SLOT(onClientSync()));

   */
   //pclCanSocketP = new QCanSocket;

   connect( ui.actionConnect, SIGNAL(triggered(bool)),
            this, SLOT(onSocketConnect(bool)));
   /*
   connect( pclCanSocketP, SIGNAL(framesReceived(uint32_t)),
            this, SLOT(onClientReceive(uint32_t)));

   connect( pclCanSocketP, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onSocketError(QAbstractSocket::SocketError)));

   */
}


MonitorWindow::~MonitorWindow()
{
   if (!pclCanSocketP.isNull())
   {
      delete(pclCanSocketP);
   }
}

void  MonitorWindow::onClientConnect(void)
{
   //QHostAddress clHostT("172.16.120.154");
   //pclCanSocketP->setHostAddress(clHostT);
   //pclCanSocketP->connectNetwork(eCAN_CHANNEL_1);
}

void  MonitorWindow::onClientDisconnect(void)
{
   //pclCanSocketP->disconnectNetwork();
}

void  MonitorWindow::onClientReceive(uint32_t ulFramesReceivedV)
{
   QCanFrame   clFrameT;
   QString     clMsgStringT;
   uint32_t    ulFrameCntT;

   for(ulFrameCntT = 0; ulFrameCntT < ulFramesReceivedV; ulFrameCntT++)
   {
      pclCanSocketP->readFrame(clFrameT);
      clMsgStringT = clFrameT.toString();
      ui.pclCanTraceP->append(clMsgStringT);
   }

}

void MonitorWindow::onSocketConnect(bool btCheckedV)
{
   Q_UNUSED(btCheckedV);

   QCanSocketDialog * pclDialogT;

   pclDialogT = new QCanSocketDialog();
   pclDialogT->show();
   connect(pclDialogT, SIGNAL(socketSelected(QCanSocket *)),
           this, SLOT(onSocketSelect(QCanSocket *)));
   qDebug() << "Geht weiter ..";
}


void MonitorWindow::onSocketError(QAbstractSocket::SocketError teSockErrorV)
{
   qDebug() << "onSocketError()" << teSockErrorV;
}

void MonitorWindow::onSocketSelect(QCanSocket * pclCanSocketV)
{
   if (!pclCanSocketP.isNull())
   {
      disconnect( pclCanSocketP, SIGNAL(framesReceived(uint32_t)),
               this, SLOT(onClientReceive(uint32_t)));
      
      delete(pclCanSocketP);
   }
   
   pclCanSocketP = pclCanSocketV;
   connect( pclCanSocketP, SIGNAL(framesReceived(uint32_t)),
            this, SLOT(onClientReceive(uint32_t)));
}
