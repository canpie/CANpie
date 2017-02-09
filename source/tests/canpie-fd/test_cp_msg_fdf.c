//============================================================================//
// File:          test_cp_msg_fdf.c                                           //
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

#define  CP_CAN_FD               1
#define  CP_CAN_MSG_MACRO        0


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

TEST_GROUP(CP_MSG_FDF);     // test group name
static CpCanMsg_ts    tsCanMsgS;

/*----------------------------------------------------------------------------*\
** Function implementations                                                   **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// TEST_SETUP()                                                               //
// init code for each test case                                               //
//----------------------------------------------------------------------------//
TEST_SETUP(CP_MSG_FDF)
{

}

//----------------------------------------------------------------------------//
// TEST_TEAR_DOWN()                                                           //
// release code for each test case                                            //
//----------------------------------------------------------------------------//
TEST_TEAR_DOWN(CP_MSG_FDF)
{

}

//----------------------------------------------------------------------------//
// CpMsgClear()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
TEST(CP_MSG_FDF, 001)
{
   //----------------------------------------------------------------
      // @SubTest01
      //
      CpMsgClear(&tsCanMsgS);
      TEST_ASSERT_EQUAL_UINT16(0, CpMsgGetStdId(&tsCanMsgS));
      TEST_ASSERT_EQUAL_UINT32(0,CpMsgGetExtId(&tsCanMsgS));
      TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
      TEST_ASSERT_EQUAL_UINT8(0,CpMsgGetDlc(&tsCanMsgS));

      //----------------------------------------------------------------
      // @SubTest02
      //
      CpMsgSetStdId(&tsCanMsgS, 0xE04);
      CpMsgSetExtId(&tsCanMsgS, 0x2000E04);
      CpMsgSetDlc(&tsCanMsgS, 0x56);
      CpMsgSetData(&tsCanMsgS, 1,0x63);

      CpMsgClear(&tsCanMsgS);

      TEST_ASSERT_EQUAL_UINT32(0, CpMsgGetStdId(&tsCanMsgS));
      TEST_ASSERT_EQUAL_UINT32(0, CpMsgGetExtId(&tsCanMsgS));
      TEST_ASSERT_EQUAL_UINT32(0, CpMsgGetDlc(&tsCanMsgS));
      TEST_ASSERT_EQUAL_UINT32(0, CpMsgGetStdId(&tsCanMsgS));
      TEST_ASSERT_EQUAL_UINT32(0, CpMsgGetData(&tsCanMsgS,1));

      //----------------------------------------------------------------
      // @SubTest03
      //
      memset(&tsCanMsgS, 0xFF, sizeof(CpCanMsg_ts));
      CpMsgClear(&tsCanMsgS);
      TEST_ASSERT_EQUAL_UINT32(0, CpMsgGetStdId(&tsCanMsgS));
      TEST_ASSERT_EQUAL_UINT32(0,CpMsgGetExtId(&tsCanMsgS));
      TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
      TEST_ASSERT_EQUAL_UINT32(0,CpMsgGetDlc(&tsCanMsgS));
}


//----------------------------------------------------------------------------//
// CpMsgSetStdId()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
TEST(CP_MSG_FDF, 002)
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
TEST(CP_MSG_FDF, 003)
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
TEST_GROUP_RUNNER(CP_MSG_FDF)
{
   UnityPrint("Run test group CP_MSG_FDF ");
   printf("\n");

   RUN_TEST_CASE(CP_MSG_FDF, 001);
   RUN_TEST_CASE(CP_MSG_FDF, 002);
   RUN_TEST_CASE(CP_MSG_FDF, 003);

   printf("\n");

}

