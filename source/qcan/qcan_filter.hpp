//====================================================================================================================//
// File:          qcan_filter.hpp                                                                                     //
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


#ifndef QCAN_FILTER_HPP_
#define QCAN_FILTER_HPP_


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include "qcan_frame.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/




//----------------------------------------------------------------------------------------------------------------
/*!
** \class   QCanFilter
**
** <p>
*/
class QCanFilter
{
public:
   

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \enum    FilterType_e
   **
   ** This enumeration defines possible filter types.
   */
   typedef enum FilterType_e {

      eFILTER_OFF = 0,

      /*! Accept: CAN frames meeting the condition will pass the filter                */
      eFILTER_ACCEPT,

      /*! Reject:  CAN frames meeting the condition will be rejected by the filter     */
      eFILTER_REJECT

   } FilterType_te;



   //---------------------------------------------------------------------------------------------------
   /*!
   ** Constructs an empty filter
   */
   QCanFilter();
   
   QCanFilter(const QCanFilter &clOtherR);

   virtual ~QCanFilter();
   

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clFrameR - CAN frame
   **
   ** \return     \c TRUE if parameter values are valid
   ** \see        match()
   **
   ** The function configures an acceptance filter for CAN frames with a frame format defined by
   ** \c teFormatR. All CAN frames with an identifier value within the range from \c ulIdentifierLowV
   ** to \c ulIdentifierHighV will match the filter condition.
   ** <p>
   ** In case the supplied parameters for \c ulIdentifierLowV and \c ulIdentifierHighV are not valid
   ** (e.g. identifier value > #QCAN_FRAME_ID_MASK_STD for Extended frames) the function will return
   ** \c FALSE and the filter type is set to QCanFilter::eFILTER_OFF.
   **
   */
   bool           acceptFrame(const QCanFrame::FrameFormat_e & teFormatR,
                              const uint32_t ulIdentifierLowV, const uint32_t ulIdentifierHighV);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clFrameR - CAN frame
   **
   ** \return  \c TRUE if filter condition is met
   **
   ** The function tests the CAN frame \c clFrameR with regards to to configured filter type.
   ** For an acceptance filter (QCanFilter::eFILTER_ACCEPT) the function returns \c TRUE if
   ** the CAN frame meets the acceptance condition (e.g. identifier value).
   ** <p>
   ** For a rejection filter (QCanFilter::eFILTER_REJECT) the function returns \c TRUE if
   ** the CAN frame meets the rejection condition (e.g. identifier value).
   **
   */
   bool           match(const QCanFrame & clFrameR) const;

   bool           rejectFrame(const QCanFrame::FrameFormat_e & teFormatR,
                              const uint32_t ulIdentifierLowV, const uint32_t ulIdentifierHighV);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Current filter type
   **
   ** The function returns the current filter type, which is implicitly set by the functions
   ** acceptFrame() or rejectFrame().
   */
   FilterType_te  type(void) const        {  return (teFilterTypeP);    };

private:
   
   FilterType_e               teFilterTypeP;
   QCanFrame::FrameFormat_e   teFormatP;
   uint32_t                   ulIdentifierLowP;
   uint32_t                   ulIdentifierHighP;

};





#endif   // QCAN_FILTER_HPP_

