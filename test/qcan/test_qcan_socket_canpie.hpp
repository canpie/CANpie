//====================================================================================================================//
// File:          test_qcan_socket.hpp                                                                                //
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



#ifndef TEST_QCAN_SOCKET_CANPIE_HPP_
#define TEST_QCAN_SOCKET_CANPIE_HPP_


#include <QtTest/QTest>

#include "qcan_socket_canpie_fd.hpp"


//-----------------------------------------------------------------------------
/*!
** \class   TestQCanSocketCpFD
** \brief   Test CAN client
** 
*/
class TestQCanSocketCpFD : public QObject
{
   Q_OBJECT

public:
   
   TestQCanSocketCpFD();
   
   
   ~TestQCanSocketCpFD();
private:
   
   bool             btServerIsActiveP;
   bool             btHasCanFdPluginP;
   CpPort_ts        tsCanPortP;

private slots:
   void initTestCase();

   void checkConnect();

   void checkFrameWrite();

   void checkFrameRead();

   void cleanupTestCase();
};




#endif   // TEST_QCAN_SOCKET_CANPIE_HPP_

