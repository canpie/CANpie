//====================================================================================================================//
// File:          qcan_network.hpp                                                                                    //
// Description:   QCAN classes - CAN network                                                                          //
//                                                                                                                    //
// Copyright (C) MicroControl GmbH & Co. KG                                                                           //
// 53844 Troisdorf - Germany                                                                                          //
// www.microcontrol.net                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the   //
// following conditions are met:                                                                                      //
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions, the following   //
//    disclaimer and the referenced file 'LICENSE'.                                                                   //
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       //
//    following disclaimer in the documentation and/or other materials provided with the distribution.                //
// 3. Neither the name of MicroControl nor the names of its contributors may be used to endorse or promote products   //
//    derived from this software without specific prior written permission.                                           //
//                                                                                                                    //
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance     //
// with the License. You may obtain a copy of the License at                                                          //
//                                                                                                                    //
//    http://www.apache.org/licenses/LICENSE-2.0                                                                      //
//                                                                                                                    //
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed   //
// on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for  //
// the specific language governing permissions and limitations under the License.                                     //
//                                                                                                                    //
//====================================================================================================================//


#ifndef QCAN_NETWORK_HPP_
#define QCAN_NETWORK_HPP_


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QDateTime>
#include <QtCore/QMutex>
#include <QtCore/QPointer>
#include <QtCore/QTimer>

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include "qcan_frame.hpp"
#include "qcan_frame_api.hpp"
#include "qcan_frame_error.hpp"

using namespace QCan;

/*--------------------------------------------------------------------------------------------------------------------*\
** Referenced classes                                                                                                 **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/
class QCanInterface;




//----------------------------------------------------------------------------------------------------------------
/*!
** \class QCanNetwork
** \brief CAN network representation
** 
** This class represents a CAN network with a unique bit-rate. It supports one physical CAN interface (see class
** QCanInterface), which can be attached during run-time to the CAN network and a limited number of virtual CAN
** interfaces (sockets). Clients can connect to a QCanNetwork via the QCanSocket class.
** <p>
** <h3>Sockets</h3>
** It is only possible to connect to a network when it is enabled (see setNetworkEnabled() and isNetworkEnabled()).
**
** <p>
** <h3>CAN Interface</h3>
** A physical CAN interface is attached to the network using the addInterface() function. It is started by
** calling startInterface(), which configures the actual bit-rate settings.
** <p>
** A physical CAN interface is stopped by calling stopInterface(), it can be removed from the network by
** calling removeInterface().
**
** <p>
** <hr>
*/
class QCanNetwork : public QObject
{
   Q_OBJECT
public:
   
   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  pclParentV     Pointer to QObject parent class
   ** \param[in]  uwPortV        Port number
   **
   ** Create new CAN network with unique channel number.
   */
   QCanNetwork(QObject * pclParentV = Q_NULLPTR, uint16_t  uwPortV = QCAN_TCP_DEFAULT_PORT);
	
	
	~QCanNetwork();

