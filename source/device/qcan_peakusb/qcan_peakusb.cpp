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

#include "qcan_defs.hpp"
#include "qcan_peakusb.hpp"


//----------------------------------------------------------------------------//
// QCanPeakUsb()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
QCanPeakUsb::QCanPeakUsb()
{
   CanChannel_ts tsCanChnT;

   qDebug() << "Constructor: prepare for PEAKUSB Library...";

   btLibFuncLoadP = false;
   ubChannelP = 0;
   atsCanChannelP.clear();

   //----------------------------------------------------------------
   // add all channels
   //
   tsCanChnT.uwChannel = PCAN_USBBUS1;
   tsCanChnT.btAvailable = false;
   tsCanChnT.btConnected = false;
   tsCanChnT.ubNumber = 0;
   tsCanChnT.clName.clear();
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS2;
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS3;
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS4;
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS5;
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS6;
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS7;
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS8;
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS9;
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS10;
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS11;
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS12;
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS13;
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS14;
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS15;
   atsCanChannelP.append(tsCanChnT);
   tsCanChnT.uwChannel = PCAN_USBBUS16;
   atsCanChannelP.append(tsCanChnT);


   // connect to library and check the state for each channel
   for (uint8_t ubCntrT = 0; ubCntrT < atsCanChannelP.length(); ubCntrT++)
   {
      if(connect(ubCntrT) == eERROR_NONE)
      {
         if (setBitrate(QCan::eCAN_BITRATE_500K, 0,ubCntrT) == eERROR_NONE)
         {

            TPCANStatus tsStatusT;
            uint8_t     ubParmBufferT;
            char        aszBufferT[64];

            // get device number of channel
            tsStatusT = pfnCAN_GetValueP(atsCanChannelP[ubCntrT].uwChannel, PCAN_DEVICE_NUMBER, (void*)&ubParmBufferT, sizeof(ubParmBufferT));

            if (tsStatusT != PCAN_ERROR_OK)
            {
               qCritical() << "FAIL to get parameter PCAN_DEVICE_NUMBER with error [hex]:" << QString::number(tsStatusT,16 );
            } else
            {
               qDebug() << "Channel" << QString::number(ubCntrT,10) << "PCAN_DEVICE_NUMBER:" << QString::number(ubParmBufferT,10);
               atsCanChannelP[ubCntrT].ubNumber = ubParmBufferT;
            }


            // get name of channel
            tsStatusT = pfnCAN_GetValueP(atsCanChannelP[ubCntrT].uwChannel, PCAN_HARDWARE_NAME, (void*)&aszBufferT[0], sizeof(aszBufferT));

            if (tsStatusT != PCAN_ERROR_OK)
            {
               qDebug() << "FAIL to get parameter PCAN_HARDWARE_NAME with error [hex]:" << QString::number(tsStatusT,16 );
            } else
            {
               qDebug() << "Channel" << QString::number(ubCntrT,10) << "PCAN_HARDWARE_NAME:" << QLatin1String(aszBufferT);
               atsCanChannelP[ubCntrT].clName.append(QLatin1String(aszBufferT));
            }

            // increment total number of available channels
            ubChannelP++;
         }
      }
   }

   // disconnect all channels from library
   for (uint8_t ubCntrT = 0; ubCntrT < atsCanChannelP.length(); ubCntrT++)
   {
      if (disconnect(ubCntrT) != eERROR_NONE)
      {
         break;
      }
   }

}

