//====================================================================================================================//
// File:          test_cp_nrl.c                                                                                       //
// Description:   Test cases for MCL NRL implementation                                                               //
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
#include "fff_mc.h"  // include Fake Function Framework (fff)

// header files of module for test
#include "cp_msg.h"
#include "cp_nrl.h"

// fake functions required by the module for test
 #include "ff_cp_core_nrl.h"

/*--------------------------------------------------------------------------------------------------------------------*\
** Defines                                                                                                            **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 
DECLARE_FAKE_VOID_FUNC(CpNrlStateHandler, uint8_t)
DEFINE_FAKE_VOID_FUNC(CpNrlStateHandler, uint8_t)

DECLARE_FAKE_VALUE_FUNC(bool_t, CpNrlMboxFlush, CpPort_ts *)
DEFINE_FAKE_VALUE_FUNC(bool_t, CpNrlMboxFlush, CpPort_ts *)

DECLARE_FAKE_VALUE_FUNC(uint8_t, CanRcvHandler, CpCanMsg_ts *, uint8_t)
DEFINE_FAKE_VALUE_FUNC(uint8_t, CanRcvHandler, CpCanMsg_ts *, uint8_t)

DECLARE_FAKE_VALUE_FUNC(uint8_t, CanTrmHandler, CpCanMsg_ts *, uint8_t)
DEFINE_FAKE_VALUE_FUNC(uint8_t, CanTrmHandler, CpCanMsg_ts *, uint8_t)

DECLARE_FAKE_VALUE_FUNC(uint8_t, CanErrHandler, CpState_ts *)
DEFINE_FAKE_VALUE_FUNC(uint8_t, CanErrHandler, CpState_ts *)



/*!
** This is the value that is used for deviation limit while testing the NRL
*/
#define DEVIATION_LIMIT  3

#define RECOVERY_LIMIT_TICKS  20


extern void CpNrlMbox_DriverInit(uint8_t ubPhyIfV);

/*--------------------------------------------------------------------------------------------------------------------*\
** Custom Function implementation of faked function                                                                   **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 
CpPort_ts tsCanPortG;

uint8_t  ubCanBufDlcG;


static CpRcvHandler_Fn apfnRcvHandlerS[2]; 
static CpTrmHandler_Fn apfnTrmHandlerS[2];
static CpErrHandler_Fn apfnErrHandlerS[2];

/*!
 * This buffer is used to fake Rcv ot Trm PDO data
*/
uint8_t  aubCanBufDataG[8];

uint8_t  ubCosMgrNodeAddressG;      // node address


/*!
 * \brief Custom Function implementation of faked function
 */
CpStatus_tv CpCoreBufferGetDlc_cf(CpPort_ts *CPP_PARM_UNUSED(ptsPortV), uint8_t CPP_PARM_UNUSED(ubBufferIdxV),
                                  uint8_t *pubDlcV)
{
   *pubDlcV = ubCanBufDlcG;
   return 0;
}

/*!
 * \brief Custom Function implementation of faked function
 */
CpStatus_tv CpCoreBufferSetDlc_cf(CpPort_ts *CPP_PARM_UNUSED(ptsPortV), uint8_t CPP_PARM_UNUSED(ubBufferIdxV),
                                  uint8_t ubDlcV)
{
   ubCanBufDlcG = ubDlcV;
   return 0;
}

/*!
 * \brief Custom Function implementation of faked function
 */
CpStatus_tv CpCoreBufferGetData_cf(CpPort_ts *CPP_PARM_UNUSED(ptsPortV),
                                   uint8_t   CPP_PARM_UNUSED(ubBufferIdxV),
                                   uint8_t   *pubDestDataV,
                                   uint8_t   ubStartPosV,
                                   uint8_t   ubSizeV)
{
   uint8_t ubCntrT;

   for (ubCntrT = ubStartPosV; ubCntrT < (ubStartPosV+ubSizeV); ubCntrT++)
   {
      *pubDestDataV = aubCanBufDataG[ubCntrT];
      pubDestDataV++;
   }
   return 0;
}

/*!
 * \brief Custom Function implementation of faked function
 */
CpStatus_tv CpCoreBufferSetData_cf(CpPort_ts *CPP_PARM_UNUSED(ptsPortV),
                                   uint8_t   CPP_PARM_UNUSED(ubBufferIdxV),
                                   uint8_t *pubDestDataV,
                                   uint8_t   ubStartPosV,
                                   uint8_t   ubSizeV)
{
   uint8_t ubCntrT;

   for (ubCntrT = ubStartPosV; ubCntrT < (ubStartPosV+ubSizeV); ubCntrT++)
   {
      aubCanBufDataG[ubCntrT] = *pubDestDataV;
      pubDestDataV++;
   }
   return 0;
}


CpStatus_tv CpCoreCanState_Nrl_cf(CpPort_ts * ptsPortV, CpState_ts * ptsStateV)
{
   ptsStateV->ubCanErrState = eCP_STATE_BUS_ACTIVE;
   ptsStateV->ubCanErrType = eCP_ERR_TYPE_NONE;
   ptsStateV->ubCanRcvErrCnt = 0;
   ptsStateV->ubCanTrmErrCnt = 0;

   return eCP_ERR_NONE;
}

CpStatus_tv CpCoreCanState_Nrl_BusOff_cf(CpPort_ts * ptsPortV, CpState_ts * ptsStateV)
{
   ptsStateV->ubCanErrState = eCP_STATE_BUS_OFF;
   ptsStateV->ubCanErrType = eCP_ERR_TYPE_ACK;
   ptsStateV->ubCanRcvErrCnt = 0;
   ptsStateV->ubCanTrmErrCnt = 0;

   return eCP_ERR_NONE;
}


uint8_t CanRcvHandler_cf(CpCanMsg_ts *ptsMsgV, uint8_t ubBufferV)
{
   uint8_t ubReturnT = 0;

   //----------------------------------------------------------------
   // process receive message
   //
   if (ptsMsgV != CPP_NULL)
   {

   }

   return ubReturnT;
}

uint8_t CanTrmHandler_cf(CpCanMsg_ts *ptsMsgV, uint8_t ubBufferV)
{
   uint8_t ubReturnT = 0;

   //----------------------------------------------------------------
   // process transmit message
   //
   if (ptsMsgV != CPP_NULL)
   {

   }

   return ubReturnT;
}

uint8_t CanErrHandler_cf(CpState_ts *ptsErrV)
{
   if (ptsErrV != CPP_NULL)
   {

   }

   return 0;
}

CpStatus_tv CpCoreDriverInit_Nrl_cf(uint8_t ubPhyIfV, CpPort_ts *ptsPortV, uint8_t ubConfigV)
{
   CpStatus_tv tsReturnT = eCP_ERR_NONE;

   if (ubPhyIfV == eCP_CHANNEL_1)
   {
      ptsPortV->ubPhyIf   = eCP_CHANNEL_1;
      ptsPortV->ubDrvInfo = 16;
   }

   else if (ubPhyIfV == eCP_CHANNEL_2)
   {
      ptsPortV->ubPhyIf   = eCP_CHANNEL_2;
      ptsPortV->ubDrvInfo = 17;
   }
   else 
   {
      tsReturnT = eCP_ERR_CHANNEL;
   }

   return tsReturnT;
}

CpStatus_tv CpCoreIntFunctions_Nrl_cf(CpPort_ts *ptsPortV, CpRcvHandler_Fn pfnRcvHandlerV, 
                                                           CpTrmHandler_Fn pfnTrmHandlerV, 
                                                           CpErrHandler_Fn pfnErrHandlerV)
{
   CpStatus_tv tsReturnT = eCP_ERR_NONE;

   if (ptsPortV->ubPhyIf <= eCP_CHANNEL_2)
   {
      apfnRcvHandlerS[ptsPortV->ubPhyIf-1] = pfnRcvHandlerV;
      apfnTrmHandlerS[ptsPortV->ubPhyIf-1] = pfnTrmHandlerV;
      apfnErrHandlerS[ptsPortV->ubPhyIf-1] = pfnErrHandlerV;
   } else
   {
      tsReturnT = eCP_ERR_CHANNEL;
   }

   return tsReturnT;

}


//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 



