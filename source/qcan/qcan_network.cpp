//============================================================================//
// File:          qcan_network.cpp                                            //
// Description:   QCAN classes - CAN network                                  //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53842 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions         //
// are met:                                                                   //
// 1. Redistributions of source code must retain the above copyright          //
//    notice, this list of conditions, the following disclaimer and           //
//    the referenced file 'COPYING'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'COPYING' file.                   //
//                                                                            //
//============================================================================//


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <QDebug>

#include "qcan_interface.hpp"
#include "qcan_network.hpp"


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------*\
** Static variables                                                           **
**                                                                            **
\*----------------------------------------------------------------------------*/
uint8_t  QCanNetwork::ubNetIdP = 0;

/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanNetwork()                                                              //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanNetwork::QCanNetwork(QObject * pclParentV,
                         uint16_t  uwPortV)
{
   //----------------------------------------------------------------
   // set the parent
   //
   this->setParent(pclParentV);

   //----------------------------------------------------------------
   // each network has a unique network number, starting with 1
   //
   ubNetIdP++;

   pclInterfaceP.clear();

   //----------------------------------------------------------------
   // set default network name
   //
   clNetNameP = "CAN " + QString("%1").arg(ubNetIdP);

   //----------------------------------------------------------------
   // create initial socket list
   //
   pclTcpSockListP = new QVector<QTcpSocket *>;
   pclTcpSockListP->reserve(QCAN_TCP_SOCKET_MAX);

   //----------------------------------------------------------------
   // setup a new local server which is listening to the
   // default network name
   //
   pclTcpSrvP = new QTcpServer();

   clTcpHostAddrP = QHostAddress(QHostAddress::LocalHost);
   uwTcpPortP = uwPortV;

   //----------------------------------------------------------------
   // clear statistic
   //
   ulCntFrameApiP = 0;
   ulCntFrameCanP = 0;
   ulCntFrameErrP = 0;

   ulDispatchTimeP = 20;
   slBitrateP = QCan::eCAN_BITRATE_500K;
}


//----------------------------------------------------------------------------//
// ~QCanNetwork()                                                             //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanNetwork::~QCanNetwork()
{
   //----------------------------------------------------------------
   // close TCP server
   //
   if(pclTcpSrvP->isListening())
   {
      pclTcpSrvP->close();
   }
   delete(pclTcpSrvP);

   ubNetIdP--;
}


//----------------------------------------------------------------------------//
// addInterface()                                                             //
// add physical CAN interface (plugin)                                        //
//----------------------------------------------------------------------------//
bool QCanNetwork::addInterface(QCanInterface * pclCanIfV)
{
   bool  btResultT = false;

   if(pclInterfaceP.isNull())
   {
      pclInterfaceP = pclCanIfV;
      btResultT = true;
   }
   return (btResultT);
}



bool QCanNetwork::hasErrorFramesSupport(void)
{
   return(false);
}

bool QCanNetwork::hasFastDataSupport(void)
{
   return(false);

}

bool QCanNetwork::hasListenOnlySupport(void)
{
   return(false);

}



//----------------------------------------------------------------------------//
// handleApiFrame()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
bool  QCanNetwork::handleApiFrame(int32_t & slSockSrcR,
                                  QCanFrameApi & clApiFrameR)
{
   bool btResultT = false;

   switch(clApiFrameR.function())
   {
      case QCanFrameApi::eAPI_FUNC_NONE:

         break;

      case QCanFrameApi::eAPI_FUNC_BITRATE:
         if(!pclInterfaceP.isNull())
         {
            pclInterfaceP->setBitrate( clApiFrameR.bitrate(),
                                       clApiFrameR.brsClock());
         }
         btResultT = true;
         break;

      case QCanFrameApi::eAPI_FUNC_CAN_MODE:

         break;

      case QCanFrameApi::eAPI_FUNC_DRIVER_INIT:

         break;

      case QCanFrameApi::eAPI_FUNC_DRIVER_RELEASE:

         break;
   }

   ulCntFrameApiP++;
   return(btResultT);
}


