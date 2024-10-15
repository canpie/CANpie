//====================================================================================================================//
// File:          fff_mc.h                                                                                            //
// Description:   MicroControl specific defines of FFF                                                                //
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


#ifndef FFF_MC_H_
#define FFF_MC_H_

/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 
#include <fff.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>


/*--------------------------------------------------------------------------------------------------------------------*\
** Function declaration                                                                                               **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 

//---------------------------------------------------------------------------------------------------------------------- 
/*!
** \brief McTestRunSetup()
*/
void McTestRunSetup(void);


/*--------------------------------------------------------------------------------------------------------------------*\
** Defines                                                                                                            **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 

//---------------------------------------------------------------------------------------------------------------------- 
/*!
** \def ASSERT_EQ
** This assertion is valid if both expressions A and B are equal.
*/
#define ASSERT_EQ(A, B) assert((A) == (B))


//---------------------------------------------------------------------------------------------------------------------- 
/*!
** \def ASSERT_RANGE
** This assertion is valid if expression A is within B±C range.
*/
#define ASSERT_RANGE(A, B, C) assert(((A) <= (B+C)) && ((A) >= (B-C)))


//---------------------------------------------------------------------------------------------------------------------- 
/*!
** \dev ASSERT_TRUE
** This assertion is valid if expression A returns true or a value > 0
*/
#define ASSERT_TRUE(A) assert((A))


//---------------------------------------------------------------------------------------------------------------------- 
/*!
** \def  RUN_TEST
** This macro calls a function to reset a test suite.
**
** This function is typically called from main.
**
*/
#define RESET_TEST_SUITE(SUITE) \
      do {  \
         printf("--- Resetting test suite %s: \n", #SUITE); \
         void TEST_SUITE_##SUITE##_reset(void); \
         TEST_SUITE_##SUITE##_reset(); \
      } while(0)


//---------------------------------------------------------------------------------------------------------------------- 
/*!
** \def  RUN_TEST
** This macro calls a function of corresponding test case given by a suite and name
*/
#define RUN_TEST(SUITE, NAME) \
   do {  \
      printf("    >>> Running %s.%s: \n", #SUITE, #NAME); \
      McTestRunSetup(); \
      TEST_##SUITE##_##NAME(); \
   } while (0)


//----------------------------------------------------------------------------------------------------------------------
/*!
** \def  RUN_TEST
** This macro calls a function to run a test suite.
** 
** This function is typically called from main.
**
*/
#define RUN_TEST_SUITE(SUITE) \
      do {  \
         printf("--- Running test suite %s: \n", #SUITE); \
         void TEST_SUITE_##SUITE##_run(void); \
         TEST_SUITE_##SUITE##_run(); \
      } while(0)


//---------------------------------------------------------------------------------------------------------------------- 
/*!
** \def  TEST
** Defines function name of a test case, generated by suite and name it's self.
*/
#define TEST(SUITE, NAME)      void TEST_##SUITE##_##NAME(void)


//---------------------------------------------------------------------------------------------------------------------- 
/*!
** \def  TEST_SUITE_RESET
** Defines function name of a test suite to reset all tests of corresponding suite.
*/
#define TEST_SUITE_RESET(SUITE)  void TEST_SUITE_##SUITE##_reset(void)


//---------------------------------------------------------------------------------------------------------------------- 
/*!
** \def  TEST_SUITE_RUN
** Defines function name of a test suite to run all tests of corresponding suite.
*/
#define TEST_SUITE_RUN(SUITE)    void TEST_SUITE_##SUITE##_run(void)

#endif // FFF_MC_H_
