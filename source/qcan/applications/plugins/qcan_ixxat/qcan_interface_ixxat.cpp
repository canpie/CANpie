//============================================================================//
// File:          qcan_ixxatusb.cpp                                           //
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
   qDebug() << "QCanInterfaceIxxat::QCanInterfaceIxxat()";

   if (!pclIxxatVciP.isAvailable())
   {
      qCritical() << "QCanInterfaceIxxat(): IXXAT library is not available!";
   }

   clDevInfoP = clDevInfoV;
}

//----------------------------------------------------------------------------//
// ~QCanInterfaceIxxat()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterfaceIxxat::~QCanInterfaceIxxat()
{

}

//----------------------------------------------------------------------------//
// connect()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceIxxat::connect()
{
   //----------------------------------------------------------------
   // try to open a device
   //
   if (pclIxxatVciP.pfnVciDeviceOpenP(clDevInfoP.VciObjectId,&vdCanInterfaceP) != VCI_OK)
   {
      qWarning() << tr("WARNING: Fail to open a device interface!");
      return eERROR_DEVICE;
   }

   btConnectedP = true;

   return eERROR_NONE;
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
   HRESULT slStatusT;
   slStatusT = pclIxxatVciP.pfnVciDeviceCloseP(vdCanInterfaceP);
   if (slStatusT != VCI_OK)
   {
      qDebug() << "Fail to disconnect the device";
   }

   btConnectedP = false;

   return eERROR_NONE;
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
   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (pclIxxatVciP.isAvailable())
   {
      return QString(clDevInfoP.Description) + QString(" CAN 1");
   }

   return QString("IXXAT VCI library is not available");
}


//----------------------------------------------------------------------------//
// read()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceIxxat::read(QCanFrame &clFrameR)
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
   // get next message in FIFO
   //
   slResultT = pclIxxatVciP.pfnCanChannelPeekMessageP(vdCanChannelP,&tsCanMsgT);

   if (slResultT == VCI_OK)
   {
      // handle data depending on type
      switch (tsCanMsgT.uMsgInfo.Bytes.bType)
      {
         case CAN_MSGTYPE_DATA :
            qDebug() << tr("handle CAN_MSGTYPE_DATA");

            // copy all needed parameters to QCanFrame structure
            if (tsCanMsgT.uMsgInfo.Bits.ext)
            {
               clFrameR.setIdentifier(tsCanMsgT.dwMsgId);
            } else
            {
               clFrameR.setIdentifier((uint16_t)tsCanMsgT.dwMsgId);
            }

            clFrameR.setDlc(tsCanMsgT.uMsgInfo.Bits.dlc);

            for (slByteCntrT = 0; slByteCntrT < clFrameR.dlc(); slByteCntrT++)
            {
               clFrameR.setData(slByteCntrT, tsCanMsgT.abData[slByteCntrT]);
            }

            clStatisticP.ulRcvCount++;

            return eERROR_NONE;

            break;

         case CAN_MSGTYPE_INFO :
            qDebug() << tr("handle CAN_MSGTYPE_INFO");
            break;

         case CAN_MSGTYPE_ERROR :
            qDebug() << tr("handle CAN_MSGTYPE_ERROR");
            break;

         case CAN_MSGTYPE_STATUS :
            qDebug() << tr("handle CAN_MSGTYPE_STATUS");
            break;

         default :
            qDebug() << tr("UNKNOWN Message Type");
            break;
      }
   }

   else if (slResultT != (HRESULT)VCI_E_RXQUEUE_EMPTY)
   {
      qWarning() << "QCanInterface::read() -> CanChannelPeekMessage()" <<
                    "fail with error:" <<
                    pclIxxatVciP.formatedError((HRESULT)slResultT);
      return eERROR_DEVICE;
   }


   return eERROR_FIFO_RCV_EMPTY;
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
   // select corresponding PEAK baud rate value
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

         break;

      case eCAN_MODE_STOP :
         if (pclIxxatVciP.pfnCanChannelActivateP(vdCanChannelP,false) != VCI_OK)
         {
            qWarning() << tr("WARNING: Fail to deactivate CAN channel!");
         }
         break;

      default :
         return eERROR_MODE;
         break;
   }

   return eERROR_NONE;
}


//----------------------------------------------------------------------------//
// state()                                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
CAN_State_e	QCanInterfaceIxxat::state()
{
   return eCAN_STATE_BUS_ACTIVE;
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
   uint32_t ulFeaturesT;

   ulFeaturesT  = QCAN_IF_SUPPORT_ERROR_FRAMES;
   ulFeaturesT += QCAN_IF_SUPPORT_LISTEN_ONLY;

   #if QCAN_SUPPORT_CAN_FD > 0
   ulFeaturesT += QCAN_IF_SUPPORT_CAN_FD;
   #endif

   return(ulFeaturesT);
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

   if (slResultT == VCI_OK)
   {
      qDebug() << tr("PosMessage() OK");

      clStatisticP.ulTrmCount++;
      return eERROR_NONE;
   }
   else if (slResultT != (HRESULT)VCI_E_TXQUEUE_FULL)
   {
      qWarning() << tr("Fail to call pfnCanChannelPostMessageP(): ") + QString::number(slResultT,16);
      return eERROR_DEVICE;
   }

   return eERROR_FIFO_TRM_FULL;

}

