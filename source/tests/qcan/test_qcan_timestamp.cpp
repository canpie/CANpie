//============================================================================//
// File:          test:qcan_frame.cpp                                         //
// Description:   QCAN classes - Test QCan frame                              //
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


#include "test_qcan_timestamp.hpp"


TestQCanTimestamp::TestQCanTimestamp()
{

}


TestQCanTimestamp::~TestQCanTimestamp()
{

}


//----------------------------------------------------------------------------//
// initTestCase()                                                             //
// prepare test cases                                                         //
//----------------------------------------------------------------------------//
void TestQCanTimestamp::initTestCase()
{
   pclTimestampA = new QCanTimeStamp();
   pclTimestampB = new QCanTimeStamp(1, 1000);
}


//----------------------------------------------------------------------------//
// checkValueRange()                                                          //
// check value range                                                          //
//----------------------------------------------------------------------------//
void TestQCanTimestamp::checkValueRange()
{
   //----------------------------------------------------------------
   // default value of Timestamp A is (0,0)
   //
   QVERIFY(pclTimestampA->seconds() == 0);
   QVERIFY(pclTimestampA->nanoSeconds() == 0);
   QVERIFY(pclTimestampA->isValid() == true);
   
   //----------------------------------------------------------------
   // default value of Timestamp B is (1,1000)
   //
   QVERIFY(pclTimestampB->seconds() == 1);
   QVERIFY(pclTimestampB->nanoSeconds() == 1000);
   QVERIFY(pclTimestampB->isValid() == true);

   //----------------------------------------------------------------
   // try to set invalid values
   //
   pclTimestampA->setSeconds(0xFFFFFFFF);
   QVERIFY(pclTimestampA->seconds()     == TIME_STAMP_SECS_LIMIT);
   QVERIFY(pclTimestampA->nanoSeconds() == 0);
   QVERIFY(pclTimestampA->isValid() == true);
   
   pclTimestampA->setNanoSeconds(0xFFFFFFFF);
   QVERIFY(pclTimestampA->seconds()     == TIME_STAMP_SECS_LIMIT);
   QVERIFY(pclTimestampA->nanoSeconds() == TIME_STAMP_NSEC_LIMIT);
   QVERIFY(pclTimestampA->isValid() == true);
   
   
   //----------------------------------------------------------------
   // clear time-stamp A
   //
   pclTimestampA->clear();
   QVERIFY(pclTimestampA->seconds() == 0);
   QVERIFY(pclTimestampA->nanoSeconds() == 0);
   QVERIFY(pclTimestampA->isValid() == true);


   //----------------------------------------------------------------
   // set new values in time-stamp B
   //
   pclTimestampB->setSeconds(2);
   pclTimestampB->setNanoSeconds(2000);
   QVERIFY(pclTimestampB->seconds()     == 2);
   QVERIFY(pclTimestampB->nanoSeconds() == 2000);
   QVERIFY(pclTimestampB->isValid() == true);

}


