//============================================================================//
// File:          qcan_network.cpp                                            //
// Description:   QCAN classes - CAN network                                  //
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


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <QDebug>

#include "qcan_server.hpp"



/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanServer()                                                               //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanServer::QCanServer( QObject * pclParentV,
                        uint16_t  uwPortStartV,
                        uint8_t   ubNetworkNumV)
{
   QCanNetwork *  pclCanNetT;

   //----------------------------------------------------------------
   // set the parent
   //
   this->setParent(pclParentV);

   ulDispatchTimeP = 10;

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

}


//----------------------------------------------------------------------------//
// QCanServer()                                                               //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanServer::~QCanServer()
{

}

uint8_t QCanServer::maximumNetwork(void) const
{
   return(pclListNetsP->count());
}


//----------------------------------------------------------------------------//
// network()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
QCanNetwork * QCanServer::network(uint8_t ubNetworkIdxV)
{
   return(pclListNetsP->at(ubNetworkIdxV));
}


//----------------------------------------------------------------------------//
// setHostAddress()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServer::setServerAddress(QHostAddress clHostAddressV)
{
   QCanNetwork *  pclNetworkT;

   clServerAddressP = clHostAddressV;

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


//----------------------------------------------------------------------------//
// setDispatcherTime()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServer::setDispatcherTime(uint32_t ulTimeV)
{
   QCanNetwork *  pclNetworkT;

   ulDispatchTimeP = ulTimeV;

   for(uint8_t ubNetCntT = 0; ubNetCntT < maximumNetwork(); ubNetCntT++)
   {
      pclNetworkT = network(ubNetCntT);

      //-------------------------------------------------------------
      // assign new time value
      //
      pclNetworkT->setDispatcherTime(ulTimeV);

   }
}
