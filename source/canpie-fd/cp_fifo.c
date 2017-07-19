//============================================================================//
// File:          cp_fifo.h                                                   //
// Description:   CANpie FIFO functions                                       //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
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
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'LICENSE' file.                   //
//                                                                            //
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
inline CpCanMsg_ts *CpFifoDataInPtr(CpFifo_ts *ptsFifoV)
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
inline CpCanMsg_ts *CpFifoDataOutPtr(CpFifo_ts *ptsFifoV)
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
inline void CpFifoIncIn(CpFifo_ts *ptsFifoV)
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
inline void CpFifoIncOut(CpFifo_ts *ptsFifoV)
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
inline void CpFifoInit(CpFifo_ts *ptsFifoV, CpCanMsg_ts *ptsCanMsgV,
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
inline bool_t CpFifoIsEmpty(CpFifo_ts *ptsFifoV)
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
inline bool_t CpFifoIsFull(CpFifo_ts *ptsFifoV)
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
