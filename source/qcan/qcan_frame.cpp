//====================================================================================================================//
// File:          qcan_frame.cpp                                                                                      //
// Description:   QCAN classes - CAN frame                                                                            //
//                                                                                                                    //
// Copyright (C) MicroControl GmbH & Co. KG                                                                           //
// 53844 Troisdorf - Germany                                                                                          //
// www.microcontrol.net                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the   //
// following conditions are met:                                                                                      //
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions, the following   //
//    disclaimer and the referenced file 'LICENSE'.                                                                   //
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       //
//    following disclaimer in the documentation and/or other materials provided with the distribution.                //
// 3. Neither the name of MicroControl nor the names of its contributors may be used to endorse or promote products   //
//    derived from this software without specific prior written permission.                                           //
//                                                                                                                    //
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance     //
// with the License. You may obtain a copy of the License at                                                          //
//                                                                                                                    //
//    http://www.apache.org/licenses/LICENSE-2.0                                                                      //
//                                                                                                                    //
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed   //
// on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for  //
// the specific language governing permissions and limitations under the License.                                     //
//                                                                                                                    //
//====================================================================================================================//

/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QCanFrame>

//----------------------------------------------------------------------------------------------------------------
// These three files are included with an absolute file path to ensure that the correct platform definition is
// passed to the CANpie structures and settings. The definitions for the CANpie message structure are done
// inside the file "cp_platform.h" located inside the "device/qcan" directory.
//
#include "../device/qcan/cp_platform.h"
#include "../misc/mc_compiler.h"
#include "../canpie-fd/canpie.h"

/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#define CAN_FRAME_FORMAT_EXT ((uint8_t)0x01)

#define CAN_FRAME_FORMAT_FDF ((uint8_t)0x02)

#define CAN_FRAME_FORMAT_RTR ((uint8_t)0x04)

#define CAN_FRAME_TYPE_ERROR ((uint32_t)0x20000000)

#define CAN_FRAME_TYPE_MASK ((uint32_t)0xE0000000)

#define CAN_FRAME_ISO_FD_BRS ((uint8_t)0x40)

#define CAN_FRAME_ISO_FD_ESI ((uint8_t)0x80)

