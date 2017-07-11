//============================================================================//
// File:          qcan_send.cpp                                               //
// Description:   Send CAN messages                                           //
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

#include "qcan_send.hpp"

#include <QTime>
#include <QTimer>
#include <QDebug>


//----------------------------------------------------------------------------//
// main()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
int main(int argc, char *argv[])
{
   QCoreApplication clAppT(argc, argv);
   QCoreApplication::setApplicationName("can-send");
   
   //----------------------------------------------------------------
   // get application version (defined in .pro file)
   //
   QString clVersionT;
   clVersionT += QString("%1.%2.").arg(VERSION_MAJOR).arg(VERSION_MINOR);
   clVersionT += QString("%1").arg(VERSION_BUILD);
   QCoreApplication::setApplicationVersion(clVersionT);


   //----------------------------------------------------------------
   // create the main class
   //
   QCanSend clMainT;

   
   //----------------------------------------------------------------
   // connect the signals
   //
   QObject::connect(&clMainT, SIGNAL(finished()),
                    &clAppT, SLOT(quit()));
   
   QObject::connect(&clAppT, SIGNAL(aboutToQuit()),
                    &clMainT, SLOT(aboutToQuitApp()));

   
   //----------------------------------------------------------------
   // This code will start the messaging engine in QT and in 10 ms 
   // it will start the execution in the clMainT.runCmdParser() 
   // routine.
   //
   QTimer::singleShot(10, &clMainT, SLOT(runCmdParser()));

   clAppT.exec();
}


//----------------------------------------------------------------------------//
// QCanSend()                                                                 //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanSend::QCanSend(QObject *parent) :
    QObject(parent)
{
   //----------------------------------------------------------------
   // get the instance of the main application
   //
   pclAppP = QCoreApplication::instance();

   ubChannelP = eCAN_CHANNEL_NONE;
   
   //----------------------------------------------------------------
   // connect signals for socket operations
   //
   QObject::connect(&clCanSocketP, SIGNAL(connected()),
                    this, SLOT(socketConnected()));

   QObject::connect(&clCanSocketP, SIGNAL(disconnected()),
                    this, SLOT(socketDisconnected()));
   
   QObject::connect(&clCanSocketP, SIGNAL(error(QAbstractSocket::SocketError)),
                    this, SLOT(socketError(QAbstractSocket::SocketError)));
   
}

// shortly after quit is called the CoreApplication will signal this routine
// this is a good place to delete any objects that were created in the
// constructor and/or to stop any threads
void QCanSend::aboutToQuitApp()
{
    // stop threads
    // sleep(1);   // wait for threads to stop.
    // delete any objects
}


//----------------------------------------------------------------------------//
// quit()                                                                     //
// call this routine to quit the application                                  //
//----------------------------------------------------------------------------//
void QCanSend::quit()
{
   //qDebug() << "I will quit soon";
   clCanSocketP.disconnectNetwork();

   emit finished();
}


