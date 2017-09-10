//============================================================================//
// File:          qcan_frame_api.cpp                                          //
// Description:   QCan classes - CAN API frame                                //
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
   ulMsgMarkerP = QCanFrameApi::eAPI_FUNC_NONE;
}


//----------------------------------------------------------------------------//
// QCanFrameApi()                                                             //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanFrameApi::~QCanFrameApi()
{

}


//----------------------------------------------------------------------------//
// dataBitrate()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t QCanFrameApi::dataBitrate(void)
{
   int32_t slBitrateT = eCAN_BITRATE_NONE;

   if (ulMsgMarkerP == QCanFrameApi::eAPI_FUNC_BITRATE)
   {
      slBitrateT = dataUInt32(4);
   }

   return (slBitrateT);
}

//----------------------------------------------------------------------------//
// nominalBitrate()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
int32_t QCanFrameApi::nominalBitrate(void)
{
   int32_t slBitrateT = eCAN_BITRATE_NONE;

   if (ulMsgMarkerP == QCanFrameApi::eAPI_FUNC_BITRATE)
   {
      slBitrateT = dataUInt32(0);
   }

   return (slBitrateT);
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

QString QCanFrameApi::name(void)
{
   QString  clNameT;
   uint8_t  ubPosT;

   if(ulMsgMarkerP == QCanFrameApi::eAPI_FUNC_NAME)
   {
      clNameT.clear();
      for (ubPosT = 0; ubPosT < ubMsgDlcP; ubPosT++)
      {
         clNameT.append(aubByteP[ubPosT]);
      }
   }

   return (clNameT);
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
   
   Q_UNUSED(btShowTimeR);
   
   //----------------------------------------------------------------
   // setup a string object
   //
   QString clStringT; 
   

   
   

   //----------------------------------------------------------------
   // print information
   //
   switch(function())
   {
      case eAPI_FUNC_BITRATE:
         clStringT = "Nominal bit-rate: ";
         if (nominalBitrate() == eCAN_BITRATE_NONE)
         {
            clStringT += "None";
         }
         else
         {
            if (nominalBitrate() < 1000000)
            {
               //------------------------------------------------
               // print values < 1000000 in kBit/s
               //
               clStringT += QString("%1 kBit/s").arg(nominalBitrate() / 1000);
            }
            else
            {
               //------------------------------------------------
               // print values >= 1000000 in MBit/s
               //
               clStringT += QString("%1 MBit/s").arg(nominalBitrate() / 1000000);
            }
         }

         if (dataBitrate() != eCAN_BITRATE_NONE)
         {
            clStringT += ", Data bit-rate: ";
            if (dataBitrate() < 1000000)
            {
               //------------------------------------------------
               // print values < 1000000 in kBit/s
               //
               clStringT += QString("%1 kBit/s").arg(dataBitrate() / 1000);
            }
            else
            {
               //------------------------------------------------
               // print values >= 1000000 in MBit/s
               //
               clStringT += QString("%1 MBit/s").arg(dataBitrate() / 1000000);
            }
         }
         break;

      case eAPI_FUNC_NAME:
         clStringT = name();
         break;
         
      default:
         
         break;
   }
   
   return(clStringT);
}
      
