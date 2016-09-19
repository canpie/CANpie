//============================================================================//
// File:          qcan_peakusb.cpp                                            //
// Description:   CAN plugin for PEAK USB device                              //
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
#include <qcan_peakusb.hpp>

/*!
** \var aszErrTextG
** Zero terminated string buffer to print error descriptions
*/
char aszErrTextG[128];

//----------------------------------------------------------------------------//
// echo()                                                                     //
// dummy implementation of a function to evaluate string                      //
//----------------------------------------------------------------------------//
QString QCanPeakUsb::echo(const QString &message)
{
   uint32_t ulStatusT;
   QString clRetStringT;
   QCanFrame clFrameT;

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
         qWarning() << "Warning: read() fail with " << QString::number(slStatusT,10);
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

   if (message == "write_ext")
   {
      clFrameT.setExtId(175234);
      clFrameT.setDlc(8);
      clFrameT.setData(0,0x55);
      clFrameT.setData(1,0x44);
      clFrameT.setData(2,0x33);
      clFrameT.setData(3,0x22);
      clFrameT.setData(4,0x11);
      clFrameT.setData(5,0x14);
      clFrameT.setData(6,0x15);
      clFrameT.setData(7,0x16);

      int32_t slStatusT = write(clFrameT);

      if (slStatusT == eERROR_OK)
      {

      }
      else if (slStatusT != eERROR_FIFO_IN_EMPTY)
      {
         qWarning() << "Warning: write() fail!";
      }
   }

   return ("peak: " + message);
}
//! [0]

//----------------------------------------------------------------------------//
// setBitrate()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t QCanPeakUsb::setBitrate(uint32_t ulBitrateV, uint32_t ulBrsClockV)
{
   WORD uwBtr0Btr1T;

   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (!btLibFuncLoadP)
   {
      return eERROR_LIBRARY;
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
         uwBtr0Btr1T = PCAN_BAUD_10K;
         break;
      case eBITRATE_20K:
         uwBtr0Btr1T = PCAN_BAUD_20K;
         break;
      case eBITRATE_50K:
         uwBtr0Btr1T = PCAN_BAUD_50K;
         break;
      case eBITRATE_100K:
         uwBtr0Btr1T = PCAN_BAUD_100K;
         break;
      case eBITRATE_125K:
         uwBtr0Btr1T = PCAN_BAUD_125K;
         break;
      case eBITRATE_250K:
         uwBtr0Btr1T = PCAN_BAUD_250K;
         break;
      case eBITRATE_500K:
         uwBtr0Btr1T = PCAN_BAUD_500K;
         break;
      case eBITRATE_800K:
         uwBtr0Btr1T = PCAN_BAUD_800K;
         break;
      case eBITRATE_1M:
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

   //----------------------------------------------------------------
   // perform initalisation CAN Interface
   //
   pfnCAN_UninitializeP(uwCanChannelP);
   uint32_t ulStatusT = pfnCAN_InitializeP(uwCanChannelP, uwBtr0Btr1T, 0, 0, 0);
   if (ulStatusT != PCAN_ERROR_OK)
   {
      // get default description string of error code
      pfnCAN_GetErrorTextP(ulStatusT,0x00,aszErrTextG);
      qCritical() << "ERROR: " << QString(QLatin1String(aszErrTextG));

      return eERROR_DEVICE;
   }

   return eERROR_OK;
}


//----------------------------------------------------------------------------//
// setMode()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t	QCanPeakUsb::setMode(const Mode_te teModeV)
{
   TPCANStatus ulStatusT;
   uint8_t ubValueBufT;

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

         ubValueBufT = 1;
         if (pfnCAN_SetValueP(uwCanChannelP,PCAN_BUSOFF_AUTORESET,&ubValueBufT,1))
         {
            qWarning() << tr("WARNING: Fail to set AutoReset of Device!");
         }
         break;

      case eMODE_STOP :

         break;

      default :
         return eERROR_MODE;
         break;
   }

   return eERROR_OK;
}


int32_t	QCanPeakUsb::state(void)
{
   return 0;
}

//----------------------------------------------------------------------------//
// statistic()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
void	QCanPeakUsb::statistic(QCanStatistic_ts &clStatisticR)
{
   clStatisticR = clStatisticP;
}