//----------------------------------------------------------------------------//
// runCmdParser()                                                             //
// 10ms after the application starts this method will parse all commands      //
//----------------------------------------------------------------------------//
void QCanSend::runCmdParser(void)
{
   //----------------------------------------------------------------
   // setup command line parser
   //
   clCmdParserP.setApplicationDescription(tr("Send messages on CAN interface"));
   clCmdParserP.addHelpOption();
   
   //----------------------------------------------------------------
   // argument <interface> is required
   //
   clCmdParserP.addPositionalArgument("interface", 
                                      tr("CAN interface, e.g. can1"));

   //-----------------------------------------------------------
   // command line option: -D <dlc>
   //
   QCommandLineOption clOptFrameDlcT("D", 
         tr("Set DLC to <dlc>"),
         tr("dlc"),
         "0");          // default value
   clCmdParserP.addOption(clOptFrameDlcT);
   
   //-----------------------------------------------------------
   // command line option: -f <format>
   //
   QCommandLineOption clOptFormatT("f", 
         tr("Set frame format to [CBFF|CEFF|FBFF|FEFF]"),
         tr("format"),
         "CBFF");       // default value
   clCmdParserP.addOption(clOptFormatT);
   
   //-----------------------------------------------------------
   // command line option: -g <msec>
   //
   QCommandLineOption clOptGapT("g", 
         tr("Time gap in milli-seconds between multiple CAN frames"),
         tr("gap"),
         "10");          // default value
   clCmdParserP.addOption(clOptGapT);
   
   //-----------------------------------------------------------
   // command line option: -H <host>
   //
   QCommandLineOption clOptHostT("H", 
         tr("Connect to <host>"),
         tr("host"));
   clCmdParserP.addOption(clOptHostT);
   
   //-----------------------------------------------------------
   // command line option: -i <type>
   //
   QCommandLineOption clOptIncT("i", 
         tr("Increment the requested type"),
         tr("I|D|P"));
   clCmdParserP.addOption(clOptIncT);
   
   //-----------------------------------------------------------
   // command line option: -I <id>
   //
   QCommandLineOption clOptFrameIdT("I", 
         tr("Set identifier to <id>"),
         tr("id"));
   clCmdParserP.addOption(clOptFrameIdT);
   
   //-----------------------------------------------------------
   // command line option: -n <count>
   //
   QCommandLineOption clOptCountT("n", 
         tr("Terminate after transmission of <count> CAN frames"),
         tr("count"),
         "1");          // default value
   clCmdParserP.addOption(clOptCountT);
   
   //-----------------------------------------------------------
   // command line option: -P <payload>
   //
   QCommandLineOption clOptFrameDataT("P", 
         tr("Set payload to <payload>, i.e. a string of hex values"),
         tr("payload"));
   clCmdParserP.addOption(clOptFrameDataT);
   
   
   clCmdParserP.addVersionOption();

   //----------------------------------------------------------------
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

   
   //----------------------------------------------------------------
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
   
   //-----------------------------------------------------------
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
   
   //-----------------------------------------------------------
   // store CAN interface channel (CAN_Channel_e)
   //
   ubChannelP = (uint8_t) (slChannelT);

   //----------------------------------------------------------------
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
   
   //----------------------------------------------------------------
   // get identifier value
   //
   ulFrameIdP = clCmdParserP.value(clOptFrameIdT).toInt(Q_NULLPTR, 16);
   
   //----------------------------------------------------------------
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
   
   //----------------------------------------------------------------
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
   
   //----------------------------------------------------------------
   // get number of frames to send
   //
   ulFrameCountP = clCmdParserP.value(clOptCountT).toInt(Q_NULLPTR, 10);

   //----------------------------------------------------------------
   // get time gap between frames
   //
   ulFrameGapP = clCmdParserP.value(clOptGapT).toInt(Q_NULLPTR, 10);
   
   //----------------------------------------------------------------
   // get increment type
   //
   btIncIdP  = clCmdParserP.value(clOptIncT).contains("I", Qt::CaseInsensitive);
   btIncDlcP = clCmdParserP.value(clOptIncT).contains("D", Qt::CaseInsensitive);
   btIncDataP= clCmdParserP.value(clOptIncT).contains("P", Qt::CaseInsensitive);
   
   //----------------------------------------------------------------
   // set host address for socket
   //
   if(clCmdParserP.isSet(clOptHostT))
   {
      QHostAddress clAddressT = QHostAddress(clCmdParserP.value(clOptHostT));
      clCanSocketP.setHostAddress(clAddressT);
   }

   //----------------------------------------------------------------
   // connect to CAN interface
   //
   clCanSocketP.connectNetwork((CAN_Channel_e) ubChannelP);

}

//----------------------------------------------------------------------------//
// sendFrame()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSend::sendFrame(void)
{
   QTime          clSystemTimeT;
   QCanTimeStamp  clCanTimeT;
   
   clSystemTimeT = QTime::currentTime();
   clCanTimeT.fromMilliSeconds(clSystemTimeT.msec());
   clCanFrameP.setTimeStamp(clCanTimeT);
   
   clCanSocketP.writeFrame(clCanFrameP);
   
   if (ulFrameCountP > 1)
   {
      ulFrameCountP--;
      
      //--------------------------------------------------------
      // test if identifier value must be incemented
      //
      if (btIncIdP)
      {
         ulFrameIdP++;
         
         //------------------------------------------------
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
         
         //------------------------------------------------
         // set new identifier value
         //
         clCanFrameP.setIdentifier(ulFrameIdP);
      }
      
      //--------------------------------------------------------
      // test if DLC value must be incemented
      //
      if (btIncDlcP)
      {
         ubFrameDlcP++;
         
         //------------------------------------------------
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
         //------------------------------------------------
         // set new DLC value
         //
         clCanFrameP.setDlc(ubFrameDlcP);
      }  
      
      //--------------------------------------------------------
      // test if data value must be incemented
      //
      if (btIncDataP)
      {
         clCanFrameP.setDataUInt32(0, clCanFrameP.dataUInt32(0) + 1);
      }
      
      QTimer::singleShot(ulFrameGapP, this, SLOT(sendFrame()));
   }
   else
   {
      QTimer::singleShot(50, this, SLOT(quit()));
   }

}


//----------------------------------------------------------------------------//
// socketConnected()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSend::socketConnected()
{
   //----------------------------------------------------------------
   // initial setup of CAN frame
   //
   clCanFrameP.setFrameFormat((QCanFrame::Format_e) ubFrameFormatP);
   clCanFrameP.setIdentifier(ulFrameIdP);
   clCanFrameP.setDlc(ubFrameDlcP);
   for(uint8_t ubCntT = 0; ubCntT < clCanFrameP.dataSize(); ubCntT++)
   {
      clCanFrameP.setData(ubCntT, aubFrameDataP[ubCntT]);
   }
    
   QTimer::singleShot(10, this, SLOT(sendFrame()));
}


//----------------------------------------------------------------------------//
// socketDisconnected()                                                       //
// show error message and quit                                                //
//----------------------------------------------------------------------------//
void QCanSend::socketDisconnected()
{
   qDebug() << "Disconnected from CAN " << ubChannelP;
   
}


//----------------------------------------------------------------------------//
// socketError()                                                              //
// show error message and quit                                                //
//----------------------------------------------------------------------------//
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
