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
#include <QtCore/QElapsedTimer>
#include <QtCore/QMutex>
#include <QtCore/QPointer>
#include <QtCore/QTimer>

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

#include <QtWebSockets/QWebSocket>

#include "qcan_frame.hpp"
#include "qcan_interface.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Referenced classes                                                                                                 **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/




//----------------------------------------------------------------------------------------------------------------
/*!
** \class QCanNetwork
** \brief CAN network representation
**
** This class represents a CAN network with a unique bit-rate. It supports one physical CAN interface (see class
** QCanInterface), which can be attached during run-time to the CAN network and a limited number of virtual CAN
** interfaces (sockets).
** <p>
** <h2>Sockets</h2>
** Clients can connect to a QCanNetwork via the QCanSocket class, either via a local socket or a WebSocket. The
** maximum number of available sockets is defined by #QCAN_LOCAL_SOCKET_MAX and #QCAN_WEB_SOCKET_MAX.
** It is only possible to connect to a network when it is enabled (see setNetworkEnabled() and isNetworkEnabled()).
**
**
** <p>
** <h2>CAN Interface</h2>
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
   **
   ** Create new CAN network with unique channel number.
   */
   QCanNetwork(QObject * pclParentV = nullptr);


	~QCanNetwork() override;

   QCanNetwork(const QCanNetwork&) = delete;                   // no copy constructor
   QCanNetwork& operator=(const QCanNetwork&) = delete;        // no assignment operator
   QCanNetwork(QCanNetwork&&) = delete;                        // no move constructor
   QCanNetwork& operator=(QCanNetwork&&) = delete;             // no move operator

   enum SocketType_e {
      eSOCKET_TYPE_CAN_FRAME = 1,
      eSOCKET_TYPE_SETTINGS  = 2
   };

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
   ** \param[in]  pclSocketV     Pointer to WebSocket 
   ** \param[in]  teSocketTypeV  WebSocket type
   **
   ** This function attaches a WebSocket to the CAN network. The WebSocket can either be used to
   ** exchange CAN frames or to retrieve and alter the CAN network settings. The WebSocket type is
   ** defined by the parameter \a teSocketTypeV.
   */
   void  attachWebSocket(QWebSocket * pclSocketV, const enum SocketType_e teSocketTypeV = eSOCKET_TYPE_CAN_FRAME);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Bit rate value for nominal bit-timing
   ** \see        setBitrate()
   **
   ** This function returns the nominal bit rate of the CAN network. For <b>Classic CAN</b>, the
   ** return value defines the bit rate for the complete frame. For <b>CAN FD</b> the return value
   ** defines the bit rate for the arbitration phase.
   ** <p>
   ** If no bit rate is configured, the function will return QCan::eCAN_BITRATE_NONE.
   */
	inline int32_t  nominalBitrate(void) const      {  return (slNomBitRateP);    }


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Bit rate value for nominal bit-timing
   **
   ** This function returns the nominal bit rate of the CAN network as a QString object.
   ** <p>
   ** If no bit-rate is configured, the function will return "None".
   */
	QString nominalBitrateString(void) const;

   inline QCan::CAN_Channel_e channel() const      { return (static_cast< QCan::CAN_Channel_e >(ubIdP)); }

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Bit rate value for data bit-timing
   ** \see        setBitrate()
   **
   ** This function returns the data bit rate of the CAN network. For <b>Classic CAN</b>, the return
   ** value is always CANpie::eCAN_BITRATE_NONE. For <b>CAN FD</b> the return value defines the bit rate
   ** for the data phase.
   ** <p>
   ** If no bit rate is configured, the function will return QCan::eCAN_BITRATE_NONE.
   */
	inline int32_t  dataBitrate(void) const         {  return (slDatBitRateP);    }


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Bit rate value for data bit-timing
   **
   ** This function returns the data bit rate of the CAN network as a QString.
   ** <p>
   ** If no bit rate is configured, the function will return
   ** "None".
   */
	QString dataBitrateString(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Number of CAN frames
   **
   ** This function returns the actual number of CAN frames that have been transmitted via the
   ** network.
   */
	uint32_t frameCount(void) const                 { return (ulCntFrameCanP);          }


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Number of Error frames
   **
   ** This function returns the actual number of error frames that have been transmitted via the
   ** network.
   */
	uint32_t frameCountError(void) const            { return (ulCntFrameErrP);          }



   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if error frames are supported
   ** \see        isErrorFrameEnabled()
   **
   ** This function returns \c true if the attached CAN interface supports the evaluation of error
   ** frames, otherwise it returns \c false.
   */
   bool hasErrorFrameSupport(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if CAN FD is supported
   ** \see        isFlexibleDataEnabled()
   **
   ** This function returns \c true if the attached CAN interface supports CAN FD frames, otherwise it
   ** returns \c false.
   */
   bool hasFlexibleDataSupport(void) const;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if listen-only is supported
   ** \see        isListenOnlyEnabled()
   **
   ** This function returns \c true if the attached CAN interface supports listen-only mode, otherwise
   ** it returns \c false.
   */
   bool hasListenOnlySupport(void) const;


   bool hasSpecificConfigurationSupport(void) const;

   inline uint8_t    id(void) const                { return (ubIdP);                   }


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if handling of error frames is enabled
   ** \see        setErrorFrameEnabled()
   **
   ** This function returns \c true if handling of error frames is enabled, otherwise it returns
   ** \c false.
   */
   bool isErrorFrameEnabled(void) const            { return (btErrorFrameEnabledP);    }


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if CAN FD is enabled
   ** \see        setFlexibleDataEnabled()
   **
   ** This function returns \c true if CAN FD mode is enabled, otherwise it returns
   ** \c false.
   */
   bool isFlexibleDataEnabled(void) const          { return (btFlexibleDataEnabledP);  }


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if listen-only is enabled
   ** \see        setListenOnlyEnabled()
   **
   ** This function returns \c true if listen-only mode is enabled, otherwise it returns
   ** \c false.
   */
   bool isListenOnlyEnabled(void) const            { return (btListenOnlyEnabledP);    }


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if network is enabled
   ** \see        setNetworkEnabled()
   **
   ** This function returns \c true if the network is enabled, otherwise it returns \c false.
   */
   bool isNetworkEnabled(void) const               { return (btNetworkEnabledP);       }


	QString  name() const                           { return(clNetNameP);               }

	void reset(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \see     addInterface()
   **
   ** Remove a physical CAN interface from the CAN network.
   */
	void removeInterface(void);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  slNomBitRateV  Nominal bit rate value
   ** \param[in]  slDatBitRateV  Data bit rate value
   ** \see        dataBitrate(), nominalBitrate()
   **
   ** This function sets the bit rate for the CAN network. For <b>Classic CAN</b>, the parameter
   ** \c slNomBitRateV defines the bit rate for the complete frame; the parameter \c slDatBitRateV is
   ** not evaluated in that case. For <b>CAN FD</b> the parameter \c slNomBitRateV defines the
   ** bit rate for the arbitration phase; the parameter \c slDatBitRateV defines the bit rate for the
   ** data phase.
   ** <p>
   ** For selection of predefined bit rates the value can be taken from the enumeration
   ** CANpie::CAN_Bitrate_e.
   */
	void setBitrate(const int32_t slNomBitRateV, const int32_t slDatBitRateV = QCan::eCAN_BITRATE_NONE);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  btEnableV      Enable / disable error frames
   ** \see        isErrorFrameEnabled()
   **
   ** This function enables the dispatching of CAN error frames if \a btEnable is \c true, it is
   ** disabled on \c false.
   */
   void setErrorFrameEnabled(const bool btEnableV = true);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  btEnableV      Enable / disable CAN FD mode
   ** \see        isFlexibleDataEnabled()
   **
   ** This function enables the CAN FD mode if \a btEnable is \c true, it is disabled on \c false.
   */
   void setFlexibleDataEnabled(const bool btEnableV = true);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** Perform a device specific interface configuration. This function is only executed if a CAN
   ** interface has been added to the network (refer to addInterface()).
   */
   void setInterfaceConfiguration(void);


   void setListenOnlyEnabled(const bool btEnableV = true);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  btEnableV      Enable / disable network
   ** \see        isNetworkEnabled()
   **
   ** This function enables the dispatching of CAN frames if \a btEnable is \c true, it is disabled
   ** on \c false.
   */
   void setNetworkEnabled(const bool btEnableV = true);


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
   inline QCan::CAN_State_e state(void) const      { return (teCanStateP);   }

signals:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubChannelR  CAN channel
   ** \param[in]  clMessageR  Logging message
   ** \param[in]  teLogLevelR Logging level
   **
   ** This signal is emitted by the CAN network to inform the application about status changes or
   ** error conditions.
   */
   void  addLogMessage(const QCan::CAN_Channel_e & ubChannelR,
                       const QString & clMessageR, const QCan::LogLevel_e & teLogLevelR = QCan::eLOG_LEVEL_WARN);

   void  showBitrate(const QCan::CAN_Channel_e & ubChannelR, const int32_t & slNomBitRateR,
                     const int32_t & slDatBitRateR);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubChannelR     CAN channel
   ** \param[in]  ulFrameTotalR  Total number of frames
   **
   ** This signal is emitted every second. The parameter \a ulFrameTotalV denotes the total number of
   ** CAN frames.
   */
   void  showCanFrames(const QCan::CAN_Channel_e & ubChannelR, const uint32_t & ulFrameTotalR);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubChannelR     CAN channel
   ** \param[in]  ulFrameTotalR  Total number of frames
   **
   ** This signal is emitted every second. The parameter \a ulFrameTotalV denotes the total number of
   ** CAN error frames.
   */
   void  showErrFrames(const QCan::CAN_Channel_e & ubChannelR, const uint32_t & ulFrameTotalR);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubChannelR           CAN channel
   ** \param[in]  teConnectionStateR   Connection state of CAN interface
   **
   ** This signal is once upon a connection state change of a physical CAN interface.
   */
   void  showInterfaceState(const QCan::CAN_Channel_e & ubChannelR,
                            const QCanInterface::ConnectionState_e & teConnectionStateR);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  ubChannelR     CAN channel
   ** \param[in]  ubLoadR        Bus load in percent
   ** \param[in]  ulMsgPerSecR   Messages per second
   **
   **
   ** This signal is emitted every second. The parameter \a ubLoadV
   ** denotes the bus load in percent (value range 0 .. 100).
   */
   void  showLoad(const QCan::CAN_Channel_e & ubChannelR, const uint8_t & ubLoadR,
                  const uint32_t & ulMsgPerSecR);


   //---------------------------------------------------------------------------------------------------
   void  showSocketState(const QCan::CAN_Channel_e & ubChannelR,
                         const uint32_t & ulLocalSocketsR, const uint32_t & ulTcpSocketsR);

   void  showState(const QCan::CAN_Channel_e & ubChannelR, const QCan::CAN_State_e & teStateR);


private slots:

   void  onInterfaceConnectionChanged(const QCanInterface::ConnectionState_e & teConnectionStateR);

   void  onInterfaceLogMessage(QString clMessageV, QCan::LogLevel_e teLogLevelV);

   void  onInterfaceNewData(void);

   void  onInterfaceStateChange(QCan::CAN_State_e teStateV);

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
   ** This slot is called when new data is available on any open web socket.
   */
   void onWebSocketBinaryData(const QByteArray &clMessageR);

   /*!
   ** This slot is called upon WebSocket disconnection.
   */
   void onWebSocketDisconnect(void);

   /*!
   ** This slot is called when new data is available on any open web socket.
   */
   void onWebSocketTextData(const QString &clMessageR);

   void onTimerEvent(void);



private:

   //---------------------------------------------------------------------------------------------------
   // This enumeration defines the source of the frame
   //
   enum FrameSource_e {
      eFRAME_SOURCE_CAN_IF = 1,
      eFRAME_SOURCE_LOCAL_SOCKET,
      eFRAME_SOURCE_WEB_SOCKET
   };

   //---------------------------------------------------------------------------------------------------
   // returns number of bits inside a data frame for static calculations
   //
   uint32_t frameSize(const QByteArray & clSockDataR);

   bool     handleCanFrame(enum FrameSource_e teFrameSrcV, const int32_t slSockSrcV, QByteArray clSockDataV);

   void     logSocketState(const QString & clInfoR);
   
   void     sendNetworkSettings(uint32_t flags = 0);

   void     setCanState(QCan::CAN_State_e teStateV);

   //---------------------------------------------------------------------------------------------------
   // unique network ID, ubNetIdP is used to manage a unique id
   // for all networks, ubIdP holds the id of the current instance
   //
   static uint8_t          ubNetIdP;
   uint8_t                 ubIdP;

   //---------------------------------------------------------------------------------------------------
   // unique network name
   //
   QString                 clNetNameP;

   //---------------------------------------------------------------------------------------------------
   // pointer to QCanInterface class, see method addInterface()
   //
   QPointer<QCanInterface> pclInterfaceP;

   //---------------------------------------------------------------------------------------------------
   // Management of local sockets:  a QLocalServer (pclLocalSrvP) is used to handle a fixed number of 
   // QLocalSockets (pclLocalSockListP)
   //
   QPointer<QLocalServer>  pclLocalSrvP;
   QVector<QLocalSocket*>  clLocalSockListP;
   QMutex                  clLocalSockMutexP;

   //---------------------------------------------------------------------------------------------------
   // Management of WebSockets for CAN frames: a fixed number of QWebSockets (pclWebSockListP) is kept 
   // in a list
   //
   QVector<QWebSocket *>   clWebSockListP;
   QMutex                  clWebSockMutexP;

   //---------------------------------------------------------------------------------------------------
   // Management of WebSockets for network settings 
   //
   QVector<QWebSocket *>   clSettingsListP;

   QTimer                  clRefreshTimerP;

   //---------------------------------------------------------------------------------------------------
   // bit-rate settings: the variables hold the bit-rate in bit/s, if no bit-rate is configured the 
   // value is eCAN_BITRATE_NONE
   //
   int32_t                 slNomBitRateP;
   int32_t                 slDatBitRateP;

   //---------------------------------------------------------------------------------------------------
   // status of CAN bus
   //
   QCan::CAN_State_e       teCanStateP;

   QCanFrame               clCanFrameOutP;

   //---------------------------------------------------------------------------------------------------
   // statistic frame counter
   //
   uint32_t                ulCntFrameCanP;
   uint32_t                ulCntFrameErrP;

   //---------------------------------------------------------------------------------------------------
   // statistic bit counter
   //
   uint32_t                ulCntBitCurP;

   //---------------------------------------------------------------------------------------------------
   // statistic timing
   //
   QElapsedTimer           clStatisticTimeP;

   uint32_t                ulStatisticTimeP;
   uint32_t                ulFramePerSecMaxP;
   uint32_t                ulFrameCntSaveP;
   uint8_t                 ubBusLoadP;

   bool                    btErrorFrameEnabledP;
   bool                    btFlexibleDataEnabledP;
   bool                    btListenOnlyEnabledP;
   bool                    btNetworkEnabledP;
   bool                    btBitrateChangeEnabledP;
   bool                    btTimeStampEnabledP;

};

#endif   // QCAN_NETWORK_HPP_
