//============================================================================//
// File:          qcan_interface_peak.cpp                                     //
// Description:   QCan Inteface of PCAN Basic library                         //
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

#include "qcan_interface_peak.hpp"


//----------------------------------------------------------------------------//
// QCanInterfacePeak()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterfacePeak::QCanInterfacePeak(uint16_t uwPCanChannelV)
{
   if (!pclPcanBasicP.isAvailable())
   {
      qCritical() << "QCanInterfacePeak(): PEAKBasic library is not available!";
   }

   //----------------------------------------------------------------
   // setup PCAN Channel of this interface
   //
   uwPCanChannelP = uwPCanChannelV;
//   qInfo() << "QCanInterfacePeak::QCanInterfacePeak() INFO: New instance of PCAN Basic interface on channel" << QString::number(uwPCanChannelV,16) << "[hex]";

   btConnectedP = false;
   btFdUsedP = false;
}

//----------------------------------------------------------------------------//
// ~QCanInterfacePeak()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterfacePeak::~QCanInterfacePeak()
{
   qDebug() << "QCanInterfacePeak::~QCanInterfacePeak()";

   if (pclPcanBasicP.isAvailable())
   {
      if (btConnectedP)
      {
         pclPcanBasicP.unInitialize(uwPCanChannelP);
      }
   }
}

//----------------------------------------------------------------------------//
// connect()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfacePeak::connect(void)
{
   InterfaceError_e teReturnT = eERROR_LIBRARY;
   uint8_t          ubValueT;
   TPCANStatus      tsStatusT;
   qDebug() << QString("QCanInterfacePeak::connect()...");

   //----------------------------------------------------------------
   //
   //
   if (pclPcanBasicP.isAvailable())
   {
      //--------------------------------------------------------
      // get channel condition
      //
      tsStatusT = pclPcanBasicP.getValue(uwPCanChannelP, PCAN_CHANNEL_CONDITION, (void*)&ubValueT, 1);
      if (tsStatusT != PCAN_ERROR_OK)
      {
         qWarning() << QString("QCanInterfacePeak::connect(0x" +QString::number(uwPCanChannelP,16)+")") << "fail with error:" << pclPcanBasicP.formatedError(tsStatusT);
      }

      //--------------------------------------------------------
      // channel is not available, quit here
      //
      else if (ubValueT == PCAN_CHANNEL_UNAVAILABLE)
      {
         qDebug() << QString("QCanInterfacePeak::connect(0x" +QString::number(uwPCanChannelP,16)+")") << " is not available " << pclPcanBasicP.formatedError(tsStatusT);
         teReturnT = eERROR_CHANNEL;
      }

      //--------------------------------------------------------
      // channel is ocupied, quit here
      //
      else if (ubValueT == 0)
      {
         qWarning() << QString("QCanInterfacePeak::connect(0x" +QString::number(uwPCanChannelP,16)+")") << " is ouccupied " << pclPcanBasicP.formatedError(tsStatusT);
         teReturnT = eERROR_USED;
      }

      //--------------------------------------------------------
      // channel is available, get other parameters
      //
      else
      {
         //-------------------------------------------------
         // release can channel before init it
         //
         if (btConnectedP)
         {
            pclPcanBasicP.unInitialize(uwPCanChannelP);
            btConnectedP = false;
         }

         //-------------------------------------------------
         // initialise selected channel it again
         //
         tsStatusT = pclPcanBasicP.initialize(uwPCanChannelP, PCAN_BAUD_500K);
         if (tsStatusT != PCAN_ERROR_OK)
         {
            qWarning() << QString("QCanInterfacePeak::connect(0x" +QString::number(uwPCanChannelP,16)+")") << "fail with error:" << pclPcanBasicP.formatedError(tsStatusT);
            teReturnT = eERROR_USED;
         }

         else
         {
            btConnectedP = true;
            teReturnT = eERROR_NONE;
         }
      }
   }

   return teReturnT;
}

//----------------------------------------------------------------------------//
// connected()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanInterfacePeak::connected(void)
{
   return btConnectedP;
}