//----------------------------------------------------------------------------//
// checkConversion()                                                          //
// check conversions from other units                                         //
//----------------------------------------------------------------------------//
void TestQCanTimestamp::checkConversion()
{
   //----------------------------------------------------------------
   // set 5005 micro-seconds
   //
   pclTimestampA->fromMicroSeconds(5005);
   QVERIFY(pclTimestampA->seconds()     == 0);
   QVERIFY(pclTimestampA->nanoSeconds() == 5005000);
   
   //----------------------------------------------------------------
   // set 999999 micro-seconds
   //
   pclTimestampA->fromMicroSeconds(999999);
   QVERIFY(pclTimestampA->seconds()     == 0);
   QVERIFY(pclTimestampA->nanoSeconds() == 999999000);
   
   //----------------------------------------------------------------
   // set 1000000 micro-seconds
   //
   pclTimestampA->fromMicroSeconds(1000000);
   QVERIFY(pclTimestampA->seconds()     == 1);
   QVERIFY(pclTimestampA->nanoSeconds() == 0);

   //----------------------------------------------------------------
   // set 5000005 micro-seconds
   //
   pclTimestampA->fromMicroSeconds(5000005);
   QVERIFY(pclTimestampA->seconds()     == 5);
   QVERIFY(pclTimestampA->nanoSeconds() == 5000);

   //----------------------------------------------------------------
   // set 4294967295 micro-seconds
   //
   pclTimestampA->fromMicroSeconds(4294967295);
   QVERIFY(pclTimestampA->seconds()     == 4294);
   QVERIFY(pclTimestampA->nanoSeconds() == 967295000);
   

   //----------------------------------------------------------------
   // set 505 milli-seconds
   //
   pclTimestampA->fromMilliSeconds(505);
   QVERIFY(pclTimestampA->seconds()     == 0);
   QVERIFY(pclTimestampA->nanoSeconds() == 505000000);
   
   //----------------------------------------------------------------
   // set 999 milli-seconds
   //
   pclTimestampA->fromMilliSeconds(999);
   QVERIFY(pclTimestampA->seconds()     == 0);
   QVERIFY(pclTimestampA->nanoSeconds() == 999000000);
   
   //----------------------------------------------------------------
   // set 1001 milli-seconds
   //
   pclTimestampA->fromMilliSeconds(1001);
   QVERIFY(pclTimestampA->seconds()     == 1);
   QVERIFY(pclTimestampA->nanoSeconds() == 1000000);

   //----------------------------------------------------------------
   // set 3200015 milli-seconds
   //
   pclTimestampA->fromMilliSeconds(3200156);
   QVERIFY(pclTimestampA->seconds()     == 3200);
   QVERIFY(pclTimestampA->nanoSeconds() == 156000000);

   //----------------------------------------------------------------
   // set 4294967295 milli-seconds
   //
   pclTimestampA->fromMilliSeconds(4294967295);
   QVERIFY(pclTimestampA->seconds()     == 4294967);
   QVERIFY(pclTimestampA->nanoSeconds() == 295000000);
   
}

//----------------------------------------------------------------------------//
// checkOperatorCompare()                                                     //
// check comparision between time-stamp values                                //
//----------------------------------------------------------------------------//
void TestQCanTimestamp::checkOperatorCompare()
{
   //----------------------------------------------------------------
   // time-stamp A and B are equal
   //
   pclTimestampA->setSeconds(1);
   pclTimestampA->setNanoSeconds(9009);

   pclTimestampB->setSeconds(1);
   pclTimestampB->setNanoSeconds(9009);

   QVERIFY((*pclTimestampA == *pclTimestampB) == true);
   QVERIFY((*pclTimestampA != *pclTimestampB) == false);
   QVERIFY((*pclTimestampA <  *pclTimestampB) == false);
   QVERIFY((*pclTimestampA <= *pclTimestampB) == true);
   QVERIFY((*pclTimestampA >  *pclTimestampB) == false);
   QVERIFY((*pclTimestampA >= *pclTimestampB) == true);

   //----------------------------------------------------------------
   // time-stamp A is smaller than B 
   //
   pclTimestampA->setSeconds(1);
   pclTimestampA->setNanoSeconds(9009);

   pclTimestampB->setSeconds(1);
   pclTimestampB->setNanoSeconds(9099);

   QVERIFY((*pclTimestampA == *pclTimestampB) == false);
   QVERIFY((*pclTimestampA != *pclTimestampB) == true);
   QVERIFY((*pclTimestampA <  *pclTimestampB) == true);
   QVERIFY((*pclTimestampA <= *pclTimestampB) == true);
   QVERIFY((*pclTimestampA >  *pclTimestampB) == false);
   QVERIFY((*pclTimestampA >= *pclTimestampB) == false);

   //----------------------------------------------------------------
   // time-stamp A is smaller than B 
   //
   pclTimestampA->setSeconds(70);
   pclTimestampA->setNanoSeconds(155000);

   pclTimestampB->setSeconds(71);
   pclTimestampB->setNanoSeconds(155000);

   QVERIFY((*pclTimestampA == *pclTimestampB) == false);
   QVERIFY((*pclTimestampA != *pclTimestampB) == true);
   QVERIFY((*pclTimestampA <  *pclTimestampB) == true);
   QVERIFY((*pclTimestampA <= *pclTimestampB) == true);
   QVERIFY((*pclTimestampA >  *pclTimestampB) == false);
   QVERIFY((*pclTimestampA >= *pclTimestampB) == false);

}


