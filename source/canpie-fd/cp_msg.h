//============================================================================//
// File:          cp_msg.h                                                    //
// Description:   CANpie message access functions                             //
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


#ifndef  CP_MSG_H_
#define  CP_MSG_H_



//-----------------------------------------------------------------------------
/*!
** \file    cp_msg.h
** \brief   %CANpie message access
** \anchor  CP_MSG_H
**
** In order to create small and fast code, %CANpie supplies a set of
** macros to access the CAN message structure (CpCanMsg_ts). These
** macros can be used instead of the functions defined in the cp_msg.h
** header file. However keep in mind that macros can't be used to check
** for value ranges or parameter consistency. Usage of macros is enabled
** via the symbol #CP_CAN_MSG_MACRO.<p>
** \b Example
** \code
** //--- setup a CAN message ----------------------------------------
** CpCanMsg_ts   myMessage;
** CpMsgInit(&myMessage, CP_MSG_FORMAT_CBFF);   // initialise message
** CpMsgSetIdentifier(&myMessage, 100);         // identifier is 100 dec.
** CpMsgSetDlc(&myMessage, 2);                  // data length code is 2
** CpMsgSetData(&myMessage, 0, 0x11);           // byte 0 has the value 0x11
** CpMsgSetData(&myMessage, 1, 0x22);           // byte 1 has the value 0x22
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

#include "canpie.h"

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
**
** \deprecated It is advised to use CpMsgInit() for new applications.
**
*/
void  CpMsgClear(CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Clear Overrun bit
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \see     CpMsgSetOverrun()
**
** This function clears the Overrun bit (RTR).
*/
void  CpMsgClrOverrun(CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Clear RTR bit
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \see     CpMsgSetRemote()
**
** This function clears the Remote Transmission bit (RTR).
*/
void  CpMsgClrRemote(CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Convert DLC to payload size
** \param   ubDlcV      Data Length Code
** \return  Number of bytes in CAN payload
** \see     CpMsgSizeToDlc()
**
** This function converts the DLC value to the corresponding size of the CAN
** message payload. 
** 
*/
uint8_t CpMsgDlcToSize(const uint8_t ubDlcV);


//------------------------------------------------------------------------------
/*!
** \brief   Get Data
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \param   ubPosV      Zero based index of byte position
** \return  Value of data at position ubPosV
**
** This function retrieves the data of a CAN message. The parameter
** \c ubPosV must be within the range 0 .. 7 for Classical CAN frames. For
** ISO CAN FD frames the valid range is 0 .. 63. Please note that the
** macro implementation does not check the value range of the parameter
** \c ubPosV.
*/
uint8_t  CpMsgGetData(const CpCanMsg_ts *ptsCanMsgV, uint8_t ubPosV);


//------------------------------------------------------------------------------
/*!
** \brief   Get Data Length Code
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  Data Length Code (DLC) of CAN message
**
** This function retrieves the data length code (DLC) of a CAN message.
** The return value range is between 0 and 8.
*/
uint8_t  CpMsgGetDlc(const CpCanMsg_ts *ptsCanMsgV);


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
**
** \deprecated It is advised to use CpMsgGetIdentifier() for new applications.
**
*/
uint32_t  CpMsgGetExtId(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Get Identifier Value
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  Identifier value
** \see     CpMsgSetIdentifier()
**
** This function retrieves the value for the identifier of a CAN frame.
** The frame format of the CAN message can be tested with the
** CpMsgIsExtended() function.
*/
uint32_t  CpMsgGetIdentifier(const CpCanMsg_ts *ptsCanMsgV);


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
**
** \deprecated It is advised to use CpMsgGetIdentifier() for new applications.
**
*/
uint16_t  CpMsgGetStdId(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Get time-stamp value
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  Pointer to time-stamp structure
** \see     CpMsgSetTime
**
** This function returns the time-stamp value for a CAN frame.
*/
/*@null@*/ CpTime_ts   *CpMsgGetTime(CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Initialise message structure
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \param   ubFormatV   Frame format
**
** This function sets the identifier field and the DLC field
** of a CAN message structure to 0. The parameter \a ubFormatV defines
** the frame format. Possible value are:
** - #CP_MSG_FORMAT_CBFF : Classical base frame format
** - #CP_MSG_FORMAT_CEFF : Classical extended frame format
** - #CP_MSG_FORMAT_FBFF : FD base frame format
** - #CP_MSG_FORMAT_FEFF : FD extended frame format
**
** The contents of the data field and all other optional fields
** (time-stamp, user, message marker) are not altered.
*/
void      CpMsgInit(CpCanMsg_ts *ptsCanMsgV, uint8_t ubFormatV);

//------------------------------------------------------------------------------
/*!
** \brief   Check for bit-rate
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  \c true if bit-rate switch is set
** \see     CpMsgSetBitrateSwitch()
**
** This function checks the frame type. If the frame is a CAN FD
** frame and the bit-rate switch (BRS) bit is set, the value \c true
** is returned.
*/
bool_t    CpMsgIsBitrateSwitchSet(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Check for Extended CAN frame
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  \c true for Extended CAN frame
**
** This function checks the frame type. If the frame is CAN 2.0A
** (Standard Frame), the value \c false is returned. If the frame is
** CAN 2.0B (Extended Frame), the value \c true is returned.
*/
bool_t   CpMsgIsExtended(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Check for CAN FD frame
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  \c true for CAN FD frame
**
** This function checks the frame type. If the frame is a CAN FD frame
** CAN 2.0B (Extended Frame), the value \c true is returned.
*/
bool_t   CpMsgIsFdFrame(const CpCanMsg_ts *ptsCanMsgV);

//------------------------------------------------------------------------------
/*!
** \brief   Check for message overrun
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  1 for data overrun, else 0
**
** This function checks if a data overrun has occurred for the message.
*/

bool_t   CpMsgIsOverrun(const CpCanMsg_ts *ptsCanMsgV);


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
bool_t   CpMsgIsRemote(const CpCanMsg_ts *ptsCanMsgV);

//------------------------------------------------------------------------------
/*!
** \brief   Check for Remote Procedure Call
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \return  0 for CAN frame, 1 for Remote Procedure Call
**
** This function checks if the Remote Procedure Call bit (RPC) is
** set. If the RPC bit is set, the function will return 1, otherwise
** 0.
*/
bool_t   CpMsgIsRpc(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief      Get CAN controller mode via RPC
** \param[in]  ptsCanMsgV  Pointer to a CpCanMsg_ts message
** 
** \return     Mode selection
**
** This function returns the operating mode of the CAN controller which is 
** provided via a Remote Procedure Call.
** Possible return values are defined in the #CpMode_e enumeration.
*/
uint8_t CpMsgRpcGetCanMode(const CpCanMsg_ts *ptsCanMsgV);

//------------------------------------------------------------------------------
/*!
** \brief      Get Data Bit Timing selection via RPC
** \param[in]  ptsCanMsgV  Pointer to a CpCanMsg_ts message
** 
** \return     Data Bitrate
**
** This function returns the Data Bit Timing selection of the CAN controller
** which is provided via a Remote Procedure Call.
** Possible return values are defined in the #CpBitrate_e enumeration.
*/
int32_t CpMsgRpcGetDataBitrate(const CpCanMsg_ts *ptsCanMsgV);

//------------------------------------------------------------------------------
/*!
** \brief      Get Nominal Bit Timing selection via RPC
** \param[in]  ptsCanMsgV  Pointer to a CpCanMsg_ts message
** 
** \return     Nominal Bitrate
**
** This function returns the Nominal Bit Timing selection of the CAN controller
** which is provided via a Remote Procedure Call.
** Possible return values are defined in the #CpBitrate_e enumeration.
*/
int32_t CpMsgRpcGetNominalBitrate(const CpCanMsg_ts *ptsCanMsgV);

//------------------------------------------------------------------------------
/*!
** \brief      Set state of CAN controller via RPC
** \param[in]  ptsCanMsgV     Pointer to a CpCanMsg_ts message
** \param[in]  ubModeV        Mode selection
**
** This function changes the operating mode of the CAN controller FSA.
** Possible values for the parameter \c ubModeV are defined in the
** #CpMode_e enumeration.
*/
void     CpMsgRpcSetCanMode(CpCanMsg_ts *ptsCanMsgV, uint8_t ubModeV);

//------------------------------------------------------------------------------
/*!
** \brief      Set bit-rate of CAN controller via RPC
** \param[in]  ptsCanMsgV     Pointer to a CpCanMsg_ts message
** \param[in]  slNomBitRateV  Nominal Bit Timing selection
** \param[in]  slDatBitRateV  Data Bit Timing selection
**
** This function initializes the bit timing registers of a CAN controller
** to pre-defined values. The values are defined by the enumeration
** #CpBitrate_e. 
*/
void     CpMsgRpcSetBitrate(CpCanMsg_ts *ptsCanMsgV, int32_t slNomBitRateV,
                            int32_t slDatBitRateV);

//------------------------------------------------------------------------------
/*!
** \brief      Identify Remote Procedure Call 
** \param[in]  ptsCanMsgV     Pointer to a CpCanMsg_ts message
** \return     Value from #CpRpc_e enumeration
**
** This function returns a value defined inside the #CpRpc_e enumeration
** which identifies the RPC type.
*/
uint32_t  CpMsgRpcType(const CpCanMsg_ts *ptsCanMsgV);

//------------------------------------------------------------------------------
/*!
** \brief   Set bit-rate switch
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \see     CpMsgIsBitrateSwitch()
**
** This function checks the frame type. If the frame is a CAN FD
** frame the bit-rate switch (BRS) bit is set, otherwise the bit
** value in the message control field is not altered.
*/
void     CpMsgSetBitrateSwitch(CpCanMsg_ts *ptsCanMsgV);

//------------------------------------------------------------------------------
/*!
** \brief   Set Data
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \param   ubPosV      Zero based index of byte position
** \param   ubValueV    Value of data byte in CAN message
**
** This function sets the data in a CAN message. The parameter
** \c ubPosV must be within the range 0 .. 7 for Classical CAN frames. For
** ISO CAN FD frames the valid range is 0 .. 63. Please note that the
** macro implementation does not check the value range of the parameter
** \c ubPosV.
*/
void  CpMsgSetData(CpCanMsg_ts *ptsCanMsgV, uint8_t ubPosV, uint8_t ubValueV);


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
void  CpMsgSetDlc(CpCanMsg_ts *ptsCanMsgV, uint8_t ubDlcV);


//------------------------------------------------------------------------------
/*!
** \brief   Set 29 Bit Identifier Value
** \param   ptsCanMsgV     Pointer to a CpCanMsg_ts message
** \param   ulExtIdV       Identifier value
** \see     CpMsgSetStdId()
**
** This function sets the identifier value for an
** Extended Frame (CAN 2.0B) and marks the frame format as
** accordingly. The value of \c ulExtIdV is limited to
** #CP_MASK_EXT_FRAME.
**
** \deprecated It is advised to use CpMsgSetIdentifier() for new applications.
*/
void  CpMsgSetExtId(CpCanMsg_ts *ptsCanMsgV, uint32_t ulExtIdV);


//------------------------------------------------------------------------------
/*!
** \brief   Set Identifier Value
** \param   ptsCanMsgV     Pointer to a CpCanMsg_ts message
** \param   ulIdentifierV  Identifier value
** \see     CpMsgGetIdentifier()
**
** This function sets the identifier value for a CAN frame. The
** parameter \c ulIdentifierV is truncated to a 11-bit value (AND
** operation with #CP_MASK_STD_FRAME) when the message uses base
** frame format. The parameter \c ulIdentifierV is truncated to
** a 29-bit value (AND operation with #CP_MASK_EXT_FRAME) when the
** message uses extended frame format.
**
*/
void  CpMsgSetIdentifier(CpCanMsg_ts *ptsCanMsgV, uint32_t ulIdentifierV);


//------------------------------------------------------------------------------
/*!
** \brief   Set Overrun bit
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \see     CpMsgClrOverrun()
**
** This function sets the Overrun bit.
*/
void  CpMsgSetOverrun(CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Set RTR bit
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \see     CpMsgClrRemote()
**
** This function sets the Remote Transmission bit (RTR).
*/
void  CpMsgSetRemote(CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------
/*!
** \brief   Set 11 Bit Identifier Value
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \param   uwStdIdV    Identifier value
** \see     CpMsgSetExtFrame()
**
** This function sets the identifier value for a Standard frame (CAN 2.0A).
** The value of \c uwStdIdV is limited to #CP_MASK_STD_FRAME.
**
** \deprecated It is advised to use CpMsgSetIdentifier() for new applications.
*/
void  CpMsgSetStdId(CpCanMsg_ts *ptsCanMsgV, uint16_t uwStdIdV);


//------------------------------------------------------------------------------
/*!
** \brief   Set time-stamp value
** \param   ptsCanMsgV  Pointer to a CpCanMsg_ts message
** \param   ptsTimeV    Pointer to time-stamp structure
**
** This function sets the time-stamp value for a CAN frame.
*/
void  CpMsgSetTime(CpCanMsg_ts *ptsCanMsgV, const CpTime_ts *ptsTimeV);


//------------------------------------------------------------------------------
/*!
** \brief   Convert CAN payload size to DLC
** \param   ubSizeV      Size of CAN message payload in bytes
** \return  DLC value
** \see     CpMsgDlcToSize()
**
** This function converts the size of the CAN message payload into a DLC value. 
** 
*/
uint8_t CpMsgSizeToDlc(uint8_t ubSizeV);




//-------------------------------------------------------------------//
// Macros for CpMsgXXX() commands                                    //
//-------------------------------------------------------------------//
#if   CP_CAN_MSG_MACRO == 1

#define  CpMsgClear(MSG_PTR)                                      \
         do {                                                     \
            (MSG_PTR)->ulIdentifier = 0;                          \
            (MSG_PTR)->ubMsgCtrl = 0;                             \
            (MSG_PTR)->ubMsgDLC  = 0;                             \
         } while(0)

#define  CpMsgClrOverrun(MSG_PTR)                                 \
         do {                                                     \
            (MSG_PTR)->ubMsgCtrl &= ~CP_MSG_CTRL_OVR_BIT;         \
         } while(0)

#define  CpMsgClrRemote(MSG_PTR)                                  \
         do {                                                     \
            (MSG_PTR)->ubMsgCtrl &= ~CP_MSG_CTRL_RTR_BIT;         \
         } while(0)


#define  CpMsgGetData(MSG_PTR, POS)                               \
            ((MSG_PTR)->tuMsgData.aubByte[POS])

#define  CpMsgGetDlc(MSG_PTR)                                     \
            ((MSG_PTR)->ubMsgDLC)

#define  CpMsgGetExtId(MSG_PTR)                                   \
            ((MSG_PTR)->ulIdentifier)

#define  CpMsgGetIdentifier(MSG_PTR)                              \
            ( ((MSG_PTR)->ulIdentifier) & CP_MASK_EXT_FRAME )

#define  CpMsgGetStdId(MSG_PTR)                                   \
            (uint16_t)((MSG_PTR)->ulIdentifier)

#define  CpMsgGetTime(MSG_PTR)                                    \
            (&(MSG_PTR)->tsMsgTime)

#define  CpMsgInit(MSG_PTR, VAL)                                  \
         do {                                                     \
            (MSG_PTR)->ubMsgCtrl = (VAL);                         \
            (MSG_PTR)->ulIdentifier = 0;                          \
            (MSG_PTR)->ubMsgDLC  = 0;                             \
         } while(0)

#define  CpMsgIsBitrateSwitchSet(MSG_PTR)                         \
            ( (MSG_PTR)->ubMsgCtrl & CP_MSG_CTRL_BRS_BIT  )

#define  CpMsgIsExtended(MSG_PTR)                                 \
            ( (MSG_PTR)->ubMsgCtrl & CP_MSG_CTRL_EXT_BIT )

#define  CpMsgIsFdFrame(MSG_PTR)                                  \
            ( (MSG_PTR)->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT  )

#define  CpMsgIsOverrun(MSG_PTR)                                  \
            ( (MSG_PTR)->ubMsgCtrl & CP_MSG_CTRL_OVR_BIT )

#define  CpMsgIsRemote(MSG_PTR)                                   \
            ( (MSG_PTR)->ubMsgCtrl & CP_MSG_CTRL_RTR_BIT )

#define  CpMsgSetBitrateSwitch(MSG_PTR)                           \
         do {                                                     \
            if (( (MSG_PTR)->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT  ))  \
            { (MSG_PTR)->ubMsgCtrl |= CP_MSG_CTRL_BRS_BIT; }      \
         } while(0)

#define  CpMsgSetData(MSG_PTR, POS, VAL)                          \
            ( (MSG_PTR)->tuMsgData.aubByte[POS] = (VAL) )

#define  CpMsgSetDlc(MSG_PTR, DLC)                                \
         do {                                                     \
            (MSG_PTR)->ubMsgDLC = (DLC);                          \
         } while(0)


#define  CpMsgSetExtId(MSG_PTR, VAL)                              \
         do {                                                     \
            (MSG_PTR)->ulIdentifier = (VAL) & CP_MASK_EXT_FRAME;  \
            (MSG_PTR)->ubMsgCtrl |= CP_MSG_CTRL_EXT_BIT;          \
         } while(0)

#define  CpMsgSetIdentifier(MSG_PTR, VAL)                            \
         do {                                                        \
            if ((MSG_PTR)->ubMsgCtrl & CP_MSG_CTRL_EXT_BIT)          \
            { (MSG_PTR)->ulIdentifier = (VAL) & CP_MASK_EXT_FRAME; } \
            else                                                     \
            { (MSG_PTR)->ulIdentifier = (VAL) & CP_MASK_STD_FRAME; } \
         } while(0)

#define  CpMsgSetOverrun(MSG_PTR)                                 \
         do {                                                     \
            (MSG_PTR)->ubMsgCtrl |= CP_MSG_CTRL_OVR_BIT;          \
         } while(0)

#define  CpMsgSetRemote(MSG_PTR)                                  \
         do {                                                     \
            if (((MSG_PTR)->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT) == 0)\
            { (MSG_PTR)->ubMsgCtrl |= CP_MSG_CTRL_RTR_BIT; }      \
         } while(0)

#define  CpMsgSetStdId(MSG_PTR, VAL)                              \
         do {                                                     \
            (MSG_PTR)->ulIdentifier = (VAL) & CP_MASK_STD_FRAME;  \
            (MSG_PTR)->ubMsgCtrl &= ~CP_MSG_CTRL_EXT_BIT;         \
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

