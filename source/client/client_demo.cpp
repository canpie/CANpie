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

	pclCanSocketP = new QCanSocket;

	connect( pclCanSocketP, SIGNAL(framesReceived(uint32_t)),
	         this, SLOT(onClientReceive(uint32_t)));
}


ClientDemo::~ClientDemo()
{
   delete(pclCanSocketP);
}

void  ClientDemo::onClientConnect(void)
{
   pclCanSocketP->connectNetwork(QCan::eCAN_CHANNEL_1);
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

   clCanFrameT.setStdId(0x00);
   clCanFrameT.setDlc(2);
   clCanFrameT.setData(0, 1);
   clCanFrameT.setData(1, 0);

   pclCanSocketP->writeFrame(clCanFrameT);
}

void  ClientDemo::onClientSync(void)
{
   QCanFrame   clCanFrameT;

   clCanFrameT.setStdId(0x80);
   clCanFrameT.setDlc(0);

   pclCanSocketP->writeFrame(clCanFrameT);
}
