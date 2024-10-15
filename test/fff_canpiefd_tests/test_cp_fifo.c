//============================================================================//
// File:          test_cp_fifo.c                                              //
// Description:   Test cases for MCL FIFO implementation                      //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53844 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// The copyright to the computer program(s) herein is the property of         //
// MicroControl GmbH & Co. KG, Germany. The program(s) may be used            //
// and/or copied only with the written permission of MicroControl GmbH &      //
// Co. KG or in accordance with the terms and conditions stipulated in        //
// the agreement/contract under which the program(s) have been supplied.      //
//                                                                            //
//============================================================================//


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include "fff_mc.h"  // include Fake Function Framework (fff)

#include "cp_fifo.h"
#include "cp_msg.h"

#ifdef _WIN32
#include <windows.h>
#else
#ifdef __STDC_NO_THREADS__
#include <pthread.h>
#include <unistd.h>
#else
#include <threads.h>
#endif
#endif


/*!
** What should be tested for minimum:
** 1. Fifo Full -- read / write
** 2. Fifo Empty -- read / write
** 3. Fifo half full -- read /write
** 4. Fifo last but one full -- read/write
** 5. Fifo empty -- continuous read
** 6. Fifo full -- continuous write
 */
#define  NUMBER_OF_FIFO_ENTRIES     7

static CpFifo_ts     tsFifoS;
static CpCanMsg_ts   atsCanMsgS[NUMBER_OF_FIFO_ENTRIES];



/*----------------------------------------------------------------------------*\
** Test implementation                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

//----------------------------------------------------------------------------//
//                                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
TEST(CP_FIFO, Initialisation)
{
   /*!
    * Testfall:
    * Intialisierung des Filters auf Korrektheit prüfen
    *
    * Ergebnis:
    * Nach der Initialisierung darf die FIFO keine elemente beinhalten
    *
    * Testablauf:
    * 1) FIFO initialsieren
    * 2) Prüfen dass FIFO leer ist
    * 3) Prüfen dass FIFO nicht voll ist
    * 4) Prüfe auf Speicherüberlauf
    */

   // 1) FIFO initialsieren
   CpFifoInit(&tsFifoS,&atsCanMsgS[0],NUMBER_OF_FIFO_ENTRIES);

   // 2) Prüfen dass FIFO leer ist
   ASSERT_EQ(CpFifoIsEmpty(&tsFifoS), true);

   // 3)Prüfen dass FIFO nicht voll ist
   ASSERT_EQ(CpFifoIsFull(&tsFifoS), false);

   // 4) Prüfe, dass der Index nicht größer ist als die definierte FIFO Größe!
   // In anderem Fall liegt ein Speicherüberlauf vor!
   ASSERT_TRUE((tsFifoS.ulIndexMax) <= (sizeof(atsCanMsgS) /  sizeof(CpCanMsg_ts)));
}

//----------------------------------------------------------------------------//
//                                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
TEST(CP_FIFO, WriteAndRead)
{
   /*!
    * Testfall:
    * In eine leere FIFO wird ein element geschrieben und gelesen.
    *
    * Ergebnis:
    * Die FIFO muss leer sein
    *
    * Testablauf:
    * 1) Element in die FIFO schreiben
    * 2) Element lesen
    * 3) Prüfen, dass FIFO leer ist und daten gültig
    */

   CpCanMsg_ts *ptsFifoEntryT;
   uint32_t  ulFifoInDataT;
   uint32_t  ulFifoOutDataT;

   // 1) Element in die FIFO schreiben
   ulFifoInDataT = 0x12345678;
   ptsFifoEntryT = CpFifoDataInPtr(&tsFifoS);
   ptsFifoEntryT->tuMsgData.aulLong[0] = ulFifoInDataT;
   CpFifoIncIn(&tsFifoS);

   // 2) Element lesen
   ulFifoOutDataT = 0x00000000;
   ptsFifoEntryT = CpFifoDataOutPtr(&tsFifoS);
   ulFifoOutDataT = ptsFifoEntryT->tuMsgData.aulLong[0];
   CpFifoIncOut(&tsFifoS);

   // 3) Prüfen, dass FIFO leer ist und daten gültig
   ASSERT_EQ(CpFifoIsEmpty(&tsFifoS), 1);
   ASSERT_EQ(ulFifoInDataT, ulFifoOutDataT);
}

