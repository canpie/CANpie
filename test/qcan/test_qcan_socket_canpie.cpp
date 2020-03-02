//====================================================================================================================//
// File:          test_qcan_socket.cpp                                                                                //
// Description:   Unit tests for QCanSocket class                                                                     //
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


#include "test_qcan_socket_canpie.hpp"


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanSocketCpFD::TestQCanSocketCpFD()                                                                           //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
TestQCanSocketCpFD::TestQCanSocketCpFD()
{
   btServerIsActiveP = false;
   btHasCanFdPluginP = false;

}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanSocket::~TestQCanSocketCpFD()                                                                              //
// destructor                                                                                                         //
//--------------------------------------------------------------------------------------------------------------------//
TestQCanSocketCpFD::~TestQCanSocketCpFD()
{

}



//--------------------------------------------------------------------------------------------------------------------//
// TestQCanSocketCpFD::initTestCase()                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanSocketCpFD::initTestCase()
{
   //-----------------------
   // Todo: check if server is active and test for CAN plugin
   //
   btServerIsActiveP = true;
   btHasCanFdPluginP = true;

    /*
   if (pclSocketP->isConnected())
   {
      btSkipTestP = false;
      pclSocketP->disconnectNetwork();
   }
   else
   {
      btSkipTestP = true;
      QSKIP("No connection to server available");
   }
   */
}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanSocketCpFD::checkConnect()                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanSocketCpFD::checkConnect()
{
   if (btServerIsActiveP)
   {
      CpStatus_tv tvResultT = eCP_ERR_NONE;
      memset(&tsCanPortP, 0, sizeof(CpPort_ts));
      tvResultT = CpCoreDriverInit(eCP_CHANNEL_1, &tsCanPortP, 0);
      QVERIFY(tvResultT == eCP_ERR_NONE);

      //---------------------------------------------------
      // setup 500 kBit/s
      //
      tvResultT = CpCoreBitrate(&tsCanPortP, eCP_BITRATE_500K, eCP_BITRATE_NONE);
      QVERIFY(tvResultT == eCP_ERR_NONE);

      //---------------------------------------------------
      // start CAN operation
      //
      tvResultT = CpCoreCanMode(&tsCanPortP, eCP_MODE_OPERATION);
      QVERIFY(tvResultT == eCP_ERR_NONE);
   }
   else
   {

   }
}



//--------------------------------------------------------------------------------------------------------------------//
// TestQCanSocketCpFD::checkFrameWrite()                                                                              //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanSocketCpFD::checkFrameWrite()
{
   if (btServerIsActiveP)
   {
      CpStatus_tv tvResultT = eCP_ERR_NONE;
      tvResultT = CpCoreBufferConfig(&tsCanPortP, eCP_BUFFER_1, 0x200, 
                                     CP_MASK_STD_FRAME, CP_MSG_FORMAT_CBFF, eCP_BUFFER_DIR_TRM);
      QVERIFY(tvResultT == eCP_ERR_NONE);

      tvResultT = CpCoreBufferSetDlc(&tsCanPortP, eCP_BUFFER_1, 5);
      QVERIFY(tvResultT == eCP_ERR_NONE);

      tvResultT = CpCoreBufferSend(&tsCanPortP, eCP_BUFFER_1);
      QVERIFY(tvResultT == eCP_ERR_NONE);
      QTest::qWait(100);

      tvResultT = CpCoreBufferConfig(&tsCanPortP, eCP_BUFFER_2, 0x181020EE, 
                                     CP_MASK_EXT_FRAME, CP_MSG_FORMAT_CEFF, eCP_BUFFER_DIR_TRM);
      QVERIFY(tvResultT == eCP_ERR_NONE);

      tvResultT = CpCoreBufferSetDlc(&tsCanPortP, eCP_BUFFER_2, 6);
      QVERIFY(tvResultT == eCP_ERR_NONE);

      tvResultT = CpCoreBufferSend(&tsCanPortP, eCP_BUFFER_2);
      QVERIFY(tvResultT == eCP_ERR_NONE);
      QTest::qWait(100);

   }
   else
   {
      QSKIP("No connection to server available");
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanSocketCpFD::checkFrameRead()                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanSocketCpFD::checkFrameRead()
{
   if (btServerIsActiveP)
   {
      CpStatus_tv tvResultT = eCP_ERR_NONE;
      tvResultT = CpCoreBufferConfig(&tsCanPortP, eCP_BUFFER_5, 0x201, 
                                     0, CP_MSG_FORMAT_CBFF, eCP_BUFFER_DIR_RCV);
      QVERIFY(tvResultT == eCP_ERR_NONE);

      tvResultT = CpCoreBufferSend(&tsCanPortP, eCP_BUFFER_1);
      QVERIFY(tvResultT == eCP_ERR_NONE);
      
      QTest::qWait(200);

      tvResultT = CpCoreBufferSend(&tsCanPortP, eCP_BUFFER_1);
      QVERIFY(tvResultT == eCP_ERR_NONE);
      
      QTest::qWait(2500);

      uint8_t ubDlcValueT = 0;
      tvResultT = CpCoreBufferGetDlc(&tsCanPortP, eCP_BUFFER_5, &ubDlcValueT);
      fprintf(stdout, "Got DLC value %d \n", ubDlcValueT);
      uint8_t aubDataT[8];
      CpCoreBufferGetData(&tsCanPortP, eCP_BUFFER_5, &aubDataT[0], 0, 8);
      fprintf(stdout, "Got data value %02X %02X \n", aubDataT[0], aubDataT[1]);

      QVERIFY(tvResultT   == eCP_ERR_NONE);
      //QVERIFY(ubDlcValueT == 5);


   }
   else
   {
      QSKIP("No connection to server available");
   }

}

void TestQCanSocketCpFD::cleanupTestCase()
{

}


