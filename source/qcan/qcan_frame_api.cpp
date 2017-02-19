//============================================================================//
// File:          qcan_frame_api.cpp                                          //
// Description:   QCan classes - CAN API frame                                //
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
QCanFrameApi::QCanFrameApi() : QCanData(eTYPE_API)
{

}


//----------------------------------------------------------------------------//
// QCanFrameApi()                                                             //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanFrameApi::~QCanFrameApi()
{

}



//----------------------------------------------------------------------------//
// bitrate()                                                                  //
// get bit-rate value (byte 0 .. 3)                                           //
//----------------------------------------------------------------------------//
bool QCanFrameApi::bitrate(int32_t & slNomBitRateV, int32_t & slDatBitRateV)
{
   bool  btResultT = false;

   if(ulMsgMarkerP == QCanFrameApi::eAPI_FUNC_BITRATE)
   {
      slNomBitRateV = dataUInt32(0);
      slDatBitRateV = dataUInt32(4);
      btResultT = true;
   }

   return(btResultT);
}

int32_t QCanFrameApi::bitrateData(void)
{
   return (dataUInt32(4));
}

int32_t QCanFrameApi::bitrateNominal(void)
{
   return (dataUInt32(0));
}

//----------------------------------------------------------------------------//
// function()                                                                 //
// determine the function code                                                //
//----------------------------------------------------------------------------//
QCanFrameApi::ApiFunc_e QCanFrameApi::function(void)
{
   return((QCanFrameApi::ApiFunc_e) ulMsgMarkerP);

}


//----------------------------------------------------------------------------//
// mode()                                                                     //
// get operation mode                                                         //
//----------------------------------------------------------------------------//
CAN_Mode_e QCanFrameApi::mode(void)
{
   return((CAN_Mode_e ) aubByteP[0]);
}

bool QCanFrameApi::name(QString & clNameR)
{
   bool     btResultT = false;
   uint8_t  ubPosT;

   if(ulMsgMarkerP == QCanFrameApi::eAPI_FUNC_NAME)
   {
      clNameR.clear();
      for (ubPosT = 0; ubPosT < ubMsgDlcP; ubPosT++)
      {
         clNameR.append(aubByteP[ubPosT]);
      }
      btResultT = true;
   }

   return (btResultT);
}


//----------------------------------------------------------------------------//
// setBitrate()                                                               //
// Byte 0 .. 3: slBitrateV, Byte 4 .. 7: slBrsClockV                          //
//----------------------------------------------------------------------------//
void QCanFrameApi::setBitrate(int32_t slBitrateV, int32_t slBrsClockV)
{
   ulMsgMarkerP = QCanFrameApi::eAPI_FUNC_BITRATE;
   setDataUInt32(0, slBitrateV);
   setDataUInt32(4, slBrsClockV);
}


//----------------------------------------------------------------------------//
// setMode()                                                                  //
// set operation mode                                                         //
//----------------------------------------------------------------------------//
void QCanFrameApi::setMode(CAN_Mode_e teModeV)
{
   ulMsgMarkerP = QCanFrameApi::eAPI_FUNC_CAN_MODE;
   aubByteP[0] = (uint8_t) teModeV;
}


void QCanFrameApi::setDriverInit()
{
   ulMsgMarkerP = QCanFrameApi::eAPI_FUNC_DRIVER_INIT;

}
void QCanFrameApi::setDriverRelease()
{
   ulMsgMarkerP = QCanFrameApi::eAPI_FUNC_DRIVER_RELEASE;

}

void QCanFrameApi::setName(QString clNameV)
{
   int32_t  slSizeT;
   int32_t  slPosT;

   ulMsgMarkerP = QCanFrameApi::eAPI_FUNC_NAME;

   //----------------------------------------------------------------
   // the maximum number of characters is limited to 64
   //
   slSizeT = clNameV.size();
   if(slSizeT > QCAN_MSG_DATA_MAX)
   {
      slSizeT = QCAN_MSG_DATA_MAX;
   }

   //----------------------------------------------------------------
   // copy size in DLC field
   //
   ubMsgDlcP = (uint8_t) slSizeT;

   //----------------------------------------------------------------
   // copy string to payload
   //
   for (slPosT = 0; slPosT < slSizeT; slPosT++)
   {
      aubByteP[slPosT] = clNameV.at(slPosT).toLatin1();
   }
}


//----------------------------------------------------------------------------//
// fromByteArray()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanFrameApi::fromByteArray(const QByteArray & clByteArrayR)
{
   bool  btResultT = false;
   
   //----------------------------------------------------------------
   // an API frame must have the value 0x40 in the first byte,
   // refer to QCanData class implementation
   //
   if ((clByteArrayR.at(0) & 0x40) > 0)
   {
      btResultT = (QCanData::fromByteArray(clByteArrayR)); 
   }
   
   return (btResultT);
}


//----------------------------------------------------------------------------//
// toByteArray()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
QByteArray QCanFrameApi::toByteArray() const
{
   return QCanData::toByteArray();
}


//----------------------------------------------------------------------------//
// toString()                                                                 //
// print API frame                                                            //
//----------------------------------------------------------------------------//
QString QCanFrameApi::toString(const bool & btShowTimeR)
{
   //----------------------------------------------------------------
   // setup a string object
   //
   QString clStringT; 
   
   if(btShowTimeR == true)
   {
      
   }
   
   

   //----------------------------------------------------------------
   // print information
   //
   switch(function())
   {
      case eAPI_FUNC_BITRATE:
         clStringT = "Bit-rate:";
         break;

      case eAPI_FUNC_NAME:
         name(clStringT);
         break;
         
      default:
         
         break;
   }
   
   return(clStringT);
}
      
