//====================================================================================================================//
// File:          qcan_filter_list.hpp                                                                                //
// Description:   QCAN classes - CAN frame filter list                                                                //
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


#ifndef QCAN_FILTER_LIST_HPP_
#define QCAN_FILTER_LIST_HPP_


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QVector>

#include "qcan_frame.hpp"
#include "qcan_filter.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/




//----------------------------------------------------------------------------------------------------------------
/*!
** \class   QCanFilterList
** \brief   Collection of CAN frame filters (QCanFilter)
**
** A filter list assembles one or multiple CAN frame filters (QCanFilter) into a composite structure that
** can be assigned to a CAN socket (refer to QCanSocket::setFilterList).
*/
class QCanFilterList
{
public:
   
   //---------------------------------------------------------------------------------------------------
   /*!
   ** Constructs an empty filter list.
   */
   QCanFilterList();
   
   virtual ~QCanFilterList() {}

   QCanFilterList(const QCanFilterList &clOtherR);
 
   //--------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clOtherR   Reference to filter list to assign
   ** \return     Reference to this filter list
   **
   ** Assigns \a clOtherR to this filter list and returns a reference to this filter list.
   */   
   QCanFilterList & operator=(const QCanFilterList &clOtherR);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clFilterR   CAN filter to append
   **
   ** \return     Position of filter inside the list
   **
   ** The function appends a CAN filter of type QCanFilter to the filter list.
   **
   */
   int32_t        appendFilter(const QCanFilter & clFilterR);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** Clear the filter list (remove all existing filters). An empty filter list does not filter any CAN
   ** frame; all CAN frames will pass.
   **
   */
   void           clear(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clFrameR   CAN frame to process
   **
   ** \return     \c TRUE if the frame is filtered (rejected), otherwise \c FALSE
   **
   ** Process the CAN frame \a clFrameR against all filters in the list. If the frame passes the filter
   ** list the function returns \c FALSE; if any filter causes rejection the function returns \c TRUE.
   **
   */
   bool           filter(const QCanFrame & clFrameR) const;

   //---------------------------------------------------------------------------------------------------
   /*!
   ** Return \c true if the filter list is empty (no filters installed).
   **
   */
   bool           isEmpty(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  slPositionR  Index of filter to remove (0-based)
   ** \return     \c true if the filter at \a slPositionR existed and was removed, otherwise \c false
   **
   ** Remove a filter by index from the filter list.
   **
   */
   bool           removeFilter(const int32_t slPositionR);
private:
   
   QVector<QCanFilter>     clFilterP;

};





#endif   // QCAN_FILTER_LIST_HPP_