/*--------------------------------------------------------------------------------------------------------------------*\
** Local variables                                                                                                    **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

const uint8_t aubDlcToSizeC[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};

/*--------------------------------------------------------------------------------------------------------------------*\
** Class methods                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame()                                                                                                        //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanFrame::QCanFrame()
{
   //---------------------------------------------------------------------------------------------------
   // set default values
   //
   ubMsgCtrlP = 0;
   ulMsgUserP = 0;
   ulMsgMarkerP = 0;

   clMsgTimeP.clear();

   setFrameType(eFRAME_TYPE_DATA);
   setFrameFormat(eFORMAT_CAN_STD);
   setDlc(0);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame()                                                                                                        //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanFrame::QCanFrame(const FrameType_e &ubTypeR)
{
   //---------------------------------------------------------------------------------------------------
   // set default values
   //
   ubMsgCtrlP = 0;
   ulMsgUserP = 0;
   ulMsgMarkerP = 0;

   clMsgTimeP.clear();

   setFrameType(ubTypeR);
   setFrameFormat(eFORMAT_CAN_STD);
   setDlc(0);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame()                                                                                                        //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanFrame::QCanFrame(const FrameFormat_e &ubFormatR, const uint32_t &ulIdentifierR, const uint8_t &ubDlcR)
{
   //---------------------------------------------------------------------------------------------------
   // set default values
   //
   ubMsgCtrlP = 0;
   ulMsgUserP = 0;
   ulMsgMarkerP = 0;

   clMsgTimeP.clear();

   setFrameType(eFRAME_TYPE_DATA);
   setFrameFormat(ubFormatR);
   setIdentifier(ulIdentifierR);
   setDlc(ubDlcR);
}


//--------------------------------------------------------------------------------------------------------------------//
// ~QCanFrame()                                                                                                       //
// destructor                                                                                                         //
//--------------------------------------------------------------------------------------------------------------------//
QCanFrame::~QCanFrame()
{
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::bitrateSwitch()                                                                                         //
// get value of bit-rate switch                                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanFrame::bitrateSwitch(void) const
{
   bool btResultT = false;

   if (frameFormat() > eFORMAT_CAN_EXT)
   {
      if ((ubMsgCtrlP & CAN_FRAME_ISO_FD_BRS) > 0)
      {
         btResultT = true;
      }
   }
   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::data()                                                                                                  //
// get data                                                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
uint8_t QCanFrame::data(const uint8_t &ubPosR) const
{
   if (ubPosR < this->dataSize())
   {
      return (aubByteP[ubPosR]);
   }

   return (0);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::data()                                                                                                  //
// get data                                                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
QByteArray QCanFrame::data(void) const
{
   QByteArray clDataT = QByteArray((const char *)&aubByteP[0],
                                   this->dataSize());
   return (clDataT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::dataSize()                                                                                              //
// get data size in bytes                                                                                             //
//--------------------------------------------------------------------------------------------------------------------//
uint8_t QCanFrame::dataSize(void) const
{
   uint8_t ubSizeT = aubDlcToSizeC[ubMsgDlcP];

   return (ubSizeT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::dataUInt16()                                                                                            //
// get uint16_t value which is stored at position ubPosR                                                              //
//--------------------------------------------------------------------------------------------------------------------//
uint16_t QCanFrame::dataUInt16(const uint8_t &ubPosR, const bool &btMsbFirstR) const
{
   uint16_t uwValueT = 0;

   if ( (ubPosR < (this->dataSize() - 1)) && ((this->dataSize() > 1)) )
   {
      if (btMsbFirstR)
      {
         uwValueT = aubByteP[ubPosR];
         uwValueT = uwValueT << 8;
         uwValueT = uwValueT + (uint8_t)aubByteP[ubPosR + 1];
      }
      else
      {
         uwValueT = aubByteP[ubPosR + 1];
         uwValueT = uwValueT << 8;
         uwValueT = uwValueT + (uint8_t)aubByteP[ubPosR];
      }
   }

   return (uwValueT);
}

//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::dataUInt32()                                                                                            //
// get uint32_t value which is stored at position ubPosR                                                              //
//--------------------------------------------------------------------------------------------------------------------//
uint32_t QCanFrame::dataUInt32(const uint8_t &ubPosR, const bool &btMsbFirstR) const
{
   uint32_t ulValueT = 0;

   if ( (ubPosR < (this->dataSize() - 3)) && ((this->dataSize() > 3)) )
   {
      if (btMsbFirstR)
      {
         ulValueT = aubByteP[ubPosR];
         ulValueT = ulValueT << 8;
         ulValueT = ulValueT + (uint8_t)aubByteP[ubPosR + 1];
         ulValueT = ulValueT << 8;
         ulValueT = ulValueT + (uint8_t)aubByteP[ubPosR + 2];
         ulValueT = ulValueT << 8;
         ulValueT = ulValueT + (uint8_t)aubByteP[ubPosR + 3];
      }
      else
      {
         ulValueT = aubByteP[ubPosR + 3];
         ulValueT = ulValueT << 8;
         ulValueT = ulValueT + (uint8_t)aubByteP[ubPosR + 2];
         ulValueT = ulValueT << 8;
         ulValueT = ulValueT + (uint8_t)aubByteP[ubPosR + 1];
         ulValueT = ulValueT << 8;
         ulValueT = ulValueT + (uint8_t)aubByteP[ubPosR];
      }
   }

   return (ulValueT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::dlc()                                                                                                   //
// get CAN DLC value                                                                                                  //
//--------------------------------------------------------------------------------------------------------------------//
uint8_t QCanFrame::dlc(void) const
{
   return (ubMsgDlcP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::errorCounterReceive()                                                                                   //
// use data byte 2 for storage                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
uint8_t QCanFrame::errorCounterReceive(void) const
{
   uint8_t ubCounterValueT = 0;

   if (frameType() == eFRAME_TYPE_ERROR)
   {
      ubCounterValueT = aubByteP[2];
   }

   return (ubCounterValueT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::errorCounterTransmit()                                                                                  //
// use data byte 3 for storage                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
uint8_t QCanFrame::errorCounterTransmit(void) const
{
   uint8_t ubCounterValueT = 0;

   if (frameType() == eFRAME_TYPE_ERROR)
   {
      ubCounterValueT = aubByteP[3];
   }

   return (ubCounterValueT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::errorState()                                                                                            //
// use data byte 0 for storage                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
CAN_State_e QCanFrame::errorState(void) const
{
   CAN_State_e teStateT = eCAN_STATE_STOPPED;

   if (frameType() == eFRAME_TYPE_ERROR)
   {
      teStateT = (CAN_State_e)aubByteP[0];
   }

   return (teStateT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::errorStateIndicator()                                                                                   //
// get value of error state indicator                                                                                 //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanFrame::errorStateIndicator(void) const
{
   bool btResultT = false;

   if (frameFormat() > eFORMAT_CAN_EXT)
   {
      if ((ubMsgCtrlP & CAN_FRAME_ISO_FD_ESI) > 0)
      {
         btResultT = true;
      }
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::errorType()                                                                                             //
// use data byte 1 for storage                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanFrame::ErrorType_e QCanFrame::errorType(void) const
{
   ErrorType_e teTypeT = eERROR_TYPE_NONE;

   if (frameType() == eFRAME_TYPE_ERROR)
   {
      teTypeT = (ErrorType_e)aubByteP[1];
   }

   return (teTypeT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::frameFormat()                                                                                           //
// get CAN frame format                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
QCanFrame::FrameFormat_e QCanFrame::frameFormat(void) const
{
   return ((FrameFormat_e)(ubMsgCtrlP & 0x03));
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::frameType()                                                                                             //
// get CAN frame type                                                                                                 //
//--------------------------------------------------------------------------------------------------------------------//
QCanFrame::FrameType_e QCanFrame::frameType(void) const
{
   QCanFrame::FrameType_e teTypeT = eFRAME_TYPE_UNKNOWN;

   if ((ulIdentifierP & CAN_FRAME_TYPE_MASK) == 0)
   {
      teTypeT = eFRAME_TYPE_DATA;
   }

   if ((ulIdentifierP & CAN_FRAME_TYPE_ERROR) > 0)
   {
      teTypeT = eFRAME_TYPE_ERROR;
   }

   return (teTypeT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::fromByteArray()                                                                                         //
// Convert byte array to a QCanFrame object                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanFrame::fromByteArray(const QByteArray &clByteArrayR)
{
   bool btResultT = false;

   //---------------------------------------------------------------------------------------------------
   // test size of byte array and marker at the end
   //
   if ( (clByteArrayR.size() == QCAN_FRAME_ARRAY_SIZE) && 
        ((uint8_t) clByteArrayR[94] == 0xCA)           && 
        ((uint8_t) clByteArrayR[95] == 0x01) )
   {
      //---------------------------------------------------------------------------------------------------
      // structure seems to be valid, now start copying the contents,
      // start with the identifier value
      //---------------------------------------------------------------------------------------------------

      //---------------------------------------------------------------------------------------------------
      // set identifier field from byte 0 .. 3, MSB first
      //
      ulIdentifierP = clByteArrayR[0];
      ulIdentifierP = ulIdentifierP << 8;
      ulIdentifierP = ulIdentifierP + (uint8_t)clByteArrayR[1];
      ulIdentifierP = ulIdentifierP << 8;
      ulIdentifierP = ulIdentifierP + (uint8_t)clByteArrayR[2];
      ulIdentifierP = ulIdentifierP << 8;
      ulIdentifierP = ulIdentifierP + (uint8_t)clByteArrayR[3];

      //---------------------------------------------------------------------------------------------------
      // set DLC field from byte 4
      //
      ubMsgDlcP = clByteArrayR[4];

      //---------------------------------------------------------------------------------------------------
      // set message control field from byte 5
      //
      ubMsgCtrlP = clByteArrayR[5];

      //---------------------------------------------------------------------------------------------------
      // set message data field from byte 6 .. 69
      //
      for (uint8_t ubPosT = 0; ubPosT < QCAN_MSG_DATA_MAX; ubPosT++)
      {
         aubByteP[ubPosT] = clByteArrayR[6 + ubPosT];
      }

      //---------------------------------------------------------------------------------------------------
      // set message time-stamp field from byte 70 .. 77, MSB first
      //
      uint32_t ulTimeValT = 0;

      ulTimeValT = clByteArrayR[70];
      ulTimeValT = ulTimeValT << 8;
      ulTimeValT += (uint8_t)clByteArrayR[71];
      ulTimeValT = ulTimeValT << 8;
      ulTimeValT += (uint8_t)clByteArrayR[72];
      ulTimeValT = ulTimeValT << 8;
      ulTimeValT += (uint8_t)clByteArrayR[73];
      clMsgTimeP.setSeconds(ulTimeValT);

      ulTimeValT = 0;
      ulTimeValT = clByteArrayR[74];
      ulTimeValT = ulTimeValT << 8;
      ulTimeValT += (uint8_t)clByteArrayR[75];
      ulTimeValT = ulTimeValT << 8;
      ulTimeValT += (uint8_t)clByteArrayR[76];
      ulTimeValT = ulTimeValT << 8;
      ulTimeValT += (uint8_t)clByteArrayR[77];
      clMsgTimeP.setNanoSeconds(ulTimeValT);

      //---------------------------------------------------------------------------------------------------
      // set message user field from byte 78 .. 81, MSB first
      //
      ulMsgUserP = clByteArrayR[78];
      ulMsgUserP = ulMsgUserP << 8;
      ulMsgUserP += (uint8_t)clByteArrayR[79];
      ulMsgUserP = ulMsgUserP << 8;
      ulMsgUserP += (uint8_t)clByteArrayR[80];
      ulMsgUserP = ulMsgUserP << 8;
      ulMsgUserP += (uint8_t)clByteArrayR[81];

      //---------------------------------------------------------------------------------------------------
      // set message marker field from byte 82 .. 85, MSB first
      //
      ulMsgMarkerP = clByteArrayR[82];
      ulMsgMarkerP = ulMsgMarkerP << 8;
      ulMsgMarkerP += (uint8_t)clByteArrayR[83];
      ulMsgMarkerP = ulMsgMarkerP << 8;
      ulMsgMarkerP += (uint8_t)clByteArrayR[84];
      ulMsgMarkerP = ulMsgMarkerP << 8;
      ulMsgMarkerP += (uint8_t)clByteArrayR[85];

      btResultT = true;
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::fromCpCanMsg()                                                                                          //
// convert from CpCanMsg_ts structure                                                                                 //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanFrame::fromCpCanMsg(const CpCanMsg_ts *ptsCanMsgV)
{
   bool btResultT = false;

   //---------------------------------------------------------------------------------------------------
   // The conversion is only performed if the pointer to the CpCanMsg_s structure is valid and
   // the size of the structure has the defined size.
   //
   if ((ptsCanMsgV != (CpCanMsg_ts *)0))
   {
      uint8_t ubDataCntT;

      //-------------------------------------------------------------------------------------------
      // check frame format
      //
      if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_FDF_BIT) > 0)
      {
         if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_EXT_BIT) > 0)
         {
            this->setFrameFormat(QCanFrame::eFORMAT_FD_EXT);
         }
         else
         {
            this->setFrameFormat(QCanFrame::eFORMAT_FD_STD);
         }
      }
      else
      {
         if ((ptsCanMsgV->ubMsgCtrl & CP_MSG_CTRL_EXT_BIT) > 0)
         {
            this->setFrameFormat(QCanFrame::eFORMAT_CAN_EXT);
         }
         else
         {
            this->setFrameFormat(QCanFrame::eFORMAT_CAN_STD);
         }
      }

      //-------------------------------------------------------------------------------------------
      // set identifier and DLC
      //
      this->setIdentifier(ptsCanMsgV->ulIdentifier); // & CP_MASK_EXT_FRAME);
      this->setDlc(ptsCanMsgV->ubMsgDLC);

      for (ubDataCntT = 0; ubDataCntT < this->dataSize(); ubDataCntT++)
      {
         this->setData(ubDataCntT, ptsCanMsgV->tuMsgData.aubByte[ubDataCntT]);
      }

      btResultT = true;
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::identifier()                                                                                            //
// get identifier value                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
uint32_t QCanFrame::identifier(void) const
{
   uint32_t ulIdValueT;

   if ((ubMsgCtrlP & CAN_FRAME_FORMAT_EXT) > 0)
   {
      ulIdValueT = ulIdentifierP & QCAN_FRAME_ID_MASK_EXT;
   }
   else
   {
      ulIdValueT = ulIdentifierP & QCAN_FRAME_ID_MASK_STD;
   }

   return (ulIdValueT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::isExtended()                                                                                            //
// test for Extended Frame format                                                                                     //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanFrame::isExtended(void) const
{
   bool btResultT = false;

   if ((ubMsgCtrlP & CAN_FRAME_FORMAT_EXT) > 0)
   {
      btResultT = true;
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::isRemote()                                                                                              //
// test for Remote Frame format                                                                                       //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanFrame::isRemote(void) const
{
   bool btResultT = false;

   if ((ubMsgCtrlP & CAN_FRAME_FORMAT_RTR) > 0)
   {
      btResultT = true;
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::marker()                                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
uint32_t QCanFrame::marker(void) const
{
   return (ulMsgMarkerP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setBitrateSwitch()                                                                                      //
// set / reset bit-rate switch bit value                                                                              //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setBitrateSwitch(const bool &btBrsR)
{
   if (frameFormat() > eFORMAT_CAN_EXT)
   {
      if (btBrsR == true)
      {
         ubMsgCtrlP |= CAN_FRAME_ISO_FD_BRS;
      }
      else
      {
         ubMsgCtrlP &= (~CAN_FRAME_ISO_FD_BRS);
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setData()                                                                                               //
// set data value                                                                                                     //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setData(const uint8_t &ubPosR, const uint8_t &ubValueR)
{
   if (ubPosR < dataSize())
   {
      aubByteP[ubPosR] = ubValueR;
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setDataSize()                                                                                           //
// set data size value                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setDataSize(uint8_t ubSizeV)
{
   if (frameFormat() > eFORMAT_CAN_EXT)
   {
      //-------------------------------------------------------------------------------------------
      // set DLC value to maximum requested size value
      //
      if (ubSizeV <= 8)
      {
         ubMsgDlcP = ubSizeV;
      }
      else if ((ubSizeV > 8) && (ubSizeV <= 12))
      {
         ubMsgDlcP = 9;
      }
      else if ((ubSizeV > 12) && (ubSizeV <= 16))
      {
         ubMsgDlcP = 10;
      }
      else if ((ubSizeV > 16) && (ubSizeV <= 20))
      {
         ubMsgDlcP = 11;
      }
      else if ((ubSizeV > 20) && (ubSizeV <= 24))
      {
         ubMsgDlcP = 12;
      }
      else if ((ubSizeV > 24) && (ubSizeV <= 32))
      {
         ubMsgDlcP = 13;
      }
      else if ((ubSizeV > 32) && (ubSizeV <= 48))
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
      if (ubSizeV > 8)
      {
         ubSizeV = 8;
      }
      ubMsgDlcP = ubSizeV;
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setDataUInt16()                                                                                         //
// set data value                                                                                                     //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setDataUInt16(const uint8_t &ubPosR, const uint16_t &uwValueR, const bool &btMsbFirstR)
{
   uint16_t uwValueT = uwValueR;

   if ( (ubPosR < (this->dataSize() - 1)) && ((this->dataSize() > 1)) )
   {
      if (btMsbFirstR)
      {
         aubByteP[ubPosR + 1] = (uint8_t)uwValueT;
         uwValueT = uwValueT >> 8;
         aubByteP[ubPosR + 0] = (uint8_t)uwValueT;
      }
      else
      {
         aubByteP[ubPosR + 0] = (uint8_t)uwValueT;
         uwValueT = uwValueT >> 8;
         aubByteP[ubPosR + 1] = (uint8_t)uwValueT;
      }
   }
}

//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setDataUInt32()                                                                                         //
// set data value                                                                                                     //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setDataUInt32(const uint8_t &ubPosR, const uint32_t &ulValueR, const bool &btMsbFirstR)
{
   uint32_t ulValueT = ulValueR;

   if ( (ubPosR < (this->dataSize() - 3) && (this->dataSize() > 3)) )
   {
      if (btMsbFirstR)
      {
         aubByteP[ubPosR + 3] = (uint8_t)ulValueT;
         ulValueT = ulValueT >> 8;
         aubByteP[ubPosR + 2] = (uint8_t)ulValueT;
         ulValueT = ulValueT >> 8;
         aubByteP[ubPosR + 1] = (uint8_t)ulValueT;
         ulValueT = ulValueT >> 8;
         aubByteP[ubPosR + 0] = (uint8_t)ulValueT;
      }
      else
      {
         aubByteP[ubPosR + 0] = (uint8_t)ulValueT;
         ulValueT = ulValueT >> 8;
         aubByteP[ubPosR + 1] = (uint8_t)ulValueT;
         ulValueT = ulValueT >> 8;
         aubByteP[ubPosR + 2] = (uint8_t)ulValueT;
         ulValueT = ulValueT >> 8;
         aubByteP[ubPosR + 3] = (uint8_t)ulValueT;
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setDlc()                                                                                                //
// set DLC value, range test depends on frame format                                                                  //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setDlc(uint8_t ubDlcV)
{
   if (frameFormat() < eFORMAT_FD_STD)
   {
      //-------------------------------------------------------------------------------------------
      // classic CAN is limited to a DLC value of 8
      //
      if (ubDlcV <= 8)
      {
         ubMsgDlcP = ubDlcV;
      }
   }
   else
   {
      //-------------------------------------------------------------------------------------------
      // CAN FD can have DLC values from 0 .. 15
      //
      if (ubDlcV <= 15)
      {
         ubMsgDlcP = ubDlcV;
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setErrorCounterReceive()                                                                                //
// use data byte 2 for storage                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setErrorCounterReceive(const uint8_t &ubErrorCntR)
{
   if (frameType() == eFRAME_TYPE_ERROR)
   {
      aubByteP[2] = ubErrorCntR;

      //-------------------------------------------------------------------------------------------
      // Test the limits for both - receive and transmit error counter value - and change
      // the error state according to the limits
      //
      if ((aubByteP[2] < CAN_ERROR_LIMIT_WARNING) && (aubByteP[3] < CAN_ERROR_LIMIT_WARNING))
      {
         setErrorState(eCAN_STATE_BUS_ACTIVE);
      }

      if ((aubByteP[2] >= CAN_ERROR_LIMIT_WARNING) || (aubByteP[3] >= CAN_ERROR_LIMIT_WARNING))
      {
         setErrorState(eCAN_STATE_BUS_WARN);
      }

      if ((aubByteP[2] >= CAN_ERROR_LIMIT_PASSIVE) || (aubByteP[3] >= CAN_ERROR_LIMIT_PASSIVE))
      {
         setErrorState(eCAN_STATE_BUS_PASSIVE);
      }

      if (aubByteP[3] == CAN_ERROR_LIMIT_BUS_OFF)
      {
         setErrorState(eCAN_STATE_BUS_OFF);
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setErrorCounterTransmit()                                                                               //
// use data byte 3 for storage                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setErrorCounterTransmit(const uint8_t &ubErrorCntR)
{
   if (frameType() == eFRAME_TYPE_ERROR)
   {
      aubByteP[3] = ubErrorCntR;
   }

   //-------------------------------------------------------------------------------------------
   // Test the limits for both - receive and transmit error counter value - and change
   // the error state according to the limits
   //
   if ((aubByteP[2] < CAN_ERROR_LIMIT_WARNING) && (aubByteP[3] < CAN_ERROR_LIMIT_WARNING))
   {
      setErrorState(eCAN_STATE_BUS_ACTIVE);
   }

   if ((aubByteP[2] >= CAN_ERROR_LIMIT_WARNING) || (aubByteP[3] >= CAN_ERROR_LIMIT_WARNING))
   {
      setErrorState(eCAN_STATE_BUS_WARN);
   }

   if ((aubByteP[2] >= CAN_ERROR_LIMIT_PASSIVE) || (aubByteP[3] >= CAN_ERROR_LIMIT_PASSIVE))
   {
      setErrorState(eCAN_STATE_BUS_PASSIVE);
   }

   if (aubByteP[3] == CAN_ERROR_LIMIT_BUS_OFF)
   {
      setErrorState(eCAN_STATE_BUS_OFF);
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setErrorState()                                                                                         //
// use data byte 0 for storage                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setErrorState(CAN_State_e ubStateV)
{
   if (frameType() == eFRAME_TYPE_ERROR)
   {
      aubByteP[0] = ubStateV;
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setErrorStateIndicator()                                                                                //
// set the ESI bit                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setErrorStateIndicator(const bool &btEsiR)
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


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setErrorType()                                                                                          //
// use data byte 1 for storage                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setErrorType(ErrorType_e ubTypeV)
{
   if (frameType() == eFRAME_TYPE_ERROR)
   {
      aubByteP[1] = ubTypeV;
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setFrameFormat()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setFrameFormat(const FrameFormat_e &ubFormatR)
{
   ubMsgCtrlP &= (~0x03); // remove existing frame type bits
   ubMsgCtrlP |= ubFormatR;

   //----------------------------------------------------------------
   // check for Classic CAN frame
   //
   if (ubFormatR < eFORMAT_FD_STD)
   {
      //--------------------------------------------------------
      // ESI and BRS are cleared
      //
      ubMsgCtrlP &= (~CAN_FRAME_ISO_FD_BRS);
      ubMsgCtrlP &= (~CAN_FRAME_ISO_FD_ESI);

      //--------------------------------------------------------
      // make sure the DLC value is limited to 8
      //
      if (ubMsgDlcP > 8)
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


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setFrameType()                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setFrameType(const FrameType_e &ubTypeR)
{
   switch (ubTypeR)
   {
   //------------------------------------------------------------------------------------------------
   // CAN data frame
   //
   case eFRAME_TYPE_DATA:
      ulIdentifierP = (uint32_t)0;
      break;

   //------------------------------------------------------------------------------------------------
   // Mark error frame
   //
   case eFRAME_TYPE_ERROR:
      ulIdentifierP = CAN_FRAME_TYPE_ERROR;
      break;

   //------------------------------------------------------------------------------------------------
   // The frame type is not known, so set all bits inside the identifier field to 1
   //
   default:
      ulIdentifierP = (uint32_t)(~0);
   }

   memset(&aubByteP[0], 0x00, QCAN_MSG_DATA_MAX);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setIdentifier()                                                                                         //
// set identifier value                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setIdentifier(uint32_t ulIdentifierV)
{
   switch (frameFormat())
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


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setMarker()                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setMarker(const uint32_t &ulMarkerValueR)
{
   ulMsgMarkerP = ulMarkerValueR;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setRemote()                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setRemote(const bool &btRtrR)
{
   if (frameFormat() < eFORMAT_FD_STD)
   {
      if (btRtrR == true)
      {
         ubMsgCtrlP |= CAN_FRAME_FORMAT_RTR;
      }
      else
      {
         ubMsgCtrlP &= (~CAN_FRAME_FORMAT_RTR);
      }
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::setUser()                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanFrame::setUser(const uint32_t &ulUserValueR)
{
   ulMsgUserP = ulUserValueR;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::toByteArray()                                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QByteArray QCanFrame::toByteArray() const
{
   //----------------------------------------------------------------
   // setup a defined length and clear contents
   //
   QByteArray clByteArrayT(QCAN_FRAME_ARRAY_SIZE, 0x00);

   //----------------------------------------------------------------
   // place identifier field in byte 0 .. 3, MSB first
   //
   clByteArrayT[0] = (uint8_t)(ulIdentifierP >> 24);
   clByteArrayT[1] = (uint8_t)(ulIdentifierP >> 16);
   clByteArrayT[2] = (uint8_t)(ulIdentifierP >> 8);
   clByteArrayT[3] = (uint8_t)(ulIdentifierP >> 0);

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
   for (uint8_t ubPosT = 0; ubPosT < QCAN_MSG_DATA_MAX; ubPosT++)
   {
      clByteArrayT[6 + ubPosT] = aubByteP[ubPosT];
   }

   //----------------------------------------------------------------
   // place message timestamp field in byte 70 .. 77, MSB first
   //
   uint32_t ulTimeValT = 0;

   ulTimeValT = clMsgTimeP.seconds();
   clByteArrayT[70] = (uint8_t)(ulTimeValT >> 24);
   clByteArrayT[71] = (uint8_t)(ulTimeValT >> 16);
   clByteArrayT[72] = (uint8_t)(ulTimeValT >> 8);
   clByteArrayT[73] = (uint8_t)(ulTimeValT >> 0);

   ulTimeValT = clMsgTimeP.nanoSeconds();
   clByteArrayT[74] = (uint8_t)(ulTimeValT >> 24);
   clByteArrayT[75] = (uint8_t)(ulTimeValT >> 16);
   clByteArrayT[76] = (uint8_t)(ulTimeValT >> 8);
   clByteArrayT[77] = (uint8_t)(ulTimeValT >> 0);

   //----------------------------------------------------------------
   // place message user field in byte 78 .. 81, MSB first
   //
   clByteArrayT[78] = (uint8_t)(ulMsgUserP >> 24);
   clByteArrayT[79] = (uint8_t)(ulMsgUserP >> 16);
   clByteArrayT[80] = (uint8_t)(ulMsgUserP >> 8);
   clByteArrayT[81] = (uint8_t)(ulMsgUserP >> 0);

   //----------------------------------------------------------------
   // place message marker field in byte 82 .. 85, MSB first
   //
   clByteArrayT[82] = (uint8_t)(ulMsgMarkerP >> 24);
   clByteArrayT[83] = (uint8_t)(ulMsgMarkerP >> 16);
   clByteArrayT[84] = (uint8_t)(ulMsgMarkerP >> 8);
   clByteArrayT[85] = (uint8_t)(ulMsgMarkerP >> 0);

   //----------------------------------------------------------------
   // byte 86 .. 93 (i.e. 8 bytes) are not used, set to 0
   //----------------------------------------------------------------

   //----------------------------------------------------------------
   // set marker at end of byte array
   //
   clByteArrayT[94] = (uint8_t)(0xCA);
   clByteArrayT[95] = (uint8_t)(0x01);

   return (clByteArrayT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::toCpCanMsg()                                                                                            //
// convert QCanFrame                                                                                                  //
//--------------------------------------------------------------------------------------------------------------------//
bool QCanFrame::toCpCanMsg(struct CpCanMsg_s *ptsCanMsgV) const
{
   bool btResultT = false;

   //---------------------------------------------------------------------------------------------------
   // The QCanFrame object is only converted if the pointer to the CpCanMsg_s structure is valid and
   // the size of the structure has the defined size.
   //
   if ((ptsCanMsgV != (CpCanMsg_ts *)0))
   {
      //-------------------------------------------------------------------------------------------
      // copy the frame format
      //
      ptsCanMsgV->ubMsgCtrl = this->frameFormat();

      //-------------------------------------------------------------------------------------------
      // test for BRS bit
      //
      if (this->bitrateSwitch())
      {
         ptsCanMsgV->ubMsgCtrl |= CP_MSG_CTRL_BRS_BIT;
      }

      //-------------------------------------------------------------------------------------------
      // test for ESI bit
      //
      if (this->errorStateIndicator())
      {
         ptsCanMsgV->ubMsgCtrl |= CP_MSG_CTRL_ESI_BIT;
      }

      //-------------------------------------------------------------------------------------------
      // copy identifier
      //
      ptsCanMsgV->ulIdentifier = this->identifier();

      //-------------------------------------------------------------------------------------------
      // copy data
      //
      for (uint8_t ubDataCountT = 0; ubDataCountT < this->dataSize(); ubDataCountT++)
      {
         ptsCanMsgV->tuMsgData.aubByte[ubDataCountT] = this->data(ubDataCountT);
      }

      //-------------------------------------------------------------------------------------------
      // copy DLC
      //
      ptsCanMsgV->ubMsgDLC = this->dlc();

      btResultT = true;
   }

   return (btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::toString()                                                                                              //
// print CAN data or error frame                                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanFrame::toString(const bool &btShowTimeR, const bool &btShowDataSizeR) const
{
   //---------------------------------------------------------------------------------------------------
   // setup a string object
   //
   QString clStringT;
   uint32_t ulSecondsT = 0;
   uint32_t ulNanoSecT = 0;

   if (btShowTimeR == true)
   {
      ulSecondsT = this->timeStamp().seconds();
      ulNanoSecT = this->timeStamp().nanoSeconds();
      ulNanoSecT = ulNanoSecT / 10000;
      clStringT = QString("%1.%2 ").arg(ulSecondsT, 5, 10).arg(ulNanoSecT, 5, 10, QChar('0'));
   }

   //---------------------------------------------------------------------------------------------------
   // check frame type: CAN data frame
   //
   if (frameType() == eFRAME_TYPE_DATA)
   {
      //-------------------------------------------------------------------------------------------
      // print identifier
      //
      clStringT += QString("%1  ").arg(identifier(), 8, 16).toUpper();

      //-------------------------------------------------------------------------------------------
      // print frame format
      //
      switch (frameFormat())
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

      //-------------------------------------------------------------------------------------------
      // print frame flag for classic CAN : RTR           -> ubFrameFlagsT = 4
      // print frame flag for CAN FD      : BRS and ESI   -> ubFrameFlagsT = 1, 2, 3
      //
      uint8_t ubFrameFlagsT = 0;
      if (bitrateSwitch())
      {
         ubFrameFlagsT += 1;
      }

      if (errorStateIndicator())
      {
         ubFrameFlagsT += 2;
      }

      if (isRemote())
      {
         ubFrameFlagsT += 4;
      }

      switch (ubFrameFlagsT)
      {

      case 1:
         clStringT += "BRS     ";
         break;

      case 2:
         clStringT += "ESI     ";
         break;

      case 3:
         clStringT += "BRS,ESI ";
         break;

      case 4:
         clStringT += "RTR     ";
         break;

      default:
         clStringT += "        ";
         break;
      }

      //-------------------------------------------------------------------------------------------
      // print DLC or data size
      //
      if (btShowDataSizeR)
      {
      }
      else
      {
         clStringT += QString("%1  ").arg(dlc(), 2, 10);
      }

      //-------------------------------------------------------------------------------------------
      // print data
      //
      uint8_t ubDataSizeT = dataSize();
      if (isRemote())
      {
         ubDataSizeT = 0;
      }
      for (uint8_t ubCntT = 0; ubCntT < ubDataSizeT; ubCntT++)
      {
         //-----------------------------------------------------------------------------------
         // print a newline and 19/31 spaces after 32 data bytes
         //
         if ((ubCntT > 0) && ((ubCntT % 32) == 0))
         {
            if (btShowTimeR == true)
            {
               clStringT += "\n                               ";
            }
            else
            {
               clStringT += "\n                   ";
            }
         }

         //-----------------------------------------------------------------------------------
         // print data with 2 digits and in hex format
         //
         clStringT += QString("%1 ").arg(data(ubCntT), 2, 16, QLatin1Char('0')).toUpper();
      }
   }

   //---------------------------------------------------------------------------------------------------
   // check frame type: CAN error frame
   //
   if (frameType() == eFRAME_TYPE_ERROR)
   {
      //-------------------------------------------------------------------------------------------
      // print error frame, show current state first
      //
      clStringT += "   Error frame ----------  ";

      switch (this->errorState())
      {
      case eCAN_STATE_STOPPED:
         clStringT += "CAN controller stopped";
         break;

      case eCAN_STATE_SLEEPING:
         clStringT += "CAN controller sleeping";
         break;

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
         clStringT += "Unknown state";
         break;
      }

      //-------------------------------------------------------------------------------------------
      // print current values of error counters
      //
      clStringT += QString(", Error counter: Rcv=%1, Trm=%2 ").arg(errorCounterReceive()).arg(errorCounterTransmit());

      //-------------------------------------------------------------------------------------------
      // print current error type
      //
      switch (this->errorType())
      {
      case eERROR_TYPE_NONE:

         break;

      case eERROR_TYPE_BIT0:
      case eERROR_TYPE_BIT1:
         clStringT += ", bit error";
         break;

      case eERROR_TYPE_STUFF:
         clStringT += ", stuff error";
         break;

      case eERROR_TYPE_FORM:
         clStringT += ", form error";
         break;

      case eERROR_TYPE_CRC:
         clStringT += ", CRC error";
         break;

      case eERROR_TYPE_ACK:
         clStringT += ", acknowledgement error";
         break;
      }
   }

   return (clStringT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanFrame::user()                                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
uint32_t QCanFrame::user(void) const
{
   return (ulMsgUserP);
}


//--------------------------------------------------------------------------------------------------------------------//
// operator <<                                                                                                        //
// stream to a QDataStream object                                                                                     //
//--------------------------------------------------------------------------------------------------------------------//
QDataStream &operator<<(QDataStream &clStreamR, const QCanFrame &clCanFrameR)
{
   //---------------------------------------------------------------------------------------------------
   // set version of stream
   //
   clStreamR.setVersion(QDataStream::Qt_5_6);

   //---------------------------------------------------------------------------------------------------
   // place all members to the stream
   //
   clStreamR << clCanFrameR.ulIdentifierP;
   clStreamR << clCanFrameR.ubMsgDlcP;
   clStreamR << clCanFrameR.ubMsgCtrlP;

   for (uint8_t ubIndexT = 0; ubIndexT < QCAN_MSG_DATA_MAX; ubIndexT++)
   {
      clStreamR << clCanFrameR.aubByteP[ubIndexT];
   }

   clStreamR << clCanFrameR.clMsgTimeP.seconds();
   clStreamR << clCanFrameR.clMsgTimeP.nanoSeconds();

   clStreamR << clCanFrameR.ulMsgUserP;
   clStreamR << clCanFrameR.ulMsgMarkerP;

   return (clStreamR);
}


//--------------------------------------------------------------------------------------------------------------------//
// operator >>                                                                                                        //
// stream to a QDataStream object                                                                                     //
//--------------------------------------------------------------------------------------------------------------------//
QDataStream & operator>> (QDataStream & clStreamR, QCanFrame & clCanFrameR)
{
   if (clStreamR.version() == QDataStream::Qt_5_6)
   {
      clStreamR >> clCanFrameR.ulIdentifierP;
      clStreamR >> clCanFrameR.ubMsgDlcP;
      clStreamR >> clCanFrameR.ubMsgCtrlP;

      for (uint8_t ubIndexT = 0; ubIndexT < QCAN_MSG_DATA_MAX; ubIndexT++)
      {
         clStreamR >> clCanFrameR.aubByteP[ubIndexT];
      }

      uint32_t ulTimeValueT = 0;
      clStreamR >> ulTimeValueT;
      clCanFrameR.clMsgTimeP.setSeconds(ulTimeValueT);
      clStreamR >> ulTimeValueT;
      clCanFrameR.clMsgTimeP.setNanoSeconds(ulTimeValueT);

      clStreamR >> clCanFrameR.ulMsgUserP;
      clStreamR >> clCanFrameR.ulMsgMarkerP;      
   }
   return (clStreamR);
}