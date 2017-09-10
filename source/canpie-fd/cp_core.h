//============================================================================//
// File:          cp_core.h                                                   //
// Description:   CANpie core functions                                       //
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


#ifndef  CP_CORE_H_
#define  CP_CORE_H_



//-----------------------------------------------------------------------------
/*!
** \file    cp_core.h
** \brief   CANpie core functions
**
** The core functions provide the direct interface to the CAN controller
** (hardware). Please note that due to hardware limitations maybe certain
** functions are not implemented on the target platform. A call to an
** unsupported function will always return the error code
** #eCP_ERR_NOT_SUPPORTED.
** <p>
** For a "FullCAN" controller (i.e. a CAN controller that has several message
** buffers) an extended set of powerful functions (CpCoreBuffer..())is provided.
** <p>
** <h3>Initialisation Process</h3>
** <p>
** The CAN driver is initialised with the function CpCoreDriverInit(). This
** routine will setup the CAN controller, but not configure a certain bitrate
** nor switch the CAN controller to active operation. The following core
** functions must be called in that order:
** \li CpCoreDriverInit()
** \li CpCoreBitrate()
** \li CpCoreCanMode()
**
** The function CpCoreDriverInit() must be called before any other core
** function in order to have a valid handle / pointer to the open CAN interface.
**
** \b Example
** \code
** void MyCanInit(void)
** {
**    CpPort_ts tsCanPortT; // logical CAN port
**    //---------------------------------------------------
**    // setup the CAN controller / open a physical CAN
**    // port
**    //
**    CpCoreDriverInit(eCP_CHANNEL_1, &tsCanPortT);
**    //---------------------------------------------------
**    // setup 500 kBit/s
**    //
**    CpCoreBitrate(&tsCanPortT, eCP_BITRATE_500K, eCP_BITRATE_NONE);
**    //---------------------------------------------------
**    // start CAN operation
**    //
**    CpCoreCanMode(&tsCanPortT, eCP_MODE_START);
**    //.. now we are operational
** }
** \endcode
*/


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "cp_fifo.h"

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


/*-------------------------------------------------------------------------
** A driver with only one channel and small memory resources does not need
** the 'channel' parameter.
** The definition CP_SMALL_CODE is checked for the value '1' and the
** function prototypes are converted then. The function call in the
** application stays the same (with 'channel' parameter).
**
*/
#if   CP_SMALL_CODE == 1
#define  CpCoreBitrate(CH, A, B)                CpCoreBitrate(A, B)

#define  CpCoreBufferGetData(CH, A, B, C, D)    CpCoreBufferGetData(A, B, C, D)
#define  CpCoreBufferGetDlc(CH, A, B)           CpCoreBufferGetDlc(A, B)
#define  CpCoreBufferConfig(CH, A, B, C, D, E)  CpCoreBufferConfig(A, B, C, D, E)
#define  CpCoreBufferRelease(CH, A)             CpCoreBufferRelease(A)
#define  CpCoreBufferSetData(CH, A, B, C, D)    CpCoreBufferSetData(A, B, C, D)
#define  CpCoreBufferSetDlc(CH, A, B)           CpCoreBufferSetDlc(A, B)
#define  CpCoreBufferSend(CH, A)                CpCoreBufferSend(A)
#define  CpCoreCanMode(CH, A)                   CpCoreCanMode(A)
#define  CpCoreCanState(CH, A)                  CpCoreCanState(A)
#define  CpCoreFifoConfig(CH, A, B)             CpCoreFifoConfig(A, B)
#define  CpCoreFifoRead(CH, A, B, C)            CpCoreFifoRead(A, B, C)
#define  CpCoreFifoRelease(CH, A)               CpCoreFifoRelease(A)
#define  CpCoreFifoWrite(CH, A, B, C)           CpCoreFifoWrite(A, B, C)
#define  CpCoreHDI(CH, A)                       CpCoreHDI(A)

#define  CpCoreIntFunctions(CH, A, B, C)        CpCoreIntFunctions(A, B, C)

#define  CpCoreStatistic(CH, A)                 CpCoreStatistic(A)

#endif


typedef uint8_t (* CpRcvHandler_Fn)(CpCanMsg_ts *ptsMsgV, uint8_t ubBufferV);
typedef uint8_t (* CpTrmHandler_Fn)(CpCanMsg_ts *ptsMsgV, uint8_t ubBufferV);
typedef uint8_t (* CpErrHandler_Fn)(CpState_ts   *ptsErrV);



/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/



