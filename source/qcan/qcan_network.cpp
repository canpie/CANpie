//============================================================================//
// File:          qcan_network.cpp                                            //
// Description:   QCAN classes - CAN network                                  //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53842 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions         //
// are met:                                                                   //
// 1. Redistributions of source code must retain the above copyright          //
//    notice, this list of conditions, the following disclaimer and           //
//    the referenced file 'COPYING'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'COPYING' file.                   //
//                                                                            //
//============================================================================//


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <QDebug>

#include "qcan_interface.hpp"
#include "qcan_network.hpp"
#include "qcan_stub.hpp"


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define  QCAN_STUB_LIST_SIZE        16

/*----------------------------------------------------------------------------*\
** Static variables                                                           **
**                                                                            **
\*----------------------------------------------------------------------------*/
uint8_t  QCanNetwork::ubNetIdP = 0;

/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanNetwork()                                                              //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanNetwork::QCanNetwork()
{
   //----------------------------------------------------------------
   // each network has a unique network number, starting with 1
   //
   ubNetIdP++;

   pclInterfaceP.clear();

   //----------------------------------------------------------------
   // set default network name
   //
   clNetNameP = "CAN network " + QString("%1").arg(ubNetIdP);


   //----------------------------------------------------------------
   // create initial stub list
   //
   pclStubListP = new QVector<QCanStub *>;
   pclStubListP->reserve(QCAN_STUB_LIST_SIZE);

   //----------------------------------------------------------------
   // start network thread
   //
   this->start();

}


//----------------------------------------------------------------------------//
// ~QCanNetwork()                                                             //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanNetwork::~QCanNetwork()
{
   ubNetIdP--;
   while(this->isRunning())
   {
      this->requestInterruption();
      msleep(1);
   }
}


//----------------------------------------------------------------------------//
// addInterface()                                                             //
// add physical CAN interface (plugin)                                        //
//----------------------------------------------------------------------------//
bool QCanNetwork::addInterface(QCanInterface * pclCanIfV)
{
   pclInterfaceP = pclCanIfV;
   return 0;
}


//----------------------------------------------------------------------------//
// connectStub()                                                              //
// add new QCanStub to the internal list                                      //
//----------------------------------------------------------------------------//
void  QCanNetwork::connectStub(QCanStub * pclStubV)
{
   clStubMutexP.lock();
   pclStubListP->append(pclStubV);
   clStubMutexP.unlock();
}


//----------------------------------------------------------------------------//
// disconnectStub()                                                           //
// remove QCanStub from the internal stub list                                //
//----------------------------------------------------------------------------//
void  QCanNetwork::disconnectStub(QCanStub *pclStubV)
{
   int32_t slIndexT;

   clStubMutexP.lock();
   slIndexT = pclStubListP->indexOf(pclStubV);
   if(slIndexT != -1)
   {
      pclStubListP->remove(slIndexT);
   }
   clStubMutexP.unlock();
}


void  QCanNetwork::handleApiFrame(int32_t & slStubSrcR, QCanFrameApi & clCanFrameR)
{

}

void  QCanNetwork::handleErrorFrame(int32_t & slStubSrcR, QCanFrameError & clErrorFrameR)
{

}

//----------------------------------------------------------------------------//
// pushFrame()                                                                //
// push QCan frame to all open stubs                                          //
//----------------------------------------------------------------------------//
void  QCanNetwork::handleQCanFrame(int32_t & slStubSrcR, QCanFrame & clCanFrameR)
{
   int32_t    slStubIdxT;
   QCanStub * pclStubT;


   //----------------------------------------------------------------
   // check all open stubs and write CAN frame
   //
   for(slStubIdxT = 0; slStubIdxT < pclStubListP->size(); slStubIdxT++)
   {
      if(slStubIdxT != slStubSrcR)
      {
         pclStubT = pclStubListP->at(slStubIdxT);
         pclStubT->pushReceiveFifo(clCanFrameR);
      }
   }

}


//----------------------------------------------------------------------------//
// removeInterface()                                                          //
// remove physical CAN interface (plugin)                                     //
//----------------------------------------------------------------------------//
void QCanNetwork::removeInterface(void)
{

}


//----------------------------------------------------------------------------//
// run()                                                                      //
// QCan Network thread                                                        //
//----------------------------------------------------------------------------//
void QCanNetwork::run()
{
   static int32_t    slStubIdxS;
   static QCanStub * pclStubS;
   static QCanFrame  clCanFrameS;

   //----------------------------------------------------------------
   // Start CAN network thread:
   // It checks all CAN stubs in the list for messages which
   // need to be send (transferred) to other stubs.
   //
   qDebug() << "Start CAN network thread";
   while(1)
   {
      clStubMutexP.lock();

      //--------------------------------------------------------
      // check all open stubs and read messages
      //
      for(slStubIdxS = 0; slStubIdxS < pclStubListP->size(); slStubIdxS++)
      {

         pclStubS = pclStubListP->at(slStubIdxS);
         if(pclStubS->popTransmitFifo(clCanFrameS))
         {

            switch(clCanFrameS.frameType())
            {
               case QCanFrame::eTYPE_QCAN_API:
                  handleApiFrame(slStubIdxS, (QCanFrameApi &) clCanFrameS);
                  break;

               case QCanFrame::eTYPE_QCAN_ERR:
                  handleErrorFrame(slStubIdxS, (QCanFrameError &) clCanFrameS);
                  break;

               default:
                  handleQCanFrame(slStubIdxS, clCanFrameS);
                  break;
            }
         }
      }
      clStubMutexP.unlock();

      //--------------------------------------------------------
      // check if the thread execution shall be stopped
      //
      if ( QThread::currentThread()->isInterruptionRequested() )
      {
         qDebug() << "Quit CAN network thread";
         return;
      }
   }

}

uint32_t  QCanNetwork::stubCount(void)
{
   uint32_t ulCountT;

   ulCountT = pclStubListP->size();

   return(ulCountT);
}