//----------------------------------------------------------------------------//
// ~QCanPeakUsb()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
QCanPeakUsb::~QCanPeakUsb()
{
   qDebug() << "Destructor: release PEAKUSB Library...";

   // disconnect all channels from library
   for (uint8_t ubCntrT = 0; ubCntrT < atsCanChannelP.length(); ubCntrT++)
   {
      if (disconnect(ubCntrT) != eERROR_NONE)
      {
         break;
      }
   }

   //-----------------------------------------------------------------------
   // Unload library and remove all API functions
   //
   if (clCanLibP.isLoaded())
   {
      clCanLibP.unload();
   }

   pfnCAN_InitializeP = NULL;
   pfnCAN_UninitializeP = NULL;
   pfnCAN_ResetP = NULL;
   pfnCAN_GetStatusP = NULL;
   pfnCAN_ReadP = NULL;
   pfnCAN_WriteP = NULL;
   pfnCAN_FilterMessagesP = NULL;
   pfnCAN_GetValueP = NULL;
   pfnCAN_SetValueP = NULL;
   pfnCAN_GetErrorTextP = NULL;

   #if QCAN_SUPPORT_CAN_FD > 0
   pfnCAN_InitializeFDP = NULL;
   pfnCAN_ReadFDP = NULL;
   pfnCAN_WriteFDP = NULL;
   #endif
   btLibFuncLoadP = false;

}

//----------------------------------------------------------------------------//
// channels()                                                                 //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t QCanPeakUsb::channel()
{
   return ubChannelP;
}

//----------------------------------------------------------------------------//
// connect()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanPeakUsb::connect(uint8_t ubChannelV)
{
   //-----------------------------------------------------------------------
   // Load library only if it hase not been laoded yet
   //
   if (!btLibFuncLoadP)
   {
      // get file name
      clCanLibP.setFileName(QCAN_PEAKLIB);

      if (!clCanLibP.load()) {
          qCritical() << "Failed to load the library:" << qPrintable(clCanLibP.fileName());
          return eERROR_LIBRARY;
      }

      //----------------------------------------------------------------
      // Loads API functions
      //
      pfnCAN_InitializeP = (CAN_Initialize_tf) clCanLibP.resolve("CAN_Initialize");
      #if QCAN_SUPPORT_CAN_FD > 0
      pfnCAN_InitializeFDP = (CAN_InitializeFD_tf) clCanLibP.resolve("CAN_InitializeFD");
      #endif
      pfnCAN_UninitializeP = (CAN_Uninitialize_tf)clCanLibP.resolve("CAN_Uninitialize");
      pfnCAN_ResetP = (CAN_Reset_tf)clCanLibP.resolve("CAN_Reset");
      pfnCAN_GetStatusP = (CAN_GetStatus_tf)clCanLibP.resolve("CAN_GetStatus");
      pfnCAN_ReadP = (CAN_Read_tf)clCanLibP.resolve("CAN_Read");
      #if QCAN_SUPPORT_CAN_FD > 0
      pfnCAN_ReadFDP = (CAN_ReadFD_tf)clCanLibP.resolve("CAN_ReadFD");
      #endif
      pfnCAN_WriteP = (CAN_Write_tf)clCanLibP.resolve("CAN_Write");
      #if QCAN_SUPPORT_CAN_FD > 0
      pfnCAN_WriteFDP = (CAN_WriteFD_tf)clCanLibP.resolve("CAN_WriteFD");
      #endif
      pfnCAN_FilterMessagesP = (CAN_FilterMessages_tf)clCanLibP.resolve("CAN_FilterMessages");
      pfnCAN_GetValueP = (CAN_GetValue_tf)clCanLibP.resolve("CAN_GetValue");
      pfnCAN_SetValueP = (CAN_SetValue_tf)clCanLibP.resolve("CAN_SetValue");
      pfnCAN_GetErrorTextP = (CAN_GetErrorText_tf)clCanLibP.resolve("CAN_GetErrorText");


      //----------------------------------------------------------------
      // check for success
      //
      btLibFuncLoadP =  pfnCAN_InitializeP &&
                        #if QCAN_SUPPORT_CAN_FD > 0
                        pfnCAN_InitializeFDP &&
                        #endif
                        pfnCAN_UninitializeP && pfnCAN_ResetP &&
                        pfnCAN_GetStatusP && pfnCAN_ReadP  &&
                        pfnCAN_WriteP &&
                        #if QCAN_SUPPORT_CAN_FD > 0
                        pfnCAN_FilterMessagesP &&
                        pfnCAN_ReadFDP && pfnCAN_WriteFDP &&
                        #endif
                        pfnCAN_GetValueP && pfnCAN_SetValueP &&
                        pfnCAN_GetErrorTextP;

      //----------------------------------------------------------------
      // If the API was not loaded (Wrong version), an error message is shown.
      //
      if (!btLibFuncLoadP)
      {
         qCritical() << "ERROR: DLL functions could not be loaded!";
         return eERROR_LIBRARY;
      }

      qInfo() << "SUCCESS: All DLL functions could be loaded!";
   }
   else
   {
      qInfo() << "INFO: Library have been already loaded!";
   }

   //----------------------------------------------------------------
   // connect only not connected channel
   //
   if (ubChannelV >= atsCanChannelP.length())
   {
      qCritical() << "FAIL: Channel" << QString::number(ubChannelV,10) << "is out of range!";
      return eERROR_CHANNEL;
   }

   //----------------------------------------------------------------
   // check channels for connection
   //
   if (!atsCanChannelP[ubChannelV].btAvailable &&
       !atsCanChannelP[ubChannelV].btConnected)
   {
      TPCANStatus tsStatusT;
      uint32_t    ulParmBufferT;

      tsStatusT = pfnCAN_GetValueP(atsCanChannelP[ubChannelV].uwChannel, PCAN_CHANNEL_CONDITION, (void*)&ulParmBufferT, sizeof(ulParmBufferT));
      if (tsStatusT != PCAN_ERROR_OK)
      {
         qDebug() << formatedError(tsStatusT);
      } else if ((ulParmBufferT & PCAN_CHANNEL_AVAILABLE) == PCAN_CHANNEL_AVAILABLE)
      {
         qInfo() << "SUCCESS: Channel" << QString::number(ubChannelV,10) << "is available!";
         atsCanChannelP[ubChannelV].btAvailable = true;
      } else
      {
         qWarning() << "WARNING: Channel" << QString::number(ubChannelV,10) << "is NOT available!";
         return eERROR_CHANNEL;
      }
   } else
   {
      qInfo() << "INFO: Channel" << QString::number(ubChannelV,10) << "is already masked as available or is already connected!";
   }

   return eERROR_NONE;
}


