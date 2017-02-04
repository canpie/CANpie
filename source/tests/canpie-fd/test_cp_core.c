//============================================================================//
// File:          com_test_mgr.c                                              //
// Description:   Unit tests for CANopen Master MGR module                    //
// Author:        Uwe Koppe                                                   //
// e-mail:        koppe@microcontrol.net                                      //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// Junkersring 23                                                             //
// 53844 Troisdorf                                                            //
// Germany                                                                    //
// Tel: +49-2241-25659-0                                                      //
// Fax: +49-2241-25659-11                                                     //
//                                                                            //
// The copyright to the computer program(s) herein is the property of         //
// MicroControl GmbH & Co. KG, Germany. The program(s) may be used            //
// and/or copied only with the written permission of MicroControl GmbH &      //
// Co. KG or in accordance with the terms and conditions stipulated in        //
// the agreement/contract under which the program(s) have been supplied.      //
//----------------------------------------------------------------------------//
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 05.10.2015  Initial version                                                //
//                                                                            //
//============================================================================//


//------------------------------------------------------------------------------
// SVN  $Date: 2015-10-17 18:16:40 +0200 (Sa, 17 Okt 2015) $
// SVN  $Rev: 7160 $ --- $Author: koppe $
//------------------------------------------------------------------------------


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "cp_core.h"
#include "unity_fixture.h"

/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

TEST_GROUP(CP_CORE);     // test group name


/*----------------------------------------------------------------------------*\
** Function implementations                                                   **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// TEST_SETUP()                                                               //
// init code for each test case                                               //
//----------------------------------------------------------------------------//
TEST_SETUP(CP_CORE)
{

}

//----------------------------------------------------------------------------//
// TEST_TEAR_DOWN()                                                           //
// release code for each test case                                            //
//----------------------------------------------------------------------------//
TEST_TEAR_DOWN(CP_CORE)
{

}

//----------------------------------------------------------------------------//
// Test case CP_CORE_001                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
TEST(CP_CORE, 001)
{
   CpStatus_tv    tvResultT;
   CpPort_ts      tsPortT;

   //----------------------------------------------------------------
   // valid initialisation
   //
   tvResultT = CpCoreDriverInit( eCP_CHANNEL_1, &tsPortT, 0);
   TEST_ASSERT_EQUAL(tvResultT, eCP_ERR_NONE);

   tvResultT = CpCoreDriverRelease(&tsPortT);
   TEST_ASSERT_EQUAL(tvResultT, eCP_ERR_NONE);


   //----------------------------------------------------------------
   // init two times initialisation
   //
   tvResultT = CpCoreDriverInit( eCP_CHANNEL_1, &tsPortT, 0);
   TEST_ASSERT_EQUAL(eCP_ERR_NONE, tvResultT);

  // tvResultT = CpCoreDriverInit( eCP_CHANNEL_1, &tsPortT, 0);
   //TEST_ASSERT_EQUAL(eCP_ERR_INIT_FAIL, tvResultT);

   tvResultT = CpCoreDriverRelease(&tsPortT);
   TEST_ASSERT_EQUAL(tvResultT, eCP_ERR_NONE);

   //----------------------------------------------------------------
   // release two times
   //

}


//----------------------------------------------------------------------------//
// Test case CP_CORE_002                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
TEST(CP_CORE, 002)
{
   CpStatus_tv    tvResultT;
   CpPort_ts      tsPortT;

   //----------------------------------------------------------------
   // valid initialisation
   //
   tvResultT = CpCoreDriverInit( eCP_CHANNEL_1, &tsPortT, 0);
   TEST_ASSERT_EQUAL(tvResultT, eCP_ERR_NONE);

   tvResultT = CpCoreDriverRelease(&tsPortT);
   TEST_ASSERT_EQUAL(tvResultT, eCP_ERR_NONE);


   //----------------------------------------------------------------
   // calling functions without valid initialisation
   //
  // tvResultT = CpCoreBitrate(tsPortT, eCP_BITRATE_500K, eCP_BITRATE_NONE);
 //  TEST_ASSERT_EQUAL(eCP_ERR_INIT_MISSING, tvResultT);


}

//----------------------------------------------------------------------------//
// TEST_GROUP_RUNNER()                                                        //
// execute all test cases                                                     //
//----------------------------------------------------------------------------//
TEST_GROUP_RUNNER(CP_CORE)
{
   //printf("Run test group CP_MSG_FDM ...\n");

   UnityPrint("Run test group CP_CORE");
   printf("\n");
   RUN_TEST_CASE(CP_CORE, 001);
   RUN_TEST_CASE(CP_CORE, 002);
   printf("\n");

}

