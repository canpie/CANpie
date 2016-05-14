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



/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "cp_msg.h"
#include "unity_fixture.h"

/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

TEST_GROUP(CP_MSG);     // test group name


/*----------------------------------------------------------------------------*\
** Function implementations                                                   **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// TEST_SETUP()                                                               //
// init code for each test case                                               //
//----------------------------------------------------------------------------//
TEST_SETUP(CP_MSG)
{

}

//----------------------------------------------------------------------------//
// TEST_TEAR_DOWN()                                                           //
// release code for each test case                                            //
//----------------------------------------------------------------------------//
TEST_TEAR_DOWN(CP_MSG)
{

}

//----------------------------------------------------------------------------//
// CpMsgClear()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
TEST(CP_MSG, CpMsgClear)
{
   CpCanMsg_ts    tsCanMsgT;

   CpMsgClear(&tsCanMsgT);                // clear the message


   TEST_ASSERT_EQUAL( CpMsgGetDlc(&tsCanMsgT), 0);

   TEST_ASSERT_EQUAL( CpMsgGetStdId(&tsCanMsgT), 0);

}


//----------------------------------------------------------------------------//
// CpMsgSetStdId()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
TEST(CP_MSG, CpMsgSetStdId)
{
   CpCanMsg_ts    tsCanMsgT;
   uint16_t       uwStdIdT;

   CpMsgClear(&tsCanMsgT);                // clear the message


   for(uwStdIdT = 0; uwStdIdT < CP_MASK_STD_FRAME; uwStdIdT ++)
   {
      CpMsgSetStdId(&tsCanMsgT, uwStdIdT);
      TEST_ASSERT_EQUAL( CpMsgGetStdId(&tsCanMsgT), uwStdIdT);
      TEST_ASSERT_EQUAL( CpMsgIsExtended(&tsCanMsgT), 0);
   }
}


//----------------------------------------------------------------------------//
// CpMsgSetExtId()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
TEST(CP_MSG, CpMsgSetExtId)
{
   CpCanMsg_ts    tsCanMsgT;
   uint32_t       ulExtIdT;

   CpMsgClear(&tsCanMsgT);                // clear the message


   for(ulExtIdT = 0; ulExtIdT < CP_MASK_EXT_FRAME; ulExtIdT ++)
   {
      CpMsgSetExtId(&tsCanMsgT, ulExtIdT);
      TEST_ASSERT_EQUAL( CpMsgGetExtId(&tsCanMsgT), ulExtIdT);
      TEST_ASSERT_EQUAL( CpMsgIsExtended(&tsCanMsgT), 1);
   }
}


//----------------------------------------------------------------------------//
// TEST_GROUP_RUNNER()                                                        //
// execute all test cases                                                     //
//----------------------------------------------------------------------------//
TEST_GROUP_RUNNER(CP_MSG)
{
   RUN_TEST_CASE(CP_MSG, CpMsgClear);
   RUN_TEST_CASE(CP_MSG, CpMsgSetStdId);
   RUN_TEST_CASE(CP_MSG, CpMsgSetExtId);

}

