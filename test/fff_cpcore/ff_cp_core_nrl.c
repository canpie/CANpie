//====================================================================================================================//
// File:          ff_mc_cp_core_nrl.c                                                                                 //
// Description:   Fake Functions CP CORE for NRL implementation                                                       //
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



/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 
#include "ff_cp_core_nrl.h"  // function that should be faked

/*--------------------------------------------------------------------------------------------------------------------*\
** Faked test functions                                                                                               **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBitrate_Nrl, CpPort_ts *, int32_t, int32_t)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferConfig_Nrl, CpPort_ts *, uint8_t, uint32_t, uint32_t, uint8_t, uint8_t)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferEnable_Nrl, CpPort_ts *, uint8_t, uint8_t)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferGetData_Nrl, CpPort_ts *, uint8_t, uint8_t *, uint8_t, uint8_t)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferGetDlc_Nrl, CpPort_ts *, uint8_t, uint8_t *)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferRelease_Nrl, CpPort_ts *, uint8_t)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferSend_Nrl, CpPort_ts *, uint8_t)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferSetData_Nrl, CpPort_ts *, uint8_t, uint8_t *, uint8_t, uint8_t)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreBufferSetDlc_Nrl, CpPort_ts *, uint8_t, uint8_t)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreCanMode_Nrl, CpPort_ts *, uint8_t)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreCanState_Nrl, CpPort_ts *, CpState_ts *)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreDriverInit_Nrl, uint8_t, CpPort_ts *, uint8_t)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreDriverRelease_Nrl, CpPort_ts *)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreFifoConfig_Nrl, CpPort_ts *, uint8_t, CpFifo_ts *)

DEFINE_FAKE_VOID_FUNC(              CpCoreFifoEvent_Nrl, CpPort_ts *, uint8_t)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreFifoRead_Nrl, CpPort_ts *, uint8_t, CpCanMsg_ts *, uint32_t *)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreFifoRelease_Nrl, CpPort_ts *, uint8_t)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreFifoWrite_Nrl, CpPort_ts *, uint8_t, CpCanMsg_ts *, uint32_t *)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreHDI_Nrl, CpPort_ts *, CpHdi_ts *)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreIntFunctions_Nrl, CpPort_ts *, CpRcvHandler_Fn, CpTrmHandler_Fn, CpErrHandler_Fn)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreStatistic_Nrl, CpPort_ts *, CpStatistic_ts *)

DEFINE_FAKE_VALUE_FUNC(CpStatus_tv, CpCoreStatisticClear_Nrl, CpPort_ts *)