/*--------------------------------------------------------------------------------------------------------------------*\
** Internal data and function declaration                                                                             **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 
static void AcknowledgeCanRcv(uint8_t ubPhyIfV, CpCanMsg_ts * ptcCanMessageV, uint8_t ubBufferIdxV);
static void AcknowledgeCanTrm(uint8_t ubPhyIfV, CpCanMsg_ts * ptcCanMessageV, uint8_t ubBufferIdxV);

static CpNrlDriver_ts tsNrlDriverS;


//--------------------------------------------------------------------------------------------------------------------//
// Helper function that confirms the successful reception, so NRL can handle its state machine                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
static void AcknowledgeCanRcv(uint8_t ubPhyIfV, CpCanMsg_ts *ptcCanMessageV, uint8_t ubBufferIdxV)
{
   CpNrlMboxTickLoad(ubPhyIfV, ubBufferIdxV+1);
   CpNrlMboxCounterEvent(ubPhyIfV, ubBufferIdxV+1);

   if (apfnRcvHandlerS[ubPhyIfV-1] != 0)
   {
      (* apfnRcvHandlerS[ubPhyIfV-1])(ptcCanMessageV, ubBufferIdxV);
   }
}

//--------------------------------------------------------------------------------------------------------------------//
// Helper function that confirms the successful transmision, so NRL can handle its state machine                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
static void AcknowledgeCanTrm(uint8_t ubPhyIfV, CpCanMsg_ts *ptcCanMessageV, uint8_t ubBufferIdxV)
{
   CpNrlMboxTickLoad(ubPhyIfV, ubBufferIdxV+1);
   CpNrlMboxCounterEvent(ubPhyIfV, ubBufferIdxV+1);

   if (apfnTrmHandlerS[ubPhyIfV-1] != 0)
   {
      (* apfnTrmHandlerS[ubPhyIfV-1])(ptcCanMessageV, ubBufferIdxV);
   }
}

/*--------------------------------------------------------------------------------------------------------------------*\
** Test implementation                                                                                                **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 


//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, Initialisation)
{
   CpStatus_tv tvStatusT;
   /*!
    * Testfall:
    * Initialisierung des NRL auf Korrektheit prüfen
    *
    * Ergebnis:
    * Nach der Initialisierung muss die NRL sich im definierten Zustand befinden.
    *
    * Testablauf:
    * 1) Register driver with combination of INVALID paramters and check the return code
    * 2) register drive with combination of VALID paramters and check the return code
    * 3) Register event handler between NRL and application with INVALID parameters
    * 4) Register service on identifier 0x07A but test before with INVALID parameters
    * 5) Configure deviation limit and test before for handling with INVALID parameters
    * 6) Configure mailbox timeout to 10 ms
    * 7) Check actual state of the NRL
    */


   //--------------------------------------------------------------------------------------------------- 
   // 1) Register driver with combination of INVALID paramters and check the return code
   //
   tvStatusT = CpNrlRegisterDriver(eCP_NRL_CHANNEL_LIMIT + 1, eCP_CHANNEL_1, eCP_CHANNEL_2, &tsNrlDriverS);
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   tvStatusT = CpNrlRegisterDriver(eCP_NRL_CHANNEL_1, eCP_CHANNEL_7, eCP_CHANNEL_2, &tsNrlDriverS);
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   tvStatusT = CpNrlRegisterDriver(eCP_NRL_CHANNEL_1, eCP_CHANNEL_1, eCP_CHANNEL_1, &tsNrlDriverS);
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   tvStatusT = CpNrlRegisterDriver(eCP_NRL_CHANNEL_1, eCP_CHANNEL_2, eCP_CHANNEL_2, &tsNrlDriverS);
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   tvStatusT = CpNrlRegisterDriver(eCP_NRL_CHANNEL_1, eCP_CHANNEL_1, eCP_CHANNEL_2, (CpNrlDriver_ts *) 0);
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   tvStatusT = CpNrlRegisterDriver(eCP_NRL_CHANNEL_1, eCP_CHANNEL_1, eCP_CHANNEL_2, &tsNrlDriverS);
   // ASSERT_EQ(tvStatusT, eCP_ERR_PARAM); // ERROR ?

   //--------------------------------------------------------------------------------------------------- 
   // 2) register drive with combination of VALID paramters and check the return code
   //
   tsNrlDriverS.cp_core_bitrate           = CpCoreBitrate_Nrl;
   tsNrlDriverS.cp_core_buffer_config     = CpCoreBufferConfig_Nrl;
   tsNrlDriverS.cp_core_buffer_get_data   = CpCoreBufferGetData_Nrl;
   tsNrlDriverS.cp_core_buffer_get_dlc    = CpCoreBufferGetDlc_Nrl;
   tsNrlDriverS.cp_core_buffer_release    = CpCoreBufferRelease_Nrl;
   tsNrlDriverS.cp_core_buffer_send       = CpCoreBufferSend_Nrl;
   tsNrlDriverS.cp_core_buffer_set_data   = CpCoreBufferSetData_Nrl;
   tsNrlDriverS.cp_core_buffer_set_dlc    = CpCoreBufferSetDlc_Nrl;
   tsNrlDriverS.cp_core_can_mode          = CpCoreCanMode_Nrl;
   tsNrlDriverS.cp_core_can_state         = CpCoreCanState_Nrl;
   tsNrlDriverS.cp_core_driver_init       = CpCoreDriverInit_Nrl;
   tsNrlDriverS.cp_core_driver_release    = CpCoreDriverRelease_Nrl;
   tsNrlDriverS.cp_core_fifo_config       = CpCoreFifoConfig_Nrl;
   tsNrlDriverS.cp_core_fifo_read         = CpCoreFifoRead_Nrl;
   tsNrlDriverS.cp_core_fifo_release      = CpCoreFifoRelease_Nrl;
   tsNrlDriverS.cp_core_fifo_write        = CpCoreFifoWrite_Nrl;
   tsNrlDriverS.cp_core_hdi               = CpCoreHDI_Nrl;
   tsNrlDriverS.cp_core_int_functions     = CpCoreIntFunctions_Nrl;
   tsNrlDriverS.cp_core_statistic         = CpCoreStatistic_Nrl;
   tsNrlDriverS.cp_core_statistic_clear   = CpCoreStatisticClear_Nrl;

   tvStatusT = CpNrlRegisterDriver(eCP_NRL_CHANNEL_1, eCP_CHANNEL_2, eCP_CHANNEL_1, &tsNrlDriverS);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);

   tvStatusT = CpNrlRegisterDriver(eCP_NRL_CHANNEL_1, eCP_CHANNEL_1, eCP_CHANNEL_2, &tsNrlDriverS);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);

   //----------------------------------------------------------------------------------------------
   // 3) Register event handler between NRL and application with INVALID parameters
   //
   tvStatusT = CpNrlRegisterHandler(eCP_NRL_CHANNEL_LIMIT + 1, (CpNrlStateHandler_Fn)0);
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   tvStatusT = CpNrlRegisterHandler(eCP_NRL_CHANNEL_1, (CpNrlStateHandler_Fn)0);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);

   tvStatusT = CpNrlRegisterHandler(eCP_NRL_CHANNEL_1, CpNrlStateHandler);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);


   //--------------------------------------------------------------------------------------------------- 
   // 4) Register service on identifier 0x07A but test before with INVALID parameters
   //
   tvStatusT = CpNrlRegisterConnectionService(eCP_NRL_CHANNEL_LIMIT + 1, 0x07A, CP_MSG_FORMAT_CBFF);
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   // tvStatusT = CpNrlRegisterService(eCP_NRL_CHANNEL_1, 0x000, CP_MSG_FORMAT_CBFF);
   // ASSERT_EQ(tvStatusT, eCP_ERR_PARAM); // ERROR ?
   
   tvStatusT = CpNrlRegisterConnectionService(eCP_NRL_CHANNEL_1, 0x07A, CP_MSG_FORMAT_CBFF);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);

   // check buffer configuration has been really done for both channel with different identifiers
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.call_count, 2);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg2_history[0], 0x07A);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg2_history[1], 0x07b);
   

   //-----------------------------------------------------------------------------------------
   // 5) Configure deviation limit and test before for handling with INVALID parameters
   //
   tvStatusT = CpNrlSetDeviationLimit(eCP_NRL_CHANNEL_LIMIT + 1, 3);
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   tvStatusT = CpNrlSetDeviationLimit(eCP_NRL_CHANNEL_1, 0);
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   tvStatusT = CpNrlSetDeviationLimit(eCP_NRL_CHANNEL_1, 1);
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   tvStatusT = CpNrlSetDeviationLimit(eCP_NRL_CHANNEL_1, DEVIATION_LIMIT);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);


   //----------------------------------------------------------------------------------------------
   // 6) Configure mailbox timeout to 10 ms
   //
   tvStatusT = CpNrlSetTimeout(eCP_NRL_CHANNEL_LIMIT + 1, 10000);
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   tvStatusT = CpNrlSetTimeout(eCP_NRL_CHANNEL_1, 0);
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   tvStatusT = CpNrlSetTimeout(eCP_NRL_CHANNEL_1, (CP_NRL_TIMER_PERIOD - 1));
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   tvStatusT = CpNrlSetTimeout(eCP_NRL_CHANNEL_1, CP_NRL_TIMER_PERIOD);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);

   tvStatusT = CpNrlSetTimeout(eCP_NRL_CHANNEL_1, 10000);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);

   //----------------------------------------------------------------------------------------------
   // set recovery time for NRL passive state
   //
   tvStatusT = CpNrlSetRecoveryTime(eCP_NRL_CHANNEL_1, 20000);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);

   //----------------------------------------------------------------------------------------------
   // 7) Check actual state of the NRL
   //
   CpNrlState_ts tsNrlStateT;
   tvStatusT = CpNrlState(eCP_NRL_CHANNEL_LIMIT + 1, &tsNrlStateT);
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   tvStatusT = CpNrlState(eCP_NRL_CHANNEL_LIMIT + 1, (CpNrlState_ts *)0);
   ASSERT_EQ(tvStatusT, eCP_ERR_PARAM);

   tvStatusT = CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);

   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_INIT);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);
   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrState, eCP_ERR_NONE);
   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrType, eCP_ERR_TYPE_NONE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrState, eCP_ERR_NONE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrType, eCP_ERR_TYPE_NONE);

   ASSERT_EQ(tsNrlStateT.ubDclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubDclCross, 0);
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubRclCross, 0);
}

  


