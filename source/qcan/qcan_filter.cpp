//====================================================================================================================//
// File:          qcan_filter.cpp                                                                                     //
// Description:   QCAN classes - CAN message filter                                                                   //
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

#include "qcan_filter.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*\
** Class methods                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/



//--------------------------------------------------------------------------------------------------------------------//
// QCanFilter()                                                                                                       //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanFilter::QCanFilter()
{
   //---------------------------------------------------------------------------------------------------
   // set default values
   //
   teFilterTypeP     = QCanFilter::eFILTER_OFF;
   teFormatP         = QCanFrame::eFORMAT_CAN_STD;
   ulIdentifierLowP  = 0;
   ulIdentifierHighP = 0;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFilter()                                                                                                       //
// copy constructor                                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
QCanFilter::QCanFilter(const QCanFilter &clOtherR)
{
   teFilterTypeP     = clOtherR.teFilterTypeP;
   teFormatP         = clOtherR.teFormatP;
   ulIdentifierLowP  = clOtherR.ulIdentifierLowP;
   ulIdentifierHighP = clOtherR.ulIdentifierHighP;
}


//--------------------------------------------------------------------------------------------------------------------//
// ~QCanFilter()                                                                                                      //
// destructor                                                                                                         //
//--------------------------------------------------------------------------------------------------------------------//
QCanFilter::~QCanFilter()
{

}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFilter::acceptFrame()                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanFilter::acceptFrame(const QCanFrame::FrameFormat_e & teFormatR,
                             const uint32_t ulIdentifierLowV, const uint32_t ulIdentifierHighV)
{
   bool  btResultT = false;


   //---------------------------------------------------------------------------------------------------
   // Disable filter in advance just in case the operation fails.
   //
   teFilterTypeP = QCanFilter::eFILTER_OFF;

   //---------------------------------------------------------------------------------------------------
   // The higher limit for the identifier can't be lower than the lower limit
   //
   if (ulIdentifierHighV >= ulIdentifierLowV)
   {
      if ((teFormatR == QCanFrame::eFORMAT_CAN_STD) || (teFormatR == QCanFrame::eFORMAT_FD_STD))
      {
         //-----------------------------------------------------------------------------------
         // Check maximum value for 11-bit identifier
         //
         if (ulIdentifierHighV <= QCAN_FRAME_ID_MASK_STD)
         {
            teFilterTypeP     = QCanFilter::eFILTER_ACCEPT;
            teFormatP         = teFormatR;
            ulIdentifierLowP  = ulIdentifierLowV;
            ulIdentifierHighP = ulIdentifierHighV;

            btResultT = true;
         }
      }
      else
      {
         //-----------------------------------------------------------------------------------
         // Check maximum value for 29-bit identifier
         //
         if (ulIdentifierHighV <= QCAN_FRAME_ID_MASK_EXT)
         {
            teFilterTypeP     = QCanFilter::eFILTER_ACCEPT;
            teFormatP         = teFormatR;
            ulIdentifierLowP  = ulIdentifierLowV;
            ulIdentifierHighP = ulIdentifierHighV;

            btResultT = true;
         }
      }
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFilter::match()                                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanFilter::match(const QCanFrame & clFrameR) const
{
   bool  btResultT = false;

   //---------------------------------------------------------------------------------------------------
   // Test for filter type "off"
   //
   if (teFilterTypeP == QCanFilter::eFILTER_OFF)
   {
      btResultT = true;
   }


   //---------------------------------------------------------------------------------------------------
   // Test for filter type "accept"
   //
   if (teFilterTypeP == QCanFilter::eFILTER_ACCEPT)
   {
      //-------------------------------------------------------------------------------------------
      // Test for the frame format first
      //
      if (clFrameR.frameFormat() == teFormatP)
      {

         //-----------------------------------------------------------------------------------
         // Test identifier range
         //
         if ( (clFrameR.identifier() >= ulIdentifierLowP) &&
              (clFrameR.identifier() <= ulIdentifierHighP)     )
         {
            btResultT = true;
         }
      }
   }


   //---------------------------------------------------------------------------------------------------
   //  Test for filter type "reject"
   //
   if (teFilterTypeP == QCanFilter::eFILTER_REJECT)
   {
      //-------------------------------------------------------------------------------------------
      // Test for the frame format first
      //
      if (clFrameR.frameFormat() == teFormatP)
      {

         //-----------------------------------------------------------------------------------
         // Test identifier range
         //
         if ( (clFrameR.identifier() >= ulIdentifierLowP) &&
              (clFrameR.identifier() <= ulIdentifierHighP)     )
         {
            btResultT = true;
         }
      }
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFilter::rejectFrame()                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanFilter::rejectFrame(const QCanFrame::FrameFormat_e & teFormatR,
                             const uint32_t ulIdentifierLowV, const uint32_t ulIdentifierHighV)
{
   bool  btResultT;

   //---------------------------------------------------------------------------------------------------
   // Testing for allowed parameter range is performed by method acceptFrame().
   // If the call succeeds, we simply change the filter type.
   //
   btResultT = acceptFrame(teFormatR, ulIdentifierLowV, ulIdentifierHighV);
   if (btResultT)
   {
      teFilterTypeP     = QCanFilter::eFILTER_REJECT;
   }

   return (btResultT);
}


