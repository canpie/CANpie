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

#include <string.h>        // for memcpy


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoClear()                                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE void CpFifoClear(CpFifo_ts* ptsFifoV)
{
   ptsFifoV->ulIndexIn  = 0;
   ptsFifoV->ulIndexOut = 0;

   #if CPP_HAS_ATOMIC == 0
   ptsFifoV->ulElements = 0;
   #else
   atomic_init(&ptsFifoV->ulElements, 0);
   #endif
}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoCopy()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE bool_t CpFifoCopy(CpFifo_ts *ptsDestFifoV, CpFifo_ts *ptsSrcFifoV)
{
   bool_t   btResultT     = false;
   uint32_t ulBufferSizeT = 0;

   //---------------------------------------------------------------------------------------------------
   // Check for Null pointer first
   //
   if ( (ptsDestFifoV != (CpFifo_ts*) 0L) && (ptsSrcFifoV != (CpFifo_ts*) 0L) )
   {
      //-------------------------------------------------------------------------------------------
      // Do the FIFOs have the same size
      //
      if ((ptsDestFifoV->ulIndexMax) == (ptsSrcFifoV->ulIndexMax))
      {
         //-----------------------------------------------------------------------------------
         // Have the FIFOs been initialized in advance
         //
         if ( (ptsDestFifoV->ptsCanMsg != (CpCanMsg_ts*) 0L) && (ptsSrcFifoV->ptsCanMsg != (CpCanMsg_ts*) 0L) )
         {
            ptsDestFifoV->ulIndexIn  = ptsSrcFifoV->ulIndexIn;
            ptsDestFifoV->ulIndexOut = ptsSrcFifoV->ulIndexOut;
            ptsDestFifoV->ulElements = ptsSrcFifoV->ulElements;
            ulBufferSizeT = (ptsSrcFifoV->ulIndexMax) + 1;
            ulBufferSizeT = ulBufferSizeT * sizeof(CpCanMsg_ts);
            memcpy(ptsDestFifoV->ptsCanMsg, ptsSrcFifoV->ptsCanMsg,  ulBufferSizeT);
            btResultT = true;
         }
      }
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoDataInPtr()                                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE CpCanMsg_ts* CpFifoDataInPtr(CpFifo_ts* ptsFifoV)
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
CPP_INLINE CpCanMsg_ts* CpFifoDataOutPtr(CpFifo_ts* ptsFifoV)
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
CPP_INLINE uint32_t CpFifoFree(CpFifo_ts* ptsFifoV)
{
   uint32_t ulFreeT = 0;

   #if CPP_HAS_ATOMIC == 0
   ulFreeT = ptsFifoV->ulIndexMax - ptsFifoV->ulElements + 1;
   #else
   ulFreeT = ptsFifoV->ulIndexMax - atomic_load(&ptsFifoV->ulElements) + 1;
   #endif

   return (ulFreeT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoIncIn()                                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE void CpFifoIncIn(CpFifo_ts* ptsFifoV)
{
   uint32_t ulIndexIn = (ptsFifoV->ulIndexIn + 1);
   if ((ulIndexIn) > (ptsFifoV->ulIndexMax))
   {
      ulIndexIn = 0;
   }

   ptsFifoV->ulIndexIn = ulIndexIn;

   #if CPP_HAS_ATOMIC == 0
   ptsFifoV->ulElements++;
   #else
   atomic_fetch_add(&ptsFifoV->ulElements, 1); 
   #endif

}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoIncOut()                                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE void CpFifoIncOut(CpFifo_ts* ptsFifoV)
{
   uint32_t ulIndexOut = (ptsFifoV->ulIndexOut + 1);
   if ((ulIndexOut) > (ptsFifoV->ulIndexMax))
   {
      ulIndexOut = 0;
   }

   ptsFifoV->ulIndexOut = ulIndexOut;

   #if CPP_HAS_ATOMIC == 0
   ptsFifoV->ulElements--;
   #else
   atomic_fetch_sub(&ptsFifoV->ulElements, 1); 
   #endif
}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoInit()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE void CpFifoInit(CpFifo_ts* ptsFifoV, CpCanMsg_ts* ptsCanMsgV, uint32_t ulSizeV)
{
   ptsFifoV->ulIndexIn  = 0;
   ptsFifoV->ulIndexOut = 0;
   ptsFifoV->ulIndexMax = ulSizeV - 1;                // index range is 0 to (size - 1)

   #if CPP_HAS_ATOMIC == 0
   ptsFifoV->ulElements = 0;
   #else
   atomic_init(&ptsFifoV->ulElements, 0);
   #endif

   /*@ -mustfreeonly -temptrans @*/
   ptsFifoV->ptsCanMsg  = ptsCanMsgV;                 // store pointer to array of CpCanMsg_ts structure
   /*@ +mustfreeonly +temptrans @*/
}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoIsEmpty()                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE bool_t CpFifoIsEmpty(CpFifo_ts* ptsFifoV)
{
   bool_t btResultT = false;

   #if CPP_HAS_ATOMIC == 0
   if (ptsFifoV->ulElements == 0)
   {
      btResultT = true;
   }
   #else
   if (atomic_load(&ptsFifoV->ulElements) == 0)
   {
      btResultT = true;
   }
   #endif

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoIsFull()                                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE bool_t CpFifoIsFull(CpFifo_ts* ptsFifoV)
{
   bool_t btResultT = false;

   #if CPP_HAS_ATOMIC == 0
   if (ptsFifoV->ulElements == ptsFifoV->ulIndexMax + 1)
   {
      btResultT = true;
   }
   #else
   if (atomic_load(&ptsFifoV->ulElements) == ((ptsFifoV->ulIndexMax) + 1) )
   {
      btResultT = true;
   }
   #endif

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CpFifoPending()                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CPP_INLINE uint32_t CpFifoPending(CpFifo_ts* ptsFifoV)
{
   uint32_t ulPendingT;

   #if CPP_HAS_ATOMIC == 0
   ulPendingT = ptsFifoV->ulElements;
   #else
   ulPendingT = atomic_load(&ptsFifoV->ulElements);
   #endif

   return (ulPendingT);
}

