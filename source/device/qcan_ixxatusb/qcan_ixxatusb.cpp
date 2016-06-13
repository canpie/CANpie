//============================================================================//
// File:          qcan_ixxatusb.cpp                                           //
// Description:   CAN plugin for IXXAT USB device                             //
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

#include <QtWidgets>
#include <qcan_ixxatusb.hpp>

/*!
** \var aszErrTextG
** Zero terminated string buffer to print error descriptions
*/
char aszErrTextG[128];



QString QCanIxxatUsb::echo(const QString &message)
{
   uint32_t ulStatusT;
   QString clRetStringT;
   QCanFrame clFrameT; //  QCanFrame(QCanFrame::eTYPE_CAN_STD,0,0);

   aszErrTextG[0] = 0;
   aszErrTextG[1] = 0;
   aszErrTextG[2] = 0;

   emit errorOccurred(17);
   if (message == "load")
   {
      connect();
      clRetStringT = ";)";
   }

   if (message == "bitrate")
   {
      ulStatusT = setBitrate(eBITRATE_500K, 0);
   }

   if (message == "run")
   {
      if (connect() != eERROR_OK)
         qWarning() << "Warning: connect() fail!";

      if (setBitrate(eBITRATE_500K, 0) != eERROR_OK)
         qWarning() << "Warning: setBitrate() fail!";

      if (setMode(eMODE_START) != eERROR_OK)
         qWarning() << "Warning: setMode() fail!";

   }

   if (message == "read")
   {
      int32_t slStatusT = read(clFrameT);

      if (slStatusT == eERROR_OK)
      {
         if (clFrameT.isExtended())
         {
            qDebug() << "Receive Ext: " + QString::number((uint32_t)clFrameT.identifier(),16) + "h " +
                         QString::number((uint8_t)clFrameT.dlc(),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(0),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(1),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(2),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(3),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(4),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(5),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(6),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(7),16) + "h ";


         } else
         {
            qDebug() << "Receive Std: " + QString::number((uint32_t)clFrameT.identifier(),16) + "h " +
                         QString::number((uint8_t)clFrameT.dlc(),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(0),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(1),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(2),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(3),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(4),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(5),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(6),16) + "h " +
                         QString::number((uint8_t)clFrameT.data(7),16) + "h ";
         }
      }
      else if (slStatusT != eERROR_FIFO_IN_EMPTY)
      {
         qWarning() << "Warning: read() fail!";
      }

   }

   if (message == "write")
   {
      clFrameT.setStdId(175);
      clFrameT.setDlc(5);
      clFrameT.setData(0,5);
      clFrameT.setData(1,4);
      clFrameT.setData(2,3);
      clFrameT.setData(3,2);
      clFrameT.setData(4,1);

      int32_t slStatusT = write(clFrameT);

      if (slStatusT == eERROR_OK)
      {

      }
      else if (slStatusT != eERROR_FIFO_IN_EMPTY)
      {
         qWarning() << "Warning: write() fail!";
      }

   }
   return ("ixxat: " + message);
}

