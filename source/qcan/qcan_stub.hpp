#ifndef QCAN_STUB_HPP_
#define QCAN_STUB_HPP_

#include <QPointer>
#include <QMutex>
#include <QUuid>
#include <QVector>

#include "qcan_defs.hpp"
#include "qcan_frame.hpp"
#include "qcan_network.hpp"


//-----------------------------------------------------------------------------
/*!
** \class QCanStub
** \brief CAN device
** 
** This class represents a CAN network with a unique bitrate.
** It supports one physical CAN interface, which can be assigned
** during run-time to the CAN network and a limited number of
** virtual CAN interfaces (sockets).
**
*/

class QCanStub
{

public:
   
   QCanStub();
	
	virtual ~QCanStub();

	bool connectNetwork(QCanNetwork * pclNetworkV);
	void disconnectNetwork(void);


   bool  readFrame(QCanFrame & clFrameR);

   bool  setMode(QCan::CAN_Mode_e & ubModeR);

   bool  setFifoSize(uint16_t & uwRcvFifoSizeR, uint16_t & uwTrmFifoSizeR);

   /* get error state */
   QCan::CAN_State_e state(void);

   bool  writeFrame(const QCanFrame & clFrameR);

   friend class QCanNetwork;

protected:
   bool  popTransmitFifo(QCanFrame & clFrameR);
   bool  pushReceiveFifo(const QCanFrame & clFrameR);

private:
   
   QUuid                   clUuidP;    // identifier
   QPointer<QCanNetwork>   pclNetworkP;
   QVector<QCanFrame>      clRcvFifoP;
   QVector<QCanFrame>      clTrmFifoP;
   QMutex                  clRcvMutexP;
   QMutex                  clTrmMutexP;

   QCan::CAN_State_e       ubCanStateP;

   bool  btIsConnectedP;
};

#endif   // QCAN_STUB_HPP_