/*!
** \brief      Set bitrate of CAN controller
** \param[in]  ptsPortV       Pointer to CAN port structure
** \param[in]  slNomBitRateV  Nominal Bit Timing selection
** \param[in]  slDatBitRateV  Data Bit Timing selection
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** This function initializes the bit timing registers of a CAN controller
** to pre-defined values. The values are defined by the enumeration
** #CpBitrate_e. For a classical CAN controller (or if bit-rate switching
** is not required) the parameter \c slDatBitRateV is set to #eCP_BITRATE_NONE.
*/
CpStatus_tv CpCoreBitrate(CpPort_ts *ptsPortV, int32_t slNomBitRateV,
                          int32_t slDatBitRateV);

/*!
** \brief      Initialise message buffer
** \param[in]  ptsPortV          Pointer to CAN port structure
** \param[in]  ubBufferIdxV      Buffer number
** \param[in]  ulIdentifierV     Identifier value
** \param[in]  ulAcceptMaskV     Acceptance mask value
** \param[in]  ubFormatV         Message format
** \param[in]  ubDirectionV      Message direction
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** \see     CpCoreBufferRelease()
**
** This function allocates a message buffer in a CAN controller.
** The number of the message buffer inside the CAN controller is
** denoted via the parameter \c ubBufferIdxV. The first buffer starts
** at position eCP_BUFFER_1. The message buffer is allocated with the
** identifier value \c ulIdentifierV. If the buffer is used for
** reception (parameter \c ubDirectionV is eCP_BUFFER_DIR_RCV), the
** parameter \c ulAcceptMaskV is used for acceptance filtering.
**
**
** The parameter \c ubFormatV can have the following values:
** \li #CP_MSG_FORMAT_CBFF : Classic CAN frame, Standard Identifier
** \li #CP_MSG_FORMAT_CEFF : Classic CAN frame, Extended Identifier
** \li #CP_MSG_FORMAT_FBFF : ISO CAN FD frame, Standard Identifier
** \li #CP_MSG_FORMAT_FEFF : ISO CAN FD frame, Extended Identifier
**
** The following example shows the setup of a transmit buffer
** \dontinclude demo_buffer_config.c
** \skip    void DemoTransmitBufferConfiguration
** \until   }
**
** An allocated transmit buffer can be sent via the function
** CpCoreBufferSend().
*/
CpStatus_tv CpCoreBufferConfig(CpPort_ts *ptsPortV,
                               uint8_t   ubBufferIdxV,
                               uint32_t  ulIdentifierV,
                               uint32_t  ulAcceptMaskV,
                               uint8_t   ubFormatV,
                               uint8_t   ubDirectionV);

CpStatus_tv CpCoreBufferEnable(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV,
                               uint8_t ubEnableV);

/*!
** \brief      Get data from message buffer
** \param[in]  ptsPortV       Pointer to CAN port structure
** \param[in]  ubBufferIdxV   Buffer number
** \param[out] pubDestDataV   Pointer to destination data buffer
** \param[in]  ubStartPosV    Array start position
** \param[in]  ubSizeV        Number of bytes to read
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** The functions copies \c ubSizeV data bytes from the CAN message buffer
** defined by \c ubBufferIdxV. The first message buffer starts at the index
** #eCP_BUFFER_1. The parameter \c ubStartPosV denotes the start position,
** the first data byte is at position 0. The destination buffer (pointer
** \c pubDestDataV) must have sufficient space for the data. The buffer has
** to be configured by CpCoreBufferConfig() in advance.
**
*/
CpStatus_tv CpCoreBufferGetData(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV,
                                uint8_t *pubDestDataV,
                                uint8_t   ubStartPosV,
                                uint8_t   ubSizeV);


/*!
** \brief      Get DLC of specified buffer
** \param[in]  ptsPortV       Pointer to CAN port structure
** \param[in]  ubBufferIdxV   Buffer number
** \param[out] pubDlcV        Data Length Code
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** This function retrieves the Data Length Code (DLC) of the selected buffer
** \c ubBufferIdxV. The first message buffer starts at the index #eCP_BUFFER_1.
** The parameter \c pubDlcV is a pointer to a memory location where the
** function will store the DLC value on success. The buffer has to be
** configured by CpCoreBufferConfig() in advance.
*/
CpStatus_tv CpCoreBufferGetDlc(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV,
                               uint8_t *pubDlcV);



