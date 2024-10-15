//====================================================================================================================//
// File:          test_cp_nrl.c                                                                                       //
// Description:   Test cases for MCL NRL implementation                                                               //
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

// header files of module for test
#include "cp_nrl.h"

#include <stdio.h>

/*--------------------------------------------------------------------------------------------------------------------*\
** Defines                                                                                                            **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 


/*--------------------------------------------------------------------------------------------------------------------*\
** Custom Function implementation of faked function                                                                   **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 

/*--------------------------------------------------------------------------------------------------------------------*\
** Internal data and function declaration                                                                             **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 
static CpNrlCoreData_ts *  ptsNrlCoreDataS = (CpNrlCoreData_ts *) 0L;


/*--------------------------------------------------------------------------------------------------------------------*\
** Test implementation                                                                                                **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 


//--------------------------------------------------------------------------------------------------------------------//
// CpNrlMboxCounterEvent()                                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool_t CpNrlMboxCounterEvent(uint8_t ubPhyIfV, uint8_t ubBufferIdxV)
{
   uint8_t ubLineIndexT  = eCP_NRL_LINE_NONE;

   if (ptsNrlCoreDataS != (CpNrlCoreData_ts *) 0L)
   {
      // printf("----- CpNrlMboxCounterEvent >>>>> \r\n");

      //-------------------------------------------------------------------------------------------
      // Find the physical CAN interface inside the CpNrlCoreData_ts structure
      //
      if (ubPhyIfV == ptsNrlCoreDataS->ubInterfaceDCL)
      {
         ubLineIndexT = eCP_NRL_LINE_DCL;
        
      }

      if (ubPhyIfV == ptsNrlCoreDataS->ubInterfaceRCL)
      {
         ubLineIndexT = eCP_NRL_LINE_RCL;
      }

      if (ubLineIndexT < eCP_NRL_LINE_NONE) 
      {
         //-----------------------------------------------------------------------------------
         // Read the current value
         // The instruction __LDREXH() requires a pointer to unsigned 16-Bit, so we have to 
         // cast the pointer here.
         //
         // int16_t swCounterValueT = __LDREXH((uint16_t *) &(ptsNrlCoreDataS->aswMboxCounter[ubBufferIdxV]));
         int16_t swCounterValueT = ptsNrlCoreDataS->aswMboxCounter[ubBufferIdxV];
 
         // printf("      CpNrlMboxCounterEvent  ->  swCounterValueT_bef = %i\r\n", swCounterValueT);

         //-----------------------------------------------------------------------------------
         // An event on the DCL increases the counter by 1, an event on the RCL decreases the
         // counter by 1.
         //
         if (ubLineIndexT == eCP_NRL_LINE_DCL)
         {
            swCounterValueT++;
         }
         else
         {
            swCounterValueT--;
         }

         // printf("      CpNrlMboxCounterEvent  ->  swCounterValueT_aft = %i\r\n", swCounterValueT);

         //-----------------------------------------------------------------------------------
         // Try to write new counter value
         // The instruction __STREXH() requires a pointer to unsigned 16-Bit, so we have to 
         // cast the pointer here.
         //
         //
         // if (0 == __STREXH(swCounterValueT, (uint16_t *) &(ptsNrlCoreDataS->aswMboxCounter[ubBufferIdxV]))) 
         ptsNrlCoreDataS->aswMboxCounter[ubBufferIdxV] = swCounterValueT;
         if (1) // storage was OK
         {
            //--------------------------------------------------------------------------------
            // Data memory barrier instruction.
            //
            // __DMB();
            
            // Written, return success.
            return true;
         }
      } while (1);

   }

   //---------------------------------------------------------------------------------------------------
   // Failure
   //
   return false;

}


//--------------------------------------------------------------------------------------------------------------------//
// CpNrlMboxTickDecrement()                                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
static void CpNrlMboxTickDecrement(uint8_t ubBufferIdxV)
{
   // printf("------CpNrlMboxTickDecrement------\r\n");
   do {
      //-------------------------------------------------------------------------------------------
      // Read the current value.
      //
      // uint16_t uwTickValueT = __LDREXH(&(ptsNrlCoreDataS->auwMboxTick[0][ubBufferIdxV]));
      uint16_t uwTickValueT = ptsNrlCoreDataS->auwMboxTick[0][ubBufferIdxV];
 
      // printf("CpNrlMboxTickDecrement [%i] ->  uwTickValueT_0 = %i\r\n", ubBufferIdxV, uwTickValueT);

      //-------------------------------------------------------------------------------------------
      // Try to write reload value
      //
      if (uwTickValueT > 0)
      {
         uwTickValueT--;
      }
      // if (0 == __STREXH(uwTickValueT, &(ptsNrlCoreDataS->auwMboxTick[0][ubBufferIdxV]))) 
      ptsNrlCoreDataS->auwMboxTick[0][ubBufferIdxV] = uwTickValueT;
      if (1) // Storage was OK
      {
         //-----------------------------------------------------------------------------------
         // Data memory barrier instruction.
         //
         // __DMB();
         break;
      }
   } while (1);

   do {
      //-------------------------------------------------------------------------------------------
      // Read the current value.
      //
      // uint16_t uwTickValueT = __LDREXH(&(ptsNrlCoreDataS->auwMboxTick[1][ubBufferIdxV]));
      uint16_t uwTickValueT = ptsNrlCoreDataS->auwMboxTick[1][ubBufferIdxV];
 
      // printf("CpNrlMboxTickDecrement [%i]  ->  uwTickValueT_1 = %i\r\n", ubBufferIdxV , uwTickValueT);

      //-------------------------------------------------------------------------------------------
      // Try to write reload value
      //
      if (uwTickValueT > 0)
      {
         uwTickValueT--;
      }

      //if (0 == __STREXH(uwTickValueT, &(ptsNrlCoreDataS->auwMboxTick[1][ubBufferIdxV]))) 
      ptsNrlCoreDataS->auwMboxTick[1][ubBufferIdxV] = uwTickValueT;
      if(1)
      {
         //-----------------------------------------------------------------------------------
         // Data memory barrier instruction.
         //
         // __DMB();
         break;
      }
   } while (1);

}


//--------------------------------------------------------------------------------------------------------------------//
// CpNrlMboxTickLoad()                                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool_t CpNrlMboxTickLoad(uint8_t ubPhyIfV, uint8_t ubBufferIdxV)
{
   uint8_t ubLineIndexT  = eCP_NRL_LINE_NONE;

   if (ptsNrlCoreDataS != (CpNrlCoreData_ts *) 0L)
   {
      //-------------------------------------------------------------------------------------------
      // Find the physical CAN interface inside the CpNrlCoreData_ts structure
      //
      if (ubPhyIfV == ptsNrlCoreDataS->ubInterfaceDCL)
      {
         ubLineIndexT = eCP_NRL_LINE_DCL;
      }

      if (ubPhyIfV == ptsNrlCoreDataS->ubInterfaceRCL)
      {
         ubLineIndexT = eCP_NRL_LINE_RCL;
      }

      if (ubLineIndexT < eCP_NRL_LINE_NONE) 
      {
         //-----------------------------------------------------------------------------------
         // Read the current value.
         //
         // uint16_t uwTickValueT = __LDREXH(&(ptsNrlCoreDataS->auwMboxTick[ubLineIndexT][ubBufferIdxV]));
         uint16_t uwTickValueT = ptsNrlCoreDataS->auwMboxTick[ubLineIndexT][ubBufferIdxV];
 
         //-----------------------------------------------------------------------------------
         // Try to write reload value
         //
         uwTickValueT = ptsNrlCoreDataS->uwMboxTickReload;
         // if (0 == __STREXH(uwTickValueT, &(ptsNrlCoreDataS->auwMboxTick[ubLineIndexT][ubBufferIdxV]))) 
         ptsNrlCoreDataS->auwMboxTick[ubLineIndexT][ubBufferIdxV] = uwTickValueT;
         if (1)
         {
            //--------------------------------------------------------------------------------
            // Data memory barrier instruction.
            //
            // __DMB();
            
            // Written, return success.
            return true;
         }
      } while (1);

   }

   //---------------------------------------------------------------------------------------------------
   // Failure
   //
   return false;
}


//--------------------------------------------------------------------------------------------------------------------//
// CpNrlMboxTickEvent()                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void CpNrlMboxTickEvent(void)
{
   // printf("------ -------------------------------------------------------------- CpNrlMboxTickEvent------\r\n");  
   if (ptsNrlCoreDataS != (CpNrlCoreData_ts *) 0L)
   {
      for (uint8_t ubBufferT = 0; ubBufferT < CP_BUFFER_MAX; ubBufferT++)
      {
         CpNrlMboxTickDecrement(ubBufferT);
      }
   }
}

void CpNrlMbox_DriverInit(uint8_t ubPhyIfV)
{
   ptsNrlCoreDataS = CpNrlCoreData(ubPhyIfV);
}