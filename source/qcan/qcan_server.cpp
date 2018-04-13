//====================================================================================================================//
// File:          qcan_server.cpp                                                                                     //
// Description:   QCAN classes - CAN server                                                                           //
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

#include "qcan_server.hpp"
#include "qcan_server_memory.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/




/*--------------------------------------------------------------------------------------------------------------------*\
** Class methods QCanServer                                                                                           **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanServer::QCanServer( QObject * pclParentV, uint16_t  uwPortStartV, uint8_t   ubNetworkNumV)
{
   QCanNetwork *  pclCanNetT;

   //----------------------------------------------------------------
   // set the parent
   //
   this->setParent(pclParentV);

   //----------------------------------------------------------------
   // create CAN networks
   //
   pclListNetsP = new QVector<QCanNetwork *>;
   pclListNetsP->reserve(ubNetworkNumV);

   for(uint8_t ubNetCntT = 0; ubNetCntT < ubNetworkNumV; ubNetCntT++)
   {
      pclCanNetT = new QCanNetwork(pclParentV, uwPortStartV + ubNetCntT);
      pclListNetsP->append(pclCanNetT);

   }

   initSettings();
}


//--------------------------------------------------------------------------------------------------------------------//
// ~QCanServer()                                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanServer::~QCanServer()
{
   releaseSettings();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::enableBitrateChange()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::enableBitrateChange(bool btEnabledV)
{
   QCanNetwork *  pclNetworkT;

   for(uint8_t ubNetCntT = 0; ubNetCntT < maximumNetwork(); ubNetCntT++)
   {
      pclNetworkT = network(ubNetCntT);

      //-------------------------------------------------------------
      // assign new time value
      //
      pclNetworkT->setBitrateFrameEnabled(btEnabledV);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::initSettings()                                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::initSettings(void)
{
   pclSettingsP = new QSharedMemory(QString(QCAN_MEMORY_KEY));
   btMemoryAttachedP = pclSettingsP->create(sizeof(ServerSettings_ts), QSharedMemory::ReadWrite);

   if (btMemoryAttachedP)
   {
      pclSettingsP->lock();
      memset(pclSettingsP->data(), 0, sizeof(ServerSettings_ts));
      pclSettingsP->unlock();
   }
   else
   {
      btMemoryAttachedP = pclSettingsP->attach();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::maximumNetwork()                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
uint8_t QCanServer::maximumNetwork(void) const
{
   return(pclListNetsP->count());
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::network()                                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanNetwork * QCanServer::network(uint8_t ubNetworkIdxV)
{
   return(pclListNetsP->at(ubNetworkIdxV));
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServer::releaseSettings()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanServer::releaseSettings(void)
{
   pclSettingsP->detach();
}

//----------------------------------------------------------------------------//
// setHostAddress()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServer::setServerAddress(QHostAddress clHostAddressV)
{
   QCanNetwork *  pclNetworkT;

   if (!clHostAddressV.isEqual(clServerAddressP))
   {
      clServerAddressP = clHostAddressV;

      qDebug() << "QCanServer::setServerAddress()" << clHostAddressV;

      for(uint8_t ubNetCntT = 0; ubNetCntT < maximumNetwork(); ubNetCntT++)
      {
         pclNetworkT = network(ubNetCntT);

         //-------------------------------------------------------------
         // check current state of network
         //
         if(pclNetworkT->isNetworkEnabled() == true)
         {
            //-----------------------------------------------------
            // assign new host address
            //
            pclNetworkT->setNetworkEnabled(false);
            pclNetworkT->setServerAddress(clHostAddressV);
            pclNetworkT->setNetworkEnabled(true);
         }
         else
         {
            //-----------------------------------------------------
            // assign new host address
            //
            pclNetworkT->setServerAddress(clHostAddressV);
         }
      }
   }
}


