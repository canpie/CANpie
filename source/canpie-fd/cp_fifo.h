//============================================================================//
// File:          cp_fifo.h                                                   //
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




#ifndef  CP_FIFO_H_
#define  CP_FIFO_H_

//-----------------------------------------------------------------------------
/*!
** \file    cp_fifo.h
** \brief   CANpie FIFO functions
**
** A CAN message FIFO can be assigned to every message message buffer
** by calling CpCoreFifoConfig(). This file defines the structure of
** a CAN message FIFO (CpFifo_s) and inline functions to access the
** FIFO.
*/

/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "canpie.h"

//-------------------------------------------------------------------//
// take precautions if compiled with C++ compiler                    //
#ifdef __cplusplus                                                   //
extern "C" {                                                         //
#endif                                                               //
//-------------------------------------------------------------------//

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

#ifndef CP_FIFO_MACRO
#define CP_FIFO_MACRO   0
#endif


/*----------------------------------------------------------------------------*\
** Structures                                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*!
** \struct  CpFifo_s
** \brief   Administration variables of a CAN message FIFO
**
** This structure is initialised by CpFifoInit().
*/
struct CpFifo_s
{
   /*! Index where the last data has been written to
   */
   uint32_t  ulIndexIn;

   /*! Index where the last data has been read from
   */
   uint32_t  ulIndexOut;

   /*! Maximum number of FIFO entries
   */
   uint32_t  ulIndexMax;

   /*!
   ** Pointer to CAN message buffer
   */
   CpCanMsg_ts *ptsCanMsg;
};
/*!
** \typedef    CpFifo_ts
*/
typedef struct CpFifo_s CpFifo_ts;

/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

/*!
** \brief   Get next free entry from FIFO
** \param   ptsFifoV - Pointer to CAN message FIFO
** \return  Pointer to CAN message
**
** This function returns a pointer to the next free CAN message entry
** inside the FIFO. Please make sure to call CpFifoIsFull() in advance.
** After writing to the FIFO the index has to adjusted by calling
** CpFifoIncIn(), like shown in this code example:
**
** \code
**
** static CpFifo_ts  tsCanFifoS;
** CpCanMsg_ts *     ptsCanNewMsgT;
** CpCanMsg_ts *     ptsCanInEntryT;
**
**
**
** if (!(CpFifoIsFull(&tsCanFifoS)))
** {
**    ptsCanInEntryT = CpFifoDataInPtr(&tsCanFifoS);
**
**    memcpy(ptsCanInEntryT, ptsCanNewMsgT, sizeof(CpCanMsg_ts));
**
**    CpFifoIncIn(&tsCanFifoS);
** }
** \endcode
*/
CpCanMsg_ts *CpFifoDataInPtr(CpFifo_ts *ptsFifoV);


/*!
** \brief   Get first element of FIFO
** \param   ptsFifoV - Pointer to CAN message FIFO
** \return  Pointer to CAN message
**
** This function returns a pointer to the first CAN message entry
** inside the FIFO. Please make sure to call CpFifoIsEmpty() in advance.
** After reading from the FIFO the index has to adjusted by calling
** CpFifoIncOut(), like shown in this code example:
**
** \code
**
** static CpFifo_ts  tsCanFifoS;
** CpCanMsg_ts *     ptsCanNewMsgT;
** CpCanMsg_ts *     ptsCanOutEntryT;
**
**
**
** if (!(CpFifoIsEmpty(&tsCanFifoS)))
** {
**    ptsCanOutEntryT = CpFifoDataOutPtr(&tsCanFifoS);
**
**    memcpy(ptsCanNewMsgT, ptsCanOutEntryT, sizeof(CpCanMsg_ts));
**
**    CpFifoIncout(&tsCanFifoS);
** }
** \endcode
*/
CpCanMsg_ts *CpFifoDataOutPtr(CpFifo_ts *ptsFifoV);


/*!
** \brief   Increment data in pointer
** \param   ptsFifoV - Pointer to CAN message FIFO
**
** This function increments the CpFifo_ts::ulIndexIn element of the
** CAN message FIFO.
*/
void CpFifoIncIn(CpFifo_ts *ptsFifoV);


/*!
** \brief   Increment data out pointer
** \param   ptsFifoV - Pointer to CAN message FIFO
**
** This function increments the CpFifo_ts::ulIndexOut element of the
** CAN message FIFO.
*/
void CpFifoIncOut(CpFifo_ts *ptsFifoV);


/*!
** \brief   Initialise CAN message FIFO
** \param   ptsFifoV - Pointer to CAN message FIFO
** \param   ptsCanMsgV - Pointer to array of CAN messages
** \param   ulSizeV - Size if CAN message array
**
** This function initialises a CA message FIFO. The paramter
** \a ptsCanMsgV points to an array of CpCanMsg_ts elements.
** The number of messages which can be stored inside the array
** is determined by the paramter \a ulSizeV.
**
** Here is an example for initialisation of a CAN message FIFO:
** \code
** ...
** #define NUMBER_OF_FIFO_ENTRIES   20
** static CpFifo_ts     tsCanFifoS;
** static CpCanMsg_ts   atsCanMsgS[NUMBER_OF_FIFO_ENTRIES];
**
** CpFifoInit(&tsCanFifoS, &atsCanMsgS, NUMBER_OF_FIFO_ENTRIES);
**
** \endcode
**
*/
void CpFifoInit(CpFifo_ts *ptsFifoV, CpCanMsg_ts *ptsCanMsgV,
                uint32_t ulSizeV);


/*!
** \brief   Test for FIFO empty
** \param   ptsFifoV - Pointer to CAN message FIFO
** \return  true if FIFO is empty, otherwise false
**
** The function returns \a true if the FIFO is empty, otherwise it will
** return \a false.
*/
bool_t CpFifoIsEmpty(CpFifo_ts *ptsFifoV);



/*!
** \brief   Test for FIFO full
** \param   ptsFifoV - Pointer to CAN message FIFO
** \return  true if FIFO is full, otherwise false
**
** The function returns \a true if the FIFO is full, otherwise it will
** return \a false.
**
*/
bool_t CpFifoIsFull(CpFifo_ts *ptsFifoV);



//-------------------------------------------------------------------//
// Macros for CpFifoXXX() commands                                   //
//-------------------------------------------------------------------//
#if   CP_FIFO_MACRO == 1

#define  CpFifoDataInPtr(FIFO_PTR)                            \
            (((FIFO_PTR)->ptsCanMsg) + ((FIFO_PTR)->ulIndexIn))

#define  CpFifoDataOutPtr(FIFO_PTR) \
      (((FIFO_PTR)->ptsCanMsg) + ((FIFO_PTR)->ulIndexOut))


#define  CpFifoIsEmpty(FIFO_PTR) \
   (((FIFO_PTR)->ulIndexIn == (FIFO_PTR)->ulIndexOut) ? 1 : 0)

#define  CpFifoIsFull(FIFO_PTR)                                            \
            ( (((FIFO_PTR)->ulIndexIn+1 == (FIFO_PTR)->ulIndexOut) ||       \
              (((FIFO_PTR)->ulIndexOut  == 0) &&                             \
               ((FIFO_PTR)->ulIndexIn+1 == (FIFO_PTR)->ulIndexMax) )) ? 1 : 0)

#define  CpFifoIncIn(FIFO_PTR)                                       \
         do {                                                        \
            (FIFO_PTR)->ulIndexIn++;                                 \
            if ((FIFO_PTR)->ulIndexIn >= (FIFO_PTR)->ulIndexMax)     \
            {                                                        \
               (FIFO_PTR)->ulIndexIn = 0;                            \
            }                                                        \
         } while (0)

#define  CpFifoIncOut(FIFO_PTR)                                      \
         do {                                                        \
            (FIFO_PTR)->ulIndexOut++;                                \
            if ((FIFO_PTR)->ulIndexOut >= (FIFO_PTR)->ulIndexMax)    \
            {                                                        \
               (FIFO_PTR)->ulIndexOut = 0;                           \
            }                                                        \
         } while (0)

#define  CpFifoInit(FIFO_PTR, MSG_PTR, SIZE)                         \
         do {                                                        \
            (FIFO_PTR)->ulIndexIn  = 0;                              \
            (FIFO_PTR)->ulIndexOut = 0;                              \
            (FIFO_PTR)->ulIndexMax = (SIZE);                         \
            (FIFO_PTR)->ptsCanMsg  = (MSG_PTR);                      \
         } while (0)
#endif


//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//


#endif   // CP_FIFO_H_
