#include "client_demo.hpp"

ClientDemo::ClientDemo(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	
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

	pclCanSocketP = new QCanSocket;

	connect( pclCanSocketP, SIGNAL(framesReceived(uint32_t)),
	         this, SLOT(onClientReceive(uint32_t)));

	connect( pclCanSocketP, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onSocketError(QAbstractSocket::SocketError)));


}


ClientDemo::~ClientDemo()
{
   delete(pclCanSocketP);
}

void  ClientDemo::onClientConnect(void)
{
   //QHostAddress clHostT("172.16.120.154");
   //pclCanSocketP->setHostAddress(clHostT);
   pclCanSocketP->connectNetwork(eCAN_CHANNEL_1);
}

void  ClientDemo::onClientDisconnect(void)
{
   pclCanSocketP->disconnectNetwork();
}

void  ClientDemo::onClientReceive(uint32_t ulFramesReceivedV)
{
   QCanFrame   clFrameT;
   QString     clMsgStringT;
   uint32_t    ulFrameCntT;

   for(ulFrameCntT = 0; ulFrameCntT < ulFramesReceivedV; ulFrameCntT++)
   {
      pclCanSocketP->readFrame(clFrameT);
      clMsgStringT = clFrameT.toString();
      ui.pclCanReceive->append(clMsgStringT);
   }

}

void  ClientDemo::onClientNmtStart(void)
{
   QCanFrame   clCanFrameT;

   clCanFrameT.setIdentifier(0x00);
   clCanFrameT.setDlc(2);
   clCanFrameT.setData(0, 1);
   clCanFrameT.setData(1, 0);

   pclCanSocketP->writeFrame(clCanFrameT);
}

void  ClientDemo::onClientNmtStop(void)
{
   QCanFrame   clCanFrameT;

   clCanFrameT.setFrameType(QCanFrame::eTYPE_FD_STD);
   clCanFrameT.setIdentifier(0x60A);
   clCanFrameT.setDlc(12);
   clCanFrameT.setData(0, 0x22);
   clCanFrameT.setData(1, 0x17);
   clCanFrameT.setData(2, 0x10);
   clCanFrameT.setData(3, 0x00);
   clCanFrameT.setData(4, 0x32);
   clCanFrameT.setData(5, 0x00);
   clCanFrameT.setData(6, 0x00);
   clCanFrameT.setData(7, 0x00);

   pclCanSocketP->writeFrame(clCanFrameT);
}

void  ClientDemo::onClientSync(void)
{
   QCanFrame   clCanFrameT;

   clCanFrameT.setIdentifier(0x80);
   clCanFrameT.setDlc(0);

   pclCanSocketP->writeFrame(clCanFrameT);
}

void ClientDemo::onSocketError(QAbstractSocket::SocketError teSockErrorV)
{
   qDebug() << "onSocketError()" << teSockErrorV;
}
