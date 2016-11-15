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

using namespace CANpie;

/*----------------------------------------------------------------------------*\
** Referenced classes                                                         **
**                                                                            **
\*----------------------------------------------------------------------------*/
class QCanInterface;




//-----------------------------------------------------------------------------
/*!
** \class QCanNetwork
** \brief CAN network representation
** 
** This class represents a CAN network with a unique bit-rate.
** It supports one physical CAN interface (QCanInterface), which can be
** assigned during run-time to the CAN network and a limited number of
** virtual CAN interfaces (sockets). Clients can connect to a QCanNetwork
** via the QCanSocket class.
**
*/
class QCanNetwork : public QObject
{
   Q_OBJECT
public:
   
   /*!
   ** \param[in]  pclParentV     Pointer to QObject parent class
   ** \param[in]  uwPortV        Port number
   **
   ** Create new CAN network with unique channel number.
   */
   QCanNetwork(QObject * pclParentV = Q_NULLPTR,
               uint16_t  uwPortV = QCAN_TCP_DEFAULT_PORT);
	
	
	~QCanNetwork();

	/*!
   ** \param[in]  pclCanIfV     Pointer to CAN interface class
   ** \return     \c true if CAN interface added successfully
	** \see        removeInterface()
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
   ** \return     Bit-rate value for Nominal Bit Timing
   ** \see        setBitrate()
   **
   ** This function returns the current bit-rate of the CAN network.
   ** For <b>classic CAN</b>, the return value defines the bit-rate for the
   ** complete frame.
   ** For <b>CAN FD</b> the return value defines the bit-rate for
   ** the arbitration phase.
   ** <p>
   ** If no bit-rate is configured, the function will return
   ** CANpie::eCAN_BITRATE_NONE.
   */
	inline int32_t  bitrate(void)          {  return (slNomBitRateP);    };

	inline int32_t  nominalBitrate(void)   {  return (slNomBitRateP);    };

	inline int32_t  dataBitrate(void)      {  return (slDatBitRateP);    };

   /*!
   ** \return     Current dispatcher time
   ** \see        setDispatcherTime()
   **
   ** This function returns the current dispatcher time for the
   ** internal CAN frame handler in milliseconds.
   */
	uint32_t dispatcherTime(void)    {return (ulDispatchTimeP); };

   bool hasErrorFramesSupport(void);

   bool hasFastDataSupport(void);

   bool hasListenOnlySupport(void);

   bool isErrorFramesEnabled(void)  {return (btErrorFramesEnabledP); };

   bool isFastDataEnabled(void)     {return (btFastDataEnabledP);    };

   bool isListenOnlyEnabled(void)   {return (btListenOnlyEnabledP);  };

   /*!
   ** \return     \c true if CAN is enabled
   ** \see        setNetworkEnabled()
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

   QHostAddress serverAddress(void);

   /*!
   ** \param[in]  slNomBitRateV  Nominal Bit-rate value
   ** \param[in]  slDatBitRateV  Data Bit-rate value
   ** \see        bitrate()
   **
   ** This function sets the bit-rate for the CAN network. For <b>classic CAN</b>,
   ** the parameter \c slBitrateV defines the bit-rate for the complete frame,
   ** the parameter \c slBrsClockV is not evaluated in that case.
   ** For <b>CAN FD</b> the parameter \c slBitrateV defines the bit-rate for
   ** the arbitration phase, the parameter \c slBrsClockV defines the
   ** bit-rate for the data phase.
   ** <p>
   ** For selection of predefined bit-rates the value can be taken from
   ** the enumeration CANpie::CAN_Bitrate_e.
   */
	void setBitrate(int32_t slNomBitRateV,
	                int32_t slDatBitRateV = eCAN_BITRATE_NONE);


   /*!
   ** \param[in]  ulTimeV        Dispatcher time
   ** \see        dispatcherTime()
   **
   ** This function sets the dispatcher time for the internal CAN frame
   ** handler in milliseconds.
   */
	void setDispatcherTime(uint32_t ulTimeV);


