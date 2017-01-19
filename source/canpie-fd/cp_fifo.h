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




#ifndef  CP_FIFO_H_
#define  CP_FIFO_H_

//-----------------------------------------------------------------------------
/*!
** \file    cp_fifo.h
** \brief   Definitions and prototypes for FIFO implementation
**
*/

/*----------------------------------------------------------------------------*\
** Includes                                                                   **
**                                                                            **
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
** Structures                                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*!
** \struct  CpFifo_s
** \brief   This structure holds administration variables of a FIFO.
**
** Opposite to this structure FIFO macros can be used to manage the
** FIFO access.
*/
struct CpFifo_s {
   /*! Index where the last data has been written to
   */
   uint32_t  ulIndexIn;

   /*! Index where the last data has been read from
   */
   uint32_t  ulIndexOut;

   /*! Maximum number of FIFO entries
   */
   uint32_t  ulIndexMax;

   CpCanMsg_ts * ptsCanMsg;
};
/*!
** \typedef    CpFifo_ts
*/
typedef struct CpFifo_s CpFifo_ts;

/*----------------------------------------------------------------------------*\
** Macros                                                                     **
**                                                                            **
\*----------------------------------------------------------------------------*/

/*!
** \def     CpFifoDataInPtr(FIFO_PTR)
** \return  Pointer to next empty FIFO element.
**
** Value ctrl have to be a (static) variable from type
** of McFifoCtrl_ts structure. Value pdata is pointer to the FIFO array.
**
** \code
**
** static CpFifo_ts  tsCanFifoS;
** CpCanMsg_ts *     ptsCanMsgT
**
**
** if (!(CpFifoIsFull(&tsCanFifoS)))
** {
**    ptsCanMsgT = CpFifoDataInPtr(&tsCanFifoS);
**
**    memcpy(ptsFifoInEntryT, ptsFifoNewEntryT, sizeof(CpCanMsg_ts));
**
**    CpFifoIncIn(&tsCanFifoS);
** }
** \endcode
*/
#define  CpFifoDataInPtr(FIFO_PTR)                            \
            (((FIFO_PTR)->ptsCanMsg) + ((FIFO_PTR)->ulIndexIn))


/*!
** \def     CpFifoDataOutPtr(FIFO_PTR)
** \brief   This macro returns pointer to next FIFO out element.
**
** Value ctrl have to be a (static) variable from type
** of McFifoCtrl_ts structure. Value pdata is pointer to the FIFO array.
**
** \code
** UserFifoEntry_ts * ptsFifoOutEntryT;
**
** if (!(MC_FIFO_EMPTY(tsFifoCntrlS)))
** {
**    ptsFifoOutEntryT = MC_FIFO_DATA_OUT_PTR(tsFifoCntrlS,&atsFifoDataS[0]);
**
**    memcpy(ptsUserDataT, ptsFifoOutEntryT, sizeof(UserFifoEntry_ts));
**
**    // handle ptsUserDataT data from FIFO
**    ...
**
**    MC_FIFO_INC_OUT(tsFifoCntrlS);
** }
** \endcode
*/
#define  CpFifoDataOutPtr(FIFO_PTR)                            \
   do {                                                        \
      (((FIFO_PTR)->ptsCanMsg) + ((FIFO_PTR)->ulIndexOut));    \
   } while(0)


/*!
** \def     CpFifoIsEmpty(ctrl)
** \return  1 if FIFO is empty, otherwise 0
**
** Value x have to be a (static) variable from type of
** McFifoCtrl_ts structure.
** e.g :
** \code
** uint32_t * pulFifoElementT;
** ...
** if (MC_FIFO_EMPTY(tsMcFifoCtrlS))
** {
**    return CpErr_FIFO_EMPTY;
** }
**
** // access to next data of FIFO
** pulFifoElementT = (uint32_t *) MC_FIFO_DATA_OUT_PTR(tsMcFifoCtrlS);
**
** Dummy = *pulFifoElementT;
** ...
** MC_FIFO_INC_OUT(tsMcFifoCtrlS);
** \endcode
*/
#define  CpFifoIsEmpty(FIFO_PTR) \
   (((FIFO_PTR)->ulIndexIn == (FIFO_PTR)->ulIndexOut) ? 1 : 0)



