//====================================================================================================================//
// File:          test_qcan_frame.cpp                                                                                 //
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

#include "test_qcan_frame.hpp"

#include "cp_msg.h"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#define  TEST_VALUE_DATA         ((uint8_t)  0xA5)
#define  TEST_VALUE_DLC          ((uint8_t)  0x05)
#define  TEST_VALUE_ID_STD       ((uint32_t) 0x00000405)
#define  TEST_VALUE_ID_EXT       ((uint32_t) 0x01B2F405)
#define  TEST_VALUE_MARKER       ((uint32_t) 0x98765432)
#define  TEST_VALUE_TIME_STAMP   ((uint32_t) 10245)
#define  TEST_VALUE_USER         ((uint32_t) 0xAB64281F)



//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFrame::TestQCanFrame()                                                                                     //
// frame type and frame format stay the same through all test cases                                                   //
//--------------------------------------------------------------------------------------------------------------------//
TestQCanFrame::TestQCanFrame()
{
   pclCanStdP = (QCanFrame *) 0L;
   pclCanExtP = (QCanFrame *) 0L;

   pclFdStdP  = (QCanFrame *) 0L;
   pclFdExtP  = (QCanFrame *) 0L;

   pclErrorP  = (QCanFrame *) 0L;

   pclFrameP  = (QCanFrame *) 0L;

}


TestQCanFrame::~TestQCanFrame()
{


}