//----------------------------------------------------------------------------//
// disconnect()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t QCanIxxatUsb::setBitrate(uint32_t ulBitrateV, uint32_t ulBrsClockV)
{
   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (!btLibFuncLoadP)
   {
      return eERROR_LIBRARY;
   }


   //----------------------------------------------------------------
   // open control interface of selected device
   //
   if (pfnCanControlOpenP(vdCanInterfaceP,0,&vdCanControlP) != VCI_OK)
   {
      qWarning() << tr("WARNING: Fail to open control interface!");
   }


   //----------------------------------------------------------------
   // Check bitrate value
   //
   if (ulBitrateV >= eBITRATE_MAX)
   {
      // maybe we get a value in Hz, normalise it to kHz
      ulBitrateV = ulBitrateV / 1000;
   }

   //----------------------------------------------------------------
   // select corresponding PEAK baud rate value
   //
   switch (ulBitrateV)
   {
      // value from CANpie enumeration
      case eBITRATE_10K:
         tsBitrateP.ubBtr0 = CAN_BT0_10KB;
         tsBitrateP.ubBtr1 = CAN_BT1_10KB;
         break;
      case eBITRATE_20K:
         tsBitrateP.ubBtr0 = CAN_BT0_20KB;
         tsBitrateP.ubBtr1 = CAN_BT1_20KB;
         break;
      case eBITRATE_50K:
         tsBitrateP.ubBtr0 = CAN_BT0_50KB;
         tsBitrateP.ubBtr1 = CAN_BT1_50KB;
         break;
      case eBITRATE_100K:
         tsBitrateP.ubBtr0 = CAN_BT0_100KB;
         tsBitrateP.ubBtr1 = CAN_BT1_100KB;
         break;
      case eBITRATE_125K:
         tsBitrateP.ubBtr0 = CAN_BT0_125KB;
         tsBitrateP.ubBtr1 = CAN_BT1_125KB;
         break;
      case eBITRATE_250K:
         tsBitrateP.ubBtr0 = CAN_BT0_250KB;
         tsBitrateP.ubBtr1 = CAN_BT1_250KB;
         break;
      case eBITRATE_500K:
         tsBitrateP.ubBtr0 = CAN_BT0_500KB;
         tsBitrateP.ubBtr1 = CAN_BT1_500KB;
         break;
      case eBITRATE_800K:
         tsBitrateP.ubBtr0 = CAN_BT0_800KB;
         tsBitrateP.ubBtr1 = CAN_BT1_800KB;
         break;
      case eBITRATE_1M:
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

   return eERROR_OK;
}

//----------------------------------------------------------------------------//
// setMode()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t	QCanIxxatUsb::setMode(const Mode_te teModeV)
{
   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (!btLibFuncLoadP)
   {
      return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // select mode
   //
   switch (teModeV)
   {
      case eMODE_START :
         //---------------------------------------------------
         // reset statistic values
         //
         clStatisticP.ulErrCount = 0;
         clStatisticP.ulRcvCount = 0;
         clStatisticP.ulTrmCount = 0;

         pfnCanControlResetP(vdCanControlP);
         if (pfnCanControlInitializeP(vdCanControlP,
                                      (CAN_OPMODE_STANDARD |
                                       CAN_OPMODE_EXTENDED |
                                       CAN_OPMODE_ERRFRAME),
                                      tsBitrateP.ubBtr0,
                                      tsBitrateP.ubBtr1) != VCI_OK)
         {
            qWarning() << tr("WARNING: Fail to intialize control interface!");
         }

         if (pfnCanControlStartP(vdCanControlP,true) != VCI_OK)
         {
            qWarning() << tr("WARNING: Fail to start control interface!");
         }

         // open a can channel in NOT exlusive mode
         pfnCanChannelCloseP(vdCanChannelP);
         if (pfnCanChannelOpenP(vdCanInterfaceP,uwCanChannelP,false,&vdCanChannelP) != VCI_OK)
         {
            qWarning() << tr("WARNING: Fail to open a channel on seleceted interface!");
         }

         if (pfnCanChannelInitializeP(vdCanChannelP,64,1,64,1) != VCI_OK)
         {
            qWarning() << tr("WARNING: Fail to initialise CAN channel!");
         }

         if (pfnCanChannelActivateP(vdCanChannelP,true) != VCI_OK)
         {
            qWarning() << tr("WARNING: Fail to activate CAN channel!");
         }

         break;

      case eMODE_STOP :
         if (pfnCanChannelActivateP(vdCanChannelP,false) != VCI_OK)
         {
            qWarning() << tr("WARNING: Fail to deactivate CAN channel!");
         }
         break;

      default :
         return eERROR_MODE;
         break;
   }

   return eERROR_OK;
}

int32_t	QCanIxxatUsb::state(void)
{
   return 0;
}

void	QCanIxxatUsb::statistic(QCanStatistic_ts &clStatisticR)
{
   clStatisticR = clStatisticP;
}

//----------------------------------------------------------------------------//
// read()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t	QCanIxxatUsb::read(QCanFrame &clFrameR)
{
   CANMSG  tsCanMsgT;
   HRESULT slResultT;
   int32_t slByteCntrT;

   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (!btLibFuncLoadP)
   {
      return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // get next message in FIFO
   //
   slResultT = pfnCanChannelPeekMessageP(vdCanChannelP,&tsCanMsgT);

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
               clFrameR.setExtId(tsCanMsgT.dwMsgId);
            } else
            {
               clFrameR.setStdId((uint16_t)tsCanMsgT.dwMsgId);
            }

            clFrameR.setDlc(tsCanMsgT.uMsgInfo.Bits.dlc);

            for (slByteCntrT = 0; slByteCntrT < clFrameR.dlc(); slByteCntrT++)
            {
               clFrameR.setData(slByteCntrT, tsCanMsgT.abData[slByteCntrT]);
            }

            clStatisticP.ulRcvCount++;

            return eERROR_OK;

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
      qWarning() << tr("Fail to call PeekMessage(): ") + QString::number(slResultT,16);
      return eERROR_DEVICE;
   }


   return eERROR_FIFO_IN_EMPTY;
}

//----------------------------------------------------------------------------//
// write()                                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t	QCanIxxatUsb::write(const QCanFrame &clFrameR)
{
   CANMSG  tsCanMsgT;
   HRESULT slResultT;
   int32_t slByteCntrT;

   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (!btLibFuncLoadP)
   {
      return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // prepare CAN message
   //

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
   slResultT = pfnCanChannelPostMessageP(vdCanChannelP, &tsCanMsgT);

   if (slResultT == VCI_OK)
   {
      qDebug() << tr("PosMessage() OK");

      clStatisticP.ulTrmCount++;
      return eERROR_OK;
   }
   else if (slResultT != (HRESULT)VCI_E_TXQUEUE_FULL)
   {
      qWarning() << tr("Fail to call PeekMessage(): ") + QString::number(slResultT,16);
      return eERROR_DEVICE;
   }

   return eERROR_FIFO_OUT_FULL;

}

//----------------------------------------------------------------------------//
// disconnect()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t QCanIxxatUsb::connect(void)
{
   //-----------------------------------------------------------------------
   // Loads library
   //
   clCanLibP.setFileName(QCAN_IXXATLIB);

   if (!clCanLibP.load()) {
       qCritical() << tr("Failed to load the library: ") + qPrintable(clCanLibP.fileName());
       return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // Loads API functions
   //
   pfnVciEnumDeviceOpenP = (vciEnumDeviceOpen_tf) clCanLibP.resolve("vciEnumDeviceOpen");
   pfnVciEnumDeviceCloseP = (vciEnumDeviceClose_tf) clCanLibP.resolve("vciEnumDeviceClose");
   pfnVciEnumDeviceNextP = (vciEnumDeviceNext_tf) clCanLibP.resolve("vciEnumDeviceNext");
   pfnVciEnumDeviceResetP = (vciEnumDeviceReset_tf) clCanLibP.resolve("vciEnumDeviceReset");
   pfnVciEnumDeviceWaitEventP = (vciEnumDeviceWaitEvent_tf) clCanLibP.resolve("vciEnumDeviceWaitEvent");
   pfnVciFindDeviceByHwidP = (vciFindDeviceByHwid_tf) clCanLibP.resolve("vciFindDeviceByHwid");
   pfnVciFindDeviceByClassP = (vciFindDeviceByClass_tf) clCanLibP.resolve("vciFindDeviceByClass");
   pfnVciSelectDeviceDlgP = (vciSelectDeviceDlg_tf) clCanLibP.resolve("vciSelectDeviceDlg");

   pfnVciDeviceOpenP = (vciDeviceOpen_tf) clCanLibP.resolve("vciDeviceOpen");
   pfnVciDeviceOpenDlgP = (vciDeviceOpenDlg_tf) clCanLibP.resolve("vciDeviceOpenDlg");
   pfnVciDeviceCloseP = (vciDeviceClose_tf) clCanLibP.resolve("vciDeviceClose");
   pfnVciDeviceGetInfoP = (vciDeviceGetInfo_tf) clCanLibP.resolve("vciDeviceGetInfo");
   pfnVciDeviceGetCapsP = (vciDeviceGetCaps_tf) clCanLibP.resolve("vciDeviceGetCaps");

   pfnCanControlOpenP = (canControlOpen_tf) clCanLibP.resolve("canControlOpen");
   pfnCanControlCloseP = (canControlClose_tf) clCanLibP.resolve("canControlClose");
   pfnCanControlGetCapsP = (canControlGetCaps_tf) clCanLibP.resolve("canControlGetCaps");
   pfnCanControlGetStatusP = (canControlGetStatus_tf) clCanLibP.resolve("canControlGetStatus");
   pfnCanControlDetectBitrateP = (canControlDetectBitrate_tf) clCanLibP.resolve("canControlDetectBitrate");
   pfnCanControlInitializeP = (canControlInitialize_tf) clCanLibP.resolve("canControlInitialize");
   pfnCanControlResetP = (canControlReset_tf) clCanLibP.resolve("canControlReset");
   pfnCanControlStartP = (canControlStart_tf) clCanLibP.resolve("canControlStart");
   pfnCanControlSetAccFilterP = (canControlSetAccFilter_tf) clCanLibP.resolve("canControlSetAccFilter");
   pfnCanControlAddFilterIdsP = (canControlAddFilterIds_tf) clCanLibP.resolve("canControlAddFilterIds");
   pfnCanControlRemFilterIdsP = (canControlRemFilterIds_tf) clCanLibP.resolve("canControlRemFilterIds");

   pfnCanChannelOpenP = (canChannelOpen_tf) clCanLibP.resolve("canChannelOpen");
   pfnCanChannelCloseP = (canChannelClose_tf) clCanLibP.resolve("canChannelClose");
   pfnCanChannelGetCapsP = (canChannelGetCaps_tf) clCanLibP.resolve("canChannelGetCaps");
   pfnCanChannelGetStatusP = (canChannelGetStatus_tf) clCanLibP.resolve("canChannelGetStatus");
   pfnCanChannelInitializeP = (canChannelInitialize_tf) clCanLibP.resolve("canChannelInitialize");
   pfnCanChannelActivateP = (canChannelActivate_tf) clCanLibP.resolve("canChannelActivate");
   pfnCanChannelPeekMessageP = (canChannelPeekMessage_tf) clCanLibP.resolve("canChannelPeekMessage");
   pfnCanChannelPostMessageP = (canChannelPostMessage_tf) clCanLibP.resolve("canChannelPostMessage");
   pfnCanChannelWaitRxEventP = (canChannelWaitRxEvent_tf) clCanLibP.resolve("canChannelWaitRxEvent");
   pfnCanChannelWaitTxEventP = (canChannelWaitTxEvent_tf) clCanLibP.resolve("canChannelWaitTxEvent");
   pfnCanChannelReadMessageP = (canChannelReadMessage_tf) clCanLibP.resolve("canChannelReadMessage");
   pfnCanChannelSendMessageP = (canChannelSendMessage_tf) clCanLibP.resolve("canChannelSendMessage");
   pfnCanChannelPeekMultipleMessagesP = (canChannelPeekMultipleMessages_tf) clCanLibP.resolve("canChannelPeekMultipleMessages");
   pfnCanChannelPostMultipleMessagesP = (canChannelPostMultipleMessages_tf) clCanLibP.resolve("canChannelPostMultipleMessages");
   pfnCanChannelReadMultipleMessagesP = (canChannelReadMultipleMessages_tf) clCanLibP.resolve("canChannelReadMultipleMessages");
   pfnCanChannelSendMultipleMessagesP = (canChannelSendMultipleMessages_tf) clCanLibP.resolve("canChannelSendMultipleMessages");

   pfnCanSchedulerOpenP = (canSchedulerOpen_tf) clCanLibP.resolve("canSchedulerOpen");
   pfnCanSchedulerCloseP = (canSchedulerClose_tf) clCanLibP.resolve("canSchedulerClose");
   pfnCanSchedulerGetCapsP = (canSchedulerGetCaps_tf) clCanLibP.resolve("canSchedulerGetCaps");
   pfnCanSchedulerGetStatusP = (canSchedulerGetStatus_tf) clCanLibP.resolve("canSchedulerGetStatus");
   pfnCanSchedulerActivateP = (canSchedulerActivate_tf) clCanLibP.resolve("canSchedulerActivate");
   pfnCanSchedulerResetP = (canSchedulerReset_tf) clCanLibP.resolve("canSchedulerReset");
   pfnCanSchedulerAddMessageP = (canSchedulerAddMessage_tf) clCanLibP.resolve("canSchedulerAddMessage");
   pfnCanSchedulerRemMessageP = (canSchedulerRemMessage_tf) clCanLibP.resolve("canSchedulerRemMessage");
   pfnCanSchedulerStartMessageP = (canSchedulerStartMessage_tf) clCanLibP.resolve("canSchedulerStartMessage");
   pfnCanSchedulerStopMessageP = (canSchedulerStopMessage_tf) clCanLibP.resolve("canSchedulerStopMessage");

   //----------------------------------------------------------------
   // check for success
   //
   btLibFuncLoadP = (pfnVciEnumDeviceOpenP &&
                     pfnVciEnumDeviceCloseP &&
                     pfnVciEnumDeviceNextP &&
                     pfnVciEnumDeviceResetP &&
                     pfnVciEnumDeviceWaitEventP &&
                     pfnVciFindDeviceByHwidP &&
                     pfnVciFindDeviceByClassP &&
                     pfnVciSelectDeviceDlgP &&

                     pfnVciDeviceOpenP &&
                     pfnVciDeviceOpenDlgP &&
                     pfnVciDeviceCloseP &&
                     pfnVciDeviceGetInfoP &&
                     pfnVciDeviceGetCapsP &&

                     pfnCanControlOpenP &&
                     pfnCanControlCloseP &&
                     pfnCanControlGetCapsP &&
                     pfnCanControlGetStatusP &&
                     pfnCanControlDetectBitrateP &&
                     pfnCanControlInitializeP &&
                     pfnCanControlResetP &&
                     pfnCanControlStartP &&
                     pfnCanControlSetAccFilterP &&
                     pfnCanControlAddFilterIdsP &&
                     pfnCanControlRemFilterIdsP &&

                     pfnCanChannelOpenP &&
                     pfnCanChannelCloseP &&
                     pfnCanChannelGetCapsP &&
                     pfnCanChannelGetStatusP &&
                     pfnCanChannelInitializeP &&
                     pfnCanChannelActivateP &&
                     pfnCanChannelPeekMessageP &&
                     pfnCanChannelPostMessageP &&
                     pfnCanChannelWaitRxEventP &&
                     pfnCanChannelWaitTxEventP &&
                     pfnCanChannelReadMessageP &&
                     pfnCanChannelSendMessageP &&
                     pfnCanChannelPeekMultipleMessagesP &&
                     pfnCanChannelPostMultipleMessagesP &&
                     pfnCanChannelReadMultipleMessagesP &&
                     pfnCanChannelSendMultipleMessagesP &&

                     pfnCanSchedulerOpenP &&
                     pfnCanSchedulerCloseP &&
                     pfnCanSchedulerGetCapsP &&
                     pfnCanSchedulerGetStatusP &&
                     pfnCanSchedulerActivateP &&
                     pfnCanSchedulerResetP &&
                     pfnCanSchedulerAddMessageP &&
                     pfnCanSchedulerRemMessageP &&
                     pfnCanSchedulerStartMessageP &&
                     pfnCanSchedulerStopMessageP);

   //----------------------------------------------------------------
   // If the API was not loaded (Wrong version), an error message is shown.
   //
   if (!btLibFuncLoadP)
   {
      qCritical() << tr("ERROR: DLL functions could not be loaded!");
      return eERROR_LIBRARY;
   }

   qInfo() << tr("SUCCESS: All DLL functions could be loaded!");


   //----------------------------------------------------------------
   // Try to open device list
   //
   if (pfnVciEnumDeviceOpenP(&vdDevListP) != VCI_OK)
   {
      qWarning() << tr("WARNING: Can't open device list!") + QString::number(pfnVciEnumDeviceOpenP(&vdDevListP),16);
   }

   //----------------------------------------------------------------
   // Count number of connected devices
   //
   VCIDEVICEINFO clCanDevT;
   uint8_t ubDevCntrT = 0;
   while (pfnVciEnumDeviceNextP(vdDevListP,&clCanDevT) == VCI_OK)
   {
      ubDevCntrT++;
   }

   if (!ubDevCntrT)
   {
      qWarning() << tr("WARNING: No devices are connected!");
      return eERROR_DEVICE;
   }

   // create a device list for easy access
   pfnVciEnumDeviceResetP(vdDevListP);
   aclCanDevInfoP.resize(ubDevCntrT);
   while (ubDevCntrT)
   {
      ubDevCntrT--;

      pfnVciEnumDeviceNextP(vdDevListP,&aclCanDevInfoP[ubDevCntrT]);
      qInfo() << tr("INFO: Device '") + aclCanDevInfoP[ubDevCntrT].Description + tr("' found");
   }

   qInfo() << tr("INFO: Total number of found devices is ") + QString::number(aclCanDevInfoP.size(),10);

   //----------------------------------------------------------------
   // try to open a device
   //
   if (pfnVciDeviceOpenP(aclCanDevInfoP[uwCanDeviceP].VciObjectId,&vdCanInterfaceP) != VCI_OK)
   {
      qWarning() << tr("WARNING: Fail to open a device interface!");
      return eERROR_DEVICE;
   }

   return eERROR_OK;
}

//----------------------------------------------------------------------------//
// disconnect()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanIxxatUsb::disconnect(void)
{
   //-----------------------------------------------------------------------
   // If library have been loaded before close device list
   //
   if (btLibFuncLoadP)
   {
      pfnVciEnumDeviceCloseP(vdDevListP);
      aclCanDevInfoP.clear();
   }

   //-----------------------------------------------------------------------
   // Unload library and remove all API functions
   //
   if (clCanLibP.isLoaded())
   {
      clCanLibP.unload();
   }

   pfnVciEnumDeviceOpenP = NULL;
   pfnVciEnumDeviceCloseP = NULL;
   pfnVciEnumDeviceNextP = NULL;
   pfnVciEnumDeviceResetP = NULL;
   pfnVciEnumDeviceWaitEventP = NULL;
   pfnVciFindDeviceByHwidP = NULL;
   pfnVciFindDeviceByClassP = NULL;
   pfnVciSelectDeviceDlgP = NULL;

   pfnVciDeviceOpenP = NULL;
   pfnVciDeviceOpenDlgP = NULL;
   pfnVciDeviceCloseP = NULL;
   pfnVciDeviceGetInfoP = NULL;
   pfnVciDeviceGetCapsP = NULL;

   pfnCanControlOpenP = NULL;
   pfnCanControlCloseP = NULL;
   pfnCanControlGetCapsP = NULL;
   pfnCanControlGetStatusP = NULL;
   pfnCanControlDetectBitrateP = NULL;
   pfnCanControlInitializeP = NULL;
   pfnCanControlResetP = NULL;
   pfnCanControlStartP = NULL;
   pfnCanControlSetAccFilterP = NULL;
   pfnCanControlAddFilterIdsP = NULL;
   pfnCanControlRemFilterIdsP = NULL;

   pfnCanChannelOpenP = NULL;
   pfnCanChannelCloseP = NULL;
   pfnCanChannelGetCapsP = NULL;
   pfnCanChannelGetStatusP = NULL;
   pfnCanChannelInitializeP = NULL;
   pfnCanChannelActivateP = NULL;
   pfnCanChannelPeekMessageP = NULL;
   pfnCanChannelPostMessageP = NULL;
   pfnCanChannelWaitRxEventP = NULL;
   pfnCanChannelWaitTxEventP = NULL;
   pfnCanChannelReadMessageP = NULL;
   pfnCanChannelSendMessageP = NULL;
   pfnCanChannelPeekMultipleMessagesP = NULL;
   pfnCanChannelPostMultipleMessagesP = NULL;
   pfnCanChannelReadMultipleMessagesP = NULL;
   pfnCanChannelSendMultipleMessagesP = NULL;

   pfnCanSchedulerOpenP = NULL;
   pfnCanSchedulerCloseP = NULL;
   pfnCanSchedulerGetCapsP = NULL;
   pfnCanSchedulerGetStatusP = NULL;
   pfnCanSchedulerActivateP = NULL;
   pfnCanSchedulerResetP = NULL;
   pfnCanSchedulerAddMessageP = NULL;
   pfnCanSchedulerRemMessageP = NULL;
   pfnCanSchedulerStartMessageP = NULL;
   pfnCanSchedulerStopMessageP = NULL;

   btLibFuncLoadP = false;
}
