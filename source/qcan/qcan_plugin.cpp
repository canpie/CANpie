//====================================================================================================================//
// File:          qcan_plugin.cpp                                                                                     //
// Description:   QCAN classes - CAN interface plugin                                                                 //
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

#include <QCanPlugin>


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*\
** Class methods                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// QCanPlugin()                                                                                                       //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanPlugin::QCanPlugin()
{
   //---------------------------------------------------------------------------------------------------
   // set default values
   //
   clLogMessageP.clear();
   pclLogFileP.reset();
   teLogLevelP = QCan::eLOG_LEVEL_INFO;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanPlugin()::appendMessage()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanPlugin::appendMessage(const QString & clLogMessageV, const QCan::LogLevel_e teLogLevelV)
{
   clLogMessageP.clear();

   if (teLogLevelV <= teLogLevelP)
   {
      clTimeP        = QDateTime::currentDateTime();
      clLogMessageP  = clTimeP.toString("hh:mm:ss.zzz - ");
      clLogMessageP += clLogMessageV;

      if (pclLogFileP.get() != nullptr)
      {
         pclLogFileP->write(clLogMessageP.toLatin1() + "\n");
         pclLogFileP->flush();
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanPlugin()::setFileName()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanPlugin::setFileName(const QString clFileNameV)
{
   bool     btResultT = false;


   if (pclLogFileP.get() == nullptr)
   {
      pclLogFileP = std::unique_ptr<QFile>(new QFile());
   }
   else
   {
      //-------------------------------------------------------------------------------------------
      // make sure the file is closed in case it was used before
      //
      pclLogFileP->close();
   }

   if (pclLogFileP.get() != nullptr)
   {
      //-------------------------------------------------------------------------------------------
      // open the new file in rw mode for text files, existing contents is truncated
      //
      pclLogFileP->setFileName(clFileNameV);
      if (pclLogFileP->open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) == true)
      {
         btResultT = true;
      }
      else
      {
         pclLogFileP->close();
         pclLogFileP->reset();
      }
   }

   return (btResultT);
}