//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, Startup)
{
   CpStatus_tv tvStatusT;
   uint8_t     aubPayloadT[8];

   /*!
    * Testfall:
    * Starte den CAN, konfiguriere die Buffer und sende jeweils eine CAN Nachricht auf beiden CAN Schnittstellen
    *
    * Ergebnis:
    * Kein NRL event wird ausgelöst und NRL befindet sich im Zustand "Redundancy active"
    *
    * Testablauf:
    * 1) Initialisiere die CAN Schnittstelle(n) und sende eine CAN Nachrichte (2x wegen NRL)
    * 2) Das Senden der beiden CAN Nachrichten für die NRL bestätigen/melden
    * 3) Aktuellen Zustand der NRL auf "Redundancy active" prüfen
    */

   //--------------------------------------------------------------------------------------------------- 
   // 1) Initialisiere die CAN Schnittstelle(n) und sende eine CAN Nachrichte (2x wegen NRL)
   //
   tvStatusT = CpCoreDriverInit(eCP_CHANNEL_1, &tsCanPortG, 0);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);
   ASSERT_EQ(CpCoreDriverInit_Nrl_fake.call_count, 2);
   ASSERT_EQ(CpCoreDriverInit_Nrl_fake.arg0_history[0], eCP_CHANNEL_1);
   ASSERT_EQ(CpCoreDriverInit_Nrl_fake.arg0_history[1], eCP_CHANNEL_2);
   
   ASSERT_EQ(CpCoreIntFunctions_Nrl_fake.call_count, 2);

   // simulate calls done from CpCoreDriverInit: 
   CpNrlMbox_DriverInit(eCP_CHANNEL_1);
   CpNrlMbox_DriverInit(eCP_CHANNEL_2);

   tvStatusT = CpCoreBitrate(&tsCanPortG, eCP_BITRATE_500K, eCP_BITRATE_NONE);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);
   ASSERT_EQ(CpCoreBitrate_Nrl_fake.call_count, 2);
   ASSERT_EQ(CpCoreBitrate_Nrl_fake.arg1_history[0], eCP_BITRATE_500K);
   ASSERT_EQ(CpCoreBitrate_Nrl_fake.arg1_history[1], eCP_BITRATE_500K);

   tvStatusT = CpCoreCanMode(&tsCanPortG, eCP_MODE_OPERATION);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);
   ASSERT_EQ(CpCoreCanMode_Nrl_fake.call_count, 2);
   ASSERT_EQ(CpCoreCanMode_Nrl_fake.arg1_history[0], eCP_MODE_OPERATION);
   ASSERT_EQ(CpCoreCanMode_Nrl_fake.arg1_history[1], eCP_MODE_OPERATION);

   RESET_TEST_SUITE(CP_NRL);

   tvStatusT = CpCoreBufferConfig(&tsCanPortG, eCP_BUFFER_2, 0x321, CP_MASK_STD_FRAME, CP_MSG_FORMAT_CBFF, eCP_BUFFER_DIR_RCV);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.call_count, 2);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg1_history[0], eCP_BUFFER_2+1);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg1_history[1], eCP_BUFFER_2+1);   
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg2_history[0], 0x321);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg2_history[1], 0x321);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg3_history[0], CP_MASK_STD_FRAME);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg3_history[1], CP_MASK_STD_FRAME);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg4_history[0], CP_MSG_FORMAT_CBFF);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg4_history[1], CP_MSG_FORMAT_CBFF);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg5_history[0], eCP_BUFFER_DIR_RCV);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg5_history[1], eCP_BUFFER_DIR_RCV);   


   tvStatusT = CpCoreBufferConfig(&tsCanPortG, eCP_BUFFER_1, 0x123, CP_MASK_STD_FRAME, CP_MSG_FORMAT_CBFF, eCP_BUFFER_DIR_TRM);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.call_count, 4);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg1_history[2], eCP_BUFFER_1+1);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg1_history[3], eCP_BUFFER_1+1);   
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg2_history[2], 0x123);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg2_history[3], 0x123);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg3_history[2], CP_MASK_STD_FRAME);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg3_history[3], CP_MASK_STD_FRAME);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg4_history[2], CP_MSG_FORMAT_CBFF);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg4_history[3], CP_MSG_FORMAT_CBFF);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg5_history[2], eCP_BUFFER_DIR_TRM);
   ASSERT_EQ(CpCoreBufferConfig_Nrl_fake.arg5_history[3], eCP_BUFFER_DIR_TRM);   

   tvStatusT = CpCoreBufferSetDlc(&tsCanPortG, eCP_BUFFER_1, 8);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);
   ASSERT_EQ(CpCoreBufferSetDlc_Nrl_fake.call_count, 2);
   ASSERT_EQ(CpCoreBufferSetDlc_Nrl_fake.arg1_history[0], eCP_BUFFER_1+1);
   ASSERT_EQ(CpCoreBufferSetDlc_Nrl_fake.arg1_history[1], eCP_BUFFER_1+1);     

   aubPayloadT[0] = 0x01;
   aubPayloadT[1] = 0x23;
   aubPayloadT[2] = 0x45;
   aubPayloadT[3] = 0x67;
   aubPayloadT[4] = 0x89;
   aubPayloadT[5] = 0xAB;
   aubPayloadT[6] = 0xCD;
   aubPayloadT[7] = 0xEF;
   tvStatusT = CpCoreBufferSetData(&tsCanPortG, eCP_BUFFER_1, &aubPayloadT[0], 0, 8);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);
   ASSERT_EQ(CpCoreBufferSetData_Nrl_fake.call_count, 2);
   ASSERT_EQ(CpCoreBufferSetData_Nrl_fake.arg1_history[0], eCP_BUFFER_1+1);
   ASSERT_EQ(CpCoreBufferSetData_Nrl_fake.arg1_history[1], eCP_BUFFER_1+1);     

   tvStatusT = CpCoreBufferSend(&tsCanPortG, eCP_BUFFER_1);
   ASSERT_EQ(tvStatusT, eCP_ERR_NONE);
   ASSERT_EQ(CpCoreBufferSend_Nrl_fake.call_count, 2);
   ASSERT_EQ(CpCoreBufferSend_Nrl_fake.arg1_history[0], eCP_BUFFER_1+1);
   ASSERT_EQ(CpCoreBufferSend_Nrl_fake.arg1_history[1], eCP_BUFFER_1+1);     
   
   //-------------------------------------------------------------------------------------------
   // 2) Das Senden der beiden CAN Nachrichten für die NRL bestätigen/melden
   //
   CpNrlMboxTickLoad(eCP_CHANNEL_1, eCP_BUFFER_1+1);
   CpNrlMboxTickLoad(eCP_CHANNEL_2, eCP_BUFFER_1+1);

   CpNrlMboxCounterEvent(eCP_CHANNEL_1, eCP_BUFFER_1+1);
   CpNrlMboxCounterEvent(eCP_CHANNEL_2, eCP_BUFFER_1+1);

   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
}
  

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, StateChange_001__RCL_RedundancyDisabled)
{
   /*!
    * Testfall:
    * Zähler-Abweichung provozieren und auf den NRL Zustand "Redundancy disabled" prüfen
    * Auf CAN Kanal 1 werden Nachrichten ausgesended und über CAN Kanal 2 nicht (Stecker nicht dran).
    *
    * Ergebnis:
    * - NRL befindet sich im Zustand "Redundancy disabled"
    * - Die Aktive Line ist die DCL
    *
    * Testablauf:
    * 1) Sende die Nachricht so oft bis der DEVIATION_LIMIT überschritten ist,
    *    dabei wird der NRL nur das Aussenden auf CAN1 (DLC) bestätigt.
    * 
    * 2) Prüfe den aktuellen NRL Zustand auf "Redundancy disabled"
    */
   RESET_TEST_SUITE(CP_NRL);
   RUN_TEST(CP_NRL, Initialisation);
   CpCoreDriverInit_Nrl_fake.custom_fake = CpCoreDriverInit_Nrl_cf;
   CpCoreIntFunctions_Nrl_fake.custom_fake = CpCoreIntFunctions_Nrl_cf;
   CpCoreCanState_Nrl_fake.custom_fake = CpCoreCanState_Nrl_cf;
   RUN_TEST(CP_NRL, Startup);
   RESET_TEST_SUITE(CP_NRL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Sende die Nachricht 4x (deviation limit wurde in Initialisation() auf DEVIATION_LIMIT gesetzt),
   //    dabei wird der NRL nur das Aussenden auf CAN1 (DLC) bestätigt.  
   //
   int32_t slRunStepsT = 0;
   while (slRunStepsT < DEVIATION_LIMIT)
   {
      CpCoreBufferSend(&tsCanPortG, eCP_BUFFER_1);
      CpNrlMboxTickLoad(eCP_CHANNEL_1, eCP_BUFFER_1+1);
      CpNrlMboxCounterEvent(eCP_CHANNEL_1, eCP_BUFFER_1+1);
      
      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
      
      ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);

      slRunStepsT++;
   }

   //--------------------------------------------------------------------------------------------------- 
   // 1) .... sende die letzte CAN Nachricht bevor der NRL Event auslöst
   //   
   CpCoreBufferSend(&tsCanPortG, eCP_BUFFER_1);
   CpNrlMboxTickLoad(eCP_CHANNEL_1, eCP_BUFFER_1+1);
   CpNrlMboxCounterEvent(eCP_CHANNEL_1, eCP_BUFFER_1+1);
   
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   

   // prüfe dass der NRL Event 1x und mit korrekten Daten aufgerufen wurde
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_DISABLED);


   // Prüfe, dass nach mehrmaligen Event Aufruf der Zustand sich weiterhin nicht verändert hat
   // und der Event Handler auch nicht mehrfach aufgerufen wird.
   slRunStepsT = 1000;
   while (slRunStepsT > 0)
   {
      slRunStepsT--;
      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   }

   // prüfe dass der NRL event nicht aufgerufen wurde, die Anzahl der Aufrufe beim alten Wert geblieben ist.
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);

   //--------------------------------------------------------------------------------------------------- 
   // 2) Prüfe den aktuellen NRL Zustand auf "Redundancy passive"
   //
   CpNrlState_ts tsNrlStateT;
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_DISABLED);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrType,  eCP_ERR_TYPE_NONE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrType,  eCP_ERR_TYPE_NONE);

   ASSERT_EQ(tsNrlStateT.ubDclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubDclCross, 0);
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubRclCross, 0);
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, StateChange_002__RCL_RedundancyPassive)
{
   CpNrlState_ts tsNrlStateT;

   /*!
    * Testfall:
    * Zähler-Abweichung provozieren und auf den NRL Zustand "Redundancy passive" prüfen
    * Auf CAN Kanal 1 werden Nachrichten empfangen und über CAN Kanal 2 nicht (Stecker nicht dran).
    *
    * Ergebnis:
    * - NRL befindet sich im Zustand "Redundancy passive"
    * - Die Aktive Line ist die DCL
    *
    * Testablauf:
    * 1) Empfange die Nachricht so oft bis der DEVIATION_LIMIT überschritten ist,
    *    dabei wird der NRL nur das Empfangen auf CAN1 (DLC) bestätigt.
    * 
    * 2) Prüfe den aktuellen NRL Zustand auf "Redundancy passive"
    */
   RESET_TEST_SUITE(CP_NRL);
   RUN_TEST(CP_NRL, Initialisation);
   CpCoreDriverInit_Nrl_fake.custom_fake   = CpCoreDriverInit_Nrl_cf;
   CpCoreIntFunctions_Nrl_fake.custom_fake = CpCoreIntFunctions_Nrl_cf;
   CpCoreCanState_Nrl_fake.custom_fake     = CpCoreCanState_Nrl_cf;
   RUN_TEST(CP_NRL, Startup);
   RESET_TEST_SUITE(CP_NRL);

   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState,   eCP_NRL_STATE_ACTIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Sende die Nachricht 4x (deviation limit wurde in Initialisation() auf DEVIATION_LIMIT gesetzt),
   //    dabei wird der NRL nur das Aussenden auf CAN1 (DLC) bestätigt.  
   //
   int32_t slRunStepsT = 0;
   while (slRunStepsT < DEVIATION_LIMIT)
   {
      AcknowledgeCanRcv(eCP_CHANNEL_1, 0, eCP_BUFFER_2);

      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

      ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);

      slRunStepsT++;
   }

   //--------------------------------------------------------------------------------------------------- 
   // 1) .... empfange die letzte CAN Nachricht bevor der NRL Event auslöst
   //   
   AcknowledgeCanRcv(eCP_CHANNEL_1, 0, eCP_BUFFER_2);
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   //--------------------------------------------------------------------------------------------------- 
   // check that:
   // - the NRL event is called only once 
   // - the NRL state shall be 'passive'
   //
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_PASSIVE);


   //--------------------------------------------------------------------------------------------------- 
   // Now wait for a time period of 'RECOVERY_LIMIT_TICKS - 1'. After that period, the previous state
   // shall not be changed.
   //
   slRunStepsT = RECOVERY_LIMIT_TICKS - 1;
   while (slRunStepsT > 0)
   {
      slRunStepsT--;
      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   }

   //--------------------------------------------------------------------------------------------------- 
   // check that:
   // - the NRL event is called only once 
   // - the NRL state shall be 'passive'
   //
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_PASSIVE);

   //--------------------------------------------------------------------------------------------------- 
   // After the recovery time has elapsed, the state shall change to 'Redundancy active'
   //
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 2);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[1], eCP_NRL_STATE_ACTIVE);

   //--------------------------------------------------------------------------------------------------- 
   // Check the actual NRL state for 'Redundancy active'
   //
   
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState,   eCP_NRL_STATE_ACTIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrType,  eCP_ERR_TYPE_NONE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrType,  eCP_ERR_TYPE_NONE);

   ASSERT_EQ(tsNrlStateT.ubDclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubDclCross, 0);
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubRclCross, 0);
}