//----------------------------------------------------------------------------//
//                                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
TEST(CP_FIFO, WriteTillFull_ReadTillEmpty)
{
   /*!
    * Testfall:
    * In eine leere FIFO wird maximale Anzahl an elementen geschrrieben.
    *
    * Ergebnis:
    * Die FIFO muss voll sein
    *
    * Testablauf:
    * 0) Neuinitialisierung der FIFO
    * 1) NUMBER_OF_FIFO_ENTRIES Elemente in die FIFO schreiben
    * 2) Prüfen, dass FIFO voll ist und daten gültig
    * 3) NUMBER_OF_FIFO_ENTRIES Element aus FIFO lesen, Daten prüfen
    * 4) Prüfen, dass FIFO leer ist
    */

	CpCanMsg_ts *ptsFifoEntryT;
   uint32_t  slFifoInDataT = 0xFACE0000;
   uint8_t   ubCntrT;

   // 0) Neuinitialisierung der FIFO
   CpFifoInit(&tsFifoS,&atsCanMsgS[0],NUMBER_OF_FIFO_ENTRIES);

   // 1) NUMBER_OF_FIFO_ENTRIES Elemente in die FIFO schreiben
   ubCntrT = 0;
//   printf("Pending number %i, free number %i\n", CpFifoPending(&tsFifoS),CpFifoFree(&tsFifoS));

   while (ubCntrT < NUMBER_OF_FIFO_ENTRIES)
   {
      ptsFifoEntryT = CpFifoDataInPtr(&tsFifoS);
      ptsFifoEntryT->tuMsgData.aulLong[0] = ubCntrT + slFifoInDataT;
//      printf("Content of value %u: %lx\n",ubCntrT,*pslFifoEntryT);
      CpFifoIncIn(&tsFifoS);

   //   printf("Pending number after in %i, free number %i\n", CpFifoPending(&tsFifoS),CpFifoFree(&tsFifoS));
      ubCntrT++;
      ASSERT_EQ(CpFifoPending(&tsFifoS),ubCntrT);
   }

   // 2) Prüfen, dass FIFO voll ist und daten gültig
   ASSERT_EQ(CpFifoIsFull(&tsFifoS), 1);

   // 3)  NUMBER_OF_FIFO_ENTRIES Element aus FIFO lesen, Daten prüfen
   ubCntrT = 0;
   while (ubCntrT < NUMBER_OF_FIFO_ENTRIES)
   {
      ptsFifoEntryT = CpFifoDataOutPtr(&tsFifoS);
//      printf("Content of value %u: %lx\n",ubCntrT,*pslFifoEntryT);
      ASSERT_EQ((ptsFifoEntryT->tuMsgData.aulLong[0]), slFifoInDataT + ubCntrT);

      // decrement number of entries
      CpFifoIncOut(&tsFifoS);
//      printf("Pending number after out %i, free number %i\n", CpFifoPending(&tsFifoS),CpFifoFree(&tsFifoS));
      ubCntrT++;
      ASSERT_EQ(CpFifoFree(&tsFifoS),ubCntrT);
   }

   // 4) Prüfen, dass FIFO leer ist
   ASSERT_EQ(CpFifoIsEmpty(&tsFifoS), 1);
}


