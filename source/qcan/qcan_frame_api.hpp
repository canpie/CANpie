//============================================================================//
// File:          qcan_frame_api.hpp                                          //
// Description:   QCAN classes - CAN API frame                                //
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



#ifndef QCAN_FRAME_API_HPP_
#define QCAN_FRAME_API_HPP_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "qcan_data.hpp"

using namespace QCan;

//-----------------------------------------------------------------------------
/*!
** \class   QCanFrameApi
** \brief   CAN API frame
** 
** The QCanFrameApi class is used to transfer information between a CAN
** network (QCanNetwork, i.e. the server) and a CAN socket (QCanSocket, i.e.
** the client). The information exchange includes the current bit-rate
** settings, the CAN mode (refer to CAN_Mode_e) and the name of the
** CAN interface (QCanPlugin).
*/
class QCanFrameApi :  private QCanData
{
public:
  
   enum ApiFunc_e {

      /*! Function code for no operation                 */
      eAPI_FUNC_NONE   = 0,

      /*! Function code for bit-rate information         */
      eAPI_FUNC_BITRATE,

      /*! Function code for CAN mode information         */
      eAPI_FUNC_CAN_MODE,

      /*! Function code for CAN interface name           */
      eAPI_FUNC_NAME

   };

   /*!
   ** Constructs an empty CAN API frame with a function
   ** code of QCanFrameApi::eAPI_FUNC_NONE.
   */
   QCanFrameApi();
   
   
   ~QCanFrameApi();


   /*!
   ** \return     Bit-rate value for Data Bit Timing
   ** \see        setBitrate()
   **
   ** This method returns the data bit-rate of the CAN network for
   ** API frames which have a function() return value of
   ** QCanFrameApi::eAPI_FUNC_BITRATE.
   **
   */
   int32_t           dataBitrate(void);


   /*!
   ** \return     API function code
   **
   ** This method returns the function code of the QCanFrameApi
   ** instance (QCanFrameApi::ApiFunc_e). The function code is
   ** set by the methods setBitrate(), setMode() and setName().
   **
   */
   ApiFunc_e         function(void);


   /*!
   ** \return     Name
   ** \see        setName()
   **
   ** This method returns the name string for API frames which
   ** have a function() return value of QCanFrameApi::eAPI_FUNC_NAME.
   **
   */
   QString           name(void);

   /*!
   ** \return     Bit-rate value for Nominal Bit Timing
   ** \see        setBitrate()
   **
   ** This method returns the nominal bit-rate of the CAN network for
   ** API frames which have a function() return value of
   ** QCanFrameApi::eAPI_FUNC_BITRATE.
   **
   */
   int32_t           nominalBitrate(void);


   /*!
   ** \return     CAN mode
   ** \see        setMode()
   **
   ** This method returns the mode (CAN_Mode_e) value for API frames which
   ** have a function() return value of QCanFrameApi::eAPI_FUNC_BITRATE.
   **
   */
   CAN_Mode_e        mode(void);


   /*!
   ** \param[in]  slNomBitRateV  Nominal Bit-rate value
   ** \param[in]  slDatBitRateV  Data Bit-rate value
   ** \see        dataBitrate(), nominalBitrate()
   **
   ** This method sets the bit-rate values for the nominal bit-rate
   ** and the data bit-rate. The API function code value is set to
   ** QCanFrameApi::eAPI_FUNC_BITRATE.
   **
   */
   void              setBitrate(int32_t slNomBitRateV,
                                int32_t slDatBitRateV = eCAN_BITRATE_NONE);


   /*!
   ** \param[in]  teModeV        Mode of CAN interface
   ** \see        mode()
   **
   ** This method sets the mode of the CAN interface.
   ** The API function code value is set to
   ** QCanFrameApi::eAPI_FUNC_CAN_MODE.
   **
   */
   void              setMode(CAN_Mode_e teModeV);

   /*!
   ** \param[in]  clNameV        Name of CAN interface
   ** \see        name()
   **
   ** This method sets the name of the CAN interface, the maximum string
   ** length is limited to #QCAN_MSG_DATA_MAX bytes. Longer strings are
   ** truncated.
   ** The API function code value is set to QCanFrameApi::eAPI_FUNC_NAME.
   **
   */
   void              setName(QString clNameV);
   
   bool              fromByteArray(const QByteArray & clByteArrayR);
   QByteArray        toByteArray() const;
   virtual QString   toString(const bool & btShowTimeR = false);

private:
   
   
};





#endif   // QCAN_FRAME_API_HPP_ 