//----------------------------------------------------------------------------//
// handleCanFrame()                                                           //
// push QCan frame to all open sockets                                        //
//----------------------------------------------------------------------------//
bool  QCanNetwork::handleCanFrame(int32_t & slSockSrcR,
                                  QByteArray & clSockDataR)
{
   int32_t        slSockIdxT;
   bool           btResultT = false;
   QTcpSocket *   pclSockS;


   //----------------------------------------------------------------
   // check all open sockets and write CAN frame
   //
   for(slSockIdxT = 0; slSockIdxT < pclTcpSockListP->size(); slSockIdxT++)
   {
      if(slSockIdxT != slSockSrcR)
      {
         pclSockS = pclTcpSockListP->at(slSockIdxT);
         pclSockS->write(clSockDataR);
         pclSockS->flush();
         btResultT = true;
      }
   }


   if(btResultT == true)
   {
      ulCntFrameCanP++;
   }
   return(btResultT);
}


//----------------------------------------------------------------------------//
// handleErrorFrame()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
bool  QCanNetwork::handleErrFrame(int32_t & slSockSrcR,
                                  QCanFrameError & clErrorFrameR)
{
   bool           btResultT = false;

   return(btResultT);
}


//----------------------------------------------------------------------------//
// onTcpSrvNewConnection()                                                    //
// slot that manages a new local server connection                            //
//----------------------------------------------------------------------------//
void QCanNetwork::onSocketConnect(void)
{
   QTcpSocket *    pclSocketT;

   //----------------------------------------------------------------
   // Get next pending connect and add this socket to the
   // the socket list
   //
   pclSocketT =  pclTcpSrvP->nextPendingConnection();
   clTcpSockMutexP.lock();
   pclTcpSockListP->append(pclSocketT);
   clTcpSockMutexP.unlock();

   qDebug() << "QCanNetwork::onSocketConnect()" << pclTcpSockListP->size() << "open sockets";
   qDebug() << "Socket" << pclSocketT;

   //----------------------------------------------------------------
   // Add a slot that handles the disconnection of the socket
   // from the local server
   //
   connect( pclSocketT,
            SIGNAL(disconnected()),
            this,
            SLOT(onSocketDisconnect())   );
}


//----------------------------------------------------------------------------//
// setBitrate()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanNetwork::setBitrate(int32_t slBitrateV, int32_t slBrsClockV)
{
   slBitrateP  = slBitrateV;
   slBrsClockP = slBrsClockV;
}


//----------------------------------------------------------------------------//
// setDispatcherTime()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanNetwork::setDispatcherTime(uint32_t ulTimeV)
{
   ulDispatchTimeP = ulTimeV;
}


//----------------------------------------------------------------------------//
// setErrorFramesEnabled()                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanNetwork::setErrorFramesEnabled(bool btEnableV)
{
   if(hasErrorFramesSupport() == true)
   {
      btErrorFramesEnabledP = btEnableV;
   }
   else
   {
      btErrorFramesEnabledP = false;
   }
}


//----------------------------------------------------------------------------//
// setFastDataEnabled()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanNetwork::setFastDataEnabled(bool btEnableV)
{
   if(hasFastDataSupport() == true)
   {
      btFastDataEnabledP = btEnableV;
   }
   else
   {
      btFastDataEnabledP = false;
   }
}


//----------------------------------------------------------------------------//
// setListenOnlyEnabled()                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanNetwork::setListenOnlyEnabled(bool btEnableV)
{
   {btListenOnlyEnabledP = btEnableV;};

}


//----------------------------------------------------------------------------//
// setNetworkEnabled()                                                        //
// start / stop the TCP server                                                //
//----------------------------------------------------------------------------//
void QCanNetwork::setNetworkEnabled(bool btEnableV)
{

   if(btEnableV == true)
   {
      //--------------------------------------------------------
      // limit the number of connections
      //
      pclTcpSrvP->setMaxPendingConnections(QCAN_TCP_SOCKET_MAX);

      if(!pclTcpSrvP->listen(clTcpHostAddrP, uwTcpPortP))
      {
         qDebug() << "QCanNetwork(): can not listen to " << clNetNameP;
      }

      //--------------------------------------------------------
      // a new connection is handled by the onTcpSrvNewConnection()
      // method
      //
      connect( pclTcpSrvP, SIGNAL(newConnection()),
               this, SLOT(onSocketConnect()));


      //--------------------------------------------------------
      // start network thread
      //
      clDispatchTmrP.singleShot(ulDispatchTimeP, this, SLOT(onTimerEvent()));


      //--------------------------------------------------------
      // set flag for further operations
      //
      btNetworkEnabledP =  true;

   }
   else
   {
      //--------------------------------------------------------
      // stop timer for message dispatching
      //
      clDispatchTmrP.stop();

      //--------------------------------------------------------
      // remove signal / slot connection
      //
      disconnect( pclTcpSrvP, SIGNAL(newConnection()),
                     this, SLOT(onSocketConnect()));


      //--------------------------------------------------------
      // close TCP server
      //
      qDebug() << "Close server";
      pclTcpSrvP->close();

      //--------------------------------------------------------
      // set flag for further operations
      //
      btNetworkEnabledP =  false;

   }

}


