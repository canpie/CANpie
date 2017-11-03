//============================================================================//
// File:          qcan_frame.cpp                                              //
// Description:   QCAN classes - CAN frame                                    //
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

#include <QCanFrame>


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define  CAN_FRAME_FORMAT_EXT       ((uint8_t) 0x01)

#define  CAN_FRAME_FORMAT_FDF       ((uint8_t) 0x02)

#define  CAN_FRAME_FORMAT_RTR       ((uint8_t) 0x04)

#define  CAN_FRAME_ISO_FD_BRS       ((uint8_t) 0x40)

#define  CAN_FRAME_ISO_FD_ESI       ((uint8_t) 0x80)


/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanFrame()                                                                //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanFrame::QCanFrame() : QCanData(eTYPE_CAN)
{
   setFrameFormat(eFORMAT_CAN_STD);
   setIdentifier(0);
   setDlc(0);
}


//----------------------------------------------------------------------------//
// QCanFrame()                                                                //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanFrame::QCanFrame(const Format_e & ubFormatR, const uint32_t & ulIdentifierR, 
                     const uint8_t & ubDlcR) 
         : QCanData(eTYPE_CAN)
{
   setFrameFormat(ubFormatR);
   setIdentifier(ulIdentifierR);
   setDlc(ubDlcR);
}


//----------------------------------------------------------------------------//
// ~QCanFrame()                                                               //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanFrame::~QCanFrame()
{

}



