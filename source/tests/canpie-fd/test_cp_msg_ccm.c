//============================================================================//
// File:          test_cp_msg_ccm.c                                           //
// Description:   Unit tests for CANpie message macros (classic CAN)          //
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


/*----------------------------------------------------------------------------*\
** Pre-condition                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define  CP_CAN_FD               0
#define  CP_CAN_MSG_MACRO        1

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

TEST_GROUP(CP_MSG_CCM);     // test group name


/*----------------------------------------------------------------------------*\
** Function implementations                                                   **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// TEST_SETUP()                                                               //
// init code for each test case                                               //
//----------------------------------------------------------------------------//
TEST_SETUP(CP_MSG_CCM)
{

}

//----------------------------------------------------------------------------//
// TEST_TEAR_DOWN()                                                           //
// release code for each test case                                            //
//----------------------------------------------------------------------------//
TEST_TEAR_DOWN(CP_MSG_CCM)
{

}

//----------------------------------------------------------------------------//
// Test case CP_MSG_CCM_001                                                   //
//                                                                            //
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCM, 001)
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
TEST(CP_MSG_CCM, 002)
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
TEST(CP_MSG_CCM, 003)
{
   CpCanMsg_ts    tsCanMsgT;

   CpMsgClear(&tsCanMsgT);                // clear the message


}


//----------------------------------------------------------------------------//
// TEST_GROUP_RUNNER()                                                        //
// execute all test cases                                                     //
//----------------------------------------------------------------------------//
TEST_GROUP_RUNNER(CP_MSG_CCM)
{
   UnityPrint("Run test group CP_MSG_CCM ");
   printf("\n");
 //  printf("Size of structure CpCanMsg_s %d\n", sizeof(struct CpCanMsg_s));

   RUN_TEST_CASE(CP_MSG_CCM, 001);
   RUN_TEST_CASE(CP_MSG_CCM, 002);
   RUN_TEST_CASE(CP_MSG_CCM, 003);
   printf("\n");

}