/*!
** \brief      Release message buffer
** \param[in]  ptsPortV       Pointer to CAN port structure
** \param[in]  ubBufferIdxV   Buffer number
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** \see     CpCoreBufferConfig()
**
** The function releases the allocated message buffer specified by the
** parameter \c ubBufferIdxV. The first message buffer starts at the index
** #eCP_BUFFER_1. Both - reception and transmission - will be disabled on
** the specified message buffer afterwards.
*/
CpStatus_tv CpCoreBufferRelease(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV);


/*!
** \brief      Send message from message buffer
** \param[in]  ptsPortV       Pointer to CAN port structure
** \param[in]  ubBufferIdxV   Index of message buffer
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** This function transmits a message from the specified message buffer
** \c ubBufferIdxV. The first message buffer starts at
** the index #eCP_BUFFER_1. The message buffer has to be configured as
** transmit buffer (#eCP_BUFFER_DIR_TRM) by a call to CpCoreBufferConfig()
** in advance. A transmission request on a receive buffer will fail with
** the return code #eCP_ERR_INIT_FAIL.
**
*/
CpStatus_tv CpCoreBufferSend(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV);


/*!
** \brief      Set data of message buffer
** \param[in]  ptsPortV       Pointer to CAN port structure
** \param[in]  ubBufferIdxV   Buffer number
** \param[in]  pubSrcDataV    Pointer to source data buffer
** \param[in]  ubStartPosV    Array start position
** \param[in]  ubSizeV        Number of bytes to write
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** This function copies \c ubSizeV data bytes from the source buffer (pointer
** \a pubSrcDataV) into the message buffer defined by the parameter
** \c ubBufferIdxV. The first message buffer starts at the index
** #eCP_BUFFER_1. The parameter \c ubStartPosV denotes the start position,
** the first data byte is at position 0. The message buffer has to be
** configured by CpCoreBufferConfig() in advance.
**
** The following example demonstrates the access to the data bytes of a CAN
** message:
** \code
**  uint8_t aubDataT[8];   // buffer for 8 bytes
**
** aubDataT[0] = 0x11;  // byte 0: set to 11hex
** aubDataT[1] = 0x22;  // byte 1: set to 22hex
**
** //--- copy the stuff to message buffer 1 ---------------
** CpCoreBufferSetData(ptsCanPortS, eCP_BUFFER_1, &aubDataT, 0, 2);
**
** //--- send this message out ----------------------------
** CpCoreBufferSend(ptsCanPortS, eCP_BUFFER_1);
** \endcode
**
*/
CpStatus_tv CpCoreBufferSetData(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV,
                                uint8_t *pubSrcDataV,
                                uint8_t   ubStartPosV,
                                uint8_t   ubSizeV);


/*!
** \brief      Set DLC of specified buffer
** \param[in]  ptsPortV       Pointer to CAN port structure
** \param[in]  ubBufferIdxV   Buffer number
** \param[in]  ubDlcV         Data Length Code
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** This function sets the Data Length Code (DLC) of the specified message
** buffer \c ubBufferIdxV. The DLC value \c ubDlcV must be in the range
** from 0 to 8 for Classical CAN frames and 0 to 15 for ISO CAN FD frames.
** An invalid DLC value is rejected with the return value #eCP_ERR_CAN_DLC.
** The message buffer has to be configured by a call to CpCoreBufferConfig()
** in advance.
*/
CpStatus_tv CpCoreBufferSetDlc(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV,
                               uint8_t ubDlcV);


/*!
** \brief      Set state of CAN controller
** \param[in]  ptsPortV       Pointer to CAN port structure
** \param[in]  ubModeV        Mode selection
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** This function changes the operating mode of the CAN controller FSA.
** Possible values for the parameter \c ubModeV are defined in the
** #CpMode_e enumeration. At least the modes #eCP_MODE_INIT and
** #eCP_MODE_OPERATION shall be supported. Other modes depend on the
** capabilities of the CAN controller.
** \image html can_mode_statechart.png "CAN controller FSA"
**
*/
CpStatus_tv CpCoreCanMode(CpPort_ts *ptsPortV, uint8_t ubModeV);


/*!
** \brief      Retrieve state of CAN controller
** \param[in]  ptsPortV       Pointer to CAN port structure
** \param[out] ptsStateV      Pointer to CAN state structure
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** This function retrieves the present state of the CAN controller. The
** parameter \c ptsStateV is a pointer to a memory location where the
** function will store the state. The value of the structure element
** CpState_ts::ubCanErrState is defined by the #CpState_e enumeration.
** The value of the structure element CpState_ts::ubCanErrType is defined
** by the #CpErrType_e enumeration.
*/
CpStatus_tv CpCoreCanState(CpPort_ts *ptsPortV, CpState_ts *ptsStateV);



