//============================================================================//
// File:          canpie_frame.cpp                                            //
// Description:   CANpie classes - CAN frame                                  //
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

#include "canpie_data.hpp"


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define  CAN_FRAME_TYPE_API      ((uint32_t) 0x40000000)
#define  CAN_FRAME_TYPE_ERR      ((uint32_t) 0x80000000)


/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// CpData()                                                                   //
// constructor                                                                //
//----------------------------------------------------------------------------//
CpData::CpData()
{
   ulIdentifierP = 0;
   ubMsgDlcP     = 0;
   ubMsgCtrlP    = 0;
   for(uint8_t ubPosT = 0; ubPosT < CAN_MSG_DATA_MAX; ubPosT++)
   {
      aubByteP[ubPosT] = 0;
   }
   
   clMsgTimeP.clear();
  
   ulMsgUserP   = 0;
   ulMsgMarkerP = 0;
}


//----------------------------------------------------------------------------//
// CpData()                                                                   //
// constructor                                                                //
//----------------------------------------------------------------------------//
CpData::CpData(const Type_e & ubTypeR)
{
   switch(ubTypeR)
   {
      case eTYPE_CAN:
         ulIdentifierP = 0;
         ubMsgDlcP     = 0;
         break;
            
      case eTYPE_ERROR:
         ulIdentifierP = CAN_FRAME_TYPE_ERR;
         ubMsgDlcP     = 15;
         break;
         
      case eTYPE_API:
         ulIdentifierP = CAN_FRAME_TYPE_API;
         ubMsgDlcP     = 15;
         break;
         
      default:
         ulIdentifierP = 0;
         ubMsgDlcP     = 0;
         break;
   }
   
   ubMsgCtrlP    = 0;
   for(uint8_t ubPosT = 0; ubPosT < CAN_MSG_DATA_MAX; ubPosT++)
   {
      aubByteP[ubPosT] = 0;
   }
   
   clMsgTimeP.clear();
  
   ulMsgUserP   = 0;
   ulMsgMarkerP = 0;  
}


//----------------------------------------------------------------------------//
// ~CpData()                                                                  //
// destructor                                                                 //
//----------------------------------------------------------------------------//
CpData::~CpData()
{

}


//----------------------------------------------------------------------------//
// dataUInt16()                                                               //
// get uint16_t value which is stored at position ubPosR                      //
//----------------------------------------------------------------------------//
uint16_t CpData::dataUInt16(const uint8_t & ubPosR,
                               const bool & btMsbFirstR) const
{
   uint16_t uwValueT = 0;

   if(ubPosR < (CAN_MSG_DATA_MAX - 1))
   {
      if(btMsbFirstR)
      {
         uwValueT = aubByteP[ubPosR];
         uwValueT = uwValueT << 8;
         uwValueT = uwValueT + (uint8_t) aubByteP[ubPosR + 1];
      }
      else
      {
         uwValueT = aubByteP[ubPosR + 1];
         uwValueT = uwValueT << 8;
         uwValueT = uwValueT + (uint8_t) aubByteP[ubPosR];
      }
   }

   return(uwValueT);
}


//----------------------------------------------------------------------------//
// dataUInt32()                                                               //
// get uint32_t value which is stored at position ubPosR                      //
//----------------------------------------------------------------------------//
uint32_t CpData::dataUInt32(const uint8_t & ubPosR,
                               const bool & btMsbFirstR) const
{
   uint32_t ulValueT = 0;

   if(ubPosR < (CAN_MSG_DATA_MAX - 3))
   {
      if(btMsbFirstR)
      {
         ulValueT = aubByteP[ubPosR];
         ulValueT = ulValueT << 8;
         ulValueT = ulValueT + (uint8_t) aubByteP[ubPosR + 1];
         ulValueT = ulValueT << 8;
         ulValueT = ulValueT + (uint8_t) aubByteP[ubPosR + 2];
         ulValueT = ulValueT << 8;
         ulValueT = ulValueT + (uint8_t) aubByteP[ubPosR + 3];
      }
      else
      {
         ulValueT = aubByteP[ubPosR + 3];
         ulValueT = ulValueT << 8;
         ulValueT = ulValueT + (uint8_t) aubByteP[ubPosR + 2];
         ulValueT = ulValueT << 8;
         ulValueT = ulValueT + (uint8_t) aubByteP[ubPosR + 1];
         ulValueT = ulValueT << 8;
         ulValueT = ulValueT + (uint8_t) aubByteP[ubPosR];
      }
   }

   return(ulValueT);

}




