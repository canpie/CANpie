//============================================================================//
// File:          qcan_frame.cpp                                              //
// Description:   QCAN classes - CAN frame                                    //
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

#include <QCanFrame>


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


#define  QCAN_FRAME_ID_MASK_STD     ((uint32_t) 0x000007FF)

#define  QCAN_FRAME_ID_MASK_EXT     ((uint32_t) 0x1FFFFFFF)

#define  QCAN_FRAME_FORMAT_EXT      ((uint8_t) 0x01)

#define  QCAN_FRAME_FORMAT_FD       ((uint8_t) 0x02)

/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanFrame()                                                                //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanFrame::QCanFrame()
{
   ulIdentifierP = 0;
   for(uint8_t ubPosT = 0; ubPosT < CAN_FRAME_DATA_MAX; ubPosT++)
   {
      aubByteP[ubPosT] = 0;
   }
   
   ubMsgDlcP  = 0;
   ubMsgCtrlP = eTYPE_CAN_STD;
   
   tsMsgTimeP.ulSeconds     = 0;
   tsMsgTimeP.ulNanoSeconds = 0;
   
   ulMsgUserP   = 0;
   ulMsgMarkerP = 0;
}


//----------------------------------------------------------------------------//
// QCanFrame()                                                                //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanFrame::QCanFrame(const Type_e & ubTypeR, const uint32_t & ulIdentifierR, 
                     const uint8_t & ubDlcR)
{
   ulIdentifierP = ulIdentifierR;
   for(uint8_t ubPosT = 0; ubPosT < CAN_FRAME_DATA_MAX; ubPosT++)
   {
      aubByteP[ubPosT] = 0;
   }
   
   ubMsgDlcP  = ubDlcR;
   ubMsgCtrlP = ubTypeR;
   
   tsMsgTimeP.ulSeconds     = 0;
   tsMsgTimeP.ulNanoSeconds = 0;
   
   ulMsgUserP   = 0;
   ulMsgMarkerP = 0;   
}

QCanFrame::~QCanFrame()
{

}


//----------------------------------------------------------------------------//
// data()                                                                     //
// get data                                                                   //
//----------------------------------------------------------------------------//
uint8_t  QCanFrame::data(const uint8_t & ubPosR) const
{
   if(ubPosR < this->dataSize())
   {
      return(aubByteP[ubPosR]);
   }
   
   return(0);
}

//----------------------------------------------------------------------------//
// data()                                                                     //
// get data                                                                   //
//----------------------------------------------------------------------------//
QByteArray  QCanFrame::data(void) const
{
   QByteArray clDataT((const char *)&aubByteP[0], this->dataSize());
   return(clDataT);
}


//----------------------------------------------------------------------------//
// dataSize()                                                                 //
// get data size in bytes                                                     //
//----------------------------------------------------------------------------//
uint8_t QCanFrame::dataSize(void) const
{
   uint8_t  ubSizeT;
   
   switch(ubMsgDlcP)
   {
      case 0: 
      case 1: case 2: case 3: case 4:
      case 5: case 6: case 7: case 8:
         ubSizeT = ubMsgDlcP;
         break;
         
      case 9:
         ubSizeT = 12;
         break;
         
      case 10:
         ubSizeT = 16;
         break;

      case 11:
         ubSizeT = 20;
         break;
         
      case 12:
         ubSizeT = 24;
         break;

      case 13:
         ubSizeT = 32;
         break;

      case 14:
         ubSizeT = 48;
         break;

      case 15:
         ubSizeT = 64;
         break;
         
      default:
         ubSizeT = 0;
         break;
         
   }

   return(ubSizeT);
}


