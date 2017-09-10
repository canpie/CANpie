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


#include "test_qcan_frame.hpp"


TestQCanFrame::TestQCanFrame()
{

}


TestQCanFrame::~TestQCanFrame()
{

}


//----------------------------------------------------------------------------//
// initTestCase()                                                             //
// prepare test cases                                                         //
//----------------------------------------------------------------------------//
void TestQCanFrame::initTestCase()
{
   pclCanStdP = new QCanFrame(QCanFrame::eFORMAT_CAN_STD);
   pclCanExtP = new QCanFrame(QCanFrame::eFORMAT_CAN_EXT);
   pclFdStdP  = new QCanFrame(QCanFrame::eFORMAT_FD_STD);
   pclFdExtP  = new QCanFrame(QCanFrame::eFORMAT_FD_EXT);

   pclFrameP  = new QCanFrame();
}


//----------------------------------------------------------------------------//
// checkFrameType()                                                           //
// check frame types                                                          //
//----------------------------------------------------------------------------//
void TestQCanFrame::checkFrameType()
{
   QVERIFY(pclCanStdP->frameFormat() == QCanFrame::eFORMAT_CAN_STD);
   QVERIFY(pclCanExtP->frameFormat() == QCanFrame::eFORMAT_CAN_EXT);
   QVERIFY(pclFdStdP->frameFormat()  == QCanFrame::eFORMAT_FD_STD);
   QVERIFY(pclFdExtP->frameFormat()  == QCanFrame::eFORMAT_FD_EXT);
  
   
}


//----------------------------------------------------------------------------//
// checkFrameId()                                                             //
// check frame identifier                                                     //
//----------------------------------------------------------------------------//
void TestQCanFrame::checkFrameId()
{
   uint32_t ulIdValueT;

   for(ulIdValueT = 0; ulIdValueT <= 0x0000FFFF; ulIdValueT++)
   {
      pclCanStdP->setIdentifier(ulIdValueT);
      pclCanExtP->setIdentifier(ulIdValueT);

      *pclFrameP = *pclCanStdP;
      if(ulIdValueT <= 0x07FF)
      {
         QVERIFY(pclCanStdP->identifier() == ulIdValueT);
         QVERIFY(pclFrameP->identifier()  == ulIdValueT);
      }
      else
      {
         QVERIFY(pclCanStdP->identifier() == (ulIdValueT & 0x07FF));
         QVERIFY(pclFrameP->identifier()  == (ulIdValueT & 0x07FF));
      }
      QVERIFY(pclCanExtP->identifier() == ulIdValueT);

      QVERIFY(pclCanStdP->isExtended() == 0);
      QVERIFY(pclFrameP->isExtended()  == 0);
   }
}


//----------------------------------------------------------------------------//
// checkFrameDlc()                                                            //
// check frame DLC                                                            //
//----------------------------------------------------------------------------//
void TestQCanFrame::checkFrameDlc()
{
   uint8_t  ubSizeT;
   
   //----------------------------------------------------------------
   // Test 1: set the DLC value for classic CAN and CAN FD and
   // check the result for the dataSize() return
   //
   for(ubSizeT = 0; ubSizeT < 16; ubSizeT++)
   {
      pclCanStdP->setDlc(ubSizeT);
      pclFdStdP->setDlc(ubSizeT);
      
      //--------------------------------------------------------
      // check data size
      //
      if(ubSizeT < 9)
      {
         QVERIFY(pclCanStdP->dataSize() == ubSizeT);
         QVERIFY(pclFdStdP->dataSize() == ubSizeT);
      }
      else
      {
         //------------------------------------------------
         // copy FD frame contents and convert type to
         // classic CAN
         //
         *pclFrameP = *pclFdStdP;
         pclFrameP->setFrameFormat(QCanFrame::eFORMAT_CAN_STD);
         
         //------------------------------------------------
         // make sure maximum dataSize of classic CAN is 8
         //
         QVERIFY(pclFrameP->dataSize()  == 8);
         QVERIFY(pclCanStdP->dataSize() == 8);
         
         //------------------------------------------------
         // check data size of CAN FD 
         //
         switch(ubSizeT)
         {
            case 9:
               QVERIFY(pclFdStdP->dataSize() == 12);
               break;
               
            case 10:
               QVERIFY(pclFdStdP->dataSize() == 16);
               break;
               
            case 11:
               QVERIFY(pclFdStdP->dataSize() == 20);
               break;
               
            case 12:
               QVERIFY(pclFdStdP->dataSize() == 24);
               break;
               
            case 13:
               QVERIFY(pclFdStdP->dataSize() == 32);
               break;

            case 14:
               QVERIFY(pclFdStdP->dataSize() == 48);
               break;

            case 15:
               QVERIFY(pclFdStdP->dataSize() == 64);
               break;
               
            default:
               
               break;
         }
      }
      
      //--------------------------------------------------------
      // check DLC
      //
      if(ubSizeT < 9)
      {
         QVERIFY(pclCanStdP->dlc() == ubSizeT);
         QVERIFY(pclFdStdP->dlc() == ubSizeT);
      }
      else
      {
         QVERIFY(pclCanStdP->dlc() == 8);
         QVERIFY(pclFdStdP->dlc() == ubSizeT);
      }
   }

   //----------------------------------------------------------------
   // Test 2: set DLC value out of range
   //
   pclCanStdP->setDlc(22);
   pclFdStdP->setDlc(22);
   QVERIFY(pclCanStdP->dlc() <= 8);
   QVERIFY(pclFdStdP->dlc()  <= 15);
      
   

}