TEST(CP_NRL, StateChange_003__RCL_RedundancyPassiveNoRecover)
{
   CpNrlState_ts tsNrlStateT;

   /*!
    * Testfall:
    * Zähler-Abweichung provozieren und auf den NRL Zustand "Redundancy passive" prüfen
    * Auf CAN Kanal 1 werden Nachrichten empfangen und über CAN Kanal 2 nicht (Stecker nicht dran).
    *
    * Ergebnis:
    * - NRL befindet sich im Zustand "Redundancy passive"
    * - Die Aktive Line ist die DCL
    *
    * Testablauf:
    * 1) Empfange die Nachricht so oft bis der DEVIATION_LIMIT überschritten ist,
    *    dabei wird der NRL nur das Empfangen auf CAN1 (DLC) bestätigt.
    * 
    * 2) Prüfe den aktuellen NRL Zustand auf "Redundancy passive"
    */
   RESET_TEST_SUITE(CP_NRL);
   RUN_TEST(CP_NRL, Initialisation);
   CpCoreDriverInit_Nrl_fake.custom_fake   = CpCoreDriverInit_Nrl_cf;
   CpCoreIntFunctions_Nrl_fake.custom_fake = CpCoreIntFunctions_Nrl_cf;
   CpCoreCanState_Nrl_fake.custom_fake     = CpCoreCanState_Nrl_cf;
   RUN_TEST(CP_NRL, Startup);
   RESET_TEST_SUITE(CP_NRL);

   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState,   eCP_NRL_STATE_ACTIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Sende die Nachricht 4x (deviation limit wurde in Initialisation() auf DEVIATION_LIMIT gesetzt),
   //    dabei wird der NRL nur das Aussenden auf CAN1 (DLC) bestätigt.  
   //
   int32_t slRunStepsT = 0;
   while (slRunStepsT < DEVIATION_LIMIT)
   {
      AcknowledgeCanRcv(eCP_CHANNEL_1, 0, eCP_BUFFER_2);

      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

      ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);

      slRunStepsT++;
   }

   //--------------------------------------------------------------------------------------------------- 
   // 1) .... empfange die letzte CAN Nachricht bevor der NRL Event auslöst
   //   
   AcknowledgeCanRcv(eCP_CHANNEL_1, 0, eCP_BUFFER_2);
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   //--------------------------------------------------------------------------------------------------- 
   // check that:
   // - the NRL event is called only once 
   // - the NRL state shall be 'passive'
   //
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_PASSIVE);


   //--------------------------------------------------------------------------------------------------- 
   // Now wait for a time period of 'RECOVERY_LIMIT_TICKS - 1'. After that period, the previous state
   // shall not be changed. Within this loop, we receibe meesages only on the DCL
   //
   slRunStepsT = RECOVERY_LIMIT_TICKS - 1;
   while (slRunStepsT > 0)
   {
      slRunStepsT--;
      AcknowledgeCanRcv(eCP_CHANNEL_1, 0, eCP_BUFFER_2);

      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   }

   //--------------------------------------------------------------------------------------------------- 
   // check that:
   // - the NRL event is called only once 
   // - the NRL state shall be 'passive'
   //
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_PASSIVE);

   //--------------------------------------------------------------------------------------------------- 
   // After the recovery time has elapsed, the state shall stay in 'Redundancy passive'
   //
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);

   //--------------------------------------------------------------------------------------------------- 
   // 2) Prüfe den aktuellen NRL Zustand auf "Redundancy passive"
   //
   
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState,   eCP_NRL_STATE_PASSIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrType,  eCP_ERR_TYPE_NONE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrType,  eCP_ERR_TYPE_NONE);

   ASSERT_EQ(tsNrlStateT.ubDclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubDclCross, 0);
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubRclCross, 0);
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, StateChange_004__RCL_NetworkDisconnected)
{
   CpNrlState_ts tsNrlStateT;

   /*!
    * Testfall:
    * Bei der Initialisierung sind die beiden CAN Schnittstellen im Bus OFF
    *
    * Ergebnis:
    * - NRL ist direkt im Zustand "Network Disconnected"
    *
    * Testablauf:
    * 1) Bei der Initialisierung wird Bus OFF simuliert
    * 
    * 2) Prüfe den aktuellen auf "Network disconnected"
    */
   RESET_TEST_SUITE(CP_NRL);
   RUN_TEST(CP_NRL, Initialisation);

   CpCoreDriverInit_Nrl_fake.custom_fake = CpCoreDriverInit_Nrl_cf;
   CpCoreIntFunctions_Nrl_fake.custom_fake = CpCoreIntFunctions_Nrl_cf;
   CpCoreCanState_Nrl_fake.custom_fake = CpCoreCanState_Nrl_BusOff_cf;

   RUN_TEST(CP_NRL, Startup);


   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_DISCONNECTED);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_DISCONNECTED);
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, StateChange_005__RCL_RedundancyPassiveToDisable)
{
   CpNrlState_ts tsNrlStateT;

   /*!
    * Testfall:
    * Ausgehend von dem "Redundancy passive" Zustand die     
    * Zähler-Abweichung provozieren und auf den NRL Zustand "Redundancy disabled" prüfen.
    * Auf CAN Kanal 1 werden Nachrichten empfangen und über CAN Kanal 2 nicht (Stecker nicht dran).
    *
    * Ergebnis:
    * - NRL wechselt von "Redundancy passive" Zustand in den "Redundancy disabled"
    * - Die Aktive Line ist die DCL
    *
    * Testablauf:
    * 1) Sende die Nachricht so oft bis der DEVIATION_LIMIT überschritten ist,
    *    dabei wird der NRL nur das Aussenden auf CAN1 (DLC) bestätigt.
    * 
    * 2) Prüfe den aktuellen NRL Zustand auf "Redundancy disabled"
    */
   RESET_TEST_SUITE(CP_NRL);   
   RUN_TEST(CP_NRL, StateChange_003__RCL_RedundancyPassiveNoRecover);
   RESET_TEST_SUITE(CP_NRL);


   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_PASSIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Sende die Nachricht so oft bis der DEVIATION_LIMIT überschritten ist,
   //    dabei wird der NRL nur das Aussenden auf CAN1 (DLC) bestätigt.
   //
   int32_t slRunStepsT = 0;
   while (slRunStepsT < DEVIATION_LIMIT)
   {
      CpCoreBufferSend(&tsCanPortG, eCP_BUFFER_1);
      AcknowledgeCanTrm(eCP_CHANNEL_1, 0, eCP_BUFFER_1);
  
      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
      
      // make sure there is no state change while sending can message vie CAN1 
      CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
      ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);
      ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_PASSIVE);  

      slRunStepsT++;
   }

   //--------------------------------------------------------------------------------------------------- 
   // 1) .... sende die letzte CAN Nachricht bevor der NRL Event auslöst
   //   
   CpCoreBufferSend(&tsCanPortG, eCP_BUFFER_1);
   AcknowledgeCanTrm(eCP_CHANNEL_1, 0, eCP_BUFFER_1);
   
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   
   // prüfe dass der NRL event mit korrekten Daten aufgerufen wurde
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_DISABLED);

   // Prüfe, dass nach mehrmaligen Event Aufruf der Zustand sich weiterhin nicht verändert hat
   // und der Event Handler auch nicht mehrfach aufgerufen wird.
   slRunStepsT = 1000;
   while (slRunStepsT > 0)
   {
      slRunStepsT--;
      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   }

   // prüfe dass der NRL event nicht aufgerufen wurde, die Anzahl der Aufrufe beim alten Wert geblieben ist.
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);


   //--------------------------------------------------------------------------------------------------- 
   // 2) Prüfe den aktuellen NRL Zustand auf "Redundancy passive"
   //
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_DISABLED);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrType,  eCP_ERR_TYPE_NONE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrType,  eCP_ERR_TYPE_NONE);

   ASSERT_EQ(tsNrlStateT.ubDclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubDclCross, 0);
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubRclCross, 0);

}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, StateChange_006__RCL_RedundancyActiveToNetworkDisconnected)
{
   CpNrlState_ts tsNrlStateT;

   /*!
    * Testfall:
    * Ausgehend von dem "Redundancy active" Zustand werden die beiden CAN-Schnittstellen 
    * in den BUS OFF Zustand versetzt, so dass die NRL in den Zustand "Network disconnected" übergeht.
    *
    * Ergebnis:
    * - NRL wechselt von "Redundancy active" Zustand in den "Network disconnected"
    * - Die Aktive Line ist die RCL
    *
    * Testablauf:
    * 1) Test starten und auf "Redundancy active" prüfen
    * 2) BUS OFF simulieren
    * 3) Prüfe den aktuellen NRL Zustand auf "Network disconnected"
    *    - Dabei wurde der Event Handler 2x aufgerufen und der Wechsel verlief über den Zustand "Redundancy Disabled"
    */
   RESET_TEST_SUITE(CP_NRL);
   RUN_TEST(CP_NRL, Initialisation);

   CpCoreDriverInit_Nrl_fake.custom_fake = CpCoreDriverInit_Nrl_cf;
   CpCoreIntFunctions_Nrl_fake.custom_fake = CpCoreIntFunctions_Nrl_cf;
   CpCoreCanState_Nrl_fake.custom_fake = CpCoreCanState_Nrl_cf;

   RUN_TEST(CP_NRL, Startup);
   RESET_TEST_SUITE(CP_NRL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Test starten und auf "Redundancy active" prüfen
   //
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_ACTIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

   //--------------------------------------------------------------------------------------------------- 
   // 2) BUS OFF simulieren
   //
   CpCoreCanState_Nrl_fake.custom_fake = CpCoreCanState_Nrl_BusOff_cf;

   //---------------------------------------------------------------------------------------------------  
   // 3) Prüfe den aktuellen NRL Zustand auf "Network disconnected"
   //    - Dabei wurde der Event Handler 2x aufgerufen und der Wechsel verlief über den Zustand "Redundancy Disabled"
   //
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_DISCONNECTED);

   // aktuellen Zustand holen
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_DISCONNECTED);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, StateChange_007__RCL_RedundancyDisabledToNetworkDisconnected)
{
   CpNrlState_ts tsNrlStateT;

   /*!
    * Testfall:
    * Ausgehend von dem "Redundancy disabled" Zustand werden die beiden CAN-Schnittstellen 
    * in den BUS OFF Zustand versetzt, so dass die NRL in den Zustand "Network disconnected" übergeht.
    *
    * Ergebnis:
    * - NRL wechselt von "Redundancy disabled" Zustand in den "Network disconnected"
    * - Die Aktive Line ist die RCL
    *
    * Testablauf:
    * 1) Test starten und auf "Redundancy disabled" prüfen
    * 2) BUS OFF simulieren
    * 3) Prüfe den aktuellen NRL Zustand auf "Network disconnected"
    *    - Dabei wurde der Event Handler 2x aufgerufen und der Wechsel verlief über den Zustand "Redundancy Disabled"
    */
   RUN_TEST(CP_NRL, StateChange_001__RCL_RedundancyDisabled);
   RESET_TEST_SUITE(CP_NRL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Test starten und auf "Redundancy disabled" prüfen
   //
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_DISABLED);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

   //--------------------------------------------------------------------------------------------------- 
   // 2) BUS OFF simulieren
   //
   CpCoreCanState_Nrl_fake.custom_fake = CpCoreCanState_Nrl_BusOff_cf;

   //---------------------------------------------------------------------------------------------------  
   // 3) Prüfe den aktuellen NRL Zustand auf "Network disconnected"
   //    - Dabei wurde der Event Handler 2x aufgerufen und der Wechsel verlief über den Zustand "Redundancy Disabled"
   //
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_DISCONNECTED);

   // aktuellen Zustand holen
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_DISCONNECTED);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);
}