//----------------------------------------------------------------------------//
// frameType()                                                                //
// get CAN frame type                                                         //
//----------------------------------------------------------------------------//
CpData::Type_e  CpData::frameType(void) const
{
   CpData::Type_e    eFrameTypeT = eTYPE_CAN;
   
   if(this->isFrameApi())
   {
      eFrameTypeT = eTYPE_API;
   }

   if(this->isFrameError())
   {
      eFrameTypeT = eTYPE_ERROR;
   }
   
   return(eFrameTypeT);
}


//----------------------------------------------------------------------------//
// fromByteArray()                                                            //
// convert byte array to CpData object                                        //
//----------------------------------------------------------------------------//
#if CANPIE_QT_SUPPORT > 0
bool CpData::fromByteArray(const QByteArray & clByteArrayR)
{
   //----------------------------------------------------------------
   // test size of byte array
   //
   if(clByteArrayR.size() < QCAN_FRAME_ARRAY_SIZE)
   {
      return(false);
   }
   
   
   //----------------------------------------------------------------
   // build checksum from byte 0 .. 93, and compare with checksum
   // value at the end
   //
   uint16_t uwChecksumT = clByteArrayR[94];
   uwChecksumT = uwChecksumT << 8;
   uwChecksumT = uwChecksumT + (uint8_t) clByteArrayR[95];
   
   if(uwChecksumT != qChecksum(clByteArrayR.constData(), 
                               QCAN_FRAME_ARRAY_SIZE - 2))
   {
      return(false);
   }
   
   //----------------------------------------------------------------
   // structure seems to be valid, now start copying the contents,
   // start with the identifier value
   //----------------------------------------------------------------

   
   //----------------------------------------------------------------
   // set identifier field from byte 0 .. 3, MSB first
   //
   ulIdentifierP = clByteArrayR[0];
   ulIdentifierP = ulIdentifierP << 8;
   ulIdentifierP = ulIdentifierP + (uint8_t) clByteArrayR[1];
   ulIdentifierP = ulIdentifierP << 8;
   ulIdentifierP = ulIdentifierP + (uint8_t) clByteArrayR[2];
   ulIdentifierP = ulIdentifierP << 8;
   ulIdentifierP = ulIdentifierP + (uint8_t) clByteArrayR[3];

 
   //----------------------------------------------------------------
   // set DLC field from byte 4
   //
   ubMsgDlcP = clByteArrayR[4];

   //----------------------------------------------------------------
   // set message control field from byte 5
   //
   ubMsgCtrlP = clByteArrayR[5];
   
   //----------------------------------------------------------------
   // set message data field from byte 6 .. 69
   //
   for(uint8_t ubPosT = 0; ubPosT < CAN_MSG_DATA_MAX; ubPosT++)
   {
      aubByteP[ubPosT] = clByteArrayR[6 + ubPosT];
   }   

   //----------------------------------------------------------------
   // set message timestamp field from byte 70 .. 77, MSB first
   //
   uint32_t  ulTimeValT = 0;

   ulTimeValT  = clByteArrayR[70];
   ulTimeValT  = ulTimeValT << 8;
   ulTimeValT += (uint8_t) clByteArrayR[71];
   ulTimeValT  = ulTimeValT << 8;
   ulTimeValT += (uint8_t) clByteArrayR[72];
   ulTimeValT  = ulTimeValT << 8;
   ulTimeValT += (uint8_t) clByteArrayR[73];
   clMsgTimeP.setSeconds(ulTimeValT);
   
   ulTimeValT  = 0;
   ulTimeValT  = clByteArrayR[74];
   ulTimeValT  = ulTimeValT << 8;
   ulTimeValT += (uint8_t) clByteArrayR[75];
   ulTimeValT  = ulTimeValT << 8;
   ulTimeValT += (uint8_t) clByteArrayR[76];
   ulTimeValT  = ulTimeValT << 8;
   ulTimeValT += (uint8_t) clByteArrayR[77];
   clMsgTimeP.setNanoSeconds(ulTimeValT);
   
   //----------------------------------------------------------------
   // set message user field from byte 78 .. 81, MSB first
   //
   ulMsgUserP  = clByteArrayR[78];
   ulMsgUserP  = ulMsgUserP << 8;
   ulMsgUserP += (uint8_t) clByteArrayR[79];
   ulMsgUserP  = ulMsgUserP << 8;
   ulMsgUserP += (uint8_t) clByteArrayR[80];
   ulMsgUserP  = ulMsgUserP << 8;
   ulMsgUserP += (uint8_t) clByteArrayR[81];
      
   //----------------------------------------------------------------
   // set message marker field from byte 82 .. 85, MSB first
   //
   ulMsgMarkerP  = clByteArrayR[82];
   ulMsgMarkerP  = ulMsgMarkerP << 8;
   ulMsgMarkerP += (uint8_t) clByteArrayR[83];
   ulMsgMarkerP  = ulMsgMarkerP << 8;
   ulMsgMarkerP += (uint8_t) clByteArrayR[84];
   ulMsgMarkerP  = ulMsgMarkerP << 8;
   ulMsgMarkerP += (uint8_t) clByteArrayR[85];
   
   
   return(true);
}
#endif


