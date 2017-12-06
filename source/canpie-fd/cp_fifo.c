//============================================================================//
// File:          cp_fifo.c                                                   //
// Description:   CANpie FIFO functions                                       //
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





/*----------------------------------------------------------------------------*\
** Includes                                                                   **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "cp_fifo.h"

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// CpFifoDataInPtr()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
CPP_INLINE CpCanMsg_ts *CpFifoDataInPtr(CpFifo_ts *ptsFifoV)
{
   //----------------------------------------------------------------
   // allow pointer arithmetic here, because index is limited
   // by element ulIndexMax
   //
   /*@ -ptrarith  -dependenttrans -usereleased -compdef           @*/
   return ((ptsFifoV->ptsCanMsg) + (ptsFifoV->ulIndexIn));
   /*@ +ptrarith  +dependenttrans +usereleased +compdef           @*/

}


//----------------------------------------------------------------------------//
// CpFifoDataOutPtr()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
CPP_INLINE CpCanMsg_ts *CpFifoDataOutPtr(CpFifo_ts *ptsFifoV)
{
   //----------------------------------------------------------------
   // allow pointer arithmetic here, because index is limited
   // by element ulIndexMax
   //
   /*@ -ptrarith  -dependenttrans -usereleased -compdef           @*/
   return ((ptsFifoV->ptsCanMsg) + (ptsFifoV->ulIndexOut));
   /*@ +ptrarith  +dependenttrans +usereleased +compdef           @*/

}


//----------------------------------------------------------------------------//
// CpFifoIncIn()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
CPP_INLINE void CpFifoIncIn(CpFifo_ts *ptsFifoV)
{
   ptsFifoV->ulIndexIn++;
   if ((ptsFifoV->ulIndexIn) >= (ptsFifoV->ulIndexMax))
   {
      ptsFifoV->ulIndexIn = 0;
   }
}


//----------------------------------------------------------------------------//
// CpFifoIncOut()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
CPP_INLINE void CpFifoIncOut(CpFifo_ts *ptsFifoV)
{
   ptsFifoV->ulIndexOut++;
   if ((ptsFifoV->ulIndexOut) >= (ptsFifoV->ulIndexMax))
   {
      ptsFifoV->ulIndexOut = 0;
   }
}


//----------------------------------------------------------------------------//
// CpFifoInit()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
CPP_INLINE void CpFifoInit(CpFifo_ts *ptsFifoV, CpCanMsg_ts *ptsCanMsgV,
                       uint32_t ulSizeV)
{
   ptsFifoV->ulIndexIn  = 0;
   ptsFifoV->ulIndexOut = 0;
   ptsFifoV->ulIndexMax = ulSizeV;
   /*@ -mustfreeonly -temptrans @*/
   ptsFifoV->ptsCanMsg  = ptsCanMsgV;
   /*@ +mustfreeonly +temptrans @*/
}


//----------------------------------------------------------------------------//
// CpFifoIsEmpty()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
CPP_INLINE bool_t CpFifoIsEmpty(CpFifo_ts *ptsFifoV)
{
   bool_t btResultT = false;

   if ((ptsFifoV->ulIndexIn) == (ptsFifoV->ulIndexOut))
   {
      btResultT = true;
   }

   return (btResultT);
}


//----------------------------------------------------------------------------//
// CpFifoIsFull()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
CPP_INLINE bool_t CpFifoIsFull(CpFifo_ts *ptsFifoV)
{
   bool_t btResultT = false;

   if (((ptsFifoV->ulIndexIn + 1) == (ptsFifoV->ulIndexOut)) ||
         (((ptsFifoV->ulIndexOut) == 0) &&
          ((ptsFifoV->ulIndexIn + 1) == ptsFifoV->ulIndexMax)))
   {
      btResultT = true;
   }

   return (btResultT);

}