//----------------------------------------------------------------------------//
// onSocketDisconnect()                                                       //
// remove local socket from list                                              //
//----------------------------------------------------------------------------//
void QCanNetwork::onSocketDisconnect(void)
{
   int32_t      slSockIdxT;
   QTcpSocket * pclSockT;
   QTcpSocket * pclSenderT;


   //----------------------------------------------------------------
   // get sender of signal
   //
   pclSenderT = (QTcpSocket* ) QObject::sender();

   clTcpSockMutexP.lock();
   for(slSockIdxT = 0; slSockIdxT < pclTcpSockListP->size(); slSockIdxT++)
   {
      pclSockT = pclTcpSockListP->at(slSockIdxT);
      if(pclSockT == pclSenderT)
      {
         pclTcpSockListP->remove(slSockIdxT);
         break;
      }
   }
   clTcpSockMutexP.unlock();

   qDebug() << "QCanNetwork::onSocketDisconnect()" << pclTcpSockListP->size() << "open sockets";

}


//----------------------------------------------------------------------------//
// onTimerEvent()                                                             //
// remove local socket from list                                              //
//----------------------------------------------------------------------------//
void QCanNetwork::onTimerEvent(void)
{
   int32_t        slSockIdxT;
   int32_t        slListSizeT;
   uint32_t       ulFrameCntT;
   uint32_t       ulFrameMaxT;
   QTcpSocket *   pclSockT;
   QCanFrame      clCanFrameT;
   QByteArray     clSockDataT;


   //----------------------------------------------------------------
   // lock socket list
   //
   clTcpSockMutexP.lock();


   //----------------------------------------------------------------
   // check all open sockets and read messages
   //
   slListSizeT = pclTcpSockListP->size();
   for(slSockIdxT = 0; slSockIdxT < slListSizeT; slSockIdxT++)
   {
      pclSockT = pclTcpSockListP->at(slSockIdxT);
      ulFrameMaxT = (pclSockT->bytesAvailable()) / QCAN_FRAME_ARRAY_SIZE;
      for(ulFrameCntT = 0; ulFrameCntT < ulFrameMaxT; ulFrameCntT++)
      {
         clSockDataT = pclSockT->read(QCAN_FRAME_ARRAY_SIZE);
         clCanFrameT.fromByteArray(clSockDataT);

         switch(clCanFrameT.frameType())
         {
            //---------------------------------------------
            // handle API frames
            //---------------------------------------------
            case QCanFrame::eTYPE_QCAN_API:
               handleApiFrame(slSockIdxT, (QCanFrameApi &) clCanFrameT);
               break;

            //---------------------------------------------
            // handle error frames
            //---------------------------------------------
            case QCanFrame::eTYPE_QCAN_ERR:
               handleErrFrame(slSockIdxT, (QCanFrameError &) clCanFrameT);
               break;

            //---------------------------------------------
            // handle CAN frames
            //---------------------------------------------
            default:
               //-------------------------------------
               // check for active CAN interface
               //
               if(pclInterfaceP.isNull() == false)
               {
                  pclInterfaceP->write(clCanFrameT);
               }

               //-------------------------------------
               // write to other sockets
               //
               handleCanFrame(slSockIdxT, clSockDataT);
               break;
         }
      }
   }
   clTcpSockMutexP.unlock();

   //----------------------------------------------------------------
   // signal current statistic values
   //
   showApiFrames(ulCntFrameApiP);
   showCanFrames(ulCntFrameCanP);
   showErrFrames(ulCntFrameErrP);
   showLoad(50, 1234);

   clDispatchTmrP.singleShot(ulDispatchTimeP, this, SLOT(onTimerEvent()));
}


//----------------------------------------------------------------------------//
// removeInterface()                                                          //
// remove physical CAN interface (plugin)                                     //
//----------------------------------------------------------------------------//
void QCanNetwork::removeInterface(void)
{
   pclInterfaceP.clear();
}