//----------------------------------------------------------------------------//
// isFrameApi()                                                               //
// test for API frame                                                         //
//----------------------------------------------------------------------------//
bool CpData::isFrameApi() const
{
   bool btResultT = false;

   if((ulIdentifierP & CAN_FRAME_TYPE_API) > 0)
   {
      btResultT = true;
   }

   return(btResultT);
}


//----------------------------------------------------------------------------//
// isFrameCan()                                                               //
// test for CAN frame                                                         //
//----------------------------------------------------------------------------//
bool CpData::isFrameCan() const
{
   bool btResultT = true;

   if(this->isFrameApi() || this->isFrameError())
   {
      btResultT = false;
   }

   return(btResultT);
}

//----------------------------------------------------------------------------//
// isFrameError()                                                             //
// test for error frame                                                       //
//----------------------------------------------------------------------------//
bool CpData::isFrameError() const
{
   bool btResultT = false;

   if((ulIdentifierP & CAN_FRAME_TYPE_ERR) > 0)
   {
      btResultT = true;
   }
   return(btResultT);
}



//----------------------------------------------------------------------------//
// setDataUInt16()                                                            //
// set data value                                                             //
//----------------------------------------------------------------------------//
void CpData::setDataUInt16(const uint8_t & ubPosR,
                           const uint16_t & uwValueR,
                           const bool & btMsbFirstR)
{
   uint16_t uwValueT = uwValueR;

   if(ubPosR < (CAN_MSG_DATA_MAX - 1))
   {
      if(btMsbFirstR)
      {
         aubByteP[ubPosR + 1] = (uint8_t) uwValueT;
         uwValueT = uwValueT >> 8;
         aubByteP[ubPosR + 0] = (uint8_t) uwValueT;
      }
      else
      {
         aubByteP[ubPosR + 0] = (uint8_t) uwValueT;
         uwValueT = uwValueT >> 8;
         aubByteP[ubPosR + 1] = (uint8_t) uwValueT;
      }
   }

}


//----------------------------------------------------------------------------//
// setDataUInt32()                                                            //
// set data value                                                             //
//----------------------------------------------------------------------------//
void CpData::setDataUInt32(const uint8_t & ubPosR,
                           const uint32_t & ulValueR,
                           const bool & btMsbFirstR)
{
   uint32_t ulValueT = ulValueR;

   if(ubPosR < (CAN_MSG_DATA_MAX - 3))
   {
      if(btMsbFirstR)
      {
         aubByteP[ubPosR + 3] = (uint8_t) ulValueT;
         ulValueT = ulValueT >> 8;
         aubByteP[ubPosR + 2] = (uint8_t) ulValueT;
         ulValueT = ulValueT >> 8;
         aubByteP[ubPosR + 1] = (uint8_t) ulValueT;
         ulValueT = ulValueT >> 8;
         aubByteP[ubPosR + 0] = (uint8_t) ulValueT;
      }
      else
      {
         aubByteP[ubPosR + 0] = (uint8_t) ulValueT;
         ulValueT = ulValueT >> 8;
         aubByteP[ubPosR + 1] = (uint8_t) ulValueT;
         ulValueT = ulValueT >> 8;
         aubByteP[ubPosR + 2] = (uint8_t) ulValueT;
         ulValueT = ulValueT >> 8;
         aubByteP[ubPosR + 3] = (uint8_t) ulValueT;
      }
   }
}



