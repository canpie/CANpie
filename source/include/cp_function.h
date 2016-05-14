//****************************************************************************//
// File:          cp_client.h                                                 //
// Description:   CANpie Client Server defines, structures and functions      //
// Author:        Johann Tiderko                                              //
// e-mail:        johann.tiderko@microcontrol.net                             //
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
//    the referenced file 'COPYING'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'COPYING' file.                   //
//                                                                            //
//----------------------------------------------------------------------------//
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 30.10.2014  Initial version                                                //
//                                                                            //
//****************************************************************************//



#ifndef  CP_FUNCTION_H_
#define  CP_FUNCTION_H_


//-----------------------------------------------------------------------------
// SVN  $Date: 2015-05-27 15:06:57 +0200 (Wed, 27. May 2015) $
// SVN  $Rev: 6726 $ --- $Author: tiderko $
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/*!
** \file    cp_function.h
** \brief   CANpie Client Server functions prototypes and common defines
**
*/


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "compiler.h"

//-------------------------------------------------------------------//
// take precautions if compiled with C++ compiler                    //
#ifdef __cplusplus                                                   //
extern "C" {                                                         //
#endif                                                               //
//-------------------------------------------------------------------//

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


//-------------------------------------------------------------------
/*!
** \def     CP_CLN_MSG_ID_
** \brief   CANpie Client - Server function flag bit definitions
*/
#define CP_FUNC_FLAG                   ((uint32_t)(0x20000000))

//-------------------------------------------------------------------
/*!
** \defgroup   CP_CS_SERVICE_
** \brief      CANpie Client - Server function number definitions
*/
//!@{
#define CP_FUNC_NOP          ((uint32_t)(0x00))
#define CP_FUNC_BAUDRATE     ((uint32_t)(0x01))
#define CP_FUNC_CAN_MODE     ((uint32_t)(0x02))
#define CP_FUNC_CAN_STATE    ((uint32_t)(0x03))
#define CP_FUNC_DRV_INIT     ((uint32_t)(0x04))
#define CP_FUNC_DRV_RELEASE  ((uint32_t)(0x05))
#define CP_FUNC_HDI          ((uint32_t)(0x06))
#define CP_FUNC_MASK         ((uint32_t)(0xFF))
//!@}


//-------------------------------------------------------------------
/*!
** \defgroup   CP_CS_MSG_ID_CAN_
** \brief      CANpie Client - Server CAN interface number definitions
*/
//!@{
#define CP_CLN_MSG_ID_CAN0              ((uint32_t)(0x0000))
#define CP_CLN_MSG_ID_CAN1              ((uint32_t)(0x0100))
//!@}





/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

/*!
** \brief This function is implemented by user depending on hardware
** \param ubNetV - Network number
** \param pubDataV - Pinter to the buffer for CAN read data
** \param ubLengthV - Number of bytes to read, typically multiple of sizeof(CpClnMsg_ts)
** \return Number of bytes that have been read, or negative value in case of error
*/
int32_t CpClnHalRead(uint8_t ubNetV, uint8_t *pubDataV, uint32_t ubLengthV);

/*!
** \brief This function is implemented by user depending on hardware
** \param ubNetV - Network number
** \param pubDataV - Pinter to CAN write data
** \param ubLengthV - Number of bytes to write, typically multiple of sizeof(CpClnMsg_ts)
** \return Number of bytes that have been written, or negative value in case of error
*/
int32_t CpClnHalWrite(uint8_t ubNetV, uint8_t *pubDataV, uint32_t ubLengthV);

//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//

#endif   /* CP_CLIENT_H_ */
