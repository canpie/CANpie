//====================================================================================================================//
// File:          qcan_server_logger.hpp                                                                              //
// Description:   QCAN server - logging widget                                                                        //
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



#include "qcan_server_logger.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QDebug>


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#ifdef   Q_OS_LINUX
#define  QCAN_LOG_PATH            "/var/log/canpie/"
#endif

#ifdef   Q_OS_OSX
#define  QCAN_LOG_PATH            "/var/log/canpie/"
#endif

#ifdef   Q_OS_WIN32
#define  QCAN_LOG_PATH            "./log/canpie/"
#endif




//--------------------------------------------------------------------------------------------------------------------//
// QCanServerLogger::QCanServerLogger()                                                                               //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanServerLogger::QCanServerLogger(QCanServer * pclServerV)
{
   for (uint8_t ubLogNumT = 0; ubLogNumT < QCAN_NETWORK_MAX; ubLogNumT++)
   {
      ateLogLevelP[ubLogNumT] = eLOG_LEVEL_INFO;
      apclLogFileP[ubLogNumT] = Q_NULLPTR;
   }

   if (pclServerV != Q_NULLPTR)
   {
      this->attachServer(pclServerV);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerLogger::~QCanServerLogger()                                                                              //
// destructor                                                                                                         //
//--------------------------------------------------------------------------------------------------------------------//
QCanServerLogger::~QCanServerLogger()
{
   for (uint8_t ubLogNumT = 0; ubLogNumT < QCAN_NETWORK_MAX; ubLogNumT++)
   {
      if (apclLogFileP[ubLogNumT] != Q_NULLPTR)
      {
         qDebug() << "close log file " << ubLogNumT;
         apclLogFileP[ubLogNumT]->flush();
         apclLogFileP[ubLogNumT]->close();
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerLogger::attachServer()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void  QCanServerLogger::attachServer(QCanServer * pclServerV)
{
   QString  clFileT;

   for (uint8_t ubChannelNumT = eCAN_CHANNEL_1; ubChannelNumT <= pclServerV->maximumNetwork(); ubChannelNumT++)
   {
      clFileT = QString(QCAN_LOG_PATH);
      clFileT = clFileT + QString("can%1.log").arg(ubChannelNumT);
      this->setFileName((CAN_Channel_e) ubChannelNumT, clFileT);
      this->addLoggingSource(pclServerV->network(ubChannelNumT - 1));
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerLogger::addLoggingSource()                                                                               //
// connect signal from QCanServer class to the slot appendMessage()                                                   //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerLogger::addLoggingSource(QObject * pclSenderV)
{

   connect( pclSenderV, SIGNAL(addLogMessage(const CAN_Channel_e, const QString &, LogLevel_e)),
            this,         SLOT(appendMessage(const CAN_Channel_e, const QString &, LogLevel_e)),
            Qt::DirectConnection);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerLogger::appendMessage()                                                                                  //
// append log message to log file                                                                                     //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerLogger::appendMessage(const CAN_Channel_e ubChannelV, const QString & clLogMessageV,
                                     LogLevel_e teLogLevelV)
{
   if ((ubChannelV >= eCAN_CHANNEL_1) && (ubChannelV <= QCAN_NETWORK_MAX))
   {
      clLogMessageP.clear();

      if (teLogLevelV <= ateLogLevelP[ubChannelV - 1])
      {
         clTimeP = QDateTime::currentDateTime();
         clLogMessageP  = clTimeP.toString("hh:mm:ss.zzz - ");
         clLogMessageP += clLogMessageV;

         if ((apclLogFileP[ubChannelV - 1]) != Q_NULLPTR)
         {
            apclLogFileP[ubChannelV - 1]->write(clLogMessageP.toLatin1() + "\n");
            apclLogFileP[ubChannelV - 1]->flush();
         }
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerLogger::logLevel()                                                                                       //
// return log level of selected CAN channel                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
LogLevel_e QCanServerLogger::logLevel(const CAN_Channel_e teChannelV)
{
   LogLevel_e teLevelT = eLOG_LEVEL_INFO;

   if ((teChannelV >= eCAN_CHANNEL_1) && (teChannelV <= QCAN_NETWORK_MAX))
   {
      teLevelT = ateLogLevelP[teChannelV - 1];
   }

   return teLevelT;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerLogger::setFileName()                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanServerLogger::setFileName(const CAN_Channel_e teChannelV, QString clFileNameV)
{
   QFile *  pclLogFileT;
   bool     btResultT = false;

   if ((teChannelV >= eCAN_CHANNEL_1) && (teChannelV <= QCAN_NETWORK_MAX))
   {
      if ((apclLogFileP[teChannelV - 1]) == Q_NULLPTR)
      {
         apclLogFileP[teChannelV - 1] = new QFile();
      }
      else
      {
         //-----------------------------------------------------
         // make sure the file is closed in case it was used
         // before
         //
         apclLogFileP[teChannelV - 1]->close();
      }


      pclLogFileT = apclLogFileP[teChannelV - 1];


      //-------------------------------------------------------------------------------------------
      // open the new file in rw mode for text files, existing contents is truncated
      //
      pclLogFileT->setFileName(clFileNameV);
      if (pclLogFileT->open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) == true)
      {
         btResultT = true;
      }
      else
      {
         delete (pclLogFileT);
         apclLogFileP[teChannelV - 1] = Q_NULLPTR;
         btResultT = false;
      }

   }

   return btResultT;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerLogger::setLogLevel()                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServerLogger::setLogLevel(const CAN_Channel_e ubChannelV, LogLevel_e teLogLevelV)
{

   if ((ubChannelV >= eCAN_CHANNEL_1) && (ubChannelV <= QCAN_NETWORK_MAX))
   {
      ateLogLevelP[ubChannelV - 1] = teLogLevelV;

      QString  clLogTextP;

      switch (teLogLevelV)
      {
         case eLOG_LEVEL_OFF:
            clLogTextP = tr("Set log level: Off");
            break;

         case eLOG_LEVEL_FATAL:
            clLogTextP = tr("Set log level: Fatal");
            break;

         case eLOG_LEVEL_ERROR:
            clLogTextP = tr("Set log level: Error");
            break;

         case eLOG_LEVEL_WARN:
            clLogTextP = tr("Set log level: Warning");
            break;

         case eLOG_LEVEL_INFO:
            clLogTextP = tr("Set log level: Info");
            break;

         case eLOG_LEVEL_DEBUG:
            clLogTextP = tr("Set log level: Debug");
            break;

         case eLOG_LEVEL_TRACE:
            clLogTextP = tr("Set log level: Trace");
            break;

         case eLOG_LEVEL_ALL:
            clLogTextP = tr("Set log level: All");
            break;

      }

      appendMessage(ubChannelV, clLogTextP, teLogLevelV);
   }
}