//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFrame::checkByteArray()                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::checkByteArray()
{
   QByteArray  clByteArrayT;

   for (uint8_t ubCntT = 0; ubCntT < 9; ubCntT++)
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


   //---------------------------------------------------------------------------------------------------
   // test conversion of time-stamp and injection into existing byte array
   //
   QCanTimeStamp clTimeStampT;
   clTimeStampT.setSeconds(4);
   clTimeStampT.setNanoSeconds(120340560);
   QByteArray clTimeArrayT = clTimeStampT.toByteArray();
   clByteArrayT.replace(QCAN_FRAME_TIME_STAMP_POS, 8, clTimeArrayT);
   QCanFrame clFrameT;
   QVERIFY(clFrameT.fromByteArray(clByteArrayT) == true);
   QCanTimeStamp clTimeStampResultT = clFrameT.timeStamp();
   QVERIFY(clTimeStampResultT.seconds()     == 4);
   QVERIFY(clTimeStampResultT.nanoSeconds() == 120340560);


   clTimeStampT.setSeconds(477890321);
   clTimeStampT.setNanoSeconds(987501234);
   clTimeArrayT = clTimeStampT.toByteArray();
   clByteArrayT.replace(QCAN_FRAME_TIME_STAMP_POS, 8, clTimeArrayT);
   QVERIFY(clFrameT.fromByteArray(clByteArrayT) == true);
   clTimeStampResultT = clFrameT.timeStamp();
   QVERIFY(clTimeStampResultT.seconds()     == 477890321);
   QVERIFY(clTimeStampResultT.nanoSeconds() == 987501234);
}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFrame::checkConversion()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::checkConversion()
{
   uint8_t     ubCntT = 0;
   uint8_t     ubDataCntT  = 0;

   CpCanMsg_ts tsCanMsgT;

   //---------------------------------------------------------------------------------------------------
   // test conversion of standard frame
   // QCanFrame -> CpCanMsg structure
   //
   for (ubCntT = 8; ubCntT <= 64; ubCntT ++)
   {
      pclCanStdP->setIdentifier(0x100 + ubCntT);
      pclCanStdP->setDlc(ubCntT);
      pclCanStdP->setData(0, ubCntT);
      pclCanStdP->setData(1, 0x10 + ubCntT);
      pclCanStdP->setData(2, 0x20 + ubCntT);
      pclCanStdP->setData(3, 0x30 + ubCntT);
      pclCanStdP->setData(4, 0x40 + ubCntT);
      pclCanStdP->setData(5, 0x50 + ubCntT);
      pclCanStdP->setData(6, 0x60 + ubCntT);
      pclCanStdP->setData(7, 0x70 + ubCntT);

      //-------------------------------------------------------------------------------------------
      // Convert to CpCanMsg_s structure
      //
      QVERIFY(pclCanStdP->toCpCanMsg(&tsCanMsgT) == true);

      QVERIFY(CpMsgIsFdFrame(&tsCanMsgT)     == false);
      QVERIFY(CpMsgIsExtended(&tsCanMsgT)    == false);
      QVERIFY(CpMsgGetIdentifier(&tsCanMsgT) == pclCanStdP->identifier());
      QVERIFY(CpMsgGetDlc(&tsCanMsgT)        == pclCanStdP->dlc());

      for (ubDataCntT = 0; ubDataCntT < pclCanStdP->dataSize(); ubDataCntT++)
      {
         QVERIFY(CpMsgGetData(&tsCanMsgT, ubDataCntT) == pclCanStdP->data(ubDataCntT));
      }

      //-------------------------------------------------------------------------------------------
      // Convert CpCanMsg_s structure to QCanFrame
      //
      QVERIFY(pclFrameP->fromCpCanMsg(&tsCanMsgT) == true);

      QVERIFY(pclCanStdP->frameFormat() == pclFrameP->frameFormat());
      QVERIFY(pclCanStdP->identifier()  == pclFrameP->identifier());
      QVERIFY(pclCanStdP->dlc()         == pclFrameP->dlc());

      for (ubDataCntT = 0; ubDataCntT < pclCanStdP->dataSize(); ubDataCntT++)
      {
         QVERIFY(pclCanStdP->data(ubDataCntT) == pclFrameP->data(ubDataCntT));
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFrame::checkErrorFrame()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::checkErrorFrame()
{

}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFrame::checkFrameBitrateSwitch()                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::checkFrameBitrateSwitch()
{
   //---------------------------------------------------------------------------------------------------
   // bit-rate switch is disabled by default
   //
   QVERIFY(pclCanStdP->bitrateSwitch() == false);
   QVERIFY(pclCanExtP->bitrateSwitch() == false);
   QVERIFY(pclFdStdP->bitrateSwitch()  == false);
   QVERIFY(pclFdExtP->bitrateSwitch()  == false);


   //---------------------------------------------------------------------------------------------------
   // enable bit-rate switch and test it
   //
   pclCanStdP->setBitrateSwitch(true);
   pclCanExtP->setBitrateSwitch(true);
   pclFdStdP->setBitrateSwitch(true);
   pclFdExtP->setBitrateSwitch(true);

   QVERIFY(pclCanStdP->bitrateSwitch() == false);
   QVERIFY(pclCanExtP->bitrateSwitch() == false);
   QVERIFY(pclFdStdP->bitrateSwitch()  == true);
   QVERIFY(pclFdExtP->bitrateSwitch()  == true);


   //---------------------------------------------------------------------------------------------------
   // copy FD frame contents and convert type to classic CAN
   //
   *pclFrameP = *pclFdStdP;
   QVERIFY(pclFrameP->bitrateSwitch() == true);
   pclFrameP->setFrameFormat(QCanFrame::eFORMAT_CAN_STD);
   QVERIFY(pclFrameP->bitrateSwitch() == false);

   //---------------------------------------------------------------------------------------------------
   // disable bit-rate switch and test it
   //
   pclCanStdP->setBitrateSwitch(false);
   pclCanExtP->setBitrateSwitch(false);
   pclFdStdP->setBitrateSwitch(false);
   pclFdExtP->setBitrateSwitch(false);

   QVERIFY(pclCanStdP->bitrateSwitch() == false);
   QVERIFY(pclCanExtP->bitrateSwitch() == false);
   QVERIFY(pclFdStdP->bitrateSwitch()  == false);
   QVERIFY(pclFdExtP->bitrateSwitch()  == false);

}


//--------------------------------------------------------------------------------------------------------------------//
// checkFrameData()                                                                                                   //
// check frame data                                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::checkFrameData()
{
   pclCanStdP->setDlc(4);
   pclCanStdP->setData(0, 0x12);
   pclCanStdP->setData(1, 0x34);
   pclCanStdP->setData(2, 0x56);
   pclCanStdP->setData(3, 0x78);
   
   bool btMsbFirstT = true;
   QVERIFY(pclCanStdP->dataUInt16(0, btMsbFirstT) == 0x1234);
   QVERIFY(pclCanStdP->dataUInt16(1, btMsbFirstT) == 0x3456);
   QVERIFY(pclCanStdP->dataUInt16(2, btMsbFirstT) == 0x5678);
   QVERIFY(pclCanStdP->dataUInt16(3, btMsbFirstT) == 0x0000);

   QVERIFY(pclCanStdP->dataUInt32(0, btMsbFirstT) == 0x12345678);

   btMsbFirstT = false;
   QVERIFY(pclCanStdP->dataUInt16(0, btMsbFirstT) == 0x3412);
   QVERIFY(pclCanStdP->dataUInt16(1, btMsbFirstT) == 0x5634);
   QVERIFY(pclCanStdP->dataUInt16(2, btMsbFirstT) == 0x7856);
   QVERIFY(pclCanStdP->dataUInt16(3, btMsbFirstT) == 0x0000);

   QVERIFY(pclCanStdP->dataUInt32(0, btMsbFirstT) == 0x78563412);

   pclCanStdP->setDataUInt16(0, 0xAABB);
   QVERIFY(pclCanStdP->data(0) == 0xBB);
   QVERIFY(pclCanStdP->data(1) == 0xAA);
   QVERIFY(pclCanStdP->dataUInt16(0, 0) == 0xAABB);

}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFrame::checkFrameDataSize()                                                                                //
// test invalid data size combinations                                                                                //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::checkFrameDataSize()
{
   uint8_t  ubSizeT;
   
   //---------------------------------------------------------------------------------------------------
   // Test 1: set the data size value for classic CAN and CAN FD and check the result for the
   // dlc() return value
   //
   for (ubSizeT = 0; ubSizeT < 9; ubSizeT++)
   {
      pclCanStdP->setDataSize(ubSizeT);
      pclFdStdP->setDataSize(ubSizeT);
      
      QVERIFY(pclCanStdP->dlc() == ubSizeT);
      QVERIFY(pclFdStdP->dlc()  == ubSizeT);
   }

   for (ubSizeT = 9; ubSizeT <= 12; ubSizeT++)
   {
      pclCanStdP->setDataSize(ubSizeT);
      pclFdStdP->setDataSize(ubSizeT);
      QVERIFY(pclCanStdP->dlc() == 8);
      QVERIFY(pclFdStdP->dlc()  == 9);
   }

   for (ubSizeT = 13; ubSizeT <= 16; ubSizeT++)
   {
      pclCanStdP->setDataSize(ubSizeT);
      pclFdStdP->setDataSize(ubSizeT);
      QVERIFY(pclCanStdP->dlc() ==  8);
      QVERIFY(pclFdStdP->dlc()  == 10);
   }

   for (ubSizeT = 17; ubSizeT <= 20; ubSizeT++)
   {
      pclCanStdP->setDataSize(ubSizeT);
      pclFdStdP->setDataSize(ubSizeT);
      QVERIFY(pclCanStdP->dlc() ==  8);
      QVERIFY(pclFdStdP->dlc()  == 11);
   }


}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFrame::checkFrameDlc()                                                                                     //
// test invalid DLC combinations                                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::checkFrameDlc()
{
   uint8_t  ubSizeT;
   
   //---------------------------------------------------------------------------------------------------
   // Test 1: set the DLC value for classic CAN and CAN FD and check the result for the dataSize()
   // return value
   //
   for (ubSizeT = 0; ubSizeT < 16; ubSizeT++)
   {
      pclCanStdP->setDlc(ubSizeT);
      pclFdStdP->setDlc(ubSizeT);
      
      //-------------------------------------------------------------------------------------------
      // check data size
      //
      if(ubSizeT < 9)
      {
         QVERIFY(pclCanStdP->dataSize() == ubSizeT);
         QVERIFY(pclFdStdP->dataSize() == ubSizeT);
      }
      else
      {
         //-----------------------------------------------------------------------------------
         // copy FD frame contents and convert type to classic CAN
         //
         *pclFrameP = *pclFdStdP;
         pclFrameP->setFrameFormat(QCanFrame::eFORMAT_CAN_STD);
         
         //-----------------------------------------------------------------------------------
         // make sure maximum dataSize of classic CAN is 8
         //
         QVERIFY(pclFrameP->dataSize()  == 8);
         QVERIFY(pclCanStdP->dataSize() == 8);
         
         //-----------------------------------------------------------------------------------
         // check data size of CAN FD 
         //
         switch (ubSizeT)
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
      
      //-------------------------------------------------------------------------------------------
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

   //---------------------------------------------------------------------------------------------------
   // Test 2: set DLC value out of range
   //
   pclCanStdP->setDlc(TEST_VALUE_DLC);
   pclFdStdP->setDlc(TEST_VALUE_DLC);
   for (ubSizeT = 255; ubSizeT > 15; ubSizeT--)
   {
      pclCanStdP->setDlc(ubSizeT);
      pclFdStdP->setDlc(ubSizeT);
      QVERIFY(pclCanStdP->dlc() == TEST_VALUE_DLC);
      QVERIFY(pclFdStdP->dlc()  == TEST_VALUE_DLC);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFrame::checkFrameErrorIndicator()                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::checkFrameErrorIndicator()
{

}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFrame::checkFrameFormat()                                                                                  //
// test the initial frame format                                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::checkFrameFormat()
{
   QVERIFY(pclCanStdP->frameFormat() == QCanFrame::eFORMAT_CAN_STD);
   QVERIFY(pclCanExtP->frameFormat() == QCanFrame::eFORMAT_CAN_EXT);
   QVERIFY(pclFdStdP->frameFormat()  == QCanFrame::eFORMAT_FD_STD);
   QVERIFY(pclFdExtP->frameFormat()  == QCanFrame::eFORMAT_FD_EXT);
  
   
}



//--------------------------------------------------------------------------------------------------------------------//
// checkFrameId()                                                                                                     //
// check frame identifier                                                                                             //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::checkFrameId()
{
   uint32_t ulIdValueT;

   for (ulIdValueT = 0; ulIdValueT <= 0x0000FFFF; ulIdValueT++)
   {
      pclCanStdP->setIdentifier(ulIdValueT);
      pclCanExtP->setIdentifier(ulIdValueT);

      *pclFrameP = *pclCanStdP;
      if (ulIdValueT <= 0x07FF)
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


//--------------------------------------------------------------------------------------------------------------------//
// checkFrameRemote()                                                                                                 //
// check remote frame                                                                                                 //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::checkFrameRemote()
{
   // classic CAN,set remote, change frame type to FD, check remote
   
}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFrame::checkFrameType()                                                                                    //
// test the initial frame type                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::checkFrameType()
{
   QVERIFY(pclCanStdP->frameType() == QCanFrame::eFRAME_TYPE_DATA);
   *pclFrameP = *pclCanStdP;
   QVERIFY(pclFrameP->frameType()  == QCanFrame::eFRAME_TYPE_DATA);

   QVERIFY(pclCanExtP->frameType() == QCanFrame::eFRAME_TYPE_DATA);
   *pclFrameP = *pclCanExtP;
   QVERIFY(pclFrameP->frameType()  == QCanFrame::eFRAME_TYPE_DATA);

   QVERIFY(pclErrorP->frameType()  == QCanFrame::eFRAME_TYPE_ERROR);
   *pclFrameP = *pclErrorP;
   QVERIFY(pclFrameP->frameType()  == QCanFrame::eFRAME_TYPE_ERROR);


   QVERIFY(pclFdStdP->frameType()  == QCanFrame::eFRAME_TYPE_DATA);
   QVERIFY(pclFdExtP->frameType()  == QCanFrame::eFRAME_TYPE_DATA);


}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFrame::checkOutput()                                                                                       //
// test toString() functionality                                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::checkOutput()
{
   QString clStringT;
   bool    btMsbFirstT = true;

   //---------------------------------------------------------------------------------------------------
   // set default values for classic CAN frames
   //
   pclCanStdP->setIdentifier(TEST_VALUE_ID_STD);
   pclCanStdP->setDlc(7);
   pclCanStdP->setDataUInt32(0, 0xA1B2C3D4, btMsbFirstT);

   fprintf(stdout, "%s\n", qPrintable(pclCanStdP->toString()));
   clStringT = pclCanStdP->toString();
   QVERIFY(clStringT.contains("CBFF"         , Qt::CaseSensitive) == true);
   QVERIFY(clStringT.contains("7"            , Qt::CaseSensitive) == true);
   QVERIFY(clStringT.contains("405"          , Qt::CaseSensitive) == true);
   QVERIFY(clStringT.contains("A1 B2 C3 D4"  , Qt::CaseSensitive) == true);

   pclCanExtP->setIdentifier(TEST_VALUE_ID_EXT);
   pclCanExtP->setDlc(7);
   pclCanExtP->setDataUInt32(0, 0x0123);
   fprintf(stdout, "%s\n", qPrintable(pclCanExtP->toString()));
   clStringT = pclCanExtP->toString();
   QVERIFY(clStringT.contains("CEFF", Qt::CaseSensitive) == true);
   QVERIFY(clStringT.contains("405" , Qt::CaseSensitive) == true);

   //---------------------------------------------------------------------------------------------------
   // set default values for CAN FD frames
   //
   pclFdStdP->setIdentifier(TEST_VALUE_ID_STD);
   pclFdStdP->setDlc(9);
   pclFdStdP->setDataUInt32(0, 0xA1B2C3D4);
   fprintf(stdout, "%s\n", qPrintable(pclFdStdP->toString()));

   pclFdStdP->setBitrateSwitch(true);
   fprintf(stdout, "%s\n", qPrintable(pclFdStdP->toString()));

   pclFdStdP->setErrorStateIndicator(true);
   fprintf(stdout, "%s\n", qPrintable(pclFdStdP->toString()));

   pclFdExtP->setIdentifier(TEST_VALUE_ID_EXT);
   pclCanStdP->setDlc(12);
   pclCanStdP->setDataUInt32(0, 0x0123);

   fprintf(stdout, "%s\n", qPrintable(pclFdExtP->toString()));


   //---------------------------------------------------------------------------------------------------
   // set Remote frame
   //
   pclCanStdP->setRemote(true);
   fprintf(stdout, "%s\n", qPrintable(pclCanStdP->toString()));

   //---------------------------------------------------------------------------------------------------
   // Check error frame
   //
   fprintf(stdout, "%s\n", qPrintable(pclErrorP->toString()));


}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFrame::cleanupTestCase()                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::cleanupTestCase()
{
   delete (pclCanStdP);
   delete (pclCanExtP);

   delete (pclFdStdP);
   delete (pclFdExtP);

   delete (pclErrorP);

   delete (pclFrameP);

}


//--------------------------------------------------------------------------------------------------------------------//
// TestQCanFrame::initTestCase()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void TestQCanFrame::initTestCase()
{
   pclCanStdP = new QCanFrame(QCanFrame::eFORMAT_CAN_STD);
   pclCanStdP->setIdentifier(TEST_VALUE_ID_STD);

   pclCanExtP = new QCanFrame(QCanFrame::eFORMAT_CAN_EXT);
   pclCanExtP->setIdentifier(TEST_VALUE_ID_EXT);

   pclFdStdP  = new QCanFrame(QCanFrame::eFORMAT_FD_STD);
   pclFdStdP->setIdentifier(TEST_VALUE_ID_STD);

   pclFdExtP  = new QCanFrame(QCanFrame::eFORMAT_FD_EXT);
   pclFdExtP->setIdentifier(TEST_VALUE_ID_EXT);

   pclErrorP  = new QCanFrame(QCanFrame::eFRAME_TYPE_ERROR);

   //---------------------------------------------------------------------------------------------------
   // testing is done after assignment with the pclFrameP object
   //
   pclFrameP  = new QCanFrame();
}