TEST(CP_FIFO,FifoPending_001)
{
   /*!
    * Testfall:
    * In eine leere FIFO werden Elemente rein geschrieben und gelesen, zwischenzeitlich wird auf das korrekte
    * "Pending" geprüft.
    *
    * Ergebnis:
    * "Pending" und "Free" der FIFO stimmen immer mit dem erwarteten Wert überein.
    *
    * Testablauf:
    * 0) Neuinitialisierung der FIFO
    * n) Elemente einzeln und mehrfach hinzufügen und entfernen und immer "Pending" und "Free" Werte prüfen
    */

   // 0) Neuinitialisierung der FIFO
   CpFifoInit(&tsFifoS,&atsCanMsgS[0],NUMBER_OF_FIFO_ENTRIES);

   // ein Element hinzufügen
   CpFifoIncIn(&tsFifoS);
   ASSERT_EQ(CpFifoPending(&tsFifoS),1);
//   printf("Number after 1x in pending %i, free %i, total %i\n", CpFifoPending(&tsFifoS),CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES);
   ASSERT_EQ(CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES-1);

   // ein Element entfernen
   CpFifoIncOut(&tsFifoS);
//   printf("Number after 1x out pending %i, free %i, total %i\n", CpFifoPending(&tsFifoS),CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES);
   ASSERT_EQ(CpFifoPending(&tsFifoS),0);
   ASSERT_EQ(CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES);

   CpFifoIncIn(&tsFifoS);
   CpFifoIncIn(&tsFifoS);
//   printf("Number after 2x in pending %i, free %i, total %i\n", CpFifoPending(&tsFifoS),CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES);
   ASSERT_EQ(CpFifoPending(&tsFifoS),2);
   ASSERT_EQ(CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES-2);


   CpFifoIncOut(&tsFifoS);
//   printf("Number after 1x out pending %i, free %i, total %i\n", CpFifoPending(&tsFifoS),CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES);
   ASSERT_EQ(CpFifoPending(&tsFifoS),1);
   ASSERT_EQ(CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES-1);

   CpFifoIncIn(&tsFifoS);
//   printf("Number after 1x in pending %i, free %i, total %i\n", CpFifoPending(&tsFifoS),CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES);
   ASSERT_EQ(CpFifoPending(&tsFifoS),2);
   ASSERT_EQ(CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES-2);

   CpFifoIncIn(&tsFifoS);
   CpFifoIncIn(&tsFifoS);
   CpFifoIncIn(&tsFifoS);
   CpFifoIncIn(&tsFifoS);
   CpFifoIncIn(&tsFifoS);
//   printf("Number after 5x in pending %i, free %i, total %i\n", CpFifoPending(&tsFifoS),CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES);
   ASSERT_EQ(CpFifoPending(&tsFifoS),NUMBER_OF_FIFO_ENTRIES);
   ASSERT_EQ(CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES-7);
   ASSERT_EQ(CpFifoIsFull(&tsFifoS),1);
   ASSERT_EQ(CpFifoIsEmpty(&tsFifoS),0);

   CpFifoIncOut(&tsFifoS);
//   printf("Number after 1x out pending %i, free %i, total %i\n", CpFifoPending(&tsFifoS),CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES);
   ASSERT_EQ(CpFifoPending(&tsFifoS),NUMBER_OF_FIFO_ENTRIES-1);
   ASSERT_EQ(CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES-6);

   CpFifoIncOut(&tsFifoS);
//   printf("Number after 1x out pending %i, free %i, total %i\n", CpFifoPending(&tsFifoS),CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES);
   ASSERT_EQ(CpFifoPending(&tsFifoS),NUMBER_OF_FIFO_ENTRIES-2);
   ASSERT_EQ(CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES-5);

   CpFifoIncOut(&tsFifoS);
   CpFifoIncOut(&tsFifoS);
   CpFifoIncOut(&tsFifoS);
   CpFifoIncOut(&tsFifoS);
   CpFifoIncOut(&tsFifoS);
//   printf("Number after 5x out pending %i, free %i, total %i\n", CpFifoPending(&tsFifoS),CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES);
   ASSERT_EQ(CpFifoPending(&tsFifoS),NUMBER_OF_FIFO_ENTRIES-7);
   ASSERT_EQ(CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES);

   ASSERT_EQ(CpFifoIsFull(&tsFifoS),0);
   ASSERT_EQ(CpFifoIsEmpty(&tsFifoS),1);

   CpFifoIncIn(&tsFifoS);
//   printf("Number after 1x in pending %i, free %i, total %i\n", CpFifoPending(&tsFifoS),CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES);
   ASSERT_EQ(CpFifoPending(&tsFifoS),1);
   ASSERT_EQ(CpFifoFree(&tsFifoS),NUMBER_OF_FIFO_ENTRIES-1);
}