void TestQCanFrame::checkFrameDataSize()
{
   uint8_t  ubSizeT;
   
   //----------------------------------------------------------------
   // Test 3: set the data size value for classic CAN and CAN FD and
   // check the result for the dlc() return
   //
   for(ubSizeT = 0; ubSizeT < 70; ubSizeT++)
   {
      pclCanStdP->setDataSize(ubSizeT);
      pclFdStdP->setDataSize(ubSizeT);
      
   }
}

//----------------------------------------------------------------------------//
// checkFrameData()                                                           //
// check frame data                                                           //
//----------------------------------------------------------------------------//
void TestQCanFrame::checkFrameData()
{
   pclCanStdP->setDlc(4);
   pclCanStdP->setData(0, 0x12);
   pclCanStdP->setData(1, 0x34);
   pclCanStdP->setData(2, 0x56);
   pclCanStdP->setData(3, 0x78);
   
   QVERIFY(pclCanStdP->dataUInt16(0, 1) == 0x1234);
   QVERIFY(pclCanStdP->dataUInt16(1, 1) == 0x3456);
   QVERIFY(pclCanStdP->dataUInt16(2, 1) == 0x5678);
   QVERIFY(pclCanStdP->dataUInt16(3, 1) == 0x0000);

   QVERIFY(pclCanStdP->dataUInt32(0, 1) == 0x12345678);

   pclCanStdP->setDataUInt16(0, 0xAABB);
   QVERIFY(pclCanStdP->data(0) == 0xBB);
   QVERIFY(pclCanStdP->data(1) == 0xAA);
   QVERIFY(pclCanStdP->dataUInt16(0, 0) == 0xAABB);

}


//----------------------------------------------------------------------------//
// checkFrameRemote()                                                         //
// check remote frame                                                         //
//----------------------------------------------------------------------------//
void TestQCanFrame::checkFrameRemote()
{
   
}


//----------------------------------------------------------------------------//
// checkFrameData()                                                           //
// check frame data                                                           //
//----------------------------------------------------------------------------//
void TestQCanFrame::checkByteArray()
{
   QByteArray  clByteArrayT;

   for(uint8_t ubCntT = 0; ubCntT < 9; ubCntT++)
   {
      pclCanStdP->setIdentifier(0x301 + ubCntT);
      pclCanStdP->setDlc(ubCntT);
      pclCanStdP->setData(0, ubCntT);
      pclCanStdP->setData(1, 0x10 + ubCntT);
      pclCanStdP->setData(2, 0x20 + ubCntT);
      pclCanStdP->setData(3, 0x30 + ubCntT);
      pclCanStdP->setData(4, 0x40 + ubCntT);
      pclCanStdP->setData(5, 0x50 + ubCntT);
      pclCanStdP->setData(6, 0x60 + ubCntT);
      pclCanStdP->setData(7, 0x70 + ubCntT);

      pclCanStdP->setMarker(0x223344);
      pclCanStdP->setUser(0xAB1023);

      clByteArrayT = pclCanStdP->toByteArray();

      QVERIFY(pclFrameP->fromByteArray(clByteArrayT) == true);

      QVERIFY(pclFrameP->identifier() == (0x301 + ubCntT));
      QVERIFY(pclFrameP->dlc()   == (ubCntT));
      if(ubCntT == 4)
      {
         QVERIFY(pclFrameP->data(0) == (ubCntT));
         QVERIFY(pclFrameP->data(1) == (0x10 + ubCntT));
         QVERIFY(pclFrameP->data(2) == (0x20 + ubCntT));
         QVERIFY(pclFrameP->data(3) == (0x30 + ubCntT));
         QVERIFY(pclFrameP->data(4) == 0);
      }

   }
}

//----------------------------------------------------------------------------//
// cleanupTestCase()                                                          //
// cleanup test cases                                                         //
//----------------------------------------------------------------------------//

void TestQCanFrame::cleanupTestCase()
{
   delete(pclCanStdP);
   delete(pclCanExtP);
   delete(pclFdStdP);
   delete(pclFdExtP);

   delete(pclFrameP);
}


