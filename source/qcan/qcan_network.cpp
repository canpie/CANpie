//====================================================================================================================//
// File:          qcan_network.cpp                                                                                    //
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


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QDebug>

#include "qcan_defs.hpp"
#include "qcan_interface.hpp"
#include "qcan_network.hpp"


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define  REFRESH_TIMER_CYCLE_PERIOD          500



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


//--------------------------------------------------------------------------------------------------------------------//
// QCanNetwork()                                                                                                      //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanNetwork::QCanNetwork(QObject * pclParentV,
                         uint16_t  uwPortV)
{
   //---------------------------------------------------------------------------------------------------
   // set the parent
   //
   this->setParent(pclParentV);

   //---------------------------------------------------------------------------------------------------
   // each network has a unique network number, starting with 1
   //
   ubNetIdP++;
   ubIdP = ubNetIdP;

   pclInterfaceP.clear();

   //---------------------------------------------------------------------------------------------------
   // set default network name
   //
   clNetNameP = "CAN " + QString("%1").arg(ubNetIdP);

   //---------------------------------------------------------------------------------------------------
   // setup a new local server which is listening to the
   //
   pclLocalSrvP = new QLocalServer();

   //---------------------------------------------------------------------------------------------------
   // create initial local socket list
   //
   pclLocalSockListP = new QVector<QLocalSocket*>;
   pclLocalSockListP->reserve(QCAN_LOCAL_SOCKET_MAX);


   //---------------------------------------------------------------------------------------------------
   // setup a new TCP server which is listening to the default network name
   //
   pclTcpSrvP = new QTcpServer();
   clTcpHostAddrP = QHostAddress(QHostAddress::LocalHost);
   uwTcpPortP = uwPortV;

   //---------------------------------------------------------------------------------------------------
   // create initial TCP socket list
   //
   pclTcpSockListP = new QVector<QTcpSocket *>;
   pclTcpSockListP->reserve(QCAN_TCP_SOCKET_MAX);


   //---------------------------------------------------------------------------------------------------
   // clear statistic
   //
   ulCntFrameApiP = 0;
   ulCntFrameCanP = 0;
   ulCntFrameErrP = 0;
   ulCntBitCurP   = 0;

   ulFramePerSecMaxP = 0;
   ulFrameCntSaveP   = 0;

   ubBusLoadP = 0;

   //---------------------------------------------------------------------------------------------------
   // setup timing values
   //
   ulStatisticTimeP = 1000;

   btNetworkEnabledP       = false;
   btErrorFrameEnabledP    = false;
   btListenOnlyEnabledP    = false;
   btFastDataEnabledP      = false;
   btBitrateFrameEnabledP  = false;

   //---------------------------------------------------------------------------------------------------
   // setup default bit-rate
   //
   slNomBitRateP = eCAN_BITRATE_NONE;
   slDatBitRateP = eCAN_BITRATE_NONE;

   //---------------------------------------------------------------------------------------------------
   // Set initial error state of an interface
   // it is in "Stopped" state because it is not enabled
   //
   teCanStateP   = eCAN_STATE_STOPPED;


   //---------------------------------------------------------------------------------------------------
   // configure the refresh timer which updates all statistic information and sends some signals
   //
   connect(&clRefreshTimerP, SIGNAL(timeout()), this, SLOT(onTimerEvent()));
   clRefreshTimerP.start(REFRESH_TIMER_CYCLE_PERIOD);
}


//----------------------------------------------------------------------------//
// ~QCanNetwork()                                                             //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanNetwork::~QCanNetwork()
{
   //----------------------------------------------------------------
   // close local server
   //
   if (pclLocalSrvP->isListening())
   {
      pclLocalSrvP->close();
   }
   delete (pclLocalSrvP);

   //----------------------------------------------------------------
   // close TCP server
   //
   if (pclTcpSrvP->isListening())
   {
      pclTcpSrvP->close();
   }
   delete (pclTcpSrvP);

   ubNetIdP--;
}


