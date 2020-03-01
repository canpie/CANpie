//====================================================================================================================//
// File:          cp_fifo.c                                                                                           //
// Description:   CANpie FIFO functions                                                                               //
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

#include "cp_fifo.h"

/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#if   CP_FIFO_MACRO == 0

//--------------------------------------------------------------------------------------------------------------------//
// CpFifoDataInPtr()                                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE CpCanMsg_ts *CpFifoDataInPtr(CpFifo_ts *ptsFifoV)
{
   //---------------------------------------------------------------------------------------------------
   // allow pointer arithmetic here, because index is limited by element ulIndexMax
   //
   /*@ -ptrarith  -dependenttrans -usereleased -compdef           @*/
   return ((ptsFifoV->ptsCanMsg) + (ptsFifoV->ulIndexIn));
   /*@ +ptrarith  +dependenttrans +usereleased +compdef           @*/

}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoDataOutPtr()                                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE CpCanMsg_ts *CpFifoDataOutPtr(CpFifo_ts *ptsFifoV)
{
   //---------------------------------------------------------------------------------------------------
   // allow pointer arithmetic here, because index is limited by element ulIndexMax
   //
   /*@ -ptrarith  -dependenttrans -usereleased -compdef           @*/
   return ((ptsFifoV->ptsCanMsg) + (ptsFifoV->ulIndexOut));
   /*@ +ptrarith  +dependenttrans +usereleased +compdef           @*/

}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoFree()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE uint32_t CpFifoFree(CpFifo_ts *ptsFifoV)
{
   uint32_t ulFreeT;

   if (ptsFifoV->ulState == CP_FIFO_STATE_EMPTY)
   {
      ulFreeT = ptsFifoV->ulIndexMax;
   }
   else if (ptsFifoV->ulState == CP_FIFO_STATE_FULL)
   {
      ulFreeT = 0;
   }
   else
   {
      if (ptsFifoV->ulIndexIn > ptsFifoV->ulIndexOut)
      {
         ulFreeT = (ptsFifoV->ulIndexMax - (ptsFifoV->ulIndexIn - ptsFifoV->ulIndexOut));
      } else
      {
         ulFreeT = (ptsFifoV->ulIndexOut - ptsFifoV->ulIndexIn);
      }
   }

   return (ulFreeT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoIncIn()                                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE void CpFifoIncIn(CpFifo_ts *ptsFifoV)
{
   ptsFifoV->ulIndexIn++;
   if ((ptsFifoV->ulIndexIn) >= (ptsFifoV->ulIndexMax))
   {
      ptsFifoV->ulIndexIn = 0;
   }
   if (ptsFifoV->ulIndexIn == ptsFifoV->ulIndexOut)
   {
      //-------------------------------------------------------------------------------------------
      // set state to full
      //
      ptsFifoV->ulState = CP_FIFO_STATE_FULL;
   }

   //---------------------------------------------------------------------------------------------------
   // clear empty state
   //
   ptsFifoV->ulState &= CP_FIFO_STATE_MASK_EMPTY;
}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoIncOut()                                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE void CpFifoIncOut(CpFifo_ts *ptsFifoV)
{
   ptsFifoV->ulIndexOut++;
   if ((ptsFifoV->ulIndexOut) >= (ptsFifoV->ulIndexMax))
   {
      ptsFifoV->ulIndexOut = 0;
   }

   if (ptsFifoV->ulIndexIn == ptsFifoV->ulIndexOut)
   {
      //-------------------------------------------------------------------------------------------
      // set empty state
      //
      ptsFifoV->ulState = CP_FIFO_STATE_EMPTY;
   }

   //---------------------------------------------------------------------------------------------------
   // clear full state
   //
   ptsFifoV->ulState &= CP_FIFO_STATE_MASK_FULL;
}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoInit()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE void CpFifoInit(CpFifo_ts *ptsFifoV, CpCanMsg_ts *ptsCanMsgV, uint32_t ulSizeV)
{
   ptsFifoV->ulIndexIn  = 0;
   ptsFifoV->ulIndexOut = 0;
   ptsFifoV->ulIndexMax = ulSizeV;
   ptsFifoV->ulState    = CP_FIFO_STATE_EMPTY;     // set initial state to empty
   /*@ -mustfreeonly -temptrans @*/
   ptsFifoV->ptsCanMsg  = ptsCanMsgV;
   /*@ +mustfreeonly +temptrans @*/
}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoIsEmpty()                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE bool_t CpFifoIsEmpty(CpFifo_ts *ptsFifoV)
{
   bool_t btResultT = false;

   if (ptsFifoV->ulState == CP_FIFO_STATE_EMPTY)
   {
      btResultT = true;
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoIsFull()                                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE bool_t CpFifoIsFull(CpFifo_ts *ptsFifoV)
{
   bool_t btResultT = false;

   if (ptsFifoV->ulState == CP_FIFO_STATE_FULL)
   {
      btResultT = true;
   }

   return (btResultT);

}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoPending()                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE uint32_t CpFifoPending(CpFifo_ts *ptsFifoV)
{
   uint32_t ulPendingT;

   if (ptsFifoV->ulState == CP_FIFO_STATE_EMPTY)
   {
      ulPendingT = 0;
   }
   else if (ptsFifoV->ulState == CP_FIFO_STATE_FULL)
   {
      ulPendingT = ptsFifoV->ulIndexMax;
   }
   else
   {
      if (ptsFifoV->ulIndexOut > ptsFifoV->ulIndexIn)
      {
         ulPendingT = (ptsFifoV->ulIndexMax - (ptsFifoV->ulIndexOut - ptsFifoV->ulIndexIn));
      } 
      else
      {
         ulPendingT = (ptsFifoV->ulIndexIn - ptsFifoV->ulIndexOut);
      }
   }

   return (ulPendingT);
}


#endif   // CP_FIFO_MACRO == 0
