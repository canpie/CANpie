//====================================================================================================================//
// File:          qcan_error.cpp                                                                                      //
// Description:   Send CAN error messages                                                                             //
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

#include "qcan_error.hpp"

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtCore/QTimer>


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------------------------------
// Version information is controlled via cmake project file, the following defintions are only
// placeholders
//
#ifndef  VERSION_MAJOR
#define  VERSION_MAJOR                       1
#endif

#ifndef  VERSION_MINOR
#define  VERSION_MINOR                       0
#endif

#ifndef  VERSION_BUILD
#define  VERSION_BUILD                       0
#endif


//--------------------------------------------------------------------------------------------------------------------//
// main()                                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
int main(int argc, char *argv[])
{
   QCoreApplication clAppT(argc, argv);
   QCoreApplication::setApplicationName("can-error");
   
   //---------------------------------------------------------------------------------------------------
   // get application version (defined in cmake file)
   //
   QString clVersionT = "version ";
   clVersionT += QString("%1.").arg(VERSION_MAJOR);
   clVersionT += QString("%1.").arg(VERSION_MINOR, 2, 10, QLatin1Char('0'));
   clVersionT += QString("%1").arg(VERSION_BUILD, 2, 10, QLatin1Char('0'));
   QCoreApplication::setApplicationVersion(clVersionT);


   //---------------------------------------------------------------------------------------------------
   // create the main class
   //
   QCanError clMainT;

   
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
// QCanError()                                                                                                        //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanError::QCanError(QObject *parent) :
    QObject(parent)
{
   //---------------------------------------------------------------------------------------------------
   // get instance of the main application
   //
   pclAppP = QCoreApplication::instance();

   
   //---------------------------------------------------------------------------------------------------
   // set default values
   //
   teCanChannelP  = QCan::eCAN_CHANNEL_NONE;

   ubRcvErrorCntP = 0;
   ubTrmErrorCntP = 0;

   btDecRcvErrorP = false;
   btDecTrmErrorP = false;

   btIncRcvErrorP = false;
   btIncTrmErrorP = false;

   ulFrameGapP    = 0;
   ulFrameCountP  = 0;

   teErrorTypeP   = QCanFrame::eERROR_TYPE_NONE;

   //---------------------------------------------------------------------------------------------------
   // connect signals for socket operations
   //
   QObject::connect(&clCanSocketP, SIGNAL(connected()),
                    this, SLOT(socketConnected()));

   QObject::connect(&clCanSocketP, SIGNAL(disconnected()),
                    this, SLOT(socketDisconnected()));
   
   QObject::connect(&clCanSocketP, SIGNAL(error(QAbstractSocket::SocketError)),
                    this, SLOT(socketError(QAbstractSocket::SocketError)));
   
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanError::aboutToQuitApp()                                                                                        //
// shortly after quit is called the CoreApplication will signal this routine: delete objects / clean up               //
//--------------------------------------------------------------------------------------------------------------------//
void QCanError::aboutToQuitApp()
{

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanError::quit()                                                                                                  //
// call this routine to quit the application                                                                          //
//--------------------------------------------------------------------------------------------------------------------//
void QCanError::quit()
{
   clCanSocketP.disconnectNetwork();

   emit finished();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanError::runCmdParser()                                                                                          //
// 10ms after the application starts this method will parse all commands                                              //
//--------------------------------------------------------------------------------------------------------------------//
void QCanError::runCmdParser(void)
{
   //---------------------------------------------------------------------------------------------------
   // setup command line parser
   //
   clCmdParserP.setApplicationDescription(tr("Send error messages to CANpie server"));
   clCmdParserP.addHelpOption();
   
   //---------------------------------------------------------------------------------------------------
   // argument <interface> is required
   //
   clCmdParserP.addPositionalArgument("interface", 
                                      tr("CAN interface, e.g. can1"));

   
   //---------------------------------------------------------------------------------------------------
   // command line option: -e <type>
   //
   QCommandLineOption clOptErrTypeT("e",
         tr("Set error type to [NONE|BIT0|BIT1|STUFF|FORM|CRC|ACK]"),
         tr("type"),
         "NONE");       // default value
   clCmdParserP.addOption(clOptErrTypeT);

   //---------------------------------------------------------------------------------------------------
   // command line option: -g <msec>
   //
   QCommandLineOption clOptGapT("g", 
         tr("Time gap in milli-seconds between multiple error frames"),
         tr("gap"),
         "10");          // default value
   clCmdParserP.addOption(clOptGapT);
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -H <host>
   //
   QCommandLineOption clOptHostT("H", 
         tr("Connect to <host>"),
         tr("host"));
   clCmdParserP.addOption(clOptHostT);
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -n <count>
   //
   QCommandLineOption clOptCountT("n", 
         tr("Terminate after transmission of <count> error frames"),
         tr("count"),
         "1");          // default value
   clCmdParserP.addOption(clOptCountT);

   //---------------------------------------------------------------------------------------------------
   // command line option: -R <value>
   //
   QCommandLineOption clOptRcvErrCountT("R",
         tr("Set receive error counter to <value>"),
         tr("value"),
         "0");          // default value
   clCmdParserP.addOption(clOptRcvErrCountT);

   //---------------------------------------------------------------------------------------------------
   // command line option: -T <value>
   //
   QCommandLineOption clOptTrmErrCountT("T",
         tr("Set transmit error counter to <value>"),
         tr("value"),
         "0");          // default value
   clCmdParserP.addOption(clOptTrmErrCountT);
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -i <type>
   //
   QCommandLineOption clOptIncT("i",
         tr("Increment the requested error counter"),
         tr("R|T"));
   clCmdParserP.addOption(clOptIncT);
   
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
   if((btConversionSuccessT == false) || (slChannelT == 0) )
   {
      fprintf(stderr, "%s \n\n", qPrintable(tr("Error: CAN interface out of range")));
      clCmdParserP.showHelp(0);
   }
   
   //---------------------------------------------------------------------------------------------------
   // store CAN interface channel (CAN_Channel_e)
   //
   teCanChannelP = static_cast< QCan::CAN_Channel_e >(slChannelT);

   //---------------------------------------------------------------------------------------------------
   // get error type
   //
   if (clCmdParserP.value(clOptErrTypeT).contains("NONE", Qt::CaseInsensitive))
   {
      teErrorTypeP = QCanFrame::eERROR_TYPE_NONE;
   }
   else if (clCmdParserP.value(clOptErrTypeT).contains("BIT0", Qt::CaseInsensitive))
   {
      teErrorTypeP = QCanFrame::eERROR_TYPE_BIT0;
   }
   else if (clCmdParserP.value(clOptErrTypeT).contains("BIT1", Qt::CaseInsensitive))
   {
      teErrorTypeP = QCanFrame::eERROR_TYPE_BIT1;
   }
   else if (clCmdParserP.value(clOptErrTypeT).contains("STUFF", Qt::CaseInsensitive))
   {
      teErrorTypeP = QCanFrame::eERROR_TYPE_STUFF;
   }
   else if (clCmdParserP.value(clOptErrTypeT).contains("FORM", Qt::CaseInsensitive))
   {
      teErrorTypeP = QCanFrame::eERROR_TYPE_FORM;
   }
   else if (clCmdParserP.value(clOptErrTypeT).contains("CRC", Qt::CaseInsensitive))
   {
      teErrorTypeP = QCanFrame::eERROR_TYPE_CRC;
   }
   else if (clCmdParserP.value(clOptErrTypeT).contains("ACK", Qt::CaseInsensitive))
   {
      teErrorTypeP = QCanFrame::eERROR_TYPE_ACK;
   }
   else
   {
      fprintf(stderr, "%s \n\n", qPrintable(tr("Error: Unknown option for error type.")));
      clCmdParserP.showHelp(0);
   }


   //---------------------------------------------------------------------------------------------------
   // get receive error counter
   //
   ubRcvErrorCntP = static_cast< uint8_t >(clCmdParserP.value(clOptRcvErrCountT).toInt(nullptr, 10));

   //---------------------------------------------------------------------------------------------------
   // get transmit error counter
   //
   ubTrmErrorCntP = static_cast< uint8_t >(clCmdParserP.value(clOptTrmErrCountT).toInt(nullptr, 10));

   //---------------------------------------------------------------------------------------------------
   // get number of frames to send
   //
   ulFrameCountP  = static_cast< uint32_t >(clCmdParserP.value(clOptCountT).toInt(nullptr, 10));

   //---------------------------------------------------------------------------------------------------
   // get time gap between frames
   //
   ulFrameGapP    = static_cast< uint32_t >(clCmdParserP.value(clOptGapT).toInt(nullptr, 10));
   
   
   //---------------------------------------------------------------------------------------------------
   // set host address for socket
   //
   if(clCmdParserP.isSet(clOptHostT))
   {
      QHostAddress clAddressT = QHostAddress(clCmdParserP.value(clOptHostT));
      clCanSocketP.setHostAddress(clAddressT);
   }

   //---------------------------------------------------------------------------------------------------
   // connect to CAN interface
   //
   clCanSocketP.connectNetwork(teCanChannelP);

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanError::sendErrorFrame()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanError::sendErrorFrame(void)
{
   QTime          clSystemTimeT;
   QCanTimeStamp  clCanTimeT;
   
   clSystemTimeT = QTime::currentTime();
   clCanTimeT.fromMilliSeconds(static_cast< uint32_t>(clSystemTimeT.msec()));
   clErrorFrameP.setTimeStamp(clCanTimeT);
   clCanSocketP.write(clErrorFrameP);
   
   if (ulFrameCountP > 1)
   {
      ulFrameCountP--;
      
      
      QTimer::singleShot(static_cast< int32_t>(ulFrameGapP), this, SLOT(sendErrorFrame()));
   }
   else
   {
      QTimer::singleShot(50, this, SLOT(quit()));
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanError::socketConnected()                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanError::socketConnected()
{
   //----------------------------------------------------------------
   // initial setup of CAN error frame
   //
   clErrorFrameP.setFrameType(QCanFrame::eFRAME_TYPE_ERROR);
   clErrorFrameP.setErrorCounterReceive(ubRcvErrorCntP);
   clErrorFrameP.setErrorCounterTransmit(ubTrmErrorCntP);
   clErrorFrameP.setErrorType(teErrorTypeP);

   //----------------------------------------------------------------
   // error state depends on counter values
   //
   if ((ubRcvErrorCntP >= 96) || (ubTrmErrorCntP >= 96))
   {
      clErrorFrameP.setErrorState(QCan::eCAN_STATE_BUS_WARN);
   }

   if ((ubRcvErrorCntP >= 127) || (ubTrmErrorCntP >= 127))
   {
      clErrorFrameP.setErrorState(QCan::eCAN_STATE_BUS_PASSIVE);
   }

   if (ubTrmErrorCntP == 255)
   {
      clErrorFrameP.setErrorState(QCan::eCAN_STATE_BUS_OFF);
   }

   QTimer::singleShot(10, this, SLOT(sendErrorFrame()));
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanError::socketDisconnected()                                                                                    //
// show error message and quit                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanError::socketDisconnected()
{
   
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanError::socketError()                                                                                           //
// show error message and quit                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanError::socketError(QAbstractSocket::SocketError teSocketErrorV)
{
   Q_UNUSED(teSocketErrorV);  // parameter not used 
   
   //---------------------------------------------------------------------------------------------------
   // show error message in case the connection to the network fails
   //
   fprintf(stderr, "%s %s\n", 
           qPrintable(tr("Failed to connect to CAN interface:")),
           qPrintable(clCanSocketP.errorString()));
   quit();
}
