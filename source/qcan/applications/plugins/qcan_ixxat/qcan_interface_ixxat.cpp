//============================================================================//
// File:          qcan_interface_ixxat.cpp                                    //
// Description:   CAN plugin for IXXAT USB device                             //
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
#include <QtWidgets>
#include "qcan_defs.hpp"
#include <qcan_interface_ixxat.hpp>


/*----------------------------------------------------------------------------*\
** Function implementation                                                    **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanInterfaceIxxat()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterfaceIxxat::QCanInterfaceIxxat(VCIDEVICEINFO clDevInfoV)
{
   if (!pclIxxatVciP.isAvailable())
   {
      qCritical() << "QCanInterfaceIxxat(): IXXAT library is not available!";
   }

   clDevInfoP = clDevInfoV;
   btConnectedP = false;
   btFdUsedP = false;
   teCanModeP = eCAN_MODE_STOP;
   btMsgAndErrPendingP = false;

   // reset pointer
   vdCanInterfaceP = NULL;
   vdCanControlP = NULL;
   vdCanChannelP = NULL;
}

//----------------------------------------------------------------------------//
// ~QCanInterfaceIxxat()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterfaceIxxat::~QCanInterfaceIxxat()
{
   if (pclIxxatVciP.isAvailable())
   {
      if (btConnectedP == true)
      {
         if (vdCanInterfaceP != NULL)
         {
            pclIxxatVciP.pfnVciDeviceCloseP(vdCanInterfaceP);
         }
      }
   }
}

//----------------------------------------------------------------------------//
// connect()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceIxxat::connect()
{
   InterfaceError_e teStatusT = eERROR_NONE;

   //----------------------------------------------------------------
   // check library is available
   //
   if (pclIxxatVciP.isAvailable() != true)
   {
      teStatusT = eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // try to open a device
   //
   if (pclIxxatVciP.pfnVciDeviceOpenP(clDevInfoP.VciObjectId, &vdCanInterfaceP) != VCI_OK)
   {
      teStatusT = eERROR_DEVICE;
   }

   //----------------------------------------------------------------
   // successfully open
   //
   else
   {
      btConnectedP = true;
   }

   return teStatusT;
}

//----------------------------------------------------------------------------//
// connected()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanInterfaceIxxat::connected()
{
   return btConnectedP;
}


//----------------------------------------------------------------------------//
// disconnect()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceIxxat::disconnect()
{
   qDebug() << "QCanInterfaceIxxat::disconnect()";
   InterfaceError_e teStatusT = eERROR_NONE;

   //----------------------------------------------------------------
   // check library is available
   //
   if (pclIxxatVciP.isAvailable() != true)
   {
      teStatusT = eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // check library is connected
   //
   else if (btConnectedP != true)
   {
      teStatusT = eERROR_DEVICE;
   }

   //----------------------------------------------------------------
   // check interface pointer is valid
   //
   else if (vdCanInterfaceP == NULL)
   {
      teStatusT = eERROR_DEVICE;
   }

   //----------------------------------------------------------------
   // close
   //
   else if (pclIxxatVciP.pfnVciDeviceCloseP(vdCanInterfaceP) != VCI_OK)
   {
      qDebug() << "Fail to disconnect the device";
      teStatusT = eERROR_DEVICE;
   }

   //----------------------------------------------------------------
   // reset handler in case of success
   //
   else
   {
      vdCanInterfaceP = NULL;
      btConnectedP = false;
   }

   return teStatusT;
}


//----------------------------------------------------------------------------//
// icon()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QIcon QCanInterfaceIxxat::icon()
{
   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (pclIxxatVciP.isAvailable())
   {
      QString clIconNameT = clDevInfoP.Description;
      clIconNameT = clIconNameT.toLower();
      clIconNameT.replace(QString(" "),QString("-"));

      return QIcon(QString(":/images/"+clIconNameT+".png"));
   }

   return QIcon("images/ixxat_fail.ico");
}


//----------------------------------------------------------------------------//
// name()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QString QCanInterfaceIxxat::name()
{
   QString clReturnT = "IXXAT VCI 3.5 library is not available";

   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (pclIxxatVciP.isAvailable())
   {
      clReturnT = QString(clDevInfoP.Description) + " " +clDevInfoP.UniqueHardwareId.AsChar;
   }

   return clReturnT;
}

//----------------------------------------------------------------------------//
// read()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfaceIxxat::read(QByteArray &clDataR)
{
   CANMSG  tsCanMsgT;
   HRESULT slResultT;
   int32_t slByteCntrT;
   QCanFrame      clCanFrameT;
   QCanFrameError clErrFrameT;
   QCanTimeStamp  clTimeStampT;
   InterfaceError_e  clRetValueT = eERROR_NONE;

   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (!pclIxxatVciP.isAvailable())
   {
      return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // get and process next message in FIFO
   //
   slResultT = pclIxxatVciP.pfnCanChannelPeekMessageP(vdCanChannelP,&tsCanMsgT);
   if (slResultT == VCI_OK)
   {
      //--------------------------------------------------------
      // handle message data
      //
      if ((tsCanMsgT.uMsgInfo.Bytes.bType) == CAN_MSGTYPE_DATA)
      {
         // copy all needed parameters to QCanFrame structure
         if (tsCanMsgT.uMsgInfo.Bits.ext)
         {
            clCanFrameT.setIdentifier(tsCanMsgT.dwMsgId);
         } else
         {
            clCanFrameT.setIdentifier((uint16_t)tsCanMsgT.dwMsgId);
         }

         clCanFrameT.setDlc(tsCanMsgT.uMsgInfo.Bits.dlc);

         for (slByteCntrT = 0; slByteCntrT < clCanFrameT.dlc(); slByteCntrT++)
         {
            clCanFrameT.setData(slByteCntrT, tsCanMsgT.abData[slByteCntrT]);
         }

         //------------------------------------------------
         // increase statistic counter
         //
         clStatisticP.ulRcvCount++;

         //------------------------------------------------
         // copy the CAN frame to a byte array for transfer
         //
         clDataR = clCanFrameT.toByteArray();
      }

      //--------------------------------------------------------
      // handle message data
      //
      else if ((tsCanMsgT.uMsgInfo.Bytes.bType) == CAN_MSGTYPE_INFO)
      {
         //------------------------------------------------
         // get info and pass it via error frame to application
         //
         if (tsCanMsgT.abData[0] == CAN_INFO_START)
         {
            clErrFrameT.setErrorState(eCAN_STATE_BUS_ACTIVE);
         }

         else
         {
            clErrFrameT.setErrorState(eCAN_STATE_STOPPED);
         }
         clErrFrameT.setErrorType(QCanFrameError::eERROR_TYPE_NONE);

         //------------------------------------------------
         // copy the error frame to a byte array
         //
         clDataR = clErrFrameT.toByteArray();
      }


      //--------------------------------------------------------
      // handle message error data
      //
      else if ((tsCanMsgT.uMsgInfo.Bytes.bType) == CAN_MSGTYPE_ERROR)
      {
         //-------------------------------------------------
         // prepare error frame
         //
         setupErrorFrame(tsCanMsgT.abData[1], tsCanMsgT.abData[0],
                           clErrFrameT);

         //------------------------------------------------
         // copy the error frame to a byte array
         //
         clDataR = clErrFrameT.toByteArray();

         //-------------------------------------------------
         // update number of received error frames
         //
         clStatisticP.ulErrCount++;
      }

      //--------------------------------------------------------
      // handle message status data
      //
      else if ((tsCanMsgT.uMsgInfo.Bytes.bType) == CAN_MSGTYPE_STATUS)
      {
         //----------------------------------------------------------------
         // set bus status
         //
         if ((tsCanMsgT.abData[0] & CAN_STATUS_ERRLIM) != 0)
         {
            clErrFrameT.setErrorState(eCAN_STATE_BUS_WARN);
         }

         else if ((tsCanMsgT.abData[0] & CAN_STATUS_OVRRUN) != 0)
         {
            clErrFrameT.setErrorState(eCAN_STATE_BUS_PASSIVE);
         }

         else if ((tsCanMsgT.abData[0] & CAN_STATUS_BUSOFF) != 0)
         {
            clErrFrameT.setErrorState(eCAN_STATE_BUS_OFF);
         }
         else
         {
            clErrFrameT.setErrorState(eCAN_STATE_BUS_ACTIVE);
         }

         clErrFrameT.setErrorType(QCanFrameError::eERROR_TYPE_NONE);


         //------------------------------------------------
         // copy the error frame to a byte array
         //
         clDataR = clErrFrameT.toByteArray();
      }

      //--------------------------------------------------------
      // all other not supported data
      //
      else
      {
         // do not process frame data
         clRetValueT = eERROR_FIFO_RCV_EMPTY;
      }
   }

   //----------------------------------------------------------------
   // some other error is pending
   //
   else if (slResultT != (HRESULT)VCI_E_RXQUEUE_EMPTY)
   {
      qWarning() << "QCanInterface::read() -> CanChannelPeekMessage()" <<
                    "fail with error:" <<
                    pclIxxatVciP.formatedError((HRESULT)slResultT);

      clRetValueT =  eERROR_DEVICE;
   }

   //----------------------------------------------------------------
   // no data in receive buffer
   //
   else
   {
      clRetValueT =  eERROR_FIFO_RCV_EMPTY;
   }

   return clRetValueT;
}

//----------------------------------------------------------------------------//
// reset()                                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfaceIxxat::reset()
{
   qDebug() << "QCanInterfaceIxxat::reset()";

   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (!pclIxxatVciP.isAvailable())
   {
      return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // reset statistic values
   //
   clStatisticP.ulErrCount = 0;
   clStatisticP.ulRcvCount = 0;
   clStatisticP.ulTrmCount = 0;


   //----------------------------------------------------------------
   // reset device
   //
   if (pclIxxatVciP.pfnCanControlResetP(vdCanControlP) != VCI_OK)
   {
      qDebug() << "QCanInterfaceIxxat::reset() FAIL pfnCanControlResetP";

      return eERROR_DEVICE;
   }

   //----------------------------------------------------------------
   // set can interface in previous mode
   //
   return setMode(teCanModeP);
}


//----------------------------------------------------------------------------//
// setupErrorFrame()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanInterfaceIxxat::setupErrorFrame(uint8_t ubStatusV,
                                         uint8_t ubErrorV,
                                         QCanFrameError &clFrameR)
{
   //---------------------------------------------------------
   // set frame type
   //
   switch (ubErrorV)
   {
      case CAN_ERROR_STUFF :
         clFrameR.setErrorType(QCanFrameError::eERROR_TYPE_STUFF);
         break;
      case CAN_ERROR_FORM  :
         clFrameR.setErrorType(QCanFrameError::eERROR_TYPE_FORM);
         break;
      case CAN_ERROR_ACK   :
         clFrameR.setErrorType(QCanFrameError::eERROR_TYPE_ACK);
         break;
      case CAN_ERROR_BIT   :
         clFrameR.setErrorType(QCanFrameError::eERROR_TYPE_BIT0);
         break;
      case CAN_ERROR_CRC   :
         clFrameR.setErrorType(QCanFrameError::eERROR_TYPE_CRC);
         break;
      case CAN_ERROR_OTHER :
         clFrameR.setErrorType(QCanFrameError::eERROR_TYPE_BIT1);
         break;
      default :
         clFrameR.setErrorType(QCanFrameError::eERROR_TYPE_NONE);
         break;
   }

   btMsgAndErrPendingP = false;

   //----------------------------------------------------------------
   // set bus status
   //
   if ((ubStatusV & CAN_STATUS_ERRLIM) != 0)
   {
      clFrameR.setErrorState(eCAN_STATE_BUS_WARN);

      if ((ubStatusV & CAN_STATUS_TXPEND) != 0)
      {
         btMsgAndErrPendingP = true;
      }
   }

   else if ((ubStatusV & CAN_STATUS_OVRRUN) != 0)
   {
      clFrameR.setErrorState(eCAN_STATE_BUS_PASSIVE);
   }

   else if ((ubStatusV & CAN_STATUS_BUSOFF) != 0)
   {
      clFrameR.setErrorState(eCAN_STATE_BUS_OFF);
   }
   else
   {
      clFrameR.setErrorState(eCAN_STATE_BUS_ACTIVE);
   }


}
//----------------------------------------------------------------------------//
// setBitrate()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceIxxat::setBitrate(int32_t slBitrateV,
                                                               int32_t slBrsClockV)
{
   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (!pclIxxatVciP.isAvailable())
   {
      return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // open control interface of selected device
   //
   if (pclIxxatVciP.pfnCanControlOpenP(vdCanInterfaceP,0,&vdCanControlP) != VCI_OK)
   {
      qWarning() << tr("WARNING: Fail to open control interface!");
   }

   //----------------------------------------------------------------
   // Check bit-rate value
   //
   if (slBitrateV >= eCAN_BITRATE_AUTO)
   {
      // maybe we get a value in Hz, normalise it to kHz
      slBitrateV = slBitrateV / 1000;
   }

   //----------------------------------------------------------------
   // Check BRS clock value for CAN-FD
   // todo : test with CAN-FD interface
   //
   if (slBrsClockV != eCAN_BITRATE_NONE)
   {

   }

   //----------------------------------------------------------------
   // select corresponding baud rate value
   //
   switch (slBitrateV)
   {
      // value from CANpie enumeration
      case eCAN_BITRATE_10K:
         tsBitrateP.ubBtr0 = CAN_BT0_10KB;
         tsBitrateP.ubBtr1 = CAN_BT1_10KB;
         break;
      case eCAN_BITRATE_20K:
         tsBitrateP.ubBtr0 = CAN_BT0_20KB;
         tsBitrateP.ubBtr1 = CAN_BT1_20KB;
         break;
      case eCAN_BITRATE_50K:
         tsBitrateP.ubBtr0 = CAN_BT0_50KB;
         tsBitrateP.ubBtr1 = CAN_BT1_50KB;
         break;
      case eCAN_BITRATE_100K:
         tsBitrateP.ubBtr0 = CAN_BT0_100KB;
         tsBitrateP.ubBtr1 = CAN_BT1_100KB;
         break;
      case eCAN_BITRATE_125K:
         tsBitrateP.ubBtr0 = CAN_BT0_125KB;
         tsBitrateP.ubBtr1 = CAN_BT1_125KB;
         break;
      case eCAN_BITRATE_250K:
         tsBitrateP.ubBtr0 = CAN_BT0_250KB;
         tsBitrateP.ubBtr1 = CAN_BT1_250KB;
         break;
      case eCAN_BITRATE_500K:
         tsBitrateP.ubBtr0 = CAN_BT0_500KB;
         tsBitrateP.ubBtr1 = CAN_BT1_500KB;
         break;
      case eCAN_BITRATE_800K:
         tsBitrateP.ubBtr0 = CAN_BT0_800KB;
         tsBitrateP.ubBtr1 = CAN_BT1_800KB;
         break;
      case eCAN_BITRATE_1M:
         tsBitrateP.ubBtr0 = CAN_BT0_1000KB;
         tsBitrateP.ubBtr1 = CAN_BT0_1000KB;
         break;

      // value normalized to kHz
      case 10:
         tsBitrateP.ubBtr0 = CAN_BT0_10KB;
         tsBitrateP.ubBtr1 = CAN_BT1_10KB;
         break;
      case 20:
         tsBitrateP.ubBtr0 = CAN_BT0_20KB;
         tsBitrateP.ubBtr1 = CAN_BT1_20KB;
      break;
      case 50:
         tsBitrateP.ubBtr0 = CAN_BT0_50KB;
         tsBitrateP.ubBtr1 = CAN_BT1_50KB;
         break;
      case 100:
         tsBitrateP.ubBtr0 = CAN_BT0_100KB;
         tsBitrateP.ubBtr1 = CAN_BT1_100KB;
         break;
      case 125:
         tsBitrateP.ubBtr0 = CAN_BT0_125KB;
         tsBitrateP.ubBtr1 = CAN_BT1_125KB;
         break;
      case 250:
         tsBitrateP.ubBtr0 = CAN_BT0_250KB;
         tsBitrateP.ubBtr1 = CAN_BT1_250KB;
         break;
      case 500:
         tsBitrateP.ubBtr0 = CAN_BT0_500KB;
         tsBitrateP.ubBtr1 = CAN_BT1_500KB;
         break;
      case 800:
         tsBitrateP.ubBtr0 = CAN_BT0_800KB;
         tsBitrateP.ubBtr1 = CAN_BT1_800KB;
         break;
      case 1000:
         tsBitrateP.ubBtr0 = CAN_BT0_1000KB;
         tsBitrateP.ubBtr1 = CAN_BT1_1000KB;
         break;

      default:
         tsBitrateP.ubBtr0 = 0;
         tsBitrateP.ubBtr1 = 0;

         return eERROR_BITRATE;
         break;
   }

   return eERROR_NONE;
}


//----------------------------------------------------------------------------//
// setMode()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e	QCanInterfaceIxxat::setMode(const CAN_Mode_e teModeV)
{
   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (!pclIxxatVciP.isAvailable())
   {
      return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // select mode
   //
   switch (teModeV)
   {
      case eCAN_MODE_START :
         //---------------------------------------------------
         // reset statistic values
         //
         clStatisticP.ulErrCount = 0;
         clStatisticP.ulRcvCount = 0;
         clStatisticP.ulTrmCount = 0;

         pclIxxatVciP.pfnCanControlResetP(vdCanControlP);
         if (pclIxxatVciP.pfnCanControlInitializeP(vdCanControlP,
                                       (CAN_OPMODE_STANDARD |
                                        CAN_OPMODE_EXTENDED |
                                        CAN_OPMODE_ERRFRAME),
                                       tsBitrateP.ubBtr0,
                                       tsBitrateP.ubBtr1) != VCI_OK)
         {
            qWarning() << tr("WARNING: Fail to intialize control interface!");
         }

         if (pclIxxatVciP.pfnCanControlStartP(vdCanControlP,true) != VCI_OK)
         {
            qWarning() << tr("WARNING: Fail to start control interface!");
         }

         // open a can channel in NOT exlusive mode
         pclIxxatVciP.pfnCanChannelCloseP(vdCanChannelP);
         if (pclIxxatVciP.pfnCanChannelOpenP(vdCanInterfaceP,uwCanChannelP,false,&vdCanChannelP) != VCI_OK)
         {
            qWarning() << tr("WARNING: Fail to open a channel on seleceted interface!");
         }

         if (pclIxxatVciP.pfnCanChannelInitializeP(vdCanChannelP,64,1,64,1) != VCI_OK)
         {
            qWarning() << tr("WARNING: Fail to initialise CAN channel!");
         }

         if (pclIxxatVciP.pfnCanChannelActivateP(vdCanChannelP,true) != VCI_OK)
         {
            qWarning() << tr("WARNING: Fail to activate CAN channel!");
         }

         teCanModeP = eCAN_MODE_START;
         break;

      case eCAN_MODE_STOP :
         if (pclIxxatVciP.pfnCanChannelActivateP(vdCanChannelP,false) != VCI_OK)
         {
            qWarning() << tr("WARNING: Fail to deactivate CAN channel!");
         }
         teCanModeP = eCAN_MODE_STOP;
         break;

      default :
         return eERROR_MODE;
         break;
   }

   return eERROR_NONE;
}


//----------------------------------------------------------------------------//
// statistic()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e	QCanInterfaceIxxat::statistic(QCanStatistic_ts &clStatisticR)
{
   clStatisticR = clStatisticP;

   return(eERROR_NONE);
}


//----------------------------------------------------------------------------//
// supportedFeatures()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
uint32_t QCanInterfaceIxxat::supportedFeatures()
{
   PCANCAPABILITIES pclFunctionsT = NULL;
   uint32_t         ulReturnT = 0;

   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (!pclIxxatVciP.isAvailable())
   {
   }

   //----------------------------------------------------------------
   // read capabilities
   //
   else if (pclIxxatVciP.pfnCanControlGetCapsP(vdCanControlP, pclFunctionsT) == VCI_OK)
   {
      if ((pclFunctionsT->dwFeatures & CAN_FEATURE_ERRFRAME) != 0)
      {
         ulReturnT |= QCAN_IF_SUPPORT_ERROR_FRAMES;
      }

      if ((pclFunctionsT->dwFeatures & CAN_FEATURE_LISTONLY) != 0)
      {
         ulReturnT |= QCAN_IF_SUPPORT_LISTEN_ONLY;
      }
   }

   return(ulReturnT);
}


//----------------------------------------------------------------------------//
// write()                                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e	QCanInterfaceIxxat::write( const QCanFrame &clFrameR)
{
   CANMSG  tsCanMsgT;
   HRESULT slResultT;
   int32_t slByteCntrT;

   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (!pclIxxatVciP.isAvailable())
   {
      return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // prepare CAN message
   //
   tsCanMsgT.uMsgInfo.Bytes.bAccept = 0;
   tsCanMsgT.uMsgInfo.Bytes.bAddFlags = 0;
   tsCanMsgT.uMsgInfo.Bytes.bFlags = 0;
   tsCanMsgT.uMsgInfo.Bytes.bType = 0;

   // copy all needed parameters to QCanFrame structure
   if (clFrameR.isExtended())
   {
      tsCanMsgT.uMsgInfo.Bits.ext = 1;
   } else
   {
      tsCanMsgT.uMsgInfo.Bits.ext = 0;
   }

   tsCanMsgT.dwMsgId = clFrameR.identifier();

   tsCanMsgT.uMsgInfo.Bits.dlc = clFrameR.dlc();

   for (slByteCntrT = 0; slByteCntrT < clFrameR.dlc(); slByteCntrT++)
   {
      tsCanMsgT.abData[slByteCntrT] = clFrameR.data(slByteCntrT);
   }
   tsCanMsgT.uMsgInfo.Bytes.bType = CAN_MSGTYPE_DATA;
   tsCanMsgT.dwTime = 0;
   slResultT = pclIxxatVciP.pfnCanChannelPostMessageP(vdCanChannelP, &tsCanMsgT);

   //----------------------------------------------------------------
   // check resul of transmission
   //
   if (slResultT == VCI_OK)
   {
      clStatisticP.ulTrmCount++;
      return eERROR_NONE;
   }

   else if (slResultT != (HRESULT)VCI_E_TXQUEUE_FULL)
   {
      qCritical() << tr("Fail to call pfnCanChannelPostMessageP(): ") + QString::number(slResultT,16);
      return eERROR_DEVICE;
   }

   qCritical() << "FIFO full at call of pfnCanChannelPostMessageP()";
   return eERROR_FIFO_TRM_FULL;

}

