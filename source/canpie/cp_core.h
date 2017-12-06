//****************************************************************************//
// File:          cp_core.h                                                   //
// Description:   CANpie core functions                                       //
// Author:        Uwe Koppe                                                   //
// e-mail:        koppe@microcontrol.net                                      //
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
// 29.07.2003  Initial version                                                //
//                                                                            //
//****************************************************************************//


#ifndef  CP_CORE_H_
#define  CP_CORE_H_


//-----------------------------------------------------------------------------
// SVN  $Date: 2016-03-24 16:38:41 +0100 (Thu, 24. Mar 2016) $
// SVN  $Rev: 7445 $ --- $Author: tiderko $
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/*!
** \file    cp_core.h
** \brief   CANpie core functions
**
** The core functions provide the direct interface to the CAN controller
** (hardware). Please note that due to hardware limitations maybe certain
** functions are not implemented on the target platform. A call to an
** unsupported function will always return the error code
** #CpErr_NOT_SUPPORTED.
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
** \li CpCoreBaudrate() / CpCoreBittiming()
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
**    CpCoreDriverInit(CP_CHANNEL_1, &tsCanPortT);
**    //---------------------------------------------------
**    // setup 500 kBit/s
**    //
**    CpCoreBaudrate(&tsCanPortT, CP_BAUD_500K);
**    //---------------------------------------------------
**    // start CAN operation
**    //
**    CpCoreCanMode(&tsCanPortT, CP_MODE_START);
**    //.. now we are operational
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


/*-------------------------------------------------------------------------
** A driver with only one channel and small memory resources does not need
** the 'channel' parameter.
** The definition CP_SMALL_CODE is checked for the value '1' and the
** function prototypes are converted then. The function call in the
** application stays the same (with 'channel' parameter).
**
*/
#if   CP_SMALL_CODE == 1
#define  CpCoreAccMask(CH, A)                CpCoreAccMask(A)
#define  CpCoreAutobaud(CH, A, B)            CpCoreAutobaud(A, B)
#define  CpCoreBaudrate(CH, A)               CpCoreBaudrate(A)
#define  CpCoreBittiming(CH, A)              CpCoreBittiming(A)

#define  CpCoreBufferAccMask(CH, A, B)       CpCoreBufferAccMask(A, B)
#define  CpCoreBufferGetData(CH, A, B)       CpCoreBufferGetData(A, B)
#define  CpCoreBufferGetDlc(CH, A, B)        CpCoreBufferGetDlc(A, B)
#define  CpCoreBufferInit(CH, A, B, C)       CpCoreBufferInit(A, B, C)
#define  CpCoreBufferRelease(CH, A)          CpCoreBufferRelease(A)
#define  CpCoreBufferSetData(CH, A, B)       CpCoreBufferSetData(A, B)
#define  CpCoreBufferSetDlc(CH, A, B)        CpCoreBufferSetDlc(A, B)
#define  CpCoreBufferSend(CH, A)             CpCoreBufferSend(A)
#define  CpCoreBufferTransmit(CH, A, B)      CpCoreBufferTransmit(A, B)
#define  CpCoreCanMode(CH, A)                CpCoreCanMode(A)
#define  CpCoreCanState(CH, A)               CpCoreCanState(A)

#define  CpCoreFilterAll(CH, A)              CpCoreFilterAll(A)
#define  CpCoreFilterMsg(CH, A, B)           CpCoreFilterMsg(A, B)

#define  CpCoreHDI(CH, A)                    CpCoreHDI(A)

#define  CpCoreIntFunctions(CH, A, B, C)     CpCoreIntFunctions(A, B, C)

#define  CpCoreMsgRead(CH, A, B)             CpCoreMsgRead(A, B)
#define  CpCoreMsgWrite(CH, A, B)            CpCoreMsgWrite(A, B)

#define  CpCoreStatistic(CH, A)              CpCoreStatistic(A)

#endif


/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/


CpStatus_tv CpCoreAutobaud(CpPort_ts * ptsPortV, uint8_t * pubBaudSelV,
                           uint16_t * puwWaitV);


/*!
** \brief   Set bitrate of CAN controller
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBaudSelV     Bitrate selection
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function directly writes to the bit timing registers of the CAN
** controller. The value for the parameter \e ubBaudSelV is taken
** from the CP_BAUD enumeration.
**
*/
CpStatus_tv CpCoreBaudrate(CpPort_ts * ptsPortV, uint8_t ubBaudSelV);


