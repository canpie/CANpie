//============================================================================//
// File:          qcan_stub.cpp                                               //
// Description:   QCAN classes - CAN stub                                     //
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

#include "qcan_stub.hpp"

#include <QDebug>

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define  QCAN_STUB_FRAME_FIFO_SIZE     128

/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanStub()                                                                 //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanStub::QCanStub()
{
   clUuidP.createUuid();
   btIsConnectedP = false;

   clRcvFifoP.reserve(QCAN_STUB_FRAME_FIFO_SIZE);
   clTrmFifoP.reserve(QCAN_STUB_FRAME_FIFO_SIZE);

   ubCanStateP = QCan::eCAN_STATE_STOPPED;
}




QCanStub::~QCanStub()
{

}


//----------------------------------------------------------------------------//
// connectNetwork()                                                           //
// connect stub to network                                                    //
//----------------------------------------------------------------------------//
bool QCanStub::connectNetwork(QCanNetwork * pclNetworkV)
{
   bool  btResultT = false;

   if(pclNetworkP.isNull())
   {
      pclNetworkP = pclNetworkV;
      pclNetworkP->connectStub(this);
      btIsConnectedP = true;
      btResultT = true;
   }
   return(btResultT);
}


//----------------------------------------------------------------------------//
// disconnectNetwork()                                                        //
// disconnect stub from network                                               //
//----------------------------------------------------------------------------//
void QCanStub::disconnectNetwork()
{
   if(btIsConnectedP)
   {
      if(!pclNetworkP.isNull())
      {
         pclNetworkP->disconnectStub(this);
         pclNetworkP.clear();
      }
      btIsConnectedP = false;
   }
}


//----------------------------------------------------------------------------//
// popTransmitFifo()                                                          //
// pop CAN frame from transmit FIFO                                           //
//----------------------------------------------------------------------------//
bool QCanStub::popTransmitFifo(QCanFrame & clFrameR)
{
   bool  btResultT = false;

   if(btIsConnectedP)
   {

      clTrmMutexP.lock();
      if(clTrmFifoP.isEmpty() == false)
      {
         clFrameR = clTrmFifoP.takeFirst();
         btResultT = true;
      }
      clTrmMutexP.unlock();
   }
   return(btResultT);
}


//----------------------------------------------------------------------------//
// pushReceiveFifo()                                                          //
// push CAN frame to receive FIFO                                             //
//----------------------------------------------------------------------------//
bool QCanStub::pushReceiveFifo(const QCanFrame & clFrameR)
{
   bool  btResultT = false;

   if(btIsConnectedP)
   {
      clRcvMutexP.lock();
      clRcvFifoP.append(clFrameR);
      btResultT = true;
      clRcvMutexP.unlock();
   }
   return(btResultT);
}


//----------------------------------------------------------------------------//
// readFrame()                                                                //
// read frame from receive FIFO                                               //
//----------------------------------------------------------------------------//
bool QCanStub::readFrame(QCanFrame & clFrameR)
{
   bool  btResultT = false;

   if(btIsConnectedP)
   {

      clRcvMutexP.lock();
      if(clRcvFifoP.isEmpty() == false)
      {
         clFrameR = clRcvFifoP.takeFirst();
         btResultT = true;
      }
      clRcvMutexP.unlock();
   }
   return(btResultT);
}


//----------------------------------------------------------------------------//
// setMode()                                                                  //
// set CAN mode                                                               //
//----------------------------------------------------------------------------//
bool QCanStub::setMode(QCan::CAN_Mode_e & ubModeR)
{
   bool  btResultT = false;

   return(btResultT);
}


//----------------------------------------------------------------------------//
// setFifoSize()                                                              //
// set FIFO size of Rx and Tx FIFOs                                                        //
//----------------------------------------------------------------------------//
bool QCanStub::setFifoSize(uint16_t & uwRcvFifoSizeR,
                           uint16_t & uwTrmFifoSizeR)
{
   bool  btResultT = false;

   return(btResultT);
}


QCan::CAN_State_e QCanStub::state(void)
{
   return(ubCanStateP);
}


//----------------------------------------------------------------------------//
// writeFrame()                                                               //
// write frame to transmit FIFO                                               //
//----------------------------------------------------------------------------//
bool QCanStub::writeFrame(const QCanFrame & clFrameR)
{
   bool  btResultT = false;

   if(btIsConnectedP)
   {
      clTrmMutexP.lock();
      clTrmFifoP.append(clFrameR);
      clTrmMutexP.unlock();
      btResultT = true;
   }
   return(btResultT);

}