//-------------------------------------------------------------------
// When the option CP_SMALL_CODE is set, the following function
// has no parameters. Inside the header file it must have the
// parameter type *void* then. The function is re-defined after-
// wards!
//
#if   CP_SMALL_CODE == 1
#define  CpCoreDriverInit(A, CH, B)             CpCoreDriverInit(B)
#endif
/*!
** \brief      Initialise the CAN driver
** \param[in]  ubPhyIfV     CAN channel of the hardware
** \param[out] ptsPortV     Pointer to CAN port structure
** \param[in]  ubConfigV    This parameter is reserved for future enhancement
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** \see     CpCoreDriverRelease()
**
** The functions opens the physical CAN interface defined by the
** parameter \c ubPhyIfV. The value for \c ubPhyIfV is taken from
** the enumeration #CpChannel_e. The function sets up the field
** members of the CAN port structure CpPort_ts. The parameter \c ptsPortV
** is a pointer to a memory location where structure CpPort_ts is stored.
** An opened CAN port must be closed via the CpCoreDriverRelease()
** function.
*/
CpStatus_tv CpCoreDriverInit(uint8_t ubPhyIfV, CpPort_ts *ptsPortV,
                             uint8_t ubConfigV);

//-------------------------------------------------------------------
// Re-define the function for proper compilation.
//
#if   CP_SMALL_CODE == 1
#undef   CpCoreDriverInit
#define  CpCoreDriverInit(A, CH, B)          CpCoreDriverInit(B)
#endif


//-------------------------------------------------------------------
// When the option CP_SMALL_CODE is set, the following function
// has no parameters. Inside the header file it must have the
// parameter type *void* then. The function is re-defined after-
// wards!
//
#if   CP_SMALL_CODE == 1
#define  CpCoreDriverRelease(CH)             CpCoreDriverRelease(void)
#endif
/*!
** \brief      Release the CAN driver
** \param[in]  ptsPortV       Pointer to CAN port structure
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** \see     CpCoreDriverInit()
**
** The function closes a CAN port. The parameter \c ptsPortV is a pointer
** to a memory location where structure CpPort_ts is stored.
** The implementation of this function is dependent on the operating
** system. Typical tasks might be:
** <ul>
** <li>clear the interrupt vector / routine
** <li>close all open paths to the hardware
** </ul>
**
*/
CpStatus_tv CpCoreDriverRelease(CpPort_ts *ptsPortV);
//-------------------------------------------------------------------
// Re-define the function for proper compilation.
//
#if   CP_SMALL_CODE == 1
#undef   CpCoreDriverRelease
#define  CpCoreDriverRelease(CH)             CpCoreDriverRelease()
#endif


/*!
** \brief      Assign FIFO to message buffer
** \param[in]  ptsPortV       Pointer to CAN port structure
** \param[in]  ubBufferIdxV   Buffer number
** \param[in]  ptsFifoV       Pointer to FIFO structure
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** This function assigns a FIFO to a message buffer defined by the parameter
** \c ubBufferIdxV. The first message buffer starts at the index #eCP_BUFFER_1.
** The buffer has to be configured by CpCoreBufferConfig() in advance.
** The parameter \c ptsFifoV is a pointer to a memory location where a
** FIFO has been initialized using the CpFifoInit() function.
** <p>
** The following example shows the configuration of a receive FIFO
** \dontinclude demo_buffer_fifo.c
** \skip    #define  FIFO_RCV_SIZE  32
** \until   }
*/
CpStatus_tv CpCoreFifoConfig(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV,
                             CpFifo_ts *ptsFifoV);

void        CpCoreFifoEvent(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV);

/*!
** \brief         Read a CAN message from FIFO
** \param[in]     ptsPortV       Pointer to CAN port structure
** \param[in]     ubBufferIdxV   Buffer number
** \param[out]    ptsCanMsgV     Pointer to a CAN message structure
** \param[in,out] pulMsgCntV     Pointer to message count variable
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** This function reads CAN messages from a receive FIFO defined by the
** parameter \c ubBufferIdxV. The first message buffer starts at the index
** #eCP_BUFFER_1. The FIFO has to be configured by CpCoreFifoConfig() in
** advance. The parameter \c ptsCanMsgV is a pointer to the application
** buffer as array of \c CpCanMsg_ts objects to store the received CAN
** messages. The parameter \c pulMsgCntV is a pointer to a memory location
** which has to be initialized before the call to the size of the buffer
** referenced by \c ptsCanMsgV as multiple of \c CpCanMsg_ts objects. Upon
** return, the driver has stored the number of messages copied into the
** application buffer into this parameter.
** <p>
** The following example shows a read operation from a receive FIFO
** \dontinclude demo_buffer_fifo.c
** \skip    void DemoFifoRead
** \until   }
*/
CpStatus_tv CpCoreFifoRead(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV,
                           CpCanMsg_ts *ptsCanMsgV,
                           uint32_t *pulMsgCntV);