//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, StateChange_011__DCL_RedundancyDisabled)
{
   /*!
    * Testfall:
    * Zähler-Abweichung provozieren und auf den NRL Zustand "Redundancy disabled" prüfen
    * Auf CAN Kanal 2 werden Nachrichten ausgesended und über CAN Kanal 1 nicht (Stecker nicht dran).
    *
    * Ergebnis:
    * - NRL befindet sich im Zustand "Redundancy disabled"
    * - Die Aktive Line ist die RCL
    *
    * Testablauf:
    * 1) Sende die Nachricht so oft bis der DEVIATION_LIMIT überschritten ist,
    *    dabei wird der NRL nur das Aussenden auf CAN2 (RLC) bestätigt.
    * 
    * 2) Prüfe dass die Umschaltung erfolgte 
    * 
    * 3) Prüfe den aktuellen NRL Zustand auf "Redundancy disabled"
    */
   RESET_TEST_SUITE(CP_NRL);
   RUN_TEST(CP_NRL, Initialisation);
   CpCoreDriverInit_Nrl_fake.custom_fake = CpCoreDriverInit_Nrl_cf;
   CpCoreIntFunctions_Nrl_fake.custom_fake = CpCoreIntFunctions_Nrl_cf;
   CpCoreCanState_Nrl_fake.custom_fake = CpCoreCanState_Nrl_cf;
   RUN_TEST(CP_NRL, Startup);
   RESET_TEST_SUITE(CP_NRL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Sende die Nachricht 4x (deviation limit wurde in Initialisation() auf DEVIATION_LIMIT gesetzt),
   //    dabei wird der NRL nur das Aussenden auf CAN1 (DLC) bestätigt.  
   //
   int32_t slRunStepsT = 0;
   while (slRunStepsT < DEVIATION_LIMIT)
   {
      CpCoreBufferSend(&tsCanPortG, eCP_BUFFER_1);
      CpNrlMboxTickLoad(eCP_CHANNEL_2, eCP_BUFFER_1+1);
      CpNrlMboxCounterEvent(eCP_CHANNEL_2, eCP_BUFFER_1+1);
      
      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
      
      ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);

      slRunStepsT++;
   }

   //--------------------------------------------------------------------------------------------------- 
   // 1) .... sende die letzte CAN Nachricht bevor der NRL Event auslöst
   //   
   CpCoreBufferSend(&tsCanPortG, eCP_BUFFER_1);
   CpNrlMboxTickLoad(eCP_CHANNEL_2, eCP_BUFFER_1+1);
   CpNrlMboxCounterEvent(eCP_CHANNEL_2, eCP_BUFFER_1+1);
   
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   

   // prüfe dass der NRL Event 1x und mit korrekten Daten aufgerufen wurde
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_DISABLED);


   // Prüfe, dass nach mehrmaligen Event Aufruf der Zustand sich weiterhin nicht verändert hat
   // und der Event Handler auch nicht mehrfach aufgerufen wird.
   slRunStepsT = 1000;
   while (slRunStepsT > 0)
   {
      slRunStepsT--;
      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   }

   // prüfe dass der NRL event nicht aufgerufen wurde, die Anzahl der Aufrufe beim alten Wert geblieben ist.
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);

   //--------------------------------------------------------------------------------------------------- 
   // 2) Prüfe den aktuellen NRL Zustand auf "Redundancy passive"
   //
   CpNrlState_ts tsNrlStateT;
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_DISABLED);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_RCL);

   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrType,  eCP_ERR_TYPE_NONE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrType,  eCP_ERR_TYPE_NONE);

   ASSERT_EQ(tsNrlStateT.ubDclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubDclCross, 0);
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubRclCross, 0);
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, StateChange_012__DCL_RedundancyPassive)
{
   CpNrlState_ts tsNrlStateT;

   /*!
    * Testfall:
    * Zähler-Abweichung provozieren und auf den NRL Zustand "Redundancy passive" prüfen
    * Auf CAN Kanal 2 werden Nachrichten empfangen und über CAN Kanal 1 nicht (Stecker nicht dran).
    *
    * Ergebnis:
    * - NRL befindet sich im Zustand "Redundancy passive"
    * - Die Aktive Line ist die RCL
    *
    * Testablauf:
    * 1) Empfange die Nachricht so oft bis der DEVIATION_LIMIT überschritten ist,
    *    dabei wird der NRL nur das Empfangen auf CAN2 (RLC) bestätigt.
    * 
    * 2) Prüfe dass die Umschaltung erfolgte 
    * 
    * 3) Prüfe den aktuellen NRL Zustand auf "Redundancy passive"
    */
   RESET_TEST_SUITE(CP_NRL);
   RUN_TEST(CP_NRL, Initialisation);
   CpCoreDriverInit_Nrl_fake.custom_fake = CpCoreDriverInit_Nrl_cf;
   CpCoreIntFunctions_Nrl_fake.custom_fake = CpCoreIntFunctions_Nrl_cf;
   CpCoreCanState_Nrl_fake.custom_fake = CpCoreCanState_Nrl_cf;
   RUN_TEST(CP_NRL, Startup);
   RESET_TEST_SUITE(CP_NRL);

   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_ACTIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Sende die Nachricht 4x (deviation limit wurde in Initialisation() auf DEVIATION_LIMIT gesetzt),
   //    dabei wird der NRL nur das Aussenden auf CAN1 (DLC) bestätigt.  
   //
   int32_t slRunStepsT = 0;
   while (slRunStepsT < DEVIATION_LIMIT)
   {
      AcknowledgeCanRcv(eCP_CHANNEL_2, 0, eCP_BUFFER_2);

      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

      ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);

      slRunStepsT++;
   }

   //--------------------------------------------------------------------------------------------------- 
   // 1) .... empfange die letzte CAN Nachricht bevor der NRL Event auslöst
   //   
   AcknowledgeCanRcv(eCP_CHANNEL_2, 0, eCP_BUFFER_2);
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   //--------------------------------------------------------------------------------------------------- 
   // check that:
   // - the NRL event is called only once 
   // - the NRL state shall be 'passive'
   //
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_PASSIVE);


   //--------------------------------------------------------------------------------------------------- 
   // Now wait for a time period of 'RECOVERY_LIMIT_TICKS - 1'. After that period, the previous state
   // shall not be changed.
   //
   slRunStepsT = RECOVERY_LIMIT_TICKS - 1;
   while (slRunStepsT > 0)
   {
      slRunStepsT--;
      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   }

   //--------------------------------------------------------------------------------------------------- 
   // check that:
   // - the NRL event is called only once 
   // - the NRL state shall be 'passive'
   //
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_PASSIVE);

   //--------------------------------------------------------------------------------------------------- 
   // After the recovery time has elapsed, the state shall change to 'Redundancy active'
   //
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 2);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[1], eCP_NRL_STATE_ACTIVE);

   //--------------------------------------------------------------------------------------------------- 
   // Check the actual NRL state for 'Redundancy active'
   //
   
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState,   eCP_NRL_STATE_ACTIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_RCL);

   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrType,  eCP_ERR_TYPE_NONE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrType,  eCP_ERR_TYPE_NONE);

   ASSERT_EQ(tsNrlStateT.ubDclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubDclCross, 0);
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubRclCross, 0);
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, StateChange_013__DCL_RedundancyPassiveNoRecover)
{
   CpNrlState_ts tsNrlStateT;

   /*!
    * Testfall:
    * Zähler-Abweichung provozieren und auf den NRL Zustand "Redundancy passive" prüfen
    * Auf CAN Kanal 2 werden Nachrichten empfangen und über CAN Kanal 1 nicht (Stecker nicht dran).
    *
    * Ergebnis:
    * - NRL befindet sich im Zustand "Redundancy passive"
    * - Die Aktive Line ist die RCL
    *
    * Testablauf:
    * 1) Empfange die Nachricht so oft bis der DEVIATION_LIMIT überschritten ist,
    *    dabei wird der NRL nur das Empfangen auf CAN2 (RLC) bestätigt.
    * 
    * 2) Prüfe dass die Umschaltung erfolgte 
    * 
    * 3) Prüfe den aktuellen NRL Zustand auf "Redundancy passive"
    */
   RESET_TEST_SUITE(CP_NRL);
   RUN_TEST(CP_NRL, Initialisation);
   CpCoreDriverInit_Nrl_fake.custom_fake = CpCoreDriverInit_Nrl_cf;
   CpCoreIntFunctions_Nrl_fake.custom_fake = CpCoreIntFunctions_Nrl_cf;
   CpCoreCanState_Nrl_fake.custom_fake = CpCoreCanState_Nrl_cf;
   RUN_TEST(CP_NRL, Startup);
   RESET_TEST_SUITE(CP_NRL);

   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_ACTIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Sende die Nachricht 4x (deviation limit wurde in Initialisation() auf DEVIATION_LIMIT gesetzt),
   //    dabei wird der NRL nur das Aussenden auf CAN1 (DLC) bestätigt.  
   //
   int32_t slRunStepsT = 0;
   while (slRunStepsT < DEVIATION_LIMIT)
   {
      AcknowledgeCanRcv(eCP_CHANNEL_2, 0, eCP_BUFFER_2);

      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

      ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);

      slRunStepsT++;
   }

   //--------------------------------------------------------------------------------------------------- 
   // 1) .... empfange die letzte CAN Nachricht bevor der NRL Event auslöst
   //   
   AcknowledgeCanRcv(eCP_CHANNEL_2, 0, eCP_BUFFER_2);
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   //--------------------------------------------------------------------------------------------------- 
   // check that:
   // - the NRL event is called only once 
   // - the NRL state shall be 'passive'
   //
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_PASSIVE);


   //--------------------------------------------------------------------------------------------------- 
   // Now wait for a time period of 'RECOVERY_LIMIT_TICKS - 1'. After that period, the previous state
   // shall not be changed.
   //
   slRunStepsT = RECOVERY_LIMIT_TICKS - 1;
   while (slRunStepsT > 0)
   {
      slRunStepsT--;
      AcknowledgeCanRcv(eCP_CHANNEL_2, 0, eCP_BUFFER_2);

      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   }

   //--------------------------------------------------------------------------------------------------- 
   // check that:
   // - the NRL event is called only once 
   // - the NRL state shall be 'passive'
   //
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_PASSIVE);

   //--------------------------------------------------------------------------------------------------- 
   // After the recovery time has elapsed, the state shall stay 'Redundancy passive'
   //
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);

   //--------------------------------------------------------------------------------------------------- 
   // Check the actual NRL state for 'Redundancy passive'
   //
   
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState,   eCP_NRL_STATE_PASSIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_RCL);

   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrType,  eCP_ERR_TYPE_NONE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrType,  eCP_ERR_TYPE_NONE);

   ASSERT_EQ(tsNrlStateT.ubDclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubDclCross, 0);
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubRclCross, 0);
}


