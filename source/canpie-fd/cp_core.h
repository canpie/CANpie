//============================================================================//
// File:          cp_core.h                                                   //
// Description:   CANpie core functions                                       //
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


typedef uint8_t (* CpRcvHandler_Fn)(CpCanMsg_ts * ptsMsgV, uint8_t ubBufferV);
typedef uint8_t (* CpTrmHandler_Fn)(CpCanMsg_ts * ptsMsgV, uint8_t ubBufferV);
typedef uint8_t (* CpErrHandler_Fn)(CpState_ts *  ptsErrV);



/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/



/*!
** \brief   Set bitrate of CAN controller
** \param   ptsPortV       Pointer to CAN port structure
** \param   slNomBitRateV  Nominal Bit Timing selection
** \param   slDatBitRateV  Data Bit Timing selection
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
*/
CpStatus_tv CpCoreBitrate( const CpPort_ts * ptsPortV, int32_t slNomBitRateV,
                           int32_t slDatBitRateV);

/*!
** \brief   Initialise message buffer
** \param   ptsPortV          Pointer to CAN port structure
** \param   ubBufferIdxV      Buffer number
** \param   ulIdentifierV     Identifier value
** \param   ulAcceptMaskV     Acceptance mask value
** \param   ubFormatV         Message format
** \param   ubDirectionV      Message direction
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
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
CpStatus_tv CpCoreBufferConfig( const CpPort_ts * ptsPortV,
                                uint8_t   ubBufferIdxV,
                                uint32_t  ulIdentifierV,
                                uint32_t  ulAcceptMaskV,
                                uint8_t   ubFormatV,
                                uint8_t   ubDirectionV);

CpStatus_tv CpCoreBufferEnable(const CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                uint8_t ubEnableV);

/*!
** \brief   Get data from message buffer
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
** \param   pubDestDataV   Buffer for data
** \param   ubStartPosV    Array start position
** \param   ubSizeV        Number of bytes to read
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** The function reads \a ubSizeV byte from a CAN message buffer, starting
** at position \a ubStartPosV. The first CAN data byte is located at postion
** 0. The data is copied into the buffer \a pubDestDataV.
**
*/
CpStatus_tv CpCoreBufferGetData(const CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubDestDataV,
                                 uint8_t   ubStartPosV,
                                 uint8_t   ubSizeV);


/*!
** \brief   Get DLC of specified buffer
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
** \param   pubDlcV        Data Length Code
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** This function retrieves the Data Length Code (DLC) of the selected buffer
** \c ubBufferIdxV.
*/
CpStatus_tv CpCoreBufferGetDlc(const  CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubDlcV);



/*!
** \brief   Release message buffer of FullCAN controller
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** \see     CpCoreBufferInit()
*/
CpStatus_tv CpCoreBufferRelease(const CpPort_ts * ptsPortV, uint8_t ubBufferIdxV);


/*!
** \brief   Send message from message buffer
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Index of message buffer
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** This function transmits a message from the specified message buffer
** \c ubBufferIdxV. The message buffer has to be configured by a call to
** CpCoreBufferInit() in advance. The first message buffer starts at
** the index #eCP_BUFFER_1.
**
*/
CpStatus_tv CpCoreBufferSend(const CpPort_ts * ptsPortV, uint8_t ubBufferIdxV);


/*!
** \brief   Set data of message buffer
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
** \param   pubSrcDataV    Pointer to data buffer
** \param   ubStartPosV    Array start position
** \param   ubSizeV        Number of bytes to write
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** This function is the fastest method to set the data bytes of a CAN message.
** It can be used in combination with the function CpCoreBufferSend(). It
** will write 8 data bytes into the buffer defined by \e ubBufferIdxV. The
** buffer has to be configured by CpCoreBufferInit() in advance. The size
** of the data buffer \e pubSrcDataV must have a size of 8 bytes.
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
** CpCoreBufferSetData(ptsCanPortS, eCP_BUFFER_1, &aubDataT);
**
** //--- send this message out ----------------------------
** CpCoreBufferSend(ptsCanPortS, eCP_BUFFER_1);
** \endcode
**
*/
CpStatus_tv CpCoreBufferSetData( const CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubSrcDataV,
                                 uint8_t   ubStartPosV,
                                 uint8_t   ubSizeV);


/*!
** \brief   Set DLC of specified buffer
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
** \param   ubDlcV         Data Length Code
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** This function sets the Data Length Code (DLC) of the selected buffer
** ubBufferIdxV. The DLC must be in the range from 0 to 8.
*/
CpStatus_tv CpCoreBufferSetDlc(const CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                               uint8_t ubDlcV);



/*!
** \brief   Set state of CAN controller
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubModeV        Mode selection
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** This function changes the operating mode of the CAN controller.
** Possible values for mode are defined in the #CpMode_e enumeration.
*/
CpStatus_tv CpCoreCanMode(const CpPort_ts * ptsPortV, uint8_t ubModeV);