TEST(CP_FIFO, UsageOfFifoArray)
{
   #define NUMBER_OF_FIFOS 5
   static CpFifo_ts   atsMulFifoCntrlS[NUMBER_OF_FIFOS];
   static CpCanMsg_ts atsMulFifoDataS[NUMBER_OF_FIFOS][NUMBER_OF_FIFO_ENTRIES];

   /*!
    * Testfall:
    * Intialisierung des Filters auf Korrektheit prüfen
    *
    * Ergebnis:
    * Nach der Initialisierung darf die FIFO keine elemente beinhalten
    *
    * Testablauf:
    * 1) Filter initialsieren
    * 2) Prüfen dass FIFO leer ist
    * 3) Prüfen dass FIFO nicht voll ist
    * 4) Prüfe auf Speicherüberlauf
    */
   uint8_t ubCntrT;
   uint8_t ubFifoCntrT = 0;
   while (ubFifoCntrT < NUMBER_OF_FIFOS)
   {
      // 1) Filter initialsieren
      CpFifoInit(&atsMulFifoCntrlS[ubFifoCntrT],&atsMulFifoDataS[ubFifoCntrT][0],NUMBER_OF_FIFO_ENTRIES);

      // 2) Prüfen dass FIFO leer ist
      ASSERT_EQ(CpFifoIsEmpty(&atsMulFifoCntrlS[ubFifoCntrT]), 1);

      // 3)Prüfen dass FIFO nicht voll ist
      ASSERT_EQ(CpFifoIsFull(&atsMulFifoCntrlS[ubFifoCntrT]), 0);

      // 4) Prüfe, dass der Index nicht größer ist als die definierte FIFO Größe!
      // In anderem Fall liegt ein Speicherüberlauf vor!
      ASSERT_TRUE((atsMulFifoCntrlS[ubFifoCntrT].ulIndexMax) <= (sizeof(atsMulFifoDataS) /  sizeof(uint32_t)));
      // 4) Prüfe die Gesamtlänge
      ASSERT_TRUE((atsMulFifoCntrlS[ubFifoCntrT].ulIndexMax) <= (sizeof(atsMulFifoDataS) /  (sizeof(uint32_t))*NUMBER_OF_FIFOS));

      ubFifoCntrT++;
   }



   /*!
    * Testfall:
    * In eine leere FIFO wird maximale Anzahl an elementen geschrrieben.
    *
    * Ergebnis:
    * Die FIFO muss voll sein
    *
    * Testablauf:
    * 1) NUMBER_OF_FIFO_ENTRIES Elemente in die FIFO schreiben
    * 2) Prüfen, dass FIFO voll ist und daten gültig
    * 3) NUMBER_OF_FIFO_ENTRIES Element aus FIFO lesen, Daten prüfen
    * 4) Prüfen, dass FIFO leer ist
    */

   CpCanMsg_ts *ptsFifoEntryT;
   uint32_t  slFifoInDataT = 0xFACE0000;

   // 1) NUMBER_OF_FIFO_ENTRIES Elemente in die FIFO schreiben
   ubFifoCntrT = 0;
   while (ubFifoCntrT < NUMBER_OF_FIFOS)
   {
      ubCntrT = 0;
      while (ubCntrT < NUMBER_OF_FIFO_ENTRIES)
      {
         ptsFifoEntryT = CpFifoDataInPtr(&atsMulFifoCntrlS[ubFifoCntrT]);
         ptsFifoEntryT->tuMsgData.aulLong[0] = ubCntrT + slFifoInDataT;
   //      printf("Content of value %u: %lx\n",ubCntrT,*pslFifoEntryT);
         CpFifoIncIn(&atsMulFifoCntrlS[ubFifoCntrT]);
         ubCntrT++;
      }


      // 2) Prüfen, dass FIFO voll ist und daten gültig
      ASSERT_EQ(CpFifoIsFull(&atsMulFifoCntrlS[ubFifoCntrT]), 1);

      // 3)  NUMBER_OF_FIFO_ENTRIES Element aus FIFO lesen, Daten prüfen
      ubCntrT = 0;
      while (ubCntrT < NUMBER_OF_FIFO_ENTRIES)
      {
         ptsFifoEntryT = CpFifoDataOutPtr(&atsMulFifoCntrlS[ubFifoCntrT]);
   //      printf("Content of value %u: %lx\n",ubCntrT,*pslFifoEntryT);

         ASSERT_EQ((ptsFifoEntryT->tuMsgData.aulLong[0]), slFifoInDataT + ubCntrT);

         // decrement number of entries
         CpFifoIncOut(&atsMulFifoCntrlS[ubFifoCntrT]);
         ubCntrT++;
      }

      // 4) Prüfen, dass FIFO leer ist
      ASSERT_EQ(CpFifoIsEmpty(&atsMulFifoCntrlS[ubFifoCntrT]), 1);


      ubFifoCntrT++;
   }
}