//----------------------------------------------------------------------------//
// checkOperatorPlus()                                                        //
// add time-stamp values                                                      //
//----------------------------------------------------------------------------//
void TestQCanTimestamp::checkOperatorPlus()
{
   QCanTimeStamp clResultT;
   
   //----------------------------------------------------------------
   // add time-stamp A and B 
   //
   pclTimestampA->setSeconds(1);
   pclTimestampA->setNanoSeconds(999999000);

   pclTimestampB->setSeconds(0);
   pclTimestampB->setNanoSeconds(1000);   // 1 us
   
   clResultT = (*pclTimestampA) + (*pclTimestampB);
   QVERIFY(clResultT.seconds()     == 2);
   QVERIFY(clResultT.nanoSeconds() == 0);
   QVERIFY(clResultT.isValid() == true);
   
   //----------------------------------------------------------------
   // time-stamp B is 100 ms 
   //
   pclTimestampB->setSeconds(0);
   pclTimestampB->setNanoSeconds(100000000);
   
   for(int8_t ubCntT = 0; ubCntT < 25; ubCntT++)
   {
      //--------------------------------------------------------
      // add 25 times the value 100 ms to 2.0 secs
      //
      clResultT += (*pclTimestampB);
   }
   
   QVERIFY(clResultT.seconds()     == 4);
   QVERIFY(clResultT.nanoSeconds() == 500000000);
   QVERIFY(clResultT.isValid() == true);
   
   
   //----------------------------------------------------------------
   // time-stamp A is 17,345813268 sec 
   // time-stamp B is 38,653272134 sec 
   //
   pclTimestampA->setSeconds(17);
   pclTimestampA->setNanoSeconds(345813268);
   pclTimestampB->setSeconds(38);
   pclTimestampB->setNanoSeconds(653272134);
   
   clResultT = (*pclTimestampA) + (*pclTimestampB);
   QVERIFY(clResultT.seconds()     == 55);
   QVERIFY(clResultT.nanoSeconds() == 999085402);
   QVERIFY(clResultT.isValid() == true);

   //----------------------------------------------------------------
   // time-stamp A is TIME_STAMP_SECS_LIMIT sec 
   // time-stamp B is TIME_STAMP_NSEC_LIMIT nano-sec 
   //
   pclTimestampA->setSeconds(TIME_STAMP_SECS_LIMIT);
   pclTimestampA->setNanoSeconds(0);
   pclTimestampB->setSeconds(0);
   pclTimestampB->setNanoSeconds(TIME_STAMP_NSEC_LIMIT);
   
   clResultT = (*pclTimestampA) + (*pclTimestampB);
   QVERIFY(clResultT.seconds()     == TIME_STAMP_SECS_LIMIT);
   QVERIFY(clResultT.nanoSeconds() == TIME_STAMP_NSEC_LIMIT);
   QVERIFY(clResultT.isValid() == true);
   
   //----------------------------------------------------------------
   // time-stamp B is 1 nano-sec 
   //
   clResultT += (*pclTimestampB);
   QVERIFY(clResultT.seconds()     == TIME_STAMP_INVALID_VALUE);
   QVERIFY(clResultT.nanoSeconds() == TIME_STAMP_INVALID_VALUE);
   QVERIFY(clResultT.isValid() == false);
   
}


