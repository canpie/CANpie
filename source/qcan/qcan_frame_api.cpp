//============================================================================//
// File:          qcan_frame_api.cpp                                          //
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
#include "qcan_frame_api.hpp"

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanFrameApi()                                                             //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanFrameApi::QCanFrameApi() : CpFrameApi()
{

}


//----------------------------------------------------------------------------//
// QCanFrameApi()                                                             //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanFrameApi::~QCanFrameApi()
{

}


QByteArray QCanFrameApi::toByteArray() const
{
   return(QCanFrame::toByteArray());
}

//----------------------------------------------------------------------------//
// toString()                                                                 //
// print CAN frame                                                            //
//----------------------------------------------------------------------------//
QString QCanFrameApi::toString(const bool & btShowTimeR)
{
   //----------------------------------------------------------------
   // setup a string object
   //
   QString clStringT; //(QCAN_FRAME_STRING_SIZE, '\0');
   
   if(btShowTimeR == true)
   {
      
   }
   
   

   //----------------------------------------------------------------
   // print frame format
   //
   switch(QCanFrame::frameType())
   {
      case eTYPE_CAN_STD:
         clStringT += "CAN-STD ";
         break;
         
      case eTYPE_CAN_EXT:
         clStringT += "CAN-EXT ";
         break;
         
      case eTYPE_FD_STD:
         clStringT += " FD-STD ";
         break;
         
      case eTYPE_FD_EXT:
         clStringT += " FD-EXT ";
         break;
         
      case eTYPE_QCAN_ERR:
         clStringT += "QCan-ERR";
         break;
         
      case eTYPE_QCAN_API:
         clStringT += "QCan-API";
         break;

      default:
         clStringT += " N/A    ";
         break;
   }

   
   return(clStringT);
}
      
