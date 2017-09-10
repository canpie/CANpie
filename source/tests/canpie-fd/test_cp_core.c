//============================================================================//
// File:          test_cp_core.c                                              //
// Description:   Unit tests for CANpie core functions                        //
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





/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "cp_core.h"
#include "unity_fixture.h"

#include <string.h>

/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

TEST_GROUP(CP_CORE);     // test group name

static    CpPort_ts      tsPortS;

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
   CpStatus_tv    tvResultT;
   
   memset(&tsPortS, 0, sizeof(CpPort_ts));
   
   //----------------------------------------------------------------
   // valid initialisation
   //
   tvResultT = CpCoreDriverInit(eCP_CHANNEL_1, &tsPortS, 0);
   TEST_ASSERT_EQUAL(tvResultT, eCP_ERR_NONE);
}

//----------------------------------------------------------------------------//
// TEST_TEAR_DOWN()                                                           //
// release code for each test case                                            //
//----------------------------------------------------------------------------//
TEST_TEAR_DOWN(CP_CORE)
{
   CpStatus_tv    tvResultT;
   
   //----------------------------------------------------------------
   // valid release
   //
   tvResultT = CpCoreDriverRelease(&tsPortS);
   TEST_ASSERT_EQUAL(tvResultT, eCP_ERR_NONE);
}

//----------------------------------------------------------------------------//
// Test case CP_CORE_001                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
TEST(CP_CORE, 001)
{
   CpStatus_tv    tvResultT;


   tvResultT = CpCoreDriverRelease(&tsPortS);
   TEST_ASSERT_EQUAL(tvResultT, eCP_ERR_NONE);


   //----------------------------------------------------------------
   // init two times initialisation
   //
   tvResultT = CpCoreDriverInit(eCP_CHANNEL_1, &tsPortS, 0);
   TEST_ASSERT_EQUAL(eCP_ERR_NONE, tvResultT);

   tvResultT = CpCoreDriverInit(eCP_CHANNEL_1, &tsPortS, 0);
   TEST_ASSERT_EQUAL(eCP_ERR_INIT_FAIL, tvResultT);



   //----------------------------------------------------------------
   // release two times
   //

   UnityPrint("CP_CORE_001: PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
// Test case CP_CORE_002                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
TEST(CP_CORE, 002)
{
   CpStatus_tv    tvResultT;


   tvResultT = CpCoreDriverRelease(&tsPortS);
   TEST_ASSERT_EQUAL(tvResultT, eCP_ERR_NONE);


   //----------------------------------------------------------------
   // calling functions without valid initialisation
   //
   tvResultT = CpCoreBitrate(&tsPortS, eCP_BITRATE_500K, eCP_BITRATE_NONE);
   TEST_ASSERT_EQUAL(eCP_ERR_INIT_MISSING, tvResultT);

   UnityPrint("CP_CORE_002: PASSED");
   printf("\n");

}


//----------------------------------------------------------------------------//
// Test case CP_CORE_003                                                      //
// test bit-rate setting                                                      //
//----------------------------------------------------------------------------//
TEST(CP_CORE, 003)
{
   CpStatus_tv    tvResultT;



   //----------------------------------------------------------------
   // set in-valid bit-rate
   //
   tvResultT = CpCoreBitrate(&tsPortS, eCP_BITRATE_NONE, eCP_BITRATE_NONE);
   TEST_ASSERT_EQUAL(eCP_ERR_BITRATE, tvResultT);

   tvResultT = CpCoreBitrate(&tsPortS, eCP_BITRATE_MAX + 1, eCP_BITRATE_NONE);
   TEST_ASSERT_EQUAL(eCP_ERR_BITRATE, tvResultT);

   UnityPrint("CP_CORE_003: PASSED");
   printf("\n");

}


//----------------------------------------------------------------------------//
// Test case CP_CORE_004                                                      //
// test mode setting                                                          //
//----------------------------------------------------------------------------//
TEST(CP_CORE, 004)
{
   CpStatus_tv    tvResultT;



   //----------------------------------------------------------------
   // calling functions without valid initialisation
   //
   tvResultT = CpCoreBitrate(&tsPortS, eCP_BITRATE_500K, eCP_BITRATE_NONE);
   TEST_ASSERT_EQUAL(eCP_ERR_NONE, tvResultT);


   UnityPrint("CP_CORE_004: PASSED");
   printf("\n");

}


//----------------------------------------------------------------------------//
// TEST_GROUP_RUNNER()                                                        //
// execute all test cases                                                     //
//----------------------------------------------------------------------------//
TEST_GROUP_RUNNER(CP_CORE)
{
   UnityPrint("--- Run test group: CP_CORE ----------------------------------");
   printf("\n");

   RUN_TEST_CASE(CP_CORE, 001);
   RUN_TEST_CASE(CP_CORE, 002);
   RUN_TEST_CASE(CP_CORE, 003);
   RUN_TEST_CASE(CP_CORE, 004);
   printf("\n");

}