/*!
** \brief   Set bitrate of CAN controller
** \param   ptsPortV       Pointer to CAN port structure
** \param   ptsBitrateV    Pointer to bit timing structure
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function directly writes to the bit timing registers of the CAN
** controller. Usage of the function requires a detailed knowledge of
** the used CAN controller hardware.
**
*/
CpStatus_tv CpCoreBittiming(CpPort_ts * ptsPortV, CpBitTiming_ts * ptsBitrateV);


/*!
** \brief   Set buffer acceptance mask
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
** \param   ulAccMaskV     Acceptance mask
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function sets the acceptance mask \a ulAccMaskV for the message
** buffer with index \a ubBufferIdxV. An acceptance mask value of
** 0x000007FF configures the message buffer to receive all possible
** CAN standard frames.
**
*/
CpStatus_tv CpCoreBufferAccMask( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint32_t ulAccMaskV);

/*!
** \brief   Get data from message buffer
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
** \param   pubDataV       Buffer for data
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function is the fastest method to get data from a FullCAN message
** buffer. The buffer has to be configured by a call to CpCoreBufferInit()
** before.
**
*/
CpStatus_tv CpCoreBufferGetData( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubDataV);


/*!
** \brief   Get DLC of specified buffer
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
** \param   pubDlcV        Data Length Code
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function retrieves the Data Length Code (DLC) of the selected buffer
** \c ubBufferIdxV.
*/
CpStatus_tv CpCoreBufferGetDlc(  CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubDlcV);



/*!
** \brief  	Check if the specified buffer is busy
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
**
** \return  Error code taken from the #CpErr enumeration. If the device is
** 			busy the function will return \c CpErr_BUFFER_BUSY. If not busy and
** 			no other error occurred the function will return \c CpErr_OK.
**
** This function check if the selected \c ubBufferIdxV is busy.
*/
CpStatus_tv CpCoreBufferBusy(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV);



/*!
** \brief   Initialise buffer in FullCAN controller
** \param   ptsPortV       Pointer to CAN port structure
** \param   ptsCanMsgV     Pointer to a CAN message structure
** \param   ubBufferIdxV   Buffer number
** \param   ubDirectionV   Direction of message
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** \see     CpCoreBufferRelease()
**
** This function allocates the message buffer in a FullCAN controller.
** The number of the message buffer inside the FullCAN controller is
** denoted via the parameter \c ubBufferIdxV.
** The parameter \c ubDirectionV can have the following values:
** \li CP_BUFFER_DIR_RX: receive
** \li CP_BUFFER_DIR_TX: transmit
**
** The following example shows the setup of a transmit buffer
** \dontinclude buf_init.c
** \skip    void AllocateTrmBuffer
** \until   }
**
** An allocated transmit buffer can be sent via the function
** CpCoreBufferSend().
*/
CpStatus_tv CpCoreBufferInit( CpPort_ts * ptsPortV, CpCanMsg_ts * ptsCanMsgV,
                              uint8_t ubBufferIdxV, uint8_t ubDirectionV);


/*!
** \brief   Release message buffer of FullCAN controller
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** \see     CpCoreBufferInit()
*/
CpStatus_tv CpCoreBufferRelease( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV);


/*!
** \brief   Send message from message buffer
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Index of message buffer
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function transmits a message from the specified message buffer
** \c ubBufferIdxV. The message buffer has to be configured by a call to
** CpCoreBufferInit() in advance. The first message buffer starts at
** the index CP_BUFFER_1.
**
*/
CpStatus_tv CpCoreBufferSend(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV);


/*!
** \brief   Set data of message buffer
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
** \param   pubDataV       Pointer to data buffer
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function is the fastest method to set the data bytes of a CAN message.
** It can be used in combination with the function CpCoreBufferSend(). It
** will write 8 data bytes into the buffer defined by \e ubBufferIdxV. The
** buffer has to be configured by CpCoreBufferInit() in advance. The size
** of the data buffer \e pubDataV must have a size of 8 bytes.
**
** The following example demonstrates the access to the data bytes of a CAN
** message:
** \code
**  uint8_t aubDataT[8];   // buffer for 8 bytes
**
** aubDataT[0] = 0x11;  // byte 0: set to 11hex
** aubDataT[1] = 0x22;  // byte 1: set to 22hex

** //--- copy the stuff to message buffer 1 ---------------
** CpCoreBufferSetData(CP_CHANNEL_1, CP_BUFFER_1, &aubDataT);
**
** //--- send this message out ----------------------------
** CpCoreBufferSend(CP_CHANNEL_1, CP_BUFFER_1);
** \endcode
**
*/
CpStatus_tv CpCoreBufferSetData( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubDataV);


