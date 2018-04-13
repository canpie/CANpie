//============================================================================//
// File:          qcan_socket.hpp                                             //
// Description:   CAN socket class                                            //
//                                                                            //
// Copyright 2017 MicroControl GmbH & Co. KG                                  //
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
// Licensed under the Apache License, Version 2.0 (the "License");            //
// you may not use this file except in compliance with the License.           //
// You may obtain a copy of the License at                                    //
//                                                                            //
//    http://www.apache.org/licenses/LICENSE-2.0                              //
//                                                                            //
// Unless required by applicable law or agreed to in writing, software        //
// distributed under the License is distributed on an "AS IS" BASIS,          //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   //
// See the License for the specific language governing permissions and        //
// limitations under the License.                                             //
//============================================================================//


#ifndef QCAN_SOCKET_HPP_
#define QCAN_SOCKET_HPP_


#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QPointer>

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QTcpSocket>

#include "qcan_defs.hpp"
#include "qcan_frame.hpp"
#include "qcan_frame_api.hpp"
#include "qcan_frame_error.hpp"



//-----------------------------------------------------------------------------
/*!
** \class QCanSocket
** \brief CAN socket
** 
** A QCanSocket is used for connection to an existing QCanNetwork. The number
** of sockets that can be connected to a QCanNetwork is limited by the symbol
** #QCAN_TCP_SOCKET_MAX.
**
** Upon creation, the socket is in an unconnected state. The current socket
** state can be evaluated with isConnected() and error(). Each CAN socket
** has an unique identifier for socket management (uuidString()).
**
*/

class QCanSocket : public QObject
{
   Q_OBJECT

public:
   
   /*!
   ** \param[in]  pclParentV     Pointer to QObject parent class
   **
   ** Create a new CAN socket which is in unconnected state.
   */
   QCanSocket(QObject * pclParentV = Q_NULLPTR);
	
	virtual ~QCanSocket();

   /*!
   ** \param[in]  teChannelV     CAN channel
   ** \param[in]  slMilliSecsV   Time to wait for connection
   ** \return     \c true if connection is possible
   ** \see        disconnectNetwork()
   **
   ** Connect the CAN socket to a CAN network (class QCanNetwork). The signal
   ** connected() is used on connection to the CAN network. On failure, the
   ** reason can be evaluated by the error() signal. A socket can connect
   ** only once to a single CAN network. If the socket is already connected,
   ** the method returns \c false. The optional parameter \a slMilliSecsV
   ** defines how many milli-seconds the method shall wait until a
   ** connection has been established.
   ** <p>
   ** The connection is made to QHostAddress::LocalHost, using the port
   ** #QCAN_TCP_DEFAULT_PORT. The host address can be changed with
   ** setHostAddress().
   */
   bool connectNetwork(CAN_Channel_e teChannelV, const int32_t slMilliSecsV = 0);


   /*!
   ** \see  connectNetwork()
   **
   ** Disconnect the CAN socket from a CAN network (class QCanNetwork). The
   ** signal disconnected() is used on disconnection from the CAN network.
   */
   void disconnectNetwork(void);

   
   /*!
   ** \return     SocketError value
   **
   ** Returns the type of error that last occurred.
   */
   QAbstractSocket::SocketError  error() const;

   
   /*!
   ** \return     Error string
   **
   ** Returns a description of error that last occurred.
   */
   QString  errorString() const;
   
   
   /*!
   ** \return     Number of CAN frames available
   **
   ** Returns the number of CAN frames available on the socket.
   */
   int32_t  framesAvailable(void) const;


   /*!
   ** \return     \c true if socket is connected
   **
   ** The function returns the connection state of the socket: \c true if
   ** connected to a CAN network, \c false if not connected.
   */
   bool isConnected(void);


   /*!
   ** \return     UUID string
   **
   ** Get the Universal unique identifier of the CAN socket.
   */
   QString  uuidString(void);


   /*!
   ** \param[in]  clHostAddressV    Host address
   ** \see        connectNetwork()
   **
   ** Set the host address of the CAN network. The host address can only
   ** be modified in unconnected state.
   */
   void  setHostAddress(QHostAddress clHostAddressV);


   /*!
   ** \return  CAN error state
   **
   ** Get the actual CAN error state, which is defined by the 
   ** QCan::CAN_State_e enumeration.
   */
   CAN_State_e state(void);

   bool  setMode(CAN_Mode_e & teModeR);

   /*!
   ** \param[out]    clFrameDataR   Reference to data frame
   ** \param[in,out] pubFrameTypeV  Pointer to frame type   
   ** \return        \c true if CAN frame was read
   ** \see           writeFrame()
   **
   ** The function reads a data frame from the CAN socket and places the
   ** result in \a clFrameDataR. If no CAN frame is available, the function
   ** returns \c false. The optional pointer \a pubFrameTypeV holds
   ** the frame type, defined by the QCanData::Type_e enumeration.
   */
   bool  read( QByteArray & clFrameDataR, 
               QCanData::Type_e * pubFrameTypeV = Q_NULLPTR);
   

   bool  readFrame(QCanFrame & clFrameR);

   bool  write(const QByteArray & clFrameDataR);
   
   /*!
   ** \param[in]  clFrameR       Reference to CAN frame
   ** \return     \c true if CAN frame was written
   ** \see  readFrame()
   **
   ** The function writes the CAN frame \a clFrameR to the CAN socket. If
   ** writing fails, the function returns \c false.
   */
   bool  writeFrame(const QCanFrame & clFrameR);

   /*!
   ** \param[in]  clFrameR       Reference to CAN API frame
   ** \return     \c true if CAN frame was written
   **
   ** This is an overloaded function, using QCanFrameApi as parameter.
   */
   bool  writeFrame(const QCanFrameApi & clFrameR);

   /*!
   ** \param[in]  clFrameR       Reference to CAN error frame
   ** \return     \c true if CAN frame was written
   **
   ** This is an overloaded function, using QCanFrameError as parameter.
   */
   bool  writeFrame(const QCanFrameError & clFrameR);
   

public slots:



signals:

   /*!
   ** This signal is emitted after connectNetwork() has been called and
   ** a connection has been successfully established.
   */
   void  connected(void);

   /*!
   ** This signal is emitted when the socket has been disconnected.
   */
   void  disconnected(void);

   /*!
   ** \param[in]  teSockErrorV   Type of error
   **
   ** This signal is emitted after an error occurred. The \c teSockErrorV
   ** parameter describes the type of error that occurred.
   */
   void  error(QAbstractSocket::SocketError teSockErrorV);


   /*!
   ** \param[in]  ulFrameCntV    Number of CAN frames
   **
   ** This signal is emitted when CAN frames are available for reading
   ** from the socket. The \a ulFrameCntV parameter holds the number
   ** of available CAN frames.
   */
   void  framesReceived(uint32_t ulFrameCntV);

protected:

private:

   QPointer<QLocalSocket>  pclLocalSockP;
   QPointer<QTcpSocket>    pclTcpSockP;
   QHostAddress            clTcpHostAddrP;
   uint16_t                uwTcpPortP;
   bool                    btIsConnectedP;
   bool                    btIsLocalConnectionP;
   int32_t                 slSocketErrorP;

private slots:
   virtual void   onSocketConnect(void);
   virtual void   onSocketDisconnect(void);
   void           onSocketErrorLocal(QLocalSocket::LocalSocketError teSocketErrorV);
   void           onSocketErrorTcp(QAbstractSocket::SocketError teSocketErrorV);
   virtual void   onSocketReceive(void);
};

#endif   // QCAN_SOCKET_HPP_
