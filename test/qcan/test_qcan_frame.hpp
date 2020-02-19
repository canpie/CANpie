//====================================================================================================================//
// File:          test_qcan_frame.hpp                                                                                 //
// Description:   QCAN classes - CAN frame tests                                                                      //
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


#ifndef TEST_QCAN_FRAME_HPP_
#define TEST_QCAN_FRAME_HPP_


#include <QtTest/QTest>
#include <QCanFrame>


//-----------------------------------------------------------------------------
/*!
** \class   TestQCanFrame
** \brief   Test CAN frame
** 
*/
class TestQCanFrame : public QObject
{
   Q_OBJECT

public:
   
   TestQCanFrame();
   
   
   ~TestQCanFrame();

private:
   
   QCanFrame *    pclCanStdP;
   QCanFrame *    pclCanExtP;
   QCanFrame *    pclFdStdP;
   QCanFrame *    pclFdExtP;
   QCanFrame *    pclFrameP;
   QCanFrame *    pclErrorP;

private slots:

   void initTestCase();
   
   void checkByteArray();

   void checkConversion();

   void checkErrorFrame();

   void checkFrameType();

   void checkFrameFormat();

   void checkFrameId();
   void checkFrameDlc();
   void checkFrameDataSize();
   
   void checkFrameData();
   void checkFrameRemote();

   void checkFrameBitrateSwitch();
   void checkFrameErrorIndicator();

   void checkOutput();

   void cleanupTestCase();
};




#endif   // TEST_QCAN_FRAME_HPP_