TEST(CP_FIFO,FifoWriteFull_001)
{
   /*!
    * Testfall:
    * In eine leere FIFO werden Elemente rein geschrieben bis diese voll ist. Anschließen wird ein Element 
    * raus genommen und wieder voll geschrieben.
    *
    * Ergebnis:
    * Zugriff auf die richtigen FIFO stimmen immer mit dem erwarteten Wert überein.
    *
    * Testablauf:
    * 0) Neuinitialisierung der FIFO
    * n) Elemente einzeln und mehrfach hinzufügen und entfernen 
    */
   CpCanMsg_ts *ptsFifoEntryT;
   uint32_t  ulFifoInDataT;
   uint32_t  ulFifoOutDataT;

   // 0) Neuinitialisierung der FIFO
   CpFifoInit(&tsFifoS,&atsCanMsgS[0],NUMBER_OF_FIFO_ENTRIES);


   // 1) Die FIFO voll schreiben
   uint32_t ulIndexT = 0;
   while (ulIndexT < NUMBER_OF_FIFO_ENTRIES)
   {
      ulFifoInDataT = 0x01+ulIndexT;
      ptsFifoEntryT = CpFifoDataInPtr(&tsFifoS);
      ptsFifoEntryT->tuMsgData.aulLong[0] = ulFifoInDataT;
      CpFifoIncIn(&tsFifoS);

      ulIndexT++;
   }
   ASSERT_EQ(CpFifoIsFull(&tsFifoS), 1);


   // 2) 1 Element lesen
   ulFifoOutDataT = 0x00000000;
   ptsFifoEntryT = CpFifoDataOutPtr(&tsFifoS);
   ulFifoOutDataT = ptsFifoEntryT->tuMsgData.aulLong[0];
   CpFifoIncOut(&tsFifoS);

   // 3) Prüfen, dass FIFO nicht voll ist und daten gültig
   ASSERT_EQ(CpFifoIsFull(&tsFifoS), 0);
   ASSERT_EQ(0x01, ulFifoOutDataT);


   // weiteres element hinzufügen, so dass die fifo wieder voll ist
   ulFifoInDataT = NUMBER_OF_FIFO_ENTRIES+5;
   ptsFifoEntryT = CpFifoDataInPtr(&tsFifoS);
   ptsFifoEntryT->tuMsgData.aulLong[0] = ulFifoInDataT;
   CpFifoIncIn(&tsFifoS);

   ASSERT_EQ(CpFifoIsFull(&tsFifoS), 1);

   // Alle elemente lesen und auf Gültigkeit prüfen
   ulIndexT = 0;
   while (ulIndexT < NUMBER_OF_FIFO_ENTRIES-1)
   {
      ulFifoOutDataT = 0;
      ptsFifoEntryT = CpFifoDataOutPtr(&tsFifoS);
      ulFifoOutDataT = ptsFifoEntryT->tuMsgData.aulLong[0];
      CpFifoIncOut(&tsFifoS);

      ASSERT_EQ(0x02+ulIndexT, ulFifoOutDataT);

      ulIndexT++;
   }   

   ASSERT_EQ(CpFifoPending(&tsFifoS),1);

   ulFifoOutDataT = 0;
   ptsFifoEntryT = CpFifoDataOutPtr(&tsFifoS);
   ulFifoOutDataT = ptsFifoEntryT->tuMsgData.aulLong[0];
   CpFifoIncOut(&tsFifoS);

   ASSERT_EQ(NUMBER_OF_FIFO_ENTRIES+5, ulFifoOutDataT);

}


