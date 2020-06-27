//====================================================================================================================//
// File:          qcan_config.cpp                                                                                     //
// Description:   Configure CAN interface                                                                             //
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

#include "qcan_config.hpp"

#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QTimer>


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#define  COMMAND_FLAG_NONE          ((uint32_t) 0x00000000)
#define  COMMAND_FLAG_SET_BITRATE   ((uint32_t) 0x00000001)
#define  COMMAND_FLAG_SET_MODE      ((uint32_t) 0x00000002)
#define  COMMAND_FLAG_RESET         ((uint32_t) 0x00000004)
#define  COMMAND_FLAG_MASK          ((uint32_t) 0x000000FF)

#define  COMMAND_FLAG_SHOW_ALL      ((uint32_t) 0x00000100)
#define  COMMAND_FLAG_SHOW_VERBOSE  ((uint32_t) 0x00000200)

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
   QCoreApplication::setApplicationName("can-config");
   
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
   QCanConfig clMainT;

   
   //---------------------------------------------------------------------------------------------------
   // connect the signals between QCoreApplication and the main class 
   //
   QObject::connect(&clMainT, &QCanConfig::finished, &clAppT,  &QCoreApplication::quit);
   
   QObject::connect(&clAppT,  &QCoreApplication::aboutToQuit, &clMainT, &QCanConfig::aboutToQuitApp);

   
   //---------------------------------------------------------------------------------------------------
   // This code will start the messaging engine in QT and in 10 ms it will start the execution of the
   // clMainT.runCommandParser() routine.
   //
   QTimer::singleShot(10, &clMainT, SLOT(runCommandParser()));

   clAppT.exec();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig()                                                                                                       //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanConfig::QCanConfig(QObject *parent) :
    QObject(parent)
{
   //---------------------------------------------------------------------------------------------------
   // get the instance of the main application
   //
   pclApplicationP = QCoreApplication::instance();

   //---------------------------------------------------------------------------------------------------
   // set default values
   //
   teCanChannelP      = eCAN_CHANNEL_NONE;
   slNomBitRateP      = eCAN_BITRATE_NONE;
   slDatBitRateP      = eCAN_BITRATE_NONE;
   teCanModeP         = eCAN_MODE_SELF_TEST;

   ulCommandFlagsP    = COMMAND_FLAG_NONE;

   //---------------------------------------------------------------------------------------------------
   // create QServerSettings object
   //
   connect(&clServerSettingsP, &QCanServerSettings::stateChanged, 
           this,               &QCanConfig::onServerStateChanged);

   connect(&clServerSettingsP, &QCanServerSettings::objectReceived, 
           this,               &QCanConfig::onServerObjectReceived);

   connect(&clNetworkSettingsP, &QCanNetworkSettings::objectReceived, 
           this,                &QCanConfig::onNetworkObjectReceived);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::aboutToQuitApp()                                                                                       //
// shortly after quit is called the CoreApplication will signal this routine: delete objects / clean up               //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::aboutToQuitApp()
{

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::execCommand()                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::execCommand(void)
{
   showServerSettings();

   //---------------------------------------------------------------------------------------------------
   // update bit-rate settings if enabled
   //
   if ((ulCommandFlagsP & COMMAND_FLAG_SET_BITRATE) > 0)
   {
      clNetworkSettingsP.setBitrate(slNomBitRateP, slDatBitRateP);
   }

   //---------------------------------------------------------------------------------------------------
   // update mode settings if enabled
   //
   if ((ulCommandFlagsP & COMMAND_FLAG_SET_MODE) > 0)
   {
      clNetworkSettingsP.setMode(teCanModeP);
   }

   //---------------------------------------------------------------------------------------------------
   // update bit-rate settings if enabled
   //
   if ((ulCommandFlagsP & COMMAND_FLAG_RESET) > 0)
   {
      clNetworkSettingsP.reset();
   }

   clNetworkSettingsP.send();


   QTimer::singleShot(50, this, SLOT(quit()));
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::onServerObjectReceived()                                                                               //
// handle JSON object from CANpie FD Server                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::onNetworkObjectReceived(const CAN_Channel_e teChannelV, QJsonObject clNetworkConfigV)
{

   if (teChannelV == teCanChannelP)
   {
      //------------------------------------------------------------------------------------------------
      // show all networks ?
      //
      if ((ulCommandFlagsP & COMMAND_FLAG_SHOW_ALL) > 0)
      {
         fprintf(stdout, "--------------------------------------------------------------------------------\n");

         //----------------------------------------------------------------------------------------
         // in verbose mode we also show the JSON document
         //
         if ((ulCommandFlagsP & COMMAND_FLAG_SHOW_VERBOSE) > 0)
         {
            fprintf(stdout, "%s %d %s \n", qPrintable("Network"), teChannelV, qPrintable("JSON object:"));
            QJsonDocument clJsonDocumentT(clNetworkConfigV);
            fprintf(stdout, "%s\n", qPrintable(clJsonDocumentT.toJson()));
         }

         showNetworkSettings();
         clNetworkSettingsP.closeConnection();
         if (teCanChannelP < clServerSettingsP.networkCount())
         {
            teCanChannelP = static_cast<CAN_Channel_e>(static_cast<uint8_t>(teCanChannelP) + 1);
            clNetworkSettingsP.setChannel(teCanChannelP);
            clNetworkSettingsP.connectToServer(clHostAddressP);
         }
         else
         {
            quit();
         }
      }

      //------------------------------------------------------------------------------------------------
      // show single network?
      //
      else if ((ulCommandFlagsP & COMMAND_FLAG_MASK) > 0)
      {
         execCommand();
      }
      else
      {
         fprintf(stdout, "\n");

         showNetworkSettings();
         showNetworkStatistics();
         quit();
      }

   }
   

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::onServerObjectReceived()                                                                               //
// handle JSON object from CANpie FD Server                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::onServerObjectReceived(QJsonObject clServerConfigV)
{
   if ((ulCommandFlagsP & COMMAND_FLAG_SHOW_VERBOSE) > 0)
   {
      fprintf(stdout, "%s\n", qPrintable("Server JSON object:"));
      QJsonDocument clJsonDocumentT(clServerConfigV);
      fprintf(stdout, "%s\n", qPrintable(clJsonDocumentT.toJson()));
   }

   if (teCanChannelP <= clServerSettingsP.networkCount())
   {
      clNetworkSettingsP.setChannel(teCanChannelP);
      clNetworkSettingsP.connectToServer(clHostAddressP);
   }
   else
   {
      fprintf(stdout, "%s%d %s \n", qPrintable("CAN interface can"), teCanChannelP, qPrintable("not available"));
      quit();
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::onStateChange()                                                                                        //
// check for new state of the CANpie FD Server                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::onServerStateChanged(enum QCanServerSettings::State_e teStateV)
{  
   bool btQuitProgramT = false;

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
// QCanConfig::quit()                                                                                                 //
// call this routine to quit the application                                                                          //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::quit()
{
   //---------------------------------------------------------------------------------------------------
   // disconnect all signals first before we close the WebSocket connections
   //
   clServerSettingsP.disconnect();
   clNetworkSettingsP.disconnect();

   //---------------------------------------------------------------------------------------------------
   // close the WebSocket connections
   //
   clNetworkSettingsP.closeConnection();
   clServerSettingsP.closeConnection();

   emit finished();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::runCmdParser()                                                                                         //
// 10ms after the application starts this method will parse all commands                                              //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::runCommandParser(void)
{
   //---------------------------------------------------------------------------------------------------
   // setup command line parser, options are added in alphabetical order
   //
   clCommandParserP.setApplicationDescription(tr("Configure CAN interface"));


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
   // command line option: -a, --all
   //
   QCommandLineOption clOptAllT(QStringList() << "a" << "all", 
         tr("Show all CAN interfaces"));
   clCommandParserP.addOption(clOptAllT);
   
   
   //---------------------------------------------------------------------------------------------------
   // command line option: --btr-data <value>
   //
   QCommandLineOption clOptDatBtrT("btr-data",
         tr("Set data bit-rate"),
         tr("value"));
   clCommandParserP.addOption(clOptDatBtrT);


   //---------------------------------------------------------------------------------------------------
   // command line option: --btr-nominal <value>
   //
   QCommandLineOption clOptNomBtrT("btr-nominal",
         tr("Set nominal bit-rate"),
         tr("value"));
   clCommandParserP.addOption(clOptNomBtrT);


   //---------------------------------------------------------------------------------------------------
   // command line option: -m <mode>
   //
   QCommandLineOption clOptModeT(QStringList() << "m" << "mode", 
         tr("Set mode of CAN interface"),
         tr("start|stop|listen-only"));
   clCommandParserP.addOption(clOptModeT);
   
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -r, --reset
   //
   QCommandLineOption clOptResetT(QStringList() << "r" << "reset",
         tr("Reset CAN interface"));
   clCommandParserP.addOption(clOptResetT);

   //---------------------------------------------------------------------------------------------------
   // command line option: --verbose
   //
   QCommandLineOption clOptVerboseT("verbose",
         tr("Show more details"));
   clCommandParserP.addOption(clOptVerboseT);

   
   //---------------------------------------------------------------------------------------------------
   // command line option: -v, --version
   //
   clCommandParserP.addVersionOption();


   //---------------------------------------------------------------------------------------------------
   // Process the actual command line arguments given by the user
   //
   clCommandParserP.process(*pclApplicationP);
   
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

   //----------------------------------------------ping wol-----------------------------------------------------
   // test for verbose option set
   //
   if (clCommandParserP.isSet(clOptVerboseT))
   {
      ulCommandFlagsP |= COMMAND_FLAG_SHOW_VERBOSE;
   }

   //---------------------------------------------------------------------------------------------------
   // Test for --all option
   //
   if (clCommandParserP.isSet(clOptAllT))
   {
      //-------------------------------------------------------------------------------------------
      // start with first channel
      //
      teCanChannelP = eCAN_CHANNEL_1;

      ulCommandFlagsP |= COMMAND_FLAG_SHOW_ALL;
      clServerSettingsP.connectToServer(clHostAddressP);
      return;
   }
   
   const QStringList clArgsT = clCommandParserP.positionalArguments();
   if (clArgsT.size() != 1) 
   {
      fprintf(stdout, "%s\n",
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
   // set bit-rate
   //
   slNomBitRateP = eCAN_BITRATE_NONE;
   slDatBitRateP = eCAN_BITRATE_NONE;
   
   if (clCommandParserP.isSet(clOptNomBtrT))
   {
      slNomBitRateP  = clCommandParserP.value(clOptNomBtrT).toInt(Q_NULLPTR, 10);    
      ulCommandFlagsP |= COMMAND_FLAG_SET_BITRATE;
   }
   
   if (clCommandParserP.isSet(clOptDatBtrT))
   {
      if (slNomBitRateP == eCAN_BITRATE_NONE)
      {
         fprintf(stderr, "%s \n\n", 
                 qPrintable(tr("Error: Must set nominal bit-rate also")));
         clCommandParserP.showHelp(0);         
      }
      else
      {
         slDatBitRateP  = clCommandParserP.value(clOptDatBtrT).toInt(Q_NULLPTR, 10);
      }
   }
   
   //---------------------------------------------------------------------------------------------------
   // Check the "mode" option
   //
   if (clCommandParserP.value(clOptModeT).contains("start", Qt::CaseInsensitive))
   {
      teCanModeP = eCAN_MODE_OPERATION;
      ulCommandFlagsP |= COMMAND_FLAG_SET_MODE;
   }

   if (clCommandParserP.value(clOptModeT).contains("stop", Qt::CaseInsensitive))
   {
      teCanModeP = eCAN_MODE_INIT;
      ulCommandFlagsP |= COMMAND_FLAG_SET_MODE;
   }

   if (clCommandParserP.value(clOptModeT).contains("listen-only", Qt::CaseInsensitive))
   {
      teCanModeP = eCAN_MODE_LISTEN_ONLY;
      ulCommandFlagsP |= COMMAND_FLAG_SET_MODE;
   }

   //---------------------------------------------------------------------------------------------------
   // test for reset CAN interface option set
   //
   if (clCommandParserP.isSet(clOptResetT))
   {
      ulCommandFlagsP |= COMMAND_FLAG_RESET;
   }


   clServerSettingsP.connectToServer(clHostAddressP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::showNetworkSettings()                                                                                  //
// show settings of selected CAN network                                                                              //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::showNetworkSettings(void)
{

   //-------------------------------------------------------------------------------------------
   // network name
   //
   fprintf(stdout, "CAN %d         : %s \n", teCanChannelP, qPrintable(clNetworkSettingsP.name())     );


   //-------------------------------------------------------------------------------------------
   // network state
   //
   fprintf(stdout, "CAN state     : %s \n", qPrintable(clNetworkSettingsP.stateString())              );

   //-------------------------------------------------------------------------------------------
   // bit-rate settings
   //
   if (clNetworkSettingsP.dataBitrate() == eCAN_BITRATE_NONE)
   {
      fprintf(stdout, "Bit-rate      : %s \n", qPrintable(clNetworkSettingsP.nominalBitrateString())  );
   }
   else
   {
      fprintf(stdout, "Bit-rate      : %s (nominal), %s (data) \n",
              qPrintable(clNetworkSettingsP.nominalBitrateString()),
              qPrintable(clNetworkSettingsP.dataBitrateString())                                      );
   }
}

//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::showNetworkStatistics()                                                                                //
// show statistics of CAN interface                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::showNetworkStatistics(void)
{
   //---------------------------------------------------------------------------------------------------
   // network frame count
   //
   fprintf(stdout, "CAN frames    : %d \n"   , clNetworkSettingsP.frameCount()      );

   //---------------------------------------------------------------------------------------------------
   // network error count
   //
   fprintf(stdout, "Error frames  : %d \n"   , clNetworkSettingsP.errorCount()      );


   fprintf(stdout, "\n");
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::showServerSettings()                                                                                   //
// show settings of CAN server                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::showServerSettings(void)
{
   if (clServerSettingsP.state() == QCanServerSettings::eSTATE_ACTIVE)
   {
      fprintf(stdout, "%s %d.%d.%d \n",
              qPrintable(tr("CANpie FD server active, version:")),
              clServerSettingsP.versionMajor(),
              clServerSettingsP.versionMinor(),
              clServerSettingsP.versionBuild() );

      fprintf(stdout, "%s %d \n",
              qPrintable(tr("Supported CAN networks:")),
              clServerSettingsP.networkCount() );
   }
   else
   {
      fprintf(stdout, "CANpie FD server %s \n", qPrintable(clServerSettingsP.stateString()));
      exit(0);
   }

}
