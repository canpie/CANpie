#include "qcan_client.hpp"
#include "cp_core.h"



QCanClient::QCanClient(QObject *parent)
{
    pclRcvSocketP = new QUdpSocket();
    pclTrmSocketP = new QUdpSocket();
	
}


QCanClient::~QCanClient()
{

}

int QCanClient::sendDatagram(QCanFrame & clCanFrameR)
{
   return 0;
}



static QCanClient *  pclCanpieS;

//----------------------------------------------------------------------------//
// CpCoreDriverInit()                                                         //
// init CAN controller                                                        //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverInit(uint8_t ubPhyIfV, CpPort_ts * ptsPortV)
{

   pclCanpieS = new QCanClient();


   return (CpErr_OK);
}

//----------------------------------------------------------------------------//
// CpCoreMsgRead()                                                            //
// dummy implementation, use CPP_PARM_UNUSED to avoid compiler warning        //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreMsgRead( CpPort_ts *   CPP_PARM_UNUSED(ptsPortV),
                           CpCanMsg_ts * CPP_PARM_UNUSED(ptsBufferV),
                           uint32_t *    CPP_PARM_UNUSED(pulBufferSizeV))
{
   return (CpErr_OK);
}
