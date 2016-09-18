#ifndef QCAN_SOCKET_HPP_
#define QCAN_SOCKET_HPP_


#include <QHostAddress>
#include <QPointer>
#include <QTcpSocket>
#include <QVector>

#include "qcan_defs.hpp"
#include "qcan_frame.hpp"

//-----------------------------------------------------------------------------
/*!
** \class QCanSocket
** \brief CAN socket
** 
**
*/

class QCanSocket : public QObject
{
   Q_OBJECT

public:
   
   QCanSocket(QObject * pclParentV = Q_NULLPTR);
	
	virtual ~QCanSocket();

   /*!
   ** \see  disconnectNetwork()
   **
   ** Connect the CAN stub to a CAN network.
   */
   bool connectNetwork(QCan::CAN_Channel_e ubChannelV);


   /*!
   ** \see  connectNetwork()
   **
   ** Disconnect the CAN stub from a CAN network.
   */
   void disconnectNetwork(void);

   int32_t  framesAvailable(void) const;

   bool isConnected(void);

   /*!
   ** Get the Universal unique identifier of the CAN stub.
   */
   QString  uuidString(void);

   /*!
   ** Get error state
   **
   */
   QCan::CAN_State_e state(void);

   bool  setMode(QCan::CAN_Mode_e & ubModeR);


   bool  readFrame(QCanFrame & clFrameR);

   /*!
   ** \see  readFrame()
   **
   ** Write a CAN frame to the CAN socket.
   */
   bool  writeFrame(const QCanFrame & clFrameR);



public slots:
   void  onSocketConnect(void);
   void  onSocketDisconnect(void);
   void  onSocketError(QAbstractSocket::SocketError eSocketErrorV);
   virtual void  onSocketReceive(void);

signals:
   void  framesReceived(uint32_t ulFrameCntV);

protected:

private:

   QPointer<QTcpSocket> pclTcpSockP;
   QHostAddress         clTcpHostAddrP;
   uint16_t             uwTcpPortP;
   bool                 btIsConnectedP;
};

#endif   // QCAN_SOCKET_HPP_