//----------------------------------------------------------------------------//
// dataUInt16()                                                               //
// get uint16_t value which is stored at position ubPosR                      //
//----------------------------------------------------------------------------//
uint16_t QCanFrame::dataUInt16(const uint8_t & ubPosR,
                               const bool & btMsbFirstR) const
{
   uint16_t uwValueT = 0;

   if(ubPosR < (this->dataSize() - 1) && (this->dataSize() > 1))
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
uint32_t QCanFrame::dataUInt32(const uint8_t & ubPosR,
                               const bool & btMsbFirstR) const
{
   uint32_t ulValueT = 0;

   if(ubPosR < (this->dataSize() - 3) && (this->dataSize() > 3))
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
// dlc()                                                                      //
// get CAN DLC value                                                          //
//----------------------------------------------------------------------------//
uint8_t QCanFrame::dlc(void) const
{
   return(ubMsgDlcP);
}


//----------------------------------------------------------------------------//
// frameType()                                                                //
// get CAN frame type                                                         //
//----------------------------------------------------------------------------//
QCanFrame::Type_e  QCanFrame::frameType(void) const
{
   return((Type_e) (ubMsgCtrlP & 0x33));
}


//----------------------------------------------------------------------------//
// fromByteArray()                                                            //
// test for Extended Frame format                                             //
//----------------------------------------------------------------------------//
bool QCanFrame::fromByteArray(const QByteArray & clByteArrayR)
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
   for(uint8_t ubPosT = 0; ubPosT < CAN_FRAME_DATA_MAX; ubPosT++)
   {
      aubByteP[ubPosT] = clByteArrayR[6 + ubPosT];
   }   

   //----------------------------------------------------------------
   // set message timestamp field from byte 70 .. 77, MSB first
   //
   tsMsgTimeP.ulSeconds  = clByteArrayR[70];
   tsMsgTimeP.ulSeconds  = tsMsgTimeP.ulSeconds << 8;
   tsMsgTimeP.ulSeconds += (uint8_t) clByteArrayR[71];
   tsMsgTimeP.ulSeconds  = tsMsgTimeP.ulSeconds << 8;
   tsMsgTimeP.ulSeconds += (uint8_t) clByteArrayR[72];
   tsMsgTimeP.ulSeconds  = tsMsgTimeP.ulSeconds << 8;
   tsMsgTimeP.ulSeconds += (uint8_t) clByteArrayR[73];
   
   tsMsgTimeP.ulNanoSeconds  = clByteArrayR[74];
   tsMsgTimeP.ulNanoSeconds  = tsMsgTimeP.ulNanoSeconds << 8;
   tsMsgTimeP.ulNanoSeconds += (uint8_t) clByteArrayR[75];
   tsMsgTimeP.ulNanoSeconds  = tsMsgTimeP.ulNanoSeconds << 8;
   tsMsgTimeP.ulNanoSeconds += (uint8_t) clByteArrayR[76];
   tsMsgTimeP.ulNanoSeconds  = tsMsgTimeP.ulNanoSeconds << 8;
   tsMsgTimeP.ulNanoSeconds += (uint8_t) clByteArrayR[77];
   
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


//----------------------------------------------------------------------------//
// identifier()                                                               //
// get identifier value                                                       //
//----------------------------------------------------------------------------//
uint32_t QCanFrame::identifier(void) const
{
   uint32_t ulIdValueT;
   
   if((ubMsgCtrlP & QCAN_FRAME_FORMAT_EXT) > 0)
   {
      ulIdValueT = ulIdentifierP & QCAN_FRAME_ID_MASK_EXT;
   }
   else
   {
      ulIdValueT = ulIdentifierP & QCAN_FRAME_ID_MASK_STD;
   }
   
   return(ulIdValueT);
}


//----------------------------------------------------------------------------//
// isExtended()                                                               //
// test for Extended Frame format                                             //
//----------------------------------------------------------------------------//
bool QCanFrame::isExtended(void) const
{
   bool btResultT = false;
   
   if((ubMsgCtrlP & QCAN_FRAME_FORMAT_EXT) > 0)
   {
      btResultT = true;
   }
   return(btResultT);
}


//----------------------------------------------------------------------------//
// setData()                                                                  //
// set data value                                                             //
//----------------------------------------------------------------------------//
void QCanFrame::setData(const uint8_t & ubPosR, const uint8_t & ubValueR)
{
   if(ubPosR < dataSize())
   {
      aubByteP[ubPosR] = ubValueR;
   }
}


//----------------------------------------------------------------------------//
// setDataUInt16()                                                            //
// set data value                                                             //
//----------------------------------------------------------------------------//
void QCanFrame::setDataUInt16(const uint8_t & ubPosR,
                              const uint16_t & uwValueR,
                              const bool & btMsbFirstR)
{
   uint16_t uwValueT = uwValueR;

   if(ubPosR < (this->dataSize() - 1) && (this->dataSize() > 1))
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
void QCanFrame::setDataUInt32(const uint8_t & ubPosR,
                              const uint32_t & ulValueR,
                              const bool & btMsbFirstR)
{
   uint32_t ulValueT = ulValueR;

   if(ubPosR < (this->dataSize() - 3) && (this->dataSize() > 3))
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
// setDlc()                                                                   //
// set DLC value, depends on frame type                                       //
//----------------------------------------------------------------------------//
void QCanFrame::setDlc(uint8_t ubDlcV)
{
   if(ubDlcV < 16)
   {
      if((ubMsgCtrlP & QCAN_FRAME_FORMAT_FD) > 0)
      {
         ubMsgDlcP = ubDlcV;
      }
      else
      {
         if(ubDlcV < 9)
         {
            ubMsgDlcP = ubDlcV;
         }
      }
   }
}


//----------------------------------------------------------------------------//
// setExtId()                                                                 //
// test for Extended Frame format                                             //
//----------------------------------------------------------------------------//
void QCanFrame::setExtId(uint32_t ulIdentifierV)
{
   ulIdentifierP = (ulIdentifierV & QCAN_FRAME_ID_MASK_EXT);
   ubMsgCtrlP   |= QCAN_FRAME_FORMAT_EXT;
}

void QCanFrame::setFrameType(const Type_e &ubTypeR)
{
   ubMsgCtrlP &= (~0x33);     // remove existing frame type bits
   ubMsgCtrlP |= ubTypeR;
}

void QCanFrame::setMarker(const uint32_t & ulMarkerValueR)
{
   ulMsgMarkerP = ulMarkerValueR;
}


//----------------------------------------------------------------------------//
// setStdId()                                                                 //
// test for Extended Frame format                                             //
//----------------------------------------------------------------------------//
void QCanFrame::setStdId(uint16_t uwIdentifierV)
{
   ulIdentifierP = (uwIdentifierV & QCAN_FRAME_ID_MASK_STD);
   ubMsgCtrlP   &= (~QCAN_FRAME_FORMAT_EXT);
}


void QCanFrame::setUser(const uint32_t & ulUserValueR)
{
   ulMsgUserP = ulUserValueR;
}


//----------------------------------------------------------------------------//
// toByteArray()                                                              //
// test for Extended Frame format                                             //
//----------------------------------------------------------------------------//
QByteArray QCanFrame::toByteArray() const
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
   for(uint8_t ubPosT = 0; ubPosT < CAN_FRAME_DATA_MAX; ubPosT++)
   {
      clByteArrayT[6 + ubPosT] = aubByteP[ubPosT];
   }

   //----------------------------------------------------------------
   // place message timestamp field in byte 70 .. 77, MSB first
   //
   clByteArrayT[70] = (uint8_t) (tsMsgTimeP.ulSeconds >> 24);
   clByteArrayT[71] = (uint8_t) (tsMsgTimeP.ulSeconds >> 16);
   clByteArrayT[72] = (uint8_t) (tsMsgTimeP.ulSeconds >>  8);
   clByteArrayT[73] = (uint8_t) (tsMsgTimeP.ulSeconds >>  0);
   clByteArrayT[74] = (uint8_t) (tsMsgTimeP.ulNanoSeconds >>  24);
   clByteArrayT[75] = (uint8_t) (tsMsgTimeP.ulNanoSeconds >>  16);
   clByteArrayT[76] = (uint8_t) (tsMsgTimeP.ulNanoSeconds >>   8);
   clByteArrayT[77] = (uint8_t) (tsMsgTimeP.ulNanoSeconds >>   0);
   
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
   
   if(btShowTimeR == true)
   {
      
   }
   
   
   //----------------------------------------------------------------
   // print identifier
   //
   clStringT += QString("%1  ").arg(ulIdentifierP, 8, 16).toUpper();

   //----------------------------------------------------------------
   // print frame format
   //
   switch(frameType())
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

   //----------------------------------------------------------------
   // print DLC
   //
   clStringT += QString("%1  ").arg(dlc(), 2, 10);

   
   //----------------------------------------------------------------
   // print data
   //
   for(uint8_t ubCntT = 0; ubCntT < dataSize(); ubCntT++)
   {
      clStringT += QString("%1 ").arg( aubByteP[ubCntT], 
                                       2,          // 2 digits 
                                      16,          // hex value
                                      QLatin1Char('0')).toUpper();
   }
   
   return(clStringT);
}
      

//----------------------------------------------------------------------------//
// operator ==                                                                //
// compare two CAN frames                                                     //
//----------------------------------------------------------------------------//
bool QCanFrame::operator==(const QCanFrame & clCanFrameR)
{
   bool btResultT = false;
   
   if(this->frameType() == clCanFrameR.frameType())
   {
      if(this->identifier() == clCanFrameR.identifier())
      {
         if(this->dlc() == clCanFrameR.dlc())
         {
            btResultT = true;
            for(uint8_t ubCntT = 0; ubCntT < this->dataSize(); ubCntT++)
            {
               if(this->data(ubCntT) != clCanFrameR.data(ubCntT))
               {
                  btResultT = false;
                  break;
               }
            }
         }
      }
   }
   
   return(btResultT);
}


//----------------------------------------------------------------------------//
// operator !=                                                                //
// compare two CAN frames                                                     //
//----------------------------------------------------------------------------//
bool QCanFrame::operator!=(const QCanFrame & clCanFrameR)
{
   return(!(*this == clCanFrameR));
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
   clStreamR << clCanFrameR.ulIdentifierP;
   
   for(uint8_t ubIndexT = 0; ubIndexT < CAN_FRAME_DATA_MAX; ubIndexT++)
   {
      clStreamR << clCanFrameR.aubByteP[ubIndexT];
   }
      
   clStreamR << clCanFrameR.ubMsgDlcP;
   clStreamR << clCanFrameR.ubMsgCtrlP;
      
   clStreamR << clCanFrameR.tsMsgTimeP.ulSeconds;
   clStreamR << clCanFrameR.tsMsgTimeP.ulNanoSeconds;
      
   clStreamR << clCanFrameR.ulMsgUserP;
   
   return(clStreamR);
}



