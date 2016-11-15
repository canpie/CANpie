//============================================================================//
// File:          qcan_network.cpp                                            //
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


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <QDebug>

#include "qcan_defs.hpp"
#include "qcan_interface.hpp"
#include "qcan_network.hpp"


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-------------------------------------------------------------------
// The "socket number" of the physical CAN interface gets a high
// value in order to avoid conflicts with real sockets
//
#define  QCAN_SOCKET_CAN_IF      22345


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

   clTcpHostAddrP = QHostAddress(QHostAddress::Any);
   uwTcpPortP = uwPortV;

   //----------------------------------------------------------------
   // clear statistic
   //
   ulCntFrameApiP = 0;
   ulCntFrameCanP = 0;
   ulCntFrameErrP = 0;
   ulCntBitCurP   = 0;

   //----------------------------------------------------------------
   // setup timing values
   //
   ulDispatchTimeP  = 20;
   ulStatisticTimeP = 1000;
   ulStatisticTickP = ulStatisticTimeP / ulDispatchTimeP;


   //----------------------------------------------------------------
   // setup default bit-rate
   //
   setBitrate(eCAN_BITRATE_500K, -1);
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
      //--------------------------------------------------------
      // connect the interface
      //
      if(pclCanIfV->connect() == QCanInterface::eERROR_NONE)
      {
         // demo initialisation
         if (pclCanIfV->supportedFeatures() & QCAN_IF_SUPPORT_CAN_FD)
         {
            slNomBitRateP  = eCAN_BITRATE_500K;
            slDatBitRateP = 2000000;
         }

         if(pclCanIfV->setBitrate(slNomBitRateP, slDatBitRateP) == QCanInterface::eERROR_NONE)
         {
            if (pclCanIfV->setMode(eCAN_MODE_START) == QCanInterface::eERROR_NONE)
            {
               pclInterfaceP = pclCanIfV;
               btResultT = true;
            }
         }
      }

   }
   return (btResultT);
}


//----------------------------------------------------------------------------//
// hasErrorFramesSupport()                                                    //
// Check if the CAN interface has error frame support                         //
//----------------------------------------------------------------------------//
bool QCanNetwork::hasErrorFramesSupport(void)
{
   bool btResultT;
   
   //----------------------------------------------------------------
   // If no physical CAN interface is connected, the virtual CAN 
   // network can support error frames. Hence the default is TRUE.
   //
   btResultT = true;  
   
   //----------------------------------------------------------------
   // Check supported features of physical CAN interface.
   //
   if (!pclInterfaceP.isNull())
   {
      if (pclInterfaceP->supportedFeatures() & QCAN_IF_SUPPORT_ERROR_FRAMES)
      {
         btResultT = true;
      }
      else
      {
         btResultT = false;
      }
   }

   return(btResultT);
}


//----------------------------------------------------------------------------//
// hasFastDataSupport()                                                       //
// Check if the CAN interface has CAN FD support                              //
//----------------------------------------------------------------------------//
bool QCanNetwork::hasFastDataSupport(void)
{
   bool btResultT;
   
   //----------------------------------------------------------------
   // If no physical CAN interface is connected, the virtual CAN 
   // network can support FD frames. Hence the default is TRUE.
   //
   btResultT = true;  
   
   //----------------------------------------------------------------
   // Check supported features of physical CAN interface.
   //
   if (!pclInterfaceP.isNull())
   {
      if (pclInterfaceP->supportedFeatures() & QCAN_IF_SUPPORT_CAN_FD)
      {
         btResultT = true;
      }
      else
      {
         btResultT = false;
      }
   }
   
   return(btResultT);
}


//----------------------------------------------------------------------------//
// hasListenOnlySupport()                                                     //
// Check if the CAN interface has Listen-Only support                         //
//----------------------------------------------------------------------------//
bool QCanNetwork::hasListenOnlySupport(void)
{
   bool btResultT;
   
   //----------------------------------------------------------------
   // If no physical CAN interface is connected, the virtual CAN 
   // network can not support Listen-Only. Hence the default is FALSE.
   //
   btResultT = false;  
   
   
   if (!pclInterfaceP.isNull())
   {
      if (pclInterfaceP->supportedFeatures() & QCAN_IF_SUPPORT_LISTEN_ONLY)
      {
         btResultT = true;
      }
      else
      {
         btResultT = false;
      }
   }
   
   return(btResultT);
}


//----------------------------------------------------------------------------//
// handleApiFrame()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
bool  QCanNetwork::handleApiFrame(int32_t & slSockSrcR,
                                  QCanFrameApi & clApiFrameR)
{
   bool btResultT = false;

   if(slSockSrcR != QCAN_SOCKET_CAN_IF)
   {
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

         case QCanFrameApi::eAPI_FUNC_PROCESS_ID:

            break;

         default:

            break;
      }

      ulCntFrameApiP++;
      
   }
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


   //----------------------------------------------------------------
   // count frame if source is a CAN interface or if the message
   // could be dispatched
   //
   if((slSockSrcR == QCAN_SOCKET_CAN_IF) || (btResultT == true))
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


   //----------------------------------------------------------------
   // count frame if source is a CAN interface or if the message
   // could be dispatched
   //
   if((slSockSrcR == QCAN_SOCKET_CAN_IF) || (btResultT == true))
   {
      ulCntFrameErrP++;
   }
   return(btResultT);
}


QHostAddress QCanNetwork::serverAddress(void)
{
   return (pclTcpSrvP->serverAddress());
}