QIcon QCanPeakUsb::icon()
{
   return QIcon(":/qcan_peakusb.png");
}

QString QCanPeakUsb::name()
{
   return "PEAK USB device";
}

//----------------------------------------------------------------------------//
// read()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t	QCanPeakUsb::read(QCanFrame &clFrameR)
{
   TPCANStatus ulStatusT;
   int32_t slByteCntrT;
   TPCANMsg tsCanMsgT;
   TPCANTimestamp tsTimestampBufferT;

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
   ulStatusT = pfnCAN_ReadP(uwCanChannelP, &tsCanMsgT, &tsTimestampBufferT);

   if (ulStatusT == PCAN_ERROR_OK)
   {
      // handle data depending on type
      switch (tsCanMsgT.MSGTYPE)
      {
         case PCAN_MESSAGE_STANDARD :
            qDebug() << tr("handle PCAN_MESSAGE_STANDARD");

            // copy all needed parameters to QCanFrame structure
            clFrameR.setStdId((uint16_t)tsCanMsgT.ID);

            clFrameR.setDlc(tsCanMsgT.LEN);

            for (slByteCntrT = 0; slByteCntrT < clFrameR.dlc(); slByteCntrT++)
            {
               clFrameR.setData(slByteCntrT, tsCanMsgT.DATA[slByteCntrT]);
            }

            clStatisticP.ulRcvCount++;

            return eERROR_OK;

            break;

         case PCAN_MESSAGE_EXTENDED :
            qDebug() << tr("handle PCAN_MESSAGE_EXTENDED");

            // copy all needed parameters to QCanFrame structure
            clFrameR.setExtId(tsCanMsgT.ID);

            clFrameR.setDlc(tsCanMsgT.LEN);

            for (slByteCntrT = 0; slByteCntrT < clFrameR.dlc(); slByteCntrT++)
            {
               clFrameR.setData(slByteCntrT, tsCanMsgT.DATA[slByteCntrT]);
            }
            break;

         case PCAN_MESSAGE_RTR :
            qDebug() << tr("handle PCAN_MESSAGE_RTR");
            break;

         case PCAN_MESSAGE_FD :
            qDebug() << tr("handle PCAN_MESSAGE_FD");
            break;

         case PCAN_MESSAGE_BRS :
            qDebug() << tr("handle PCAN_MESSAGE_BRS");
            break;

         case PCAN_MESSAGE_ESI :
            qDebug() << tr("handle PCAN_MESSAGE_ESI");
            break;

         case PCAN_MESSAGE_STATUS :
            qDebug() << tr("handle PCAN_MESSAGE_STATUS");
            break;

         default :
            qDebug() << tr("UNKNOWN Message Type");
            break;
      }
   }

   else if (ulStatusT != (TPCANStatus)PCAN_ERROR_QRCVEMPTY)
   {
      qWarning() << tr("Fail to call CAN_Read(): ") + QString::number(ulStatusT,16);
      return eERROR_DEVICE;
   }


   return eERROR_FIFO_IN_EMPTY;

}

//----------------------------------------------------------------------------//
// write()                                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t	QCanPeakUsb::write(const QCanFrame &clFrameR)
{
   TPCANStatus ulStatusT;
   TPCANMsg    tsCanMsgT;
   int32_t     slByteCntrT;

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
      tsCanMsgT.MSGTYPE = PCAN_MESSAGE_EXTENDED;
   } else
   {
      tsCanMsgT.MSGTYPE = PCAN_MESSAGE_STANDARD;
   }

   tsCanMsgT.ID = clFrameR.identifier();

   tsCanMsgT.LEN= clFrameR.dlc();

   for (slByteCntrT = 0; slByteCntrT < clFrameR.dlc(); slByteCntrT++)
   {
      tsCanMsgT.DATA[slByteCntrT] = clFrameR.data(slByteCntrT);
   }

   ulStatusT = pfnCAN_WriteP(uwCanChannelP, &tsCanMsgT);

   if (ulStatusT == PCAN_ERROR_OK)
   {
      qDebug() << tr("CAN_Write() OK");

      clStatisticP.ulTrmCount++;
      return eERROR_OK;
   }
   else if (ulStatusT != (TPCANStatus)PCAN_ERROR_QRCVEMPTY)
   {
      qWarning() << tr("Fail to call CAN_Write(): ") + QString::number(ulStatusT,16);
      return eERROR_DEVICE;
   }

   return eERROR_FIFO_OUT_FULL;

}


