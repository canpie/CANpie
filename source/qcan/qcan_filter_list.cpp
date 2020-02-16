//====================================================================================================================//
// File:          qcan_filter_list.cpp                                                                                //
// Description:   QCAN classes - CAN message filter list                                                              //
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

#include "qcan_filter_list.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/



//--------------------------------------------------------------------------------------------------------------------//
// QCanFilterList::QCanFilterList()                                                                                   //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanFilterList::QCanFilterList()
{
   clFilterP.clear();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFilterList()                                                                                                   //
// copy constructor                                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
QCanFilterList::QCanFilterList(const QCanFilterList &clOtherR)
{
   clFilterP = clOtherR.clFilterP;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFilterList::~QCanFilterList()                                                                                  //
// destructor                                                                                                         //
//--------------------------------------------------------------------------------------------------------------------//
QCanFilterList::~QCanFilterList()
{

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFilterList::appendFilter()                                                                                     //
// append filter to list                                                                                              //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanFilterList::appendFilter(const QCanFilter & clFilterR)
{
   clFilterP.append(clFilterR);
   return (clFilterP.size() - 1);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFilterList::clear()                                                                                            //
// clear filter list                                                                                                  //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFilterList::clear(void)
{
   clFilterP.clear();
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFilterList::filter()                                                                                           //
// run filter operation, return true when CAN frame is filtered                                                       //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanFilterList::filter(const QCanFrame & clFrameR) const
{
   bool     btFilterResultT = false;
   uint32_t ulAcceptFilterCountT = 0;
   uint32_t ulAcceptMatchCountT  = 0;
   uint32_t ulRejectMatchCountT  = 0;
   int32_t  slPosT = 0;

   //---------------------------------------------------------------------------------------------------
   // Run through all filters inside the list and check for a match condition. The filter types are
   // handled individually.
   //
   //
   for (slPosT = 0; slPosT < clFilterP.size(); slPosT++)
   {
      if (clFilterP.at(slPosT).type() == QCanFilter::eFILTER_ACCEPT)
      {
         ulAcceptFilterCountT++;
         if (clFilterP.at(slPosT).match(clFrameR) == true)
         {
            ulAcceptMatchCountT++;
         }
      }
      else
      {
         if (clFilterP.at(slPosT).match(clFrameR) == true)
         {
            ulRejectMatchCountT++;
         }
      }
   }

   //---------------------------------------------------------------------------------------------------
   // Check result of individual counters
   //
   if (ulRejectMatchCountT > 0)
   {
      //-------------------------------------------------------------------------------------------
      // got at least one rejection, so filter this message
      //
      btFilterResultT = true;
   }
   else
   {
      //-------------------------------------------------------------------------------------------
      // got no acceptance, so filter this message
      //
      if ((ulAcceptFilterCountT > 0) && (ulAcceptMatchCountT == 0))
      {
         btFilterResultT = true;
      }

   }

   return (btFilterResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFilterList::isEmpty()                                                                                          //
// check if filter list is empty                                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanFilterList::isEmpty(void)
{
   return (clFilterP.isEmpty());
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFilterList::removeFilter()                                                                                     //
// remove filter from list                                                                                            //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanFilterList::removeFilter(const int32_t slPositionR)
{
   bool  btResultT = false;

   if (slPositionR < clFilterP.size())
   {
      clFilterP.remove(slPositionR);
      btResultT = true;
   }

   return (btResultT);
}
   
