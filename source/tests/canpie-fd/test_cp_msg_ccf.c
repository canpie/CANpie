//============================================================================//
// File:          test_cp_msg_ccf.c                                           //
// Description:   Unit tests for CANpie message functions (classic CAN)       //
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
** Pre Condition files                                                        **
**                                                                            **
\*---------------------------------------------------------------------------*/

/*!
** \def  CP_CAN_FD
** ** This symbol enables or disables the CAN FD extension
** \def CP_CAN_MSG_MACRO
** This symbol enables or disables using MSG_MACROs or MSG_FUNCTIONs.
*/
#define  CP_CAN_FD               0
#define  CP_CAN_MSG_MACRO        0

/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include "cp_msg.h"
#include "unity_fixture.h"

#include <stdio.h>
#include <string.h>

//----------------------------------------------------------------------------//
/*!
** \file    test_cp_msg_ccf.c
** \brief   CANpie test cases for message-functions
**
** &nbsp;<p>
** The complex CANpie API uses a mass of functions to program and control any supported controller.
** To test these functions this file will include all test cases as introduced in the MicroControl
** Test Specification.
** <p>
**
*/
//----------------------------------------------------------------------------//


/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/
TEST_GROUP(CP_MSG_CCF);             // test group name
static CpCanMsg_ts    tsCanMsgS;    // CAN message


