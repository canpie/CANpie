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
#include <QtCore/QTimer>



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
   // connect the signals
   //
   QObject::connect(&clMainT, SIGNAL(finished()),
                    &clAppT, SLOT(quit()));
   
   QObject::connect(&clAppT, SIGNAL(aboutToQuit()),
                    &clMainT, SLOT(aboutToQuitApp()));

   
   //---------------------------------------------------------------------------------------------------
   // This code will start the messaging engine in QT and in 10 ms  it will start the execution in the
   // clMainT.runCmdParser() routine.
   //
   QTimer::singleShot(10, &clMainT, SLOT(runCmdParser()));

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
   pclAppP = QCoreApplication::instance();

   //---------------------------------------------------------------------------------------------------
   // set default values
   //
   teCanChannelP    = eCAN_CHANNEL_NONE;
   slNomBitRateP    = eCAN_BITRATE_NONE;
   slDatBitRateP    = eCAN_BITRATE_NONE;
   teCanModeP       = eCAN_MODE_SELF_TEST;
   btConfigBitrateP = false;
   btResetCanIfP    = false;

   //---------------------------------------------------------------------------------------------------
   // create QServerSettings object
   //
   pclServerSettingsP = new QCanServerSettings();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::aboutToQuitApp()                                                                                       //
