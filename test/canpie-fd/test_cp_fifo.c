//====================================================================================================================//
// File:          test_cp_fifo.c                                                                                      //
// Description:   Unit tests for CANpie FIFO functions                                                                //
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
** Pre-condition                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------------
// Support CAN FD with message field 'user'
//
#define  CP_CAN_FD               1
#define  CP_CAN_MSG_MACRO        0
#define  CP_CAN_MSG_MARKER       0
#define  CP_CAN_MSG_TIME         0
#define  CP_CAN_MSG_USER         1


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include "cp_fifo.h"
#include "cp_msg.h"

#include "unity_fixture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>

//----------------------------------------------------------------------------//
/*!
** \file    test_cp_fifo.c
** \brief   CANpie test cases for FIFO functions
**
**
*/
//----------------------------------------------------------------------------//


/*--------------------------------------------------------------------------------------------------------------------*\
** Variables of module                                                                                                **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/
TEST_GROUP(CP_FIFO);             // test group name

#define  NUMBER_OF_FIFO_ENTRIES   32

static CpFifo_ts     tsFifoS;                               // FIFO structrue
static CpCanMsg_ts   atsCanMsgS[NUMBER_OF_FIFO_ENTRIES];    // buffer for entries


/*----------------------------------------------------------------------------*\
** Function implementations                                                   **
**                                                                            **
\*----------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// PushToFifoThread()                                                                                                 //
// init code for each test case                                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
void *PushToFifoThread(void *pvdArgumentV)
{
   //-------------------------------------------------------------------------------------------
   //
   uint32_t       ulNumberOfFramesT = *((uint32_t *) pvdArgumentV);
   uint32_t       ulCntT;
   CpCanMsg_ts    tsCanMsgT;
   CpCanMsg_ts    *ptsCanInEntryT;

   UnityPrint(" CP_FIFO_002: start pushing to FIFO .. \n");


   UnityPrint(" CP_FIFO_002: stopped pushing to FIFO. \n");

   return NULL;
}

void *PopFromFifoThread()
{
   UnityPrint(" CP_FIFO_002: start popping from FIFO ..");


   UnityPrint(" CP_FIFO_002: stopped popping from FIFO.");

   return NULL;
}


//--------------------------------------------------------------------------------------------------------------------//
// TEST_SETUP()                                                                                                       //
// init code for each test case                                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
TEST_SETUP(CP_FIFO)
{
   CpFifoInit(&tsFifoS, &atsCanMsgS[0], NUMBER_OF_FIFO_ENTRIES);

}


//--------------------------------------------------------------------------------------------------------------------//
// TEST_TEAR_DOWN()                                                           //
// release code for each test case                                            //
//--------------------------------------------------------------------------------------------------------------------//
TEST_TEAR_DOWN(CP_FIFO)
{

}


//--------------------------------------------------------------------------------------------------------------------//
// TEST_TEAR_DOWN()                                                           //
// release code for each test case                                            //
//--------------------------------------------------------------------------------------------------------------------//
TEST(CP_FIFO, 001)
{
   CpCanMsg_ts    tsCanMsgT;
   CpCanMsg_ts    *ptsCanInEntryT;
   uint8_t        ubCntT;

   //----------------------------------------------------------------
   // @SubTest01
   //
   TEST_ASSERT_EQUAL_UINT32(NUMBER_OF_FIFO_ENTRIES, CpFifoFree(&tsFifoS));

   for (ubCntT = 0; ubCntT < NUMBER_OF_FIFO_ENTRIES; ubCntT++)
   {
      CpMsgInit(&tsCanMsgT, CP_MSG_FORMAT_FBFF);
      CpMsgSetDlc(&tsCanMsgT, 0x04);
      CpMsgSetIdentifier(&tsCanMsgT, 0x100 + ubCntT);
   
      if (!(CpFifoIsFull(&tsFifoS)))
      {
         ptsCanInEntryT = CpFifoDataInPtr(&tsFifoS);
         memcpy(ptsCanInEntryT, &tsCanMsgT, sizeof(CpCanMsg_ts));
         CpFifoIncIn(&tsFifoS);
      }
   }
   TEST_ASSERT_EQUAL_UINT32(0, CpFifoFree(&tsFifoS));

   UnityPrint(" CP_FIFO_001: PASSED");
   printf("\n");
}


//--------------------------------------------------------------------------------------------------------------------//
// TEST_TEAR_DOWN()                                                           //
// release code for each test case                                            //
//--------------------------------------------------------------------------------------------------------------------//
TEST(CP_FIFO, 002)
{
   int rc1, rc2;
   pthread_t thread1, thread2;
   uint32_t    ulFrameCountT = 16;
   /* Create independent threads each of which will execute functionC */

   if( (rc1=pthread_create( &thread1, NULL, &PushToFifoThread, &ulFrameCountT)) )
   {
      printf("Thread creation failed: %d\n", rc1);
   }

   if( (rc2=pthread_create( &thread2, NULL, &PopFromFifoThread, &ulFrameCountT)) )
   {
      printf("Thread creation failed: %d\n", rc2);
   }

   pthread_join( thread1, NULL);
   pthread_join( thread2, NULL); 

   UnityPrint(" CP_FIFO_002: PASSED");
   printf("\n");
}


//----------------------------------------------------------------------------//
// TEST_GROUP_RUNNER()                                                        //
// execute all test cases                                                     //
//----------------------------------------------------------------------------//
TEST_GROUP_RUNNER(CP_FIFO)
{
   UnityPrint("--- Run test group: CP_FIFO ----------------------------------");
   printf("\n");
   RUN_TEST_CASE(CP_FIFO, 001);
   RUN_TEST_CASE(CP_FIFO, 002);
   printf("\n");
}
