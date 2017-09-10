//============================================================================//
// File:          qcan_socket_dialog.hpp                                      //
// Description:   QCAN classes - CAN socket dialog                            //
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


#ifndef QCAN_SOCKET_DIALOG_HPP_
#define QCAN_SOCKET_DIALOG_HPP_

#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtNetwork/QHostAddress>
#include <QtWidgets/QDialog>

#include "qcan_socket.hpp"

class QCanSocketDialogPrivate;

//-----------------------------------------------------------------------------
/*!
** \class QCanSocketDialog
** \brief CAN socket dialog
**
** The QCanSocketDialog class provides a dialog for connection to an existing 
** QCanNetwork. The dialog creates a new QCanSocket and tries to connect
** after show() is called. It is possible to setup the desired CAN channel
** by calling setChannel() is advance. Also the %CANpie server host address
** can be configured by calling setHostAddress().
** <p>
** The status of the connection is displayed after the <b>CAN details</b>
** label. 
** \image html qcan_socket_dialog.png "QCanSocketDialog"
**
*/
class QCanSocketDialog : public QDialog
{
   Q_OBJECT

public:
   
   /*!
   ** \param[in]  pclParentV     Pointer to QWidget parent class
   **
   ** Create a new CAN socket dialog.
   */
   QCanSocketDialog(QWidget *pclParentV, Qt::WindowFlags f);

   /*!
   ** \param[in]  pclParentV     Pointer to QWidget parent class
   **
   ** Create a new CAN socket dialog.
   */
   QCanSocketDialog(QWidget * pclParentV = Q_NULLPTR,
                    const QString &caption = QString());

   ~QCanSocketDialog();
   
   QHostAddress   peerAddress() const;
   
   /*!
   ** \param[in]  clHostAddressV Server host address
   **
   ** Setup the %CANpie server host address which should be displayed as
   ** default before the show() method is called.
   */
   void           setHostAddress(const QHostAddress clHostAddressV);
   
   /*!
   ** \param[in]  teChannelV     CAN channel number
   **
   ** Setup the CAN channel number which should be displayed as
   ** default before the show() method is called.
   */
   void           setChannel(const CAN_Channel_e teChannelV);

   void           setSocket(QCanSocket * pclSocketV);

   /*!
   ** Show CAN socket dialog.
   */
   void           show(void);
   
   QCanSocket     socket(void) const;
   
Q_SIGNALS:
   /*!
   ** \param[in]  pclSocketV   Pointer to socket
   **
   ** This signal is emitted when the dialog window is closed using the
   ** OK button.
   */
   void           socketSelected(QCanSocket * pclSocketV);
   
   
protected:
   virtual void   done(int slResultV);
   virtual void   accept(void);
   
private Q_SLOTS:
   void           onHostChanged(int slIndexV);
   void           onChannelChanged(int slIndexV);
   void           onConnectionTimer(void);
   void           onSocketConnected(void);
   void           onSocketDisconnected(void);
   void           onSocketError(QAbstractSocket::SocketError teSocketErrorV);

private:
   QCanSocketDialogPrivate *  pclWidgetP;
   QPointer<QCanSocket>       pclSocketP;
   QTimer *                   pclTimerP;

   void           connect(void);
   void           connectSlots(void);
   void           disconnectSlots(void);
   
};


#endif // QCAN_SOCKET_DIALOG_HPP_
