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


#include "test_qcan_data.hpp"


#define  DATA_TEST_VALUE         ((uint8_t)  0xB0)
#define  DLC_TEST_VALUE          ((uint8_t)  0x05)
#define  ID_TEST_VALUE           ((uint32_t) 0x00000405)
#define  MARKER_TEST_VALUE       ((uint32_t) 0x98765432)
#define  TIME_STAMP_TEST_VALUE   ((uint32_t) 10245)
#define  USER_TEST_VALUE         ((uint32_t) 0xAB64281F)
#define  API_TEST_VALUE          "QCan driver"

TestQCanData::TestQCanData()
{

}


TestQCanData::~TestQCanData()
{

}


//----------------------------------------------------------------------------//
// initTestCase()                                                             //
// prepare test cases                                                         //
//----------------------------------------------------------------------------//
void TestQCanData::initTestCase()
{
   pclCanDataP = new QCanData(QCanData::eTYPE_CAN);

   //----------------------------------------------------------------
   // setup classical CAN frame 
   //
   QCanTimeStamp  clTimeStampT;
   clTimeStampT.setNanoSeconds(TIME_STAMP_TEST_VALUE);
   clTimeStampT.setSeconds(TIME_STAMP_TEST_VALUE);
   pclCanFrameP = new QCanFrame(QCanFrame::eFORMAT_CAN_STD, ID_TEST_VALUE);
   pclCanFrameP->setDlc(DLC_TEST_VALUE);
   for (uint8_t ubCntT = 0; ubCntT < DLC_TEST_VALUE; ubCntT++)
   {
      pclCanFrameP->setData(ubCntT, DATA_TEST_VALUE + ubCntT);
   }
   pclCanFrameP->setMarker(MARKER_TEST_VALUE);
   pclCanFrameP->setTimeStamp(clTimeStampT);
   pclCanFrameP->setUser(USER_TEST_VALUE);
   
   //----------------------------------------------------------------
   // setup API frame 
   //
   pclCanApiP = new QCanFrameApi();
   pclCanApiP->setName(API_TEST_VALUE);

   //----------------------------------------------------------------
   // setup Error frame 
   //
   pclCanErrorP = new QCanFrameError();
   pclCanErrorP->setErrorState(eCAN_STATE_BUS_WARN);
   pclCanErrorP->setErrorCounterReceive(96);
   
}


//----------------------------------------------------------------------------//
// checkFrameType()                                                           //
// check frame types                                                          //
//----------------------------------------------------------------------------//
void TestQCanData::checkFrameType()
{
   QVERIFY(pclCanDataP->frameType() == QCanData::eTYPE_CAN);
  
   
}

//----------------------------------------------------------------------------//
// checkFrameType()                                                           //
// check frame types                                                          //
//----------------------------------------------------------------------------//
void TestQCanData::checkConversion()
{
   QByteArray     clByteArrayT;
   QCanFrame      clCanFrameCheckT;
   QCanFrameApi   clCanApiCheckT;
   QCanFrameError clCanErrorCheckT;
   
   //----------------------------------------------------------------
   // convert CAN frame to byte array 
   //   
   clByteArrayT = pclCanFrameP->toByteArray();

   //----------------------------------------------------------------
   // test conversion to non-compliant classes 
   //   
   QVERIFY(clCanApiCheckT.fromByteArray(clByteArrayT)   == false);
   QVERIFY(clCanErrorCheckT.fromByteArray(clByteArrayT) == false);

   //----------------------------------------------------------------
   // test conversion to correct class 
   //   
   QVERIFY(clCanFrameCheckT.fromByteArray(clByteArrayT) == true);
   
   
   //----------------------------------------------------------------
   // convert API frame to byte array 
   //   
   clByteArrayT = pclCanApiP->toByteArray();

   //----------------------------------------------------------------
   // test conversion to non-compliant classes 
   //   
   QVERIFY(clCanFrameCheckT.fromByteArray(clByteArrayT) == false);
   QVERIFY(clCanErrorCheckT.fromByteArray(clByteArrayT) == false);

   //----------------------------------------------------------------
   // test conversion to correct class 
   //   
   QVERIFY(clCanApiCheckT.fromByteArray(clByteArrayT)   == true);

   
   //----------------------------------------------------------------
   // convert error frame to byte array 
   //   
   clByteArrayT = pclCanErrorP->toByteArray();

   //----------------------------------------------------------------
   // test conversion to non-compliant classes 
   //   
   QVERIFY(clCanApiCheckT.fromByteArray(clByteArrayT)   == false);
   QVERIFY(clCanFrameCheckT.fromByteArray(clByteArrayT) == false);

   //----------------------------------------------------------------
   // test conversion to correct class 
   //   
   QVERIFY(clCanErrorCheckT.fromByteArray(clByteArrayT) == true);

}


