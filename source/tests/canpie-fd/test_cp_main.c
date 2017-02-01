/*
 * com_test_main.c
 *
 *  Created on: 21.10.2015
 *      Author: koppe
 */

#include "canpie.h"
#include "stdio.h"
#include "unity_fixture.h"



//----------------------------------------------------------------------------//
// RunAllTests()                                                              //
// initialise Transmit PDOs                                                   //
//----------------------------------------------------------------------------//
static void RunAllTests(void)
{
   RUN_TEST_GROUP(CP_CORE);

   RUN_TEST_GROUP(CP_MSG_CCF);
   RUN_TEST_GROUP(CP_MSG_CCM);
   RUN_TEST_GROUP(CP_MSG_FDF);
   RUN_TEST_GROUP(CP_MSG_FDM);



}



//----------------------------------------------------------------------------//
// main()                                                                     //
// initialise Transmit PDOs                                                   //
//----------------------------------------------------------------------------//
int main(int argc, const char* argv[])
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
