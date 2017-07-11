//============================================================================//
// File:          qcan_dump.cpp                                               //
// Description:   Dump CAN messages                                           //
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

#include "qcan_dump.hpp"
#include <QCanFrameError>

#include <QDebug>



//----------------------------------------------------------------------------//
// main()                                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
int main(int argc, char *argv[])
{
   QCoreApplication clAppT(argc, argv);
   QCoreApplication::setApplicationName("can-dump");
   
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
   QCanDump clMainT;

   
   //----------------------------------------------------------------
   // connect the signals
   //
   QObject::connect(&clMainT, SIGNAL(finished()),
                    &clAppT,  SLOT(quit()));
   
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
// QCanDump()                                                                 //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanDump::QCanDump(QObject *parent) :
    QObject(parent)
{
   //----------------------------------------------------------------
   // get the instance of the main application
   //
   pclAppP = QCoreApplication::instance();

   
   //----------------------------------------------------------------
   // connect signals for socket operations
   //
   QObject::connect(&clCanSocketP, SIGNAL(connected()),
                    this, SLOT(socketConnected()));

   QObject::connect(&clCanSocketP, SIGNAL(disconnected()),
                    this, SLOT(socketDisconnected()));
   
   QObject::connect(&clCanSocketP, SIGNAL(error(QAbstractSocket::SocketError)),
                    this, SLOT(socketError(QAbstractSocket::SocketError)));

   QObject::connect(&clCanSocketP, SIGNAL(framesReceived(uint32_t)),
                    this, SLOT(socketReceive(uint32_t)));
}


// shortly after quit is called the CoreApplication will signal this routine
// this is a good place to delete any objects that were created in the
// constructor and/or to stop any threads
void QCanDump::aboutToQuitApp()
{
    // stop threads
    // sleep(1);   // wait for threads to stop.
    // delete any objects
}


//----------------------------------------------------------------------------//
// quit()                                                                     //
// call this routine to quit the application                                  //
//----------------------------------------------------------------------------//
void QCanDump::quit()
{
   if((btQuitNeverP == false) && (ulQuitTimeP > 0))
   {
      fprintf(stderr, "%s %d %s\n", 
              qPrintable(tr("Quit after")),
              ulQuitTimeP,
              qPrintable(tr("[ms] without frame reception.")));
   }
   
   emit finished();
}


//----------------------------------------------------------------------------//
// runCmdParser()                                                             //
// 10ms after the application starts this method will parse all commands      //
//----------------------------------------------------------------------------//
void QCanDump::runCmdParser()
{
   //----------------------------------------------------------------
   // setup command line parser
   //
   clCmdParserP.setApplicationDescription(tr("Show messages of CAN interface"));
   clCmdParserP.addHelpOption();
   clCmdParserP.addVersionOption();

   
   //----------------------------------------------------------------
   // argument <interface> is required
   //
   clCmdParserP.addPositionalArgument("interface", 
                                      tr("CAN interface, e.g. can1"));

   //-----------------------------------------------------------
   // command line option: -H <host>
   //
   QCommandLineOption clOptHostT("H", 
         tr("Connect to <host>"),
         tr("host"));
   clCmdParserP.addOption(clOptHostT);

   //-----------------------------------------------------------
   // command line option: -n <count>
   //
   QCommandLineOption clOptCountT("n", 
         tr("Terminate after reception of <count> CAN frames"),
         tr("count"));
   clCmdParserP.addOption(clOptCountT);
   
   //-----------------------------------------------------------
   // command line option: -t 
   //
   QCommandLineOption clOptTimeStampT("t", 
         tr("Show time-stamp"));
   clCmdParserP.addOption(clOptTimeStampT);

   //-----------------------------------------------------------
   // command line option: -T 
   //
   QCommandLineOption clOptTimeOutT("T", 
         tr("Terminate after <msec> without reception"),
         tr("msec"),
         "0");
   clCmdParserP.addOption(clOptTimeOutT);


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
   // check for time-stamp
   //
   btTimeStampP = clCmdParserP.isSet(clOptTimeStampT);

   
   //----------------------------------------------------------------
   // check for termination options
   //
   btQuitNeverP = true;
   ulQuitCountP = clCmdParserP.value(clOptCountT).toInt(Q_NULLPTR, 10);    
   ulQuitTimeP  = clCmdParserP.value(clOptTimeOutT).toInt(Q_NULLPTR, 10);      
   if ((ulQuitCountP > 0) || (ulQuitTimeP > 0))
   {
      btQuitNeverP = false; 
   }
   
   
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
// socketConnected()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanDump::socketConnected()
{
   qDebug() << "Connected to CAN " << ubChannelP;
   qDebug() << "Quit time" << ulQuitTimeP;
   
   if ((btQuitNeverP == false) && (ulQuitTimeP > 0))
   {
      clActivityTimerP.setInterval(ulQuitTimeP);
      clActivityTimerP.setSingleShot(true);
      connect(&clActivityTimerP, SIGNAL(timeout()), SLOT(quit()));
      clActivityTimerP.start();
   }
}


//----------------------------------------------------------------------------//
// socketDisconnected()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanDump::socketDisconnected()
{
   qDebug() << "Disconnected from CAN " << ubChannelP;
   
}


//----------------------------------------------------------------------------//
// socketError()                                                              //
// show error message and quit                                                //
//----------------------------------------------------------------------------//
void QCanDump::socketError(QAbstractSocket::SocketError teSocketErrorV)
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


//----------------------------------------------------------------------------//
// socketReceive()                                                            //
// show messages that are available                                           //
//----------------------------------------------------------------------------//
void QCanDump::socketReceive(uint32_t ulFrameCntV)
{
   QByteArray     clCanDataT;
   QCanFrame      clCanFrameT;
   QCanFrameApi   clCanApiT;
   QCanFrameError clCanErrorT;
   QString        clCanStringT;
   QCanData::Type_e  ubFrameTypeT;
   
   if ((btQuitNeverP == false) && (ulQuitTimeP > 0))
   {
      clActivityTimerP.start(ulQuitTimeP);
   }
   
   while(ulFrameCntV)
   {
      if(clCanSocketP.read(clCanDataT, &ubFrameTypeT) == true)
      {
         switch(ubFrameTypeT)
         {
            case QCanData::eTYPE_API:
               if (clCanApiT.fromByteArray(clCanDataT) == true)
               {
                  clCanStringT = clCanApiT.toString(btTimeStampP);
                  fprintf(stderr, "%s\n", qPrintable(clCanStringT));
               }
               break;
               
            case QCanData::eTYPE_CAN:
               if (clCanFrameT.fromByteArray(clCanDataT) == true)
               {
                  clCanStringT = clCanFrameT.toString(btTimeStampP);
                  fprintf(stderr, "%s\n", qPrintable(clCanStringT));
               }
               break;

            case QCanData::eTYPE_ERROR:
               if (clCanErrorT.fromByteArray(clCanDataT) == true)
               {
                  clCanStringT = clCanErrorT.toString(btTimeStampP);
                  fprintf(stderr, "%s\n", qPrintable(clCanStringT));
               }
               break;
            default:

               break;
               
         }
      }
      ulFrameCntV--;
      ulQuitCountP--;
      if(ulQuitCountP == 0)
      {
         quit();
      }
   }
}