   //---------------------------------------------------------------------------------------------------
	/*!
   ** \param[in]  pclCanIfV     Pointer to CAN interface class
   ** \return     \c true if CAN interface added successfully
	** \see        removeInterface()
	**
	** The function adds a physical CAN interface to the CAN network. Each CAN network supports only one
	** physical CAN interface. The CAN interface is removed from the network by calling the
	** removeInterface() method. The parameter \c pclCanIfV is a pointer to an instance of a QCanInterface
	** class.
	** <p>
	** The function returns \c true if the CAN interface is added, otherwise it will return \c false.
	*/
	bool addInterface(QCanInterface * pclCanIfV);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Bit-rate value for Nominal Bit Timing
   ** \see        nominalBitrate()
   ** \deprecated It is advised to use nominalBitrate() for new applications.
   **
   */
	inline int32_t  bitrate(void)          {  return (slNomBitRateP);    };

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Bit-rate value for Nominal Bit Timing
   ** \see        setBitrate()
   **
   ** This function returns the nominal bit-rate of the CAN network. For <b>classical CAN</b>, the
   ** return value defines the bit-rate for the complete frame. For <b>CAN FD</b> the return value
   ** defines the bit-rate for the arbitration phase.
   ** <p>
   ** If no bit-rate is configured, the function will return CANpie::eCAN_BITRATE_NONE.
   */
	inline int32_t  nominalBitrate(void)   {  return (slNomBitRateP);    };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Bit-rate value for Nominal Bit Timing
   **
   ** This function returns the nominal bit-rate of the CAN network as QString object.
   ** <p>
   ** If no bit-rate is configured, the function will return "None".
   */
	QString nominalBitrateString(void);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Bit-rate value for Data Bit Timing
   ** \see        setBitrate()
   **
   ** This function returns the data bit-rate of the CAN network. For <b>classical CAN</b>, the return
   ** value is always CANpie::eCAN_BITRATE_NONE. For <b>CAN FD</b> the return value defines the bit-rate
   ** for the data phase.
   ** <p>
   ** If no bit-rate is configured, the function will return CANpie::eCAN_BITRATE_NONE.
   */
	inline int32_t  dataBitrate(void)      {  return (slDatBitRateP);    };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Bit-rate value for Data Bit Timing
   **
   ** This function returns the data bit-rate of the CAN network as
   ** QString.
   ** <p>
   ** If no bit-rate is configured, the function will return
   ** "None".
   */
	QString dataBitrateString(void);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Number of CAN frames
   **
   ** This function returns the actual number of CAN frames that have been transmitted via the
   ** network.
   */
	uint32_t frameCount(void)        { return (ulCntFrameCanP);          };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Number of Error frames
   **
   ** This function returns the actual number of error frames that have been transmitted via the
   ** network.
   */
	uint32_t frameCountError(void)   { return (ulCntFrameErrP);          };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if error frames are supported
   ** \see        isErrorFrameEnabled()
   **
   ** This function returns \c true if the attached CAN interface supports the evaluation of error
   ** frames, otherwise it returns \c false.
   */
   bool hasErrorFrameSupport(void);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if CAN FD is supported
   ** \see        isFastDataEnabled()
   **
   ** This function returns \c true if the attached CAN interface supports CAN FD frames, otherwise it
   ** returns \c false.
   */
   bool hasFastDataSupport(void);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if listen-only is supported
   ** \see        isListenOnlyEnabled()
   **
   ** This function returns \c true if the attached CAN interface supports listen-only mode, otherwise
   ** it returns \c false.
   */
   bool hasListenOnlySupport(void);


   bool hasSpecificConfigurationSupport(void);

   inline uint8_t    id(void)       { return (ubIdP);                   };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if handling of error frames is enabled
   ** \see        setErrorFrameEnabled()
   **
   ** This function returns \c true if handling of error frames is enabled, otherwise it returns
   ** \c false.
   */
   bool isErrorFrameEnabled(void)   { return (btErrorFrameEnabledP);    };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if CAN FD is enabled
   ** \see        setFastDataEnabled()
   **
   ** This function returns \c true if CAN FD mode is enabled, otherwise it returns
   ** \c false.
   */
   bool isFastDataEnabled(void)     { return (btFastDataEnabledP);      };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if listen-only is enabled
   ** \see        setListenOnlyEnabled()
   **
   ** This function returns \c true if listen-only mode is enabled, otherwise it returns
   ** \c false.
   */
   bool isListenOnlyEnabled(void)   { return (btListenOnlyEnabledP);    };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if network is enabled
   ** \see        setNetworkEnabled()
   **
   ** This function returns \c true if the network is enabled, otherwise it returns \c false.
   */
   bool isNetworkEnabled(void)      { return (btNetworkEnabledP);       };


	QString  name()                  { return(clNetNameP);               };

