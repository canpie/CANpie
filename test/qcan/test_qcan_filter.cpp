//====================================================================================================================//
// File:          test_qcan_filter.cpp                                                                                 //
// Description:   QCAN classes - Test QCan frame                                                                      //
//                                                                                                                    //
// Copyright (C) MicroControl GmbH & Co. KG                                                                           //
// 53844 Troisdorf - Germany                                                                                          //
// www.microcontrol.net                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the   //
// following conditions are met:                                                                                      //
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions, the following   //
//    disclaimer and the referenced file 'LICENSE'.                                                                   //
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       //
//    following disclaimer in the documentation and/or other materials provided with the distribution.                //
// 3. Neither the name of MicroControl nor the names of its contributors may be used to endorse or promote products   //
//    derived from this software without specific prior written permission.                                           //
//                                                                                                                    //
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance     //
// with the License. You may obtain a copy of the License at                                                          //
//                                                                                                                    //
//    http://www.apache.org/licenses/LICENSE-2.0                                                                      //
//                                                                                                                    //
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed   //
// on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for  //
// the specific language governing permissions and limitations under the License.                                     //
//                                                                                                                    //
//====================================================================================================================//


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <iostream>

using namespace std;

#include "test_qcan_filter.hpp"

#include "cp_msg.h"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#define  TEST_VALUE_ID_STD       ((uint32_t) 0x00000222)
#define  TEST_VALUE_ID_EXT       ((uint32_t) 0x01B20222)


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFilter::TestQCanFilter()                                                                                     //
// frame type and frame format stay the same through all test cases                                                   //
//--------------------------------------------------------------------------------------------------------------------//
TestQCanFilter::TestQCanFilter()
{

}


TestQCanFilter::~TestQCanFilter()
{


}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFilter::initTestCase()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFilter::initTestCase()
{
   clCanStdP.setFrameFormat(QCanFrame::eFORMAT_CAN_STD);
   clCanStdP.setIdentifier(TEST_VALUE_ID_STD);

   clCanExtP.setFrameFormat(QCanFrame::eFORMAT_CAN_EXT);
   clCanExtP.setIdentifier(TEST_VALUE_ID_EXT);

   clFdStdP.setFrameFormat(QCanFrame::eFORMAT_FD_STD);
   clFdStdP.setIdentifier(TEST_VALUE_ID_STD);

   clFdExtP.setFrameFormat(QCanFrame::eFORMAT_FD_EXT);
   clFdExtP.setIdentifier(TEST_VALUE_ID_EXT);

}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFilter::checkFrameAccept()                                                                                 //
// test invalid DLC combinations                                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
void  TestQCanFilter::checkFrameAccept()
{
   QCanFilter     clAcceptT;
   QCanFilterList clFilterListT;

   //---------------------------------------------------------------------------------------------------
   // initial accept filter: 100h .. 120h
   //
   clAcceptT.acceptFrame(QCanFrame::eFORMAT_CAN_STD, 0x100, 0x120);
   clFilterListT.clear();
   clFilterListT.appendFilter(clAcceptT);
   QVERIFY(clFilterListT.filter(clCanStdP) == true);

   //---------------------------------------------------------------------------------------------------
   // test high / low limit
   //
   QVERIFY(clAcceptT.acceptFrame(QCanFrame::eFORMAT_CAN_STD, 0x300, 0x280) == false);

   //---------------------------------------------------------------------------------------------------
   // seconds accept filter
   //
   QVERIFY(clAcceptT.acceptFrame(QCanFrame::eFORMAT_CAN_STD, 0x300, 0x400) == true);
   QVERIFY(clFilterListT.appendFilter(clAcceptT) == 1);
   QVERIFY(clFilterListT.filter(clCanStdP) == true);

   //---------------------------------------------------------------------------------------------------
   // now accept the test identifier
   //
   QVERIFY(clAcceptT.acceptFrame(QCanFrame::eFORMAT_CAN_STD, 0x200, 0x240) == true);
   QVERIFY(clFilterListT.appendFilter(clAcceptT) == 2);
   QVERIFY(clFilterListT.filter(clCanStdP) == false);

}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFilter::checkFrameReject()                                                                                 //
// test invalid DLC combinations                                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
void  TestQCanFilter::checkFrameReject()
{
   QCanFilter     clRejectT;
   QCanFilterList clFilterListT;

   //---------------------------------------------------------------------------------------------------
   // initial reject filter: 100h .. 120h
   //
   QVERIFY(clRejectT.rejectFrame(QCanFrame::eFORMAT_CAN_STD, 0x100, 0x120) == true);
   clFilterListT.clear();
   QVERIFY(clFilterListT.appendFilter(clRejectT) == 0);
   QVERIFY(clFilterListT.filter(clCanStdP) == false);

   //---------------------------------------------------------------------------------------------------
   // test high / low limit
   //
   QVERIFY(clRejectT.rejectFrame(QCanFrame::eFORMAT_CAN_STD, 0x300, 0x280) == false);

   //---------------------------------------------------------------------------------------------------
   // seconds reject filter
   //
   QVERIFY(clRejectT.rejectFrame(QCanFrame::eFORMAT_CAN_STD, 0x300, 0x400) == true);
   QVERIFY(clFilterListT.appendFilter(clRejectT) == 1);
   QVERIFY(clFilterListT.filter(clCanStdP) == false);

   //---------------------------------------------------------------------------------------------------
   // now reject the test identifier
   //
   QVERIFY(clRejectT.rejectFrame(QCanFrame::eFORMAT_CAN_STD, 0x200, 0x230) == true);
   QVERIFY(clFilterListT.appendFilter(clRejectT) == 2);
   QVERIFY(clFilterListT.filter(clCanStdP) == true);

   //---------------------------------------------------------------------------------------------------
   // remove the last filter again
   //
   QVERIFY(clFilterListT.removeFilter(2) == true);
   QVERIFY(clFilterListT.filter(clCanStdP) == false);

   //---------------------------------------------------------------------------------------------------
   // test upper border
   //
   QVERIFY(clRejectT.rejectFrame(QCanFrame::eFORMAT_CAN_STD, 0x200, 0x221) == true);
   QVERIFY(clFilterListT.appendFilter(clRejectT) == 2);
   QVERIFY(clFilterListT.filter(clCanStdP) == false);

   //---------------------------------------------------------------------------------------------------
   // test lower border
   //
   QVERIFY(clRejectT.rejectFrame(QCanFrame::eFORMAT_CAN_STD, 0x223, 0x230) == true);
   QVERIFY(clFilterListT.appendFilter(clRejectT) == 3);
   QVERIFY(clFilterListT.filter(clCanStdP) == false);

   //---------------------------------------------------------------------------------------------------
   // benchmark for 4 filter
   //
   for (uint16_t uwIdentifierT = 0; uwIdentifierT < 0x010; uwIdentifierT++)
   {
      clCanStdP.setIdentifier(uwIdentifierT);
      QBENCHMARK {
         clFilterListT.filter(clCanStdP);
      }

   }

   clCanStdP.setIdentifier(TEST_VALUE_ID_STD);
}



//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFilter::cleanupTestCase()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFilter::cleanupTestCase()
{

}