//----------------------------------------------------------------------------//
// disconnect()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanPeakUsb::disconnect(uint8_t ubChannelV)
{
   //-----------------------------------------------------------------------
   //
   //
   if (ubChannelV >= atsCanChannelP.length())
   {
      qCritical() << "FAIL: Channel" << QString::number(ubChannelV,10) << "is out of range!";
      return eERROR_CHANNEL;
   }

   //-----------------------------------------------------------------------
   // unintitialize can channels
   //
   if (btLibFuncLoadP)
   {
      if (atsCanChannelP[ubChannelV].btConnected)
      {
         pfnCAN_UninitializeP(atsCanChannelP[ubChannelV].uwChannel);
         atsCanChannelP[ubChannelV].btConnected = false;
      }
   }

   //-----------------------------------------------------------------------
   // if one of channels is connected,  than we return here and do not
   // disconnect the the lib
   //
   for (ubChannelV = 0; ubChannelV < atsCanChannelP.length(); ubChannelV++)
   {
      if (atsCanChannelP[ubChannelV].btConnected)
      {
         qInfo() << "Channel" << QString::number(ubChannelV,10) << "is still connected, so lib will be not disconnected!";
         return eERROR_LIBRARY;
      }
   }

   //-----------------------------------------------------------------------
   // Unload library and remove all API functions
   //
   if (clCanLibP.isLoaded())
   {
      clCanLibP.unload();
   }

   pfnCAN_InitializeP = NULL;
   pfnCAN_UninitializeP = NULL;
   pfnCAN_ResetP = NULL;
   pfnCAN_GetStatusP = NULL;
   pfnCAN_ReadP = NULL;
   pfnCAN_WriteP = NULL;
   pfnCAN_FilterMessagesP = NULL;
   pfnCAN_GetValueP = NULL;
   pfnCAN_SetValueP = NULL;
   pfnCAN_GetErrorTextP = NULL;

   #if QCAN_SUPPORT_CAN_FD > 0
   pfnCAN_InitializeFDP = NULL;
   pfnCAN_ReadFDP = NULL;
   pfnCAN_WriteFDP = NULL;
   #endif
   btLibFuncLoadP = false;

   return eERROR_NONE;
}

