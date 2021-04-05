//====================================================================================================================//
// File:          qcan_send.cpp                                                                                       //
// Description:   Send CAN messages                                                                                   //
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


#include "qcan_send.hpp"

#include "qcan_server_settings.hpp"

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtCore/QTimer>


//--------------------------------------------------------------------------------------------------------------------//
// main()                                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
int main(int argc, char *argv[])
{
   QCoreApplication clAppT(argc, argv);
   QCoreApplication::setApplicationName("can-send");
   
   //---------------------------------------------------------------------------------------------------
   // get application version (defined in .pro file)
   //
   QString clVersionT;
   clVersionT += QString("%1.").arg(VERSION_MAJOR);
   clVersionT += QString("%1.").arg(VERSION_MINOR, 2, 10, QLatin1Char('0'));
   clVersionT += QString("%1,").arg(VERSION_BUILD, 2, 10, QLatin1Char('0'));
   clVersionT += " build on ";
   clVersionT += __DATE__;
   QCoreApplication::setApplicationVersion(clVersionT);


   //---------------------------------------------------------------------------------------------------
   // create the main class
   //
   QCanSend clMainT;

   
   //---------------------------------------------------------------------------------------------------
   // connect the signals
   //
   QObject::connect(&clMainT, SIGNAL(finished()),
                    &clAppT, SLOT(quit()));
   
   QObject::connect(&clAppT, SIGNAL(aboutToQuit()),
                    &clMainT, SLOT(aboutToQuitApp()));

   
   //---------------------------------------------------------------------------------------------------
   // This code will start the messaging engine in QT and in 10 ms it will start the execution in the
   // clMainT.runCmdParser() routine.
   //
   QTimer::singleShot(10, &clMainT, SLOT(runCmdParser()));

   clAppT.exec();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSend()                                                                                                         //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanSend::QCanSend(QObject *parent) 
         :QObject(parent)
{
   //---------------------------------------------------------------------------------------------------
   // get the instance of the main application
   //
   pclApplicationP = QCoreApplication::instance();
   
   //---------------------------------------------------------------------------------------------------
   // connect signals for socket operations
   //
   QObject::connect( &clCanSocketP, &QCanSocket::connected,
                     this,          &QCanSend::onSocketConnected);

   QObject::connect( &clCanSocketP, &QCanSocket::disconnected,
                    this,           &QCanSend::onSocketDisconnected);
   
   QObject::connect( &clCanSocketP, QOverload<QAbstractSocket::SocketError>::of(&QCanSocket::error), 
                     this,          &QCanSend::onSocketError);

   
   //---------------------------------------------------------------------------------------------------
   // connect QServerSettings object
   //
   connect(&clServerSettingsP, &QCanServerSettings::stateChanged, 
           this,               &QCanSend::onServerStateChanged);

   connect(&clServerSettingsP, &QCanServerSettings::objectReceived, 
           this,               &QCanSend::onServerObjectReceived);


   //---------------------------------------------------------------------------------------------------
   // set default values
   //
   teCanChannelP  = eCAN_CHANNEL_NONE;
   ulFrameIdP     = 0;
   ulFrameGapP    = 0;
   ubFrameDlcP    = 0;
   ubFrameFormatP = 0;
   btIncIdP       = false;
   btIncDlcP      = false;
   btIncDataP     = false;
   ulFrameCountP  = 0;

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSend::aboutToQuitApp()                                                                                         //
// shortly after quit is called the CoreApplication will signal this routine: delete objects / clean up               //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSend::aboutToQuitApp()
{

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSend::onNetworkObjectReceived()                                                                                //
// print details about CAN interface and connect to interface                                                         //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSend::onNetworkObjectReceived(const CAN_Channel_e teChannelV, QJsonObject clNetworkConfigV)
{
   Q_UNUSED (clNetworkConfigV);

   //---------------------------------------------------------------------------------------------------
   // remove all signals from this class
   //
   disconnect(&clNetworkSettingsP, nullptr, this, nullptr);

   if (teChannelV == teCanChannelP)
   {
      //-------------------------------------------------------------------------------------------
      // since this slot has been called, we can connect to the network
      //
      clCanSocketP.connectNetwork(teCanChannelP);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSend::onServerObjectReceived()                                                                                 //
// print version of CANpie FD Server                                                                                  //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSend::onServerObjectReceived(QJsonObject clServerConfigV)
{
   Q_UNUSED(clServerConfigV);

   if (teCanChannelP <= clServerSettingsP.networkCount())
   {
      clNetworkSettingsP.setChannel(teCanChannelP);
      clNetworkSettingsP.connectToServer(clHostAddressP);

      connect(&clNetworkSettingsP, &QCanNetworkSettings::objectReceived, 
              this,                &QCanSend::onNetworkObjectReceived);
   }
   else
   {
      fprintf(stdout, "%s%d %s \n", qPrintable("CAN interface can"), teCanChannelP, qPrintable("not available"));
      disconnect(&clServerSettingsP, nullptr, this, nullptr);
      quit();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSend::onServerStateChanged()                                                                                   //
// check for new state of the CANpie FD Server                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSend::onServerStateChanged(enum QCanServerSettings::State_e teStateV)
{  
   bool btQuitProgramT = false;

   qDebug() << " QCanSend::onServerStateChanged()" << teStateV;

   switch (teStateV)
   {
      case QCanServerSettings::eSTATE_CLOSED:
         fprintf(stdout, "CANpie FD server %s \n", qPrintable(clServerSettingsP.stateString()));
         btQuitProgramT = true;
         break;

      case QCanServerSettings::eSTATE_UNKNOWN:
         fprintf(stdout, "CANpie FD server %s \n", qPrintable(clServerSettingsP.stateString()));
         btQuitProgramT = true;
         break;

      case QCanServerSettings::eSTATE_INACTIVE:
         fprintf(stdout, "CANpie FD server %s \n", qPrintable(clServerSettingsP.stateString()));
         btQuitProgramT = true;
         break;

      case QCanServerSettings::eSTATE_ACTIVE:
         break;

      default:
         btQuitProgramT = true;
         break;
   }

   if (btQuitProgramT)
   {
      this->quit();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSend::quit()                                                                                                   //
// call this routine to quit the application                                                                          //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSend::quit()
{
   disconnect(&clServerSettingsP, nullptr, this, nullptr);
   clCanSocketP.disconnectNetwork();

   emit finished();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSend::runCmdParser()                                                                                           //
// 10ms after the application starts this method will parse all commands                                              //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSend::runCmdParser(void)
{
   //---------------------------------------------------------------------------------------------------
   // setup command line parser
   //
   clCommandParserP.setApplicationDescription(tr("Send messages on CAN interface"));
   
   //---------------------------------------------------------------------------------------------------
   // argument <interface> is required
   //
   clCommandParserP.addPositionalArgument("interface", 
                                          tr("CAN interface, e.g. can1"));

   //---------------------------------------------------------------------------------------------------
   // command line option: -D <dlc>
   //
   QCommandLineOption clOptFrameDlcT("D", 
         tr("Set DLC to <dlc>"),
         tr("dlc"),
         "0");          // default value
   clCommandParserP.addOption(clOptFrameDlcT);
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -f <format>
   //
   QCommandLineOption clOptFormatT("f", 
         tr("Set frame format to [CBFF|CEFF|FBFF|FEFF]"),
         tr("format"),
         "CBFF");       // default value
   clCommandParserP.addOption(clOptFormatT);
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -g <msec>
   //
   QCommandLineOption clOptGapT("g", 
         tr("Time gap in milli-seconds between multiple CAN frames"),
         tr("gap"),
         "0");          // default value
   clCommandParserP.addOption(clOptGapT);
   
   clCommandParserP.addHelpOption();

   //---------------------------------------------------------------------------------------------------
   // command line option: -H <host>
   //
   QCommandLineOption clOptHostT("H", 
         tr("Connect to <host>"),
         tr("host"));
   clCommandParserP.addOption(clOptHostT);
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -i <type>
   //
   QCommandLineOption clOptIncT("i", 
         tr("Increment the requested type"),
         tr("I|D|P"));
   clCommandParserP.addOption(clOptIncT);
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -I <id>
   //
   QCommandLineOption clOptFrameIdT("I", 
         tr("Set identifier to <id>"),
         tr("id"));
   clCommandParserP.addOption(clOptFrameIdT);
   

   //---------------------------------------------------------------------------------------------------
   // command line option: -n <count>
   //
   QCommandLineOption clOptCountT("n", 
         tr("Terminate after transmission of <count> CAN frames"),
         tr("count"),
         "1");          // default value
   clCommandParserP.addOption(clOptCountT);

   
   //---------------------------------------------------------------------------------------------------
   // command line option: -P <payload>
   //
   QCommandLineOption clOptFrameDataT("P", 
         tr("Set payload to <payload>, i.e. a string of hex values"),
         tr("payload"));
   clCommandParserP.addOption(clOptFrameDataT);
   
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -v, --version
   //
   clCommandParserP.addVersionOption();


   //---------------------------------------------------------------------------------------------------
   // Process the actual command line arguments given by the user
   //
   clCommandParserP.process(*pclApplicationP);
   const QStringList clArgsT = clCommandParserP.positionalArguments();
   if (clArgsT.size() != 1) 
   {
      fprintf(stderr, "%s\n", 
              qPrintable(tr("Error: Must specify CAN interface.\n")));
      clCommandParserP.showHelp(0);
   }

   
   //---------------------------------------------------------------------------------------------------
   // test format of argument <interface>
   //
   QString clInterfaceT = clArgsT.at(0);
   if(!clInterfaceT.startsWith("can"))
   {
      fprintf(stderr, "%s %s\n", 
              qPrintable(tr("Error: Unknown CAN interface ")),
              qPrintable(clInterfaceT));
      clCommandParserP.showHelp(0);
   }
   
   //---------------------------------------------------------------------------------------------------
   // convert CAN channel to uint8_t value
   //
   QString clIfNumT = clInterfaceT.right(clInterfaceT.size() - 3);
   bool btConversionSuccessT;
   int32_t slChannelT = clIfNumT.toInt(&btConversionSuccessT, 10);
   if((btConversionSuccessT == false) ||
      (slChannelT == 0) )
   {
      fprintf(stderr, "%s \n\n", 
              qPrintable(tr("Error: CAN interface out of range")));
      clCommandParserP.showHelp(0);
   }
   
   //---------------------------------------------------------------------------------------------------
   // store CAN interface channel (CAN_Channel_e)
   //
   teCanChannelP = (CAN_Channel_e) (slChannelT);

   //---------------------------------------------------------------------------------------------------
   // get frame format
   //
   if (clCommandParserP.value(clOptFormatT).contains("CBFF", Qt::CaseInsensitive))
   {
      ubFrameFormatP = QCanFrame::eFORMAT_CAN_STD;
   }
   else if (clCommandParserP.value(clOptFormatT).contains("CEFF", Qt::CaseInsensitive))
   {
      ubFrameFormatP = QCanFrame::eFORMAT_CAN_EXT;
   }
   else if (clCommandParserP.value(clOptFormatT).contains("FBFF", Qt::CaseInsensitive))
   {
      ubFrameFormatP = QCanFrame::eFORMAT_FD_STD;
   }
   else if (clCommandParserP.value(clOptFormatT).contains("FEFF", Qt::CaseInsensitive))
   {
      ubFrameFormatP = QCanFrame::eFORMAT_FD_EXT;
   }
   else
   {
      fprintf(stderr, "%s \n\n", 
              qPrintable(tr("Error: Unknown option for frame format.")));
      clCommandParserP.showHelp(0);
   }
   
   //---------------------------------------------------------------------------------------------------
   // get identifier value
   //
   ulFrameIdP = clCommandParserP.value(clOptFrameIdT).toInt(Q_NULLPTR, 16);
   
   //---------------------------------------------------------------------------------------------------
   // get DLC value
   //
   ubFrameDlcP = clCommandParserP.value(clOptFrameDlcT).toInt(Q_NULLPTR, 10);
   if( ((ubFrameFormatP > QCanFrame::eFORMAT_CAN_EXT) && (ubFrameDlcP > 15)) ||
       ((ubFrameFormatP < QCanFrame::eFORMAT_FD_STD)  && (ubFrameDlcP >  8)) )
   {
      fprintf(stderr, "%s \n\n", 
              qPrintable(tr("Error: DLC value out of range.")));
      clCommandParserP.showHelp(0);
   }
   
   //---------------------------------------------------------------------------------------------------
   // get payload
   //
   QString clPayloadT = clCommandParserP.value(clOptFrameDataT);
   for (uint8_t ubCntT = 0; ubCntT < QCAN_MSG_DATA_MAX; ubCntT++)
   {
      if (clPayloadT.size() >= 2)
      {
         //-----------------------------------------------------
         // convert two characters from string and remove them
         // afterwards
         //
         aubFrameDataP[ubCntT] = clPayloadT.left(2).toUShort(Q_NULLPTR, 16);
         clPayloadT.remove(0, 2);
      }
      else
      {
         aubFrameDataP[ubCntT] = 0x00;
      }
   }
   
   //---------------------------------------------------------------------------------------------------
   // get number of frames to send
   //
   ulFrameCountP = clCommandParserP.value(clOptCountT).toInt(Q_NULLPTR, 10);

   //---------------------------------------------------------------------------------------------------
   // get time gap between frames
   //
   ulFrameGapP = clCommandParserP.value(clOptGapT).toInt(Q_NULLPTR, 10);
   
   //---------------------------------------------------------------------------------------------------
   // get increment type
   //
   btIncIdP  = clCommandParserP.value(clOptIncT).contains("I", Qt::CaseInsensitive);
   btIncDlcP = clCommandParserP.value(clOptIncT).contains("D", Qt::CaseInsensitive);
   btIncDataP= clCommandParserP.value(clOptIncT).contains("P", Qt::CaseInsensitive);
   

   //---------------------------------------------------------------------------------------------------
   // set host address for socket
   //
   if (clCommandParserP.isSet(clOptHostT))
   {
      clHostAddressP = QHostAddress(clCommandParserP.value(clOptHostT));
   }
   else
   {
      clHostAddressP.setAddress(QHostAddress::LocalHost);
   }
   

   //---------------------------------------------------------------------------------------------------
   // check for valid server host address
   //
   if (clHostAddressP.isNull())
   {
      fprintf(stdout, "No valid address for CANpie FD Server\n");
      quit();
   }
   clCanSocketP.setHostAddress(clHostAddressP);


   //---------------------------------------------------------------------------------------------------
   // connect to QCanServer class (i.e. CANpie FD Server)
   //
   clServerSettingsP.connectToServer(clHostAddressP);

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSend::sendFrame()                                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSend::sendFrame(void)
{
   QTime          clSystemTimeT;
   QCanTimeStamp  clCanTimeT;
   
   clSystemTimeT = QTime::currentTime();
   clCanTimeT.fromMilliSeconds(clSystemTimeT.msec());
   clCanFrameP.setTimeStamp(clCanTimeT);
   
   clCanSocketP.write(clCanFrameP);
   
   if (ulFrameCountP > 1)
   {
      ulFrameCountP--;
      
      //-------------------------------------------------------------------------------------------
      // test if identifier value must be incremented
      //
      if (btIncIdP)
      {
         ulFrameIdP++;
         
         //-----------------------------------------------------------------------------------
         // test for wrap-around
         //
         if (clCanFrameP.isExtended())
         {
            if (ulFrameIdP > QCAN_FRAME_ID_MASK_EXT)
            {
               ulFrameIdP = 0;
            }
         }
         else
         {
            if (ulFrameIdP > QCAN_FRAME_ID_MASK_STD)
            {
               ulFrameIdP = 0;
            }
         }
         
         //-----------------------------------------------------------------------------------
         // set new identifier value
         //
         clCanFrameP.setIdentifier(ulFrameIdP);
      }
      
      //-------------------------------------------------------------------------------------------
      // test if DLC value must be incremented
      //
      if (btIncDlcP)
      {
         ubFrameDlcP++;
         
         //-----------------------------------------------------------------------------------
         // test for wrap-around
         //
         if (clCanFrameP.frameFormat() > QCanFrame::eFORMAT_CAN_EXT)
         {
            if (ubFrameDlcP > 15)
            {
               ubFrameDlcP = 0;
            }
         }
         else
         {
            if (ubFrameDlcP > 8)
            {
               ubFrameDlcP = 0;
            }
         }

         //-----------------------------------------------------------------------------------
         // set new DLC value
         //
         clCanFrameP.setDlc(ubFrameDlcP);
      }  
      
      //-------------------------------------------------------------------------------------------
      // test if data value must be incremented
      //
      if (btIncDataP)
      {

         if (clCanFrameP.dataSize() == 1)
         {
            clCanFrameP.setData(0, clCanFrameP.data(0) + 1);
         }

         if (clCanFrameP.dataSize() == 2)
         {
            clCanFrameP.setDataUInt16(0, clCanFrameP.dataUInt16(0) + 1);
         }

         if (clCanFrameP.dataSize() >= 4)
         {
            clCanFrameP.setDataUInt32(0, clCanFrameP.dataUInt32(0) + 1);
         }
      }
      
      QTimer::singleShot(ulFrameGapP, this, SLOT(sendFrame()));
   }
   else
   {
      QTimer::singleShot(50, this, SLOT(quit()));
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSend::onSocketConnected()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSend::onSocketConnected()
{
   //---------------------------------------------------------------------------------------------------
   // initial setup of CAN frame
   //
   clCanFrameP.setFrameFormat((QCanFrame::FrameFormat_e) ubFrameFormatP);
   clCanFrameP.setIdentifier(ulFrameIdP);
   clCanFrameP.setDlc(ubFrameDlcP);
   for(uint8_t ubCntT = 0; ubCntT < clCanFrameP.dataSize(); ubCntT++)
   {
      clCanFrameP.setData(ubCntT, aubFrameDataP[ubCntT]);
   }
    
   QTimer::singleShot(10, this, SLOT(sendFrame()));
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSend::onSocketDisconnected()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSend::onSocketDisconnected()
{
   qDebug() << "Disconnected from CAN " << teCanChannelP;
   
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSend::onSocketError()                                                                                          //
// Show error message and quit                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSend::onSocketError(QAbstractSocket::SocketError teSocketErrorV)
{
   Q_UNUSED(teSocketErrorV);  // parameter not used 
   
   //----------------------------------------------------------------
   // show error message in case the connection to the network fails
   //
   fprintf(stderr, "%s %s\n", 
           qPrintable(tr("Failed to connect to CAN interface:")),
           qPrintable(clCanSocketP.errorString()));
   quit();
}
