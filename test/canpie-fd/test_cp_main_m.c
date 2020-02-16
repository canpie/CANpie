//============================================================================//
// File:          test_cp_main_m.c                                            //
// Description:   Unit tests for CANpie (using message access macros)         //
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


#include "canpie.h"
#include "stdio.h"
#include "unity_fixture.h"



//----------------------------------------------------------------------------//
// RunAllTests()                                                              //
// initialise Transmit PDOs                                                   //
//----------------------------------------------------------------------------//
static void RunAllTests(void)
{

   RUN_TEST_GROUP(CP_MSG_CCM);
   RUN_TEST_GROUP(CP_MSG_FDM);

}



//----------------------------------------------------------------------------//
// main()                                                                     //
// initialise Transmit PDOs                                                   //
//----------------------------------------------------------------------------//
int main(int argc, const char *argv[])
{
   printf("--------------------------------------------------------------\n");
   printf("| CANpie unit tests\n");
   printf("| Test API version %d.%d \n",
          (CP_VERSION_MAJOR), (CP_VERSION_MINOR));
   printf("--------------------------------------------------------------\n");


   //----------------------------------------------------------------
   // start unit tests
   //
   UnityMain(argc, argv, RunAllTests);

   return 0;

}