//----------------------------------------------------------------------------//
// onSocketConnect()                                                          //
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
   uint32_t       ulMsgPerSecT;
   uint32_t       ulMsgBitCntT;
   QTcpSocket *   pclSockT;
   QCanFrame      clCanFrameT;
   QByteArray     clSockDataT;


   //----------------------------------------------------------------
   // lock socket list
   //
   clTcpSockMutexP.lock();

   //----------------------------------------------------------------
   // read messages from active CAN interface
   //
   if(pclInterfaceP.isNull() == false)
   {
      slSockIdxT = QCAN_SOCKET_CAN_IF;
      while(pclInterfaceP->read(clCanFrameT) == QCanInterface::eERROR_NONE)
      {
         clSockDataT = clCanFrameT.toByteArray();

         //-----------------------------------------------------
         // handle API frames
         //
         if(clCanFrameT.isFrameApi())
         {
            handleApiFrame(slSockIdxT, (QCanFrameApi &) clCanFrameT);
         }
         //-----------------------------------------------------
         // handle error frames
         //
         else if(clCanFrameT.isFrameError())
         {
            handleErrFrame(slSockIdxT, clSockDataT);
         }
         //-------------------------------------
         // write CAN frame to other sockets
         //
         else
         {
            switch(clCanFrameT.frameType())
            {
               case CpFrame::eTYPE_CAN_STD:
                  ulMsgBitCntT = 66 + (8 * clCanFrameT.dataSize());
                  break;

               case CpFrame::eTYPE_CAN_EXT:
                  ulMsgBitCntT = 92 + (8 * clCanFrameT.dataSize());
                  break;

               default:
                  ulMsgBitCntT = 0;
                  break;
            }
            ulCntBitCurP += ulMsgBitCntT;
            handleCanFrame(slSockIdxT, clSockDataT);
         }
      }
   }

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

         //-----------------------------------------------------
         // handle API frames
         //
         if(clCanFrameT.isFrameApi())
         {
            handleApiFrame(slSockIdxT, (QCanFrameApi &) clCanFrameT);
         }
         //-----------------------------------------------------
         // handle error frames
         //
         else if(clCanFrameT.isFrameError())
         {
            handleErrFrame(slSockIdxT, clSockDataT);
         }
         //-----------------------------------------------------
         // handle CAN frames
         //
         else
         {
            //---------------------------------------------
            // check for active CAN interface
            //
            if(pclInterfaceP.isNull() == false)
            {
               pclInterfaceP->write(clCanFrameT);
            }

            //---------------------------------------------
            // write to other sockets
            //
            handleCanFrame(slSockIdxT, clSockDataT);
         }
      }
   }
   clTcpSockMutexP.unlock();

   //----------------------------------------------------------------
   // signal current statistic values
   //
   if(ulStatisticTickP > 0)
   {
      ulStatisticTickP--;
   }
   else
   {
      //--------------------------------------------------------
      // reload tick value
      //
      ulStatisticTickP = ulStatisticTimeP / ulDispatchTimeP;

      //--------------------------------------------------------
      // signal current counter values
      //
      showApiFrames(ulCntFrameApiP);
      showCanFrames(ulCntFrameCanP);
      showErrFrames(ulCntFrameErrP);

      //--------------------------------------------------------
      // calculate messages per second
      //
      ulMsgPerSecT = ulCntFrameCanP - ulFrameCntSaveP;

      //--------------------------------------------------------
      // calculate bus load
      //
      ulCntBitCurP = ulCntBitCurP * 100;
      ulCntBitCurP = ulCntBitCurP / ulCntBitMaxP;
      if(ulCntBitCurP > 100)
      {
         ulCntBitCurP = 100;
      }
      //--------------------------------------------------------
      // signal bus load and msg/sec
      //
      showLoad((uint8_t) ulCntBitCurP, ulMsgPerSecT);
      ulCntBitCurP = 0;

      //--------------------------------------------------------
      // store actual frame counter value
      //
      ulFrameCntSaveP = ulCntFrameCanP;
   }

   clDispatchTmrP.singleShot(ulDispatchTimeP, this, SLOT(onTimerEvent()));
}


//----------------------------------------------------------------------------//
// removeInterface()                                                          //
// remove physical CAN interface (plugin)                                     //
//----------------------------------------------------------------------------//
void QCanNetwork::removeInterface(void)
{
   if(pclInterfaceP.isNull() == false)
   {
      if (pclInterfaceP->connected())
      {
         pclInterfaceP->disconnect();
      }
   }
   pclInterfaceP.clear();
}


//----------------------------------------------------------------------------//
// setBitrate()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanNetwork::setBitrate(int32_t slNomBitRateV, int32_t slDatBitRateV)
{
   slNomBitRateP  = slNomBitRateV;
   slDatBitRateP  = slDatBitRateV;

   switch(slNomBitRateV)
   {
      case eCAN_BITRATE_250K:
         ulCntBitMaxP = 250000;
         break;

      case eCAN_BITRATE_500K:
         ulCntBitMaxP = 500000;
         break;
   }
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
   if(hasListenOnlySupport() == true)
   {
      btListenOnlyEnabledP = btEnableV;
   }
   else
   {
      btListenOnlyEnabledP = false;
   }
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
// setServerAddress()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanNetwork::setServerAddress(QHostAddress clHostAddressV)
{
   bool  btResultT = false;

   //----------------------------------------------------------------
   // host address can only be changed when network is disabled
   //
   if(btNetworkEnabledP == false)
   {
      clTcpHostAddrP = clHostAddressV;
      btResultT = true;
   }

   return(btResultT);
}