TEST(CP_FIFO,FifoWriteFull_002)
{
   /*!
    * Testfall:
    * Während aus der FIFO Elemente gelesen werden, werden neue geschrieben
    *
    * Ergebnis:
    * Zugriff auf die richtigen FIFO stimmen immer mit dem erwarteten Wert überein.
    *
    * Testablauf:
    * 0) Neuinitialisierung der FIFO
    * n) Elemente einzeln und mehrfach hinzufügen und entfernen 
    */
   CpCanMsg_ts *ptsFifoEntryT;
   uint32_t  ulFifoInDataT;
   uint32_t  ulFifoOutDataT;

   // 0) Neuinitialisierung der FIFO
   CpFifoInit(&tsFifoS,&atsCanMsgS[0],NUMBER_OF_FIFO_ENTRIES);


   // 1) Die FIFO voll schreiben
   uint32_t ulIndexT = 0;
   while (ulIndexT < NUMBER_OF_FIFO_ENTRIES)
   {
      ulFifoInDataT = 0x01+ulIndexT;
      ptsFifoEntryT = CpFifoDataInPtr(&tsFifoS);
      ptsFifoEntryT->tuMsgData.aulLong[0] = ulFifoInDataT;
      CpFifoIncIn(&tsFifoS);

      ulIndexT++;
   }
   ASSERT_EQ(CpFifoIsFull(&tsFifoS), 1);


   // 2) 1 Element lesen und während dessen versuchen ein Elment zu schreiben
   ulIndexT = 0;
   while (ulIndexT < NUMBER_OF_FIFO_ENTRIES)
   {
      ulFifoOutDataT = 0x00000000;
      ptsFifoEntryT = CpFifoDataOutPtr(&tsFifoS);
      ulFifoOutDataT = ptsFifoEntryT->tuMsgData.aulLong[0];
      CpFifoIncOut(&tsFifoS);

      // Prüfen, dass FIFO nicht voll ist und daten gültig
      ASSERT_EQ(CpFifoIsFull(&tsFifoS), 0);
      ASSERT_EQ( 0x01+ulIndexT, ulFifoOutDataT);

      // Neues Element in die FIFO schreiben
      ulFifoInDataT = 0x10+ulIndexT;
      ptsFifoEntryT = CpFifoDataInPtr(&tsFifoS);
      ptsFifoEntryT->tuMsgData.aulLong[0] = ulFifoInDataT;
      CpFifoIncIn(&tsFifoS);

      ulIndexT++;

      // check FIFO is FULL again
      ASSERT_EQ(CpFifoIsFull(&tsFifoS), 1);
   }

   // Jetzt 3 Elemente entfernen
   ulIndexT = 0;
   while (ulIndexT < 3)
   {
      ulFifoOutDataT = 0x00000000;
      ptsFifoEntryT = CpFifoDataOutPtr(&tsFifoS);
      ulFifoOutDataT = ptsFifoEntryT->tuMsgData.aulLong[0];
      CpFifoIncOut(&tsFifoS);

      // Prüfen, dass FIFO nicht voll ist und daten gültig
      ASSERT_EQ(CpFifoIsFull(&tsFifoS), 0);
      ASSERT_EQ( 0x10+ulIndexT, ulFifoOutDataT);

      ulIndexT++;
   }

   // Neues Element in die FIFO schreiben und prüfen, dass diese nicht voll ist
   ulFifoInDataT = 0x10+ulIndexT;
   ptsFifoEntryT = CpFifoDataInPtr(&tsFifoS);
   ptsFifoEntryT->tuMsgData.aulLong[0] = ulFifoInDataT;
   CpFifoIncIn(&tsFifoS);
   ASSERT_EQ(CpFifoIsFull(&tsFifoS), 0);

}