// shortly after quit is called the CoreApplication will signal this routine: delete objects / clean up               //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::aboutToQuitApp()
{
   delete (pclServerSettingsP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::quit()                                                                                                 //
// call this routine to quit the application                                                                          //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::quit()
{
   emit finished();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::runCmdParser()                                                                                         //
// 10ms after the application starts this method will parse all commands                                              //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::runCmdParser(void)
{
   //---------------------------------------------------------------------------------------------------
   // setup command line parser, options are added in alphabetical order
   //
   clCmdParserP.setApplicationDescription(tr("Configure CAN interface"));


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
   // command line option: -a, --all
   //
   QCommandLineOption clOptAllT(QStringList() << "a" << "all", 
         tr("Show all CAN interfaces"));
   clCmdParserP.addOption(clOptAllT);
   
   
   //---------------------------------------------------------------------------------------------------
   // command line option: --btr-data <value>
   //
   QCommandLineOption clOptDatBtrT("btr-data",
         tr("Set data bit-rate"),
         tr("value"));
   clCmdParserP.addOption(clOptDatBtrT);


   //---------------------------------------------------------------------------------------------------
   // command line option: --btr-nominal <value>
   //
   QCommandLineOption clOptNomBtrT("btr-nominal",
         tr("Set nominal bit-rate"),
         tr("value"));
   clCmdParserP.addOption(clOptNomBtrT);


   //---------------------------------------------------------------------------------------------------
   // command line option: -m <mode>
   //
   QCommandLineOption clOptModeT(QStringList() << "m" << "mode", 
         tr("Set mode of CAN interface"),
         tr("start|stop|listen-only"));
   clCmdParserP.addOption(clOptModeT);
   
   
   //---------------------------------------------------------------------------------------------------
   // command line option: -r, --reset
   //
   QCommandLineOption clOptResetT(QStringList() << "r" << "reset",
         tr("Reset CAN interface"));
   clCmdParserP.addOption(clOptResetT);

   
   //---------------------------------------------------------------------------------------------------
   // command line option: -v, --version
   //
   clCmdParserP.addVersionOption();


   //---------------------------------------------------------------------------------------------------
   // Process the actual command line arguments given by the user
   //
   clCmdParserP.process(*pclAppP);
   
   //---------------------------------------------------------------------------------------------------
   // Test for --all option
   //
   if(clCmdParserP.isSet(clOptAllT))
   {
      showServerSettings();
      for (uint8_t ubCanChannelT = eCAN_CHANNEL_1; ubCanChannelT <= pclServerSettingsP->networkCount(); ubCanChannelT++)
      {
      	showNetworkSettings((CAN_Channel_e) ubCanChannelT);
      }
      quit();
      return;
   }
   
   const QStringList clArgsT = clCmdParserP.positionalArguments();
   if (clArgsT.size() != 1) 
   {
      fprintf(stdout, "%s\n",
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
   teCanChannelP = (CAN_Channel_e) (slChannelT);


   //---------------------------------------------------------------------------------------------------
   // set bit-rate
   //
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
   
   //---------------------------------------------------------------------------------------------------
   // Check the "mode" option
   //
   if (clCmdParserP.value(clOptModeT).contains("start", Qt::CaseInsensitive))
   {
      teCanModeP = eCAN_MODE_OPERATION;
   }

   if (clCmdParserP.value(clOptModeT).contains("stop", Qt::CaseInsensitive))
   {
      teCanModeP = eCAN_MODE_INIT;
   }

   if (clCmdParserP.value(clOptModeT).contains("listen-only", Qt::CaseInsensitive))
   {
      teCanModeP = eCAN_MODE_LISTEN_ONLY;
   }

   //---------------------------------------------------------------------------------------------------
   // test for reset CAN interface option set
   //
   if (clCmdParserP.isSet(clOptResetT))
   {
      btResetCanIfP = true;
   }


   //---------------------------------------------------------------------------------------------------
   // Test for active CANpie server
   //
   if (pclServerSettingsP->state() < QCanServerSettings::eSTATE_ACTIVE)
   {
      fprintf(stdout, "CANpie FD server %s \n", qPrintable(pclServerSettingsP->stateString()));
      exit(0);
   }


   execCommand();
}

//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::execCommand()                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::execCommand(void)
{
   uint8_t  ubCmdExecutedT = 0;     // number of commands executed

   //---------------------------------------------------------------------------------------------------
   // update bit-rate settings if enabled
   //
   if (btConfigBitrateP)
   {
      qDebug() << "Set bit-rate: nominal" << slNomBitRateP << " ,data" << slDatBitRateP;
      if (pclServerSettingsP->setNetworkBitrate(teCanChannelP, slNomBitRateP, slDatBitRateP) == false)
      {
         fprintf(stdout, "Bit-rate configuration failed, check CANpie server settings.\n");
      }
      else
      {
         fprintf(stdout, "Bit-rate configuration done.\n");
      }
      ubCmdExecutedT++;
   }
   
   //---------------------------------------------------------------------------------------------------
   // reset CAN interface if enabled
   //
   if (btResetCanIfP)
   {
      if (pclServerSettingsP->resetNetwork(teCanChannelP) == false)
      {
         fprintf(stdout, "CAN interface reset failed, check CANpie server settings.\n");
      }
      else
      {
         fprintf(stdout, "CAN interface reset done.\n");
      }
      ubCmdExecutedT++;
   }

   //---------------------------------------------------------------------------------------------------
   // check if CAN interface state needs to be changed
   //
   if (teCanModeP != eCAN_MODE_SELF_TEST)
   {
      if (pclServerSettingsP->setNetworkMode(teCanChannelP, teCanModeP) == false)
      {
         fprintf(stdout, "Failed to changed CAN interface mode, check CANpie server settings.\n");
      }
      else
      {
         fprintf(stdout, "CAN interface mode changed.\n");
      }
      ubCmdExecutedT++;
   }


   //---------------------------------------------------------------------------------------------------
   // if no command has been executed up to this line just show the configuration of the network
   //
   if (ubCmdExecutedT == 0)
   {
      showNetworkSettings(teCanChannelP);
      showNetworkStatistics(teCanChannelP);
   }

   QTimer::singleShot(50, this, SLOT(quit()));
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::showNetworkSettings()                                                                                  //
// show settings of selected CAN network                                                                              //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::showNetworkSettings(CAN_Channel_e teCanChannelV)
{
   if (teCanChannelV <= pclServerSettingsP->networkCount())
   {
      fprintf(stdout, "\n");

      //-------------------------------------------------------------------------------------------
      // network name
      //
      fprintf(stdout, "CAN %d         : %s \n", teCanChannelV,
              qPrintable(pclServerSettingsP->networkName(teCanChannelV)) );

      //-------------------------------------------------------------------------------------------
      // network configuration
      //
      fprintf(stdout, "Configuration : %s \n",
              qPrintable(pclServerSettingsP->networkConfigurationString(teCanChannelV)) );

      //-------------------------------------------------------------------------------------------
      // network state
      //
      fprintf(stdout, "CAN state     : %s \n",
              qPrintable(pclServerSettingsP->networkStateString(teCanChannelV)) );

      //-------------------------------------------------------------------------------------------
      // bit-rate settings
      //
      if (pclServerSettingsP->networkDataBitrate() == eCAN_BITRATE_NONE)
      {
         fprintf(stdout, "Bit-rate      : %s \n",
                 qPrintable(pclServerSettingsP->networkNominalBitrateString(teCanChannelV)) );
      }
      else
      {
         fprintf(stdout, "Bit-rate      : %s (nominal), %s (data) \n",
                 qPrintable(pclServerSettingsP->networkNominalBitrateString(teCanChannelV)),
                 qPrintable(pclServerSettingsP->networkDataBitrateString(teCanChannelV))        );
      }

   }
   else
   {
      fprintf(stdout, "CAN interface not available\n");
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::showNetworkStatistics()                                                                                //
// show statistics of CAN interface                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::showNetworkStatistics(CAN_Channel_e teCanChannelV)
{
   if (teCanChannelV <= pclServerSettingsP->networkCount())
   {
      //-------------------------------------------------------------------------------------------
      // network frame count
      //
      fprintf(stdout, "CAN frames    : %d \n",
              pclServerSettingsP->networkFrameCount(teCanChannelV) );

      //-------------------------------------------------------------------------------------------
      // network error count
      //
      fprintf(stdout, "Error frames  : %d \n",
              pclServerSettingsP->networkErrorCount(teCanChannelV) );

      //-------------------------------------------------------------------------------------------
      // network frame count
      //
      fprintf(stdout, "Bus load      : %d \n",
              pclServerSettingsP->networkBusLoad(teCanChannelV) );

   }

   fprintf(stdout, "\n");
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanConfig::showServerSettings()                                                                                   //
// show settings of CAN server                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanConfig::showServerSettings(void)
{
   if (pclServerSettingsP->state() == QCanServerSettings::eSTATE_ACTIVE)
   {
      fprintf(stdout, "%s %d.%d.%d \n",
              qPrintable(tr("CANpie FD server active, version:")),
              pclServerSettingsP->versionMajor(),
              pclServerSettingsP->versionMinor(),
              pclServerSettingsP->versionBuild() );

      fprintf(stdout, "%s %d \n",
              qPrintable(tr("Supported CAN networks:")),
              pclServerSettingsP->networkCount() );
   }
   else
   {
      fprintf(stdout, "CANpie FD server %s \n", qPrintable(pclServerSettingsP->stateString()));
      exit(0);
   }

}