/*----------------------------------------------------------------------------*\
** Function implementations                                                   **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// TEST_SETUP()                                                               //
// init code for each test case                                               //
//----------------------------------------------------------------------------//
TEST_SETUP(CP_MSG_CCF)
{

}


//----------------------------------------------------------------------------//
// TEST_TEAR_DOWN()                                                           //
// release code for each test case                                            //
//----------------------------------------------------------------------------//
TEST_TEAR_DOWN(CP_MSG_CCF)
{

}

//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_001
**
** This test cases shall verify the correct field values of the CAN message
** structure CpCanMsg_s using the CpMsgClear() function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, 001)
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
/*!
** \brief   CP_MSG_CCF_002
**
** The cases shall check the correct behavior of setting and getting a
** 11-Bit identifier using the CpMsgSetStdId() and  CpMsgIsExtended() function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, 002)
{
   //----------------------------------------------------------------
   // @SubTest01
   //
   uint16_t uwCountS;
   CpMsgClear(&tsCanMsgS);
   for(uwCountS= 0x0000; uwCountS <= 0x07FF;uwCountS++)
   {
      CpMsgSetStdId(&tsCanMsgS, uwCountS);
      TEST_ASSERT_EQUAL_UINT32(uwCountS, CpMsgGetStdId(&tsCanMsgS));
      TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgClear(&tsCanMsgS);
   for(uwCountS = 0x0FF0; uwCountS < 0xFEFF ; uwCountS++)
   {
      CpMsgSetStdId(&tsCanMsgS, uwCountS);
      TEST_ASSERT_EQUAL_UINT32(uwCountS & CP_MASK_STD_FRAME, CpMsgGetStdId(&tsCanMsgS));
      TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest03
   //
   CpMsgClear(&tsCanMsgS);
   for(uwCountS = 0xFFFF; uwCountS >= 0xFF00; uwCountS--) //0xF800
   {
      CpMsgSetStdId(&tsCanMsgS, uwCountS);
      TEST_ASSERT_EQUAL_UINT32(uwCountS & CP_MASK_STD_FRAME, CpMsgGetStdId(&tsCanMsgS));
      TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
   }
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_003
**
** The cases shall check the correct behavior of setting and getting a
** Data Length Code in a range of 8-Bit using the CpMsgSetDlc()
** and  CpMsgGetDlc() function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, 003)
{
   //----------------------------------------------------------------
   // @SubTest01
   //
   uint8_t ubCountS;
   CpMsgClear(&tsCanMsgS);
   for(ubCountS = 0x00 ; ubCountS <= 0x08 ; ubCountS++)
   {
      CpMsgSetDlc(&tsCanMsgS, ubCountS);
      TEST_ASSERT_EQUAL_UINT32(ubCountS & 0xFF, CpMsgGetDlc(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest02
   //

      CpMsgClear(&tsCanMsgS);

      CpMsgSetDlc(&tsCanMsgS, 0x01);

      for(ubCountS = 0xFF ; ubCountS >=0x09 ; ubCountS--)
      {
         CpMsgSetDlc(&tsCanMsgS, ubCountS);
         TEST_ASSERT_EQUAL_UINT32(0x01, CpMsgGetDlc(&tsCanMsgS));
      }
}

//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_004
** <br>The cases shall check the correct behaviour of setting and getting
** The 8 -Bits of Data using the CpMsgSetData() and CpMsgGetData() function.<br>
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, 004)
{
   //----------------------------------------------------------------
   // @SubTest01
   //
   uint8_t ubCountS;
   CpMsgClear(&tsCanMsgS);
   for(ubCountS = 0x00 ; ubCountS <= 0x07; ubCountS++)
   {
      CpMsgSetData(&tsCanMsgS, ubCountS, 0xAA);
      TEST_ASSERT_EQUAL_UINT8(0xAA & CP_MASK_STD_FRAME,CpMsgGetData(&tsCanMsgS, ubCountS));
   }

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgClear(&tsCanMsgS);
   for(ubCountS = 0xFF; ubCountS >= 0x08; ubCountS--)
   {
      CpMsgSetData(&tsCanMsgS, ubCountS, 0x55);
      TEST_ASSERT_EQUAL_UINT8(0x00 & CP_MASK_STD_FRAME, CpMsgGetData(&tsCanMsgS,ubCountS));
   }
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_005
**
** The cases shall check the correct behavior of setting and getting the
** extended 29-Bit identifier using the CpMsgSetExtId()and CpMsgGetExtId()
** function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, 005)
{

   //----------------------------------------------------------------
   // @SubTest01
   //
   uint8_t ubCountS;
   uint16_t uwCountS;
   uint32_t ulCountS;
   CpMsgClear(&tsCanMsgS);
   for(uwCountS= 0x00; uwCountS<= 0x7FF; uwCountS++)
   {
      CpMsgSetExtId(&tsCanMsgS, uwCountS);
      TEST_ASSERT_EQUAL_UINT32(uwCountS,CpMsgGetExtId(&tsCanMsgS));
      TEST_ASSERT_TRUE( CpMsgIsExtended(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest02
   //
   ulCountS = 0x4000000;
   CpMsgClear(&tsCanMsgS);  // clear the message
   for(ubCountS= 0; ubCountS<21;ubCountS++)
   {
      CpMsgSetExtId(&tsCanMsgS, ulCountS);
      TEST_ASSERT_EQUAL_UINT32(ulCountS,CpMsgGetExtId(&tsCanMsgS));
      TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
      ulCountS +=0x1000000;
    }

   //----------------------------------------------------------------
   // @SubTest03
   //
   ulCountS = 0x8000000;
   CpMsgClear(&tsCanMsgS);
   for(ubCountS= 0; ubCountS<21;ubCountS++)
   {
      CpMsgSetExtId(&tsCanMsgS, ulCountS);
      TEST_ASSERT_EQUAL_UINT32(ulCountS,CpMsgGetExtId(&tsCanMsgS));
      TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
      ulCountS +=0x1000000;
   }

   //----------------------------------------------------------------
   // @SubTest04
   //
   CpMsgSetExtId(&tsCanMsgS, 0xC0000000);
   TEST_ASSERT_EQUAL_UINT32(0,CpMsgGetExtId(&tsCanMsgS));
   TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
   CpMsgSetStdId(&tsCanMsgS, ubCountS);
   TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_006
**
** The cases shall check the correct behaviour of setting the RTR -Bit
** (Remote-Bit) of an Can Message Frame using the CpMsgSetRemote() function.
** The Cases also will check a Can Message Frame if the RTS -bit is set or not
** by calling the CpMsgIsRemote() function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, 006)
{
   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgClear(&tsCanMsgS);
   TEST_ASSERT_FALSE(CpMsgIsRemote(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgClear(&tsCanMsgS);
   CpMsgSetRemote(&tsCanMsgS);
   TEST_ASSERT_TRUE(CpMsgIsRemote(&tsCanMsgS));
}


//-----------------------------------------------------------------------------------//
// TEST_GROUP_RUNNER()                                                               //
// execute all test cases                                                            //
//-----------------------------------------------------------------------------------//
TEST_GROUP_RUNNER(CP_MSG_CCF)
{
   UnityPrint("Run test group CP_MSG_CCF ");
   printf("\n");
   RUN_TEST_CASE(CP_MSG_CCF, 001);
   RUN_TEST_CASE(CP_MSG_CCF, 002);
   RUN_TEST_CASE(CP_MSG_CCF, 003);
   RUN_TEST_CASE(CP_MSG_CCF, 004);
   RUN_TEST_CASE(CP_MSG_CCF, 005);
   RUN_TEST_CASE(CP_MSG_CCF, 006);
   printf("\n");
}