TEST(CP_FIFO, CopyFifo)
{
   CpFifo_ts      tsDestFifoT;
   CpCanMsg_ts    atsDestCanMsgT[NUMBER_OF_FIFO_ENTRIES];
   CpCanMsg_ts    *ptsFifoEntryT;

   //---------------------------------------------------------------------------------------------------
   // Initialise the source FIFO
   //
   CpFifoInit(&tsFifoS, &atsCanMsgS[0], NUMBER_OF_FIFO_ENTRIES);

   //---------------------------------------------------------------------------------------------------
   // Initialise the destination FIFO with wrong size
   //
   CpFifoInit(&tsDestFifoT, &atsDestCanMsgT[0], NUMBER_OF_FIFO_ENTRIES - 2);

   //---------------------------------------------------------------------------------------------------
   // FIFO copy shall fail because of size
   //
   ASSERT_EQ(CpFifoCopy(&tsDestFifoT, &tsFifoS), false);

   //---------------------------------------------------------------------------------------------------
   // Initialise the destination FIFO with correct size
   //
   CpFifoInit(&tsDestFifoT, &atsDestCanMsgT[0], NUMBER_OF_FIFO_ENTRIES);

   //---------------------------------------------------------------------------------------------------
   // Put data into destination FIFO
   //
   ptsFifoEntryT = CpFifoDataInPtr(&tsDestFifoT);
   CpMsgInit(ptsFifoEntryT, CP_MSG_FORMAT_CBFF);
   CpMsgSetIdentifier(ptsFifoEntryT, 0x100);
   CpFifoIncIn(&tsDestFifoT);
   ASSERT_EQ(CpFifoPending(&tsDestFifoT), 1);

   //---------------------------------------------------------------------------------------------------
   // FIFO copy shall succeed
   //
   ASSERT_EQ(CpFifoCopy(&tsDestFifoT, &tsFifoS), true);

   //---------------------------------------------------------------------------------------------------
   // Destination FIFO shall have 0 messages 
   //
   ASSERT_EQ(CpFifoPending(&tsDestFifoT), 0);

   //---------------------------------------------------------------------------------------------------
   // Put data into source FIFO
   //
   for (uint8_t ubMsgCntT = 0; ubMsgCntT < NUMBER_OF_FIFO_ENTRIES; ubMsgCntT++)
   {
      ptsFifoEntryT = CpFifoDataInPtr(&tsFifoS);
      CpMsgInit(ptsFifoEntryT, CP_MSG_FORMAT_CBFF);
      CpMsgSetIdentifier(ptsFifoEntryT, 0x100 + ubMsgCntT);
      CpFifoIncIn(&tsFifoS);
   }
   ASSERT_EQ(CpFifoPending(&tsFifoS), NUMBER_OF_FIFO_ENTRIES);

   //---------------------------------------------------------------------------------------------------
   // Remove 4 messages from source FIFO
   //
   CpFifoIncOut(&tsFifoS);
   CpFifoIncOut(&tsFifoS);
   CpFifoIncOut(&tsFifoS);
   CpFifoIncOut(&tsFifoS);
   ASSERT_EQ(CpFifoPending(&tsFifoS), NUMBER_OF_FIFO_ENTRIES - 4);

   //---------------------------------------------------------------------------------------------------
   // FIFO copy shall succeed
   //
   ASSERT_EQ(CpFifoCopy(&tsDestFifoT, &tsFifoS), true);

   //---------------------------------------------------------------------------------------------------
   // Destination FIFO shall have 3 messages 
   //
   ASSERT_EQ(CpFifoPending(&tsDestFifoT), 3);

   //---------------------------------------------------------------------------------------------------
   // Check contents of destination FIFO 
   //
   ptsFifoEntryT = CpFifoDataOutPtr(&tsDestFifoT);
   ASSERT_EQ(CpMsgGetIdentifier(ptsFifoEntryT), 0x104);
   CpFifoIncOut(&tsDestFifoT);

   ptsFifoEntryT = CpFifoDataOutPtr(&tsDestFifoT);
   ASSERT_EQ(CpMsgGetIdentifier(ptsFifoEntryT), 0x105);
   CpFifoIncOut(&tsDestFifoT);

   ptsFifoEntryT = CpFifoDataOutPtr(&tsDestFifoT);
   ASSERT_EQ(CpMsgGetIdentifier(ptsFifoEntryT), 0x106);
   CpFifoIncOut(&tsDestFifoT);

   //---------------------------------------------------------------------------------------------------
   // Destination FIFO shall have 0 messages 
   //
   ASSERT_EQ(CpFifoPending(&tsDestFifoT), 0);
}

#ifdef __STDC_NO_THREADS__

#define  THREAD_MSG_COUNT  2000

