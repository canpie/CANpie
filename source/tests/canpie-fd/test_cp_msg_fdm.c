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
** Pre-condition                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

/*!
** Support classical CAN only and do not use macros for message
** access.
*/
#define  CP_CAN_FD               1
#define  CP_CAN_MSG_MACRO        1


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
TEST_GROUP(CP_MSG_FDM);             // test group name
static CpCanMsg_ts    tsCanMsgS;    // CAN message
//static CpTime_ts      tsCanTimeS;    // CAN TimeStamp


/*----------------------------------------------------------------------------*\
** Function implementations                                                   **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// TEST_SETUP()                                                               //
// init code for each test case                                               //
//----------------------------------------------------------------------------//
TEST_SETUP(CP_MSG_FDM)
{

}


//----------------------------------------------------------------------------//
// TEST_TEAR_DOWN()                                                           //
// release code for each test case                                            //
//----------------------------------------------------------------------------//
TEST_TEAR_DOWN(CP_MSG_FDM)
{

}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_001
** This test shall check the correct behaviour of initialising a
** CpCanMsg_s and setting the Format to C and E using the CpMsgInit() function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_FDM, 001)
{
   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FBFF);
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetDlc(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetIdentifier(&tsCanMsgS));
   TEST_ASSERT_TRUE(CpMsgIsFastData(&tsCanMsgS));
   TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgSetDlc(&tsCanMsgS, 0x07);
   CpMsgSetIdentifier(&tsCanMsgS, 0x01);
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FBFF);
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetDlc(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetIdentifier(&tsCanMsgS));
   TEST_ASSERT_TRUE(CpMsgIsFastData(&tsCanMsgS));
   TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest03
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FEFF);
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetDlc(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetIdentifier(&tsCanMsgS));
   TEST_ASSERT_TRUE(CpMsgIsFastData(&tsCanMsgS));
   TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest4
   //
   CpMsgSetDlc(&tsCanMsgS, 0x07);
   CpMsgSetIdentifier(&tsCanMsgS, 0x01);
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FEFF);
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetDlc(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0, CpMsgGetIdentifier(&tsCanMsgS));
   TEST_ASSERT_TRUE(CpMsgIsFastData(&tsCanMsgS));
   TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
   UnityPrint("CP_MSG_CCF_001 PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_002
**
** The cases shall check the correct behaviour of setting and getting a
** Identifier (11-bit for Classic CAN or 29-bit for Extended CAN) using the
**  CpMsgSetIdentifier(), CpMsgGetIdentifier() and CpMsgIsExtended() function
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_FDM, 002)
{
   uint32_t ulIdValueT;

   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FBFF);
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
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FBFF);
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
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FEFF);
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
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FEFF);
   for(ulIdValueT = 0x8000000; ulIdValueT <= 0x1C000000; ulIdValueT += 0x400)
   {
      CpMsgSetIdentifier(&tsCanMsgS, ulIdValueT);
      TEST_ASSERT_EQUAL_UINT32(ulIdValueT & CP_MASK_EXT_FRAME,
                               CpMsgGetIdentifier(&tsCanMsgS));
      TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
    }

   //----------------------------------------------------------------
   // @SubTest05
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FEFF);
   for(ulIdValueT = 0x1FF00240; ulIdValueT <=0x10000000; ulIdValueT += 0x800)
   {
      CpMsgSetIdentifier(&tsCanMsgS, ulIdValueT);
      TEST_ASSERT_EQUAL_UINT32(ulIdValueT & CP_MASK_EXT_FRAME,
                               CpMsgGetIdentifier(&tsCanMsgS));
      TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest07
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FEFF);
   for(ulIdValueT = 0x30000000; ulIdValueT <=0xE0000000; ulIdValueT += 0x400)
   {
      CpMsgSetIdentifier(&tsCanMsgS, ulIdValueT);
      TEST_ASSERT_EQUAL_UINT32(ulIdValueT & CP_MASK_EXT_FRAME,
                               CpMsgGetIdentifier(&tsCanMsgS));
      TEST_ASSERT_TRUE(CpMsgIsExtended(&tsCanMsgS));
   }
   UnityPrint("CP_MSG_CCF_002 PASSED");
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
TEST(CP_MSG_FDM, 003)
{
   uint8_t ubDlcT;

   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FBFF);
   for(ubDlcT = 0x00; ubDlcT <= 0x0F; ubDlcT++)
   {
      CpMsgSetDlc(&tsCanMsgS, ubDlcT);
      TEST_ASSERT_EQUAL_UINT8(ubDlcT, CpMsgGetDlc(&tsCanMsgS));
   }

   UnityPrint("CP_MSG_CCF_003 PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_004
**
** The cases shall check the correct behaviour of setting and getting
** the payload using the CpMsgSetData() and CpMsgGetData() function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_FDM, 004)
{
   uint8_t ubPosT;

   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FBFF);
   for (ubPosT = 0x00 ; ubPosT <= 0x3F; ubPosT++)  //FD: 0..63
   {
      CpMsgSetData(&tsCanMsgS, ubPosT, 0xAA);
      TEST_ASSERT_EQUAL_UINT8(0xAA, CpMsgGetData(&tsCanMsgS, ubPosT));
   }

   UnityPrint("CP_MSG_CCF_004 PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_005
**
** The cases shall check the correct behaviour of setting a time-stamp to a
** CAN message and reading/getting a time-stamp from a CAN Message by using
** CpMsgSetTime() and CpMsgGetTime()functions.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_FDM, 005)
{
//   //----------------------------------------------------------------
//   // @SubTest01
//   //
//   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FBFF);
//   tsCanTimeS.ulNanoSec = 5;
//   tsCanTimeS.ulSec1970 = 5;
//   CpMsgSetTime(&tsCanMsgS, &tsCanTimeS);
//   TEST_ASSERT_EQUAL(5, (CpMsgGetTime(&tsCanMsgS))->ulNanoSec);
//   TEST_ASSERT_EQUAL(5, (CpMsgGetTime(&tsCanMsgS))->ulSec1970);

   UnityPrint("CP_MSG_CCF_005 skipped  [ CpMsgSetTime() has to be checked! ]");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_006
**
** BRS
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_FDM, 006)
{

   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FBFF);
   CpMsgSetBitrateSwitch(&tsCanMsgS);
   TEST_ASSERT_TRUE(CpMsgIsBitrateSwitch(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FBFF);
   TEST_ASSERT_FALSE(CpMsgIsBitrateSwitch(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest03
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FEFF);
   CpMsgSetBitrateSwitch(&tsCanMsgS);
   TEST_ASSERT_TRUE(CpMsgIsBitrateSwitch(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest04
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FEFF);
   TEST_ASSERT_FALSE(CpMsgIsBitrateSwitch(&tsCanMsgS));

   UnityPrint("CP_MSG_CCF_006 PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_007
**
** Remote
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_FDM, 007)
{

   //----------------------------------------------------------------
   // @SubTest01
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FBFF);
   CpMsgSetRemote(&tsCanMsgS);
   TEST_ASSERT_FALSE(CpMsgIsRemote(&tsCanMsgS));

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgInit(&tsCanMsgS, CP_MSG_FORMAT_FEFF);
   CpMsgSetRemote(&tsCanMsgS);
   TEST_ASSERT_FALSE(CpMsgIsRemote(&tsCanMsgS));

   UnityPrint("CP_MSG_CCF_006 PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCF_008
**
** The cases shall check the correct behaviour of setting and checking the
** overrun behaviour of an Can Message Frame using the CpMsgSetOverrun()
** and CpMsgIsOverrun() function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_FDM, 008)
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

   UnityPrint("CP_MSG_CCF_007 PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCFD_009
**
** This test cases shall verify the correct field values of the CAN message
** structure CpCanMsg_s using the CpMsgClear() function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_FDM, D009)
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
   TEST_ASSERT_EQUAL_UINT8(0,CpMsgGetExtId(&tsCanMsgS));
   TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
   TEST_ASSERT_EQUAL_UINT8(0,CpMsgGetDlc(&tsCanMsgS));
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCFD_010
**
** The cases shall check the correct behavior of setting and getting a
** 11-Bit identifier using the CpMsgSetStdId() and  CpMsgIsExtended() function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_FDM, D010)
{
   //----------------------------------------------------------------
   // @SubTest01
   //
   uint16_t uwCountS;
   CpMsgClear(&tsCanMsgS);
   for(uwCountS= 0x0000; uwCountS <= 0x07FF;uwCountS++)
   {
      CpMsgSetStdId(&tsCanMsgS, uwCountS);
      TEST_ASSERT_EQUAL_UINT16(uwCountS, CpMsgGetStdId(&tsCanMsgS));
      TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
   }

   //----------------------------------------------------------------
   // @SubTest02
   //
   CpMsgClear(&tsCanMsgS);
   for(uwCountS = 0x0FF0; uwCountS < 0xFEFF ; uwCountS++)
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
   for(uwCountS = 0xFFFF; uwCountS >= 0xFF00; uwCountS--) //0xF800
   {
      CpMsgSetStdId(&tsCanMsgS, uwCountS);
      TEST_ASSERT_EQUAL_UINT16(uwCountS & CP_MASK_STD_FRAME,
                               CpMsgGetStdId(&tsCanMsgS));
      TEST_ASSERT_FALSE(CpMsgIsExtended(&tsCanMsgS));
   }
}


//----------------------------------------------------------------------------//
/*!
** \brief   CP_MSG_CCFD_011
**
** The cases shall check the correct behavior of setting and getting the
** extended 29-Bit identifier using the CpMsgSetExtId()and CpMsgGetExtId()
** function.
*/
//----------------------------------------------------------------------------//
TEST(CP_MSG_FDM, D011)
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
   CpMsgClear(&tsCanMsgS);
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
// TEST_GROUP_RUNNER()                                                        //
// execute all test cases                                                     //
//----------------------------------------------------------------------------//
TEST_GROUP_RUNNER(CP_MSG_FDM)
{
   UnityPrint("*************** RUN  TEST  GROUP  CP_MSG_FDM: ****************");
   printf("\n");
   RUN_TEST_CASE(CP_MSG_FDM, 001);
   RUN_TEST_CASE(CP_MSG_FDM, 002);
   RUN_TEST_CASE(CP_MSG_FDM, 003);
   RUN_TEST_CASE(CP_MSG_FDM, 004);
   RUN_TEST_CASE(CP_MSG_FDM, 005);
   RUN_TEST_CASE(CP_MSG_FDM, 006);
   RUN_TEST_CASE(CP_MSG_FDM, 007);
   RUN_TEST_CASE(CP_MSG_FDM, 008);
   RUN_TEST_CASE(CP_MSG_FDM, D009);
   RUN_TEST_CASE(CP_MSG_FDM, D010);
   RUN_TEST_CASE(CP_MSG_FDM, D011);
   printf("\n");
}
