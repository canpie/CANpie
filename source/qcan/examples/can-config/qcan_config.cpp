//============================================================================//
// File:          qcan_config.cpp                                             //
// Description:   Configure CAN interface                                     //
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

#include "qcan_config.hpp"

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
   QCoreApplication::setApplicationName("can-config");
   
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
   QCanConfig clMainT;

   
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
// QCanConfig()                                                                 //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanConfig::QCanConfig(QObject *parent) :
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
   
}

// shortly after quit is called the CoreApplication will signal this routine
// this is a good place to delete any objects that were created in the
// constructor and/or to stop any threads
void QCanConfig::aboutToQuitApp()
{
    // stop threads
    // sleep(1);   // wait for threads to stop.
    // delete any objects
}


//----------------------------------------------------------------------------//
// quit()                                                                     //
// call this routine to quit the application                                  //
//----------------------------------------------------------------------------//
void QCanConfig::quit()
{
   //qDebug() << "I will quit soon";
   clCanSocketP.disconnectNetwork();

   emit finished();
}


//----------------------------------------------------------------------------//
// runCmdParser()                                                             //
// 10ms after the application starts this method will parse all commands      //
//----------------------------------------------------------------------------//
void QCanConfig::runCmdParser(void)
{
   //----------------------------------------------------------------
   // setup command line parser
   //
   clCmdParserP.setApplicationDescription(tr("Configure CAN interface"));
   clCmdParserP.addHelpOption();
   
   //----------------------------------------------------------------
   // argument <interface> is required
   //
   clCmdParserP.addPositionalArgument("interface", 
                                      tr("CAN interface, e.g. can1"));

   //-----------------------------------------------------------
   // command line option: -a 
   //
   QCommandLineOption clOptAllT(QStringList() << "a" << "all", 
         tr("Show all CAN interfaces"));
   clCmdParserP.addOption(clOptAllT);
   
   //-----------------------------------------------------------
   // command line option: -H <host>
   //
   QCommandLineOption clOptHostT("H", 
         tr("Connect to <host>"),
         tr("host"));
   clCmdParserP.addOption(clOptHostT);
   
   //-----------------------------------------------------------
   // command line option: -m <mode>
   //
   QCommandLineOption clOptModeT(QStringList() << "m" << "mode", 
         tr("Set mode of CAN interface"),
         tr("start|stop|listen-only"));
   clCmdParserP.addOption(clOptModeT);
   
   //-----------------------------------------------------------
   // command line option: -nbtr <value>
   //
   QCommandLineOption clOptNomBtrT("nbtr", 
         tr("Set nominal bit-rate"),
         tr("value"));
   clCmdParserP.addOption(clOptNomBtrT);
   
   //-----------------------------------------------------------
   // command line option: -dbtr <value>
   //
   QCommandLineOption clOptDatBtrT("dbtr", 
         tr("Set data bit-rate"),
         tr("value"));
   clCmdParserP.addOption(clOptDatBtrT);

   
   
   clCmdParserP.addVersionOption();

   //----------------------------------------------------------------
   // Process the actual command line arguments given by the user
   //
   clCmdParserP.process(*pclAppP);
   
   //----------------------------------------------------------------
   // Test for --all option
   //
   if(clCmdParserP.isSet(clOptAllT))
   {
      quit();
   }
   
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
   // set bit-rate
   //
   btConfigBitrateP = false;
   slNomBitRateP = eCAN_BITRATE_NONE;
   slDatBitRateP = eCAN_BITRATE_NONE;
   
   if (clCmdParserP.isSet(clOptNomBtrT))
   {
      slNomBitRateP  = clCmdParserP.value(clOptNomBtrT).toInt(Q_NULLPTR, 10);    
      btConfigBitrateP = true;
   }
   
   if (clCmdParserP.isSet(clOptDatBtrT))
   {
      if (slNomBitRateP == eCAN_BITRATE_NONE)
      {
         fprintf(stderr, "%s \n\n", 
                 qPrintable(tr("Error: Must set nominal bit-rate also")));
         clCmdParserP.showHelp(0);         
      }
      else
      {
         slDatBitRateP  = clCmdParserP.value(clOptDatBtrT).toInt(Q_NULLPTR, 10);
      }
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
// sendFrame()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanConfig::sendCommand(void)
{
   if (btConfigBitrateP)
   {
      clCanApiP.setBitrate(slNomBitRateP, slDatBitRateP);
      clCanSocketP.writeFrame(clCanApiP);
   }
   
   QTimer::singleShot(50, this, SLOT(quit()));
}


//----------------------------------------------------------------------------//
// socketConnected()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanConfig::socketConnected()
{
   //----------------------------------------------------------------
   // initial setup of CAN frame
   //
    
   QTimer::singleShot(10, this, SLOT(sendCommand()));
}


//----------------------------------------------------------------------------//
// socketDisconnected()                                                       //
// show error message and quit                                                //
//----------------------------------------------------------------------------//
void QCanConfig::socketDisconnected()
{
   qDebug() << "Disconnected from CAN " << ubChannelP;
   
}


//----------------------------------------------------------------------------//
// socketError()                                                              //
// show error message and quit                                                //
//----------------------------------------------------------------------------//
void QCanConfig::socketError(QAbstractSocket::SocketError teSocketErrorV)
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
