//====================================================================================================================//
// File:          qcan_interface_vci4.cpp                                                                             //
// Description:   QCan Interface of IXXAT VIC4 library                                                                //
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
#include <qcan_interface_vci4.hpp>


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*\
** Class methods                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::QCanInterfaceVci()                                                                               //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterfaceVci::QCanInterfaceVci(VCIDEVICEINFO clDevInfoV)
{
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanInterfaceVci::QCanInterfaceVci(VCIDEVICEINFO clDevInfoV)";
   #endif

   //---------------------------------------------------------------------------------------------------
   // Store device info
   //
   clDevInfoP     = clDevInfoV;

   //---------------------------------------------------------------------------------------------------
   // Set class members to default
   //
   ulBusCtlNumP   = 0;
   ulSocketNumP   = 0;

   ptsBalObjectP  = nullptr;
   ptsCanControlP = nullptr;
   ptsCanChannelP = nullptr;

   ptsRcvFifoP    = nullptr;
   ptsTrmFifoP    = nullptr;

   teConnectedP   = UnconnectedState;
   btFdUsedP      = false;
   teCanModeP     = QCan::eCAN_MODE_STOP;

   ulFeaturesActiveP    = 0;

   //---------------------------------------------------------------------------------------------------
   // Create event for disconnect
   //
   ulDeviceEventP = CreateEvent(NULL, FALSE, FALSE, NULL);

   //---------------------------------------------------------------------------------------------------
   // clear local error frame contents
   //
   clErrFrameP.setFrameType(QCanFrame::eFRAME_TYPE_ERROR);
   clErrFrameP.setErrorCounterReceive(0);
   clErrFrameP.setErrorCounterTransmit(0);
   clErrFrameP.setErrorState(QCan::eCAN_STATE_BUS_ACTIVE);


   //---------------------------------------------------------------------------------------------------
   // Default configuration for CAN interface setup
   //
   tsLineInitP =  {
      CAN_OPMODE_STANDARD | CAN_OPMODE_EXTENDED,      // opmode
      0,                                              // exmode
      CAN_FILTER_INCL,                                // mode for line specific 11-bit ID filter
      CAN_FILTER_INCL,                                // mode for line specific 29-bit ID filter
      0,                                              // size of line specific 11-bit ID filter
      0,                                              // size of line specific 29-bit ID filter
      {0, 0, 0, 0, 0, 0},                             // arbitration bitrate
      {0, 0, 0, 0, 0, 0}                              // data bitrate
   };

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::~QCanInterfaceVci()                                                                              //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterfaceVci::~QCanInterfaceVci()
{
   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanInterfaceVci::~QCanInterfaceVci()";
   #endif

//   if (clVciP.isAvailable())
//   {
      if (teConnectedP == ConnectedState)
      {
//         if (vdCanInterfaceP != nullptr)
//         {
//            clVciP.pfnVciDeviceCloseP(vdCanInterfaceP);
//         }
      }
//   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::connect()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceVci::connect()
{
   InterfaceError_e     teReturnT = eERROR_DEVICE;
   HRESULT              teResultT;                 // error code of VCI functions

   IVciDeviceManager *  pclDevMgrT  = nullptr;      // device manager
   IVciEnumDevice *     pclDevEnumT = nullptr;    // enumerator handle
   IVciDevice *         pclDeviceT  = nullptr;      // device

   teResultT = VciGetDeviceManager(&pclDevMgrT);
   if (teResultT == VCI_OK)
   {
      //-------------------------------------------------------------------------------------------
      // Install handler for device removal
      //
      pclDevMgrT->EnumDevices(&pclDevEnumT);
      pclDevEnumT->AssignEvent(ulDeviceEventP);
      
      teResultT = pclDevMgrT->OpenDevice(clDevInfoP.VciObjectId, &pclDeviceT);

      //-------------------------------------------------------------------------------------------
      // Get the BAL object
      //
      if (teResultT == VCI_OK)
      {
         teResultT = pclDeviceT->OpenComponent(CLSID_VCIBAL, IID_IBalObject, (void**)&ptsBalObjectP);
         pclDeviceT->Release();
         teReturnT = eERROR_NONE;

         //-----------------------------------------------------------------------------------
         // Check if controller supports CAN 
         //
         BALFEATURES tsBalFeaturesT = { 0 };
         teResultT = ptsBalObjectP->GetFeatures(&tsBalFeaturesT);
         if (teResultT == VCI_OK)
         {
            //---------------------------------------------------------------------------
            // check if controller number is valid
            //
            if (ulBusCtlNumP >= tsBalFeaturesT.BusSocketCount)
            {
               teReturnT = eERROR_LIBRARY;
            }

            //---------------------------------------------------------------------------
            // check for the expected controller type
            //
            if (VCI_BUS_TYPE(tsBalFeaturesT.BusSocketType[ulBusCtlNumP]) != VCI_BUS_CAN)
            {
               teReturnT = eERROR_LIBRARY;
            }
         }

         if (teReturnT == eERROR_NONE)
         {
            if (socketInit() == true)
            {
               //-------------------------------------------------------------------
               // update the pending state
               //
               teConnectedP = ConnectedState;
               emit connectionChanged(QCanInterface::ConnectedState);

               //-------------------------------------------------------------------
               // Configure the refresh timer which updates all statistic 
               // information and sends some signals
               //
               QTimer::singleShot(200, this, SLOT(onTimerEvent()));
               teReturnT = eERROR_NONE;
            }
         }
      }
   }

   if (teReturnT == eERROR_NONE)
   {
      emit addLogMessage("QCanInterfaceVci::connect()                  : success", QCan::eLOG_LEVEL_TRACE);
   }
   else
   {
      emit addLogMessage("QCanInterfaceVci::connect()                  : failed", QCan::eLOG_LEVEL_TRACE);
   }

   return teReturnT;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::connectionState()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::ConnectionState_e QCanInterfaceVci::connectionState()
{
   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanInterfaceVci::connectionState()";
   #endif

   return teConnectedP;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::disconnect()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceVci::disconnect()
{
   //---------------------------------------------------------------------------------------------------
   // debug information
   //
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanInterfaceVci::disconnect()";
   #endif

   InterfaceError_e teStatusT = eERROR_NONE;

   //---------------------------------------------------------------------------------------------------
   // perform release of socket
   //
   socketRelease();

   teConnectedP = UnconnectedState;
   emit connectionChanged(QCanInterface::UnconnectedState);

   return teStatusT;
}



//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::disableFeatures()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void  QCanInterfaceVci::disableFeatures(uint32_t ulFeatureMaskV)
{
   //------------------------------------------------------------------------------------
   // mask features supported by QCAN_IF_SUPPORT_xxx
   //
   ulFeatureMaskV = ulFeatureMaskV & this->supportedFeatures();

   //------------------------------------------------------------------------------------
   // disable features
   //
   ulFeaturesActiveP = ulFeaturesActiveP & (~ulFeatureMaskV);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::enableFeatures()                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void  QCanInterfaceVci::enableFeatures(uint32_t ulFeatureMaskV)
{
   //------------------------------------------------------------------------------------
   // mask features supported by QCAN_IF_SUPPORT_xxx and the interface
   //
   ulFeatureMaskV = ulFeatureMaskV & this->supportedFeatures();

   //------------------------------------------------------------------------------------
   // enable features
   //
   ulFeaturesActiveP = ulFeaturesActiveP | ulFeatureMaskV;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::icon()                                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QIcon QCanInterfaceVci::icon()
{
   //---------------------------------------------------------------------------------------------------
   // check lib have been loaded
   //
   return QIcon(":/images/mc_can_hms_usb_canfd.png");
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::name()                                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanInterfaceVci::name()
{
   QString clReturnT = "IXXAT VCI 4 library is not available";

   clReturnT = QString(clDevInfoP.Description) + " " +clDevInfoP.UniqueHardwareId.AsChar;

   return clReturnT;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::onTimerEvent()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanInterfaceVci::onTimerEvent(void)
{
   uint16_t    uwMsgCountT = 0;
   HRESULT     teResultT;                 // error code of VCI functions

   //---------------------------------------------------------------------------------------------------
   // check a new CAN message is pending
   //
   if (ptsRcvFifoP != nullptr)
   {
      teResultT = ptsRcvFifoP->GetFillCount( &uwMsgCountT );

      if ((teResultT == VCI_OK) && (uwMsgCountT > 0))
      {
         emit readyRead();
      }
      else
      {
         QTimer::singleShot(50, this, SLOT(onTimerEvent()));
      }
   }

   if (WAIT_OBJECT_0 == WaitForSingleObject(ulDeviceEventP, 0))
   {
      emit addLogMessage("QCanInterfaceVci::onTimerEvent()             : removed USB interface", QCan::eLOG_LEVEL_TRACE);
      ResetEvent(ulDeviceEventP);
      emit connectionChanged(QCanInterface::FailureState);

   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci:read()                                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfaceVci::read(QCanFrame &clFrameR)
{

   CANMSG2           tsVciCanMsgT;
   HRESULT           teResultT;                       // error code of VCI functions
   InterfaceError_e  clRetValueT = eERROR_FIFO_RCV_EMPTY;
   uint8_t           ubCountT;

   //---------------------------------------------------------------------------------------------------
   // check a new CAN message is pending
   //
   if (ptsRcvFifoP != nullptr)
   {

      teResultT = ptsRcvFifoP->GetDataEntry(&tsVciCanMsgT);
      if (teResultT == VCI_OK)
      {
         switch (tsVciCanMsgT.uMsgInfo.Bits.type)
         {
            //---------------------------------------------------------------------------
            // Handle CAN data frames
            //---------------------------------------------------------------------------
            case CAN_MSGTYPE_DATA:
         
               //-------------------------------------------------------------------
               // Make sure that the frame type is a data frame
               //
               clFrameR.setFrameType(QCanFrame::eFRAME_TYPE_DATA);

               //-------------------------------------------------------------------
               // Classical CAN frame or CAN FD frame
               //
               if (tsVciCanMsgT.uMsgInfo.Bits.edl == 0)
               {
                  //-----------------------------------------------------------
                  // Classical CAN frame: check for standard or extended
                  // format
                  //
                  if (tsVciCanMsgT.uMsgInfo.Bits.ext == 0)
                  {
                     clFrameR.setFrameFormat(QCanFrame::eFORMAT_CAN_STD);
                  }
                  else
                  {
                     clFrameR.setFrameFormat(QCanFrame::eFORMAT_CAN_EXT);
                  }
               }
               else
               {
                  //-----------------------------------------------------------
                  // CAN FD frame: check for standard or extended format
                  //
                  if (tsVciCanMsgT.uMsgInfo.Bits.ext == 0)
                  {
                     clFrameR.setFrameFormat(QCanFrame::eFORMAT_FD_STD);
                  }
                  else
                  {
                     clFrameR.setFrameFormat(QCanFrame::eFORMAT_FD_EXT);
                  }

                  //-----------------------------------------------------------
                  // CAN FD frame: check for ESI bit and FDR bit
                  //
                  if (tsVciCanMsgT.uMsgInfo.Bits.fdr != 0)
                  {
                     clFrameR.setBitrateSwitch();
                  }
                  if (tsVciCanMsgT.uMsgInfo.Bits.esi != 0)
                  {
                     clFrameR.setErrorStateIndicator();
                  }

               }

               //-------------------------------------------------------------------
               // Classical CAN remote frame
               //
               if (tsVciCanMsgT.uMsgInfo.Bits.rtr != 0)
               {
                  clFrameR.setRemote(true);
               }

               //-------------------------------------------------------------------
               // copy the identifier
               //
               clFrameR.setIdentifier(tsVciCanMsgT.dwMsgId);

               //-------------------------------------------------------------------
               // copy the DLC
               //
               clFrameR.setDlc(tsVciCanMsgT.uMsgInfo.Bits.dlc);


               //-------------------------------------------------------------------
               // copy the data
               //
               for (ubCountT = 0; ubCountT < clFrameR.dataSize(); ubCountT++)
               {
                  clFrameR.setData(ubCountT, tsVciCanMsgT.abData[ubCountT]);
               }

               //-------------------------------------------------------------------
               // increase statistic counter
               //
               clStatisticP.ulRcvCount++;

               clRetValueT = eERROR_NONE;
               break;

            case CAN_MSGTYPE_INFO:
               clLogMsgP = QString("QCanInterfaceVci::read() - CAN_MSGTYPE_INFO  : %1").arg(tsVciCanMsgT.abData[0]);
               emit addLogMessage(clLogMsgP, QCan::eLOG_LEVEL_TRACE);
               break;

            //---------------------------------------------------------------------------
            // Handle CAN error frames
            //---------------------------------------------------------------------------
            case CAN_MSGTYPE_ERROR:
               clLogMsgP = QString("QCanInterfaceVci::read() - CAN_MSGTYPE_ERROR : %1").arg(tsVciCanMsgT.abData[0]);
               emit addLogMessage(clLogMsgP, QCan::eLOG_LEVEL_TRACE);
               switch (tsVciCanMsgT.abData[0])
               {
                  case CAN_ERROR_STUFF:
                     clErrFrameP.setErrorType(QCanFrame::eERROR_TYPE_STUFF);
                     break;

                  case CAN_ERROR_FORM:
                     clErrFrameP.setErrorType(QCanFrame::eERROR_TYPE_FORM);
                     break;

                  case CAN_ERROR_ACK:
                     clErrFrameP.setErrorType(QCanFrame::eERROR_TYPE_ACK);
                     break;

                  case CAN_ERROR_BIT:
                     clErrFrameP.setErrorType(QCanFrame::eERROR_TYPE_BIT0);
                     break;
                  
                  default:
                     clErrFrameP.setErrorType(QCanFrame::eERROR_TYPE_CRC);
                     break;
               }

               clFrameR = clErrFrameP;
               clRetValueT = eERROR_NONE;
               break;

            //---------------------------------------------------------------------------
            // Read CAN status
            //---------------------------------------------------------------------------
            case CAN_MSGTYPE_STATUS:
               clLogMsgP = QString("QCanInterfaceVci::read() - CAN_MSGTYPE_STATUS : %1").arg(tsVciCanMsgT.abData[0]);
               emit addLogMessage(clLogMsgP, QCan::eLOG_LEVEL_TRACE);

               if (tsVciCanMsgT.abData[0] == 0)
               {
                  emit stateChanged(QCan::eCAN_STATE_BUS_ACTIVE);
               }
               if (tsVciCanMsgT.abData[0] & ((uint8_t) CAN_STATUS_ERRLIM))
               {
                  emit stateChanged(QCan::eCAN_STATE_BUS_WARN);
               }
               if (tsVciCanMsgT.abData[0] & ((uint8_t) CAN_STATUS_BUSOFF))
               {
                  emit stateChanged(QCan::eCAN_STATE_BUS_OFF);
               }

               break;
            
         }

      }
      else if (teResultT == VCI_E_RXQUEUE_EMPTY)
      {
         clRetValueT = eERROR_FIFO_RCV_EMPTY;
      } 
      else
      {
         emit connectionChanged(QCanInterface::FailureState);
         clRetValueT = eERROR_LIBRARY;
      }
   }

   if (clRetValueT != eERROR_NONE)
   {
      QTimer::singleShot(10, this, SLOT(onTimerEvent()));
   }

   return (clRetValueT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::reset()                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfaceVci::reset()
{

   emit addLogMessage("QCanInterfaceVci::reset()                  : ", QCan::eLOG_LEVEL_TRACE);

   //---------------------------------------------------------------------------------------------------
   // reset statistic values
   //
   clStatisticP.ulErrCount = 0;
   clStatisticP.ulRcvCount = 0;
   clStatisticP.ulTrmCount = 0;

   //---------------------------------------------------------------------------------------------------
   // clear local error frame contents
   //
   clErrFrameP.setFrameType(QCanFrame::eFRAME_TYPE_ERROR);
   clErrFrameP.setErrorCounterReceive(0);
   clErrFrameP.setErrorCounterTransmit(0);
   clErrFrameP.setErrorState(QCan::eCAN_STATE_BUS_ACTIVE);

   //---------------------------------------------------------------------------------------------------
   // reset device
   //
   if (ptsCanControlP != nullptr)
   {
      ptsCanControlP->ResetLine();
   }


   //---------------------------------------------------------------------------------------------------
   // set can interface in previous mode
   //
   return setMode(teCanModeP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::setBitrate()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceVci::setBitrate(int32_t slNomBitRateV, int32_t slDatBitRateV)
{
   HRESULT        teResultT;                 // error code of VCI functions
   QCanInterface::InterfaceError_e teReturnT = eERROR_NONE;

   clLogMsgP = QString("QCanInterfaceVci::setBitrate(%1 , %2)").arg(slNomBitRateV).arg(slDatBitRateV);
   emit addLogMessage(clLogMsgP, QCan::eLOG_LEVEL_TRACE);

   //---------------------------------------------------------------------------------------------------
   // Configure CANBTP structure for nominal bitrate settings
   //
   if (slDatBitRateV == QCan::eCAN_BITRATE_NONE)
   {
      slNomBitRateV = slNomBitRateV / 1000;
      switch (slNomBitRateV)
      {
         case 10:
            tsBitrateNomP = CAN_BITRATE_10KB;
            break;

         case 20:
            tsBitrateNomP = CAN_BITRATE_20KB;
            break;

         case 50:
            tsBitrateNomP = CAN_BITRATE_50KB;
            break;

         case 100:
            tsBitrateNomP = CAN_BITRATE_100KB;
            break;

         case 125:
            tsBitrateNomP = CAN_BITRATE_125KB;
            break;

         case 250:
            tsBitrateNomP = CAN_BITRATE_250KB;
            break;

         case 500:
            tsBitrateNomP = CAN_BITRATE_500KB;
            break;

         case 800:
            tsBitrateNomP = CAN_BITRATE_800KB;
            break;

         case 1000:
            tsBitrateNomP = CAN_BITRATE_1000KB;
            break;

      }
   }
   else
   {
      //tsBitrateDatP

   }

   tsLineInitP.sBtpSdr = tsBitrateNomP;
   tsLineInitP.sBtpFdr = tsBitrateDatP;

   if (ptsCanControlP != nullptr)
   {
      teResultT = ptsCanControlP->InitLine(&tsLineInitP);
      if (teResultT == VCI_OK)
      {

      }
      else
      {

      }
   }
   else
   {
      teReturnT = eERROR_MODE;
   }

   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanInterfaceVci::setBitrate() .... 3";
   #endif

   return teReturnT;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::setMode()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfaceVci::setMode(const QCan::CAN_Mode_e teModeV)
{
   HRESULT        teResultT;                 // error code of VCI functions


   //---------------------------------------------------------------------------------------------------
   // select mode
   //
   switch (teModeV)
   {
      case QCan::eCAN_MODE_START :
         //-----------------------------------------------------------------------------------
         // reset statistic values
         //
         clStatisticP.ulErrCount = 0;
         clStatisticP.ulRcvCount = 0;
         clStatisticP.ulTrmCount = 0;

         if (ptsCanControlP != nullptr)
         {
            ptsCanControlP->SetAccFilter(CAN_FILTER_STD, CAN_ACC_CODE_ALL, CAN_ACC_MASK_ALL);
            ptsCanControlP->SetAccFilter(CAN_FILTER_EXT, CAN_ACC_CODE_ALL, CAN_ACC_MASK_ALL);

            teResultT = ptsCanControlP->StartLine();
            if (teResultT == VCI_OK)
            {
               teCanModeP = QCan::eCAN_MODE_START;
               emit addLogMessage("QCanInterfaceVci::setMode(eCAN_MODE_START)   : success", QCan::eLOG_LEVEL_TRACE);
            }
            else
            {
               emit addLogMessage("QCanInterfaceVci::setMode(eCAN_MODE_START)   : failed", QCan::eLOG_LEVEL_TRACE);
            }
         }
         break;

      case QCan::eCAN_MODE_STOP :
         emit addLogMessage("QCanInterfaceVci::setMode(eCAN_MODE_STOP)    : ", QCan::eLOG_LEVEL_TRACE);
         if (ptsCanControlP != nullptr)
         {
            ptsCanControlP->StopLine();
         }
         teCanModeP = QCan::eCAN_MODE_STOP;
         break;

      default :
         return eERROR_MODE;
         break;
   }

   return eERROR_NONE;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::socketInit()                                                                                     //
// Initialise socket to CAN interface and check capabilities                                                          //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanInterfaceVci::socketInit(void)
{
   HRESULT     teResultT = VCI_E_NOT_INITIALIZED;  // error code of VCI functions
   bool        btFunctionResultT = true;

   if (ptsBalObjectP != nullptr)
   {
      //-------------------------------------------------------------------------------------------
      // Check controller capabilities create a message channel
      //
      ICanSocket2 * ptsCanSocketT = nullptr;
      teResultT = ptsBalObjectP->OpenSocket(ulBusCtlNumP, IID_ICanSocket2, (void**)&ptsCanSocketT);
      if (teResultT == VCI_OK)
      {
         //-----------------------------------------------------------------------------------
         // check capabilities
         //
         this->ulFeaturesDeviceP = 0;
         CANCAPABILITIES2 tsCanCapabilitiesT = { 0 };
         teResultT = ptsCanSocketT->GetCapabilities(&tsCanCapabilitiesT);
         if (teResultT == VCI_OK)
         {
            //----------------------------------------------------------------------------
            // Check for CAN FD
            //
            if (tsCanCapabilitiesT.dwFeatures & CAN_FEATURE_EXTDATA) 
            {
               // this->ulFeaturesDeviceP += QCAN_IF_SUPPORT_CAN_FD;
            }

            //----------------------------------------------------------------------------
            // Check for Listen-only
            //
            if (tsCanCapabilitiesT.dwFeatures & CAN_FEATURE_LISTONLY) 
            {
               this->ulFeaturesDeviceP += QCAN_IF_SUPPORT_LISTEN_ONLY;
            }

            //----------------------------------------------------------------------------
            // Check for Error frames
            //
            if (tsCanCapabilitiesT.dwFeatures & CAN_FEATURE_ERRFRAME) 
            {
               this->ulFeaturesDeviceP += QCAN_IF_SUPPORT_ERROR_FRAMES;
            }
         }
      }
      else
      {
         btFunctionResultT = false;
         
         #ifndef QT_NO_DEBUG_OUTPUT
         qDebug() << " Error while opening the Socket (Message Channel) with error " << QString::number(teResultT);
         #endif

      }

      //-------------------------------------------------------------------------------------------
      // Create a CAN message channel
      //
      if (teResultT == VCI_OK)
      {
        teResultT = ptsCanSocketT->CreateChannel(FALSE, &ptsCanChannelP);
      }

      //-------------------------------------------------------------------------------------------
      // Create a CAN control channel
      //
      if (teResultT == VCI_OK)
      {
         teResultT = ptsBalObjectP->OpenSocket(ulBusCtlNumP, IID_ICanControl2, (void**)&ptsCanControlP);
         if (teResultT != VCI_OK)
         {
            #ifndef QT_NO_DEBUG_OUTPUT
            qDebug() << " Error while opening the Socket (CAN control channel) with error " << QString::number(teResultT);
            #endif
         }
      } else
      {
         #ifndef QT_NO_DEBUG_OUTPUT
         qDebug() << " FAIL to create channel 'CreateChannel()' returns: " + QString::number(teResultT);
         #endif
      }

      ptsCanSocketT->Release();
    }


   //---------------------------------------------------------------------------------------------------
   // initialize the message channel
   //
   if (teResultT == VCI_OK)
   {
      uint16_t uwRxFifoSizeT  = 1024;
      uint16_t uwRxThresholdT = 1;
      uint16_t uwTxFifoSizeT  = 128;
      uint16_t uwTxThresholdT = 1;

      btFunctionResultT = false;

      //-------------------------------------------------------------------------------------------
      // inclusive filtering (pass registered IDs) and pass self-rec messages from all channels
      //
      teResultT = ptsCanChannelP->Initialize(uwRxFifoSizeT, uwTxFifoSizeT, 0, CAN_FILTER_INCL|CAN_FILTER_SRRA);
      if (teResultT == VCI_OK)
      {
         teResultT = ptsCanChannelP->GetReader(&ptsRcvFifoP);
         if (teResultT == VCI_OK)
         {
            ptsRcvFifoP->SetThreshold(uwRxThresholdT);

            //hEventReader = CreateEvent(NULL, FALSE, FALSE, NULL);
            //ptsRcvFifoP->AssignEvent(hEventReader);
         }
      }

      if (teResultT == VCI_OK)
      {
         teResultT = ptsCanChannelP->GetWriter(&ptsTrmFifoP);
         if (teResultT == VCI_OK)
         {
            ptsTrmFifoP->SetThreshold(uwTxThresholdT);
         }
      }

      //-------------------------------------------------------------------------------------------
      // activate the CAN channel
      //
      if (teResultT == VCI_OK)
      {
         teResultT = ptsCanChannelP->Activate();
         btFunctionResultT = true;
      }

   }


   if (btFunctionResultT)
   {
      emit addLogMessage("QCanInterfaceVci::socketInit()               : success", QCan::eLOG_LEVEL_TRACE);
   }
   else
   {
      emit addLogMessage("QCanInterfaceVci::socketInit()               : failed", QCan::eLOG_LEVEL_TRACE);
   }

   return (btFunctionResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::socketRelease()                                                                                  //
// Release socket to CAN interface                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanInterfaceVci::socketRelease(void)
{
   HRESULT     teResultT = VCI_OK;                 // error code of VCI functions

   if (ptsCanChannelP != nullptr)
   {
      teResultT = ptsCanChannelP->Deactivate();
   }

   if (ptsCanControlP != nullptr)
   {
      teResultT = ptsCanControlP->StopLine();

      ptsCanControlP->Release();
      ptsCanControlP = nullptr;
   }

   if (teResultT == VCI_OK)
   {
      emit addLogMessage("QCanInterfaceVci::socketRelease()            : success", QCan::eLOG_LEVEL_TRACE);
   }
   else
   {
      emit addLogMessage("QCanInterfaceVci::socketRelease()            : failed", QCan::eLOG_LEVEL_TRACE);
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::state()                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCan::CAN_State_e QCanInterfaceVci::state(void)
{
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanInterfaceVci::state()";
   #endif
   
   return (teErrorStateP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::statistic()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfaceVci::statistic(QCanStatistic_ts &clStatisticR)
{

   clStatisticR = clStatisticP;

   return(eERROR_NONE);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::supportedFeatures()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
uint32_t QCanInterfaceVci::supportedFeatures()
{
   //---------------------------------------------------------------------------------------------------
   // The value is updated inside socketInit()
   //
   return (ulFeaturesDeviceP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::version()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanInterfaceVci::version(void)
{
   #ifndef QT_NO_DEBUG_OUTPUT
   qDebug() << "QCanInterfaceVci::version()";
   #endif

   QString clVersionT;

   clVersionT  = QString("%1.%2.").arg(VERSION_MAJOR).arg(VERSION_MINOR, 2, 10, QLatin1Char('0'));
   clVersionT += QString("%1").arg(VERSION_BUILD, 2, 10, QLatin1Char('0'));

   return (clVersionT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceVci::write()                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfaceVci::write( const QCanFrame &clFrameR)
{
   CANMSG2           tsVciCanMsgT;
   HRESULT           teResultT;                       // error code of VCI functions
   InterfaceError_e  clRetValueT = eERROR_DEVICE;
   uint8_t           ubDataCountT;

   //---------------------------------------------------------------------------------------------------
   // prepare CAN message
   //
   tsVciCanMsgT.uMsgInfo.Bytes.bAccept = 0;
   tsVciCanMsgT.uMsgInfo.Bytes.bFlags = 0;
   tsVciCanMsgT.uMsgInfo.Bytes.bType = 0;

   //---------------------------------------------------------------------------------------------------
   // Convert QCanFrame structure to VCI CAN message structure
   //---------------------------------------------------------------------------------------------------

   //---------------------------------------------------------------------------------------------------
   // Check for Extended Frame Format
   //
   if (clFrameR.isExtended())
   {
      tsVciCanMsgT.uMsgInfo.Bits.ext = 1;
   } 
   else
   {
      tsVciCanMsgT.uMsgInfo.Bits.ext = 0;
   }

   //---------------------------------------------------------------------------------------------------
   // Assign identifier
   //
   tsVciCanMsgT.dwMsgId = clFrameR.identifier();

   //---------------------------------------------------------------------------------------------------
   // Assign DLC
   //
   tsVciCanMsgT.uMsgInfo.Bits.dlc = clFrameR.dlc();

   //---------------------------------------------------------------------------------------------------
   // Assign data
   //
   for (ubDataCountT = 0; ubDataCountT < clFrameR.dataSize(); ubDataCountT++)
   {
      tsVciCanMsgT.abData[ubDataCountT] = clFrameR.data(ubDataCountT);
   }
   tsVciCanMsgT.uMsgInfo.Bytes.bType = CAN_MSGTYPE_DATA;
   tsVciCanMsgT.dwTime = 0;

   //---------------------------------------------------------------------------------------------------
   // Perform frame transmission
   //
   if (ptsTrmFifoP != nullptr)
   {
      teResultT = ptsTrmFifoP->PutDataEntry(&tsVciCanMsgT);

      //-------------------------------------------------------------------------------------------
      // Check result of transmission request
      //
      switch (teResultT)
      {
         case VCI_OK:
            clStatisticP.ulTrmCount++;
            clRetValueT = eERROR_NONE;
            break;

         case VCI_E_TXQUEUE_FULL:
            clRetValueT = eERROR_FIFO_TRM_FULL;
            break;

      }

   }

   return (clRetValueT);
}

