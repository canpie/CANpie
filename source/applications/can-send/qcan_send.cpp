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
QCanSend::QCanSend(QObject *parent) :
    QObject(parent)
{
   //---------------------------------------------------------------------------------------------------
   // get the instance of the main application
   //
   pclAppP = QCoreApplication::instance();

   
   //---------------------------------------------------------------------------------------------------
   // connect signals for socket operations
   //
   QObject::connect(&clCanSocketP, SIGNAL(connected()),
                    this, SLOT(socketConnected()));

   QObject::connect(&clCanSocketP, SIGNAL(disconnected()),
                    this, SLOT(socketDisconnected()));
   
   QObject::connect(&clCanSocketP, SIGNAL(error(QAbstractSocket::SocketError)),
                    this, SLOT(socketError(QAbstractSocket::SocketError)));
   
   //---------------------------------------------------------------------------------------------------
   // set default values
   //
   ubChannelP     = eCAN_CHANNEL_NONE;
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
// QCanSend::quit()                                                                                                   //
// call this routine to quit the application                                                                          //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSend::quit()
{
   clCanSocketP.disconnectNetwork();

   emit finished();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::runCmdParser()                                                                                           //
// 10ms after the application starts this method will parse all commands                                              //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSend::runCmdParser(void)
{
   //---------------------------------------------------------------------------------------------------
   // setup command line parser
   //
   clCmdParserP.setApplicationDescription(tr("Send messages on CAN interface"));
   clCmdParserP.addHelpOption();
   
   //---------------------------------------------------------------------------------------------------
   // argument <interface> is required
   //
   clCmdParserP.addPositionalArgument("interface", 
                                      tr("CAN interface, e.g. can1"));

   //---------------------------------------------------------------------------------------------------
   // command line option: -D <dlc>
   //
   QCommandLineOption clOptFrameDlcT("D", 
         tr("Set DLC to <dlc>"),
         tr("dlc"),
         "0");          // default value
   clCmdParserP.addOption(clOptFrameDlcT);
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -f <format>
   //
   QCommandLineOption clOptFormatT("f", 
         tr("Set frame format to [CBFF|CEFF|FBFF|FEFF]"),
         tr("format"),
         "CBFF");       // default value
   clCmdParserP.addOption(clOptFormatT);
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -g <msec>
   //
   QCommandLineOption clOptGapT("g", 
         tr("Time gap in milli-seconds between multiple CAN frames"),
         tr("gap"),
         "0");          // default value
   clCmdParserP.addOption(clOptGapT);
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -H <host>
   //
   QCommandLineOption clOptHostT("H", 
         tr("Connect to <host>"),
         tr("host"));
   clCmdParserP.addOption(clOptHostT);
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -i <type>
   //
   QCommandLineOption clOptIncT("i", 
         tr("Increment the requested type"),
         tr("I|D|P"));
   clCmdParserP.addOption(clOptIncT);
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -I <id>
   //
   QCommandLineOption clOptFrameIdT("I", 
         tr("Set identifier to <id>"),
         tr("id"));
   clCmdParserP.addOption(clOptFrameIdT);
   

   //---------------------------------------------------------------------------------------------------
   // command line option: -n <count>
   //
   QCommandLineOption clOptCountT("n", 
         tr("Terminate after transmission of <count> CAN frames"),
         tr("count"),
         "1");          // default value
   clCmdParserP.addOption(clOptCountT);

   
   //---------------------------------------------------------------------------------------------------
   // command line option: -P <payload>
   //
   QCommandLineOption clOptFrameDataT("P", 
         tr("Set payload to <payload>, i.e. a string of hex values"),
         tr("payload"));
   clCmdParserP.addOption(clOptFrameDataT);
   
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -v, --version
   //
   clCmdParserP.addVersionOption();


   //---------------------------------------------------------------------------------------------------
   // Process the actual command line arguments given by the user
   //
   clCmdParserP.process(*pclAppP);
   const QStringList clArgsT = clCmdParserP.positionalArguments();
   if (clArgsT.size() != 1) 
   {
      fprintf(stderr, "%s\n", 
              qPrintable(tr("Error: Must specify CAN interface.\n")));
      clCmdParserP.showHelp(0);
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
      clCmdParserP.showHelp(0);
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
      clCmdParserP.showHelp(0);
   }
   
   //---------------------------------------------------------------------------------------------------
   // store CAN interface channel (CAN_Channel_e)
   //
   ubChannelP = (uint8_t) (slChannelT);

   //---------------------------------------------------------------------------------------------------
   // get frame format
   //
   if (clCmdParserP.value(clOptFormatT).contains("CBFF", Qt::CaseInsensitive))
   {
      ubFrameFormatP = QCanFrame::eFORMAT_CAN_STD;
   }
   else if (clCmdParserP.value(clOptFormatT).contains("CEFF", Qt::CaseInsensitive))
   {
      ubFrameFormatP = QCanFrame::eFORMAT_CAN_EXT;
   }
   else if (clCmdParserP.value(clOptFormatT).contains("FBFF", Qt::CaseInsensitive))
   {
      ubFrameFormatP = QCanFrame::eFORMAT_FD_STD;
   }
   else if (clCmdParserP.value(clOptFormatT).contains("FEFF", Qt::CaseInsensitive))
   {
      ubFrameFormatP = QCanFrame::eFORMAT_FD_EXT;
   }
   else
   {
      fprintf(stderr, "%s \n\n", 
              qPrintable(tr("Error: Unknown option for frame format.")));
      clCmdParserP.showHelp(0);
   }
   
   //---------------------------------------------------------------------------------------------------
   // get identifier value
   //
   ulFrameIdP = clCmdParserP.value(clOptFrameIdT).toInt(Q_NULLPTR, 16);
   
   //---------------------------------------------------------------------------------------------------
   // get DLC value
   //
   ubFrameDlcP = clCmdParserP.value(clOptFrameDlcT).toInt(Q_NULLPTR, 10);
   if( ((ubFrameFormatP > QCanFrame::eFORMAT_CAN_EXT) && (ubFrameDlcP > 15)) ||
       ((ubFrameFormatP < QCanFrame::eFORMAT_FD_STD)  && (ubFrameDlcP >  8)) )
   {
      fprintf(stderr, "%s \n\n", 
              qPrintable(tr("Error: DLC value out of range.")));
      clCmdParserP.showHelp(0);
   }
   
   //---------------------------------------------------------------------------------------------------
   // get payload
   //
   QString clPayloadT = clCmdParserP.value(clOptFrameDataT);
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
   ulFrameCountP = clCmdParserP.value(clOptCountT).toInt(Q_NULLPTR, 10);

   //---------------------------------------------------------------------------------------------------
   // get time gap between frames
   //
   ulFrameGapP = clCmdParserP.value(clOptGapT).toInt(Q_NULLPTR, 10);
   
   //---------------------------------------------------------------------------------------------------
   // get increment type
   //
   btIncIdP  = clCmdParserP.value(clOptIncT).contains("I", Qt::CaseInsensitive);
   btIncDlcP = clCmdParserP.value(clOptIncT).contains("D", Qt::CaseInsensitive);
   btIncDataP= clCmdParserP.value(clOptIncT).contains("P", Qt::CaseInsensitive);
   
   //---------------------------------------------------------------------------------------------------
   // set host address for socket
   //
   if (clCmdParserP.isSet(clOptHostT))
   {
      QHostAddress clAddressT = QHostAddress(clCmdParserP.value(clOptHostT));
      clCanSocketP.setHostAddress(clAddressT);
   }
   else
   {
      /*
      //-------------------------------------------------------------------------------------------
      // check to local server state and print error if it is not active
      //
      if (pclServerP->state() < QCanServerSettings::eSTATE_ACTIVE)
      {
         fprintf(stdout, "CANpie FD server %s \n", qPrintable(pclServerP->stateString()));
         exit(0);
      }
      */
   }


   //---------------------------------------------------------------------------------------------------
   // connect to CAN interface
   //
   clCanSocketP.connectNetwork((CAN_Channel_e) ubChannelP);

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::sendFrame()                                                                                              //
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
// QCanSend::socketConnected()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSend::socketConnected()
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
// QCanSend::socketDisconnected()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//

void QCanSend::socketDisconnected()
{
   qDebug() << "Disconnected from CAN " << ubChannelP;
   
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanSend::socketError()                                                                                            //
// Show error message and quit                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanSend::socketError(QAbstractSocket::SocketError teSocketErrorV)
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
