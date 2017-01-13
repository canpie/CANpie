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

#include "../canpie-fd/canpie_frame.hpp"


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


#define  CAN_FRAME_ID_MASK_STD      ((uint32_t) 0x000007FF)

#define  CAN_FRAME_ID_MASK_EXT      ((uint32_t) 0x1FFFFFFF)

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
// CpFrame()                                                                  //
// constructor                                                                //
//----------------------------------------------------------------------------//
CpFrame::CpFrame()
{
   ulIdentifierP = 0;
   for(uint8_t ubPosT = 0; ubPosT < CAN_FRAME_DATA_MAX; ubPosT++)
   {
      aubByteP[ubPosT] = 0;
   }
   
   ubMsgDlcP  = 0;
   ubMsgCtrlP = eTYPE_CAN_STD;
   
   ulMsgUserP   = 0;
   ulMsgMarkerP = 0;
}


//----------------------------------------------------------------------------//
// CpFrame()                                                                  //
// constructor                                                                //
//----------------------------------------------------------------------------//
CpFrame::CpFrame(const Type_e & ubTypeR, const uint32_t & ulIdentifierR, 
                 const uint8_t & ubDlcR)
{
   setFrameType(ubTypeR);
   setIdentifier(ulIdentifierR);
   setDlc(ubDlcR);

   for(uint8_t ubPosT = 0; ubPosT < CAN_FRAME_DATA_MAX; ubPosT++)
   {
      aubByteP[ubPosT] = 0;
   }

   ulMsgUserP   = 0;
   ulMsgMarkerP = 0;   
}


//----------------------------------------------------------------------------//
// ~CpFrame()                                                                 //
// destructor                                                                 //
//----------------------------------------------------------------------------//
CpFrame::~CpFrame()
{

}


