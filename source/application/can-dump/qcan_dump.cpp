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

#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>

#include "qcan_dump.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------------------------------
// Version information is controller via qmake project file, the following defintions are only
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
   // connect the signals between QCoreApplication and the main class 
   //
   QObject::connect(&clMainT, &QCanDump::finished, &clAppT,  &QCoreApplication::quit);
   
   QObject::connect(&clAppT,  &QCoreApplication::aboutToQuit, &clMainT, &QCanDump::aboutToQuitApp);

   
   //---------------------------------------------------------------------------------------------------
   // This code will start the messaging engine in QT and in 10 ms it will start the execution of the
   // clMainT.runCommandParser() routine.
   //
   QTimer::singleShot(10, &clMainT, SLOT(runCommandParser()));

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
   pclApplicationP = QCoreApplication::instance();

   
   //---------------------------------------------------------------------------------------------------
   // connect signals for socket operations
   //
   QObject::connect( &clCanSocketP, &QCanSocket::connected,     
                     this, &QCanDump::onSocketConnected);

   QObject::connect( &clCanSocketP, &QCanSocket::disconnected,  
                     this, &QCanDump::onSocketDisconnected);
   
   QObject::connect( &clCanSocketP, QOverload<QAbstractSocket::SocketError>::of(&QCanSocket::error), 
                     this, &QCanDump::onSocketError);

   QObject::connect( &clCanSocketP, &QCanSocket::readyRead,     
                     this, &QCanDump::onSocketReceive);

   //---------------------------------------------------------------------------------------------------
   // connect QServerSettings object
   //
   connect(&clServerSettingsP, &QCanServerSettings::stateChanged, 
           this,               &QCanDump::onServerStateChanged);

   connect(&clServerSettingsP, &QCanServerSettings::objectReceived, 
           this,               &QCanDump::onServerObjectReceived);

   //---------------------------------------------------------------------------------------------------
   // set default values
   //
   teCanChannelP        = eCAN_CHANNEL_NONE;
   btTimeStampP         = false;
   btErrorFramesP       = false;
   btQuitNeverP         = false;
   ulQuitTimeP          = 0;
   ulQuitCountP         = 0;

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::aboutToQuitApp()                                                                                         //
// shortly after quit is called the CoreApplication will signal this routine: delete objects / clean up               //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::aboutToQuitApp()
{

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::onNetworkObjectReceived()                                                                                //
// print details about CAN interface and connect to interface                                                         //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::onNetworkObjectReceived(const CAN_Channel_e teChannelV, QJsonObject clNetworkConfigV)
{
   Q_UNUSED (clNetworkConfigV);

   //---------------------------------------------------------------------------------------------------
   // remove all signals from this class
   //
   disconnect(&clNetworkSettingsP);


   if (teChannelV == teCanChannelP)
   {
      fprintf(stdout, "--------------------------------------------------------------------------------\n");

      //-------------------------------------------------------------------------------------------
      // network name
      //
      fprintf(stdout, "CAN %d         : %s \n", teChannelV,
              qPrintable(clNetworkSettingsP.name()) );


      //-------------------------------------------------------------------------------------------
      // network state
      //
      fprintf(stdout, "CAN state     : %s \n",
              qPrintable(clNetworkSettingsP.stateString()) );

      //-------------------------------------------------------------------------------------------
      // bit-rate settings
      //
      if (clNetworkSettingsP.dataBitrate() == eCAN_BITRATE_NONE)
      {
         fprintf(stdout, "Bit-rate      : %s \n",
                 qPrintable(clNetworkSettingsP.nominalBitrateString()) );
      }
      else
      {
         fprintf(stdout, "Bit-rate      : %s (nominal), %s (data) \n",
                 qPrintable(clNetworkSettingsP.nominalBitrateString()),
                 qPrintable(clNetworkSettingsP.dataBitrateString())        );
      }
      fprintf(stdout, "--------------------------------------------------------------------------------\n");


      //-------------------------------------------------------------------------------------------
      // since this slot has been called, we can connect to the network
      //
      clCanSocketP.connectNetwork(teCanChannelP);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::onServerObjectReceived()                                                                                 //
// print version of CANpie FD Server                                                                                  //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::onServerObjectReceived(QJsonObject clServerConfigV)
{
   Q_UNUSED(clServerConfigV);

   fprintf( stdout, "%s %d.%02d.%02d \n",
            qPrintable(tr("Connected to CANpie FD server, version:")),
            clServerSettingsP.versionMajor(),
            clServerSettingsP.versionMinor(),
            clServerSettingsP.versionBuild() );

   clNetworkSettingsP.setChannel(teCanChannelP);
   clNetworkSettingsP.connectToServer();

   connect(&clNetworkSettingsP, &QCanNetworkSettings::objectReceived, 
           this,                &QCanDump::onNetworkObjectReceived);


}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::onServerStateChanged()                                                                                   //
// check for new state of the CANpie FD Server                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::onServerStateChanged(enum QCanServerSettings::State_e teStateV)
{  
   bool btQuitProgramT = false;

   qDebug() << " QCanDump::onServerStateChanged()" << teStateV;

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
// QCanDump::onSocketConnected()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::onSocketConnected()
{

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
void QCanDump::onSocketDisconnected()
{
      
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanDump::socketError()                                                                                            //
// Show error message and quit                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::onSocketError(QAbstractSocket::SocketError teSocketErrorV)
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
void QCanDump::onSocketReceive(void)
{
   QCanFrame      clCanFrameT;
   QString        clCanStringT;
   int32_t        slFrameCountT = 0;

   qDebug() << "QCanDump::onSocketReceive()";
   
   if ((btQuitNeverP == false) && (ulQuitTimeP > 0))
   {
      clActivityTimerP.start(ulQuitTimeP);
   }
   
   slFrameCountT = clCanSocketP.framesAvailable();
   while (slFrameCountT)
   {
      if (clCanSocketP.read(clCanFrameT) == true)
      {
         if (clFilterListP.filter(clCanFrameT) == false)
         {
            clCanStringT = clCanFrameT.toString(btTimeStampP);
            fprintf(stdout, "%s\n", qPrintable(clCanStringT));
            ulQuitCountP--;
            if (ulQuitCountP == 0)
            {
               quit();
            }
         }
      }
      slFrameCountT--;
   }
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
// QCanDump::runCommandParser()                                                                                       //
// 10ms after the application starts this method will parse all commands                                              //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::runCommandParser()
{
   //---------------------------------------------------------------------------------------------------
   // setup command line parser, options are added in alphabetical order
   //
   clCommandParserP.setApplicationDescription(tr("Show messages of CAN interface"));


   //---------------------------------------------------------------------------------------------------
   // command line option: -h, --help
   //
   clCommandParserP.addHelpOption();

   
   //---------------------------------------------------------------------------------------------------
   // argument <interface> is required
   //
   clCommandParserP.addPositionalArgument("interface", 
                                      tr("CAN interface, e.g. can1"));

   //---------------------------------------------------------------------------------------------------
   // command line option: -H <host>
   //
   QCommandLineOption clOptHostT(QStringList() << "H" << "host",
         tr("Connect to <host>"),
         tr("host"));
   clCommandParserP.addOption(clOptHostT);

   //---------------------------------------------------------------------------------------------------
   // command line option: --id-accept <Identifier list>
   //
   QCommandLineOption clOptIdAcceptT("id-accept",
         tr("Accept CAN frames with identifier <id>"),
         tr("id"));
   clCommandParserP.addOption(clOptIdAcceptT);

   //---------------------------------------------------------------------------------------------------
   // command line option: --id-reject <Identifier list>
   //
   QCommandLineOption clOptIdRejectT("id-reject",
         tr("Reject CAN frames with identifier <id>"),
         tr("id"));
   clCommandParserP.addOption(clOptIdRejectT);

   //---------------------------------------------------------------------------------------------------
   // command line option: -n <count>
   //
   QCommandLineOption clOptCountT("n", 
         tr("Terminate after reception of <count> CAN frames"),
         tr("count"));
   clCommandParserP.addOption(clOptCountT);
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -t 
   //
   QCommandLineOption clOptTimeStampT(QStringList() << "t" << "timestamp",
         tr("Show time-stamp"));
   clCommandParserP.addOption(clOptTimeStampT);

   //---------------------------------------------------------------------------------------------------
   // command line option: -T 
   //
   QCommandLineOption clOptTimeOutT("T", 
         tr("Terminate after <msec> without reception"),
         tr("msec"),
         "0");
   clCommandParserP.addOption(clOptTimeOutT);

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
   if (!clInterfaceT.startsWith("can"))
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
   if ((btConversionSuccessT == false) ||
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
   
   //---------------------------------------------------------------------------------------------------
   // check for time-stamp
   //
   btTimeStampP = clCommandParserP.isSet(clOptTimeStampT);

   //---------------------------------------------------------------------------------------------------
   // check for termination options
   //
   btQuitNeverP = true;
   ulQuitCountP = clCommandParserP.value(clOptCountT).toInt(Q_NULLPTR, 10);    
   ulQuitTimeP  = clCommandParserP.value(clOptTimeOutT).toInt(Q_NULLPTR, 10);      
   if ((ulQuitCountP > 0) || (ulQuitTimeP > 0))
   {
      btQuitNeverP = false; 
   }
   
   

   //---------------------------------------------------------------------------------------------------
   // check for filter (reject)
   //
   if (clCommandParserP.isSet(clOptIdRejectT))
   {
      QStringList clRejectIdsT = clCommandParserP.values(clOptIdRejectT);

      QCanFilter clFilterRejectT;
      for (int32_t slRejectNumT = 0; slRejectNumT < clRejectIdsT.size(); slRejectNumT++)
      {
         clFilterRejectT.rejectFrame(QCanFrame::eFORMAT_CAN_STD,
                                     clRejectIdsT.at(slRejectNumT).toInt(Q_NULLPTR, 16),
                                     clRejectIdsT.at(slRejectNumT).toInt(Q_NULLPTR, 16));

         clFilterListP.appendFilter(clFilterRejectT);
      }

   }


   //---------------------------------------------------------------------------------------------------
   // check for filter (accept)
   //
   if (clCommandParserP.isSet(clOptIdAcceptT))
   {
      QStringList clAcceptIdsT = clCommandParserP.values(clOptIdAcceptT);

      QCanFilter clFilterAcceptT;
      for (int32_t slAcceptNumT = 0; slAcceptNumT < clAcceptIdsT.size(); slAcceptNumT++)
      {
         clFilterAcceptT.acceptFrame(QCanFrame::eFORMAT_CAN_STD,
               clAcceptIdsT.at(slAcceptNumT).toInt(Q_NULLPTR, 16),
               clAcceptIdsT.at(slAcceptNumT).toInt(Q_NULLPTR, 16));

         clFilterListP.appendFilter(clFilterAcceptT);
      }

   }
   
   //---------------------------------------------------------------------------------------------------
   // connect to QCanServer class (i.e. CANpie FD Server)
   //
   clServerSettingsP.connectToServer(clHostAddressP);

}
