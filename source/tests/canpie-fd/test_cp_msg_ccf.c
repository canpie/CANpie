//============================================================================//
// File:          test_cp_msg_ccf.c                                           //
// Description:   Unit tests for CANpie message functions (classic CAN)       //
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
** Pre-condition settings                                                     **
**                                                                            **
\*----------------------------------------------------------------------------*/

/*!
** Support classical CAN only and do not use macros for message
** access.
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
#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------------//
/*!
** \file    test_cp_msg_ccf.c
** \brief   CANpie test cases for message functions
**
**
*/
//----------------------------------------------------------------------------//


/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/
TEST_GROUP(CP_MSG_CCF);             // test group name
static CpCanMsg_ts    tsCanMsgS;    // CAN message
static CpTime_ts      tsCanTimeS;    // CAN TimeStamp


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
** This test shall check the correct behaviour of initialising a
** CpCanMsg_s and setting the Format to ClassicCanFormat or ExtendedCanFormat
** using the CpMsgInit() function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, 001)
{
   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CBFF);
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetDlc(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetIdentifier(&tsCanMsgS));
   TEST_ASSERT_FALSE(CpMsgIsFastData(&tsCanMsgS));
   TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgSetDlc(&tsCanMsgS, 0x07);
   CpMsgSetIdentifier(&tsCanMsgS, 0x01);
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CBFF);
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetDlc(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetIdentifier(&tsCanMsgS));
   TEST_ASSERT_FALSE(CpMsgIsFastData(&tsCanMsgS));
   TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest03
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CEFF);
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetDlc(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetIdentifier(&tsCanMsgS));
   TEST_ASSERT_FALSE(CpMsgIsFastData(&tsCanMsgS));
   TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest4
   //
   CpMsgSetDlc(&tsCanMsgS, 0x07);
   CpMsgSetIdentifier(&tsCanMsgS, 0x01);
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CEFF);
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetDlc(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetIdentifier(&tsCanMsgS));
   TEST_ASSERT_FALSE(CpMsgIsFastData(&tsCanMsgS));
   TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
   UnityPrint(" CP_MSG_CCF_001: PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_002
**
** The cases shall check the correct behaviour of setting and getting a
** Identifier (11-bit ClassicCanFormat or 29-bit ExtendedCanFormat) using the
** CpMsgSetIdentifier(), CpMsgGetIdentifier() and CpMsgIsExtended() functions.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, 002)
{
   uint32_t ulIdValueT;

   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CBFF);
   for (ulIdValueT = 0x00; ulIdValueT <= 0x7FF; ulIdValueT++)
   {
      CpMsgSetIdentifier(&tsCanMsgS, ulIdValueT);
      TEST_ASSERT_EQUAL_UINT32(ulIdValueT,
                               CpMsgGetIdentifier(&tsCanMsgS));
      TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CBFF);
   for (ulIdValueT = 0xFFFF; ulIdValueT > 0x7FF; ulIdValueT--)
   {
      CpMsgSetIdentifier(&tsCanMsgS, ulIdValueT);
      TEST_ASSERT_EQUAL_UINT32(ulIdValueT & CP_MASK_STD_FRAME,
                               CpMsgGetIdentifier(&tsCanMsgS));
      TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest03
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CEFF);
   for (ulIdValueT = 0x000; ulIdValueT <= 0x7FF; ulIdValueT++)
   {
      CpMsgSetIdentifier(&tsCanMsgS, ulIdValueT);
      TEST_ASSERT_EQUAL_UINT32(ulIdValueT,
                               CpMsgGetIdentifier(&tsCanMsgS));
      TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest04
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CEFF);
   for (ulIdValueT = 0x8000000; ulIdValueT <= 0x1C000000; ulIdValueT += 0x400)
   {
      CpMsgSetIdentifier(&tsCanMsgS, ulIdValueT);
      TEST_ASSERT_EQUAL_UINT32(ulIdValueT & CP_MASK_EXT_FRAME,
                               CpMsgGetIdentifier(&tsCanMsgS));
      TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest05
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CEFF);
   for (ulIdValueT = 0x1FF00240; ulIdValueT <=0x10000000; ulIdValueT += 0x800)
   {
      CpMsgSetIdentifier(&tsCanMsgS, ulIdValueT);
      TEST_ASSERT_EQUAL_UINT32(ulIdValueT & CP_MASK_EXT_FRAME,
                               CpMsgGetIdentifier(&tsCanMsgS));
      TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest07
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CEFF);
   for (ulIdValueT = 0x30000000; ulIdValueT <=0xE0000000; ulIdValueT += 0x400)
   {
      CpMsgSetIdentifier(&tsCanMsgS, ulIdValueT);
      TEST_ASSERT_EQUAL_UINT32(ulIdValueT & CP_MASK_EXT_FRAME,
                               CpMsgGetIdentifier(&tsCanMsgS));
      TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
   }
   UnityPrint(" CP_MSG_CCF_002: PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_003
**
** The cases shall check the correct behaviour of setting and getting a
** Data Length Code in a range of 8-Bit using the CpMsgSetDlc()
** and  CpMsgGetDlc() function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, 003)
{
   uint8_t ubDlcT;

   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CBFF);
   for (ubDlcT = 0x00; ubDlcT <= 0x08; ubDlcT++)
   {
      CpMsgSetDlc(&tsCanMsgS, ubDlcT);
      TEST_ASSERT_EQUAL_UINT8(ubDlcT, CpMsgGetDlc(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CBFF);
   CpMsgSetDlc(&tsCanMsgS, 0x01);
   for (ubDlcT = 0xFF; ubDlcT > 0x08; ubDlcT--)
   {
      CpMsgSetDlc(&tsCanMsgS, ubDlcT);
      TEST_ASSERT_EQUAL_UINT8(0x01, CpMsgGetDlc(&tsCanMsgS));
   }
   UnityPrint(" CP_MSG_CCF_003: PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_004
**
** The cases shall check the correct behaviour of setting and getting
** the payload by using the CpMsgSetData() and CpMsgGetData() function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, 004)
{
   uint8_t ubPosT;

   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CBFF);
   for (ubPosT = 0x00 ; ubPosT <= 0x07; ubPosT++)
   {
      CpMsgSetData(&tsCanMsgS, ubPosT, 0xAA);
      TEST_ASSERT_EQUAL_UINT8(0xAA, CpMsgGetData(&tsCanMsgS, ubPosT));
   }

   //----------------------------------------------------------------
   // @SubTest02
   //
   for (ubPosT = 0xFF; ubPosT > 0x07; ubPosT--)
   {
      CpMsgSetData(&tsCanMsgS, ubPosT, 0x55);
      TEST_ASSERT_EQUAL_UINT8(0x00, CpMsgGetData(&tsCanMsgS, ubPosT));
   }

   //----------------------------------------------------------------
   // @SubTest03
   //
   for (ubPosT = 0x00; ubPosT < 0x08; ubPosT++)
   {
      TEST_ASSERT_EQUAL_UINT8(0xAA, CpMsgGetData(&tsCanMsgS, ubPosT));
   }
   UnityPrint(" CP_MSG_CCF_004: PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_005
**
** The cases shall check the correct behaviour of getting and setting a time-
** stamp to a CAN message by using CpMsgSetTime() and CpMsgGetTime() functions.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, 005)
{
   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CBFF);
   tsCanTimeS.ulNanoSec = 3487;
   tsCanTimeS.ulSec1970 = 8981;
   CpMsgSetTime(&tsCanMsgS, &tsCanTimeS);
   TEST_ASSERT_EQUAL(3487, (CpMsgGetTime(&tsCanMsgS))->ulNanoSec);
   TEST_ASSERT_EQUAL(8981, (CpMsgGetTime(&tsCanMsgS))->ulSec1970);

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CEFF);
   tsCanTimeS.ulNanoSec = 34;
   tsCanTimeS.ulSec1970 = 55;
   CpMsgSetTime(&tsCanMsgS, &tsCanTimeS);
   TEST_ASSERT_EQUAL(34, (CpMsgGetTime(&tsCanMsgS))->ulNanoSec);
   TEST_ASSERT_EQUAL(55, (CpMsgGetTime(&tsCanMsgS))->ulSec1970);
   UnityPrint(" CP_MSG_CCF_005: PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_006
**
** The cases shall check the correct behaviour of setting the RTR-Bit in a
** Can Message Frame using the CpMsgSetRemote() function. The Cases also will
** check if the RTS-bit is set or not, by calling the CpMsgIsRemote() function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, 006)
{

   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CBFF);
   CpMsgSetRemote(&tsCanMsgS);
   TEST_ASSERT_TRUE(CpMsgIsRemote(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CBFF);
   TEST_ASSERT_FALSE(CpMsgIsRemote(&tsCanMsgS));
   UnityPrint(" CP_MSG_CCF_006: PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_007
**
** The cases shall check the correct behaviour of setting and checking the
** overrun behaviour of an Can Message Frame using the CpMsgSetOverrun()
** and CpMsgIsOverrun() function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, 007)
{

   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CBFF);
   CpMsgSetOverrun(&tsCanMsgS);
   TEST_ASSERT_TRUE(CpMsgIsOverrun(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_CBFF);
   TEST_ASSERT_FALSE(CpMsgIsOverrun(&tsCanMsgS));
   UnityPrint(" CP_MSG_CCF_007: PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_D01
**
** This test cases shall verify the correct field values of the CAN message
** structure CpCanMsg_s using the CpMsgClear() function.
** Test of deprecated functions.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, D01)
{
   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgClear(&tsCanMsgS);
   TEST_ASSERT_EQUAL_UINT16(0, CpMsgGetStdId(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT32(0, CpMsgGetExtId(&tsCanMsgS));
   TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetDlc(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgSetStdId(&tsCanMsgS, 0xE04);
   CpMsgSetExtId(&tsCanMsgS, 0x2000E04);
   CpMsgSetDlc(&tsCanMsgS, 0x07);

   CpMsgClear(&tsCanMsgS);

   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetStdId(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetExtId(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetDlc(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetStdId(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest03
   //
   memset(&tsCanMsgS, 0xFF, sizeof(CpCanMsg_ts));
   CpMsgClear(&tsCanMsgS);
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetStdId(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetExtId(&tsCanMsgS));
   TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetDlc(&tsCanMsgS));
   UnityPrint(" CP_MSG_CCF_D01: PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_D02
**
** The cases shall check the correct behavior of setting and getting a
** 11-Bit identifier using the CpMsgSetStdId() and  CpMsgIsExtended() function.
** Test of deprecated functions.
** 
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, D02)
{
   //----------------------------------------------------------------
   // @SubTest01
   //
   uint16_t uwCountS;
   CpMsgClear(&tsCanMsgS);
   for (uwCountS= 0x0000; uwCountS <= 0x07FF; uwCountS++)
   {
      CpMsgSetStdId(&tsCanMsgS, uwCountS);
      TEST_ASSERT_EQUAL_UINT16(uwCountS, CpMsgGetStdId(&tsCanMsgS));
      TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgClear(&tsCanMsgS);
   for (uwCountS = 0x0FF0; uwCountS < 0xFEFF ; uwCountS++)
   {
      CpMsgSetStdId(&tsCanMsgS, uwCountS);
      TEST_ASSERT_EQUAL_UINT16(uwCountS & CP_MASK_STD_FRAME,
                               CpMsgGetStdId(&tsCanMsgS));
      TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest03
   //
   CpMsgClear(&tsCanMsgS);
   for (uwCountS = 0xFFFF; uwCountS >= 0xFF00; uwCountS--) //0xF800
   {
      CpMsgSetStdId(&tsCanMsgS, uwCountS);
      TEST_ASSERT_EQUAL_UINT16(uwCountS & CP_MASK_STD_FRAME,
                               CpMsgGetStdId(&tsCanMsgS));
      TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
   }
   UnityPrint(" CP_MSG_CCF_D02: PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_D03
**
** The cases shall check the correct behavior of setting and getting the
** extended 29-Bit identifier using the CpMsgSetExtId()and CpMsgGetExtId()
** function.
** Test of deprecated functions.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_CCF, D03)
{

   //----------------------------------------------------------------
   // @SubTest01
   //
   uint8_t ubCountS;
   uint16_t uwCountS;
   uint32_t ulCountS;
   CpMsgClear(&tsCanMsgS);
   for (uwCountS= 0x00; uwCountS<= 0x7FF; uwCountS++)
   {
      CpMsgSetExtId(&tsCanMsgS, uwCountS);
      TEST_ASSERT_EQUAL_UINT32(uwCountS, CpMsgGetExtId(&tsCanMsgS));
      TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest02
   //
   ulCountS = 0x4000000;
   CpMsgClear(&tsCanMsgS);
   for (ubCountS= 0; ubCountS<21; ubCountS++)
   {
      CpMsgSetExtId(&tsCanMsgS, ulCountS);
      TEST_ASSERT_EQUAL_UINT32(ulCountS, CpMsgGetExtId(&tsCanMsgS));
      TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
      ulCountS +=0x1000000;
   }

   //----------------------------------------------------------------
   // @SubTest03
   //
   ulCountS = 0x8000000;
   CpMsgClear(&tsCanMsgS);
   for (ubCountS= 0; ubCountS<21; ubCountS++)
   {
      CpMsgSetExtId(&tsCanMsgS, ulCountS);
      TEST_ASSERT_EQUAL_UINT32(ulCountS, CpMsgGetExtId(&tsCanMsgS));
      TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
      ulCountS +=0x1000000;
   }

   //----------------------------------------------------------------
   // @SubTest04
   //
   CpMsgSetExtId(&tsCanMsgS, 0xC0000000);
   TEST_ASSERT_EQUAL_UINT32(0, CpMsgGetExtId(&tsCanMsgS));
   TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
   CpMsgSetStdId(&tsCanMsgS, ubCountS);
   TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
   UnityPrint(" CP_MSG_CCF_D03: PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
// TEST_GROUP_RUNNER()                                                        //
// execute all test cases                                                     //
//----------------------------------------------------------------------------//
TEST_GROUP_RUNNER(CP_MSG_CCF)
{
   UnityPrint("--- Run test group: CP_MSG_CCF -------------------------------");
   printf("\n");
   RUN_TEST_CASE(CP_MSG_CCF, 001);
   RUN_TEST_CASE(CP_MSG_CCF, 002);
   RUN_TEST_CASE(CP_MSG_CCF, 003);
   RUN_TEST_CASE(CP_MSG_CCF, 004);
   RUN_TEST_CASE(CP_MSG_CCF, 005);
   RUN_TEST_CASE(CP_MSG_CCF, 006);
   RUN_TEST_CASE(CP_MSG_CCF, 007);
   RUN_TEST_CASE(CP_MSG_CCF, D01);
   RUN_TEST_CASE(CP_MSG_CCF, D02);
   RUN_TEST_CASE(CP_MSG_CCF, D03);
   printf("\n");
}
