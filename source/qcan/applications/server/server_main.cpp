//============================================================================//
// File:          server_main.cpp                                             //
// Description:   CANpie server                                               //
//                                                                            //
// Copyright 2017 MicroControl GmbH & Co. KG                                  //
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
// Licensed under the Apache License, Version 2.0 (the "License");            //
// you may not use this file except in compliance with the License.           //
// You may obtain a copy of the License at                                    //
//                                                                            //
//    http://www.apache.org/licenses/LICENSE-2.0                              //
//                                                                            //
// Unless required by applicable law or agreed to in writing, software        //
// distributed under the License is distributed on an "AS IS" BASIS,          //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   //
// See the License for the specific language governing permissions and        //
// limitations under the License.                                             //
//============================================================================//


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <QtCore/QCommandlineParser>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

#include "qcan_server_dialog.hpp"
#include "qcan_server_settings.hpp"


#ifdef Q_OS_MACOS
#include "MacAppNapSuspender.h"
#endif


int main(int argc, char *argv[])
{

   Q_INIT_RESOURCE(server);

   //----------------------------------------------------------------
   // create application
   //
   QApplication clAppT(argc, argv);

   //----------------------------------------------------------------
   // run command parser
   //
   QCommandLineParser clCmdParserT;

   //----------------------------------------------------------------
   // Add "clean" option with multiple names (-c, --clean)
   //
   QCommandLineOption clCmdOptionCleanT(QStringList() << "c" << "clean",
                                        QCoreApplication::translate("CANpie Server", "Start in clean mode."));
   clCmdParserT.addOption(clCmdOptionCleanT);

   // Process the actual command line arguments given by the user
   clCmdParserT.process(clAppT);

   if (clCmdParserT.isSet(clCmdOptionCleanT) == false)
   {
      QCanServerSettings   clServerSettingsT;
      if (clServerSettingsT.isServerActive() == true)
      {
         fprintf(stdout, "%s \n", qPrintable("CANpie Server is already running"));

         QMessageBox::warning(0L, "CANpie Server", "CANpie Server is already running.");

         exit(0);
      }
   }
   
   //----------------------------------------------------------------
   // load translation
   //
   QTranslator clQtTranslatorT;

   if (clQtTranslatorT.load(QLocale(), 
       QLatin1String("server"), 
       QLatin1String("_"), QLatin1String(":/translations")))
   {
      clAppT.installTranslator(&clQtTranslatorT);
   }
   
   QApplication::setQuitOnLastWindowClosed(false);

   //----------------------------------------------------------------
   // stop the App Nap feature on Mac OS
   //
   #ifdef Q_OS_MACOS
   AppNapSuspender clAppNapT;
   clAppNapT.suspend();
   #endif

   //----------------------------------------------------------------
   // start the dialog, since it is a tray widget hide it initially
   //
   QCanServerDialog  clCanServerDlgT;
   clCanServerDlgT.hide();
   return clAppT.exec();
}

