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


#include "test_qcan_socket.hpp"


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanSocket::TestQCanSocket()                                                                                   //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
TestQCanSocket::TestQCanSocket()
{
   pclSocketP  = new QCanSocket();
   btSkipTestP = false;
}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanSocket::~TestQCanSocket()                                                                                  //
// destructor                                                                                                         //
//--------------------------------------------------------------------------------------------------------------------//
TestQCanSocket::~TestQCanSocket()
{
   delete pclSocketP;
}



//--------------------------------------------------------------------------------------------------------------------//
// TestQCanSocket::initTestCase()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanSocket::initTestCase()
{
   pclSocketP->connectNetwork(eCAN_CHANNEL_1);
   QTest::qWait(500);

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
}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanSocket::checkConnect()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanSocket::checkConnect()
{
   if (btSkipTestP)
   {
      QSKIP("No connection to server available");
   }
   else
   {
      pclSocketP->connectNetwork(eCAN_CHANNEL_1);
      QTest::qWait(500);

      QVERIFY(pclSocketP->isConnected() == true);
      pclSocketP->disconnectNetwork();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanSocket::checkConnectLimit()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanSocket::checkConnectLimit()
{
   if (btSkipTestP)
   {
      QSKIP("No connection to server available");
   }
   else
   {

   }

}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanSocket::checkFrameWrite()                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanSocket::checkFrameWrite()
{
   if (btSkipTestP)
   {
      QSKIP("No connection to server available");
   }
   else
   {
      pclSocketP->connectNetwork(eCAN_CHANNEL_1);
      QTest::qWait(500);

      QVERIFY(pclSocketP->isConnected() == true);

      uint32_t   ulFrameCountT = 10;
      QCanFrame  clCanFrameT;
      while (ulFrameCountT > 0)
      {
         clCanFrameT.setIdentifier(0x100 + ulFrameCountT);
         clCanFrameT.setDlc(4);
         QVERIFY(pclSocketP->write(clCanFrameT) == true);
         ulFrameCountT--;
      }

      QTest::qWait(500);
      pclSocketP->disconnectNetwork();

   }

}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanSocket::checkFrameRead()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanSocket::checkFrameRead()
{
   if (btSkipTestP)
   {
      QSKIP("No connection to server available");
   }
   else
   {

   }

}

void TestQCanSocket::cleanupTestCase()
{

}


