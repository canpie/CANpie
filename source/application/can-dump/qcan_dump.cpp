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
   QObject::connect( &clCanSocketP, &QCanSocket::connected,     
                     this, &QCanDump::onSocketConnected);

   QObject::connect( &clCanSocketP, &QCanSocket::disconnected,  
                     this, &QCanDump::onSocketDisconnected);
   
   QObject::connect( &clCanSocketP, QOverload<QAbstractSocket::SocketError>::of(&QCanSocket::error),         
                     this, &QCanDump::onSocketError);

   QObject::connect( &clCanSocketP, &QCanSocket::readyRead,     
                     this, &QCanDump::onSocketReceive);

   //---------------------------------------------------------------------------------------------------
   // create QServerSettings object
   //
   pclServerSettingsP = new QCanServerSettings();

   connect(pclServerSettingsP, &QCanServerSettings::stateChanged, 
           this,               &QCanDump::onServerStateChanged);

   connect(pclServerSettingsP, &QCanServerSettings::objectReceived, 
           this,               &QCanDump::onServerObjectReceived);

   //---------------------------------------------------------------------------------------------------
   // set default values
   //
   teChannelP           = eCAN_CHANNEL_NONE;
   btTimeStampP         = false;
   btErrorFramesP       = false;
   btQuitNeverP         = false;
   ulQuitTimeP          = 0;
   ulQuitCountP         = 0;
   btIsWebConnectionP   = false;

   pclNetworkSettingsP  = Q_NULLPTR;
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
// print details about CAN interface                                                                                  //
//--------------------------------------------------------------------------------------------------------------------//
void QCanDump::onNetworkObjectReceived(const CAN_Channel_e teChannelV, QJsonObject clNetworkConfigV)
{
   // Q_UNUSED(clNetworkConfigV);
   
   //---------------------------------------------------------------------------------------------------
   // remove all signals from this class
   //
   disconnect(pclNetworkSettingsP);


   if (teChannelV == teChannelP)
   {
      fprintf(stdout, "--------------------------------------------------------------------------------\n");

      //-------------------------------------------------------------------------------------------
      // network name
      //
      fprintf(stdout, "CAN %d         : %s \n", teChannelV,
              qPrintable(pclNetworkSettingsP->name()) );


      //-------------------------------------------------------------------------------------------
      // network state
      //
      fprintf(stdout, "CAN state     : %s \n",
              qPrintable(pclNetworkSettingsP->stateString()) );

      //-------------------------------------------------------------------------------------------
      // bit-rate settings
      //
      if (pclNetworkSettingsP->dataBitrate() == eCAN_BITRATE_NONE)
      {
         fprintf(stdout, "Bit-rate      : %s \n",
                 qPrintable(pclNetworkSettingsP->nominalBitrateString()) );
      }
      else
      {
         fprintf(stdout, "Bit-rate      : %s (nominal), %s (data) \n",
                 qPrintable(pclNetworkSettingsP->nominalBitrateString()),
                 qPrintable(pclNetworkSettingsP->dataBitrateString())        );
      }
      fprintf(stdout, "--------------------------------------------------------------------------------\n");
   }


   clCanSocketP.connectNetwork(teChannelP);

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
            pclServerSettingsP->versionMajor(),
            pclServerSettingsP->versionMinor(),
            pclServerSettingsP->versionBuild() );

   pclNetworkSettingsP = new QCanNetworkSettings(teChannelP);
   pclNetworkSettingsP->connectToServer();

   connect(pclNetworkSettingsP, &QCanNetworkSettings::objectReceived, 
           this,               &QCanDump::onNetworkObjectReceived);


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
         fprintf(stdout, "CANpie FD server %s \n", qPrintable(pclServerSettingsP->stateString()));
         btQuitProgramT = true;
         break;

      case QCanServerSettings::eSTATE_UNKNOWN:
         fprintf(stdout, "CANpie FD server %s \n", qPrintable(pclServerSettingsP->stateString()));
         btQuitProgramT = true;
         break;

      case QCanServerSettings::eSTATE_INACTIVE:
         fprintf(stdout, "CANpie FD server %s \n", qPrintable(pclServerSettingsP->stateString()));
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
   qDebug() << "Connected to CAN " << teChannelP;
   qDebug() << "Quit time" << ulQuitTimeP;

   if (btIsWebConnectionP == false)
   {

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
void QCanDump::onSocketDisconnected()
{
   qDebug() << "Disconnected from CAN " << teChannelP;
   
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

   if ((btQuitNeverP == false) && (ulQuitTimeP > 0))
   {
      clActivityTimerP.start(ulQuitTimeP);
   }
   
   slFrameCountT = clCanSocketP.framesAvailable();
   while (slFrameCountT)
   {
      if (clCanSocketP.read(clCanFrameT) == true)
      {
         clCanStringT = clCanFrameT.toString(btTimeStampP);
         fprintf(stdout, "%s\n", qPrintable(clCanStringT));
      }
      slFrameCountT--;
      ulQuitCountP--;
      if(ulQuitCountP == 0)
      {
         quit();
      }
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
   QCommandLineOption clOptHostT(QStringList() << "H" << "host",
         tr("Connect to <host>"),
         tr("host"));
   clCmdParserP.addOption(clOptHostT);

   //---------------------------------------------------------------------------------------------------
   // command line option: --id-accept <Identifier list>
   //
   QCommandLineOption clOptIdAcceptT("id-accept",
         tr("Accept CAN frames with identifier <id>"),
         tr("id"));
   clCmdParserP.addOption(clOptIdAcceptT);

   //---------------------------------------------------------------------------------------------------
   // command line option: --id-reject <Identifier list>
   //
   QCommandLineOption clOptIdRejectT("id-reject",
         tr("Reject CAN frames with identifier <id>"),
         tr("id"));
   clCmdParserP.addOption(clOptIdRejectT);

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
   QCommandLineOption clOptTimeStampT(QStringList() << "t" << "timestamp",
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
   teChannelP = (CAN_Channel_e) (slChannelT);

   
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
      btIsWebConnectionP = true;
   }

   //---------------------------------------------------------------------------------------------------
   // check for filter (reject)
   //
   QCanFilterList clFilterListT;

   if (clCmdParserP.isSet(clOptIdRejectT))
   {
      QStringList clRejectIdsT = clCmdParserP.values(clOptIdRejectT);

      QCanFilter clFilterRejectT;
      for (int32_t slRejectNumT = 0; slRejectNumT < clRejectIdsT.size(); slRejectNumT++)
      {
         clFilterRejectT.rejectFrame(QCanFrame::eFORMAT_CAN_STD,
                                     clRejectIdsT.at(slRejectNumT).toInt(Q_NULLPTR, 16),
                                     clRejectIdsT.at(slRejectNumT).toInt(Q_NULLPTR, 16));

         clFilterListT.appendFilter(clFilterRejectT);
      }

   }


   //---------------------------------------------------------------------------------------------------
   // check for filter (accept)
   //

   if (clCmdParserP.isSet(clOptIdAcceptT))
   {
      QStringList clAcceptIdsT = clCmdParserP.values(clOptIdAcceptT);

      QCanFilter clFilterAcceptT;
      for (int32_t slAcceptNumT = 0; slAcceptNumT < clAcceptIdsT.size(); slAcceptNumT++)
      {
         clFilterAcceptT.acceptFrame(QCanFrame::eFORMAT_CAN_STD,
               clAcceptIdsT.at(slAcceptNumT).toInt(Q_NULLPTR, 16),
               clAcceptIdsT.at(slAcceptNumT).toInt(Q_NULLPTR, 16));

         clFilterListT.appendFilter(clFilterAcceptT);
      }

   }
   

   //---------------------------------------------------------------------------------------------------
   // add filter list
   //
   if (!clFilterListT.isEmpty())
   {
      // clCanSocketP.setFilterList(clFilterListT);
   }

   //---------------------------------------------------------------------------------------------------
   // connect to QCanServer class (i.e. CANpie FD Server)
   //
   pclServerSettingsP->connectToServer();

}
