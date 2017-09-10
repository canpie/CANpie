//============================================================================//
// File:          qcan_network.cpp                                            //
// Description:   QCAN classes - CAN network                                  //
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

static uint32_t   aulDlc2Bitlength[] = { 0,  8,  16,  24,  32,  40,  48,  56, 
                                        64, 96, 128, 160, 192, 256, 384, 512  };

/*----------------------------------------------------------------------------*\
** Static functions                                                           **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// getBitrate()                                                               //
// The functions converts the enumeration value CAN_Bitrate_e to a value      //                                                                 
// using the unit [bit/s]                                                     //                      
//----------------------------------------------------------------------------//
static int32_t getBitrate(int32_t slPreDefValueV)
{
   int32_t slBitrateT;
   
   switch (slPreDefValueV)
   {
      case eCAN_BITRATE_10K:
         slBitrateT = 10000;
         break;
         
      case eCAN_BITRATE_20K:
         slBitrateT = 20000;
         break;
      
      case eCAN_BITRATE_50K:
         slBitrateT = 50000;
         break;
         
      case eCAN_BITRATE_100K:
         slBitrateT = 100000;
         break;
         
      case eCAN_BITRATE_125K:
         slBitrateT = 125000;
         break;
         
      case eCAN_BITRATE_250K:
         slBitrateT = 250000;
         break;
         
      case eCAN_BITRATE_500K:
         slBitrateT = 500000;
         break;
         
      case eCAN_BITRATE_800K:
         slBitrateT = 800000;
         break;

      case eCAN_BITRATE_1M:
         slBitrateT = 1000000;
         break;
         
      case eCAN_BITRATE_2M:
         slBitrateT = 2000000;
         break;
         
      case eCAN_BITRATE_4M:
         slBitrateT = 4000000;
         break;

      case eCAN_BITRATE_5M:
         slBitrateT = 5000000;
         break;

      default:
         slBitrateT = eCAN_BITRATE_NONE;
         break;
   }
   
   return (slBitrateT);
}

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
   ubIdP = ubNetIdP;

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

   ulFramePerSecMaxP = 0;
   ulFrameCntSaveP   = 0;

   //----------------------------------------------------------------
   // setup timing values
   //
   ulDispatchTimeP  = 20;
   ulStatisticTimeP = 1000;
   ulStatisticTickP = ulStatisticTimeP / ulDispatchTimeP;


   btNetworkEnabledP     = false;
   btErrorFramesEnabledP = false;
   btListenOnlyEnabledP  = false;
   btFastDataEnabledP    = false;

   //----------------------------------------------------------------
   // setup default bit-rate
   //
   slNomBitRateP = eCAN_BITRATE_NONE;
   slDatBitRateP = eCAN_BITRATE_NONE;

   //----------------------------------------------------------------
   // set initial error state of an interface
   //
   teCanStateP   = eCAN_STATE_STOPPED;
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
         if (pclCanIfV->setBitrate(slNomBitRateP, slDatBitRateP) == QCanInterface::eERROR_NONE)
         {
            if (pclCanIfV->setMode(eCAN_MODE_OPERATION) == QCanInterface::eERROR_NONE)
            {
               pclInterfaceP = pclCanIfV;
               btResultT = true;

               addLogMessage(CAN_Channel_e (id()),
                             "Add CAN interface " + pclInterfaceP->name(),
                             eLOG_LEVEL_NOTICE);
            }
            else
            {
               addLogMessage(CAN_Channel_e (id()),
                             "Failed to start CAN interface " + pclInterfaceP->name(),
                             eLOG_LEVEL_WARN);
            }
         }
         else
         {
            addLogMessage(CAN_Channel_e (id()),
                          "Failed to initialise CAN interface " + pclInterfaceP->name(),
                          eLOG_LEVEL_WARN);
         }
      }

   }
   return (btResultT);
}


QString QCanNetwork::dataBitrateString(void)
{
   QString  clDatBitRateT;

   if (slDatBitRateP == eCAN_BITRATE_NONE)
   {
      clDatBitRateT = "None";
   }
   else
   {
      if (slDatBitRateP < 1000000)
      {
         //------------------------------------------------
         // print values < 1000000 in kBit/s
         //
         clDatBitRateT = QString("%1 kBit/s").arg(slDatBitRateP / 1000);
      }
      else
      {
         //------------------------------------------------
         // print values >= 1000000 in MBit/s
         //
         clDatBitRateT = QString("%1 MBit/s").arg(slDatBitRateP / 1000000);
      }
   }
   
   
   return (clDatBitRateT);

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
// frameSize()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
uint32_t QCanNetwork::frameSize(const QByteArray & clSockDataR)
{
   uint32_t ulBitCountT = 0;
   
   //----------------------------------------------------------------
   // test for CAN data frame
   //
   if ((clSockDataR.at(0) & 0xE0) == 0x00)
   {
      //--------------------------------------------------------
      // check the DLC value and convert to the number of
      // data bits inside this frame
      //
      ulBitCountT = aulDlc2Bitlength[(clSockDataR.at(4) & 0x0F)];

      //--------------------------------------------------------
      // add the number of bits for the protocol header, 
      // including possible stuff bits
      //
      switch (clSockDataR.at(5) & 0x03)
      {
         //------------------------------------------------
         // classical CAN, Standard Frame
         //
         case 0:
            ulBitCountT = ulBitCountT + 66;
            break;

         //------------------------------------------------
         // classical CAN, Extended Frame
         //
         case 1:
            ulBitCountT = ulBitCountT + 90;
            break;

         //------------------------------------------------
         // CAN FD, Standard Frame
         //
         case 2:
            ulBitCountT = ulBitCountT + 66;
            break;

         //------------------------------------------------
         // CAN FD, Extended Frame
         //
         case 3:
            ulBitCountT = ulBitCountT + 90;
            break;
      }
   }
   
   return (ulBitCountT);
}

//----------------------------------------------------------------------------//
// frameType()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
QCanData::Type_e  QCanNetwork::frameType(const QByteArray & clSockDataR)
{
   QCanData::Type_e  ubTypeT;
   
   //----------------------------------------------------------------
   // The frame type can be tested via the first byte of the array,
   // please refer to the implmentation of QCanData for details.
   //
   switch(clSockDataR.at(0) & 0xE0)
   {
      case 0x00:
         ubTypeT = QCanData::eTYPE_CAN;
         break;
      
      case 0x40:
         ubTypeT = QCanData::eTYPE_API;
         break;
      
      case 0x80:
         ubTypeT = QCanData::eTYPE_ERROR;
         break;
         
      default:
         ubTypeT = QCanData::eTYPE_UNKNOWN;
         break;
   }

   return (ubTypeT);
}


//----------------------------------------------------------------------------//
// handleApiFrame()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
bool  QCanNetwork::handleApiFrame(int32_t & slSockSrcR,
                                  QByteArray & clSockDataR)
{
   bool           btResultT = false;
   QCanFrameApi   clApiFrameT;
   
   clApiFrameT.fromByteArray(clSockDataR);
   
   if(slSockSrcR != QCAN_SOCKET_CAN_IF)
   {
      switch(clApiFrameT.function())
      {
         case QCanFrameApi::eAPI_FUNC_NONE:

            break;

         case QCanFrameApi::eAPI_FUNC_BITRATE:

            this->setBitrate( clApiFrameT.nominalBitrate(),
                              clApiFrameT.dataBitrate());
     
            btResultT = true;
            break;

         case QCanFrameApi::eAPI_FUNC_CAN_MODE:

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
      ulCntBitCurP = ulCntBitCurP + frameSize(clSockDataR);
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
   QCanFrameError clErrFrameT;

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

      clErrFrameT.fromByteArray(clSockDataR);

      if (teCanStateP != clErrFrameT.errorState())
      {
         switch (clErrFrameT.errorState())
         {
            case eCAN_STATE_STOPPED :
               addLogMessage(CAN_Channel_e (id()),
                             "State changes to STOPPED", eLOG_LEVEL_NOTICE);
               break;

            case eCAN_STATE_SLEEPING :
               addLogMessage(CAN_Channel_e (id()),
                             "State changes to SLEEPING", eLOG_LEVEL_NOTICE);

               break;

            case eCAN_STATE_BUS_ACTIVE :
               addLogMessage(CAN_Channel_e (id()),
                             "State changes to BUS ACTIVE", eLOG_LEVEL_NOTICE);
               break;

            case eCAN_STATE_BUS_WARN :
               addLogMessage(CAN_Channel_e (id()),
                             "State changes to BUS WARN with error type " +
                             QString::number(clErrFrameT.errorType(),16) + "h",
                             eLOG_LEVEL_WARN);
               break;

            case eCAN_STATE_BUS_PASSIVE :
               addLogMessage(CAN_Channel_e (id()),
                             "State changes to BUS PASSIVE with error type " +
                             QString::number(clErrFrameT.errorType(),16) + "h",
                             eLOG_LEVEL_WARN);
               break;

            case eCAN_STATE_BUS_OFF :
               addLogMessage(CAN_Channel_e (id()),
                             "State changes to BUS OFF with error type " +
                             QString::number(clErrFrameT.errorType(),16) + "h",
                             eLOG_LEVEL_WARN);
               break;

            default :
               addLogMessage(CAN_Channel_e (id()),
                             "State changes to unknown error " +
                             QString::number(clErrFrameT.errorState(),16) +
                             " with error type " +
                             QString::number(clErrFrameT.errorType(),16) + "h",
                             eLOG_LEVEL_ERROR);
               break;
         }


         teCanStateP = clErrFrameT.errorState();
      }

      ulCntFrameErrP++;
   }
   return(btResultT);
}


QHostAddress QCanNetwork::serverAddress(void)
{
   return (pclTcpSrvP->serverAddress());
}

void QCanNetwork::reset(void)
{
   ulCntFrameApiP = 0;
   ulCntFrameCanP = 0;
   ulCntFrameErrP = 0;
   ulCntBitCurP   = 0;

   ulFramePerSecMaxP = 0;
   ulFrameCntSaveP   = 0;
   teCanStateP   = eCAN_STATE_STOPPED;

   if(pclInterfaceP.isNull() == false)
   {
      pclInterfaceP->reset();
   }
}


//----------------------------------------------------------------------------//
// nominalBitrate()                                                           //
// return nominal bit-rate as QString value                                   //
//----------------------------------------------------------------------------//
QString QCanNetwork::nominalBitrateString(void)
{
   QString  clNomBitRateT;

   if (slNomBitRateP == eCAN_BITRATE_NONE)
   {
      clNomBitRateT = "None";
   }
   else
   {
      if (slNomBitRateP < 1000000)
      {
         //------------------------------------------------
         // print values < 1000000 in kBit/s
         //
         clNomBitRateT = QString("%1 kBit/s").arg(slNomBitRateP / 1000);
      }
      else
      {
         //------------------------------------------------
         // print values >= 1000000 in MBit/s
         //
         clNomBitRateT = QString("%1 MBit/s").arg(slNomBitRateP / 1000000);
      }
   }

   return (clNomBitRateT);
}


//----------------------------------------------------------------------------//
// onSocketConnect()                                                          //
// slot that manages a new local server connection                            //
//----------------------------------------------------------------------------//
void QCanNetwork::onSocketConnect(void)
{
   QTcpSocket *   pclSocketT;
   QCanFrameApi   clFrameApiT;
   
   //----------------------------------------------------------------
   // Get next pending connect and add this socket to the
   // the socket list
   //
   pclSocketT =  pclTcpSrvP->nextPendingConnection();
   clTcpSockMutexP.lock();
   pclTcpSockListP->append(pclSocketT);
   clTcpSockMutexP.unlock();

   //----------------------------------------------------------------
   // Prepare log message and send it
   //
   QString clSockOpenT = QString("total open: %1").arg(pclTcpSockListP->size());
   emit addLogMessage(CAN_Channel_e (id()),
                      "Socket connected, " + clSockOpenT,
                      eLOG_LEVEL_NOTICE);

   //----------------------------------------------------------------
   // Add a slot that handles the disconnection of the socket
   // from the local server
   //
   connect( pclSocketT,
            SIGNAL(disconnected()),
            this,
            SLOT(onSocketDisconnect())   );
   
   //----------------------------------------------------------------
   // Send information to client:
   // - interface name
   // - actual bit-rate
   //
   if (!pclInterfaceP.isNull())
   {
      clFrameApiT.setName(pclInterfaceP->name());
   }
   else
   {
      clFrameApiT.setName("Virtual CAN");
   }
   pclSocketT->write(clFrameApiT.toByteArray());
   clFrameApiT.setBitrate(slNomBitRateP, slDatBitRateP);
   pclSocketT->write(clFrameApiT.toByteArray());
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

   //----------------------------------------------------------------
   // Prepare log message and send it
   //
   QString clSockOpenT = QString("total open: %1").arg(pclTcpSockListP->size());
   emit addLogMessage(CAN_Channel_e (id()),
                      "Socket disconnect, " + clSockOpenT,
                      eLOG_LEVEL_NOTICE);
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
   QTcpSocket *   pclSockT;
   QCanFrame      clCanFrameT;
   QByteArray     clSockDataT;
   static uint32_t   ulTimerLogT = 0;

   //----------------------------------------------------------------
   // lock socket list
   //
   clTcpSockMutexP.lock();

   if (clTimeStartP.isNull())
   {
      clTimeStartP = QDateTime::currentDateTime();
   }
   
   //----------------------------------------------------------------
   // read messages from active CAN interface
   //
   if(pclInterfaceP.isNull() == false)
   {
      slSockIdxT = QCAN_SOCKET_CAN_IF;
      while(pclInterfaceP->read(clSockDataT) == QCanInterface::eERROR_NONE)
      {
         switch(frameType(clSockDataT))
         {
            //-----------------------------------------------------
            // handle API frames
            //
            case QCanData::eTYPE_API:
               addLogMessage(CAN_Channel_e (id()),
                             "Read eTYPE_API frame type " +
                             QString::number(frameType(clSockDataT),16) + "h",
                             eLOG_LEVEL_DEBUG);
               handleApiFrame(slSockIdxT, clSockDataT);
               break;
               
            //-------------------------------------
            // write CAN frame to other sockets
            //
            case QCanData::eTYPE_CAN:
               handleCanFrame(slSockIdxT, clSockDataT);
               break;
               
            //--------------------------------------------------
            // handle error frames
            //
            case QCanData::eTYPE_ERROR:
               handleErrFrame(slSockIdxT, clSockDataT);
               break;
               
            //-------------------------------------
            // nothing we can handle
            //
            default:
               addLogMessage(CAN_Channel_e (id()),
                             "Read unknown frame type " +
                             QString::number(frameType(clSockDataT),16) + "h",
                             eLOG_LEVEL_DEBUG);
               break;
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

         switch(frameType(clSockDataT))
         {
            //-----------------------------------------------------
            // handle API frames
            //
            case QCanData::eTYPE_API:
               handleApiFrame(slSockIdxT, clSockDataT);
               break;

            //-----------------------------------------------------
            // handle CAN frames
            //
            case QCanData::eTYPE_CAN:
               //---------------------------------------------
               // check for active CAN interface
               //
               if(pclInterfaceP.isNull() == false)
               {
                  clCanFrameT.fromByteArray(clSockDataT);
                  pclInterfaceP->write(clCanFrameT);
               }

               //---------------------------------------------
               // write to other sockets
               //
               handleCanFrame(slSockIdxT, clSockDataT);
               break;
                  
            //--------------------------------------------------
            // handle error frames
            //
            case QCanData::eTYPE_ERROR:
               handleErrFrame(slSockIdxT, clSockDataT);
               break;         
               
            default:
               addLogMessage(CAN_Channel_e (id()), "Wrong message type",
                             eLOG_LEVEL_DEBUG);

               break;
         }
      }
   }
   clTcpSockMutexP.unlock();

   clTimeStopP = QDateTime::currentDateTime();
   uint32_t ulTimeDiffT = clTimeStartP.msecsTo(clTimeStopP);
   if (ulTimeDiffT > 1000)
   {
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
      ulCntBitCurP = ulCntBitCurP / slNomBitRateP;
      if(ulCntBitCurP > 100)
      {
         ulCntBitCurP = 100;
      }

      // qDebug() << "CAN state" << teCanStateP;
      
      //--------------------------------------------------------
      // signal bus load and msg/sec
      //
      showLoad((uint8_t) ulCntBitCurP, ulMsgPerSecT);
      ulCntBitCurP = 0;

      //--------------------------------------------------------
      // store actual frame counter value
      //
      ulFrameCntSaveP = ulCntFrameCanP;
      

      showState(teCanStateP);

      //--------------------------------------------------------
      // set new value for start time
      //
      clTimeStartP = QDateTime::currentDateTime();
   }
   
   
   if (btNetworkEnabledP)
   {
      clDispatchTmrP.singleShot(ulDispatchTimeP, this, SLOT(onTimerEvent()));
   }

   ulTimerLogT++;
   if (ulTimerLogT > 999)
   {
      ulTimerLogT = 0;
      addLogMessage(CAN_Channel_e (id()),
                    "Timer event ", eLOG_LEVEL_DEBUG);
   }
}


//----------------------------------------------------------------------------//
// removeInterface()                                                          //
// remove physical CAN interface (plugin)                                     //
//----------------------------------------------------------------------------//
void QCanNetwork::removeInterface(void)
{
   if(pclInterfaceP.isNull() == false)
   {
      addLogMessage(CAN_Channel_e (id()),
                    "Remove CAN interface " + pclInterfaceP->name(),
                    eLOG_LEVEL_NOTICE);

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
   //----------------------------------------------------------------
   // test for pre-defined values from enumeration CAN_Bitrate_e
   // first and convert them in "real" bit-rate values
   //
   if (slNomBitRateV < eCAN_BITRATE_MAX)
   {
      slNomBitRateV = getBitrate(slNomBitRateV);
   }

   if (slDatBitRateV < eCAN_BITRATE_MAX)
   {
      slDatBitRateV = getBitrate(slDatBitRateV);
   }
   
   qDebug() << "QCanNetwork::setBitrate()" << slNomBitRateV << slDatBitRateV;
   
   //----------------------------------------------------------------
   // test if the new values differ from the currently stored
   // values
   //
   if ( (slNomBitRateV != slNomBitRateP) || (slDatBitRateP != slDatBitRateV) )
   {
      //--------------------------------------------------------
      // Store new bit-rates:
      // If there is no CAN FD support, the data bit-rate will 
      // be set to eCAN_BITRATE_NONE
      //
      slNomBitRateP  = slNomBitRateV;
      if(btFastDataEnabledP)
      {
         slDatBitRateP  = slDatBitRateV;
      }
      else
      {
         slDatBitRateP  = eCAN_BITRATE_NONE;
      }
      
      //--------------------------------------------------------
      // If there is an active CAN interface, configure the
      // new bit-rate
      //
      if(!pclInterfaceP.isNull())
      {
         pclInterfaceP->setMode(eCAN_MODE_INIT);
         pclInterfaceP->setBitrate(slNomBitRateP, slDatBitRateP);
         pclInterfaceP->setMode(eCAN_MODE_OPERATION);
      }

      //--------------------------------------------------------
      // show log message
      //
      if (btFastDataEnabledP)
      {
         addLogMessage(CAN_Channel_e (id()),
                       "Set nominal bit-rate " + nominalBitrateString() +
                       " and data bit-rate " + dataBitrateString(),
                       eLOG_LEVEL_NOTICE);
      }
      else
      {
         addLogMessage(CAN_Channel_e (id()),
                       "Set bit-rate " + nominalBitrateString(),
                       eLOG_LEVEL_NOTICE);
      }
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

   if (btErrorFramesEnabledP)
   {
      addLogMessage(CAN_Channel_e (id()), "CAN error frame support enabled",
                    eLOG_LEVEL_NOTICE);
   }
   else
   {
      addLogMessage(CAN_Channel_e (id()), "CAN error frame support disabled",
                    eLOG_LEVEL_NOTICE);
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

   if (btFastDataEnabledP)
   {
      addLogMessage(CAN_Channel_e (id()), "CAN FD support enabled",
                    eLOG_LEVEL_NOTICE);
   }
   else
   {
      addLogMessage(CAN_Channel_e (id()), "CAN FD support disabled",
                    eLOG_LEVEL_NOTICE);
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
         btNetworkEnabledP =  false;

         addLogMessage(CAN_Channel_e (id()),
                       "Failed to open server ", eLOG_LEVEL_ERROR);

         return;
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

      //--------------------------------------------------------
      // send log message
      //
      addLogMessage(CAN_Channel_e (id()),
                    "Open server ", eLOG_LEVEL_NOTICE);


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
      pclTcpSrvP->close();

      //--------------------------------------------------------
      // set flag for further operations
      //
      btNetworkEnabledP =  false;

      //--------------------------------------------------------
      // send log message
      //
      addLogMessage(CAN_Channel_e (id()),
                    "Close server ", eLOG_LEVEL_NOTICE);

   }

}


//----------------------------------------------------------------------------//
// setServerAddress()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanNetwork::setServerAddress(QHostAddress clHostAddressV)
{
   bool  btResultT = false;

   addLogMessage(CAN_Channel_e (id()),
                 QString("Set server address to " + clHostAddressV.toString()), eLOG_LEVEL_DEBUG);
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