//----------------------------------------------------------------------------//
// disconnect()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfacePeak::disconnect()
{
   if (pclPcanBasicP.isAvailable())
   {
      TPCANStatus tsStatusT = pclPcanBasicP.unInitialize(uwPCanChannelP);
      if (tsStatusT == PCAN_ERROR_OK)
      {
         btConnectedP = false;
         return eERROR_NONE;
      }
      qWarning() << QString("QCanInterfacePeak::disconnect(0x" +QString::number(uwPCanChannelP,16)+")") << "fail with error:" << pclPcanBasicP.formatedError(tsStatusT);
   }

   return eERROR_LIBRARY;
}

//----------------------------------------------------------------------------//
// icon()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QIcon QCanInterfacePeak::icon(void)
{
   if (pclPcanBasicP.isAvailable())
   {
      char        aszBufferT[64];
      TPCANStatus tsStatusT = pclPcanBasicP.getValue(uwPCanChannelP, PCAN_HARDWARE_NAME, (void*)&aszBufferT[0], sizeof(aszBufferT));
      if (tsStatusT == PCAN_ERROR_OK)
      {
         QString clIconNameT;
         clIconNameT.append(QLatin1String(aszBufferT));
         clIconNameT = clIconNameT.toLower();
         clIconNameT.replace(QString("-"),QString("_"));
         clIconNameT.replace(QString(" "),QString("_"));

         qDebug() <<QString("QCanInterfacePeak::icon(0x" +QString::number(uwPCanChannelP,16)+")") << QString(":/images/"+clIconNameT+".png");

         return QIcon(QString(":/images/"+clIconNameT+".png"));

      }
      qWarning() << QString("QCanInterfacePeak::icon(0x" +QString::number(uwPCanChannelP,16)+")") << "fail with error:" << pclPcanBasicP.formatedError(tsStatusT);
   }

   return QIcon("images/pcan_fail.ico");
}


//----------------------------------------------------------------------------//
// name()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QString QCanInterfacePeak::name()
{
   TPCANStatus tsStatusT;
   QString     clNameT;
   char        aszBufferT[64];

   if (pclPcanBasicP.isAvailable())
   {
      clNameT.clear();

      //--------------------------------------------------------
      // get HW name
      //
      tsStatusT = pclPcanBasicP.getValue(uwPCanChannelP, PCAN_HARDWARE_NAME, (void*)&aszBufferT[0], sizeof(aszBufferT));
      if (tsStatusT != PCAN_ERROR_OK)
      {
         return QString("FAIL to get hardware name");
      }
      clNameT.append(QLatin1String(aszBufferT));
      //--------------------------------------------------------
      // name should contain device number, get it
      //
      tsStatusT = pclPcanBasicP.getValue(uwPCanChannelP, PCAN_DEVICE_NUMBER, (void*)&aszBufferT[0], 1);
      if (tsStatusT != PCAN_ERROR_OK)
      {
         return QString("FAIL to get device number");
      }
      clNameT.append(" Device " + QString::number((uint8_t)aszBufferT[0],16).toUpper());

      //--------------------------------------------------------
      // get CAN channel number
      //
      tsStatusT = pclPcanBasicP.getValue(uwPCanChannelP, PCAN_CONTROLLER_NUMBER, (void*)&aszBufferT[0], 1);
      if (tsStatusT != PCAN_ERROR_OK)
      {
         return QString("FAIL to get channel number");
      }
      clNameT.append("h CAN "+ QString::number(aszBufferT[0]+1,10));

      // return name
      return clNameT;

   }

   return QString("PCAN Basic library is not available");
}



//----------------------------------------------------------------------------//
// read()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfacePeak::read(QByteArray &clDataR)
{
   if (!pclPcanBasicP.isAvailable())
   {
      return eERROR_LIBRARY;
   }

   //----------------------------------------------------------------
   // check channel have been connected
   //
   #if QCAN_SUPPORT_CAN_FD > 0
   if (btFdUsedP == true)
   {
      return readFD(clDataR);
   }
   #endif

   return readCAN(clDataR);
}

