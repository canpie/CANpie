//====================================================================================================================//
// File:          ff_cp_core.h                                                                                        //
// Description:   Fake Functions CP CORE implementation                                                               //
//                                                                                                                    //
//                                                                                                                    //
// Copyright (C) MicroControl GmbH & Co. KG                                                                           //
// 53844 Troisdorf - Germany                                                                                          //
// www.microcontrol.net                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
// The copyright to the computer program(s) herein is the property of MicroControl GmbH & Co. KG, Germany. The        //
// program(s) may be used and/or copied only with the written permission of MicroControl GmbH & Co. KG or in          //
// accordance with the terms and conditions stipulated in the agreement/contract under which the program(s) have      //
// been supplied.                                                                                                     //
//                                                                                                                    //
//====================================================================================================================//


#ifndef  FF_CP_CORE_H_
#define  FF_CP_CORE_H_


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 
#include "fff_mc.h"  // include Fake Function Framework (fff)
#include "cp_core.h"  // function that should be faked


/*--------------------------------------------------------------------------------------------------------------------*\
** Faked test functions                                                                                               **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 
DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBitrate,CpPort_ts *, int32_t, int32_t )

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferConfig,CpPort_ts *, uint8_t, uint32_t, uint32_t, uint8_t, uint8_t)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferEnable,CpPort_ts *, uint8_t, uint8_t)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferGetData,CpPort_ts *, uint8_t, uint8_t *, uint8_t, uint8_t)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferGetDlc,CpPort_ts *, uint8_t, uint8_t *)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferRelease,CpPort_ts *, uint8_t )

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferSend,CpPort_ts *, uint8_t )

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferSetData,CpPort_ts *, uint8_t, uint8_t *, uint8_t, uint8_t)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferSetDlc,CpPort_ts *, uint8_t, uint8_t)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreCanMode,CpPort_ts *, uint8_t)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreCanState,CpPort_ts *, CpState_ts *)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreDriverInit,uint8_t , CpPort_ts *, uint8_t)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreDriverRelease,CpPort_ts *)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreFifoConfig,CpPort_ts *, uint8_t, CpFifo_ts *)

DECLARE_FAKE_VOID_FUNC(              CpCoreFifoEvent,CpPort_ts *, uint8_t)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreFifoRead,CpPort_ts *, uint8_t, CpCanMsg_ts *, uint32_t *)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreFifoRelease,CpPort_ts *, uint8_t)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreFifoWrite,CpPort_ts *, uint8_t, CpCanMsg_ts *, uint32_t *)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreHDI,CpPort_ts *, CpHdi_ts *)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreIntFunctions,CpPort_ts *, CpRcvHandler_Fn, CpTrmHandler_Fn, CpErrHandler_Fn)

DECLARE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreStatistic,CpPort_ts *, CpStatistic_ts *)

#endif   // FF_MC_CP_CORE_H_