//----------------------------------------------------------------------------//
// bitrateSwitch()                                                            //
// get value of bit-rate switch                                               //
//----------------------------------------------------------------------------//
bool QCanFrame::bitrateSwitch(void) const
{
   bool btResultT = false;

   if(frameFormat() > eFORMAT_CAN_EXT)
   {
      if((ubMsgCtrlP & CAN_FRAME_ISO_FD_BRS) > 0)
      {
         btResultT = true;
      }
   }
   return(btResultT);
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
   QByteArray clDataT = QByteArray( (const char *)&aubByteP[0],
                                    this->dataSize());
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
// errorStateIndicator()                                                      //
// get value of error state indicator                                         //
//----------------------------------------------------------------------------//
bool QCanFrame::errorStateIndicator(void) const
{
   bool btResultT = false;

   if(frameFormat() > eFORMAT_CAN_EXT)
   {
      if((ubMsgCtrlP & CAN_FRAME_ISO_FD_ESI) > 0)
      {
         btResultT = true;
      }
   }
   return(btResultT);
}


//----------------------------------------------------------------------------//
// frameFormat()                                                              //
// get CAN frame format                                                       //
//----------------------------------------------------------------------------//
QCanFrame::Format_e  QCanFrame::frameFormat(void) const
{
   return((Format_e) (ubMsgCtrlP & 0x03));
}

//----------------------------------------------------------------------------//
// fromByteArray()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
bool QCanFrame::fromByteArray(const QByteArray & clByteArrayR)
{
   bool  btResultT = false;
   
   //----------------------------------------------------------------
   // a CAN frame must have the three MSB bits set to 0 in the first 
   // byte, refer to QCanData class implementation (0b000xxxxx)
   //
   if ((clByteArrayR.at(0) & 0xE0) == 0)
   {
      btResultT = (QCanData::fromByteArray(clByteArrayR)); 
   }
   
   return (btResultT);
}


//----------------------------------------------------------------------------//
// identifier()                                                               //
// get identifier value                                                       //
//----------------------------------------------------------------------------//
uint32_t QCanFrame::identifier(void) const
{
   uint32_t ulIdValueT;

   if((ubMsgCtrlP & CAN_FRAME_FORMAT_EXT) > 0)
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

   if((ubMsgCtrlP & CAN_FRAME_FORMAT_EXT) > 0)
   {
      btResultT = true;
   }
   return(btResultT);
}



//----------------------------------------------------------------------------//
// isRemote()                                                                 //
// test for Remote Frame format                                               //
//----------------------------------------------------------------------------//
bool QCanFrame::isRemote(void) const
{
   bool btResultT = false;

   if((ubMsgCtrlP & CAN_FRAME_FORMAT_RTR) > 0)
   {
      btResultT = true;
   }
   return(btResultT);
}


//----------------------------------------------------------------------------//
// setBitrateSwitch()                                                         //
// set / reset bit-rate switch bit value                                      //
//----------------------------------------------------------------------------//
void QCanFrame::setBitrateSwitch(const bool & btBrsR)
{
   if(frameFormat() > eFORMAT_CAN_EXT)
   {
      if(btBrsR == true)
      {
         ubMsgCtrlP |= CAN_FRAME_ISO_FD_BRS;
      }
      else
      {
         ubMsgCtrlP &= (~CAN_FRAME_ISO_FD_BRS);
      }
   }
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
// setDataSize()                                                              //
// set data size value                                                        //
//----------------------------------------------------------------------------//
void QCanFrame::setDataSize(uint8_t ubSizeV)
{
   if(frameFormat() > eFORMAT_CAN_EXT)
   {
      //--------------------------------------------------------
      // set DLC value to maximum requested size value
      //
      if(ubSizeV <= 8)
      {
         ubMsgDlcP = ubSizeV;
      }
      else if((ubSizeV > 8) && (ubSizeV <= 12))
      {
         ubMsgDlcP = 9;
      }
      else if((ubSizeV > 12) && (ubSizeV <= 16))
      {
         ubMsgDlcP = 10;
      }
      else if((ubSizeV > 16) && (ubSizeV <= 20))
      {
         ubMsgDlcP = 11;
      }
      else if((ubSizeV > 20) && (ubSizeV <= 24))
      {
         ubMsgDlcP = 12;
      }
      else if((ubSizeV > 24) && (ubSizeV <= 32))
      {
         ubMsgDlcP = 13;
      }
      else if((ubSizeV > 32) && (ubSizeV <= 48))
      {
         ubMsgDlcP = 14;
      }
      else
      {
         ubMsgDlcP = 15;
      }
   }
   else
   {
      //--------------------------------------------------------
      // Classic CAN supports 0 .. 8 byte
      //
      if(ubSizeV > 8)
      {
         ubSizeV = 8;
      }
      ubMsgDlcP = ubSizeV;
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
   //----------------------------------------------------------------
   // limit the DLC value range from 0 .. 15
   //
   ubDlcV = ubDlcV & 0x0F;

   if(frameFormat() < eFORMAT_FD_STD)
   {
      //--------------------------------------------------------
      // classic CAN is limited to a DLC value of 8
      //
      if(ubDlcV > 8)
      {
         ubDlcV = 8;
      }
      ubMsgDlcP = ubDlcV;
   }
   else
   {
      //--------------------------------------------------------
      // CAN FD can have DLC values from 0 .. 15
      //
      ubMsgDlcP = ubDlcV;
   }
}

//----------------------------------------------------------------------------//
// setErrorStateIndicator()                                                   //
// set the ESI bit                                                            //
//----------------------------------------------------------------------------//
void QCanFrame::setErrorStateIndicator(const bool & btEsiR)
{
   if (btEsiR == true)
   {
      ubMsgCtrlP |= CAN_FRAME_ISO_FD_ESI;
   }
   else
   {
      ubMsgCtrlP &= (~CAN_FRAME_ISO_FD_ESI);
   }
}

//----------------------------------------------------------------------------//
// setFrameFormat()                                                           //
// set the frame format                                                       //
//----------------------------------------------------------------------------//
void QCanFrame::setFrameFormat(const Format_e &ubFormatR)
{
   ubMsgCtrlP &= (~0x03);     // remove existing frame type bits
   ubMsgCtrlP |= ubFormatR;

   //----------------------------------------------------------------
   // check for Classic CAN frame
   //
   if(ubFormatR < eFORMAT_FD_STD)
   {
      //--------------------------------------------------------
      // ESI and BRS are cleared
      //
      ubMsgCtrlP &= (~CAN_FRAME_ISO_FD_BRS);
      ubMsgCtrlP &= (~CAN_FRAME_ISO_FD_ESI);

      //--------------------------------------------------------
      // make sure the DLC value is limited to 8
      //
      if(ubMsgDlcP > 8)
      {
         ubMsgDlcP = 8;
      }
   }

   //----------------------------------------------------------------
   // check for ISO FD CAN frame
   //
   else
   {
      //--------------------------------------------------------
      // RTR is cleared
      //
      ubMsgCtrlP &= (~CAN_FRAME_FORMAT_RTR);

   }
}


//----------------------------------------------------------------------------//
// setIdentifier()                                                            //
// set identifier value                                                       //
//----------------------------------------------------------------------------//
void QCanFrame::setIdentifier(uint32_t ulIdentifierV)
{
   switch(frameFormat())
   {
      case eFORMAT_CAN_STD:
      case eFORMAT_FD_STD:
         ulIdentifierP = ulIdentifierV & QCAN_FRAME_ID_MASK_STD;
         break;

      case eFORMAT_CAN_EXT:
      case eFORMAT_FD_EXT:
         ulIdentifierP = ulIdentifierV & QCAN_FRAME_ID_MASK_EXT;
         break;

   }
}


void QCanFrame::setMarker(const uint32_t & ulMarkerValueR)
{
   ulMsgMarkerP = ulMarkerValueR;
}


//----------------------------------------------------------------------------//
// setRemote()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanFrame::setRemote(const bool & btRtrR)
{
   if(frameFormat() < eFORMAT_FD_STD)
   {
      if (btRtrR == true)
      {
         ubMsgCtrlP |=   CAN_FRAME_FORMAT_RTR;
      }
      else
      {
         ubMsgCtrlP &= (~CAN_FRAME_FORMAT_RTR);
      }
   }
   else
   {
      ubMsgCtrlP &= (~CAN_FRAME_FORMAT_RTR);
   }

}


void QCanFrame::setUser(const uint32_t & ulUserValueR)
{
   ulMsgUserP = ulUserValueR;
}


QByteArray QCanFrame::toByteArray() const
{
   return(QCanData::toByteArray());
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
   QString clStringT;
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
   
uint32_t  QCanFrame::marker(void) const
{
   return (ulMsgMarkerP);
}

uint32_t  QCanFrame::user(void) const
{
   return (ulMsgUserP);
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
   
   for(uint8_t ubIndexT = 0; ubIndexT < QCAN_MSG_DATA_MAX; ubIndexT++)
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