//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, StateChange_014__DCL_RedundancyPassiveToDisable)
{
   CpNrlState_ts tsNrlStateT;

   /*!
    * Testfall:
    * Ausgehend von dem "Redundancy passive" Zustand die     
    * Zähler-Abweichung provozieren und auf den NRL Zustand "Redundancy disabled" prüfen.
    * Auf CAN Kanal 2 werden Nachrichten empfangen und über CAN Kanal 1 nicht (Stecker nicht dran).
    *
    * Ergebnis:
    * - NRL wechselt von "Redundancy passive" Zustand in den "Redundancy disabled"
    * - Die Aktive Line ist die RCL
    *
    * Testablauf:
    * 1) Sende die Nachricht so oft bis der DEVIATION_LIMIT überschritten ist,
    *    dabei wird der NRL nur das Aussenden auf CAN2 (RLC) bestätigt.
    * 
    * 2) Prüfe den aktuellen NRL Zustand auf "Redundancy disabled"
    */
   RESET_TEST_SUITE(CP_NRL);   
   RUN_TEST(CP_NRL, StateChange_013__DCL_RedundancyPassiveNoRecover);
   RESET_TEST_SUITE(CP_NRL);


   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_PASSIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_RCL);
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Sende die Nachricht so oft bis der DEVIATION_LIMIT überschritten ist,
   //    dabei wird der NRL nur das Aussenden auf CAN1 (DLC) bestätigt.
   //
   int32_t slRunStepsT = 0;
   while (slRunStepsT < DEVIATION_LIMIT)
   {
      CpCoreBufferSend(&tsCanPortG, eCP_BUFFER_1);
      AcknowledgeCanTrm(eCP_NRL_CHANNEL_2, 0, eCP_BUFFER_1);
  
      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
      
      // make sure there is no state change while sending can message vie CAN1 
      CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
      ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);
      ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_PASSIVE);  

      slRunStepsT++;
   }

   //--------------------------------------------------------------------------------------------------- 
   // 1) .... sende die letzte CAN Nachricht bevor der NRL Event auslöst
   //   
   CpCoreBufferSend(&tsCanPortG, eCP_BUFFER_1);
   AcknowledgeCanTrm(eCP_CHANNEL_2, 0, eCP_BUFFER_1);
   
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   
   // prüfe dass der NRL event mit korrekten Daten aufgerufen wurde
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_DISABLED);

   // Prüfe, dass nach mehrmaligen Event Aufruf der Zustand sich weiterhin nicht verändert hat
   // und der Event Handler auch nicht mehrfach aufgerufen wird.
   slRunStepsT = 1000;
   while (slRunStepsT > 0)
   {
      slRunStepsT--;
      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   }

   // prüfe dass der NRL event nicht aufgerufen wurde, die Anzahl der Aufrufe beim alten Wert geblieben ist.
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);


   //--------------------------------------------------------------------------------------------------- 
   // 2) Prüfe den aktuellen NRL Zustand auf "Redundancy passive"
   //
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_DISABLED);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_RCL);

   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrType,  eCP_ERR_TYPE_NONE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrType,  eCP_ERR_TYPE_NONE);

   ASSERT_EQ(tsNrlStateT.ubDclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubDclCross, 0);
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubRclCross, 0);

}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, RecoveryMode_001__RCL_RedundancyDisabledToActive)
{
   CpStatus_tv tvStatusT;
   CpPort_ts *ptsPortRCL;
   CpNrlState_ts tsNrlStateT;

   /*!
    * Testfall:
    * Teste 'recovery mode' von (RCL), hier Kanal 2
    * A) NRL befindet sich im "Redundancy disbaled", so dass RCL keine Nachrichten aussenden kann.
    * B) Nach einer gewissen Zeit wird CpNrlFaultRecovery() für RCL ausgeführt und eine CAN Botschaft über diesen Kanal gesendet
    * C) Es wird ein erfolgreiches Aussenden der CAN Nachricht über RCL simuliert 
    * D) Dies führt dazu dass der 'deviation counter' zurück gesetzt wird 
    * E) Und der Event Handler mit "Redundancy active" aufgerufen werden.
    *
    * Ergebnis:
    * - NRL befindet sich im Zustand "Redundancy active"
    * - Die Aktive Line ist die DCL
    *
    * Testablauf:
    * 1) Prüfe den aktuellen NRL Zustand auf "Redundancy disbaled"
    * 2) Triggere time ticks, so dass der CpNrlFaultRecovery() aufgerufen wird
    * 3) Überprüfe, dass 'Connection Service' über RCL, hier CAN2 gesendet wurde
    * 4) Prüfe, dass der NRL event 1x und mit korrekten Daten aufgerufen wurde
    * 5) Schicke weitere CAN Nachrichten auf beiden Lines und prüfe den aktuellen NRL Zustand auf "Redundancy active"
    */
   RUN_TEST(CP_NRL, StateChange_001__RCL_RedundancyDisabled);
   RESET_TEST_SUITE(CP_NRL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Prüfe den aktuellen NRL Zustand auf "Redundancy disbaled"
   // stelle sicher, dass die NRL sich im Zustand "Redundancy disbaled" befindet
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_DISABLED);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);
   ASSERT_EQ(CpCoreBufferSend_Nrl_fake.call_count, 0);

   //--------------------------------------------------------------------------------------------------- 
   // 2) Triggere time ticks, so dass der CpNrlFaultRecovery() aufgerufen wird
   //

   // Der Recover Mechanismus startet nach CP_NRL_RECOVER_PERIOD timeoutout, simuliere das hier
   int32_t slRunStepsT = 0;
   while (slRunStepsT < 2001)    // Es muss ein tick mehr aufgerufen werden, so dass der time out abläuft
   {
      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
      

      slRunStepsT++;
   }

   //--------------------------------------------------------------------------------------------------- 
   // 3) Überprüfe, dass 'Connection Service' über RCL, hier CAN2 gesendet wurde
   //

   // Check CAN interface has been called
   printf("%d \n", CpCoreCanMode_Nrl_fake.call_count);
   ASSERT_EQ(CpCoreCanMode_Nrl_fake.call_count, 10);
   
   // check the a CpCoreBufferSend_Nrl was triggered to send a CAN message
   ASSERT_EQ(CpCoreBufferSend_Nrl_fake.call_count, 10);
   // check the magic message was send via eCP_BUFFER_1
   ASSERT_EQ(CpCoreBufferSend_Nrl_fake.arg1_history[0], eCP_BUFFER_1);    
   // check the send was performed on RCL, here now the eCP_CHANNEL_2
   ptsPortRCL = CpCoreBufferSend_Nrl_fake.arg0_history[0];
   ASSERT_EQ(ptsPortRCL->ubPhyIf, eCP_CHANNEL_2);    
   
   // 3) ... und nun das Senden der 'Connection Service' Nachricht für die NRL bestätigen/melden
   AcknowledgeCanTrm(eCP_CHANNEL_2, 0, eCP_BUFFER_1);

   // 3) ... trigger event handler 
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   

   //--------------------------------------------------------------------------------------------------- 
   // 4) prüfe, dass der NRL event 1x und mit korrekten Daten aufgerufen wurde
   //
   // prüfe dass der NRL event 1x und mit korrekten Daten aufgerufen wurde
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_ACTIVE);

   //--------------------------------------------------------------------------------------------------- 
   // 5) Schicke weitere CAN Nachrichten auf beiden Lines und prüfe den aktuellen NRL Zustand auf "Redundancy active"
   //

   // sende und bestätige jeweils eine CAN Nachricht
   tvStatusT = CpCoreBufferSend(&tsCanPortG, eCP_BUFFER_1);
   AcknowledgeCanTrm(eCP_CHANNEL_1, 0, eCP_BUFFER_1);
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   AcknowledgeCanTrm(eCP_CHANNEL_2, 0, eCP_BUFFER_1);
   

   // Überprüfe den aktuellen Zustand
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_ACTIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrType, eCP_ERR_TYPE_NONE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrType, eCP_ERR_TYPE_NONE);

   ASSERT_EQ(tsNrlStateT.ubDclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubDclCross, 0);
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubRclCross, 0);
}