//----------------------------------------------------------------------------//
// icon()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QIcon QCanPeakUsb::icon(uint8_t ubChannelV)
{
   //----------------------------------------------------------------
   // check channel have been connected
   //
   if (ubChannelV >= atsCanChannelP.length())
   {
       return QIcon(":/qcan_peakusb.png");
   }

   //----------------------------------------------------------------
   // check channel is avilable
   //
   if (!atsCanChannelP[ubChannelV].btAvailable)
   {
       return QIcon(":/qcan_peakusb.png");
   }

   QString clIconNameT = atsCanChannelP[ubChannelV].clName;
   clIconNameT = clIconNameT.toLower();
   clIconNameT.replace(QString("-"),QString("_"));
   clIconNameT.replace(QString(" "),QString("_"));

   qDebug() << " return icon: " << QString(":/images/"+clIconNameT+"_icon.ico");

   return QIcon(":/images/"+clIconNameT+"_icon.ico");
}


//----------------------------------------------------------------------------//
// name()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QString QCanPeakUsb::name(uint8_t ubChannelV)
{
   //----------------------------------------------------------------
   // check channel have been connected
   //
   if (ubChannelV >= atsCanChannelP.length())
   {
      return "FAIL: Channel out of range";
   }

   //----------------------------------------------------------------
   // check channel is avilable
   //
   if (!atsCanChannelP[ubChannelV].btAvailable)
   {
      return "FAIL: Channel is not available";
   }

   return QString(atsCanChannelP[ubChannelV].clName +" "+ QString::number(atsCanChannelP[ubChannelV].ubNumber,16).toUpper() + "h [" + QString::number(ubChannelV+1,10) + "]");
}