static void ThreadPushDataIn(void * pvdDataV)
{
   CpCanMsg_ts tsCanInMsgT;
   CpCanMsg_ts *ptsFifoEntryT;
   uint32_t    ulMsgCountT = 0;
   CpFifo_ts * ptsFifoT;

   ptsFifoT = pvdDataV;
   CpMsgInit(&tsCanInMsgT, CP_MSG_FORMAT_CBFF);   // initialise message
   CpMsgSetIdentifier(&tsCanInMsgT, 100);         // identifier is 100 dec.
   CpMsgSetDlc(&tsCanInMsgT, 2);                  // data length code is 2
   CpMsgSetData(&tsCanInMsgT, 0, 0x11);           // byte 0 has the value 0x11
   CpMsgSetData(&tsCanInMsgT, 1, 0x22);           // byte 1 has the value 0x22

   while (ulMsgCountT < THREAD_MSG_COUNT)
   {
      CpMsgSetIdentifier(&tsCanInMsgT, ulMsgCountT); // identifier is equal to  dec.
      if (CpFifoIsFull(ptsFifoT) == false)
      {
         ptsFifoEntryT = CpFifoDataInPtr(ptsFifoT);
         memcpy(ptsFifoEntryT, &tsCanInMsgT, sizeof(CpCanMsg_ts));
         CpFifoIncIn(ptsFifoT);
         ulMsgCountT++;
      }
      else
      {
         usleep(100);
      }
      usleep(2000);
   }

   printf("Write %d messages to FIFO \n", ulMsgCountT);

}

static void ThreadGetDataOut(void * pvdDataV)
{
   CpCanMsg_ts tsCanOutMsgT;
   CpCanMsg_ts *ptsFifoEntryT;
   uint32_t    ulMsgCountT = 0;
   uint8_t     ubWaitT = 200;
   CpFifo_ts * ptsFifoT;

   ptsFifoT = pvdDataV;

   while (ubWaitT > 0)
   {
      if (CpFifoIsEmpty(ptsFifoT) == true)
      {
         ubWaitT--;
         usleep(100);
      }
      else
      {
         ubWaitT = 200;
         ptsFifoEntryT = CpFifoDataOutPtr(ptsFifoT);
         memcpy(&tsCanOutMsgT, ptsFifoEntryT, sizeof(CpCanMsg_ts));
         ASSERT_EQ(CpMsgGetIdentifier(&tsCanOutMsgT), ulMsgCountT);
         CpFifoIncOut(ptsFifoT);
         ulMsgCountT++;
      }
   }

   printf("Read %d messages from FIFO \n", ulMsgCountT);
}

TEST(CP_FIFO, Fifo_ThreadCheck)
{
   pthread_t   tsThreadFifoInT;
   pthread_t   tsThreadFifoOutT;

   CpFifoInit(&tsFifoS, &atsCanMsgS[0], NUMBER_OF_FIFO_ENTRIES);

   pthread_create(&tsThreadFifoInT, NULL, (void *) &ThreadPushDataIn, &tsFifoS);

   pthread_create(&tsThreadFifoOutT, NULL, (void *) &ThreadGetDataOut, &tsFifoS);

   pthread_join(tsThreadFifoInT, NULL);
   pthread_join(tsThreadFifoOutT, NULL);
}
#endif


//----------------------------------------------------------------------------//
// Reset all tests of this module                                             //
//                                                                            //
//----------------------------------------------------------------------------//
TEST_SUITE_RESET(CP_FIFO)
{

}

//----------------------------------------------------------------------------//
// Run all tests of this module                                               //
//                                                                            //
//----------------------------------------------------------------------------//
TEST_SUITE_RUN(CP_FIFO)
{
   RUN_TEST(CP_FIFO, Initialisation);
   RUN_TEST(CP_FIFO, WriteAndRead);
   RUN_TEST(CP_FIFO, WriteTillFull_ReadTillEmpty);
   RUN_TEST(CP_FIFO, UsageOfFifoArray);
   RUN_TEST(CP_FIFO, FifoPending_001);
   RUN_TEST(CP_FIFO, FifoWriteFull_001);
   RUN_TEST(CP_FIFO, FifoWriteFull_002);
   RUN_TEST(CP_FIFO, CopyFifo);

   #ifdef __STDC_NO_THREADS__
   RUN_TEST(CP_FIFO, Fifo_ThreadCheck);
   #endif

}