/*!
** \brief   Set DLC of specified buffer
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubBufferIdxV   Buffer number
** \param   ubDlcV         Data Length Code
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function sets the Data Length Code (DLC) of the selected buffer
** ubBufferIdxV. The DLC must be in the range from 0 to 8.
*/
CpStatus_tv CpCoreBufferSetDlc(  CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t ubDlcV);


/*!
** \brief   Send message out of FullCAN buffer
** \param   ptsPortV       CAN channel of the hardware
** \param   ubBufferIdxV   Buffer number
** \param   ptsCanMsgV     Pointer to a CAN message structure
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occurred, the function will return CpErr_OK.
**
** This function will send a message (ptsCanMsgV) out of a FullCAN buffer
** given by ubBufferIdxV.
*/
CpStatus_tv CpCoreBufferTransmit(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 CpCanMsg_ts * ptsCanMsgV);

/*!
** \brief   Set state of CAN controller
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubModeV        Mode selection
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function changes the operating mode of the CAN controller.
** Possible values for mode are defined in the #CP_MODE enumeration.
*/
CpStatus_tv CpCoreCanMode(CpPort_ts * ptsPortV, uint8_t ubModeV);


/*!
** \brief   Retrieve status of CAN controller
** \param   ptsPortV       Pointer to CAN port structure
** \param   ptsStateV      Pointer to CAN state structure
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function retrieved the present state of the CAN controller. Possible
** values are defined in the #CP_STATE enumeration. The state of the CAN
** controller is copied to the variable pointer 'ptsStateV'.
*/
CpStatus_tv CpCoreCanState(CpPort_ts * ptsPortV, CpState_ts * ptsStateV);



//-------------------------------------------------------------------
// When the option CP_SMALL_CODE is set, the following function
// has no parameters. Inside the header file it must have the
// parameter type *void* then. The function is re-defined after-
// wards!
//
#if   CP_SMALL_CODE == 1
#define  CpCoreDriverInit(A, CH)             CpCoreDriverInit(void)
#endif
/*!
** \brief   Initialise the CAN driver
** \param   ubPhyIfV     CAN channel of the hardware
** \param   ptsPortV     Pointer to CAN port structure
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** \see     CpCoreDriverRelease()
**
** The functions opens the physical CAN interface defined by the
** parameter \c ubPhyIfV. The value for \c ubPhyIfV is taken from
** the enumeration CP_CHANNEL. The function sets up the field
** members of the CAN port handle \c ptsPortV. On success, the
** function returns CpErr_OK. On failure, the function can return
** the following values:
** <ul>
** <li>#CpErr_HARDWARE
** <li>#CpErr_INIT_FAIL
** </ul>
** An opened handle to a CAN port must be closed via the CpCoreDriverRelease()
** function.
*/
CpStatus_tv CpCoreDriverInit(uint8_t ubPhyIfV, CpPort_ts * ptsPortV);
//-------------------------------------------------------------------
// Re-define the function for proper compilation.
//
#if   CP_SMALL_CODE == 1
#undef   CpCoreDriverInit
#define  CpCoreDriverInit(A, CH)             CpCoreDriverInit()
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
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
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
** \brief   Configure selected CAN ID filter for selected CAN port
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubFilterIdxV   Index of Filter, min. value is 1 and max value is
**                         defined by #CP_FILTER_MAX
** \param   ulIdStartV     First CAN identifier to pass
** \param   ulIdEndV       Last CAN identifier to pass
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function configures the CAN Filters, that are typically implemented
** by software!
**
** To add external CAN ID to the filter use #CP_ID_FLAG_EXTENDED
** as OR operation with CAN ID value.
**
** \attention Make sure ulIdStartV is <= ulIdEndV value!
**            If configuration success that the corresponding filter
**            will be enabled directly
**
*/
CpStatus_tv CpCoreFilterConfig(CpPort_ts *ptsPortV, uint8_t ubFilterIdxV, uint32_t ulIdStartV, uint32_t ulIdEndV);