//----------------------------------------------------------------------------//
// readCAN()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfacePeak::readCAN(QByteArray &clDataR)
{
   TPCANStatus       ulStatusT;
   uint8_t           ubCntT;
   TPCANMsg          tsCanMsgT;
   TPCANTimestamp    tsCanTimeStampT;
   uint32_t          ulMicroSecsT;
   QCanFrame         clCanFrameT;
   QCanFrameError    clErrFrameT;
   QCanTimeStamp     clTimeStampT;
   InterfaceError_e  clRetValueT = eERROR_NONE;
   
   //----------------------------------------------------------------
   // get next message from FIFO
   //
   ulStatusT = pclPcanBasicP.read(uwPCanChannelP, &tsCanMsgT, 
                                  &tsCanTimeStampT);

   
   //----------------------------------------------------------------
   // read message structure 
   //
   if (ulStatusT == PCAN_ERROR_OK)
   {
      //--------------------------------------------------------
      // handle data depending on type
      //
      if((tsCanMsgT.MSGTYPE & PCAN_MESSAGE_STATUS) > 0)
      {
         //------------------------------------------------
         // this is a status message
         //
         clRetValueT = eERROR_FIFO_RCV_EMPTY;
      }
      else
      {
         //------------------------------------------------
         // Classical CAN frame with standard or
         // extended identifier
         //
         if (tsCanMsgT.MSGTYPE & PCAN_MESSAGE_EXTENDED)
         {
            clCanFrameT.setFrameFormat(QCanFrame::eFORMAT_CAN_EXT);
         }
         else
         {
            clCanFrameT.setFrameFormat(QCanFrame::eFORMAT_CAN_STD);
         }
            
         //------------------------------------------------
         // Classical CAN remote frame
         //
         if (tsCanMsgT.MSGTYPE & PCAN_MESSAGE_RTR)
         {
            clCanFrameT.setRemote(true);
         }
         
         //------------------------------------------------
         // copy the identifier
         //
         clCanFrameT.setIdentifier(tsCanMsgT.ID);

         //------------------------------------------------
         // copy the DLC
         //
         clCanFrameT.setDlc(tsCanMsgT.LEN);

         //------------------------------------------------
         // copy the data
         //
         for (ubCntT = 0; ubCntT < clCanFrameT.dataSize(); ubCntT++)
         {
            clCanFrameT.setData(ubCntT, tsCanMsgT.DATA[ubCntT]);
         }

         //------------------------------------------------
         // copy the time-stamp
         // the value is a multiple of 1 us and has a
         // total time span of 4294,9 secs 
         //
         ulMicroSecsT = tsCanTimeStampT.millis * 1000;
         ulMicroSecsT = ulMicroSecsT + tsCanTimeStampT.micros;
         clTimeStampT.fromMicroSeconds(ulMicroSecsT);
         
         clCanFrameT.setTimeStamp(clTimeStampT);
         
         //------------------------------------------------
         // increase statistic counter
         //
         clStatisticP.ulRcvCount++;
         
         //------------------------------------------------
         // copy the CAN frame to a byte array for transfer
         //
         clDataR = clCanFrameT.toByteArray();
      }
   }

   //----------------------------------------------------------------
   // test for bus error
   //
   else if ((ulStatusT & (TPCANStatus)PCAN_ERROR_ANYBUSERR) > 0)
   {
      setupErrorFrame(ulStatusT, clErrFrameT);
      //--------------------------------------------------------
      // copy the error frame to a byte array
      //
      clDataR = clErrFrameT.toByteArray();
   }

   //----------------------------------------------------------------
   // the receive queue is empty
   //
   else if (ulStatusT == PCAN_ERROR_QRCVEMPTY)
   {
      clRetValueT = eERROR_FIFO_RCV_EMPTY;
   }

   //----------------------------------------------------------------
   // some other error
   //
   else
   {
      clRetValueT = eERROR_DEVICE;
   }

   return (clRetValueT);
}


