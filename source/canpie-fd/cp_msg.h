//====================================================================================================================//
// File:          cp_msg.h                                                                                            //
// Description:   CANpie frame access functions                                                                       //
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


#ifndef  CP_MSG_H_
#define  CP_MSG_H_



//------------------------------------------------------------------------------------------------------
/*!
** \file    cp_msg.h
** \brief   %CANpie message access
** \anchor  CP_MSG_H
**
** To create compact and fast code, %CANpie supplies a set of macros to access the CAN frame 
** structure (CpCanMsg_ts). These macros can be used instead of the functions defined in the cp_msg.h
** header file. However, keep in mind that macros cannot be used to check value ranges or parameter 
** consistency. Use of macros is enabled via the symbol #CP_CAN_MSG_MACRO.<p>
** \b Example
** \code
** //----------------------------------------------------------------
** // set up a CAN frame
** CpCanMsg_ts   myMessage;
** CpMsgInit(&myMessage, CP_MSG_FORMAT_CBFF);   // initialise as a classic standard CAN frame
** CpMsgSetIdentifier(&myMessage, 100);         // identifier is 100 (decimal)
** CpMsgSetDlc(&myMessage, 2);                  // Data Length Code (DLC) is 2
** CpMsgSetData(&myMessage, 0, 0x11);           // byte 0 has the value 0x11
** CpMsgSetData(&myMessage, 1, 0x22);           // byte 1 has the value 0x22
** //... do something with it ....
**
** //--- evaluate whether a CAN frame was received ------------------
** CpCanMsg_ts   receiveMsg;
** //... receive the stuff ....
**
** if(CpMsgIsExtended(&receiveMsg))
** {
**    //--- this is an extended frame ---------------------
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


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include "canpie.h"

//-------------------------------------------------------------------//
// take precautions if compiled with C++ compiler                    //
#ifdef __cplusplus                                                   //
extern "C" {                                                         //
#endif                                                               //
//-------------------------------------------------------------------//



/*--------------------------------------------------------------------------------------------------------------------*\
** Function prototypes                                                                                                **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
**
** This function sets the identifier field and the flags field of a CAN frame structure to 0. It is 
** recommended to use this function when a CAN frame structure is assigned in memory.
**
** \deprecated It is recommended to use CpMsgInit() for new applications.
**
*/
void  CpMsgClear(CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \see        CpMsgSetOverrun()
**
** This function clears the Overrun bit.
*/
void  CpMsgClrOverrun(CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \see        CpMsgSetRemote()
**
** This function clears the Remote Transmission bit (RTR).
*/
void  CpMsgClrRemote(CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ubDlcV      Data Length Code
** \return     Number of bytes in CAN payload
** \see        CpMsgSizeToDlc()
**
** This function converts the DLC value to the corresponding size of the CAN frame payload. 
*/
uint8_t CpMsgDlcToSize(const uint8_t ubDlcV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \param[in]  ubPosV         Zero-based index of byte position
** \return     Value of data at position ubPosV
**
** This function retrieves the data of a CAN frame. The parameter \c ubPosV must be within the range 
** 0 .. 7 for Classic CAN frames. For ISO CAN FD frames the valid range is 0 .. 63. Please note that the
** macro implementation does not check whether the parameter \c ubPosV lies within the valid range.
*/
uint8_t  CpMsgGetData(const CpCanMsg_ts *ptsCanMsgV, uint8_t ubPosV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \return     Data Length Code (DLC) of CAN frame
**
** This function retrieves the Data Length Code (DLC) of a CAN frame. The return value range is 
** 0 .. 8 for Classic CAN frames and 0 .. 15 for ISO CAN FD frames.
*/
uint8_t  CpMsgGetDlc(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \return     Extended Identifier value
** \see        CpMsgGetStdId()
**
** This function retrieves the identifier value of an extended CAN frame (CAN 2.0B). The frame 
** format can be tested using the CpMsgIsExtended() function.
**
** \deprecated It is recommended to use CpMsgGetIdentifier() for new applications.
**
*/
uint32_t  CpMsgGetExtId(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \return     Identifier value
** \see        CpMsgSetIdentifier()
**
** This function retrieves the identifier value of a CAN frame. The frame format 
** can be tested using the CpMsgIsExtended() function.
*/
uint32_t  CpMsgGetIdentifier(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \return     Standard Identifier value
** \see        CpMsgGetExtId()
**
** This macro retrieves the identifier value of a standard frame (CAN 2.0A). The frame format 
** can be tested using the CpMsgIsExtended() function.
**
** \deprecated It is recommended to use CpMsgGetIdentifier() for new applications.
**
*/
uint16_t  CpMsgGetStdId(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \return     Pointer to time-stamp structure
** \see        CpMsgSetTime
**
** This function returns the time-stamp value for a CAN frame.
*/
/*@null@*/ CpTime_ts   *CpMsgGetTime(CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \param[in]  ubFormatV      Frame format
**
** This function sets the identifier field and the DLC field of a CAN frame structure to 0. The
** parameter \c ubFormatV defines the frame format. Possible values are:
** - #CP_MSG_FORMAT_CBFF : Classic base frame format
** - #CP_MSG_FORMAT_CEFF : Classic extended frame format
** - #CP_MSG_FORMAT_FBFF : FD base frame format
** - #CP_MSG_FORMAT_FEFF : FD extended frame format
**
** The contents of the data field and all other optional fields (time-stamp, user, frame marker) are
** not altered.
*/
void      CpMsgInit(CpCanMsg_ts *ptsCanMsgV, uint8_t ubFormatV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \return     \c true if bit-rate switch is set
** \see        CpMsgSetBitrateSwitch()
**
** Returns \c true if the frame is a CAN FD frame and the bit-rate switch (BRS) bit is set.
*/
bool_t    CpMsgIsBitrateSwitchSet(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \return  \c true for Extended CAN frame
**
** Returns \c true for an extended frame format; otherwise \c false (base frame format).
*/
bool_t   CpMsgIsExtended(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \return     \c true for CAN FD frame
**
** Returns \c true if the frame type is CAN FD.
*/
bool_t   CpMsgIsFdFrame(const CpCanMsg_ts *ptsCanMsgV);

//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \return  1 for data overrun, otherwise 0
**
** This function checks whether a data overrun has occurred for the CAN frame.
*/
bool_t   CpMsgIsOverrun(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \return     \c false for data frame, \c true for remote frame
**
** This function checks whether the Remote Transmission (RTR) bit is set. If the RTR bit is set, the 
** function returns \c true; otherwise \c false.
*/
bool_t   CpMsgIsRemote(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \return     0 for CAN frame, 1 for Remote Procedure Call
**
** This function checks whether the Remote Procedure Call bit (RPC) is set. If the RPC bit is set, the 
** function returns 1; otherwise 0.
*/
bool_t   CpMsgIsRpc(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \brief      Get CAN controller mode via RPC
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** 
** \return     Mode selection
**
** This function returns the operating mode of the CAN controller, provided via a Remote 
** Procedure Call. Possible return values are defined in the #CpMode_e enumeration.
*/
uint8_t CpMsgRpcGetCanMode(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \brief      Get data bit-timing selection via RPC
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** 
** \return     Data bit rate
**
** This function returns the data bit-timing selection of the CAN controller, provided via 
** a Remote Procedure Call. Possible return values are defined in the #CpBitrate_e enumeration.
*/
int32_t CpMsgRpcGetDataBitrate(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \internal
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** 
** \return     Nominal bit rate
**
** This function returns the nominal bit-timing selection of the CAN controller,
** provided via a Remote Procedure Call.
** Possible return values are defined in the #CpBitrate_e enumeration.
*/
int32_t CpMsgRpcGetNominalBitrate(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \internal
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \param[in]  ubModeV        Mode selection
**
** This function changes the operating mode of the CAN controller FSA.
** Possible values for the parameter \c ubModeV are defined in the
** #CpMode_e enumeration.
*/
void     CpMsgRpcSetCanMode(CpCanMsg_ts *ptsCanMsgV, uint8_t ubModeV);


//------------------------------------------------------------------------------------------------------
/*!
** \internal
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \param[in]  slNomBitRateV  Nominal bit-timing selection
** \param[in]  slDatBitRateV  Data bit-timing selection
**
** This function initialises the bit-timing registers of a CAN controller
** to predefined values. The values are defined by the enumeration
** #CpBitrate_e. 
*/
void     CpMsgRpcSetBitrate(CpCanMsg_ts *ptsCanMsgV, int32_t slNomBitRateV, int32_t slDatBitRateV);

//------------------------------------------------------------------------------------------------------
/*!
** \internal
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \return     Value from #CpRpc_e enumeration
**
** This function returns a value from the #CpRpc_e enumeration
** identifying the RPC type.
*/
uint32_t  CpMsgRpcType(const CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \see        CpMsgIsBitrateSwitch()
**
** Sets the bit-rate switch (BRS) bit if the CAN frame is a CAN FD frame; otherwise the frame control field is not altered.
*/
void     CpMsgSetBitrateSwitch(CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \param[in]  ubPosV         Zero-based index of byte position
** \param[in]  ubValueV       Value of data byte in CAN frame
**
** This function sets the data in a CAN frame. The parameter \c ubPosV must be within the range 0 .. 7
** for Classic CAN frames. For ISO CAN FD frames the valid range is 0 .. 63. Please note that the
** macro implementation does not check whether the parameter \c ubPosV lies within the valid range.
*/
void  CpMsgSetData(CpCanMsg_ts *ptsCanMsgV, uint8_t ubPosV, uint8_t ubValueV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \param[in]  ubDlcV         Data Length Code
**
** This function sets the Data Length Code (DLC) of a CAN frame. The parameter \c ubDlcV must be within
** the range 0 .. 8 for Classic CAN frames and the range 0 .. 15 for ISO CAN FD frames.
**
** Please note that the macro implementation does not check the value range of the parameter \c ubDlcV.
*/
void  CpMsgSetDlc(CpCanMsg_ts *ptsCanMsgV, uint8_t ubDlcV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \param[in]  ulExtIdV       Identifier value
** \see        CpMsgSetStdId()
**
** This function sets the identifier value for an extended frame (CAN 2.0B).
**
** \deprecated It is recommended to use CpMsgSetIdentifier() for new applications.
*/
void  CpMsgSetExtId(CpCanMsg_ts *ptsCanMsgV, uint32_t ulExtIdV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \param[in]  ulIdentifierV  Identifier value
** \see        CpMsgGetIdentifier()
**
** This function sets the identifier value for a CAN frame. The parameter \c ulIdentifierV is truncated
** to an 11-bit value (AND operation with #CP_MASK_STD_FRAME) when the CAN frame is initialised as base
** frame format. The parameter \c ulIdentifierV is truncated to a 29-bit value (AND operation with 
** #CP_MASK_EXT_FRAME) when the CAN frame is initialised as extended frame format.
*/
void  CpMsgSetIdentifier(CpCanMsg_ts *ptsCanMsgV, uint32_t ulIdentifierV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \see        CpMsgClrOverrun()
**
** This function sets the Overrun bit.
*/
void  CpMsgSetOverrun(CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \see        CpMsgClrRemote()
**
** This function sets the Remote Transmission bit (RTR).
*/
void  CpMsgSetRemote(CpCanMsg_ts *ptsCanMsgV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \param[in]  uwStdIdV       Identifier value
** \see        CpMsgSetExtFrame()
**
** This function sets the identifier value for a standard frame (CAN 2.0A).
**
** \deprecated It is recommended to use CpMsgSetIdentifier() for new applications.
*/
void  CpMsgSetStdId(CpCanMsg_ts *ptsCanMsgV, uint16_t uwStdIdV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ptsCanMsgV     Pointer to CAN frame structure
** \param[in]  ptsTimeV       Pointer to time-stamp structure
**
** This function sets the time-stamp value for a CAN frame.
*/
void  CpMsgSetTime(CpCanMsg_ts *ptsCanMsgV, const CpTime_ts *ptsTimeV);


//------------------------------------------------------------------------------------------------------
/*!
** \param[in]  ubSizeV        Size of CAN message payload in bytes
** \return     DLC value
** \see        CpMsgDlcToSize()
**
** This function converts the size of the CAN frame payload into a DLC value. 
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
