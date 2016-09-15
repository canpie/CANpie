#ifndef QCAN_STUB_HPP_
#define QCAN_STUB_HPP_

#include <QPointer>
#include <QLocalSocket>
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
**
*/

class QCanStub : public QObject
{
   Q_OBJECT

public:
   
   /*!
   ** Create new CAN stub with unique identifier. The Uuid value can
   ** be obtained by the method uuidString().
   */
   QCanStub(QObject *parent = Q_NULLPTR);
	
	~QCanStub();

	/*!
	** \see  disconnectNetwork()
	**
	** Connect the CAN stub to a CAN network.
	*/
	bool connectNetwork(QCanNetwork * pclNetworkV);

	bool connectNetwork(QString clNetworkNameV);


   /*!
   ** \see  connectNetwork()
   **
   ** Disconnect the CAN stub from a CAN network.
   */
	void disconnectNetwork(void);

	void disconnectNet(void);

	/*!
	** \see  writeFrame()
	**
	** Read CAN frame from the CAN stub. If no frame is avaliable
	** inside the receive FIFO, the method will return false.
	*/
   bool  readFrame(QCanFrame & clFrameR);

   bool  setMode(QCan::CAN_Mode_e & ubModeR);

   bool  setFifoSize(uint16_t & uwRcvFifoSizeR, uint16_t & uwTrmFifoSizeR);

   /*!
   ** Get error state
   **
   */
   QCan::CAN_State_e state(void);

   /*!
   ** Get the Universal unique identifier of the CAN stub.
   */
   QString  uuidString(void);

   /*!
   ** \see  readFrame()
   **
   ** Write a CAN frame to the CAN stub.
   */
   bool  writeFrame(const QCanFrame & clFrameR);

public slots:

   void onSocketConnect();
   void onSocketDisconnect();

   void onSocketError(QLocalSocket::LocalSocketError socketError);


protected:
   bool  popTransmitFifo(QCanFrame & clFrameR);
   bool  pushReceiveFifo(const QCanFrame & clFrameR);

private:
   friend class QCanNetwork;

   QUuid                   clUuidP;    // identifier
   QPointer<QCanNetwork>   pclNetworkP;
   QPointer<QLocalSocket>  pclLocalSocketP;
   QVector<QCanFrame>      clRcvFifoP;
   QVector<QCanFrame>      clTrmFifoP;
   QMutex                  clRcvMutexP;
   QMutex                  clTrmMutexP;
   QCan::CAN_State_e       ubCanStateP;

   bool  btIsConnectedP;
};

#endif   // QCAN_STUB_HPP_