/*!
** \def     CpFifoIsFull(FIFO_PTR)
** \return  1 if FIFO is full, otherwise 0
**
** Value ctrl have to be a (static) variable from type of
** McFifoCtrl_ts structure.
** e.g :
** \code
** uint32_t * pulFifoElementT;
** ...
** if (MC_FIFO_FULL(tsMcFifoCtrlS))
** {
**    return CpErr_FIFO_FULL;
** }
**
** // access to data of FIFO
** pulFifoElementT = (uint32_t *) MC_FIFO_DATA_IN_PTR(tsMcFifoCtrlS);
**
** *pulFifoElementT = Dummy;
** ...
** MC_FIFO_INC_IN(tsMcFifoCtrlS);
** \endcode
**
*/
#define  CpFifoIsFull(FIFO_PTR)                                            \
            ( (((FIFO_PTR)->ulIndexIn+1 == (FIFO_PTR)->ulIndexOut) ||       \
              (((FIFO_PTR)->ulIndexOut  == 0) &&                             \
               ((FIFO_PTR)->ulIndexIn+1 == (FIFO_PTR)->ulIndexMax) )) ? 1 : 0)



/*!
** \def     CpFifoIncIn(ctrl)
** \brief   This macro have to be execute when data was written to the FIFO.
**
** Value ctrl have to be a (static) variable from type of
** McFifoCtrl_ts structure.
*/
#define  CpFifoIncIn(FIFO_PTR)                                       \
         do {                                                        \
            (FIFO_PTR)->ulIndexIn++;                                 \
            if ((FIFO_PTR)->ulIndexIn >= (FIFO_PTR)->ulIndexMax)     \
            {                                                        \
               (FIFO_PTR)->ulIndexIn = 0;                            \
            }                                                        \
         } while (0)


/*!
** \def     CpFifoIncOut(ctrl)
** \brief   This macro have to be execute when data was read from the FIFO.
**
** Value ctrl have to be a (static) variable from type of
** McFifoCtrl_ts structure.
*/
#define  CpFifoIncOut(FIFO_PTR)                                      \
         do {                                                        \
            (FIFO_PTR)->ulIndexOut++;                                \
            if ((FIFO_PTR)->ulIndexOut >= (FIFO_PTR)->ulIndexMax)    \
            {                                                        \
               (FIFO_PTR)->ulIndexOut = 0;                           \
            }                                                        \
         } while (0)


/*!
** \def     CpFifoInit(ctrl,size)
** \brief   Initialise FIFO.
**
** Value ctrl have to be a (static) variable from type of a McFifoCtrl_ts.
** Value size gives number of entries the FIFO contains.
**
** Here is an example for initialisation of FIFO:
** \code
** ...
** // define example FIFO
** #define NUMBER_OF_FIFO_ENTRIES   20
** static McFifoCtrl_ts tsMcFifoCtrlS;
** static uint32_t      aulUserFifoDataS[NUMBER_OF_FIFO_ENTRIES]; // can be an array of struct
** ...
** // initialise example FIFO
** MC_FIFO_INIT(tsMcFifoCtrlS,NUMBER_OF_FIFO_ENTRIES);
**
** \endcode
**
** \attention It is important that NUMBER_OF_FIFO_ENTRIES value that was
** used for atsUserFifoDataS declaration is the same as value
** that is used with MC_FIFO_INIT();
*/
#define  CpFifoInit(FIFO_PTR, MSG_PTR, SIZE)                         \
         do {                                                        \
            (FIFO_PTR)->ulIndexIn  = 0;                              \
            (FIFO_PTR)->ulIndexOut = 0;                              \
            (FIFO_PTR)->ulIndexMax = (SIZE);                         \
            (FIFO_PTR)->ptsCanMsg  = (MSG_PTR);                      \
         } while (0)



/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

#endif   // CP_FIFO_H_
