#ifndef QCAN_NETWORK_HPP_
#define QCAN_NETWORK_HPP_

#include <QMutex>
#include <QPointer>
#include <QThread>

#include "qcan_frame.hpp"
#include "qcan_frame_api.hpp"
#include "qcan_frame_error.hpp"


class QCanInterface;
class QCanStub;



//-----------------------------------------------------------------------------
/*!
** \class QCanNetwork
** \brief CAN network representation
** 
** This class represents a CAN network with a unique bitrate.
** It supports one physical CAN interface, which can be assigned
** during run-time to the CAN network and a limited number of
** virtual CAN interfaces (sockets).
**
*/

class QCanNetwork : public QThread
{
   Q_OBJECT
public:
   
   /*! add network with channel number */
   QCanNetwork();
	
	
	~QCanNetwork();

	bool addInterface(QCanInterface * pclCanIfV);

	void removeInterface(void);

	//void  connectStub(const QCanStub & clStubV);
   void  connectStub(QCanStub * pclStubV);

	void  disconnectStub(QCanStub *pclStubV);

	uint8_t  load();

   uint32_t       stubCount();

protected:
   void           run(void);
   
private:

   void  handleApiFrame(int32_t & slStubSrcR, QCanFrameApi & clApiFrameR);
   void  handleErrorFrame(int32_t & slStubSrcR, QCanFrameError & clErrorFrameR);
   void  handleQCanFrame(int32_t & slStubSrcR, QCanFrame & clCanFrameR);


   QPointer<QCanInterface> pclInterfaceP;
   QVector<QCanStub *> *   pclStubListP;
   static uint8_t          ubNetIdP;
   QString                 clNetNameP;
   QMutex                  clStubMutexP;
};

#endif   // QCAN_NETWORK_HPP_