//--------------------------------------------------------------------------------------------------------------------//
// addInterface()                                                                                                     //
// add physical CAN interface (plugin)                                                                                //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanNetwork::addInterface(QCanInterface * pclCanIfV)
{
   bool  btResultT = false;

   if (pclInterfaceP.isNull())
   {
      pclInterfaceP = pclCanIfV;

      //---------------------------------------------------------------------------------------------------
      // connect message logging from CAN interface
      //
      connect(pclInterfaceP, SIGNAL(addLogMessage(QString, LogLevel_e)),
              this,          SLOT(onInterfaceLogMessage(QString, LogLevel_e) ));

      //---------------------------------------------------------------------------------------------------
      // connect new data signal from CAN interface
      //
      connect(pclInterfaceP, SIGNAL(readyRead()),
              this,          SLOT(onInterfaceNewData() ));

      //---------------------------------------------------------------------------------------------------
      // connect state change from CAN interface
      //
      connect(pclInterfaceP, SIGNAL(stateChanged(CAN_State_e)),
              this,          SLOT(onInterfaceStateChange(CAN_State_e) ));

      btResultT = true;
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// dataBitrateString()                                                                                                //
// return QString value for data bit-rate                                                                             //
//--------------------------------------------------------------------------------------------------------------------//
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
// hasErrorFrameSupport()                                                    //
// Check if the CAN interface has error frame support                         //
//----------------------------------------------------------------------------//
bool QCanNetwork::hasErrorFrameSupport(void)
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
// hasSpecificConfigurationSupport()                                          //
// Check if the CAN interface has Devuce Specific Configuration support       //
//----------------------------------------------------------------------------//
bool QCanNetwork::hasSpecificConfigurationSupport(void)
{
   bool btResultT = false;

   if (!pclInterfaceP.isNull())
   {
      if (pclInterfaceP->supportedFeatures() & QCAN_IF_SUPPORT_SPECIFIC_CONFIG)
      {
         btResultT = true;
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
bool  QCanNetwork::handleApiFrame(enum FrameSource_e teFrameSrcV,
                                  int32_t & slSockSrcR,
                                  QByteArray & clSockDataR)
{
   Q_UNUSED(slSockSrcR);

   bool  btResultT = false;
   
   if (teFrameSrcV != eFRAME_SOURCE_CAN_IF)
   {
      QCanFrameApi   clApiFrameT;

      clApiFrameT.fromByteArray(clSockDataR);
      switch (clApiFrameT.function())
      {
         case QCanFrameApi::eAPI_FUNC_NONE:

            break;

         case QCanFrameApi::eAPI_FUNC_BITRATE:
            if (btBitrateFrameEnabledP)
            {
               this->setBitrate( clApiFrameT.nominalBitrate(),
                                 clApiFrameT.dataBitrate());
               btResultT = true;
            }
            else
            {
               addLogMessage(CAN_Channel_e (id()),
                             "Attempt to change bit-rate by application - prohibited by CANpie Server",
                             eLOG_LEVEL_WARN);
            }
            break;

         case QCanFrameApi::eAPI_FUNC_CAN_MODE:

            break;



         default:

            break;
      }

      ulCntFrameApiP++;
      
   }
   return (btResultT);
}


//----------------------------------------------------------------------------//
// handleCanFrame()                                                           //
// push QCan frame to all open sockets                                        //
//----------------------------------------------------------------------------//
bool  QCanNetwork::handleCanFrame(enum FrameSource_e teFrameSrcV,
                                  int32_t & slSockSrcR,
                                  QByteArray & clSockDataR)
{
   int32_t        slSockIdxT;
   bool           btResultT = false;
   QLocalSocket * pclLocalSockS;
   QTcpSocket *   pclTcpSockS;


   //----------------------------------------------------------------
   // check all open local sockets and write CAN frame
   //
   for (slSockIdxT = 0; slSockIdxT < pclLocalSockListP->size(); slSockIdxT++)
   {
      //--------------------------------------------------------
      // If the frame source is a local socket AND the socket
      // source value is equal to the socket index: don't copy
      // message
      //
      if ( (teFrameSrcV == eFRAME_SOURCE_SOCKET_LOCAL) &&
           (slSockIdxT  == slSockSrcR))
      {
         //------------------------------------------------
         // do not copy data back to source
         //
      }
      else
      {
         pclLocalSockS = pclLocalSockListP->at(slSockIdxT);
         pclLocalSockS->write(clSockDataR);
         btResultT = true;
      }
   }

   //----------------------------------------------------------------
   // check all open TCP sockets and write CAN frame
   //
   for (slSockIdxT = 0; slSockIdxT < pclTcpSockListP->size(); slSockIdxT++)
   {
      //--------------------------------------------------------
      // If the frame source is a TCP socket AND the socket
      // source value is equal to the socket index: don't copy
      // message
      //
      if ( (teFrameSrcV == eFRAME_SOURCE_SOCKET_TCP) &&
           (slSockIdxT  == slSockSrcR))
      {
         //------------------------------------------------
         // do not copy data back to source
         //
      }
      else
      {
         pclTcpSockS = pclTcpSockListP->at(slSockIdxT);
         pclTcpSockS->write(clSockDataR);
         pclTcpSockS->flush();
         btResultT = true;
      }
   }


   //----------------------------------------------------------------
   // count frame
   //
   ulCntFrameCanP++;
   ulCntBitCurP = ulCntBitCurP + frameSize(clSockDataR);

   return(btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// handleErrFrame()                                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool  QCanNetwork::handleErrFrame(enum FrameSource_e teFrameSrcV,
                                  int32_t & slSockSrcR,
                                  QByteArray & clSockDataR)
{
   Q_UNUSED(teFrameSrcV);

   int32_t        slSockIdxT;
   bool           btResultT = false;
   QLocalSocket * pclLocalSockS;
   QTcpSocket *   pclTcpSockS;
   QCanFrameError clErrFrameT;

   //---------------------------------------------------------------------------------------------------
   // check all open local sockets and write error frame
   //
   for (slSockIdxT = 0; slSockIdxT < pclLocalSockListP->size(); slSockIdxT++)
   {
      if (slSockIdxT != slSockSrcR)
      {
         pclLocalSockS = pclLocalSockListP->at(slSockIdxT);
         pclLocalSockS->write(clSockDataR);
         btResultT = true;
      }
   }


   //---------------------------------------------------------------------------------------------------
   // check all open TCP sockets and write error frame
   //
   for(slSockIdxT = 0; slSockIdxT < pclTcpSockListP->size(); slSockIdxT++)
   {
      if(slSockIdxT != slSockSrcR)
      {
         pclTcpSockS = pclTcpSockListP->at(slSockIdxT);
         pclTcpSockS->write(clSockDataR);
         pclTcpSockS->flush();
         btResultT = true;
      }
   }


   //---------------------------------------------------------------------------------------------------
   // count frame
   //
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

   return(btResultT);
}


//----------------------------------------------------------------------------//
// reset()                                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanNetwork::reset(void)
{

   //----------------------------------------------------------------
   // clear all counters
   //
   ulCntFrameApiP = 0;
   ulCntFrameCanP = 0;
   ulCntFrameErrP = 0;
   ulCntBitCurP   = 0;

   ulFramePerSecMaxP = 0;
   ulFrameCntSaveP   = 0;


   //----------------------------------------------------------------
   // the initial network state depends if it is enabled or not
   //
   if (btNetworkEnabledP)
   {
      teCanStateP = eCAN_STATE_BUS_ACTIVE;
   }
   else
   {
      teCanStateP = eCAN_STATE_STOPPED;
   }


   //----------------------------------------------------------------
   // If there is a physical CAN interface attached: reset it also
   //
   if (pclInterfaceP.isNull() == false)
   {
      pclInterfaceP->reset();
   }
}


//----------------------------------------------------------------------------//
// serverAddress()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
QHostAddress QCanNetwork::serverAddress(void)
{
   qDebug() << "QCanNetwork::serverAddress" << pclTcpSrvP->serverAddress();
   return (pclTcpSrvP->serverAddress());
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


//--------------------------------------------------------------------------------------------------------------------//
// onInterfaceLogMessage()                                                                                            //
// pass log messages from CAN interface to destination                                                                //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onInterfaceLogMessage(QString clMessageV, LogLevel_e teLogLevelV)
{
   emit addLogMessage(CAN_Channel_e (id()), clMessageV, teLogLevelV);
}


//--------------------------------------------------------------------------------------------------------------------//
// onInterfaceNewData()                                                                                               //
// handle new data from physical CAN interface                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onInterfaceNewData(void)
{
   int32_t        slSockIdxT;
   QByteArray     clSockDataT;

   //---------------------------------------------------------------------------------------------------
   // read messages from active CAN interface
   //
   if (pclInterfaceP.isNull() == false)
   {
      slSockIdxT = 0;
      QCanInterface::InterfaceError_e teInterfaceStatusT = pclInterfaceP->read(clSockDataT);
      while (teInterfaceStatusT == QCanInterface::eERROR_NONE)
      {
         switch (frameType(clSockDataT))
         {
            //---------------------------------------------------------------------------
            // handle API frames
            //
            case QCanData::eTYPE_API:
               addLogMessage(CAN_Channel_e (id()),
                             "Read eTYPE_API frame type " +
                             QString::number(frameType(clSockDataT),16) + "h",
                             eLOG_LEVEL_DEBUG);
               handleApiFrame(eFRAME_SOURCE_CAN_IF, slSockIdxT, clSockDataT);
               break;

            //---------------------------------------------------------------------------
            // write CAN frame to other sockets
            //
            case QCanData::eTYPE_CAN:
               handleCanFrame(eFRAME_SOURCE_CAN_IF, slSockIdxT, clSockDataT);
               break;

            //---------------------------------------------------------------------------
            // handle error frames
            //
            case QCanData::eTYPE_ERROR:
               handleErrFrame(eFRAME_SOURCE_CAN_IF, slSockIdxT, clSockDataT);
               break;

            //---------------------------------------------------------------------------
            // nothing we can handle
            //
            default:
               addLogMessage(CAN_Channel_e (id()),
                             "Read unknown frame type " +
                             QString::number(frameType(clSockDataT),16) + "h",
                             eLOG_LEVEL_DEBUG);
               break;
         }

         teInterfaceStatusT = pclInterfaceP->read(clSockDataT);
      }

   }

}

//--------------------------------------------------------------------------------------------------------------------//
// onInterfaceStateChange()                                                                                           //
// handle state change from physical CAN interface                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onInterfaceStateChange(CAN_State_e teStateV)
{
   //---------------------------------------------------------------------------------------------------
   // Two state changes are critical:
   // - An interface in stopped state means a problem with the hardware
   // - An interface in bus-off state means a problem with the network
   //
   switch (teStateV)
   {
      case eCAN_STATE_STOPPED:
         stopInterface();
         removeInterface();
         break;

      case eCAN_STATE_BUS_OFF:

         break;

      default:

         break;
   }

   //---------------------------------------------------------------------------------------------------
   // store the new state
   //
   addLogMessage(CAN_Channel_e (id()),  "Got something .. " + QString::number(teStateV), eLOG_LEVEL_DEBUG);
   teCanStateP = teStateV;

   //---------------------------------------------------------------------------------------------------
   // signal the new state to destination
   //
   emit showState(CAN_Channel_e (id()), teStateV);
}


//--------------------------------------------------------------------------------------------------------------------//
// onLocalSocketConnect()                                                                                             //
// slot that manages a new local server connection                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onLocalSocketConnect(void)
{
   QLocalSocket * pclSocketT;
   QCanFrameApi   clFrameApiT;

   //---------------------------------------------------------------------------------------------------
   // Get next pending connect and add this socket to the the socket list
   //
   pclSocketT =  pclLocalSrvP->nextPendingConnection();
   clLocalSockMutexP.lock();
   pclLocalSockListP->append(pclSocketT);
   clLocalSockMutexP.unlock();

   //---------------------------------------------------------------------------------------------------
   // Prepare log message and send it
   //
   QString clSockOpenT = QString("total open: %1").arg(pclLocalSockListP->size() + pclTcpSockListP->size());
   clSockOpenT += QString(" (Local: %1,").arg(pclLocalSockListP->size());
   clSockOpenT += QString(" TCP: %1)   ").arg(pclTcpSockListP->size());
   emit addLogMessage(CAN_Channel_e (id()),
                      "Local socket connected, " + clSockOpenT,
                      eLOG_LEVEL_NOTICE);

   //---------------------------------------------------------------------------------------------------
   // Add a slot that handles the disconnection of the socket from the local server
   //
   connect( pclSocketT, SIGNAL(disconnected()),
            this,       SLOT(onLocalSocketDisconnect())   );

   //---------------------------------------------------------------------------------------------------
   // Add a slot that handles when new data is available
   //
   connect( pclSocketT, SIGNAL(readyRead()),
            this,       SLOT(onLocalSocketNewData())   );

}


//--------------------------------------------------------------------------------------------------------------------//
// onLocalSocketDisconnect()                                                                                          //
// remove local socket from list                                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onLocalSocketDisconnect(void)
{
   int32_t        slSockIdxT;
   QLocalSocket * pclSockT;
   QLocalSocket * pclSenderT;


   //---------------------------------------------------------------------------------------------------
   // get sender of signal
   //
   pclSenderT = (QLocalSocket* ) QObject::sender();

   //---------------------------------------------------------------------------------------------------
   // Disconnect everything connected to the sender
   //
   disconnect(pclSenderT, 0, 0, 0);

   //---------------------------------------------------------------------------------------------------
   // remove sender from socket list
   //
   clLocalSockMutexP.lock();
   for(slSockIdxT = 0; slSockIdxT < pclLocalSockListP->size(); slSockIdxT++)
   {
      pclSockT = pclLocalSockListP->at(slSockIdxT);
      if (pclSockT == pclSenderT)
      {
         pclLocalSockListP->remove(slSockIdxT);
         break;
      }
   }
   clLocalSockMutexP.unlock();

   //---------------------------------------------------------------------------------------------------
   // Prepare log message and send it
   //
   QString clSockOpenT = QString("total open: %1").arg(pclLocalSockListP->size() + pclTcpSockListP->size());
   clSockOpenT += QString(" (Local: %1,").arg(pclLocalSockListP->size());
   clSockOpenT += QString(" TCP: %1)   ").arg(pclTcpSockListP->size());
   emit addLogMessage(CAN_Channel_e (id()),
                      "Local socket disconnected, " + clSockOpenT,
                      eLOG_LEVEL_NOTICE);
}


//--------------------------------------------------------------------------------------------------------------------//
// onLocalSocketNewData()                                                                                             //
// handle reception of new data on local socket                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onLocalSocketNewData(void)
{
   QLocalSocket * pclLocalSockT;
   int32_t        slSockIdxT;
   int32_t        slListSizeT;
   uint32_t       ulFrameMaxT;
   QCanFrame      clCanFrameT;
   QByteArray     clSockDataT;


   //---------------------------------------------------------------------------------------------------
   // lock socket list
   //
   clLocalSockMutexP.lock();


   //---------------------------------------------------------------------------------------------------
   // check all open local sockets and read messages
   //
   slListSizeT = pclLocalSockListP->size();
   for(slSockIdxT = 0; slSockIdxT < slListSizeT; slSockIdxT++)
   {
      pclLocalSockT = pclLocalSockListP->at(slSockIdxT);
      ulFrameMaxT = (pclLocalSockT->bytesAvailable()) / QCAN_FRAME_ARRAY_SIZE;
      while (ulFrameMaxT > 0)
      {
         clSockDataT = pclLocalSockT->read(QCAN_FRAME_ARRAY_SIZE);

         switch(frameType(clSockDataT))
         {
            //---------------------------------------------------------------------------
            // handle API frames
            //
            case QCanData::eTYPE_API:
               handleApiFrame(eFRAME_SOURCE_SOCKET_LOCAL, slSockIdxT, clSockDataT);
               break;

            //---------------------------------------------------------------------------
            // handle CAN frames
            //
            case QCanData::eTYPE_CAN:
               //-------------------------------------------------------------------
               // check for active CAN interface
               //
               if(pclInterfaceP.isNull() == false)
               {
                  clCanFrameT.fromByteArray(clSockDataT);
                  pclInterfaceP->write(clCanFrameT);
               }

               //-------------------------------------------------------------------
               // write to other sockets
               //
               handleCanFrame(eFRAME_SOURCE_SOCKET_LOCAL, slSockIdxT, clSockDataT);
               break;

            //----------------------------------------------------------------------
            // handle error frames
            //
            case QCanData::eTYPE_ERROR:
               handleErrFrame(eFRAME_SOURCE_SOCKET_LOCAL, slSockIdxT, clSockDataT);
               break;

            default:
               addLogMessage(CAN_Channel_e (id()), "Wrong message type",
                             eLOG_LEVEL_DEBUG);

               break;
         }

         ulFrameMaxT = (pclLocalSockT->bytesAvailable()) / QCAN_FRAME_ARRAY_SIZE;
      }
   }

   //---------------------------------------------------------------------------------------------------
   // unlock mutex
   //
   clLocalSockMutexP.unlock();
}


//--------------------------------------------------------------------------------------------------------------------//
// onTcpSocketConnect()                                                                                               //
// slot that manages a new TCP server connection                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onTcpSocketConnect(void)
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
   QString clSockOpenT = QString("total open: %1").arg(pclLocalSockListP->size() + pclTcpSockListP->size());
   clSockOpenT += QString(" (Local: %1),").arg(pclLocalSockListP->size());
   clSockOpenT += QString(" (TCP: %1)   ").arg(pclTcpSockListP->size());
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
            SLOT(onTcpSocketDisconnect())   );
   
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
// onTcpSocketDisconnect()                                                    //
// remove local socket from list                                              //
//----------------------------------------------------------------------------//
void QCanNetwork::onTcpSocketDisconnect(void)
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


//--------------------------------------------------------------------------------------------------------------------//
// onTcpSocketNewData()                                                                                               //
// handle reception of new data on TCP socket                                                                         //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onTcpSocketNewData()
{
   QTcpSocket *   pclTcpSockT;
   int32_t        slSockIdxT;
   int32_t        slListSizeT;
   uint32_t       ulFrameMaxT;
   QCanFrame      clCanFrameT;
   QByteArray     clSockDataT;


   //---------------------------------------------------------------------------------------------------
   // lock socket list
   //
   clTcpSockMutexP.lock();


   //---------------------------------------------------------------------------------------------------
   // check all open TCP sockets and read messages
   //
   slListSizeT = pclTcpSockListP->size();
   for(slSockIdxT = 0; slSockIdxT < slListSizeT; slSockIdxT++)
   {
      pclTcpSockT = pclTcpSockListP->at(slSockIdxT);
      ulFrameMaxT = (pclTcpSockT->bytesAvailable()) / QCAN_FRAME_ARRAY_SIZE;
      while (ulFrameMaxT > 0)
      {
         clSockDataT = pclTcpSockT->read(QCAN_FRAME_ARRAY_SIZE);

         switch(frameType(clSockDataT))
         {
            //-----------------------------------------------------
            // handle API frames
            //
            case QCanData::eTYPE_API:
               handleApiFrame(eFRAME_SOURCE_SOCKET_TCP, slSockIdxT, clSockDataT);
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
               handleCanFrame(eFRAME_SOURCE_SOCKET_TCP, slSockIdxT, clSockDataT);
               break;

            //--------------------------------------------------
            // handle error frames
            //
            case QCanData::eTYPE_ERROR:
               handleErrFrame(eFRAME_SOURCE_SOCKET_TCP, slSockIdxT, clSockDataT);
               break;

            default:
               addLogMessage(CAN_Channel_e (id()), "Wrong message type",
                             eLOG_LEVEL_DEBUG);

               break;
         }

         ulFrameMaxT = (pclTcpSockT->bytesAvailable()) / QCAN_FRAME_ARRAY_SIZE;

      }
   }


   //---------------------------------------------------------------------------------------------------
   // unlock mutex
   //
   clTcpSockMutexP.unlock();

}


//--------------------------------------------------------------------------------------------------------------------//
// onTimerEvent()                                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::onTimerEvent(void)
{
   uint32_t       ulMsgPerSecT;
   uint32_t       ulElapsedTimeT;


   //---------------------------------------------------------------------------------------------------
   // start statistic timer, if it is not already running
   //
   ulElapsedTimeT = (uint32_t) clStatisticTimeP.elapsed();
   if (ulElapsedTimeT > ulStatisticTimeP)
   {
      //--------------------------------------------------------------------------------------
      // signal current counter values
      //
      showApiFrames(CAN_Channel_e (id()), ulCntFrameApiP);
      showCanFrames(CAN_Channel_e (id()), ulCntFrameCanP);
      showErrFrames(CAN_Channel_e (id()), ulCntFrameErrP);
      
      //--------------------------------------------------------------------------------------
      // calculate messages per second
      //
      ulMsgPerSecT = ulCntFrameCanP - ulFrameCntSaveP;
      ulMsgPerSecT = ulMsgPerSecT * 1000;
      ulMsgPerSecT = ulMsgPerSecT / ulElapsedTimeT;

      //--------------------------------------------------------------------------------------
      // calculate bus load
      //
      ulCntBitCurP = ulCntBitCurP * 100;
      ulCntBitCurP = ulCntBitCurP / slNomBitRateP;
      if(ulCntBitCurP > 100)
      {
         ulCntBitCurP = 100;
      }

      //--------------------------------------------------------------------------------------
      // signal bus load and msg/sec
      //
      ubBusLoadP = (uint8_t) ulCntBitCurP;
      showLoad(CAN_Channel_e (id()), ubBusLoadP, ulMsgPerSecT);
      ulCntBitCurP = 0;

      //--------------------------------------------------------------------------------------
      // store actual frame counter value
      //
      ulFrameCntSaveP = ulCntFrameCanP;
      

      //--------------------------------------------------------------------------------------
      // signal the network state
      //
      showState(CAN_Channel_e (id()), teCanStateP);


      //--------------------------------------------------------------------------------------
      // set new value for start time
      //
      clStatisticTimeP.restart();
   }
   

}


//--------------------------------------------------------------------------------------------------------------------//
// removeInterface()                                                                                                  //
// remove physical CAN interface (plugin)                                                                             //
//--------------------------------------------------------------------------------------------------------------------//
void QCanNetwork::removeInterface(void)
{
   //---------------------------------------------------------------------------------------------------
   // disconnect all signals from CAN interface to network
   //
   disconnect(pclInterfaceP, 0, 0, 0);

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

      emit changeBitrate(CAN_Channel_e (id()), slNomBitRateP, slDatBitRateP);
   }

}


//----------------------------------------------------------------------------//
// setErrorFrameEnabled()                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanNetwork::setErrorFrameEnabled(bool btEnableV)
{
   if (hasErrorFrameSupport() == true)
   {
      btErrorFrameEnabledP = btEnableV;
   }
   else
   {
      btErrorFrameEnabledP = false;
   }

   if (btErrorFrameEnabledP)
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
// setInterfaceConfiguration()                                                //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanNetwork::setInterfaceConfiguration()
{
   //----------------------------------------------------------------
   // If there is an active CAN interface, call additional config
   //
   if(!pclInterfaceP.isNull())
   {
      pclInterfaceP->configureDevice();
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
// start / stop the server                                                    //
//----------------------------------------------------------------------------//
void QCanNetwork::setNetworkEnabled(bool btEnableV)
{

   if (btEnableV == true)
   {
      //--------------------------------------------------------
      // limit the number of connections for local server
      //
      pclLocalSrvP->setMaxPendingConnections(QCAN_TCP_SOCKET_MAX);
      pclLocalSrvP->setSocketOptions(QLocalServer::WorldAccessOption);
      if(!pclLocalSrvP->listen(QString("CANpieServerChannel%1").arg(ubIdP)))
      {
         btNetworkEnabledP =  false;

         addLogMessage(CAN_Channel_e (id()),
                       "Failed to open local server ", eLOG_LEVEL_ERROR);

         return;
      }
      addLogMessage(CAN_Channel_e (id()),
                    pclLocalSrvP->fullServerName(), eLOG_LEVEL_NOTICE);

      //--------------------------------------------------------
      // a new connection is handled by the
      // onLocalSocketConnect() method
      //
      connect( pclLocalSrvP, SIGNAL(newConnection()),
               this, SLOT(onLocalSocketConnect()));

      //--------------------------------------------------------
      // limit the number of connections for TCP server
      //
      pclTcpSrvP->setMaxPendingConnections(QCAN_TCP_SOCKET_MAX);

      if(!pclTcpSrvP->listen(clTcpHostAddrP, uwTcpPortP))
      {
         btNetworkEnabledP =  false;

         addLogMessage(CAN_Channel_e (id()),
                       "Failed to open TCP server ", eLOG_LEVEL_ERROR);

         return;
      }

      //--------------------------------------------------------
      // a new connection is handled by the onTcpSrvNewConnection()
      // method
      //
      connect( pclTcpSrvP, SIGNAL(newConnection()),
               this, SLOT(onTcpSocketConnect()));


      //--------------------------------------------------------
      // set flag for further operations
      //
      btNetworkEnabledP =  true;

      //--------------------------------------------------------
      // set initial error state of an interface
      //
      teCanStateP   = eCAN_STATE_BUS_ACTIVE;

      clStatisticTimeP.start();

      //--------------------------------------------------------
      // send log message
      //
      addLogMessage(CAN_Channel_e (id()),
                    "Open server ", eLOG_LEVEL_NOTICE);


   }
   else
   {
      //--------------------------------------------------------
      // remove signal / slot connections
      //
      disconnect( pclLocalSrvP, SIGNAL(newConnection()),
                  this, SLOT(onLocalSocketConnect()));

      disconnect( pclTcpSrvP, SIGNAL(newConnection()),
                  this, SLOT(onTcpSocketConnect()));


      //--------------------------------------------------------
      // close local and TCP server
      //
      pclLocalSrvP->close();
      pclTcpSrvP->close();

      //--------------------------------------------------------
      // set flag for further operations
      //
      btNetworkEnabledP =  false;

      //--------------------------------------------------------
      // set error state of an interface
      //
      teCanStateP   = eCAN_STATE_STOPPED;


      //--------------------------------------------------------
      // send log message
      //
      addLogMessage(CAN_Channel_e (id()),
                    "Close server ", eLOG_LEVEL_NOTICE);

   }

}


//--------------------------------------------------------------------------------------------------------------------//
// setServerAddress()                                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanNetwork::setServerAddress(QHostAddress clHostAddressV)
{
   bool  btResultT = false;

   addLogMessage(CAN_Channel_e (id()),
                 QString("Set server address to " + clHostAddressV.toString()), eLOG_LEVEL_NOTICE);
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


//--------------------------------------------------------------------------------------------------------------------//
// startInterface()                                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanNetwork::startInterface(void)
{
   bool  btResultT = false;

   if (!pclInterfaceP.isNull())
   {
      //-------------------------------------------------------------------------------------------
      // connect the interface
      //
      if (pclInterfaceP->connect() == QCanInterface::eERROR_NONE)
      {
         if (pclInterfaceP->setBitrate(slNomBitRateP, slDatBitRateP) == QCanInterface::eERROR_NONE)
         {
            if (pclInterfaceP->setMode(eCAN_MODE_OPERATION) == QCanInterface::eERROR_NONE)
            {

               addLogMessage(CAN_Channel_e (id()),
                             "Add CAN interface " + pclInterfaceP->name(),
                             eLOG_LEVEL_NOTICE);

               btResultT = true;
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
                          "Failed to configure bit-rate on CAN interface " + pclInterfaceP->name(),
                          eLOG_LEVEL_WARN);
         }
      }

   }

   return (btResultT);


}


//--------------------------------------------------------------------------------------------------------------------//
// stopInterface()                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanNetwork::stopInterface(void)
{
   bool  btResultT = false;

   if (pclInterfaceP.isNull() == false)
   {
      addLogMessage(CAN_Channel_e (id()), "Remove CAN interface " + pclInterfaceP->name(), eLOG_LEVEL_NOTICE);

      if (pclInterfaceP->connected())
      {
         pclInterfaceP->disconnect();
         btResultT = true;
      }
   }

   return (btResultT);
}