	void reset(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \see     addInterface()
   **
   ** Remove a physical CAN interface from the CAN network.
   */
	void removeInterface(void);

   QHostAddress serverAddress(void);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  slNomBitRateV  Nominal Bit-rate value
   ** \param[in]  slDatBitRateV  Data Bit-rate value
   ** \see        dataBitrate(), nominalBitrate()
   ** 
   ** This function sets the bit-rate for the CAN network. For <b>Classical CAN</b>, the parameter
   ** \c slNomBitRateV defines the bit-rate for the complete frame, the parameter \c slDatBitRateV is
   ** not evaluated in that case. For <b>CAN FD</b> the parameter \c slNomBitRateV defines the
   ** bit-rate for the arbitration phase, the parameter \c slDatBitRateV defines the bit-rate for the
   ** data phase.
   ** <p>
   ** For selection of predefined bit-rates the value can be taken from the enumeration
   ** CANpie::CAN_Bitrate_e.
   */
	void setBitrate(int32_t slNomBitRateV,
	                int32_t slDatBitRateV = eCAN_BITRATE_NONE);


	void setBitrateFrameEnabled(bool btEnableV = true)    { btBitrateFrameEnabledP = btEnableV; };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  btEnableV      Enable / disable error frames
   ** \see        isErrorFrameEnabled()
   **
   ** This function enables the dispatching of CAN error frames if \a btEnable is \c true, it is
   ** disabled on \c false.
   */
   void setErrorFrameEnabled(bool btEnableV = true);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  btEnableV      Enable / disable CAN FD mode
   ** \see        isFastDataEnabled()
   **
   ** This function enables the CAN FD mode if \a btEnable is \c true, it is disabled on \c false.
   */
   void setFastDataEnabled(bool btEnableV = true);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** Perform a device specific interface configuration. This function is only executed if a CAN
   ** interface has been added to the network (refer to addInterface()).
   */
   void setInterfaceConfiguration(void);


   void setListenOnlyEnabled(bool btEnableV = true);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  btEnableV      Enable / disable network
   ** \see        isNetworkEnabled()
   **
   ** This function enables the dispatching of CAN frames if \a btEnable is \c true, it is disabled
   ** on \c false.
   */
   void setNetworkEnabled(bool btEnableV = true);

   bool setServerAddress(QHostAddress clHostAddressV);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if CAN interface is started
   ** \see        addInterface()
   **
   ** The function starts a physical CAN interface. The interface must have been attached to the
   ** network using addInterface() in advance.
   ** <p>
   ** The function returns \c true if the CAN interface is started, otherwise it will return \c false.
   */
   bool startInterface(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if CAN interface is stopped
   ** \see        addInterface()
   **
   ** The function stops a physical CAN interface. The interface must have been attached to the
   ** network using addInterface() in advance.
   ** <p>
   ** The function returns \c true if the CAN interface is stopped, otherwise it will return \c false.
   */
   bool stopInterface(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     State of CAN network
   **
   ** The function returns the CAN state of the network.
   */
   inline CAN_State_e state(void) { return (teCanStateP);   };

signals:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubChannelR - CAN channel
   ** \param[in]  clMessageV - Logging message
   ** \param[in]  teLogLevel - Logging level
   **
   ** This signal is emitted by the CAN network to inform the application about status changes or
   ** error conditions.
   */
   void  addLogMessage(const CAN_Channel_e & ubChannelR,
                       const QString & clMessageR, const LogLevel_e & teLogLevelR = eLOG_LEVEL_NOTICE);

   void  changeBitrate(const CAN_Channel_e & ubChannelR, const uint32_t & slNomBitRateR,
                       const int32_t & slDatBitRateR);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubChannelR - CAN channel
   ** \param[in]  ulFrameTotalV  Total number of frames
   **
   ** This signal is emitted every second. The parameter \a ulFrameTotalV
   ** denotes the total number of API frames.
   */
   void  showApiFrames(const CAN_Channel_e & ubChannelR, const uint32_t & ulFrameTotalR);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubChannelR - CAN channel
   ** \param[in]  ulFrameTotalV  Total number of frames
   **
   ** This signal is emitted every second. The parameter \a ulFrameTotalV
   ** denotes the total number of CAN frames.
   */
   void  showCanFrames(const CAN_Channel_e & ubChannelR, const uint32_t & ulFrameTotalR);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubChannelR    - CAN channel
   ** \param[in]  ulFrameTotalV - Total number of frames
   **
   ** This signal is emitted every second. The parameter \a ulFrameTotalV
   ** denotes the total number of CAN error frames.
   */
   void  showErrFrames(const CAN_Channel_e & ubChannelR, const uint32_t & ulFrameTotalR);

   /*!
   ** \param[in]  ubChannelR   - CAN channel
   ** \param[in]  ubLoadR      - Bus load in percent
   ** \param[in]  ulMsgPerSecR - Messages per second
   **
   **
   ** This signal is emitted every second. The parameter \a ubLoadV
   ** denotes the bus load in percent (value range 0 .. 100).
   */
   void  showLoad(const CAN_Channel_e & ubChannelR, const uint8_t & ubLoadR, const uint32_t & ulMsgPerSecR);

   void  showState(const CAN_Channel_e & ubChannelR, const CAN_State_e & teStateR);


private slots:

   void onInterfaceLogMessage(QString clMessageV, LogLevel_e teLogLevelV);

   void onInterfaceNewData(void);

   void onInterfaceStateChange(CAN_State_e teStateV);

   /*!
   ** This slot is called upon local socket connection.
   */
   void onLocalSocketConnect(void);

   /*!
   ** This slot is called upon local socket disconnection.
   */
   void onLocalSocketDisconnect(void);

   /*!
   ** This slot is called when new data is available on any open local socket.
   */
   void onLocalSocketNewData(void);

   /*!
   ** This slot is called upon TCP socket connection.
   */
   void onTcpSocketConnect(void);

   /*!
   ** This slot is called upon TCP socket disconnection.
   */
   void onTcpSocketDisconnect(void);

   /*!
   ** This slot is called when new data is available on any open TCP socket.
   */
   void onTcpSocketNewData(void);


   void onTimerEvent(void);



protected:

private:

   QCanData::Type_e  frameType(const QByteArray & clSockDataR);
   
   //----------------------------------------------------------------
   // returns number of bits inside a data frame for static
   // calculations
   //
   uint32_t          frameSize(const QByteArray & clSockDataR);

   //----------------------------------------------------------------
   // This enumeration defines the source of the frame
   //
   enum FrameSource_e {
      eFRAME_SOURCE_CAN_IF = 1,
      eFRAME_SOURCE_SOCKET_LOCAL,
      eFRAME_SOURCE_SOCKET_TCP
   };

   bool  handleApiFrame(enum FrameSource_e teFrameSrcV,
                        int32_t & slSockSrcR, QByteArray & clSockDataR);

   bool  handleCanFrame(enum FrameSource_e teFrameSrcV,
                        int32_t & slSockSrcR, QByteArray & clSockDataR);

   bool  handleErrFrame(enum FrameSource_e teFrameSrcV,
                        int32_t & slSockSrcR, QByteArray & clSockDataR);


   //----------------------------------------------------------------
   // unique network ID, ubNetIdP is used to manage a unique id
   // for all networks, ubIdP holds the id of the current instance
   //
   static uint8_t          ubNetIdP;
   uint8_t                 ubIdP;

   //----------------------------------------------------------------
   // unique network name
   //
   QString                 clNetNameP;

   //----------------------------------------------------------------
   // pointer to QCanInterface class, see method addInterface()
   //
   QPointer<QCanInterface> pclInterfaceP;

   //----------------------------------------------------------------
   // Management of local sockets:  a QLocalServer (pclLocalSrvP) is
   // used to handle a fixed number of QTcpSockets (pclTcpSockListP)
   //
   QPointer<QLocalServer>  pclLocalSrvP;
   QVector<QLocalSocket*>* pclLocalSockListP;

   QMutex                  clLocalSockMutexP;

   //----------------------------------------------------------------
   // Management of TCP sockets:  a QTcpServer (pclTcpServer) is used
   // to handle a fixed number of QTcpSockets (pclTcpSockListP)
   //
   QPointer<QTcpServer>    pclTcpSrvP;
   QVector<QTcpSocket *> * pclTcpSockListP;
   QHostAddress            clTcpHostAddrP;
   uint16_t                uwTcpPortP;

   QMutex                  clTcpSockMutexP;

   QTimer                  clRefreshTimerP;

   //----------------------------------------------------------------
   // bit-rate settings: the variables hold the bit-rate in
   // bit/s, if no bit-rate is configured the value is
   // eCAN_BITRATE_NONE 
   //
   int32_t                 slNomBitRateP;
   int32_t                 slDatBitRateP;

   //----------------------------------------------------------------
   // status of CAN bus
   //
   CAN_State_e             teCanStateP;
   
   //----------------------------------------------------------------
   // statistic frame counter
   //
   uint32_t                ulCntFrameApiP;
   uint32_t                ulCntFrameCanP;
   uint32_t                ulCntFrameErrP;

   //----------------------------------------------------------------
   // statistic bit counter
   //
   uint32_t                ulCntBitCurP;

   //----------------------------------------------------------------
   // statistic timing
   //
   QTime                   clStatisticTimeP;
   
   uint32_t                ulStatisticTimeP;
   uint32_t                ulFramePerSecMaxP;
   uint32_t                ulFrameCntSaveP;
   uint8_t                 ubBusLoadP;

   bool                    btErrorFrameEnabledP;
   bool                    btFastDataEnabledP;
   bool                    btListenOnlyEnabledP;
   bool                    btNetworkEnabledP;
   bool                    btBitrateFrameEnabledP;
};

#endif   // QCAN_NETWORK_HPP_
