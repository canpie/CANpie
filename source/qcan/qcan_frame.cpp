//============================================================================//
// File:          qcan_frame.cpp                                              //
// Description:   QCAN classes - CAN frame                                    //
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
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <QCanFrame>


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanFrame()                                                                //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanFrame::QCanFrame() : CpFrame()
{

}


//----------------------------------------------------------------------------//
// QCanFrame()                                                                //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanFrame::QCanFrame(const Format_e & ubFormatR, const uint32_t & ulIdentifierR, 
                     const uint8_t & ubDlcR) 
         : CpFrame(ubFormatR, ulIdentifierR, ubDlcR)
{
}


//----------------------------------------------------------------------------//
// ~QCanFrame()                                                               //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanFrame::~QCanFrame()
{

}



//----------------------------------------------------------------------------//
// data()                                                                     //
// get data                                                                   //
//----------------------------------------------------------------------------//
QByteArray  QCanFrame::data(void) const
{
   QByteArray clDataT; //((const char *)&data(0), this->dataSize());
   return(clDataT);
}

uint8_t QCanFrame::data(const uint8_t & ubPosR) const
{
   return(CpFrame::data(ubPosR));
}


//----------------------------------------------------------------------------//
// fromByteArray()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanFrame::fromByteArray(const QByteArray & clByteArrayR)
{
   return(CpFrame::fromByteArray(clByteArrayR));
}


void QCanFrame::setData(const uint8_t & ubPosR, const uint8_t & ubValueR)
{
   CpFrame::setData(ubPosR, ubValueR);
}

QByteArray QCanFrame::toByteArray() const
{
   return(CpFrame::toByteArray());
}

//----------------------------------------------------------------------------//
// toString()                                                                 //
// print CAN frame                                                            //
//----------------------------------------------------------------------------//
QString QCanFrame::toString(const bool & btShowTimeR) 
{
   //----------------------------------------------------------------
   // setup a string object
   //
   QString clStringT; //(QCAN_FRAME_STRING_SIZE, '\0');
   uint32_t ulSecondsT = 0;
   uint32_t ulNanoSecT = 0;
   
   
   if(btShowTimeR == true)
   {
      ulSecondsT = this->timeStamp().seconds();
      ulNanoSecT = this->timeStamp().nanoSeconds();
      ulNanoSecT = ulNanoSecT / 10000;
      clStringT = QString("%1.%2 ").arg(ulSecondsT, 5, 10).arg(ulNanoSecT, 5, 10, QChar('0'));
   }
   
   
   //----------------------------------------------------------------
   // print identifier
   //
   clStringT += QString("%1  ").arg(identifier(), 8, 16).toUpper();

   //----------------------------------------------------------------
   // print frame format
   //
   switch(frameFormat())
   {
      case eFORMAT_CAN_STD:
         clStringT += "CBFF ";
         break;
         
      case eFORMAT_CAN_EXT:
         clStringT += "CEFF ";
         break;
         
      case eFORMAT_FD_STD:
         clStringT += "FBFF ";
         break;
         
      case eFORMAT_FD_EXT:
         clStringT += "FEFF ";
         break;
         
      default:

         break;
   }

   //----------------------------------------------------------------
   // print DLC
   //
   clStringT += QString("%1  ").arg(dlc(), 2, 10);

   
   //----------------------------------------------------------------
   // print data
   //
   for(uint8_t ubCntT = 0; ubCntT < dataSize(); ubCntT++)
   {
      //---------------------------------------------------
      // print a newline and 19/31 spaces after 32 data bytes
      //
      if((ubCntT > 0) && ((ubCntT % 32) == 0))
      {
         if(btShowTimeR == true)
         {
            clStringT +="\n                               ";
         }
         else
         {
            clStringT +="\n                   ";
         }
      }
      clStringT += QString("%1 ").arg( data(ubCntT),
                                       2,          // 2 digits
                                      16,          // hex value
                                      QLatin1Char('0')).toUpper();
   }
   
   return(clStringT);
}
      

//----------------------------------------------------------------------------//
// operator <<                                                                //
// stream to a QDataStream object                                             //
//----------------------------------------------------------------------------//
QDataStream & operator<< ( QDataStream & clStreamR, 
                           const QCanFrame & clCanFrameR)
{
   //----------------------------------------------------------------
   // set version of stream
   //
   clStreamR.setVersion(QDataStream::Qt_5_6);
   
   //----------------------------------------------------------------
   // place all members to the stream
   //
   clStreamR << clCanFrameR.identifier();
   
   for(uint8_t ubIndexT = 0; ubIndexT < CAN_MSG_DATA_MAX; ubIndexT++)
   {
      clStreamR << clCanFrameR.data(ubIndexT);
   }
      
   clStreamR << clCanFrameR.dlc();
   //clStreamR << clCanFrameR.;
      
   //clStreamR << clCanFrameR.clMsgTimeP.seconds();
   //clStreamR << clCanFrameR.clMsgTimeP.nanoSeconds();
      
   //clStreamR << clCanFrameR.ulMsgUserP;
   
   return(clStreamR);
}



