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
#include "qcan_frame.hpp"

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define  QCAN_FRAME_ARRAY_SIZE      96

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
   ubMsgCtrlP = 0;
   
   tsMsgTimeP.ulSeconds     = 0;
   tsMsgTimeP.ulNanoSeconds = 0;
   
   ulMsgUserP = 0;
}


//----------------------------------------------------------------------------//
// QCanFrame()                                                                //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanFrame::~QCanFrame()
{
   
}

//----------------------------------------------------------------------------//
// data()                                                                     //
// get data                                                                   //
//----------------------------------------------------------------------------//
uint8_t  QCanFrame::data(uint8_t ubPosV)
{
   if(ubPosV < this->dataSize())
   {
      return(aubByteP[ubPosV]);
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
// dlc()                                                                      //
// get CAN DLC value                                                          //
//----------------------------------------------------------------------------//
uint8_t QCanFrame::dlc(void)
{
   return(ubMsgDlcP);
}


//----------------------------------------------------------------------------//
// frameType()                                                                //
// get CAN frame type                                                         //
//----------------------------------------------------------------------------//
QCanFrame::FrameType  QCanFrame::frameType(void) const
{
   return((FrameType) (ubMsgCtrlP & 0x03));
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
   uwChecksumT = uwChecksumT + clByteArrayR[95];
   
   if(uwChecksumT != qChecksum(clByteArrayR.constData(), 
                               QCAN_FRAME_ARRAY_SIZE - 2))
   {
      return(false);
   }
   
   //----------------------------------------------------------------
   // structure seems to be valid, now start copying the contents,
   // start with the identifier value
   //
   
   return(true);
}


//----------------------------------------------------------------------------//
// isExtended()                                                               //
// test for Extended Frame format                                             //
//----------------------------------------------------------------------------//
bool QCanFrame::isExtended(void) const
{
   if((ubMsgCtrlP & QCAN_FRAME_FORMAT_EXT) > 0)
   {
      return(true);
   }
   return(false);
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


//----------------------------------------------------------------------------//
// setStdId()                                                                 //
// test for Extended Frame format                                             //
//----------------------------------------------------------------------------//
void QCanFrame::setStdId(uint16_t uwIdentifierV)
{
   ulIdentifierP = (uwIdentifierV & QCAN_FRAME_ID_MASK_STD);
   ubMsgCtrlP   &= (~QCAN_FRAME_FORMAT_EXT);
}


//----------------------------------------------------------------------------//
// toByteArray()                                                              //
// test for Extended Frame format                                             //
//----------------------------------------------------------------------------//
void QCanFrame::toByteArray(QByteArray & clByteArrayR) const
{
   //----------------------------------------------------------------
   // setup a defined length and clear contents
   //
   clByteArrayR.resize(QCAN_FRAME_ARRAY_SIZE);
   clByteArrayR.clear();
   
   //----------------------------------------------------------------
   // place identifier field in byte 0 .. 3, MSB first
   //
   clByteArrayR[0] = (uint8_t) (ulIdentifierP >> 24);
   clByteArrayR[1] = (uint8_t) (ulIdentifierP >> 16);
   clByteArrayR[2] = (uint8_t) (ulIdentifierP >>  8);
   clByteArrayR[3] = (uint8_t) (ulIdentifierP >>  0);
   
   //----------------------------------------------------------------
   // place message DLC field in byte 4
   //
   clByteArrayR[4] = ubMsgDlcP;

   //----------------------------------------------------------------
   // place message control field in byte 5
   //
   clByteArrayR[5] = ubMsgCtrlP;

   //----------------------------------------------------------------
   // place message data field in byte 6 .. 69
   //
   for(uint8_t ubPosT = 0; ubPosT < CAN_FRAME_DATA_MAX; ubPosT++)
   {
      clByteArrayR[6 + ubPosT] = aubByteP[ubPosT];
   }

   //----------------------------------------------------------------
   // place message timestamp field in byte 70 .. 81, MSB first
   //
   clByteArrayR[70] = (uint8_t) (tsMsgTimeP.ulSeconds >> 24);
   clByteArrayR[71] = (uint8_t) (tsMsgTimeP.ulSeconds >> 16);
   clByteArrayR[72] = (uint8_t) (tsMsgTimeP.ulSeconds >>  8);
   clByteArrayR[73] = (uint8_t) (tsMsgTimeP.ulSeconds >>  0);
   clByteArrayR[74] = (uint8_t) (tsMsgTimeP.ulNanoSeconds >>  24);
   clByteArrayR[75] = (uint8_t) (tsMsgTimeP.ulNanoSeconds >>  16);
   clByteArrayR[76] = (uint8_t) (tsMsgTimeP.ulNanoSeconds >>   8);
   clByteArrayR[77] = (uint8_t) (tsMsgTimeP.ulNanoSeconds >>   0);
   
   //----------------------------------------------------------------
   // place message user field in byte 78 .. 81, MSB first
   //
   clByteArrayR[78] = (uint8_t) (ulMsgUserP >>  24);
   clByteArrayR[79] = (uint8_t) (ulMsgUserP >>  16);
   clByteArrayR[80] = (uint8_t) (ulMsgUserP >>   8);
   clByteArrayR[81] = (uint8_t) (ulMsgUserP >>   0);
   
   //----------------------------------------------------------------
   // byte 86 .. 93 (i.e. 8 bytes) are not used, set to 0
   //----------------------------------------------------------------
   
   //----------------------------------------------------------------
   // build checksum from byte 0 .. 93, add checksum at the end
   // 
   uint16_t uwChecksumT = qChecksum(clByteArrayR.constData(), 
                                    QCAN_FRAME_ARRAY_SIZE - 2);
   
   clByteArrayR[94] = (uint8_t) (uwChecksumT >> 8);
   clByteArrayR[95] = (uint8_t) (uwChecksumT >> 0);
   
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


//----------------------------------------------------------------------------//
// operator <<                                                                //
// stream to a QString object                                                 //
//----------------------------------------------------------------------------//
QString & operator<< (QString & clStringR, 
                      const QCanFrame & clCanFrameR)
{
   
   return(clStringR);
}