//----------------------------------------------------------------------------//
// checkFrameData()                                                           //
// check frame data                                                           //
//----------------------------------------------------------------------------//
void TestQCanData::checkByteArray()
{
   QByteArray  clByteArrayT;


   //----------------------------------------------------------------
   // convert CAN frame to byte array 
   //   
   clByteArrayT = pclCanFrameP->toByteArray();
   
   //----------------------------------------------------------------
   // convert to QCanData and check the frame type
   //
   pclCanDataP->fromByteArray(clByteArrayT);
   QVERIFY(pclCanDataP->frameType() == QCanData::eTYPE_CAN);
   
   //----------------------------------------------------------------
   // convert back to QCanFrame and check the contents
   //
   QCanFrame      clCanFrameCheckT;
   clCanFrameCheckT.fromByteArray(clByteArrayT);
   QVERIFY(clCanFrameCheckT.identifier() == ID_TEST_VALUE);
   QVERIFY(clCanFrameCheckT.dlc()        == DLC_TEST_VALUE);
   QVERIFY(clCanFrameCheckT.marker()     == MARKER_TEST_VALUE);  
   QVERIFY(clCanFrameCheckT.user()       == USER_TEST_VALUE);
   
   QVERIFY(clCanFrameCheckT.timeStamp().seconds()     == TIME_STAMP_TEST_VALUE);    
   QVERIFY(clCanFrameCheckT.timeStamp().nanoSeconds() == TIME_STAMP_TEST_VALUE);    
   
   
   //----------------------------------------------------------------
   // convert API frame to byte array 
   //   
   clByteArrayT = pclCanApiP->toByteArray();
   
   //----------------------------------------------------------------
   // convert to QCanData and check the frame type
   //
   pclCanDataP->fromByteArray(clByteArrayT);
   QVERIFY(pclCanDataP->frameType() == QCanData::eTYPE_API);
   
   //----------------------------------------------------------------
   // convert back to QCanFrameApi and check the contents
   //
   QCanFrameApi      clCanApiCheckT;
   clCanApiCheckT.fromByteArray(clByteArrayT);
   QString  clNameT;
   QVERIFY(clCanApiCheckT.name(clNameT) == true);
   QCOMPARE(clNameT, QString(API_TEST_VALUE));
   
   //----------------------------------------------------------------
   // convert Error frame to byte array 
   //   
   clByteArrayT = pclCanErrorP->toByteArray();
   
   //----------------------------------------------------------------
   // convert to QCanData and check the frame type
   //
   pclCanDataP->fromByteArray(clByteArrayT);
   QVERIFY(pclCanDataP->frameType() == QCanData::eTYPE_ERROR);
   
   //----------------------------------------------------------------
   // convert back to QCanFrameError and check the contents
   //
   QCanFrameError    clCanErrorCheckT;
   clCanErrorCheckT.fromByteArray(clByteArrayT);
   QVERIFY(clCanErrorCheckT.errorState() == eCAN_STATE_BUS_WARN);
   QVERIFY(clCanErrorCheckT.errorCounterReceive() == 96);
   
}


//----------------------------------------------------------------------------//
// cleanupTestCase()                                                          //
// cleanup test cases                                                         //
//----------------------------------------------------------------------------//
void TestQCanData::cleanupTestCase()
{
   delete(pclCanDataP);
}


