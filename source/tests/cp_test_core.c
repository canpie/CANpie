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
// MgrInit01()                                                                //
// check return value for ComMgrInit(                                         //
//----------------------------------------------------------------------------//
TEST(CP_CORE, CpCoreDriverInit)
{
   CpStatus_tv    tvResultT;
   CpPort_ts      tsPortT;

   //----------------------------------------------------------------
   // valid initialisation
   //
   tvResultT = CpCoreDriverInit( CP_CHANNEL_1, &tsPortT);
   TEST_ASSERT_EQUAL(tvResultT, CpErr_OK);

   tvResultT = CpCoreDriverRelease(&tsPortT);
   TEST_ASSERT_EQUAL(tvResultT, CpErr_OK);


   //----------------------------------------------------------------
   // init two times initialisation
   //
   /*
   tvResultT = ComMgrInit( CP_CHANNEL_1, eCOM_NET_1, CP_BAUD_500K, 1,
                           eCOM_MODE_NMT_MASTER);
   TEST_ASSERT_EQUAL(tvResultT, eCOM_ERR_OK);

   tvResultT = ComMgrInit( CP_CHANNEL_1, eCOM_NET_1, CP_BAUD_500K, 1,
                           eCOM_MODE_NMT_MASTER);
   TEST_ASSERT_NOT_EQUAL(tvResultT, eCOM_ERR_OK);

   tvResultT = ComMgrRelease(eCOM_NET_1);
   TEST_ASSERT_EQUAL(tvResultT, eCOM_ERR_OK);
   */

   //----------------------------------------------------------------
   // release two times
   //

}


//----------------------------------------------------------------------------//
// TEST_GROUP_RUNNER()                                                        //
// execute all test cases                                                     //
//----------------------------------------------------------------------------//
TEST_GROUP_RUNNER(CP_CORE)
{
   //UnityPrint("MGR tests ");
   RUN_TEST_CASE(CP_CORE, CpCoreDriverInit);
   //RUN_TEST_CASE(MGR, MgrInit02);

}