//----------------------------------------------------------------------------//
// bitrateSwitch()                                                            //
// get value of bit-rate switch                                               //
//----------------------------------------------------------------------------//
bool CpFrame::bitrateSwitch(void) const
{
   bool btResultT = false;

   if(frameType() > eTYPE_CAN_EXT)
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
uint8_t  CpFrame::data(const uint8_t & ubPosR) const
{
   if(ubPosR < this->dataSize())
   {
      return(aubByteP[ubPosR]);
   }
   
   return(0);
}



//----------------------------------------------------------------------------//
// dataSize()                                                                 //
// get data size in bytes                                                     //
//----------------------------------------------------------------------------//
uint8_t CpFrame::dataSize(void) const
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
uint16_t CpFrame::dataUInt16(const uint8_t & ubPosR,
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
uint32_t CpFrame::dataUInt32(const uint8_t & ubPosR,
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
uint8_t CpFrame::dlc(void) const
{
   return(ubMsgDlcP);
}


//----------------------------------------------------------------------------//
// errorStateIndicator()                                                      //
// get value of error state indicator                                         //
//----------------------------------------------------------------------------//
bool CpFrame::errorStateIndicator(void) const
{
   bool btResultT = false;

   if(frameType() > eTYPE_CAN_EXT)
   {
      if((ubMsgCtrlP & CAN_FRAME_ISO_FD_ESI) > 0)
      {
         btResultT = true;
      }
   }
   return(btResultT);
}


//----------------------------------------------------------------------------//
// frameType()                                                                //
// get CAN frame type                                                         //
//----------------------------------------------------------------------------//
CpFrame::Type_e  CpFrame::frameType(void) const
{
   return((Type_e) (ubMsgCtrlP & 0x03));
}




//----------------------------------------------------------------------------//
// identifier()                                                               //
// get identifier value                                                       //
//----------------------------------------------------------------------------//
uint32_t CpFrame::identifier(void) const
{
   uint32_t ulIdValueT;
   
   if((ubMsgCtrlP & CAN_FRAME_FORMAT_EXT) > 0)
   {
      ulIdValueT = ulIdentifierP & CAN_FRAME_ID_MASK_EXT;
   }
   else
   {
      ulIdValueT = ulIdentifierP & CAN_FRAME_ID_MASK_STD;
   }
   
   return(ulIdValueT);
}


//----------------------------------------------------------------------------//
// isExtended()                                                               //
// test for Extended Frame format                                             //
//----------------------------------------------------------------------------//
bool CpFrame::isExtended(void) const
{
   bool btResultT = false;
   
   if((ubMsgCtrlP & CAN_FRAME_FORMAT_EXT) > 0)
   {
      btResultT = true;
   }
   return(btResultT);
}


//----------------------------------------------------------------------------//
// isFrameApi()                                                               //
// test for API frame                                                         //
//----------------------------------------------------------------------------//
bool CpFrame::isFrameApi() const
{
   bool btResultT = false;

   if((ulIdentifierP & CAN_FRAME_TYPE_API) > 0)
   {
      btResultT = true;
   }

   return(btResultT);
}


//----------------------------------------------------------------------------//
// isFrameError()                                                             //
// test for error frame                                                       //
//----------------------------------------------------------------------------//
bool CpFrame::isFrameError() const
{
   bool btResultT = false;

   if((ulIdentifierP & CAN_FRAME_TYPE_ERR) > 0)
   {
      btResultT = true;
   }
   return(btResultT);
}

//----------------------------------------------------------------------------//
// isRemote()                                                                 //
// test for Remote Frame format                                               //
//----------------------------------------------------------------------------//
bool CpFrame::isRemote(void) const
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
void CpFrame::setBitrateSwitch(const bool & btBrsR)
{
   if(frameType() > eTYPE_CAN_EXT)
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
void CpFrame::setData(const uint8_t & ubPosR, const uint8_t & ubValueR)
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
void CpFrame::setDataSize(uint8_t ubSizeV)
{
   if(frameType() > eTYPE_CAN_EXT)
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
void CpFrame::setDataUInt16(const uint8_t & ubPosR,
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
void CpFrame::setDataUInt32(const uint8_t & ubPosR,
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
void CpFrame::setDlc(uint8_t ubDlcV)
{
   //----------------------------------------------------------------
   // limit the DLC value range from 0 .. 15
   //
   ubDlcV = ubDlcV & 0x0F;
   
   if(frameType() < eTYPE_FD_STD)
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
// setFrameType()                                                             //
// set the frame type                                                         //
//----------------------------------------------------------------------------//
void CpFrame::setFrameType(const Type_e &ubTypeR)
{
   ubMsgCtrlP &= (~0x03);     // remove existing frame type bits
   ubMsgCtrlP |= ubTypeR;

   //----------------------------------------------------------------
   // check for Classic CAN frame
   //
   if(ubTypeR < eTYPE_FD_STD)
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
void CpFrame::setIdentifier(uint32_t ulIdentifierV)
{
   switch(frameType())
   {
      case eTYPE_CAN_STD:
      case eTYPE_FD_STD:
         ulIdentifierP = ulIdentifierV & CAN_FRAME_ID_MASK_STD;
         break;

      case eTYPE_CAN_EXT:
      case eTYPE_FD_EXT:
         ulIdentifierP = ulIdentifierV & CAN_FRAME_ID_MASK_EXT;
         break;

   }
}





void CpFrame::setMarker(const uint32_t & ulMarkerValueR)
{
   ulMsgMarkerP = ulMarkerValueR;
}


//----------------------------------------------------------------------------//
// setRemote()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
void CpFrame::setRemote(const bool & btRtrR)
{
   if(frameType() < eTYPE_FD_STD)
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


void CpFrame::setUser(const uint32_t & ulUserValueR)
{
   ulMsgUserP = ulUserValueR;
}


//----------------------------------------------------------------------------//
// operator ==                                                                //
// compare two CAN frames                                                     //
//----------------------------------------------------------------------------//
bool CpFrame::operator==(const CpFrame & clCanFrameR)
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
bool CpFrame::operator!=(const CpFrame & clCanFrameR)
{
   return(!(*this == clCanFrameR));
}

