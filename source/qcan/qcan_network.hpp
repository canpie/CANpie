//============================================================================//
// File:          qcan_network.hpp                                            //
// Description:   QCAN classes - CAN network                                  //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53844 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions         //
// are met:                                                                   //
// 1. Redistributions of source code must retain the above copyright          //
//    notice, this list of conditions, the following disclaimer and           //
//    the referenced file 'LICENSE'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'LICENSE' file.                   //
//                                                                            //
//============================================================================//


#ifndef QCAN_NETWORK_HPP_
#define QCAN_NETWORK_HPP_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>
#include <QPointer>
#include <QTimer>

#include "qcan_frame.hpp"
#include "qcan_frame_api.hpp"
#include "qcan_frame_error.hpp"

/*----------------------------------------------------------------------------*\
** Referenced classes                                                         **
**                                                                            **
\*----------------------------------------------------------------------------*/
class QCanInterface;


//-----------------------------------------------------------------------------
/*!
** \file    qcan_network.hpp
** \brief   CAN network
**
** This file ...
**
*/


//-----------------------------------------------------------------------------
/*!
** \class QCanNetwork
** \brief CAN network representation
** 
** This class represents a CAN network with a unique bitrate.
** It supports one physical CAN interface, which can be assigned
** during run-time to the CAN network and a limited number of
** virtual CAN interfaces (sockets). Clients can connect to a QCanNetwork
** via the QCanSocket class.
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
	** \see     removeInterface()
	**
	** The function adds a physical CAN interface to the CAN network.
	** Each CAN network supports only one physical CAN interface.
	** The CAN interface is removed from the network by calling
	** the removeInterface() method. The parameter \c pclCanIfV is a pointer
	** to an instance of a QCanInterface class.
	** <p>
	** The function returns \c true if the CAN interface is added, otherwise
	** it will return \c false.
	*/
	bool addInterface(QCanInterface * pclCanIfV);

   /*!
   ** \see     setBitrate()
   **
   ** This function returns the current bit-rate of the CAN network.
   ** For <b>classic CAN</b>, the return value defines the bit-rate for the
   ** complete frame.
   ** For <b>CAN FD</b> the return value defines the bit-rate for
   ** the arbitration phase.
   ** <p>
   ** If no bit-rate is configured, the function will return
   ** QCan::eCAN_BITRATE_NONE.
   */
	int32_t  bitrate(void)           {return (slBitrateP);      };

   /*!
   ** \see     setDispatcherTime()
   **
   ** This function returns the current dispatcher time for the
   ** internal CAN frame handler in milli-seconds.
   */
	uint32_t dispatcherTime(void)    {return (ulDispatchTimeP); };

   bool hasErrorFramesSupport(void);

   bool hasFastDataSupport(void);

   bool hasListenOnlySupport(void);

   bool isErrorFramesEnabled(void)  {return (btErrorFramesEnabledP); };

   bool isFastDataEnabled(void)     {return (btFastDataEnabledP);    };

   bool isListenOnlyEnabled(void)   {return (btListenOnlyEnabledP);  };

   /*!
   ** \see     setNetworkEnabled()
   **
   ** This function returns \c true if the network is enabled,
   ** otherwise it returns \c false.
   */
   bool isNetworkEnabled(void)      {return (btNetworkEnabledP);     };


	QString  name()   { return(clNetNameP); };

	void reset(void);

   /*!
   ** \see     addInterface()
   **
   ** Remove a physical CAN interface from the CAN network.
   */
	void removeInterface(void);

   /*!
   ** \see     bitrate()
   **
   ** This function sets the bit-rate for the CAN network. For <b>classic CAN</b>,
   ** the parameter \c slBitrateV defines the bit-rate for the complete frame,
   ** the parameter \c slBrsClockV is not evaluated in that case.
   ** For <b>CAN FD</b> the parameter \c slBitrateV defines the bit-rate for
   ** the arbitration phase, the parameter \c slBrsClockV defines the
   ** bit-rate for the data phase.
   ** <p>
   ** For selection of pre-defined bit-rates the value can be taken from
   ** the enumeration QCan::CAN_Bitrate_e.
   */
	void setBitrate(int32_t slBitrateV,
	                int32_t slBrsClockV = QCan::eCAN_BITRATE_NONE);


   /*!
   ** \see     dispatcherTime()
   **
   ** This function sets the dispatcher time for the internal CAN frame
   ** handler in milli-seconds.
   */
	void setDispatcherTime(uint32_t ulTimeV);

   void setErrorFramesEnabled(bool btEnableV = true);

   void setFastDataEnabled(bool btEnableV = true);

   void setListenOnlyEnabled(bool btEnableV = true);

   /*!
   ** \see     isNetworkEnabled()
   **
   ** This function enables the dispatching of CAN frames if \c btEnable is
   ** \c true. It is disabled for <c>btEnable = false</c>.
   */
   void setNetworkEnabled(bool btEnableV = true);


public slots:
   /*!
   ** This function is called upon socket connection.
   */
   void onSocketConnect(void);

   /*!
   ** This function is called upon socket disconnection.
   */
   void onSocketDisconnect(void);

   void onTimerEvent(void);

signals:
   /*!
   ** This signal is emitted ..
   */
   void  showApiFrames(uint32_t ulFrameTotalV);
   void  showCanFrames(uint32_t ulFrameTotalV);
   void  showErrFrames(uint32_t ulFrameTotalV);
   void  showLoad(uint8_t ubLoadV, uint32_t ulMsgPerSecV);

protected:

private:

   bool  handleApiFrame(int32_t & slSockSrcR, QCanFrameApi & clApiFrameR);
   bool  handleCanFrame(int32_t & slSockSrcR, QByteArray & clSockDataR);
   bool  handleErrFrame(int32_t & slSockSrcR, QByteArray & clSockDataR);


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

   //----------------------------------------------------------------
   // statistic frame counter
   //
   uint32_t                ulCntFrameApiP;
   uint32_t                ulCntFrameCanP;
   uint32_t                ulCntFrameErrP;

   //----------------------------------------------------------------
   // statistic timing
   //
   uint32_t                ulStatisticTickP;
   uint32_t                ulStatisticTimeP;
   uint32_t                ulFramePerSecMaxP;
   uint32_t                ulFrameCntSaveP;

   bool                    btErrorFramesEnabledP;
   bool                    btFastDataEnabledP;
   bool                    btListenOnlyEnabledP;
   bool                    btNetworkEnabledP;
};

#endif   // QCAN_NETWORK_HPP_