//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, RecoveryMode_002__RCL_RedundancyPassiveToActive)
{
   CpStatus_tv tvStatusT;
   CpNrlState_ts tsNrlStateT;
   CpCanMsg_ts tsCanMessageT;

   /*!
    * Testfall:
    * Teste 'recovery mode' von (RCL), hier Kanal 2
    * A) NRL befindet sich im "Redundancy passive", so dass RCL keine Nachrichten empfangen kann.
    * B) Es wird ein erfolgreiches Empfangen der CAN Recovery Nachricht über RCL simuliert 
    * D) Dies führt dazu dass der 'deviation counter' zurück gesetzt wird 
    * E) Und der Event Handler mit "Redundancy active" aufgerufen werden.
    *
    * Ergebnis:
    * - NRL befindet sich im Zustand "Redundancy active"
    * - Die Aktive Line ist die DCL
    *
    * Testablauf:
    * 1) Prüfe den aktuellen NRL Zustand auf "Redundancy passive"
    * 2) Simuliere den 'Connection Service' Empfang über RCL
    * 3) Überprüfe, dass 'Connection Service' über RCL, hier CAN2 empfangen wurde
    * 4) Prüfe, dass der NRL event 1x und mit korrekten Daten aufgerufen wurde
    * 5) Schicke weitere CAN Nachrichten auf beiden Lines und prüfe den aktuellen NRL Zustand auf "Redundancy active"
    */
   RUN_TEST(CP_NRL, StateChange_003__RCL_RedundancyPassiveNoRecover);
   RESET_TEST_SUITE(CP_NRL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Prüfe den aktuellen NRL Zustand auf "Redundancy disbaled"
   // stelle sicher, dass die NRL sich im Zustand "Redundancy disbaled" befindet
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_PASSIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);
   ASSERT_EQ(CpCoreBufferSend_Nrl_fake.call_count, 0);

   //--------------------------------------------------------------------------------------------------- 
   // 2) Simuliere den 'Connection Service' Empfang über RCL, hier via Buffer 1 
   //

   // prepare CAN message 
   CpMsgInit(&tsCanMessageT, CP_MSG_FORMAT_CBFF);
   CpMsgSetIdentifier(&tsCanMessageT, 0x07B);

   // confirm reception
   CpNrlMboxTickLoad(eCP_CHANNEL_2, eCP_BUFFER_1);
   CpNrlMboxCounterEvent(eCP_CHANNEL_2, eCP_BUFFER_1);

   // trigger configured NRL RCV handler and provide "received" connection message
   if (apfnRcvHandlerS[eCP_CHANNEL_2-1] != 0)
   {
      (* apfnRcvHandlerS[eCP_CHANNEL_2-1])(&tsCanMessageT, eCP_BUFFER_1);
   }

   // process NRL once
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   //--------------------------------------------------------------------------------------------------- 
   // 3) Überprüfe, dass 'Connection Service' über RCL, hier CAN2 empfangen wurde
   //
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubDclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubDclCross, 0);
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 1); // Empfang 1er connection message für RCL
   ASSERT_EQ(tsNrlStateT.ubRclCross, 0);

   //--------------------------------------------------------------------------------------------------- 
   // 4) prüfe, dass der NRL event 1x und mit korrekten Daten aufgerufen wurde
   //
   // prüfe dass der NRL event 1x und mit korrekten Daten aufgerufen wurde
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_ACTIVE);

   //--------------------------------------------------------------------------------------------------- 
   // 5) Schicke weitere CAN Nachrichten auf beiden Lines und prüfe den aktuellen NRL Zustand auf "Redundancy active"
   //

   // sende und bestätige jeweils eine CAN Nachricht
   tvStatusT = CpCoreBufferSend(&tsCanPortG, eCP_BUFFER_1);
   AcknowledgeCanTrm(eCP_CHANNEL_1, 0, eCP_BUFFER_1);
   AcknowledgeCanTrm(eCP_CHANNEL_2, 0, eCP_BUFFER_1);
   AcknowledgeCanRcv(eCP_CHANNEL_1, 0, eCP_BUFFER_2);
   AcknowledgeCanRcv(eCP_CHANNEL_2, 0, eCP_BUFFER_2);

   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);


   // Überprüfe den aktuellen Zustand
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_ACTIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrType, eCP_ERR_TYPE_NONE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrType, eCP_ERR_TYPE_NONE);
}


//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, RecoveryMode_002__RCL_RedundancyPassiveToActive_2)
{
   CpStatus_tv tvStatusT;
   CpNrlState_ts tsNrlStateT;

   /*!
    * Testfall:
    * Teste 'recovery mode' von (RCL), hier Kanal 2
    * A) NRL befindet sich im "Redundancy passive", so dass RCL keine Nachrichten empfangen kann.
    * B) Eine Timeout von 2 Sekunden abwarten
    * D) Dies führt dazu dass der 'deviation counter' zurück gesetzt wird 
    * E) Und der Event Handler mit "Redundancy active" aufgerufen werden.
    *
    * Ergebnis:
    * - NRL befindet sich im Zustand "Redundancy active"
    * - Die Aktive Line ist die DCL
    *
    * Testablauf:
    * 1) Prüfe den aktuellen NRL Zustand auf "Redundancy passive"
    * 2) Zeit von 2 Sekunden simulieren
    * 3) Prüfe, dass der NRL event 1x und mit korrekten Daten aufgerufen wurde
    * 2) Schicke weitere CAN Nachrichten auf beiden Lines und prüfe den aktuellen NRL Zustand auf "Redundancy active"
    */
   RUN_TEST(CP_NRL, StateChange_003__RCL_RedundancyPassiveNoRecover);
   RESET_TEST_SUITE(CP_NRL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Prüfe den aktuellen NRL Zustand auf "Redundancy disbaled"
   // stelle sicher, dass die NRL sich im Zustand "Redundancy disbaled" befindet
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_PASSIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);
   ASSERT_EQ(CpCoreBufferSend_Nrl_fake.call_count, 0);

   //--------------------------------------------------------------------------------------------------- 
   // 2) Simuliere den ein delay von 2 Sekunden
   //    Innerhalb des vorherigen TestFalls `StateChange_002__RCL_RedundancyPassive` wurde 
   //    CpNrlTimerEvent() schon 1000 mal aufgerufen, somit müssen wir hier nur noch 1000 mal 
   //    für die 2 Sek Simulation aufrufen
   //
   int32_t slRunStepsT = 0;
   while (slRunStepsT < 999) // 1 + 0,9 sec
   {
      CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

      // check some values after event call
      CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
      ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_PASSIVE);  
      ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);

      slRunStepsT++;
   }

   // call last tick before event is triggered
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   //--------------------------------------------------------------------------------------------------- 
   // 3) prüfe, dass der NRL event 1x und mit korrekten Daten aufgerufen wurde
   //
   // prüfe dass der NRL event 1x und mit korrekten Daten aufgerufen wurde
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 1);
   ASSERT_EQ(CpNrlStateHandler_fake.arg0_history[0], eCP_NRL_STATE_ACTIVE);

   //--------------------------------------------------------------------------------------------------- 
   // 2) Schicke weitere CAN Nachrichten auf beiden Lines und prüfe den aktuellen NRL Zustand auf "Redundancy active"
   //

   // sende und bestätige jeweils eine CAN Nachricht
   tvStatusT = CpCoreBufferSend(&tsCanPortG, eCP_BUFFER_1);
   AcknowledgeCanTrm(eCP_CHANNEL_1, 0, eCP_BUFFER_1);
   AcknowledgeCanTrm(eCP_CHANNEL_2, 0, eCP_BUFFER_1);
   AcknowledgeCanRcv(eCP_CHANNEL_1, 0, eCP_BUFFER_2);
   AcknowledgeCanRcv(eCP_CHANNEL_2, 0, eCP_BUFFER_2);

   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);


   // Überprüfe den aktuellen Zustand
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);

   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_ACTIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);

   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsDclState.ubCanErrType, eCP_ERR_TYPE_NONE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrState, eCP_STATE_BUS_ACTIVE);
   ASSERT_EQ(tsNrlStateT.tsRclState.ubCanErrType, eCP_ERR_TYPE_NONE);
}


