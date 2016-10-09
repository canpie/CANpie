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


#define  QCAN_FRAME_ID_MASK_STD     ((uint32_t) 0x000007FF)

#define  QCAN_FRAME_ID_MASK_EXT     ((uint32_t) 0x1FFFFFFF)

#define  QCAN_FRAME_FORMAT_EXT      ((uint8_t) 0x01)

#define  QCAN_FRAME_FORMAT_FD       ((uint8_t) 0x02)

#define  QCAN_FRAME_FORMAT_RTR      ((uint8_t) 0x04)

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
QCanFrame::QCanFrame(const Type_e & ubTypeR, const uint32_t & ulIdentifierR, 
                     const uint8_t & ubDlcR) 
         : CpFrame(ubTypeR, ulIdentifierR, ubDlcR)
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
   QByteArray clDataT((const char *)&aubByteP[0], this->dataSize());
   return(clDataT);
}

uint8_t QCanFrame::data(const uint8_t & ubPosR) const
{
   return(CpFrame::data(ubPosR));
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
   /*
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
   */
   
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


void QCanFrame::setData(const uint8_t & ubPosR, const uint8_t & ubValueR)
{
   CpFrame::setData(ubPosR, ubValueR);
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
   /*
   clByteArrayT[70] = (uint8_t) (tsMsgTimeP.ulSeconds >> 24);
   clByteArrayT[71] = (uint8_t) (tsMsgTimeP.ulSeconds >> 16);
   clByteArrayT[72] = (uint8_t) (tsMsgTimeP.ulSeconds >>  8);
   clByteArrayT[73] = (uint8_t) (tsMsgTimeP.ulSeconds >>  0);
   clByteArrayT[74] = (uint8_t) (tsMsgTimeP.ulNanoSeconds >>  24);
   clByteArrayT[75] = (uint8_t) (tsMsgTimeP.ulNanoSeconds >>  16);
   clByteArrayT[76] = (uint8_t) (tsMsgTimeP.ulNanoSeconds >>   8);
   clByteArrayT[77] = (uint8_t) (tsMsgTimeP.ulNanoSeconds >>   0);
   */
   
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
      
   clStreamR << clCanFrameR.clMsgTimeP.seconds();
   clStreamR << clCanFrameR.clMsgTimeP.nanoSeconds();
      
   clStreamR << clCanFrameR.ulMsgUserP;
   
   return(clStreamR);
}



