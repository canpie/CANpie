//====================================================================================================================//
// File:          qcan_interface_peak.cpp                                                                             //
// Description:   QCan Interface of PCAN Basic library                                                                //
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

#include "qcan_interface_peak.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#ifndef  PCAN_ALLOW_ERROR_FRAMES
#define  PCAN_ALLOW_ERROR_FRAMES    ((uint8_t) 0x20)
#endif

#ifndef  PCAN_MESSAGE_ERRFRAME
#define  PCAN_MESSAGE_ERRFRAME      ((uint8_t) 0x40)
#endif


/*--------------------------------------------------------------------------------------------------------------------*\
** Class methods                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfacePeak()                                                                                                //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterfacePeak::QCanInterfacePeak(uint16_t uwPCanChannelV)
{
   char        aszBufferT[64];
   TPCANStatus tsStatusT;

   if (!pclPcanBasicP.isAvailable())
   {
      qCritical() << "QCanInterfacePeak(): PEAKBasic library is not available!";
   }

   //---------------------------------------------------------------------------------------------------
   // setup PCAN Channel of this interface
   //
   uwPCanChannelP = uwPCanChannelV;
   uwPCanBitrateP = PCAN_BAUD_500K; // initial value
   teCanModeP     = eCAN_MODE_STOP;

   //---------------------------------------------------------------------------------------------------
   // The interface is not yet connected
   //
   teConnectedP = UnconnectedState;

   //---------------------------------------------------------------------------------------------------
   // In this example all features are supported by the interface, the actual settings
   // are copied here
   //
   ulFeaturesP  = this->supportedFeatures();

   //---------------------------------------------------------------------------------------------------
   // message buffer for CAN message is empty
   //
   btHasReceivedFrameP = false;

   btFdUsedP = false;

   //---------------------------------------------------------------------------------------------------
   // clear error state and buffer for error frames
   //
   teErrorStateP = eCAN_STATE_STOPPED;

   clErrFrameP.setFrameType(QCanFrame::eFRAME_TYPE_ERROR);
   clErrFrameP.setErrorCounterReceive(0);
   clErrFrameP.setErrorCounterTransmit(0);
   clErrFrameP.setErrorState(eCAN_STATE_STOPPED);

   //---------------------------------------------------------------------------------------------------
   // connect the device and get alle necessary informations for further usage
   //
   connect();


   //---------------------------------------------------------------------------------------------------
   // set interface name depending on hardware name, device number and controller number
   //
   clNameP.clear();
   tsStatusT = pclPcanBasicP.pfnCAN_GetValueP(uwPCanChannelP, PCAN_HARDWARE_NAME,
                                              (void*)&aszBufferT[0], sizeof(aszBufferT));
   if (tsStatusT == PCAN_ERROR_OK)
   {
      clNameP.append(QLatin1String(aszBufferT));

      //-------------------------------------------------------------------------------------------
      // name should contain device number, get it
      //
      tsStatusT = pclPcanBasicP.pfnCAN_GetValueP(uwPCanChannelP, PCAN_DEVICE_NUMBER,
                                                 (void*)&aszBufferT[0], 1);
      if (tsStatusT == PCAN_ERROR_OK)
      {
         clNameP.append(" Device " + QString::number((uint8_t)aszBufferT[0],16).toUpper());

         //-----------------------------------------------------------------------------------
         // get CAN channel number
         //
         #ifdef Q_OS_MACOS

         #else
         tsStatusT = pclPcanBasicP.pfnCAN_GetValueP(uwPCanChannelP, PCAN_CONTROLLER_NUMBER,
                                                    (void*)&aszBufferT[0], 1);
         if (tsStatusT == PCAN_ERROR_OK)
         {
            clNameP.append("h CAN "+ QString::number(aszBufferT[0]+1,10));
         }
         #endif
      }
   }

   if (clNameP.isEmpty())
   {
      clNameP.append("Unknown");
   }

   //---------------------------------------------------------------------------------------------------
   // select icon of inteface depending on hardware name
   //
   tsStatusT = pclPcanBasicP.pfnCAN_GetValueP(uwPCanChannelP, PCAN_HARDWARE_NAME,
                                                          (void*)&aszBufferT[0], sizeof(aszBufferT));
   if (tsStatusT == PCAN_ERROR_OK)
   {
      QString clIconNameT;
      clIconNameT.append(QLatin1String(aszBufferT));
      clIconNameT = clIconNameT.toLower();
      clIconNameT.replace(QString("-"),QString("_"));
      clIconNameT.replace(QString(" "),QString("_"));
      clIconP = QIcon(QString(":/images/"+clIconNameT+".png"));
   } else
   {
      clIconP = QIcon(":/images/pcan_unknown_256.png");
   }

   //---------------------------------------------------------------------------------------------------
   // disconnect the device
   //
   disconnect();
}


//--------------------------------------------------------------------------------------------------------------------//
// ~QCanInterfacePeak()                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterfacePeak::~QCanInterfacePeak()
{
   qDebug() << "QCanInterfacePeak::~QCanInterfacePeak()";

   if (pclPcanBasicP.isAvailable())
   {
      if (teConnectedP == ConnectedState)
      {
         pclPcanBasicP.pfnCAN_UninitializeP(uwPCanChannelP);
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfacePeak::checkStatus()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanInterfacePeak::checkStatus(void)
{
   TPCANStatus       ulStatusT;

   ulStatusT = pclPcanBasicP.pfnCAN_GetStatusP(uwPCanChannelP);
   if (ulStatusT != PCAN_ERROR_OK)
   {
      QString msg = "Status: ";
      msg += QString("%1").arg(ulStatusT);
      emit addLogMessage(msg, eLOG_LEVEL_INFO);
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// connect()                                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfacePeak::connect(void)
{
   InterfaceError_e teReturnT = eERROR_LIBRARY;
   uint8_t          ubValueT;
   TPCANStatus      tsStatusT;

   #if QCAN_SUPPORT_CAN_FD > 0
   qDebug() << QString("QCanInterfacePeak::connect()... - using CAN FD mode");
   #else
   qDebug() << QString("QCanInterfacePeak::connect()... - using Classic CAN mode");
   #endif

   //---------------------------------------------------------------------------------------------------
   // tell some details about this CAN interface to the logger
   //
   emit addLogMessage("Connect driver version   : " + version(), eLOG_LEVEL_INFO);

   //---------------------------------------------------------------------------------------------------
   // Try to connect the CAN interface
   //
   if (pclPcanBasicP.isAvailable())
   {
      //-------------------------------------------------------------------------------------------
      // get channel condition
      //
      tsStatusT = pclPcanBasicP.pfnCAN_GetValueP(uwPCanChannelP, PCAN_CHANNEL_CONDITION, (void*)&ubValueT, 1);
      if (tsStatusT != PCAN_ERROR_OK)
      {
         qWarning() << QString("QCanInterfacePeak::connect(0x" + QString::number(uwPCanChannelP,16)+")") \
                    << "fail with error:" << pclPcanBasicP.formatedError(tsStatusT);
      }

      //-------------------------------------------------------------------------------------------
      // channel is not available, quit here
      //
      else if (ubValueT == PCAN_CHANNEL_UNAVAILABLE)
      {
         qDebug() << QString("QCanInterfacePeak::connect(0x" + QString::number(uwPCanChannelP,16)+")") \
                  << " is not available " << pclPcanBasicP.formatedError(tsStatusT);
         teReturnT = eERROR_CHANNEL;
      }

      //-------------------------------------------------------------------------------------------
      // channel is occupied, quit here
      //
      else if (ubValueT == 0)
      {
         qWarning() << QString("QCanInterfacePeak::connect(0x" + QString::number(uwPCanChannelP,16)+")") \
                    << " is occupied " << pclPcanBasicP.formatedError(tsStatusT);
         teReturnT = eERROR_USED;
      }

      //-------------------------------------------------------------------------------------------
      // channel is available, get other parameters
      //
      else
      {
         //-----------------------------------------------------------------------------------
         // release can channel before init it
         //
         if (teConnectedP == ConnectedState)
         {
            pclPcanBasicP.pfnCAN_UninitializeP(uwPCanChannelP);
            teConnectedP = UnconnectedState;
         }

         //-----------------------------------------------------------------------------------
         // initialise selected channel it again
         //
         tsStatusT = pclPcanBasicP.pfnCAN_InitializeP(uwPCanChannelP, uwPCanBitrateP, 0, 0, 0);

         if (tsStatusT != PCAN_ERROR_OK)
         {
            qCritical() << QString("QCanInterfacePeak::connect(0x" + QString::number(uwPCanChannelP,16)+")") \
                        << "fail with error:" << pclPcanBasicP.formatedError(tsStatusT);
            teReturnT = eERROR_USED;
         }

         else
         {
            teConnectedP = ConnectedState;
            emit connectionChanged(ConnectedState);
            teReturnT = eERROR_NONE;

            //---------------------------------------------------------------------------------------------------
            // configure the refresh timer which updates all statistic information and sends some signals
            //
            QTimer::singleShot(10, this, SLOT(onTimerEvent()));
         }
      }
   }

   return teReturnT;
}


//--------------------------------------------------------------------------------------------------------------------//
// connectionState()                                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::ConnectionState_e QCanInterfacePeak::connectionState(void)
{
   return teConnectedP;
}



//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfacePeak::disableFeatures()                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void  QCanInterfacePeak::disableFeatures(uint32_t ulFeatureMaskV)
{
   //------------------------------------------------------------------------------------
   // mask features supported by QCAN_IF_SUPPORT_xxx
   //
   ulFeatureMaskV = ulFeatureMaskV & QCAN_IF_SUPPORT_MASK;

   //------------------------------------------------------------------------------------
   // disable features
   //
   ulFeaturesP    = ulFeaturesP & (~ulFeatureMaskV);
}


//--------------------------------------------------------------------------------------------------------------------//
// disconnect()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfacePeak::disconnect()
{
   if (pclPcanBasicP.isAvailable())
   {
      TPCANStatus tsStatusT = pclPcanBasicP.pfnCAN_UninitializeP(uwPCanChannelP);
      if (tsStatusT == PCAN_ERROR_OK)
      {
         teConnectedP = UnconnectedState;
         emit connectionChanged(UnconnectedState);
         return eERROR_NONE;
      }
      qWarning() << QString("QCanInterfacePeak::disconnect(0x" + QString::number(uwPCanChannelP,16)+")") \
                 << "fail with error:" << pclPcanBasicP.formatedError(tsStatusT);
   }

   return eERROR_LIBRARY;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfacePeak::enableFeatures()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void  QCanInterfacePeak::enableFeatures(uint32_t ulFeatureMaskV)
{
   //------------------------------------------------------------------------------------
   // mask features supported by QCAN_IF_SUPPORT_xxx and the interface
   //
   ulFeatureMaskV = ulFeatureMaskV & QCAN_IF_SUPPORT_MASK;
   ulFeatureMaskV = ulFeatureMaskV & this->supportedFeatures();

   //------------------------------------------------------------------------------------
   // enable features
   //
   ulFeaturesP    = ulFeaturesP | ulFeatureMaskV;
}


//--------------------------------------------------------------------------------------------------------------------//
// icon()                                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QIcon QCanInterfacePeak::icon(void)
{
   return clIconP;
}


//--------------------------------------------------------------------------------------------------------------------//
// name()                                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanInterfacePeak::name()
{
   return clNameP;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfacePeak::onTimerEvent()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanInterfacePeak::onTimerEvent(void)
{
   if (read(clRcvFrameP) == eERROR_NONE)
   {
      btHasReceivedFrameP = true;
      emit readyRead();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// read()                                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfacePeak::read(QCanFrame &clFrameR)
{
   InterfaceError_e  clRetValueT;

   if (!pclPcanBasicP.isAvailable())
   {
      emit connectionChanged(QCanInterface::FailureState);
      clRetValueT = eERROR_LIBRARY;
   }
   else
   {
      #if QCAN_SUPPORT_CAN_FD > 0
      if (btHasReceivedFrameP)
      {
         btHasReceivedFrameP = false;
         clFrameR    = clRcvFrameP;
         clRetValueT = eERROR_NONE;
      }
      else
      {
         if (btFdUsedP == true)
         {
            clRetValueT = readFrameFD(clFrameR);
         }
         else
         {
            clRetValueT = readFrame(clFrameR);
         }
      }
      #else
      clRetValueT = readFrame(clDataR);
      #endif

   }

   if (clRetValueT == eERROR_FIFO_RCV_EMPTY)
   {
      QTimer::singleShot(50, this, SLOT(onTimerEvent()));
   }

   return (clRetValueT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfacePeak::readFrame()                                                                                     //
// Read classical CAN frame from Peak interface                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfacePeak::readFrame(QCanFrame &clFrameR)
{
   TPCANStatus       ulStatusT;
   uint8_t           ubCntT;
   TPCANMsg          tsCanMsgT;
   TPCANTimestamp    tsCanTimeStampT;
   uint32_t          ulMicroSecsT;
   QCanTimeStamp     clTimeStampT;
   InterfaceError_e  clRetValueT = eERROR_NONE;
   
   //---------------------------------------------------------------------------------------------------
   // get next message from FIFO
   //
   ulStatusT = pclPcanBasicP.pfnCAN_ReadP(uwPCanChannelP, &tsCanMsgT, &tsCanTimeStampT);


   //---------------------------------------------------------------------------------------------------
   // read message structure 
   //
   if (ulStatusT == PCAN_ERROR_OK)
   {
      //-------------------------------------------------------------------------------------------
      // handle data depending on type
      //
      if ((tsCanMsgT.MSGTYPE & PCAN_MESSAGE_STATUS) > 0)
      {

         //-----------------------------------------------------------------------------------
         // this is a status message, which is in fact a status of CAN error state
         //
         switch (tsCanMsgT.DATA[3])
         {
            case 0x02:

               break;

            case 0x04:
               emit stateChanged(eCAN_STATE_BUS_WARN);
               break;

            case 0x08:
               emit stateChanged(eCAN_STATE_BUS_PASSIVE);
               break;

            case 0x10:
               emit stateChanged(eCAN_STATE_BUS_OFF);
               break;

            default:

               break;


         }
         emit addLogMessage("PCAN message - PCAN_MESSAGE_STATUS   : status byte = " +
                            QString("0x%1").arg(tsCanMsgT.DATA[3], 2, 16, QLatin1Char('0')), eLOG_LEVEL_TRACE);
         clRetValueT = eERROR_FIFO_RCV_EMPTY;
         return (clRetValueT);
      }

      //--------------------------------------------------------------------------------------
      // this is a (undocumented) error frame message
      //
      if ((tsCanMsgT.MSGTYPE & PCAN_MESSAGE_ERRFRAME) > 0)
      {
         emit addLogMessage("PCAN message - PCAN_MESSAGE_ERRFRAME ", eLOG_LEVEL_TRACE);
         clRetValueT = eERROR_FIFO_RCV_EMPTY;
         return (clRetValueT);
      }

      //--------------------------------------------------------------------------------------
      // now handle all data frames
      //--------------------------------------------------------------------------------------

      //--------------------------------------------------------------------------------------
      // Make sure that hte frame type is Test
      //
      if (clFrameR.frameType() == QCanFrame::eFRAME_TYPE_ERROR)
      {
         clFrameR.setFrameType(QCanFrame::eFRAME_TYPE_DATA);
      }

      //--------------------------------------------------------------------------------------
      // Classical CAN frame with standard or extended identifier
      //
      if (tsCanMsgT.MSGTYPE & PCAN_MESSAGE_EXTENDED)
      {
         clFrameR.setFrameFormat(QCanFrame::eFORMAT_CAN_EXT);
      }
      else
      {
         clFrameR.setFrameFormat(QCanFrame::eFORMAT_CAN_STD);
      }
            
      //--------------------------------------------------------------------------------------
      // Classical CAN remote frame
      //
      if (tsCanMsgT.MSGTYPE & PCAN_MESSAGE_RTR)
      {
         clFrameR.setRemote(true);
      }
         
      //--------------------------------------------------------------------------------------
      // copy the identifier
      //
      clFrameR.setIdentifier(tsCanMsgT.ID);

      //--------------------------------------------------------------------------------------
      // copy the DLC
      //
      clFrameR.setDlc(tsCanMsgT.LEN);

      //--------------------------------------------------------------------------------------
      // copy the data
      //
      for (ubCntT = 0; ubCntT < clFrameR.dataSize(); ubCntT++)
      {
         clFrameR.setData(ubCntT, tsCanMsgT.DATA[ubCntT]);
      }

      //--------------------------------------------------------------------------------------
      // copy the time-stamp
      // The value is a multiple of 1 us and has a total time span of 4294,9 secs
      //
      ulMicroSecsT = tsCanTimeStampT.millis * 1000;
      ulMicroSecsT = ulMicroSecsT + tsCanTimeStampT.micros;
      clTimeStampT.fromMicroSeconds(ulMicroSecsT);
         
      clFrameR.setTimeStamp(clTimeStampT);
         
      //--------------------------------------------------------------------------------------
      // increase statistic counter
      //
      clStatisticP.ulRcvCount++;
         
   }

   //----------------------------------------------------------------------------------------------
   // test for bus error
   //
   else if ((ulStatusT & (TPCANStatus)PCAN_ERROR_ANYBUSERR) > 0)
   {
      
      setupErrorFrame(ulStatusT);
      emit addLogMessage("PCAN status  - PCAN_ERROR_ANYBUSERR  ", eLOG_LEVEL_TRACE);
      clRetValueT = eERROR_FIFO_RCV_EMPTY;
   }

   //----------------------------------------------------------------------------------------------
   // the receive queue is empty
   //
   else if (ulStatusT == PCAN_ERROR_QRCVEMPTY)
   {
      clRetValueT = eERROR_FIFO_RCV_EMPTY;
   }

   //----------------------------------------------------------------------------------------------
   // some other error, e.g. hardware failed
   //
   else
   {
      emit addLogMessage("Hardware error code " + QString("0x%1").arg(ulStatusT, 16), eLOG_LEVEL_DEBUG);
      teErrorStateP = eCAN_STATE_STOPPED;
      emit stateChanged(teErrorStateP);
      disconnect();
      clRetValueT = eERROR_DEVICE;
   }

   return (clRetValueT);
}


//--------------------------------------------------------------------------------------------------------------------//
// readFD()                                                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
#if QCAN_SUPPORT_CAN_FD > 0
QCanInterface::InterfaceError_e  QCanInterfacePeak::readFrameFD(QCanFrame &clFrameR)
{
   TPCANStatus       ulStatusT;
   uint8_t           ubCntT;
   TPCANMsgFD        tsCanMsgT;
   TPCANTimestampFD  tsCanTimeStampT;
   QCanTimeStamp     clTimeStampT;
   InterfaceError_e  clRetValueT = eERROR_NONE;

   //---------------------------------------------------------------------------------------------------
   // get next message from FIFO
   //
   ulStatusT = pclPcanBasicP.pfnCAN_ReadFDP(uwPCanChannelP, &tsCanMsgT, &tsCanTimeStampT);


   //---------------------------------------------------------------------------------------------------
   // read message structure
   //
   if (ulStatusT == PCAN_ERROR_OK)
   {
      //-------------------------------------------------------------------------------------------
      // handle data depending on type
      //
      if((tsCanMsgT.MSGTYPE & PCAN_MESSAGE_STATUS) > 0)
      {
         //-----------------------------------------------------------------------------------
         // this is a status message
         //
         clRetValueT = eERROR_FIFO_RCV_EMPTY;
      }
      else
      {
         //------------------------------------------------
         // this is a CAN message
         //
         if (tsCanMsgT.MSGTYPE & PCAN_MESSAGE_FD)
         {
            //----------------------------------------
            // ISO CAN FD frame with standard or
            // extended identifier
            //
            if (tsCanMsgT.MSGTYPE & PCAN_MESSAGE_EXTENDED)
            {
               clFrameR.setFrameFormat(QCanFrame::eFORMAT_FD_EXT);
            }
            else
            {
               clFrameR.setFrameFormat(QCanFrame::eFORMAT_FD_STD);
            }

            //----------------------------------------
            // Test for BRS bit
            //
            if (tsCanMsgT.MSGTYPE & PCAN_MESSAGE_BRS)
            {
               clFrameR.setBitrateSwitch();
            }

            //----------------------------------------
            // Test for ESI bit
            //
            if (tsCanMsgT.MSGTYPE & PCAN_MESSAGE_ESI)
            {
               clFrameR.setErrorStateIndicator();
            }
         }
         else
         {
            //----------------------------------------
            // Classical CAN frame with standard or
            // extended identifier
            //
            if (tsCanMsgT.MSGTYPE & PCAN_MESSAGE_EXTENDED)
            {
               clFrameR.setFrameFormat(QCanFrame::eFORMAT_CAN_EXT);
            }
            else
            {
               clFrameR.setFrameFormat(QCanFrame::eFORMAT_CAN_STD);
            }

            //----------------------------------------
            // Classical CAN remote frame
            //
            if (tsCanMsgT.MSGTYPE & PCAN_MESSAGE_RTR)
            {
               clFrameR.setRemote();
            }
         }

         //------------------------------------------------
         // copy the identifier
         //
         clFrameR.setIdentifier(tsCanMsgT.ID);

         //------------------------------------------------
         // copy the DLC
         //
         clFrameR.setDlc(tsCanMsgT.DLC);

         //------------------------------------------------
         // copy the data
         //
         for (ubCntT = 0; ubCntT < clFrameR.dataSize(); ubCntT++)
         {
            clFrameR.setData(ubCntT, tsCanMsgT.DATA[ubCntT]);
         }

         //------------------------------------------------
         // copy the time-stamp
         // the value is a multiple of 1 us and has a
         // total time span of 4294,9 secs
         //
         clTimeStampT.fromMicroSeconds(tsCanTimeStampT);

         clFrameR.setTimeStamp(clTimeStampT);

         //------------------------------------------------
         // increase statistic counter
         //
         clStatisticP.ulRcvCount++;

      }
   }
   else
   {
      //--------------------------------------------------------
      // test for bus error
      //
      if ((ulStatusT & (TPCANStatus)PCAN_ERROR_ANYBUSERR) > 0)
      {
         setupErrorFrame(ulStatusT);
         //------------------------------------------------
         // copy the error frame to a byte array
         //
         //clDataR = clErrFrameT.toByteArray();
         //qDebug() << "Error frame";
      }

      //--------------------------------------------------------
      // the receive queue is empty
      //
      else if (ulStatusT == PCAN_ERROR_QRCVEMPTY)
      {
         clRetValueT = eERROR_FIFO_RCV_EMPTY;
      }
      else
      {
         clRetValueT = eERROR_DEVICE;
      }
   }

   return (clRetValueT);
}
#endif


//--------------------------------------------------------------------------------------------------------------------//
// reset()                                                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfacePeak::reset()
{
   InterfaceError_e  clRetValueT = eERROR_NONE;

   //---------------------------------------------------------------------------------------------------
   // check if driver is available
   //
   if (!pclPcanBasicP.isAvailable())
   {
      clRetValueT = eERROR_LIBRARY;
   }
   else
   {
      //-------------------------------------------------------------------------------------------
      // reset statistic values
      //
      clStatisticP.ulErrCount = 0;
      clStatisticP.ulRcvCount = 0;
      clStatisticP.ulTrmCount = 0;

      //-------------------------------------------------------------------------------------------
      // reset device
      //
      if (pclPcanBasicP.pfnCAN_ResetP(uwPCanChannelP) != PCAN_ERROR_OK)
      {
         clRetValueT = eERROR_DEVICE;
      }
      else
      {
         //-----------------------------------------------------------------------------------
         // perform a hardware reset only if it has been initialised before
         //
         if (teConnectedP == ConnectedState)
         {
            pclPcanBasicP.pfnCAN_UninitializeP(uwPCanChannelP);

            if (pclPcanBasicP.pfnCAN_InitializeP(uwPCanChannelP, uwPCanBitrateP, 0, 0, 0) != PCAN_ERROR_OK)
            {
               clRetValueT = eERROR_DEVICE;
            }
            else
            {
               clRetValueT = setMode(teCanModeP);
            }
         }
      }
   }


   return (clRetValueT);

}


//--------------------------------------------------------------------------------------------------------------------//
// setBitrate()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfacePeak::setBitrate( int32_t slNomBitRateV, int32_t slDatBitRateV)
{
   if (!pclPcanBasicP.isAvailable())
   {
      return eERROR_LIBRARY;
   }

   WORD uwBtr0Btr1T = 0;
   TPCANStatus ulStatusT;
   #if QCAN_SUPPORT_CAN_FD > 0
   QString clTxtBitrateNomT;
   QString clTxtBitrateDataT;
   QByteArray clTxtBitrateT ;
   #endif
   //----------------------------------------------------------------
   // Check bit-rate value
   //
   if (slNomBitRateV >= eCAN_BITRATE_AUTO)
   {
      // maybe we get a value in Hz, normalise it to kHz
      slNomBitRateV = slNomBitRateV / 1000;
   }

   #if QCAN_SUPPORT_CAN_FD > 0
   //----------------------------------------------------------------
   // Check data bit rate  value for CAN-FD
   //
   if (slDatBitRateV != eCAN_BITRATE_NONE)
   {
      //--------------------------------------------------------
      // FD Bit-rate: all settings for 80 MHz clock
      //
      QString clDebugMsgT = QString("PCAN - setBitrate(%1 , %2)").arg(slNomBitRateV).arg(slDatBitRateV);
      emit addLogMessage(clDebugMsgT, eLOG_LEVEL_DEBUG);
      //--------------------------------------------------------
      // select corresponding PEAK configuration for
      // nominal bit rate
      //
      switch(slNomBitRateV )
      {
         case 125:
            clTxtBitrateNomT = "f_clock_mhz=80, nom_brp=2, nom_tseg1=255, nom_tseg2=64, nom_sjw=64";
            break;

         case 250:
            clTxtBitrateNomT = "f_clock_mhz=80, nom_brp=2, nom_tseg1=127, nom_tseg2=32, nom_sjw=32";
            break;

         case 500:
            clTxtBitrateNomT = "f_clock_mhz=80, nom_brp=2, nom_tseg1=63, nom_tseg2=16, nom_sjw=16";
            break;

         case 1000:
            clTxtBitrateNomT = "f_clock_mhz=80, nom_brp=2, nom_tseg1=31, nom_tseg2=8, nom_sjw=8";
            break;

         default:
            return eERROR_BITRATE;
            break;
      }

      //--------------------------------------------------------
      // select corresponding PEAK configuration for
      // data bit rate
      //
      slDatBitRateV = slDatBitRateV / 1000000;
      switch(slDatBitRateV)
      {
         case 1:     // 1 MBit/s
            clTxtBitrateDataT = ", data_brp=2, data_tseg1=31, data_tseg2=8, data_sjw=8";
            break;

         case 2:     // 2 MBit/s
            clTxtBitrateDataT = ", data_brp=2, data_tseg1=15, data_tseg2=4, data_sjw=4";
            break;

         case 4:     // 4 MBit/s
            clTxtBitrateDataT = ", data_brp=2, data_tseg1=7, data_tseg2=2, data_sjw=2";
            break;

         default:
            return eERROR_BITRATE;
            break;
      }

      clTxtBitrateNomT.append(clTxtBitrateDataT);
      clTxtBitrateT = clTxtBitrateNomT.toUtf8();
      emit addLogMessage(clTxtBitrateNomT, eLOG_LEVEL_DEBUG);

   }
   else
   #endif
   {

      //----------------------------------------------------------------
      // select corresponding PEAK baud rate value
      //
      switch(slNomBitRateV)
      {
         // value from CANpie enumeration
         case eCAN_BITRATE_10K:
            uwBtr0Btr1T = PCAN_BAUD_10K;
            break;
         case eCAN_BITRATE_20K:
            uwBtr0Btr1T = PCAN_BAUD_20K;
            break;
         case eCAN_BITRATE_50K:
            uwBtr0Btr1T = PCAN_BAUD_50K;
            break;
         case eCAN_BITRATE_100K:
            uwBtr0Btr1T = PCAN_BAUD_100K;
            break;
         case eCAN_BITRATE_125K:
            uwBtr0Btr1T = PCAN_BAUD_125K;
            break;
         case eCAN_BITRATE_250K:
            uwBtr0Btr1T = PCAN_BAUD_250K;
            break;
         case eCAN_BITRATE_500K:
            uwBtr0Btr1T = PCAN_BAUD_500K;
            break;
         case eCAN_BITRATE_800K:
            uwBtr0Btr1T = PCAN_BAUD_800K;
            break;
         case eCAN_BITRATE_1M:
            uwBtr0Btr1T = PCAN_BAUD_1M;
            break;

         // value normalized to kHz
         case 10:
            uwBtr0Btr1T = PCAN_BAUD_10K;
            break;
         case 20:
            uwBtr0Btr1T = PCAN_BAUD_20K;
            break;
         case 50:
            uwBtr0Btr1T = PCAN_BAUD_50K;
            break;
         case 100:
            uwBtr0Btr1T = PCAN_BAUD_100K;
            break;
         case 125:
            uwBtr0Btr1T = PCAN_BAUD_125K;
            break;
         case 250:
            uwBtr0Btr1T = PCAN_BAUD_250K;
            break;
         case 500:
            uwBtr0Btr1T = PCAN_BAUD_500K;
            break;
         case 800:
            uwBtr0Btr1T = PCAN_BAUD_800K;
            break;
         case 1000:
            uwBtr0Btr1T = PCAN_BAUD_1M;
            break;

         default:
            return eERROR_BITRATE;
            break;
      }
   }

   //----------------------------------------------------------------
   // perform releasing of CAN Interface
   //
   pclPcanBasicP.pfnCAN_UninitializeP(uwPCanChannelP);

   if (slDatBitRateV != eCAN_BITRATE_NONE)
   {

      #if QCAN_SUPPORT_CAN_FD > 0
      ulStatusT = pclPcanBasicP.pfnCAN_InitializeFDP(uwPCanChannelP, clTxtBitrateT.data());
      btFdUsedP = true;

      #else
      ulStatusT = PCAN_ERROR_ILLPARAMVAL;
      #endif
   }
   else
   {
      uwPCanBitrateP = uwBtr0Btr1T;
      ulStatusT = pclPcanBasicP.pfnCAN_InitializeP(uwPCanChannelP, uwPCanBitrateP, 0, 0, 0);
      btFdUsedP = false;
   }

   if (ulStatusT != PCAN_ERROR_OK)
   {
      emit addLogMessage("Failed to configure bit-rate setting: " + pclPcanBasicP.formatedError(ulStatusT),
                         eLOG_LEVEL_ERROR);
      qCritical() << pclPcanBasicP.formatedError((TPCANStatus) ulStatusT);

      return eERROR_DEVICE;
   }

   return eERROR_NONE;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfacePeak::setMode()                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//

QCanInterface::InterfaceError_e	QCanInterfacePeak::setMode(const CAN_Mode_e teModeV)
{
   uint8_t ubValueBufT;

   //---------------------------------------------------------------------------------------------------
   // Check if the library is available
   //
   if (!pclPcanBasicP.isAvailable())
   {
      return eERROR_LIBRARY;
   }

   //---------------------------------------------------------------------------------------------------
   // select mode
   //
   switch (teModeV)
   {
      //-------------------------------------------------------------------------------------------
      // set interface into operation mode
      //
      case eCAN_MODE_OPERATION :

         //-----------------------------------------------------------------------------------
         // reset statistic values
         //
         clStatisticP.ulErrCount = 0;
         clStatisticP.ulRcvCount = 0;
         clStatisticP.ulTrmCount = 0;

         ubValueBufT = 0;
         if (pclPcanBasicP.pfnCAN_SetValueP(uwPCanChannelP, PCAN_LISTEN_ONLY,
                                            &ubValueBufT, sizeof(ubValueBufT) )  )
         {
            emit addLogMessage("Failed to clear 'Listen-only'", eLOG_LEVEL_WARN);
         }

         teCanModeP = eCAN_MODE_START;
         break;

      //-------------------------------------------------------------------------------------------
      // set interface into init mode
      //
      case eCAN_MODE_INIT :
         teCanModeP = eCAN_MODE_INIT;
         break;

      //-------------------------------------------------------------------------------------------
      // set interface into listen-only mode
      //
      case eCAN_MODE_LISTEN_ONLY :
         ubValueBufT = 1;
         if (pclPcanBasicP.pfnCAN_SetValueP(uwPCanChannelP, PCAN_LISTEN_ONLY,
                                            &ubValueBufT, sizeof(ubValueBufT) )  )
         {
            emit addLogMessage("Failed to set 'Listen-only'", eLOG_LEVEL_WARN);
         }
         break;

      default :
         return eERROR_MODE;
         break;
   }

   return eERROR_NONE;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfacePeak::setupErrorFrame()                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanInterfacePeak::setupErrorFrame(TPCANStatus ulStatusV)
{

   //---------------------------------------------------------------------------------------------------
   // set frame type and the bus status
   //
   clErrFrameP.setErrorType(QCanFrame::eERROR_TYPE_NONE);

   switch (ulStatusV)
   {
      case PCAN_ERROR_BUSLIGHT :
         clErrFrameP.setErrorState(eCAN_STATE_BUS_WARN);
         break;

      case PCAN_ERROR_BUSWARNING :
         clErrFrameP.setErrorState(eCAN_STATE_BUS_WARN);
         break;

      case PCAN_ERROR_BUSPASSIVE :
         clErrFrameP.setErrorState(eCAN_STATE_BUS_PASSIVE);
         break;

      case PCAN_ERROR_BUSOFF :
         clErrFrameP.setErrorState(eCAN_STATE_BUS_OFF);
         break;

      default :
         clErrFrameP.setErrorState(eCAN_STATE_BUS_ACTIVE);
         break;
   }

   //---------------------------------------------------------------------------------------------------
   // Store the error state if it has changed
   //
   if (teErrorStateP != clErrFrameP.errorState())
   {
      teErrorStateP = clErrFrameP.errorState();
      emit stateChanged(teErrorStateP);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// statistic()                                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e	QCanInterfacePeak::statistic(QCanStatistic_ts &clStatisticR)
{
   //! \todo

   clStatisticR.ulErrCount = 0;

   return(eERROR_NONE);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfacePeak::state()                                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CAN_State_e QCanInterfacePeak::state(void)
{
   return (teErrorStateP);
}


//----------------------------------------------------------------------------//
// supportedFeatures()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
uint32_t QCanInterfacePeak::supportedFeatures()
{
   uint32_t ulFeaturesT = 0;
   #if QCAN_SUPPORT_CAN_FD > 0
   uint32_t ulBufferT = 0;
   #endif
   if(pclPcanBasicP.isAvailable())
   {

      ulFeaturesT = QCAN_IF_SUPPORT_LISTEN_ONLY;

      #if QCAN_SUPPORT_CAN_FD > 0
      TPCANStatus tsStatusT = pclPcanBasicP.pfnCAN_GetValueP(uwPCanChannelP, PCAN_CHANNEL_FEATURES, (void*)&ulBufferT, sizeof(ulBufferT));
      if (tsStatusT != PCAN_ERROR_OK)
      {
         qWarning() << QString("QCanInterfacePeak::supportedFeatures(0x" +QString::number(uwPCanChannelP,16)+")") << "PCAN_CHANNEL_FEATURES fail with error:" << pclPcanBasicP.formatedError(tsStatusT);
      }
      qWarning() << QString("QCanInterfacePeak::supportedFeatures(0x" +QString::number(uwPCanChannelP,16)+")") << "PCAN_CHANNEL_FEATURES:" << QString::number(ulBufferT,16) << "[hex]";

      if (ulBufferT & FEATURE_FD_CAPABLE)
      {
         ulFeaturesT += QCAN_IF_SUPPORT_CAN_FD;
      }
      #endif
   }

   return(ulFeaturesT);
}


//--------------------------------------------------------------------------------------------------------------------//
// version()                                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanInterfacePeak::version(void)
{
   QString clVersionT;

   clVersionT  = QString("%1.%2.").arg(VERSION_MAJOR).arg(VERSION_MINOR, 2, 10, QLatin1Char('0'));
   clVersionT += QString("%1").arg(VERSION_BUILD, 2, 10, QLatin1Char('0'));

   return (clVersionT);
}


//--------------------------------------------------------------------------------------------------------------------//
// write()                                                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e	QCanInterfacePeak::write(const QCanFrame &clFrameR)
{
   TPCANStatus ulStatusT;
   TPCANMsg    tsCanMsgT;
   #if QCAN_SUPPORT_CAN_FD > 0
   TPCANMsgFD  tsCanMsgFdT;
   #endif
   int32_t     slByteCntrT;

   if (!pclPcanBasicP.isAvailable())
   {
      return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // prepare CAN message
   //
   if (clFrameR.frameFormat() < QCanFrame::eFORMAT_FD_STD)
   {
      // copy all needed parameters to QCanFrame structure
      if (clFrameR.isExtended())
      {
         tsCanMsgT.MSGTYPE = PCAN_MESSAGE_EXTENDED;
      }
      else
      {
         tsCanMsgT.MSGTYPE = PCAN_MESSAGE_STANDARD;
      }

      tsCanMsgT.ID = clFrameR.identifier();

      tsCanMsgT.LEN= clFrameR.dlc();

      for (slByteCntrT = 0; slByteCntrT < clFrameR.dlc(); slByteCntrT++)
      {
         tsCanMsgT.DATA[slByteCntrT] = clFrameR.data(slByteCntrT);
      }

      ulStatusT = pclPcanBasicP.pfnCAN_WriteP(uwPCanChannelP, &tsCanMsgT);

   }
   else
   {
      #if QCAN_SUPPORT_CAN_FD > 0

      if (clFrameR.isExtended())
      {
         tsCanMsgFdT.MSGTYPE = PCAN_MESSAGE_EXTENDED;
      } else
      {
         tsCanMsgFdT.MSGTYPE  = PCAN_MESSAGE_STANDARD;
         tsCanMsgFdT.MSGTYPE |= PCAN_MESSAGE_FD;
         tsCanMsgFdT.MSGTYPE |= PCAN_MESSAGE_BRS;
      }

      tsCanMsgFdT.ID = clFrameR.identifier();

      tsCanMsgFdT.DLC= clFrameR.dlc();

      for (slByteCntrT = 0; slByteCntrT < clFrameR.dataSize(); slByteCntrT++)
      {
         tsCanMsgFdT.DATA[slByteCntrT] = clFrameR.data(slByteCntrT);
      }

      ulStatusT = pclPcanBasicP.pfnCAN_WriteFDP(uwPCanChannelP, &tsCanMsgFdT);
      #else
      ulStatusT = 0;
      #endif
   }


   if (ulStatusT == PCAN_ERROR_OK)
   {
      clStatisticP.ulTrmCount++;
      return eERROR_NONE;
   }
   else if (ulStatusT != (TPCANStatus)PCAN_ERROR_QRCVEMPTY)
   {
      qWarning() << "Fail to call CAN_Write(): " << QString::number(ulStatusT,16);
      return eERROR_DEVICE;
   }

   return eERROR_FIFO_TRM_FULL;

}

