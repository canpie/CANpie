//====================================================================================================================//
// File:          qcan_dump.cpp                                                                                       //
// Description:   Dump CAN messages                                                                                   //
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

#include "qcan_dump.hpp"

#include "qcan_server_settings.hpp"

#include <QtCore/QDebug>



//--------------------------------------------------------------------------------------------------------------------//
// main()                                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
int main(int argc, char *argv[])
{
   QCoreApplication clAppT(argc, argv);
   QCoreApplication::setApplicationName("can-dump");
   
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
   QCanDump clMainT;

   
   //---------------------------------------------------------------------------------------------------
   // connect the signals
   //
   QObject::connect(&clMainT, SIGNAL(finished()),
                    &clAppT,  SLOT(quit()));
   
   QObject::connect(&clAppT, SIGNAL(aboutToQuit()),
                    &clMainT, SLOT(aboutToQuitApp()));

   
   //---------------------------------------------------------------------------------------------------
   // This code will start the messaging engine in QT and in 10 ms it will start the execution of the
   // clMainT.runCmdParser() routine.
   //
   QTimer::singleShot(10, &clMainT, SLOT(runCmdParser()));

   clAppT.exec();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump()                                                                                                         //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanDump::QCanDump(QObject *parent) :
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

   QObject::connect(&clCanSocketP, SIGNAL(framesReceived(uint32_t)),
                    this, SLOT(socketReceive(uint32_t)));

   //---------------------------------------------------------------------------------------------------
   // set default values
   //
   ubChannelP         = eCAN_CHANNEL_NONE;
   btTimeStampP       = false;
   btErrorFramesP     = false;
   btQuitNeverP       = false;
   ulQuitTimeP        =  0;
   ulQuitCountP       = 0;
   btIsTcpConnectionP = false;

   //---------------------------------------------------------------------------------------------------
   // create QServerSettings object
   //
   pclServerP = new QCanServerSettings();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::aboutToQuitApp()                                                                                         //
// shortly after quit is called the CoreApplication will signal this routine: delete objects / clean up               //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::aboutToQuitApp()
{
   delete (pclServerP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::quit()                                                                                                   //
// call this routine to quit the application                                                                          //
//--------------------------------------------------------------------------------------------------------------------//
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


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::runCmdParser()                                                                                           //
// 10ms after the application starts this method will parse all commands                                              //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::runCmdParser()
{
   //---------------------------------------------------------------------------------------------------
   // setup command line parser, options are added in alphabetical order
   //
   clCmdParserP.setApplicationDescription(tr("Show messages of CAN interface"));


   //---------------------------------------------------------------------------------------------------
   // command line option: -h, --help
   //
   clCmdParserP.addHelpOption();

   
   //---------------------------------------------------------------------------------------------------
   // argument <interface> is required
   //
   clCmdParserP.addPositionalArgument("interface", 
                                      tr("CAN interface, e.g. can1"));

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
         tr("Terminate after reception of <count> CAN frames"),
         tr("count"));
   clCmdParserP.addOption(clOptCountT);
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -t 
   //
   QCommandLineOption clOptTimeStampT("t", 
         tr("Show time-stamp"));
   clCmdParserP.addOption(clOptTimeStampT);

   //---------------------------------------------------------------------------------------------------
   // command line option: -T 
   //
   QCommandLineOption clOptTimeOutT("T", 
         tr("Terminate after <msec> without reception"),
         tr("msec"),
         "0");
   clCmdParserP.addOption(clOptTimeOutT);

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
   // check for time-stamp
   //
   btTimeStampP = clCmdParserP.isSet(clOptTimeStampT);

   
   //---------------------------------------------------------------------------------------------------
   // check for termination options
   //
   btQuitNeverP = true;
   ulQuitCountP = clCmdParserP.value(clOptCountT).toInt(Q_NULLPTR, 10);    
   ulQuitTimeP  = clCmdParserP.value(clOptTimeOutT).toInt(Q_NULLPTR, 10);      
   if ((ulQuitCountP > 0) || (ulQuitTimeP > 0))
   {
      btQuitNeverP = false; 
   }
   
   
   //---------------------------------------------------------------------------------------------------
   // set host address for socket
   //
   if (clCmdParserP.isSet(clOptHostT))
   {
      QHostAddress clAddressT = QHostAddress(clCmdParserP.value(clOptHostT));
      clCanSocketP.setHostAddress(clAddressT);
      btIsTcpConnectionP = true;
   }
   else
   {
      //-------------------------------------------------------------------------------------------
      // check to local server state and print error if it is not active
      //
      if (pclServerP->state() < QCanServerSettings::eSTATE_ACTIVE)
      {
         fprintf(stdout, "CANpie FD server %s \n", qPrintable(pclServerP->stateString()));
         exit(0);
      }
   }

   
   //---------------------------------------------------------------------------------------------------
   // connect to CAN interface
   //
   clCanSocketP.connectNetwork((CAN_Channel_e) ubChannelP);

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::showNetworkSettings()                                                                                    //
// show settings of selected CAN network                                                                              //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::showNetworkSettings(CAN_Channel_e teCanChannelV)
{
   if (teCanChannelV <= pclServerP->networkCount())
   {
      fprintf(stdout, "--------------------------------------------------------------------------------\n");

      //-------------------------------------------------------------------------------------------
      // network name
      //
      fprintf(stdout, "CAN %d         : %s \n", teCanChannelV,
              qPrintable(pclServerP->networkName(teCanChannelV)) );

      //-------------------------------------------------------------------------------------------
      // network configuration
      //
      fprintf(stdout, "Configuration : %s \n",
              qPrintable(pclServerP->networkConfigurationString(teCanChannelV)) );

      //-------------------------------------------------------------------------------------------
      // network state
      //
      fprintf(stdout, "CAN state     : %s \n",
              qPrintable(pclServerP->networkStateString(teCanChannelV)) );

      //-------------------------------------------------------------------------------------------
      // bit-rate settings
      //
      if (pclServerP->networkDataBitrate() == eCAN_BITRATE_NONE)
      {
         fprintf(stdout, "Bit-rate      : %s \n",
                 qPrintable(pclServerP->networkNominalBitrateString(teCanChannelV)) );
      }
      else
      {
         fprintf(stdout, "Bit-rate      : %s (nominal), %s (data) \n",
                 qPrintable(pclServerP->networkNominalBitrateString(teCanChannelV)),
                 qPrintable(pclServerP->networkDataBitrateString(teCanChannelV))        );
      }

      fprintf(stdout, "--------------------------------------------------------------------------------\n");
   }
   else
   {
      fprintf(stdout, "CAN interface not available\n");
   }

   fprintf(stdout, "\n");
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::socketConnected()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::socketConnected()
{
   qDebug() << "Connected to CAN " << ubChannelP;
   qDebug() << "Quit time" << ulQuitTimeP;

   if (btIsTcpConnectionP == false)
   {
      showNetworkSettings((CAN_Channel_e) ubChannelP);
   }
   else
   {
      fprintf(stdout, "--------------------------------------------------------------------------------\n");
      fprintf(stdout, "Connected to remote server\n");
      fprintf(stdout, "--------------------------------------------------------------------------------\n");
   }

   if ((btQuitNeverP == false) && (ulQuitTimeP > 0))
   {
      clActivityTimerP.setInterval(ulQuitTimeP);
      clActivityTimerP.setSingleShot(true);
      connect(&clActivityTimerP, SIGNAL(timeout()), SLOT(quit()));
      clActivityTimerP.start();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::socketDisconnected()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::socketDisconnected()
{
   qDebug() << "Disconnected from CAN " << ubChannelP;
   
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::socketError()                                                                                            //
// Show error message and quit                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::socketError(QAbstractSocket::SocketError teSocketErrorV)
{
   Q_UNUSED (teSocketErrorV);
   
   //---------------------------------------------------------------------------------------------------
   // show error message in case the connection to the network fails
   //
   fprintf(stderr, "%s %s\n", 
           qPrintable(tr("Connection to CAN network failed:")),
           qPrintable(clCanSocketP.errorString()));
   quit();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::socketReceive()                                                                                          //
// Show messages that are available                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::socketReceive(uint32_t ulFrameCntV)
{
   QCanFrame      clCanFrameT;
   QString        clCanStringT;
   
   if ((btQuitNeverP == false) && (ulQuitTimeP > 0))
   {
      clActivityTimerP.start(ulQuitTimeP);
   }
   
   while(ulFrameCntV)
   {
      if (clCanSocketP.read(clCanFrameT) == true)
      {
         clCanStringT = clCanFrameT.toString(btTimeStampP);
         fprintf(stderr, "%s\n", qPrintable(clCanStringT));
      }
      ulFrameCntV--;
      ulQuitCountP--;
      if(ulQuitCountP == 0)
      {
         quit();
      }
   }
}