//----------------------------------------------------------------------------//
// toByteArray()                                                              //
// test for Extended Frame format                                             //
//----------------------------------------------------------------------------//
#if CANPIE_QT_SUPPORT > 0
QByteArray CpData::toByteArray() const
{
   //----------------------------------------------------------------
   // setup a defined length and clear contents
   //
   QByteArray clByteArrayT(QCAN_FRAME_ARRAY_SIZE, 0x00);
   
  
   //----------------------------------------------------------------
   // place identifier field in byte 0 .. 3, MSB first
   //
   clByteArrayT[0] = (uint8_t) (ulIdentifierP >> 24);
   clByteArrayT[1] = (uint8_t) (ulIdentifierP >> 16);
   clByteArrayT[2] = (uint8_t) (ulIdentifierP >>  8);
   clByteArrayT[3] = (uint8_t) (ulIdentifierP >>  0);
   
   //----------------------------------------------------------------
   // place message DLC field in byte 4
   //
   clByteArrayT[4] = ubMsgDlcP;

   //----------------------------------------------------------------
   // place message control field in byte 5
   //
   clByteArrayT[5] = ubMsgCtrlP;

   //----------------------------------------------------------------
   // place message data field in byte 6 .. 69
   //
   for(uint8_t ubPosT = 0; ubPosT < CAN_MSG_DATA_MAX; ubPosT++)
   {
      clByteArrayT[6 + ubPosT] = aubByteP[ubPosT];
   }

   //----------------------------------------------------------------
   // place message timestamp field in byte 70 .. 77, MSB first
   //
   uint32_t  ulTimeValT = 0;

   ulTimeValT = clMsgTimeP.seconds();
   clByteArrayT[70] = (uint8_t) (ulTimeValT >> 24);
   clByteArrayT[71] = (uint8_t) (ulTimeValT >> 16);
   clByteArrayT[72] = (uint8_t) (ulTimeValT >>  8);
   clByteArrayT[73] = (uint8_t) (ulTimeValT >>  0);

   ulTimeValT = clMsgTimeP.nanoSeconds();
   clByteArrayT[74] = (uint8_t) (ulTimeValT >>  24);
   clByteArrayT[75] = (uint8_t) (ulTimeValT >>  16);
   clByteArrayT[76] = (uint8_t) (ulTimeValT >>   8);
   clByteArrayT[77] = (uint8_t) (ulTimeValT >>   0);
   
   //----------------------------------------------------------------
   // place message user field in byte 78 .. 81, MSB first
   //
   clByteArrayT[78] = (uint8_t) (ulMsgUserP >>  24);
   clByteArrayT[79] = (uint8_t) (ulMsgUserP >>  16);
   clByteArrayT[80] = (uint8_t) (ulMsgUserP >>   8);
   clByteArrayT[81] = (uint8_t) (ulMsgUserP >>   0);

   //----------------------------------------------------------------
   // place message marker field in byte 82 .. 85, MSB first
   //
   clByteArrayT[82] = (uint8_t) (ulMsgMarkerP >>  24);
   clByteArrayT[83] = (uint8_t) (ulMsgMarkerP >>  16);
   clByteArrayT[84] = (uint8_t) (ulMsgMarkerP >>   8);
   clByteArrayT[85] = (uint8_t) (ulMsgMarkerP >>   0);
   
   //----------------------------------------------------------------
   // byte 86 .. 93 (i.e. 8 bytes) are not used, set to 0
   //----------------------------------------------------------------
   
   //----------------------------------------------------------------
   // build checksum from byte 0 .. 93, add checksum at the end
   // 
   uint16_t uwChecksumT = qChecksum(clByteArrayT.constData(), 
                                    QCAN_FRAME_ARRAY_SIZE - 2);
   
   clByteArrayT[94] = (uint8_t) (uwChecksumT >> 8);
   clByteArrayT[95] = (uint8_t) (uwChecksumT >> 0);

   return(clByteArrayT);
}
#endif

