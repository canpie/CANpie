//============================================================================//
// File:          qcan_frame.cpp                                              //
// Description:   QCAN Server - Network implementation                        //
// Author:        Uwe Koppe                                                   //
// e-mail:        koppe@microcontrol.net                                      //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// Junkersring 23                                                             //
// 53844 Troisdorf                                                            //
// Germany                                                                    //
// Tel: +49-2241-25659-0                                                      //
// Fax: +49-2241-25659-11                                                     //
//                                                                            //
// The copyright to the computer program(s) herein is the property of         //
// MicroControl GmbH & Co. KG, Germany. The program(s) may be used            //
// and/or copied only with the written permission of MicroControl GmbH &      //
// Co. KG or in accordance with the terms and conditions stipulated in        //
// the agreement/contract under which the program(s) have been supplied.      //
//----------------------------------------------------------------------------//
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 20.05.2015  Initial version                                                //
//                                                                            //
//============================================================================//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-09-15 18:50:11 +0200 (Mo, 15. Sep 2014) $
// SVN  $Rev: 6156 $ --- $Author: koppe $
//------------------------------------------------------------------------------


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <QDebug>
#include "qcan_frame_error.hpp"

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanFrameError()                                                           //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanFrameError::QCanFrameError() : CpFrameError()
{

}


//----------------------------------------------------------------------------//
// QCanFrame()                                                                //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanFrameError::~QCanFrameError()
{
   
}

//----------------------------------------------------------------------------//
// fromByteArray()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanFrameError::fromByteArray(const QByteArray & clByteArrayR)
{
   return(QCanFrame::fromByteArray(clByteArrayR));
}

QByteArray QCanFrameError::toByteArray() const
{
   return QCanFrame::toByteArray();
}

//----------------------------------------------------------------------------//
// toString()                                                                 //
// print CAN frame                                                            //
//----------------------------------------------------------------------------//
QString QCanFrameError::toString(const bool & btShowTimeR)
{
   //----------------------------------------------------------------
   // setup a string object
   //
   QString clStringT;
   
   if(btShowTimeR == true)
   {
      
   }
   

   //----------------------------------------------------------------
   // print frame format
   //
   clStringT += "CAN error frame   ";

   switch(this->errorState())
   {
      case eCAN_STATE_BUS_ACTIVE:
         clStringT += "Error active";
         break;

      case eCAN_STATE_BUS_WARN:
         clStringT += "Warning level reached";
         break;

      case eCAN_STATE_BUS_PASSIVE:
         clStringT += "Error passive";
         break;

      case eCAN_STATE_BUS_OFF:
         clStringT += "Bus off";
         break;

      default:

         break;
   }
   
   return(clStringT);
}
      