//----------------------------------------------------------------------------//
// checkOperatorMinus()                                                       //
// substract time-stamp values                                                //
//----------------------------------------------------------------------------//
void TestQCanTimestamp::checkOperatorMinus()
{
   QCanTimeStamp clResultT;
   
   //----------------------------------------------------------------
   // substract time-stamp A (1.0 sec) and B (0.1 sec) 
   //
   pclTimestampA->setSeconds(1);
   pclTimestampA->setNanoSeconds(0);

   pclTimestampB->setSeconds(0);
   pclTimestampB->setNanoSeconds(100000000);   // 100 ms
   
   //----------------------------------------------------------------
   // B - A should fail
   //
   clResultT = (*pclTimestampB) - (*pclTimestampA);
   QVERIFY(clResultT.seconds()     == TIME_STAMP_INVALID_VALUE);
   QVERIFY(clResultT.nanoSeconds() == TIME_STAMP_INVALID_VALUE);
   QVERIFY(clResultT.isValid() == false);
   
   //----------------------------------------------------------------
   // A - B = 0.9 sec
   //
   clResultT = (*pclTimestampA) - (*pclTimestampB);
   QVERIFY(clResultT.seconds()     == 0);
   QVERIFY(clResultT.nanoSeconds() == 900000000);
   QVERIFY(clResultT.isValid() == true);

   //----------------------------------------------------------------
   // substract time-stamp A (12.125 sec) and B (11.250 sec) 
   //
   pclTimestampA->setSeconds(12);
   pclTimestampA->setNanoSeconds(125000000);

   pclTimestampB->setSeconds(11);
   pclTimestampB->setNanoSeconds(250000000);   // 250 ms
   
   //----------------------------------------------------------------
   // B - A should fail
   //
   clResultT = (*pclTimestampB) - (*pclTimestampA);
   QVERIFY(clResultT.seconds()     == TIME_STAMP_INVALID_VALUE);
   QVERIFY(clResultT.nanoSeconds() == TIME_STAMP_INVALID_VALUE);
   QVERIFY(clResultT.isValid() == false);

   //----------------------------------------------------------------
   // A - B = 0.9 sec
   //
   clResultT = (*pclTimestampA) - (*pclTimestampB);
   QVERIFY(clResultT.seconds()     == 0);
   QVERIFY(clResultT.nanoSeconds() == 875000000);
   QVERIFY(clResultT.isValid() == true);
   
   (*pclTimestampA) -= (*pclTimestampB);
   QVERIFY(pclTimestampA->seconds()     == 0);
   QVERIFY(pclTimestampA->nanoSeconds() == 875000000);
   QVERIFY(pclTimestampA->isValid() == true);
   

   //----------------------------------------------------------------
   // substract time-stamp A (44.888777 sec) and B (22.444333 sec) 
   //
   pclTimestampA->setSeconds(44);
   pclTimestampA->setNanoSeconds(888777000);

   pclTimestampB->setSeconds(22);
   pclTimestampB->setNanoSeconds(444333000);
   
   //----------------------------------------------------------------
   // B - A should fail
   //
   clResultT = (*pclTimestampB) - (*pclTimestampA);
   QVERIFY(clResultT.seconds()     == TIME_STAMP_INVALID_VALUE);
   QVERIFY(clResultT.nanoSeconds() == TIME_STAMP_INVALID_VALUE);
   QVERIFY(clResultT.isValid() == false);

   //----------------------------------------------------------------
   // A - B = 22.444444 sec
   //
   clResultT = (*pclTimestampA) - (*pclTimestampB);
   QVERIFY(clResultT.seconds()     == 22);
   QVERIFY(clResultT.nanoSeconds() == 444444000);
   QVERIFY(clResultT.isValid() == true);
   
   //----------------------------------------------------------------
   // substract own value
   //
   (*pclTimestampA) -= (*pclTimestampA);
   QVERIFY(pclTimestampA->seconds()     == 0);
   QVERIFY(pclTimestampA->nanoSeconds() == 0);
   QVERIFY(pclTimestampA->isValid() == true);
   
   
   
}


//----------------------------------------------------------------------------//
// cleanupTestCase()                                                          //
// cleanup test cases                                                         //
//----------------------------------------------------------------------------//
void TestQCanTimestamp::cleanupTestCase()
{
   delete(pclTimestampA);
   delete(pclTimestampB);
}


