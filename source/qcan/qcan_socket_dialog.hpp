//====================================================================================================================//
// File:          qcan_socket_dialog.hpp                                                                              //
// Description:   QCAN classes - CAN socket dialog                                                                    //
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


#ifndef QCAN_SOCKET_DIALOG_HPP_
#define QCAN_SOCKET_DIALOG_HPP_

/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtNetwork/QHostAddress>
#include <QtWidgets/QDialog>

#include "qcan_socket.hpp"

class QCanSocketDialogPrivate;

//----------------------------------------------------------------------------------------------------------------
/*!
** \class QCanSocketDialog
** \brief CAN socket dialog
**
** The QCanSocketDialog class provides a dialog for connection to an existing QCanNetwork. The dialog creates a
** new QCanSocket and tries to connect after exec() is called. It is possible to setup the desired CAN channel
** by calling setChannel() in advance. Also the QCanServer host address can be configured by calling
** setHostAddress().
** <p>
** The following code snippet shows the usage of the QCanSocketDialog class.
** \code
** QCanSocketDialog * pclDialogT;
**
** pclDialogT = new QCanSocketDialog(this);
** if (pclDialogT->exec() == QDialog::Accepted)
** {
**    if (pclDialogT->channel() != eCAN_CHANNEL_NONE)
**    {
**       //------------------------------------------------
**       // get the selected CAN channel
**       //
**       uint8_t ubCanChannelT = pclDialogT->channel();
**
**       //------------------------------------------------
**       // if you don't want to use the socket, close it
**       // here
**       //
**       pclDialogT->socket()->disconnectNetwork();
**    }
** }
** delete (pclDialogT);
** \endcode
** <p>
** The status of the connection is displayed after the <b>CAN details</b> label.
** \image html qcan_socket_dialog.png "QCanSocketDialog"
**
*/
class QCanSocketDialog : public QDialog
{
   Q_OBJECT

public:
   
   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  pclParentV     Pointer to QWidget parent class
   **
   ** Create a new CAN socket dialog.
   */
   QCanSocketDialog(QWidget *pclParentV, Qt::WindowFlags f);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  pclParentV     Pointer to QWidget parent class
   **
   ** Create a new CAN socket dialog.
   */
   QCanSocketDialog(QWidget * pclParentV = nullptr, const QString &caption = QString());

   ~QCanSocketDialog() override;
   
   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     CAN channel number
   **
   ** The function returns the selected CAN channel number. In case 'Cancel' has been pressed or the
   ** connection to the QCanServer failed the function will return QCan::eCAN_CHANNEL_NONE.
   */
   QCan::CAN_Channel_e  channel() const;

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  QDialog::DialogCode
   **
   ** Shows the dialog as a modal dialog, blocking until the user closes it. The function returns either
   ** QDialog::Accepted or QDialog::Rejected.
   */
   int                  exec(void) override;

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Server host address
   ** \see        setHostAddress()
   **
   ** Setup the QCanServer host address which should be displayed as default before the exec() method
   ** is called.
   */
   QHostAddress         peerAddress() const;
   
   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clHostAddressV Server host address
   ** \see        peerAddress()
   **
   ** Setup the QCanServer host address which should be displayed as default before the exec() method
   ** is called.
   */
   void                 setHostAddress(const QHostAddress clHostAddressV);
   
   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelV     CAN channel number
   **
   ** Setup the CAN channel number which should be displayed as
   ** default before the exec() method is called.
   */
   void                 setChannel(const QCan::CAN_Channel_e teChannelV);

   void                 setSocket(QCanSocket * pclSocketV);
   
   QCanSocket           *socket(void) const;
   
Q_SIGNALS:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  pclSocketV   Pointer to socket
   **
   ** This signal is emitted when the dialog window is closed using the OK button.
   */
   void                 socketSelected(QCanSocket * pclSocketV);
   
   
protected:
   virtual void         done(int slResultV) override;
   virtual void         accept(void) override;
   
private Q_SLOTS:
   void                 onHostChanged(int slIndexV);
   void                 onChannelChanged(int slIndexV);
   void                 onConnectionTimer(void);
   void                 onSocketConnected(void);
   void                 onSocketDisconnected(void);
   void                 onSocketError(QAbstractSocket::SocketError teSocketErrorV);

private:
   QCanSocketDialogPrivate    *pclWidgetP;
   QPointer<QCanSocket>       pclSocketP;
   QHostAddress               clHostAddressP;
   QTimer *                   pclTimerP;
   QCan::CAN_Channel_e        teChannelP;
   bool                       btConnectFailP;

   void                 connect(void);
   void                 connectSlots(void);
   void                 disconnectSlots(void);
   
};


#endif // QCAN_SOCKET_DIALOG_HPP_