//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, ConnectionService_001__RCL_Receive)
{
   CpNrlState_ts tsNrlStateT;
   CpCanMsg_ts tsCanMessageT;

   /*!
    * Testfall:
    * Teste den Empfang der "Connection Service" Nachricht über RCL 
    *
    * Ergebnis:
    * - NRL hat eine "Connection Service" Nachricht für RCL empfangen
    *
    * Testablauf:
    * 1) Initialisiere NRL und prüfe den aktuellen Zustand
    * 2) Simuliere den 'Connection Service' Empfang über RCL
    * 3) Überprüfe, dass 'Connection Service' über RCL, hier CAN2 empfangen wurde
    */
   RESET_TEST_SUITE(CP_NRL);
   RUN_TEST(CP_NRL, Initialisation);
   CpCoreDriverInit_Nrl_fake.custom_fake = CpCoreDriverInit_Nrl_cf;
   CpCoreIntFunctions_Nrl_fake.custom_fake = CpCoreIntFunctions_Nrl_cf;
   CpCoreCanState_Nrl_fake.custom_fake = CpCoreCanState_Nrl_cf;
   RUN_TEST(CP_NRL, Startup);
   RESET_TEST_SUITE(CP_NRL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Initialisiere NRL und prüfe den aktuellen Zustand
   //
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_ACTIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);
   ASSERT_EQ(CpCoreBufferSend_Nrl_fake.call_count, 0);

   //--------------------------------------------------------------------------------------------------- 
   // 2) Simuliere den 'Connection Service' Empfang über RCL, hier via Buffer 1 
   //

   // prepare CAN message 
   CpMsgInit(&tsCanMessageT, CP_MSG_FORMAT_CBFF);
   CpMsgSetIdentifier(&tsCanMessageT, 0x07B);

   // confirm reception
   CpNrlMboxTickLoad(eCP_CHANNEL_2, eCP_BUFFER_1);
   CpNrlMboxCounterEvent(eCP_CHANNEL_2, eCP_BUFFER_1);

   // trigger configured NRL RCV handler and provide "received" connection message
   if (apfnRcvHandlerS[eCP_CHANNEL_2-1] != 0)
   {
      (* apfnRcvHandlerS[eCP_CHANNEL_2-1])(&tsCanMessageT, eCP_BUFFER_1);
   }

   // process NRL once
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   //--------------------------------------------------------------------------------------------------- 
   // 3) Überprüfe, dass 'Connection Service' über RCL, hier CAN2 empfangen wurde
   //
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubDclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubDclCross, 0);
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 1); // Empfang 1er connection message für RCL
   ASSERT_EQ(tsNrlStateT.ubRclCross, 0);
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, ConnectionService_002__RCL_Receive_Cross)
{
   CpNrlState_ts tsNrlStateT;
   CpCanMsg_ts tsCanMessageT;

   /*!
    * Testfall:
    * Teste den Empfang der "Connection Service" Nachricht für RCL über DCL
    *
    * Ergebnis:
    * - NRL hat eine "Connection Service" Nachricht für RCL über die DCL (falsche Leitung) empfangen
    *
    * Testablauf:
    * 1) Initialisiere NRL und prüfe den aktuellen Zustand
    * 2) Simuliere den 'Connection Service' Empfang über RCL
    * 3) Überprüfe, dass 'Connection Service' über RCL, hier CAN1 empfangen wurde
    */
   RESET_TEST_SUITE(CP_NRL);
   RUN_TEST(CP_NRL, Initialisation);
   CpCoreDriverInit_Nrl_fake.custom_fake = CpCoreDriverInit_Nrl_cf;
   CpCoreIntFunctions_Nrl_fake.custom_fake = CpCoreIntFunctions_Nrl_cf;
   CpCoreCanState_Nrl_fake.custom_fake = CpCoreCanState_Nrl_cf;
   RUN_TEST(CP_NRL, Startup);
   RESET_TEST_SUITE(CP_NRL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Initialisiere NRL und prüfe den aktuellen Zustand
   //
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_ACTIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);
   ASSERT_EQ(CpCoreBufferSend_Nrl_fake.call_count, 0);

   //--------------------------------------------------------------------------------------------------- 
   // 2) Simuliere den 'Connection Service' Empfang über RCL, hier via Buffer 1 
   //

   // prepare CAN message 
   CpMsgInit(&tsCanMessageT, CP_MSG_FORMAT_CBFF);
   CpMsgSetIdentifier(&tsCanMessageT, 0x07B);

   // confirm reception
   CpNrlMboxTickLoad(eCP_CHANNEL_1, eCP_BUFFER_1);
   CpNrlMboxCounterEvent(eCP_CHANNEL_1, eCP_BUFFER_1);

   // trigger configured NRL RCV handler and provide "received" connection message
   if (apfnRcvHandlerS[eCP_CHANNEL_1-1] != 0)
   {
      (* apfnRcvHandlerS[eCP_CHANNEL_1-1])(&tsCanMessageT, eCP_BUFFER_1);
   }

   // process NRL once
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);
   
   // prüfe dass der NRL event nicht aufgerufen wurde
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);

   //--------------------------------------------------------------------------------------------------- 
   // 3) Überprüfe, dass 'Connection Service' über RCL, hier CAN2 empfangen wurde
   //
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubDclNodes, 0);
   ASSERT_EQ(tsNrlStateT.ubDclCross, 1); // Connection Service Nachricht auf der falschen Line empfangen
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 0); // keine connection message für RCL
   ASSERT_EQ(tsNrlStateT.ubRclCross, 0);
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, ConnectionService_011__DCL_Receive)
{
   CpNrlState_ts tsNrlStateT;
   CpCanMsg_ts tsCanMessageT;

   /*!
    * Testfall:
    * Teste den Empfang der "Connection Service" Nachricht über DCL 
    *
    * Ergebnis:
    * - NRL hat eine "Connection Service" Nachricht für DCL empfangen
    *
    * Testablauf:
    * 1) Initialisiere NRL und prüfe den aktuellen Zustand
    * 2) Simuliere den 'Connection Service' Empfang über DCL
    * 3) Überprüfe, dass 'Connection Service' über DCL, hier CAN1 empfangen wurde
    */
   RESET_TEST_SUITE(CP_NRL);
   RUN_TEST(CP_NRL, Initialisation);
   CpCoreDriverInit_Nrl_fake.custom_fake = CpCoreDriverInit_Nrl_cf;
   CpCoreIntFunctions_Nrl_fake.custom_fake = CpCoreIntFunctions_Nrl_cf;
   CpCoreCanState_Nrl_fake.custom_fake = CpCoreCanState_Nrl_cf;
   RUN_TEST(CP_NRL, Startup);
   RESET_TEST_SUITE(CP_NRL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Initialisiere NRL und prüfe den aktuellen Zustand
   //
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_ACTIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);
   ASSERT_EQ(CpCoreBufferSend_Nrl_fake.call_count, 0);

   //--------------------------------------------------------------------------------------------------- 
   // 2) Simuliere den 'Connection Service' Empfang über DCL, hier via Buffer 1 
   //

   // prepare CAN message 
   CpMsgInit(&tsCanMessageT, CP_MSG_FORMAT_CBFF);
   CpMsgSetIdentifier(&tsCanMessageT, 0x07A);

   // confirm reception
   CpNrlMboxTickLoad(eCP_CHANNEL_1, eCP_BUFFER_1);
   CpNrlMboxCounterEvent(eCP_CHANNEL_1, eCP_BUFFER_1);

   // trigger configured NRL RCV handler and provide "received" connection message
   if (apfnRcvHandlerS[eCP_CHANNEL_1-1] != 0)
   {
      (* apfnRcvHandlerS[eCP_CHANNEL_1-1])(&tsCanMessageT, eCP_BUFFER_1);
   }

   // process NRL once
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   //--------------------------------------------------------------------------------------------------- 
   // 3) Überprüfe, dass 'Connection Service' über DCL, hier CAN1 empfangen wurde
   //
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubDclNodes, 1); // Empfang 1er connection message für DCL
   ASSERT_EQ(tsNrlStateT.ubDclCross, 0);
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 0); 
   ASSERT_EQ(tsNrlStateT.ubRclCross, 0);
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST(CP_NRL, ConnectionService_012__DCL_Receive_Cross)
{
   CpNrlState_ts tsNrlStateT;
   CpCanMsg_ts tsCanMessageT;

   /*!
    * Testfall:
    * Teste den Empfang der "Connection Service" Nachricht über DCL über RCL
    *
    * Ergebnis:
    * - NRL hat eine "Connection Service" Nachricht für DCL über die RCL (falsche Leitung) empfangen
    *
    * Testablauf:
    * 1) Initialisiere NRL und prüfe den aktuellen Zustand
    * 2) Simuliere den 'Connection Service' Empfang über DCL
    * 3) Überprüfe, dass 'Connection Service' über DCL, hier CAN2 empfangen wurde
    */
   RESET_TEST_SUITE(CP_NRL);
   RUN_TEST(CP_NRL, Initialisation);
   CpCoreDriverInit_Nrl_fake.custom_fake = CpCoreDriverInit_Nrl_cf;
   CpCoreIntFunctions_Nrl_fake.custom_fake = CpCoreIntFunctions_Nrl_cf;
   CpCoreCanState_Nrl_fake.custom_fake = CpCoreCanState_Nrl_cf;
   RUN_TEST(CP_NRL, Startup);
   RESET_TEST_SUITE(CP_NRL);

   //--------------------------------------------------------------------------------------------------- 
   // 1) Initialisiere NRL und prüfe den aktuellen Zustand
   //
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubNrlState, eCP_NRL_STATE_ACTIVE);  
   ASSERT_EQ(tsNrlStateT.ubActiveLine, eCP_NRL_LINE_DCL);
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);
   ASSERT_EQ(CpCoreBufferSend_Nrl_fake.call_count, 0);

   //--------------------------------------------------------------------------------------------------- 
   // 2) Simuliere den 'Connection Service' Empfang über DCL, hier via Buffer 1 
   //

   // prepare CAN message 
   CpMsgInit(&tsCanMessageT, CP_MSG_FORMAT_CBFF);
   CpMsgSetIdentifier(&tsCanMessageT, 0x07A);

   // confirm reception
   CpNrlMboxTickLoad(eCP_CHANNEL_2, eCP_BUFFER_1);
   CpNrlMboxCounterEvent(eCP_CHANNEL_2, eCP_BUFFER_1);

   // trigger configured NRL RCV handler and provide "received" connection message
   if (apfnRcvHandlerS[eCP_CHANNEL_2-1] != 0)
   {
      (* apfnRcvHandlerS[eCP_CHANNEL_2-1])(&tsCanMessageT, eCP_BUFFER_1);
   }

   // process NRL once
   CpNrlTimerEvent(eCP_NRL_CHANNEL_1);

   // prüfe dass der NRL event nicht aufgerufen wurde
   ASSERT_EQ(CpNrlStateHandler_fake.call_count, 0);

   //--------------------------------------------------------------------------------------------------- 
   // 3) Überprüfe, dass 'Connection Service' über DCL, hier CAN1 empfangen wurde
   //
   CpNrlState(eCP_NRL_CHANNEL_1, &tsNrlStateT);
   ASSERT_EQ(tsNrlStateT.ubDclNodes, 0); // kein Empfang der connection message für DCL
   ASSERT_EQ(tsNrlStateT.ubDclCross, 0);
   ASSERT_EQ(tsNrlStateT.ubRclNodes, 0); 
   ASSERT_EQ(tsNrlStateT.ubRclCross, 1); // Connection Service Nachricht auf der falschen Line empfangen
}

//--------------------------------------------------------------------------------------------------------------------//
// Reset all tests of this module                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST_SUITE_RESET(CP_NRL)
{
   RESET_FAKE(CpCoreBufferConfig_Nrl);
   RESET_FAKE(CpCoreDriverInit_Nrl);
   RESET_FAKE(CpCoreBitrate_Nrl);
   RESET_FAKE(CpCoreCanMode_Nrl);
   RESET_FAKE(CpCoreBufferSetData_Nrl);
   RESET_FAKE(CpCoreBufferSend_Nrl);
   RESET_FAKE(CpCoreBufferSetDlc_Nrl);
   RESET_FAKE(CpCoreIntFunctions_Nrl);

   RESET_FAKE(CpNrlStateHandler);
   
}


//--------------------------------------------------------------------------------------------------------------------//
// Run all tests of this module                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------// 
TEST_SUITE_RUN(CP_NRL)
{
   RUN_TEST(CP_NRL, StateChange_001__RCL_RedundancyDisabled);                       // #6 of State Machine
   RUN_TEST(CP_NRL, StateChange_002__RCL_RedundancyPassive);                        // #2 of State Machine
   RUN_TEST(CP_NRL, StateChange_003__RCL_RedundancyPassiveNoRecover);               // #2 of State Machine
   RUN_TEST(CP_NRL, StateChange_004__RCL_NetworkDisconnected);                      // #1->#8 of State Machine
   RUN_TEST(CP_NRL, StateChange_005__RCL_RedundancyPassiveToDisable);               // #4 of State Machine   
   RUN_TEST(CP_NRL, StateChange_006__RCL_RedundancyActiveToNetworkDisconnected);    // #8 of State Machine 
   RUN_TEST(CP_NRL, StateChange_007__RCL_RedundancyDisabledToNetworkDisconnected);  // #5 of State Machine 

   RUN_TEST(CP_NRL, StateChange_011__DCL_RedundancyDisabled);                       // #6 of State Machine
   RUN_TEST(CP_NRL, StateChange_012__DCL_RedundancyPassive);                        // #2 of State Machine
   RUN_TEST(CP_NRL, StateChange_013__DCL_RedundancyPassiveNoRecover);               // #2 of State Machine
   RUN_TEST(CP_NRL, StateChange_014__DCL_RedundancyPassiveToDisable);               // #4 of State Machine   

   RUN_TEST(CP_NRL, RecoveryMode_001__RCL_RedundancyDisabledToActive);              // #7 of State Machine
   RUN_TEST(CP_NRL, RecoveryMode_002__RCL_RedundancyPassiveToActive);               // #3 of State Machine

   //RUN_TEST(CP_NRL, RecoveryMode_002__RCL_RedundancyPassiveToActive_2);             // #3 of State Machine after timeout

   RUN_TEST(CP_NRL, ConnectionService_001__RCL_Receive);
   RUN_TEST(CP_NRL, ConnectionService_002__RCL_Receive_Cross);
   RUN_TEST(CP_NRL, ConnectionService_011__DCL_Receive);
   RUN_TEST(CP_NRL, ConnectionService_012__DCL_Receive_Cross);
}