//----------------------------------------------------------------------------//
// readFD()                                                                   //
//                                                                            //
//----------------------------------------------------------------------------//
#if QCAN_SUPPORT_CAN_FD > 0
QCanInterface::InterfaceError_e  QCanInterfacePeak::readFD(QByteArray &clDataR)
{
   TPCANStatus       ulStatusT;
   uint8_t           ubCntT;
   TPCANMsgFD        tsCanMsgT;
   TPCANTimestampFD  tsCanTimeStampT;
   uint32_t          ulMicroSecsT;
   QCanFrame         clCanFrameT;
   QCanFrameError    clErrFrameT;
   QCanTimeStamp     clTimeStampT;
   InterfaceError_e  clRetValueT = eERROR_NONE;

   //----------------------------------------------------------------
   // get next message from FIFO
   //
   ulStatusT = pclPcanBasicP.readFD(uwPCanChannelP, &tsCanMsgT,
                                    &tsCanTimeStampT);


   //----------------------------------------------------------------
   // read message structure
   //
   if (ulStatusT == PCAN_ERROR_OK)
   {
      //--------------------------------------------------------
      // handle data depending on type
      //
      if((tsCanMsgT.MSGTYPE & PCAN_MESSAGE_STATUS) > 0)
      {
         //------------------------------------------------
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
               clCanFrameT.setFrameFormat(QCanFrame::eFORMAT_FD_EXT);
            }
            else
            {
               clCanFrameT.setFrameFormat(QCanFrame::eFORMAT_FD_STD);
            }

            //----------------------------------------
            // Test for BRS bit
            //
            if (tsCanMsgFdT.MSGTYPE & PCAN_MESSAGE_BRS)
            {
               clCanFrameT.setBitrateSwitch();
            }

            //----------------------------------------
            // Test for ESI bit
            //
            if (tsCanMsgFdT.MSGTYPE & PCAN_MESSAGE_ESI)
            {
               clCanFrameT.setErrorStateIndicator();
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
               clCanFrameT.setFrameFormat(QCanFrame::eFORMAT_CAN_EXT);
            }
            else
            {
               clCanFrameT.setFrameFormat(QCanFrame::eFORMAT_CAN_STD);
            }

            //----------------------------------------
            // Classical CAN remote frame
            //
            if (tsCanMsgT.MSGTYPE & PCAN_MESSAGE_RTR)
            {
               clCanFrameT.setRemote();
            }
         }

         //------------------------------------------------
         // copy the identifier
         //
         clCanFrameT.setIdentifier(tsCanMsgT.ID);

         //------------------------------------------------
         // copy the DLC
         //
         clCanFrameT.setDlc(tsCanMsgT.DLC);

         //------------------------------------------------
         // copy the data
         //
         for (ubCntT = 0; ubCntT < clCanFrameT.dataSize(); ubCntT++)
         {
            clCanFrameT.setData(ubCntT, tsCanMsgT.DATA[ubCntT]);
         }

         //------------------------------------------------
         // copy the time-stamp
         // the value is a multiple of 1 us and has a
         // total time span of 4294,9 secs
         //
         /*
         ulMicroSecsT = tsCanTimeStampT.millis * 1000;
         ulMicroSecsT = ulMicroSecsT + tsCanTimeStampT.micros;
         clTimeStampT.fromMicroSeconds(ulMicroSecsT);

         clCanFrameT.setTimeStamp(clTimeStampT);
         */

         //------------------------------------------------
         // increase statistic counter
         //
         clStatisticP.ulRcvCount++;

         //------------------------------------------------
         // copy the CAN frame to a byte array for transfer
         //
         clDataR = clCanFrameT.toByteArray();
      }
   }
   else
   {
      //--------------------------------------------------------
      // test for bus error
      //
      if ((ulStatusT & (TPCANStatus)PCAN_ERROR_ANYBUSERR) > 0)
      {
         setupErrorFrame(ulStatusT, clErrFrameT);
         //------------------------------------------------
         // copy the error frame to a byte array
         //
         clDataR = clErrFrameT.toByteArray();
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


//----------------------------------------------------------------------------//
// setBitrate()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfacePeak::setBitrate( int32_t slNomBitRateV,
                                                               int32_t slDatBitRateV)
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

      //--------------------------------------------------------
      // select corresponding PEAK configuration for
      // nominal bit rate
      //
      switch(slNomBitRateV)
      {
         case eCAN_BITRATE_125K:
            clTxtBitrateNomT = "f_clock_mhz=80, nom_brp=2, nom_tseg1=255, nom_tseg2=64, nom_sjw=64";
            break;

         case eCAN_BITRATE_250K:
            clTxtBitrateNomT = "f_clock_mhz=80, nom_brp=2, nom_tseg1=127, nom_tseg2=32, nom_sjw=32";
            break;

         case eCAN_BITRATE_500K:
            clTxtBitrateNomT = "f_clock_mhz=80, nom_brp=2, nom_tseg1=63, nom_tseg2=16, nom_sjw=16";
            break;

         case eCAN_BITRATE_1M:
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
   // perform initalisation CAN Interface
   //
   pclPcanBasicP.unInitialize(uwPCanChannelP);

   if (slDatBitRateV != eCAN_BITRATE_NONE)
   {

      qDebug() << QString("QCanInterfacePeak::setBitrate(0x" +QString::number(uwPCanChannelP,16)+")") << " : FD Mode with bitrate adapting";

      #if QCAN_SUPPORT_CAN_FD > 0
      uint8_t ubValueBufT = PCAN_PARAMETER_ON;
      if (pclPcanBasicP.setValue(uwPCanChannelP,PCAN_BITRATE_ADAPTING,&ubValueBufT,sizeof(ubValueBufT)))
      {
         qWarning() << "WARNING: Fail to set adaptive bitrate!";
      }

      ulStatusT = pclPcanBasicP.initializeFD(uwPCanChannelP, clTxtBitrateT.data());
      btFdUsedP = true;

      #else
      ulStatusT = PCAN_ERROR_ILLPARAMVAL;
      #endif
   }
   else
   {
      qDebug() << QString("QCanInterfacePeak::setBitrate(0x" +QString::number(uwPCanChannelP,16)+")") << " : Standard Mode";
      ulStatusT = pclPcanBasicP.initialize(uwPCanChannelP, uwBtr0Btr1T);
      btFdUsedP = false;
   }

   if (ulStatusT != PCAN_ERROR_OK)
   {
      // get default description string of error code
      qCritical() << pclPcanBasicP.formatedError(ulStatusT);

      return eERROR_DEVICE;
   }

   return eERROR_NONE;
}


//----------------------------------------------------------------------------//
// setMode()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e	QCanInterfacePeak::setMode(const CAN_Mode_e teModeV)
{
   if (!pclPcanBasicP.isAvailable())
   {
      return eERROR_LIBRARY;
   }

   uint8_t ubValueBufT;

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

         ubValueBufT = 1;
         if (pclPcanBasicP.setValue(uwPCanChannelP,PCAN_BUSOFF_AUTORESET,&ubValueBufT,sizeof(ubValueBufT)))
         {
            qWarning() << "WARNING: Fail to set AutoReset of Device!";
         }
         break;

      case eCAN_MODE_STOP :

         break;

      default :
         return eERROR_MODE;
         break;
   }

   return eERROR_NONE;
}