   /*!
   ** \param[in]  btEnableV      Enable / disable error frames
   ** \see        isErrorFramesEnabled()
   **
   ** This function enables the dispatching of CAN error frames if \a btEnable
   ** is \c true, it is disabled on \c false.
   */
   void setErrorFramesEnabled(bool btEnableV = true);


   void setFastDataEnabled(bool btEnableV = true);

   void setListenOnlyEnabled(bool btEnableV = true);

   /*!
   ** \param[in]  btEnableV      Enable / disable network
   ** \see        isNetworkEnabled()
   **
   ** This function enables the dispatching of CAN frames if \a btEnable is
   ** \c true, it is disabled on \c false.
   */
   void setNetworkEnabled(bool btEnableV = true);

   bool setServerAddress(QHostAddress clHostAddressV);

signals:
   /*!
   ** \param[in]  ulFrameTotalV  Total number of frames
   **
   ** This signal is emitted every second. The parameter \a ulFrameTotalV
   ** denotes the total number of API frames.
   */
   void  showApiFrames(uint32_t ulFrameTotalV);

   /*!
   ** \param[in]  ulFrameTotalV  Total number of frames
   **
   ** This signal is emitted every second. The parameter \a ulFrameTotalV
   ** denotes the total number of CAN frames.
   */
   void  showCanFrames(uint32_t ulFrameTotalV);

   /*!
   ** \param[in]  ulFrameTotalV  Total number of frames
   **
   ** This signal is emitted every second. The parameter \a ulFrameTotalV
   ** denotes the total number of CAN error frames.
   */
   void  showErrFrames(uint32_t ulFrameTotalV);

   /*!
   ** \param[in]  ubLoadV        Bus load in percent
   ** \param[in]  ulMsgPerSecV   Messages per second
   **
   **
   ** This signal is emitted every second. The parameter \a ubLoadV
   ** denotes the bus load in percent (value range 0 .. 100).
   */
   void  showLoad(uint8_t ubLoadV, uint32_t ulMsgPerSecV);

private slots:
   /*!
   ** This function is called upon socket connection.
   */
   void onSocketConnect(void);

   /*!
   ** This function is called upon socket disconnection.
   */
   void onSocketDisconnect(void);

   void onTimerEvent(void);



protected:

private:

   bool  handleApiFrame(int32_t & slSockSrcR, QCanFrameApi & clApiFrameR);
   bool  handleCanFrame(int32_t & slSockSrcR, QByteArray & clSockDataR);
   bool  handleErrFrame(int32_t & slSockSrcR, QByteArray & clSockDataR);


   //----------------------------------------------------------------
   // unique network ID
   //
   static uint8_t          ubNetIdP;

   //----------------------------------------------------------------
   // unique network name
   //
   QString                 clNetNameP;

   QPointer<QCanInterface> pclInterfaceP;
   QPointer<QTcpServer>    pclTcpSrvP;
   QVector<QTcpSocket *> * pclTcpSockListP;
   QHostAddress            clTcpHostAddrP;
   uint16_t                uwTcpPortP;
   QMutex                  clTcpSockMutexP;

   //----------------------------------------------------------------
   // Frame dispatcher time
   //
   QTimer                  clDispatchTmrP;
   uint32_t                ulDispatchTimeP;

   //----------------------------------------------------------------
   // bit-rate settings
   //
   int32_t                 slNomBitRateP;
   int32_t                 slDatBitRateP;

   //----------------------------------------------------------------
   // statistic frame counter
   //
   uint32_t                ulCntFrameApiP;
   uint32_t                ulCntFrameCanP;
   uint32_t                ulCntFrameErrP;

   //----------------------------------------------------------------
   // statistic bit counter
   //
   uint32_t                ulCntBitMaxP;
   uint32_t                ulCntBitCurP;

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
