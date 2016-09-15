#ifndef QCAN_NETWORK_HPP_
#define QCAN_NETWORK_HPP_


#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>
#include <QPointer>
#include <QTimer>

#include "qcan_frame.hpp"
#include "qcan_frame_api.hpp"
#include "qcan_frame_error.hpp"


class QCanInterface;



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

class QCanNetwork : public QObject
{
   Q_OBJECT
public:
   
   /*!
   ** Create new CAN network with unique channel number.
   */
   QCanNetwork(QObject * pclParentV = Q_NULLPTR,
               uint16_t  uwPortV = QCAN_TCP_DEFAULT_PORT);
	
	
	~QCanNetwork();

	/*!
	** \param   pclCanIfV - Pointer to physical CAN interface
	** \see     removeInterface()
	**
	** Add a physical CAN interface to the CAN network.
	** Each CAN network supports only one physical CAN interface.
	** The CAN interface is removed from the network by calling
	** the removeInterface() method.
	*/
	bool addInterface(QCanInterface * pclCanIfV);

	int32_t  bitrate(void)           {return (slBitrateP);      };

	uint32_t dispatcherTime(void)    {return (ulDispatchTimeP); };

   bool hasErrorFramesSupport(void);

   bool hasFastDataSupport(void);

   bool hasListenOnlySupport(void);

   bool isErrorFramesEnabled(void)  {return (btErrorFramesEnabledP); };

   bool isFastDataEnabled(void)     {return (btFastDataEnabledP);    };

   bool isListenOnlyEnabled(void)   {return (btListenOnlyEnabledP);  };

   bool isNetworkEnabled(void)      {return (btNetworkEnabledP);     };


	/*!
	** Show actual network load in percent.
	**
	*/
	uint8_t  load();

	QString  name()   { return(clNetNameP); };

   /*!
   ** \see     addInterface()
   **
   ** Remove a physical CAN interface from the CAN network.
   */
	void removeInterface(void);

	void setBitrate(int32_t slBitrateV, int32_t slBrsClockV);

	void setDispatcherTime(uint32_t ulTimeV);

   void setErrorFramesEnabled(bool btEnableV = true);

   void setFastDataEnabled(bool btEnableV = true);

   void setListenOnlyEnabled(bool btEnableV = true);

   void setNetworkEnabled(bool btEnableV = true);


public slots:

   void onSocketConnect(void);

   void onSocketDisconnect(void);
   void onTimerEvent(void);

signals:
   void  showApiFrames(uint32_t ulFrameTotalV);
   void  showCanFrames(uint32_t ulFrameTotalV);
   void  showErrFrames(uint32_t ulFrameTotalV);
   void  showLoad(uint8_t ubLoadV, uint32_t ulMsgPerSecV);

protected:

private:

   bool  handleApiFrame(int32_t & slSockSrcR, QCanFrameApi & clApiFrameR);
   bool  handleCanFrame(int32_t & slSockSrcR, QByteArray & clSockDataR);
   bool  handleErrFrame(int32_t & slSockSrcR, QCanFrameError & clErrorFrameR);


   QPointer<QCanInterface> pclInterfaceP;
   QPointer<QTcpServer>    pclTcpSrvP;
   QVector<QTcpSocket *> * pclTcpSockListP;
   QHostAddress            clTcpHostAddrP;
   uint16_t                uwTcpPortP;
   static uint8_t          ubNetIdP;
   QString                 clNetNameP;
   QMutex                  clTcpSockMutexP;
   QTimer                  clDispatchTmrP;
   uint32_t                ulDispatchTimeP;
   int32_t                 slBitrateP;
   int32_t                 slBrsClockP;

   uint32_t                ulCntFrameApiP;
   uint32_t                ulCntFrameCanP;
   uint32_t                ulCntFrameErrP;

   bool                    btErrorFramesEnabledP;
   bool                    btFastDataEnabledP;
   bool                    btListenOnlyEnabledP;
   bool                    btNetworkEnabledP;
};

#endif   // QCAN_NETWORK_HPP_
