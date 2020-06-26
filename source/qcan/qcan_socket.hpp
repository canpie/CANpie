//====================================================================================================================//
// File:          qcan_socket.hpp                                                                                     //
// Description:   CAN socket class                                                                                    //
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


#ifndef QCAN_SOCKET_HPP_
#define QCAN_SOCKET_HPP_

#include <QtCore/QMutex>
#include <QtCore/QPointer>
#include <QtCore/QQueue>
#include <QtCore/QString>
#include <QtCore/QUuid>
#include <QtCore/QVector>

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QLocalSocket>

#include <QtWebSockets/QWebSocket>

#include "qcan_defs.hpp"
#include "qcan_filter_list.hpp"
#include "qcan_frame.hpp"



//----------------------------------------------------------------------------------------------------------------
/*!
** \class QCanSocket
** \brief CAN socket
** 
** A QCanSocket is used for connection to an existing QCanNetwork. Connection can be made eiter via LocalSockets
** or WebSockets. The number of either socket type that can be connected to a QCanNetwork is limited by the 
** symbols #QCAN_LOCAL_SOCKET_MAX and #QCAN_WEB_SOCKET_MAX during compile time.
**
** Upon creation, the socket is in an unconnected state. The current socket state can be evaluated with
** isConnected() and error(). Each CAN socket has an unique identifier for socket management (uuidString()).
**
*/

class QCanSocket : public QObject
{
   Q_OBJECT

public:
   
   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  pclParentV     Pointer to QObject parent class
   **
   ** Create a new CAN socket which is in unconnected state.
   */
   QCanSocket(QObject * pclParentV = Q_NULLPTR);
	
	virtual ~QCanSocket();


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelR     CAN channel
   ** \return     \c true if connection is possible
   ** \see        disconnectNetwork()
   **
   ** Connect the CAN socket to a CAN network (class QCanNetwork). The signal connected() is used on
   ** connection to the CAN network. On failure, the reason can be evaluated by the error() signal.
   ** A socket can connect only once to a single CAN network. If the socket is already connected,
   ** the method returns \c false. 
   ** <p>
   ** The connection is made to QHostAddress::LocalHost, using the port #QCAN_WEB_SOCKET_DEFAULT_PORT. 
   ** The host address can be changed with setHostAddress().
   */
   bool                 connectNetwork(const CAN_Channel_e & teChannelR);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \see  connectNetwork()
   **
   ** Disconnect the CAN socket from a CAN network (class QCanNetwork). The signal disconnected() is
   ** used on disconnection from the CAN network.
   */
   void                 disconnectNetwork(void);

   
   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     SocketError value
   **
   ** Returns the type of error that last occurred.
   */
   QAbstractSocket::SocketError  error() const;

   
   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Error string
   **
   ** Returns a description of error that last occurred.
   */
   QString              errorString() const;
   
   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Number of CAN frames available
   ** \see        read()
   **
   ** Returns the number of CAN frames available on the socket.
   */
   int32_t              framesAvailable(void) const;

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if socket is connected
   **
   ** The function returns the connection state of the socket: \c true if connected to a CAN network,
   ** \c false if not connected.
   */
   inline bool          isConnected(void) const          { return (btIsConnectedP);       };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[out]    clFrameR   Reference to CAN frame
   ** \return        \c true if CAN frame was read
   ** \see           write()
   **
   ** The function reads a CAN frame from the socket and places the result in \a clFrameDataR. If no
   ** CAN frame is available, the function returns \c false.
   */
   bool                 read(QCanFrame & clFrameR);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clHostAddressV    Host address
   ** \param[in]  uwPortV           Port number of server
   ** \see        connectNetwork()
   **
   ** Set the host address of the CAN server. The host address can only be modified in unconnected
   ** state.
   */
   void                 setHostAddress(const QHostAddress clHostAddressV, 
                                       const uint16_t uwPortV = QCAN_WEB_SOCKET_DEFAULT_PORT);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  CAN error state
   **
   ** Get the actual CAN error state, which is defined by the QCan::CAN_State_e enumeration.
   */
   inline CAN_State_e   state(void) const                { return teCanStateP; };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     UUID
   **
   ** Get the Universal unique identifier of the CAN socket.
   */
   inline QUuid         uuid(void) const                 { return (clUuidP);              };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     UUID string
   **
   ** Get the Universal unique identifier of the CAN socket.
   */
   QString              uuidString(void) const           { return (clUuidP.toString());   };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clFrameR       Reference to CAN frame
   ** \return     \c true if CAN frame was written
   ** \see  read()
   **
   ** The function writes the CAN frame \a clFrameR to the CAN socket. If writing fails, the function
   ** returns \c false.
   */
   bool                 write(const QCanFrame & clFrameR);



signals:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** This signal is emitted after connectNetwork() has been called and a connection has been
   ** successfully established.
   */
   void                 connected(void);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** This signal is emitted when the socket has been disconnected.
   */
   void                 disconnected(void);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teSockErrorV   Type of error
   **
   ** This signal is emitted after an error occurred. The \a teSockErrorV parameter describes the type
   ** of error that occurred.
   */
   void                 error(QAbstractSocket::SocketError teSockErrorV);


   //---------------------------------------------------------------------------------------------------
   /*!
   **
   ** This signal is emitted when CAN frames are available for reading from the socket. 
   */
   void                 readyRead(void);

protected:

private:

   QPointer<QLocalSocket>  pclLocalSocketP;
   QPointer<QWebSocket>    pclWebSocketP;
   QHostAddress            clServerHostAddrP;
   uint16_t                uwServerPortP;
   bool                    btIsConnectedP;
   bool                    btIsLocalConnectionP;
   int32_t                 slSocketErrorP;
   CAN_State_e             teCanStateP;
   QUuid                   clUuidP;

   QByteArray              clReceiveDataP;
   QCanFrame               clReceiveFrameP;
   QMutex                  clReceiveMutexP;
   QQueue<QCanFrame>       clReceiveFifoP;
   

private slots:
   void                 onSocketConnect(void);
   void                 onSocketDisconnect(void);
   void                 onSocketErrorLocal(QLocalSocket::LocalSocketError teSocketErrorV);
   void                 onSocketErrorWeb(QAbstractSocket::SocketError teSocketErrorV);
   void                 onSocketReceiveLocal(void);
   void                 onSocketReceiveWeb(const QByteArray &clMessageR);
};

#endif   // QCAN_SOCKET_HPP_