/*!
** \brief   Enable selected CAN ID filter for selected CAN port
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubFilterIdxV   Index of Filter, min. value is 1 and max value is
**                         defined by #CP_FILTER_MAX
** \param   ubEnableV      0 value  disables filter and 1 enables
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function enables selected CAN ID filter
*/
CpStatus_tv CpCoreFilterEnable(CpPort_ts *ptsPortV, uint8_t ubFilterIdxV, uint8_t ubEnableV);

/*!
** \brief   Check the CAN ID pass the filters for selected CAN port or not
** \param   ptsPortV       Pointer to CAN port structure
** \param   ptsCanMsgV     Pointer to the CAN message to check against the filter
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK if CAN ID pass the
**          filter.
**
** \attention This function is used internal by the CAN driver!
*/
CpStatus_tv CpCoreFilterPass(CpPort_ts * ptsPortV, CpCanMsg_ts * ptsCanMsgV);

/*!
** \brief   Release selected CAN ID filter for selected CAN port
** \param   ptsPortV       Pointer to CAN port structure
** \param   ubFilterIdxV   Index of Filter, min. value is 1 and max value is
**                         defined by #CP_FILTER_MAX
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK if CAN ID pass the
**          filter.
**
** This function release selected CAN ID filter.
*/
CpStatus_tv CpCoreFilterRelease(CpPort_ts *ptsPortV, uint8_t ubFilterIdxV);


/*!
** \brief   Get hardware description information
** \param   ptsPortV       Pointer to CAN port structure
** \param   ptsHdiV        Pointer to the Hardware Description Interface
**                         structure (CpHdi_ts)
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function retrieves information about the used hardware.
**
*/
CpStatus_tv CpCoreHDI(CpPort_ts * ptsPortV, CpHdi_ts * ptsHdiV);


/*!
** \brief   Install callback functions
** \param   ptsPortV       Pointer to CAN port structure
** \param   pfnRcvHandler  Pointer to callback function for receive interrupt
** \param   pfnTrmHandler  Pointer to callback function for transmit interrupt
** \param   pfnErrHandler  Pointer to callback function for error interrupt
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
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
CpStatus_tv CpCoreIntFunctions(  CpPort_ts * ptsPortV,
               /*@null@*/ uint8_t (* pfnRcvHandler) (CpCanMsg_ts *, uint8_t),
               /*@null@*/ uint8_t (* pfnTrmHandler) (CpCanMsg_ts *, uint8_t),
               /*@null@*/ uint8_t (* pfnErrHandler) (CpState_ts *)      );


/*!
** \brief   Read a CAN message from controller
** \param   ptsPortV       Pointer to CAN port structure
** \param   ptsBufferV     Pointer to a CAN message structure
** \param   pulBufferSizeV Pointer to size variable
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function reads the receive queue from a CAN controller.
*/
CpStatus_tv CpCoreMsgRead( CpPort_ts * ptsPortV, CpCanMsg_ts * ptsBufferV,
                           uint32_t * pulBufferSizeV);



/*!
** \brief   Transmit a CAN message
** \param   ptsPortV       Pointer to CAN port structure
** \param   ptsBufferV     Pointer to a CAN message structure
** \param   pulBufferSizeV Pointer to size variable
**
** \return  Error code taken from the #CpErr enumeration. If no error
**          occurred, the function will return \c CpErr_OK.
**
** This function writes to the transmit queue of a CAN controller.
*/
CpStatus_tv CpCoreMsgWrite(CpPort_ts * ptsPortV, CpCanMsg_ts * ptsBufferV,
                           uint32_t * pulBufferSizeV);


/*!
** \brief   Read CAN controller statistics
** \param   ptsPortV       Pointer to CAN port structure
** \param   ptsStatsV      Pointer to statistic data structure
**
** \return  Error code taken from the CpErr enumeration. Possible
**          return values are:
**          <ul>
**          <li>CpErr_CHANNEL - Channel number is out of range
**          <li>CpErr_OK - Function returned without error condition
**          </ul>
**
** This function copies CAN statistic information to the structure
** pointed by ptsStatsV.
**
*/
CpStatus_tv CpCoreStatistic(CpPort_ts * ptsPortV, CpStatistic_ts * ptsStatsV);




//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//

#endif   /* CP_CORE_H_ */
