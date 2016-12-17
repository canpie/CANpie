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

#include <QTimer>
#include <QDebug>

#include <qcan_dump.hpp>


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
   //
   clCmdParserP.setApplicationDescription("Test helper");
   clCmdParserP.addHelpOption();
   clCmdParserP.addVersionOption();
   clCmdParserP.addPositionalArgument("source", QCoreApplication::translate("main", "Source file to copy."));
   clCmdParserP.addPositionalArgument("destination", QCoreApplication::translate("main", "Destination directory."));


   // A boolean option with a single name (-p)
   QCommandLineOption showProgressOption("p", QCoreApplication::translate("main", "Show progress during copy"));
   clCmdParserP.addOption(showProgressOption);

   // A boolean option with multiple names (-f, --force)
   QCommandLineOption forceOption(QStringList() << "f" << "force",
           QCoreApplication::translate("main", "Overwrite existing files."));
   clCmdParserP.addOption(forceOption);

   // An option with a value
   QCommandLineOption targetDirectoryOption(QStringList() << "t" << "target-directory",
           QCoreApplication::translate("main", "Copy all source files into <directory>."),
           QCoreApplication::translate("main", "directory"));
   clCmdParserP.addOption(targetDirectoryOption);

   // Process the actual command line arguments given by the user
   clCmdParserP.process(*pclAppP);
    // setup everything here
    // create any global objects
    // setup debug and warning mode

    //clOptFrameCountP
}

// 10ms after the application starts this method will run
// all QT messaging is running at this point so threads, signals and slots
// will all work as expected.
void QCanDump::run()
{
    // Add your main code here
    qDebug() << "MainClass.Run is executing";
    // you must call quit when complete or the program will stay in the
    // messaging loop
    quit();
}

// call this routine to quit the application
void QCanDump::quit()
{
   qDebug() << "I will quit soon";

    // you can do some cleanup here
    // then do emit finished to signal CoreApplication to quit
    emit finished();
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

int main(int argc, char *argv[])
{
   QCoreApplication app(argc, argv);
   QCoreApplication::setApplicationName("my-copy-program");
   QCoreApplication::setApplicationVersion("1.0");




   // create the main class
     QCanDump myMain;

     // connect up the signals
     QObject::connect(&myMain, SIGNAL(finished()),
              &app, SLOT(quit()));
     QObject::connect(&app, SIGNAL(aboutToQuit()),
              &myMain, SLOT(aboutToQuitApp()));

     // This code will start the messaging engine in QT and in
     // 10ms it will start the execution in the MainClass.run routine;
     QTimer::singleShot(10, &myMain, SLOT(run()));

   app.exec();
}