//----------------------------------------------------------------------------//
// read()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanPeakUsb::read(QCanFrame &clFrameR,
                                                   uint8_t ubChannelV)
{
   TPCANStatus    ulStatusT;
   uint8_t        ubByteCntrT;
   TPCANMsg       tsCanMsgT;
   TPCANTimestamp tsTimestampBufferT;

   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (!btLibFuncLoadP)
   {
      return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // check channel have been connected
   //
   if (ubChannelV >= atsCanChannelP.length())
   {
      return eERROR_CHANNEL;
   }

   //----------------------------------------------------------------
   // get next message from FIFO
   //
   ulStatusT = pfnCAN_ReadP(atsCanChannelP[ubChannelV].uwChannel, &tsCanMsgT, &tsTimestampBufferT);

   if (ulStatusT == PCAN_ERROR_OK)
   {
      //--------------------------------------------------------
      // handle data depending on type
      //
      switch (tsCanMsgT.MSGTYPE)
      {
         case PCAN_MESSAGE_STANDARD :

            //---------------------------------------------
            // copy all values to QCanFrame structure
            //
            clFrameR.setFrameType(QCanFrame::eTYPE_CAN_STD);
            clFrameR.setStdId((uint16_t)tsCanMsgT.ID);

            clFrameR.setDlc(tsCanMsgT.LEN);

            for (ubByteCntrT = 0; ubByteCntrT < clFrameR.dlc(); ubByteCntrT++)
            {
               clFrameR.setData(ubByteCntrT, tsCanMsgT.DATA[ubByteCntrT]);
            }

            clStatisticP.ulRcvCount++;

            return eERROR_NONE;

            break;

         case PCAN_MESSAGE_EXTENDED :

            //---------------------------------------------
            // copy all values to QCanFrame structure
            //
            clFrameR.setFrameType(QCanFrame::eTYPE_CAN_EXT);
            clFrameR.setExtId(tsCanMsgT.ID);

            clFrameR.setDlc(tsCanMsgT.LEN);

            for (ubByteCntrT = 0; ubByteCntrT < clFrameR.dlc(); ubByteCntrT++)
            {
               clFrameR.setData(ubByteCntrT, tsCanMsgT.DATA[ubByteCntrT]);
            }
            return eERROR_NONE;
            break;

         case PCAN_MESSAGE_RTR :
            qDebug() << "handle PCAN_MESSAGE_RTR";
            break;
         #if QCAN_SUPPORT_CAN_FD > 0
         case PCAN_MESSAGE_FD :
            qDebug() << "handle PCAN_MESSAGE_FD";
            break;

         case PCAN_MESSAGE_BRS :
            qDebug() << "handle PCAN_MESSAGE_BRS";
            break;

         case PCAN_MESSAGE_ESI :
            qDebug() << "handle PCAN_MESSAGE_ESI";
            break;
         #endif
         case PCAN_MESSAGE_STATUS :
            qDebug() << "handle PCAN_MESSAGE_STATUS";
            break;

         default :
            qDebug() << "UNKNOWN Message Type";
            break;
      }
   }

   else if (ulStatusT != (TPCANStatus)PCAN_ERROR_QRCVEMPTY)
   {
      qWarning() << "Fail to call CAN_Read():" + QString::number(ulStatusT,16);
      return eERROR_DEVICE;
   }


   return eERROR_FIFO_RCV_EMPTY;

}


//----------------------------------------------------------------------------//
// setBitrate()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanPeakUsb::setBitrate( int32_t slBitrateV,
                                                         int32_t slBrsClockV,
                                                         uint8_t ubChannelV)
{
   WORD uwBtr0Btr1T;

   //----------------------------------------------------------------
   // check if library has been loaded
   //
   if (!btLibFuncLoadP)
   {
      return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // check channel have been connected
   //
   if (ubChannelV >= atsCanChannelP.length())
   {
      return eERROR_CHANNEL;
   }

   //----------------------------------------------------------------
   // Check bit-rate value
   //
   if (slBitrateV >= QCan::eCAN_BITRATE_AUTO)
   {
      // maybe we get a value in Hz, normalise it to kHz
      slBitrateV = slBitrateV / 1000;
   }

   //----------------------------------------------------------------
   // Check BRS clock value for CAN-FD
   // todo : test with CAN-FD interface
   //
   if (slBrsClockV != QCan::eCAN_BITRATE_NONE)
   {

   }

   //----------------------------------------------------------------
   // select corresponding PEAK baud rate value
   //
   switch(slBitrateV)
   {
      // value from CANpie enumeration
      case QCan::eCAN_BITRATE_10K:
         uwBtr0Btr1T = PCAN_BAUD_10K;
         break;
      case QCan::eCAN_BITRATE_20K:
         uwBtr0Btr1T = PCAN_BAUD_20K;
         break;
      case QCan::eCAN_BITRATE_50K:
         uwBtr0Btr1T = PCAN_BAUD_50K;
         break;
      case QCan::eCAN_BITRATE_100K:
         uwBtr0Btr1T = PCAN_BAUD_100K;
         break;
      case QCan::eCAN_BITRATE_125K:
         uwBtr0Btr1T = PCAN_BAUD_125K;
         break;
      case QCan::eCAN_BITRATE_250K:
         uwBtr0Btr1T = PCAN_BAUD_250K;
         break;
      case QCan::eCAN_BITRATE_500K:
         uwBtr0Btr1T = PCAN_BAUD_500K;
         break;
      case QCan::eCAN_BITRATE_800K:
         uwBtr0Btr1T = PCAN_BAUD_800K;
         break;
      case QCan::eCAN_BITRATE_1M:
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
   pfnCAN_UninitializeP(atsCanChannelP[ubChannelV].uwChannel);
   uint32_t ulStatusT = pfnCAN_InitializeP(atsCanChannelP[ubChannelV].uwChannel, uwBtr0Btr1T, 0, 0, 0);
   if (ulStatusT != PCAN_ERROR_OK)
   {
      // get default description string of error code
      qCritical() << formatedError(ulStatusT);

      return eERROR_DEVICE;
   }

   return eERROR_NONE;
}


//----------------------------------------------------------------------------//
// setMode()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e	QCanPeakUsb::setMode(const QCan::CAN_Mode_e teModeV,
                                                      uint8_t ubChannelV)
{
   // TPCANStatus ulStatusT;
   uint8_t ubValueBufT;

   //----------------------------------------------------------------
   // check lib have been loaded
   //
   if (!btLibFuncLoadP)
   {
      return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // check channel have been connected
   //
   if (ubChannelV >= atsCanChannelP.length())
   {
      return eERROR_CHANNEL;
   }

   //----------------------------------------------------------------
   // select mode
   //
   switch (teModeV)
   {
      case QCan::eCAN_MODE_START :

         //---------------------------------------------------
         // reset statistic values
         //
         clStatisticP.ulErrCount = 0;
         clStatisticP.ulRcvCount = 0;
         clStatisticP.ulTrmCount = 0;

         ubValueBufT = 1;
         if (pfnCAN_SetValueP(atsCanChannelP[ubChannelV].uwChannel,PCAN_BUSOFF_AUTORESET,&ubValueBufT,sizeof(ubValueBufT)))
         {
            qWarning() << "WARNING: Fail to set AutoReset of Device!";
         }
         break;

      case QCan::eCAN_MODE_STOP :

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
QCan::CAN_State_e	QCanPeakUsb::state(uint8_t ubChannelV)
{
   //----------------------------------------------------------------
   // check channel have been connected
   //
   if (ubChannelV >= atsCanChannelP.length())
   {
      return QCan::eCAN_STATE_STOPPED;
   }


   return QCan::eCAN_STATE_BUS_ACTIVE;
}

//----------------------------------------------------------------------------//
// statistic()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e	QCanPeakUsb::statistic( QCanStatistic_ts &clStatisticR,
                                                         uint8_t ubChannelV)
{
   //----------------------------------------------------------------
   // check channel have been connected
   //
   if (ubChannelV >= atsCanChannelP.length())
   {
      return eERROR_CHANNEL;
   }


   clStatisticR = clStatisticP;

   return(eERROR_NONE);
}


//----------------------------------------------------------------------------//
// supportedFeatures()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
uint32_t QCanPeakUsb::supportedFeatures(uint8_t ubChannelV)
{
   uint32_t ulFeaturesT;

   //----------------------------------------------------------------
   // check channel have been connected
   //
   if (ubChannelV >= atsCanChannelP.length())
   {
      return eERROR_CHANNEL;
   }

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
QCanInterface::InterfaceError_e	QCanPeakUsb::write(const QCanFrame &clFrameR,
                                                    uint8_t ubChannelV)
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
   // check channel have been connected
   //
   if (ubChannelV >= atsCanChannelP.length())
   {
      return eERROR_CHANNEL;
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

   ulStatusT = pfnCAN_WriteP(atsCanChannelP[ubChannelV].uwChannel, &tsCanMsgT);

   if (ulStatusT == PCAN_ERROR_OK)
   {
      clStatisticP.ulTrmCount++;
      return eERROR_NONE;
   }
   else if (ulStatusT != (TPCANStatus)PCAN_ERROR_QRCVEMPTY)
   {
      qWarning() << "Fail to call CAN_Write():" + QString::number(ulStatusT,16);
      return eERROR_DEVICE;
   }

   return eERROR_FIFO_TRM_FULL;

}

//----------------------------------------------------------------------------//
// formatedError()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
QString QCanPeakUsb::formatedError(TPCANStatus tvErrorV)
{
   TPCANStatus tvStatusT;
   char aszBufferT[256];
   QString clResultT;

   memset(aszBufferT,'\0',256);

   // Gets the text using the GetErrorText API function
   // If the function success, the translated error is returned. If it fails,
   // a text describing the current error is returned.
   //
//qCritical() << "ERROR: " << QString(QLatin1String(aszBufferT));

   tvStatusT = pfnCAN_GetErrorTextP(tvErrorV,0x00,aszBufferT);
   if(tvStatusT != PCAN_ERROR_OK)
   {
      clResultT = ("An error ocurred. Error-code's text ("+ QString::number(tvErrorV,16).toUpper() + "h) couldn't be retrieved");
   } else
   {
      clResultT = QLatin1String(aszBufferT);
   }

   return clResultT;
}