//----------------------------------------------------------------------------//
// setupErrorFrame()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanInterfacePeak::setupErrorFrame(TPCANStatus ulStatusV, 
                                        QCanFrameError &clFrameR)
{

   //---------------------------------------------------------
   // set frame type and the bus status
   //
   clFrameR.setErrorType(QCanFrameError::eERROR_TYPE_NONE);

   switch (ulStatusV)
   {
      case PCAN_ERROR_BUSLIGHT :
         clFrameR.setErrorState(eCAN_STATE_BUS_WARN);
         break;
      case PCAN_ERROR_BUSWARNING :
         clFrameR.setErrorState(eCAN_STATE_BUS_WARN);
         break;
      case PCAN_ERROR_BUSPASSIVE :
         clFrameR.setErrorState(eCAN_STATE_BUS_PASSIVE);
         break;
      case PCAN_ERROR_BUSOFF :
         clFrameR.setErrorState(eCAN_STATE_BUS_OFF);
         break;

      default :
         clFrameR.setErrorState(eCAN_STATE_BUS_ACTIVE);
         break;
   }

}


//----------------------------------------------------------------------------//
// state()                                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
CAN_State_e	QCanInterfacePeak::state()
{
   //! \todo

   return eCAN_STATE_BUS_ACTIVE;
}

//----------------------------------------------------------------------------//
// statistic()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
QCanInterface::InterfaceError_e	QCanInterfacePeak::statistic(QCanStatistic_ts &clStatisticR)
{
   //! \todo

   clStatisticR.ulErrCount = 0;

   return(eERROR_NONE);
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
      TPCANStatus tsStatusT = pclPcanBasicP.getValue(uwPCanChannelP, PCAN_CHANNEL_FEATURES, (void*)&ulBufferT, sizeof(ulBufferT));
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


//----------------------------------------------------------------------------//
// write()                                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
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

      ulStatusT = pclPcanBasicP.write(uwPCanChannelP, &tsCanMsgT);

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

      ulStatusT = pclPcanBasicP.writeFD(uwPCanChannelP, &tsCanMsgFdT);
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
