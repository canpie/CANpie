//====================================================================================================================//
// File:          test_cp_main.c                                                                                      //
// Description:   Test suite for CANpie modules                                                                       //
//                                                                                                                    //
// Copyright (C) MicroControl GmbH & Co. KG                                                                           //
// 53844 Troisdorf - Germany                                                                                          //
// www.microcontrol.net                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
// The copyright to the computer program(s) herein is the property of MicroControl GmbH & Co. KG, Germany. The        //
// program(s) may be used and/or copied only with the written permission of MicroControl GmbH & Co. KG or in          //
// accordance with the terms and conditions stipulated in the agreement/contract under which the program(s) have      //
// been supplied.                                                                                                     //
//                                                                                                                    //
//====================================================================================================================//


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/
#include "fff_mc.h"  // include Fake Function Framework (fff)
DEFINE_FFF_GLOBALS

#ifdef __MINGW32__
#include <windows.h>
#endif

#include "cp_fifo.h"

//--------------------------------------------------------------------------------------------------------------------//
// McTestRunSetup()                                                                                                   //
// This function is called each time before running a test case                                                       //
//--------------------------------------------------------------------------------------------------------------------//
void McTestRunSetup()
{
   FFF_RESET_HISTORY();
}


//--------------------------------------------------------------------------------------------------------------------//
// McAppCriticalSectionEnd()                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void McAppCriticalSectionEnd(void)
{
   //---------------------------------------------------------------------------------------------------
   // Dummy function for NRL
   //
}


//--------------------------------------------------------------------------------------------------------------------//
// McAppCriticalSectionStart()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void McAppCriticalSectionStart(void)
{
   //---------------------------------------------------------------------------------------------------
   // Dummy function for NRL
   //
}


//--------------------------------------------------------------------------------------------------------------------//
// main()                                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
int main(void)
{
   #ifdef NDEBUG
   printf("NDEBUG is defined, so assert() function is defined to ((void)0)!\n");
   printf("Make sure firmware is compiled in not as 'release' for test execution!\n");
   #else
   #ifdef __MINGW32__
   SetErrorMode (SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
   #endif

   printf("\n#-------------------------------------------------------------------#\n");
   printf("# Unit Testing of MCL CANpie FD modules based on FFF\n");
   printf("#-------------------------------------------------------------------#\n\n");

   //--------------------------------------------------------------------------------------------------- 
   // reset and run all FIFO tests
   //
   #if   CP_FIFO_MACRO == 1
   printf("\nRun CP FIFO Test with MACRO implementation!\n");
   #else
   printf("\nRun CP FIFO Test with FUNCTION implementation!\n");
   #endif
   RESET_TEST_SUITE(CP_FIFO);
   RUN_TEST_SUITE(CP_FIFO);


   RESET_TEST_SUITE(CP_NRL);
   RUN_TEST_SUITE(CP_NRL);

   printf("\n#-------------------------------------------------------------------#\n");
   printf("# --- FINISH ---                                                    #\n");
   printf("#-------------------------------------------------------------------#\n\n");

   #endif
   return 0;
} 
