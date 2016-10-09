//============================================================================//
// File:          cp_msg.h                                                    //
// Description:   CANpie message access functions                             //
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


#ifndef  CP_MSG_H_
#define  CP_MSG_H_


//-------------------------------------------------------------------//
// take precautions if compiled with C++ compiler                    //
#ifdef __cplusplus                                                   //
extern "C" {                                                         //
#endif                                                               //
//-------------------------------------------------------------------//



//-----------------------------------------------------------------------------
/*!
** \file    cp_msg.h
** \brief   CANpie message access
**
** &nbsp;<p>
** In order to create small and fast code, CANpie supplies a set of
** macros to access the CAN message structure (#CpCanMsg_ts). These
** macros can be used instead of the functions defined in the cp_msg.h
** header file. However keep in mind that macros can't be used to check
** for value ranges or parameter consistency. Usage of macros is enabled
** via the symbol #CP_CAN_MSG_MACRO.<p>
** \b Example
** \code
** //--- setup a CAN message ----------------------------------------
** CpCanMsg_ts   myMessage;
** CpMsgClear(&myMessage);                // clear the message
** CpMsgSetStdId(&myMessage, 100, 0);     // identifier is 100 dec, no RTR
** CpMsgSetDlc(&myMessage, 2);            // data length code is 2
** CpMsgSetData(&myMessage, 0, 0x11);     // byte 0 has the value 0x11
** CpMsgSetData(&myMessage, 1, 0x22);     // byte 1 has the value 0x22
** //... do something with it ....
**
** //--- evaluate a message that was received -----------------------
** CpCanMsg_ts   receiveMsg;
** //... receive the stuff ....
**
** if(CpMsgIsExtended(&receiveMsg))
** {
**    //--- this is an Extended Frame ---------------------
**    DoExtendedMessageService();
**    return;
** }
**
** if(CpMsgIsRemote(&receiveMsg))
** {
**    //... do something with RTR frames
** }
** \endcode
*/


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "../version3/canpie.h"



/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/


//------------------------------------------------------------------------------
/*!
** \brief   Clear message structure
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
**
** This function sets the identifier field and the flags field
** of a CAN message structure to 0. It is recommended to use
** this function when a message structure is assigned in memory.
*/
void  CpMsgClear(CpCanMsg_ts * ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Clear Overrun bit
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \see     CpMsgSetOverrun()
**
** This function clears the Overrun bit (RTR).
*/
void  CpMsgClrOverrun(CpCanMsg_ts * ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Clear RTR bit
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \see     CpMsgSetRemote()
**
** This function clears the Remote Transmission bit (RTR).
*/
void  CpMsgClrRemote(CpCanMsg_ts * ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Get Data
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \param   ubPosV      Zero based index of byte position
** \return  Value of data at position ubPosV
**
** This functions retrieves the data of a CAN message. The parameter
** \c ubPosV must be within the range 0 .. 7. Please note that the
** macro implementation does not check the value range of the parameter 
** \c ubPosV.
*/
uint8_t  CpMsgGetData(CpCanMsg_ts * ptsCanMsgV, uint8_t ubPosV);


//------------------------------------------------------------------------------
/*!
** \brief   Get Data Length Code
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  Data Length Code (DLC) of CAN message
**
** This function retrieves the data length code (DLC) of a CAN message.
** The return value range is between 0 and 8. 
*/
uint8_t  CpMsgGetDlc(CpCanMsg_ts * ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Get 29 Bit Identifier Value
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  Extended Identifier value
** \see     CpMsgGetStdId()
**
** This function retrieves the value for the identifier of an
** extended frame (CAN 2.0B). The frame format of the CAN message
** can be tested with the CpMsgIsExtended() function.
*/
uint32_t  CpMsgGetExtId(CpCanMsg_ts * ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Get 11 Bit Identifier Value
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  Standard Identifier value
** \see     CpMsgGetExtId()
** 
** This macro retrieves the value for the identifier of an
** standard frame (CAN 2.0A). The frame format of the CAN message
** can be tested with the CpMsgIsExtended() function.
*/
uint16_t  CpMsgGetStdId(CpCanMsg_ts * ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Check message enable state
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  0 for disabled, 1 for enabled
**
** This function checks the message enable state.
*/
uint8_t   CpMsgIsEnabled(CpCanMsg_ts * ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Check the frame type
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  0 for CAN 2.0A, 1 for CAN 2.0B
**
** This function checks the frame type. If the frame is CAN 2.0A
** (Standard Frame), the value 0 is returned. If the frame is
** CAN 2.0B (Extended Frame), the value 1 is returned.
*/
uint8_t  CpMsgIsExtended(CpCanMsg_ts * ptsCanMsgV);

uint8_t  CpMsgIsFastData(CpCanMsg_ts * ptsCanMsgV);

//------------------------------------------------------------------------------
/*!
** \brief   Check for message overrun
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  1 for data overrun, else 0
**
** This function checks if a data overrun has occurred for the message.
*/

uint8_t  CpMsgIsOverrun(CpCanMsg_ts * ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Check for remote frame
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  0 for data frame, 1 for remote frame
** 
** This function checks if the Remote Transmission bit (RTR) is
** set. If the RTR bit is set, the function will return 1, otherwise
** 0.
*/
uint8_t  CpMsgIsRemote(CpCanMsg_ts * ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Set Data
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \param   ubPosV      Zero based index of byte position
** \param   ubValueV    Value of data byte in CAN message
**
** This function sets the data in a CAN message. The parameter
** \c ubPosV must be within the range 0 .. 7. Please note that the
** macro implementation does not check the value range of the parameter 
** \c ubPosV.
*/
void  CpMsgSetData(CpCanMsg_ts * ptsCanMsgV, uint8_t ubPosV, uint8_t ubValueV);


//------------------------------------------------------------------------------
/*!
** \brief   Set Data Length Code
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \param   ubDlcV      Data Length Code
**
** This function sets the Data Length Code (DLC) of a CAN message.
** The parameter ubDlcV must be within the range from 0..8.
** Please note that the macro implementation does not check the value
** range of the parameter \c ubDlcV.
*/
void  CpMsgSetDlc(CpCanMsg_ts * ptsCanMsgV, uint8_t ubDlcV);


//------------------------------------------------------------------------------
/*!
** \brief   Set 29 Bit Identifier Value
** \param   ptsCanMsgV     Pointer to a CpCanMsg_ts message
** \param   ulExtIdV       Identifier value
** \see     CpMsgSetStdFrame()
**
** This function sets the identifier value for an
** Extended Frame (CAN 2.0B) and marks the frame format as
** accordingly. The value of \c ulExtIdV is limited to
** #CP_MASK_EXT_FRAME.
*/
void  CpMsgSetExtId(CpCanMsg_ts * ptsCanMsgV, uint32_t ulExtIdV);


//------------------------------------------------------------------------------
/*!
** \brief   Set Overrun bit
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \see     CpMsgClrOverrun()
**
** This function sets the Overrun bit.
*/
void  CpMsgSetOverrun(CpCanMsg_ts * ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Set RTR bit
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \see     CpMsgClrRemote()
**
** This function sets the Remote Transmission bit (RTR).
*/
void  CpMsgSetRemote(CpCanMsg_ts * ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Set 11 Bit Identifier Value
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \param   uwStdIdV    Identifier value
** \see     CpMsgSetExtFrame()
**
** This function sets the identifier value for a Standard frame (CAN 2.0A).
** The value of \c uwStdIdV is limited to #CP_MASK_STD_FRAME.
*/
void  CpMsgSetStdId(CpCanMsg_ts * ptsCanMsgV, uint16_t uwStdIdV);


//------------------------------------------------------------------------------
/*!
** \brief   Set timestamp value
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \param   ptsTimeV    Pointer to timestamp structure
**
** This function sets the timestamp value for a CAN frame.
*/
void  CpMsgSetTime(CpCanMsg_ts * ptsCanMsgV, CpTime_ts * ptsTimeV);




//-------------------------------------------------------------------//
// Macros for CpMsgXXX() commands                                    //
//-------------------------------------------------------------------//
#if   CP_CAN_MSG_MACRO == 1

#define  CpMsgClear(MSG_PTR)                                      \
         do {                                                     \
            (MSG_PTR)->tuMsgId.ulExt = 0;                         \
            (MSG_PTR)->ubMsgCtrl = 0;                             \
            (MSG_PTR)->ubMsgDLC  = 0;                             \
         } while(0)

#define  CpMsgClrOverrun(MSG_PTR)                                 \
         do {                                                     \
            (MSG_PTR)->ubMsgCtrl &= ~CP_MASK_OVR_BIT;             \
         } while(0)

#define  CpMsgClrRemote(MSG_PTR)                                  \
         do {                                                     \
            (MSG_PTR)->ubMsgCtrl &= ~CP_MASK_RTR_BIT;             \
         } while(0)


#define  CpMsgGetData(MSG_PTR, POS)                               \
            (MSG_PTR)->tuMsgData.aubByte[POS]

#define  CpMsgGetDlc(MSG_PTR)                                     \
            ((MSG_PTR)->ubMsgDLC)

#define  CpMsgGetExtId(MSG_PTR)                                   \
            ((MSG_PTR)->tuMsgId.ulExt)

#define  CpMsgGetStdId(MSG_PTR)                                   \
            ((MSG_PTR)->tuMsgId.uwStd)

#define  CpMsgIsExtended(MSG_PTR)                                 \
            ( (MSG_PTR)->ubMsgCtrl & CP_MSG_CTRL_EXT_BIT )

#define  CpMsgIsOverrun(MSG_PTR)                                  \
            ( (MSG_PTR)->ubMsgCtrl & CP_MSG_CTRL_OVR_BIT )

#define  CpMsgIsRemote(MSG_PTR)                                   \
            ( (MSG_PTR)->ubMsgCtrl & CP_MASK_RTR_BIT )

#define  CpMsgSetData(MSG_PTR, POS, VAL)                          \
            ( (MSG_PTR)->tuMsgData.aubByte[POS] = VAL )

#define  CpMsgSetDlc(MSG_PTR, DLC)                                \
            ( (MSG_PTR)->ubMsgDLC = DLC )

#define  CpMsgSetExtId(MSG_PTR, VAL)                              \
         do {                                                     \
            (MSG_PTR)->tuMsgId.ulExt = VAL & CP_MASK_EXT_FRAME;   \
            (MSG_PTR)->ubMsgCtrl |= CP_MASK_EXT_BIT;              \
         } while(0)

#define  CpMsgSetOverrun(MSG_PTR)                                 \
         do {                                                     \
            (MSG_PTR)->ubMsgCtrl |= CP_MASK_OVR_BIT;              \
         } while(0)

#define  CpMsgSetRemote(MSG_PTR)                                  \
         do {                                                     \
            (MSG_PTR)->ubMsgCtrl |= CP_MASK_RTR_BIT;              \
         } while(0)

#define  CpMsgSetStdId(MSG_PTR, VAL)                              \
         do {                                                     \
            (MSG_PTR)->tuMsgId.uwStd = VAL & CP_MASK_STD_FRAME;   \
            (MSG_PTR)->ubMsgCtrl &= ~CP_MASK_EXT_BIT;             \
         } while(0)

#define  CpMsgSetTime(MSG_PTR, VAL)                               \
         do {                                                     \
            (MSG_PTR)->tsMsgTime.ulSec1970 = (VAL)->ulSec1970;    \
            (MSG_PTR)->tsMsgTime.ulNanoSec = (VAL)->ulNanoSec;    \
         } while(0)

#endif


//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//

#endif   /* CP_MSG_H_ */