//----------------------------------------------------------------------------//
// connect()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t QCanPeakUsb::connect(void)
{
   //-----------------------------------------------------------------------
   // Loads library
   //
   clCanLibP.setFileName(QCAN_PEAKLIB);

   if (!clCanLibP.load()) {
       qCritical() << tr("Failed to load the library: ") + qPrintable(clCanLibP.fileName());
       return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // Loads API functions
   //
   pfnCAN_InitializeP = (CAN_Initialize_tf) clCanLibP.resolve("CAN_Initialize");
   pfnCAN_InitializeFDP = (CAN_InitializeFD_tf) clCanLibP.resolve("CAN_InitializeFD");
   pfnCAN_UninitializeP = (CAN_Uninitialize_tf)clCanLibP.resolve("CAN_Uninitialize");
   pfnCAN_ResetP = (CAN_Reset_tf)clCanLibP.resolve("CAN_Reset");
   pfnCAN_GetStatusP = (CAN_GetStatus_tf)clCanLibP.resolve("CAN_GetStatus");
   pfnCAN_ReadP = (CAN_Read_tf)clCanLibP.resolve("CAN_Read");
   pfnCAN_ReadFDP = (CAN_ReadFD_tf)clCanLibP.resolve("CAN_ReadFD");
   pfnCAN_WriteP = (CAN_Write_tf)clCanLibP.resolve("CAN_Write");
   pfnCAN_WriteFDP = (CAN_WriteFD_tf)clCanLibP.resolve("CAN_WriteFD");
   pfnCAN_FilterMessagesP = (CAN_FilterMessages_tf)clCanLibP.resolve("CAN_FilterMessages");
   pfnCAN_GetValueP = (CAN_GetValue_tf)clCanLibP.resolve("CAN_GetValue");
   pfnCAN_SetValueP = (CAN_SetValue_tf)clCanLibP.resolve("CAN_SetValue");
   pfnCAN_GetErrorTextP = (CAN_GetErrorText_tf)clCanLibP.resolve("CAN_GetErrorText");


   //----------------------------------------------------------------
   // check for success
   //
   btLibFuncLoadP =  pfnCAN_InitializeP && pfnCAN_InitializeFDP &&
                     pfnCAN_UninitializeP && pfnCAN_ResetP &&
                     pfnCAN_GetStatusP && pfnCAN_ReadP  &&
                     pfnCAN_ReadFDP && pfnCAN_WriteP &&
                     pfnCAN_WriteFDP && pfnCAN_FilterMessagesP &&
                     pfnCAN_GetValueP && pfnCAN_SetValueP &&
                     pfnCAN_GetErrorTextP;

   //----------------------------------------------------------------
   // If the API was not loaded (Wrong version), an error message is shown.
   //
   if (!btLibFuncLoadP)
   {
      qCritical() << tr("ERROR: DLL functions could not be loaded!");
      return eERROR_LIBRARY;
   }

   qInfo() << tr("SUCCESS: All DLL functions could be loaded!");
   return eERROR_OK;
}

//----------------------------------------------------------------------------//
// disconnect()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanPeakUsb::disconnect(void)
{
   //-----------------------------------------------------------------------
   // If library have been loaded before unintitialize can device
   //
   if (btLibFuncLoadP)
   {
      pfnCAN_UninitializeP(uwCanChannelP);
   }

   //-----------------------------------------------------------------------
   // Unload library and remove all API functions
   //
   if (clCanLibP.isLoaded())
   {
      clCanLibP.unload();
   }

   pfnCAN_InitializeP = NULL;
   pfnCAN_InitializeFDP = NULL;
   pfnCAN_UninitializeP = NULL;
   pfnCAN_ResetP = NULL;
   pfnCAN_GetStatusP = NULL;
   pfnCAN_ReadP = NULL;
   pfnCAN_ReadFDP = NULL;
   pfnCAN_WriteP = NULL;
   pfnCAN_WriteFDP = NULL;
   pfnCAN_FilterMessagesP = NULL;
   pfnCAN_GetValueP = NULL;
   pfnCAN_SetValueP = NULL;
   pfnCAN_GetErrorTextP = NULL;

   btLibFuncLoadP = false;
}