/*!
** \brief   Retrieve status of CAN controller
** \param   ptsPortV       Pointer to CAN port structure
** \param   ptsStateV      Pointer to CAN state structure
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** This function retrieved the present state of the CAN controller. Possible
** values are defined in the #CpState_e enumeration. The state of the CAN
** controller is copied to the variable pointer 'ptsStateV'.
*/
CpStatus_tv CpCoreCanState(const CpPort_ts * ptsPortV, CpState_ts * ptsStateV);



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
** \brief   Initialise the CAN driver
** \param   ubPhyIfV     CAN channel of the hardware
** \param   ptsPortV     Pointer to CAN port structure
** \param   ubConfigV    This parameter is reserved for future enhancement
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** \see     CpCoreDriverRelease()
**
** The functions opens the physical CAN interface defined by the
** parameter \c ubPhyIfV. The value for \c ubPhyIfV is taken from
** the enumeration CP_CHANNEL. The function sets up the field
** members of the CAN port handle \c ptsPortV. On success, the
** function returns eCP_ERR_NONE. On failure, the function can return
** the following values:
** <ul>
** <li>#eCP_ERR_HARDWARE
** <li>#eCP_ERR_INIT_FAIL
** </ul>
** An opened handle to a CAN port must be closed via the CpCoreDriverRelease()
** function.
*/
CpStatus_tv CpCoreDriverInit(uint8_t ubPhyIfV, CpPort_ts * ptsPortV,
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
** \brief   Release the CAN driver
** \param   ptsPortV       Pointer to CAN port structure
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** \see     CpCoreDriverInit()
**
** The implementation of this function is dependent on the operating
** system. Typical tasks might be:
** <ul>
** <li>clear the interrupt vector / routine
** <li>close all open paths to the hardware
** </ul>
**
*/
CpStatus_tv CpCoreDriverRelease(CpPort_ts * ptsPortV);
//-------------------------------------------------------------------
// Re-define the function for proper compilation.
//
#if   CP_SMALL_CODE == 1
#undef   CpCoreDriverRelease
#define  CpCoreDriverRelease(CH)             CpCoreDriverRelease()
#endif


/*!
** \brief   Assign FIFO to message buffer
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
** \param   ptsFifoV       Pointer to FIFO structure
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** This function assigns a FIFO to a message buffer with the given index
** \a ubBufferIdxV.
*/
CpStatus_tv CpCoreFifoConfig(const CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                             CpFifo_ts * ptsFifoV);

void        CpCoreFifoEvent(const CpPort_ts * ptsPortV, uint8_t ubBufferIdxV);

/*!
** \brief   Read a CAN message from FIFO
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
** \param   ptsCanMsgV     Pointer to a CAN message structure
** \param   pulBufferSizeV Pointer to size variable
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** This function reads the receive queue from a CAN controller.
*/
CpStatus_tv CpCoreFifoRead(const CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                           CpCanMsg_ts * ptsCanMsgV,
                           uint32_t * pulBufferSizeV);

/*!
** \brief   Release FIFO from message buffer
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** This function reads the receive queue from a CAN controller.
*/
CpStatus_tv CpCoreFifoRelease(const CpPort_ts * ptsPortV, uint8_t ubBufferIdxV);

/*!
** \brief   Transmit a CAN message
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
** \param   ptsCanMsgV     Pointer to a CAN message structure
** \param   pulBufferSizeV Pointer to size variable
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** This function writes to the transmit queue of a CAN controller.
*/
CpStatus_tv CpCoreFifoWrite(const CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                            CpCanMsg_ts * ptsCanMsgV,
                            uint32_t * pulBufferSizeV);

/*!
** \brief   Get hardware description information
** \param   ptsPortV       Pointer to CAN port structure
** \param   ptsHdiV        Pointer to the Hardware Description Interface
**                         structure (CpHdi_ts)
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
**
** This function retrieves information about the used hardware.
**
*/
CpStatus_tv CpCoreHDI(const CpPort_ts * ptsPortV, CpHdi_ts * ptsHdiV);


/*!
** \brief   Install callback functions
** \param   ptsPortV       Pointer to CAN port structure
** \param   pfnRcvHandlerV  Pointer to callback function for receive interrupt
** \param   pfnTrmHandlerV  Pointer to callback function for transmit interrupt
** \param   pfnErrHandlerV  Pointer to callback function for error interrupt
**
** \return  Error code taken from the #CpErr_e enumeration. If no error
**          occurred, the function will return \c eCP_ERR_NONE.
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
CpStatus_tv CpCoreIntFunctions(const CpPort_ts * ptsPortV,
               /*@null@*/ CpRcvHandler_Fn pfnRcvHandlerV,
               /*@null@*/ CpTrmHandler_Fn pfnTrmHandlerV,
               /*@null@*/ CpErrHandler_Fn pfnErrHandlerV);






/*!
** \brief   Read CAN controller statistics
** \param   ptsPortV       Pointer to CAN port structure
** \param   ptsStatsV      Pointer to statistic data structure
**
** \return  Error code taken from the CpErr_e enumeration. Possible
**          return values are:
**          <ul>
**          <li>eCP_ERR_CHANNEL - Channel number is out of range
**          <li>eCP_ERR_NONE - Function returned without error condition
**          </ul>
**
** This function copies CAN statistic information to the structure
** pointed by ptsStatsV.
**
*/
CpStatus_tv CpCoreStatistic(const CpPort_ts * ptsPortV, CpStatistic_ts * ptsStatsV);


//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//

#endif   /* CP_CORE_H_ */