/*!
** \brief      Release FIFO from message buffer
** \param[in]  ptsPortV       Pointer to CAN port structure
** \param[in]  ubBufferIdxV   Buffer number
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** This function releases an assigned FIFO from a message buffer
** defined by the parameter \c ubBufferIdxV. The first message buffer starts
** at the index #eCP_BUFFER_1. The FIFO has to be configured by
** CpCoreFifoConfig() in advance.
*/
CpStatus_tv CpCoreFifoRelease(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV);

/*!
** \brief         Transmit a CAN message
** \param[in]     ptsPortV       Pointer to CAN port structure
** \param[in]     ubBufferIdxV   Buffer number
** \param[in]     ptsCanMsgV     Pointer to a CAN message structure
** \param[in,out] pulMsgCntV     Pointer to message count variable
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** This function writes CAN messages to a transmit FIFO defined by the
** parameter \c ubBufferIdxV. The first message buffer starts at the index
** #eCP_BUFFER_1. The FIFO has to be configured by CpCoreFifoConfig() in
** advance. The parameter \c ptsCanMsgV is a pointer to the application
** buffer as array of \c CpCanMsg_ts objects which contain the CAN messages
** that should be transmitted. The parameter \c pulMsgCntV is a pointer to
** a memory location which has to be initialized before the call to the
** size of the buffer referenced by \c ptsCanMsgV as multiple of
** \c CpCanMsg_ts objects. Upon return, the driver has stored the number of
** messages transmitted successfully into this parameter.
*/
CpStatus_tv CpCoreFifoWrite(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV,
                            CpCanMsg_ts *ptsCanMsgV,
                            uint32_t *pulMsgCntV);

/*!
** \brief      Get hardware description information
** \param[in]  ptsPortV       Pointer to CAN port structure
** \param[out] ptsHdiV        Pointer to the Hardware Description Interface
**                            structure (CpHdi_ts)
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** This function retrieves information about the CAN interface. The
** parameter \c ptsHdiV is a pointer to a memory location where the
** function will store the information.
**
*/
CpStatus_tv CpCoreHDI(CpPort_ts *ptsPortV, CpHdi_ts *ptsHdiV);


/*!
** \brief      Install callback functions
** \param[in]  ptsPortV        Pointer to CAN port structure
** \param[in]  pfnRcvHandlerV  Pointer to callback function for receive handler
** \param[in]  pfnTrmHandlerV  Pointer to callback function for transmit handler
** \param[in]  pfnErrHandlerV  Pointer to callback function for error handler
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** This function will install three different callback routines in the
** interrupt handler. If you do not want to install a callback for a
** certain interrupt condition the parameter must be set to NULL.
** The callback functions for receive and transmit interrupt have the
** following syntax:
** <code>
** uint8_t Handler(CpCanMsg_ts * ptsCanMsgV, uint8_t ubBufferIdxV)
** </code>
** <p>
** The callback function for the CAN status-change / error interrupt has
** the following syntax:
** <code>
** uint8_t Handler(uint8_t ubStateV)
** </code>
** <p>
*/
CpStatus_tv CpCoreIntFunctions(CpPort_ts *ptsPortV,
                               /*@null@*/ CpRcvHandler_Fn pfnRcvHandlerV,
                               /*@null@*/ CpTrmHandler_Fn pfnTrmHandlerV,
                               /*@null@*/ CpErrHandler_Fn pfnErrHandlerV);


/*!
** \brief      Read CAN controller statistics
** \param[in]  ptsPortV       Pointer to CAN port structure
** \param[in]  ptsStatsV      Pointer to statistic data structure
**
** \return  Error code is defined by the #CpErr_e enumeration. If no error
**          occurred, the function will return the value \c #eCP_ERR_NONE.
**
** This function copies CAN statistic information to the structure
** pointed by \c ptsStatsV.
**
*/
CpStatus_tv CpCoreStatistic(CpPort_ts *ptsPortV, CpStatistic_ts *ptsStatsV);


//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//

#endif   /* CP_CORE_H_ */
