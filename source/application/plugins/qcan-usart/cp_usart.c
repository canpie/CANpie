//============================================================================//
// File:          cp_usart.c                                                  //
// Description:   CANpie core functions based on USART interface              //
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
#include "cp_core.h"
#include "cp_msg.h"
#include "mc_usart.h"

#include "string.h"

#if MC_USART_PORT_MAX > 0
#if (CP_CHANNEL_MAX > MC_USART_PORT_MAX)
#error CP_CHANNEL_MAX have to be <= MC_USART_PORT_MAX
#endif

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/
/*!
** \def   CP_USART_MODE
** Defines mode for USART interface
*/
#define CP_USART_MODE   eUSART_MODE_8E1

/*!
** \def   CP_USART_STATE_IDLE
** Flag for aubUsartStateS variable.
** It is used to mark the USART interface as available for a transmission
*/
#define CP_USART_STATE_IDLE  0x00

/*!
** \def   CP_USART_STATE_IDLE
** Flag for aubUsartStateS variable.
** It is used to mark the USART interface as busy, so a transmission is
** pending.
*/
#define CP_USART_STATE_BUSY  0x01

/*!
** \def  CP_BUFFER_VAL
** Flag for ulMsgUser variable within CANpie Message, that is used
** for handling with simulated CAN buffers
*/
#define CP_BUFFER_VAL   (uint32_t)0x00000002

/*!
** \def  CP_BUFFER_PND
** Flag for ulMsgUser variable within CANpie Message, that is used
** for handling with simulated CAN buffers
*/
#define CP_BUFFER_PND   (uint32_t)0x00000004

/*!
** \enum DrvInfo_e
*/
enum DrvInfo_e
{
   eDRV_INFO_OFF = 0,//!< eDRV_INFO_OFF
   eDRV_INFO_INIT,   //!< eDRV_INFO_INIT
   eDRV_INFO_ACTIVE  //!< eDRV_INFO_ACTIVE
};

/*----------------------------------------------------------------------------*/
/*!
** \struct  CpUsartFrame_s
** \brief   USART frame for streaming
**
*/
typedef struct CpUsartFrame_s
{

   /*!
   ** The identifier field may have 11 bits for standard frames
   ** (CAN specification 2.0A) or 29 bits for extended frames
   ** (CAN specification 2.0B). The three most significant bits
   ** are reserved (always read 0).
   ** \see CP_MASK
   */
   uint32_t  ulIdentifier;

   /*!
   ** The data field has up to 8 bytes (for classic CAN) or
   ** 64 bytes (for ISO CAN FD) of message data.
   ** The number of used bytes is described via the structure
   ** member \c ubMsgDLC.
   */
   uint8_t   aubData[CP_DATA_SIZE];

   /*!
   ** The data length code denotes the number of data bytes
   ** which are transmitted by a message.
   ** The possible value range for the data length code is
   ** from 0 to 8 for classic CAN and 0 to 15 for ISO CAN FD.
   */
   uint8_t  ubMsgDLC;


   /*!
   ** The structure member \c ubMsgCtrl defines the
   ** different data frames (2.0A / 2.0B) and the RTR frames.
   ** <ul>
   ** <li>Bit 0: Std. / Ext. Frame
   ** <li>Bit 1: ISO CAN FD: value of FDF bit
   ** <li>Bit 2: Remote Frame
   ** <li>Bit 3: Overload Frame
   ** <li>Bit 4: reserved, always 0
   ** <li>Bit 5: reserved, always 0
   ** <li>Bit 6: ISO CAN FD: value of BRS bit
   ** <li>Bit 7: ISO CAN FD: value of ESI bit
   ** </ul>
   ** \see CP_MSG_CTRL
   */
   uint8_t  ubMsgCtrl;


} CPP_PACK CpUsartFrame_ts;


/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/


/*!
** \var  atsUsartRcvFrameS
** Reception buffer for USAT frame
*/
CpUsartFrame_ts atsUsartRcvFrameS[MC_USART_PORT_MAX];

/*!
** \var  atsUsartTrmFrameS
** Transmission buffer for USAT frame
*/
CpUsartFrame_ts atsUsartTrmFrameS[MC_USART_PORT_MAX];

/*!
** \var  aubUsartStateS
** Holds status of an USART interface
*/
uint8_t         aubUsartStateS[MC_USART_PORT_MAX];


//-------------------------------------------------------------------
// simulation of CAN message buffer
//
static CpCanMsg_ts atsCanMsgS[CP_CHANNEL_MAX][CP_BUFFER_MAX];
static CpFifo_ts  *aptsFifoS[CP_CHANNEL_MAX][CP_BUFFER_MAX];
static uint32_t    aulAccMaskS[CP_CHANNEL_MAX][CP_BUFFER_MAX];
static CpPort_ts  *aptsPortS[CP_CHANNEL_MAX];

static uint8_t     ubCanModeS;
static CpState_ts  atsCpStateS[CP_CHANNEL_MAX];

#if CP_STATISTIC > 0
static CpStatistic_ts atsCpStatisticS[CP_CHANNEL_MAX];
#endif

//-------------------------------------------------------------------
// these pointers store the callback handlers
//
static CpRcvHandler_Fn  apfnRcvHandlerS[CP_CHANNEL_MAX];
static CpTrmHandler_Fn  apfnTrmHandlerS[CP_CHANNEL_MAX];
static CpErrHandler_Fn  apfnErrHandlerS[CP_CHANNEL_MAX];

/*!
 * \brief ubUsartRcvBufferS
 *
 * This buffer was introduced to adapt the implementation to the new USART API.
 * Finally, only the API was adapted, the buffer is currently not used,
 * because the qcan_usart implementation manages its own buffer internally.
 */
static uint8_t ubUsartRcvBufferS[128];

/*----------------------------------------------------------------------------*\
** Internal function declaration                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
void CpUsart1RcvHandler(uint8_t *pubDataV, uint32_t ulSizeV);
void CpUsart1TrmHandler(uint8_t *pubDataV, uint32_t ulSizeV);

#if MC_USART_PORT_MAX > 1
void CpUsart2RcvHandler(uint8_t *pubDataV, uint32_t ulSizeV);
void CpUsart2TrmHandler(uint8_t *pubDataV, uint32_t ulSizeV);
#endif

/*----------------------------------------------------------------------------*\
** Function implementation                                                    **
**                                                                            **
\*----------------------------------------------------------------------------*/

//----------------------------------------------------------------------------//
// CalcUsartBitrate()                                                         //
// Calculate USART bit rate from given nominal CAN bit rate                   //
//----------------------------------------------------------------------------//
static int32_t CalcUsartBitrate(int32_t slNomBitRateV)
{
   int32_t slUsartBitrateT = slNomBitRateV;

   if (slNomBitRateV <= eCP_BITRATE_1M)
   {
      switch (slNomBitRateV)
      {
         case eCP_BITRATE_20K:
            slUsartBitrateT = 19200;
            break;

         case eCP_BITRATE_50K:
            slUsartBitrateT = 57600;
            break;

         case eCP_BITRATE_125K:
            slUsartBitrateT = 115200;
            break;

         case eCP_BITRATE_250K:
            slUsartBitrateT = 256000;
            break;

         default:
            slUsartBitrateT = 115200;
            break;
      }
   }

   return slUsartBitrateT;
}

//----------------------------------------------------------------------------//
// CheckParam()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
static CpStatus_tv CheckParam(CPP_CONST CpPort_ts *ptsPortV,
                              CPP_CONST uint8_t ubBufferIdxV,
                              CPP_CONST uint8_t unReqStateV)
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      tvStatusT = eCP_ERR_INIT_MISSING;

      //--------------------------------------------------------
      // check for initialisation
      //
      if (ptsPortV->ubDrvInfo >= unReqStateV)
      {
         tvStatusT = eCP_ERR_BUFFER;

         //------------------------------------------------
         // check for valid buffer number
         //
         if (ubBufferIdxV < CP_BUFFER_MAX)
         {
            tvStatusT = eCP_ERR_NONE;
         }
      }
   }

   return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpPortToUsart()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
static uint8_t CpPortToUsart(CPP_CONST CpPort_ts *ptsPortV)
{
   if (ptsPortV->ubPhyIf > eCP_CHANNEL_NONE)
   {
      return (ptsPortV->ubPhyIf - 1);
   }

   return eUSART_PORT_1;
}


//----------------------------------------------------------------------------//
// CpCanMsgFromUsart()                                                        //
// Copy data from CAN message into USART frame                                //
//----------------------------------------------------------------------------//
static void CopyCanMsgToUsartFrame(CPP_CONST CpCanMsg_ts *ptsCanMsgV,
                                   CpUsartFrame_ts *ptsUsartFrameV)
{
   uint8_t ubCntrT;

   if ((ptsCanMsgV != CPP_NULL) && (ptsUsartFrameV != CPP_NULL))
   {
      ptsUsartFrameV->ubMsgDLC = ptsCanMsgV->ubMsgDLC;
      ptsUsartFrameV->ulIdentifier = ptsCanMsgV->ulIdentifier;
      ptsUsartFrameV->ubMsgCtrl = ptsCanMsgV->ubMsgCtrl;
      for (ubCntrT = 0; ubCntrT < CP_DATA_SIZE; ubCntrT++)
      {
         ptsUsartFrameV->aubData[ubCntrT] = ptsCanMsgV->tuMsgData.aubByte[ubCntrT];
      }
   }
}

//----------------------------------------------------------------------------//
// CpCanMsgFromUsart()                                                        //
// Copy data from USART frame into CAN message                                //
//----------------------------------------------------------------------------//
static void CopyUsartFrameToCanMsg(CpCanMsg_ts *ptsCanMsgV,
                                   CPP_CONST CpUsartFrame_ts *ptsUsartFrameV)
{
   uint8_t ubCntrT;

   if ((ptsCanMsgV != CPP_NULL) && (ptsUsartFrameV != CPP_NULL))
   {
      ptsCanMsgV->ubMsgDLC = ptsUsartFrameV->ubMsgDLC;
      ptsCanMsgV->ulIdentifier = ptsUsartFrameV->ulIdentifier;
      ptsCanMsgV->ubMsgCtrl = ptsUsartFrameV->ubMsgCtrl;
      for (ubCntrT = 0; ubCntrT < CP_DATA_SIZE; ubCntrT++)
      {
         ptsCanMsgV->tuMsgData.aubByte[ubCntrT] = ptsUsartFrameV->aubData[ubCntrT];
      }
   }
}



//----------------------------------------------------------------------------//
// CpCanMsgFromUsart()                                                        //
// Copy data from USART frame into CAN message                                //
//----------------------------------------------------------------------------//
static CpCanMsg_ts *CpCanMsgFromBuffer(CPP_CONST CpUsartFrame_ts
                                       *ptsUsartFrameV,
                                       uint8_t ubDirectionV,
                                       uint8_t *pubBufferIdxV)
{
   CpCanMsg_ts *ptsBufferMsgT = CPP_NULL;
   uint8_t      ubBufferIdxT;

   //----------------------------------------------------------------
   // go through all valid transmit buffers and check for identifier hit
   //
   for (ubBufferIdxT = 0; ubBufferIdxT < CP_BUFFER_MAX; ubBufferIdxT++)
   {
      if ((atsCanMsgS[eCP_CHANNEL_1-1][ubBufferIdxT].ulMsgUser &
            (CP_BUFFER_VAL | eCP_BUFFER_DIR_TRM)) == (CP_BUFFER_VAL | ubDirectionV))
      {
         // consider mask at comparison of identifier
         if ( ((atsCanMsgS[eCP_CHANNEL_1-1][ubBufferIdxT].ulIdentifier &
               aulAccMaskS[eCP_CHANNEL_1-1][ubBufferIdxT]) ==
               (ptsUsartFrameV->ulIdentifier &
                aulAccMaskS[eCP_CHANNEL_1-1][ubBufferIdxT])) &&
            // consider identifier type (standard or extended)
            ((atsCanMsgS[eCP_CHANNEL_1-1][ubBufferIdxT].ubMsgCtrl & CP_MSG_FORMAT_CEFF) ==
             (ptsUsartFrameV->ubMsgCtrl & CP_MSG_FORMAT_CEFF)))
         {
            // we have a hit, get pointer to the corresponding buffer
            ptsBufferMsgT = &(atsCanMsgS[eCP_CHANNEL_1-1][ubBufferIdxT]);

            if (pubBufferIdxV != CPP_NULL)
            {
               *pubBufferIdxV = ubBufferIdxT;
            }

            // quit loop here
            break;
         }
      }
   }

   return ptsBufferMsgT;
}


//----------------------------------------------------------------------------//
// CpCoreBitrate()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBitrate(CpPort_ts *ptsPortV, int32_t slNomBitRateV,
                          int32_t slDatBitRateV)
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {

      if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
      {
         tvStatusT = eCP_ERR_NONE;

         //-----------------------------------------------------
         // test bit-rate
         //
         if ((slNomBitRateV > eCP_BITRATE_1M) ||
               (slNomBitRateV == eCP_BITRATE_NONE))
         {
            tvStatusT = eCP_ERR_BITRATE;
         }
         if ((slDatBitRateV != eCP_BITRATE_NONE) &&
               (slNomBitRateV > slDatBitRateV))
         {
            tvStatusT = eCP_ERR_BITRATE;
         }

         //-----------------------------------------------------
         // configure the btr register here
         //
         if (tvStatusT == eCP_ERR_NONE)
         {
            McUsartRelease(CpPortToUsart(ptsPortV));
            McUsartInit(CpPortToUsart(ptsPortV), CalcUsartBitrate(slNomBitRateV),
                        CP_USART_MODE);
         }
      }
      else
      {
         tvStatusT = eCP_ERR_INIT_MISSING;
      }
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferConfig()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferConfig(CpPort_ts *ptsPortV,
                               uint8_t   ubBufferIdxV,
                               uint32_t  ulIdentifierV,
                               uint32_t  ulAcceptMaskV,
                               uint8_t   ubFormatV,
                               uint8_t   ubDirectionV)
{
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      //--------------------------------------------------------
      // test message format and mask identifier
      //
      switch (ubFormatV & CP_MASK_MSG_FORMAT)
      {
         case CP_MSG_FORMAT_CBFF:
         case CP_MSG_FORMAT_FBFF:
            ulIdentifierV = ulIdentifierV & CP_MASK_STD_FRAME;
            ulAcceptMaskV = ulAcceptMaskV & CP_MASK_STD_FRAME;
            atsCanMsgS[ptsPortV->ubPhyIf-1][ubBufferIdxV].ubMsgCtrl = CP_MSG_FORMAT_CBFF;
            break;

         case CP_MSG_FORMAT_CEFF:
         case CP_MSG_FORMAT_FEFF:
            ulIdentifierV = ulIdentifierV & CP_MASK_EXT_FRAME;
            ulAcceptMaskV = ulAcceptMaskV & CP_MASK_EXT_FRAME;
            atsCanMsgS[ptsPortV->ubPhyIf-1][ubBufferIdxV].ubMsgCtrl = CP_MSG_FORMAT_CEFF;
            break;
         default :
            break;
      }

      //-----------------------------------------------------
      // store identifier and acceptance mask
      //
      atsCanMsgS[ptsPortV->ubPhyIf-1][ubBufferIdxV].ulIdentifier = ulIdentifierV;
      atsCanMsgS[ptsPortV->ubPhyIf-1][ubBufferIdxV].ulMsgUser  =
         (uint8_t) ubDirectionV;
      atsCanMsgS[ptsPortV->ubPhyIf-1][ubBufferIdxV].ulMsgUser |= CP_BUFFER_VAL;
      aulAccMaskS[ptsPortV->ubPhyIf-1][ubBufferIdxV] = ulAcceptMaskV;
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferGetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetData(CpPort_ts *ptsPortV,
                                uint8_t    ubBufferIdxV,
                                uint8_t   *pubDestDataV,
                                uint8_t    ubStartPosV,
                                uint8_t    ubSizeV)
{
   CpStatus_tv tvStatusT;
   uint8_t     ubCntT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      //--------------------------------------------------------
      // test start position and size
      //
      if ((ubStartPosV + ubSizeV) > CP_DATA_SIZE)
      {
         tvStatusT = eCP_ERR_PARAM;
      }
      else
      {
         //---------------------------------------------------
         // copy data from simulated CAN buffer
         //
         for (ubCntT = ubStartPosV; ubCntT < ubSizeV; ubCntT++)
         {
            *pubDestDataV = CpMsgGetData(&atsCanMsgS[ptsPortV->ubPhyIf-1][ubBufferIdxV], ubCntT);
            pubDestDataV++;
         }
      }
   }


   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferGetDlc()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetDlc(CpPort_ts *ptsPortV,
                               uint8_t ubBufferIdxV,
                               uint8_t *pubDlcV)
{
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      //----------------------------------------------------------------
      // read DLC from simulated CAN buffer
      //
      *pubDlcV = atsCanMsgS[ptsPortV->ubPhyIf-1][ubBufferIdxV].ubMsgDLC;
   }

   return (tvStatusT);
}



//----------------------------------------------------------------------------//
// CpCoreBufferRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferRelease(CpPort_ts *ptsPortV,
                                uint8_t ubBufferIdxV)
{
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {

   }


   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSend()                                                         //
// send message out of the CAN controller                                     //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSend(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV)
{
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      //--------------------------------------------------------
      // if USART interface is available, transmit frame
      //
      if ((aubUsartStateS[CpPortToUsart(ptsPortV)] & CP_USART_STATE_IDLE) ==
            CP_USART_STATE_IDLE)
      {
         aubUsartStateS[CpPortToUsart(ptsPortV)] |= CP_USART_STATE_BUSY;

         CopyCanMsgToUsartFrame(&(atsCanMsgS[ptsPortV->ubPhyIf-1][ubBufferIdxV]),
                                &(atsUsartTrmFrameS[CpPortToUsart(ptsPortV)]));

         McUsartWrite((ptsPortV->ubPhyIf-1),
                      (uint8_t *)&(atsUsartTrmFrameS[CpPortToUsart(ptsPortV)]),
                      sizeof(CpUsartFrame_ts));
      }
      else
      {
         atsCanMsgS[ptsPortV->ubPhyIf-1][ubBufferIdxV].ulMsgUser |= CP_BUFFER_PND;
      }
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSetData(CpPort_ts *ptsPortV,
                                uint8_t    ubBufferIdxV,
                                uint8_t   *pubSrcDataV,
                                uint8_t    ubStartPosV,
                                uint8_t    ubSizeV)
{
   CpStatus_tv tvStatusT;
   uint8_t     ubCntT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      //--------------------------------------------------------
      // test start position and size
      //
      if ((ubStartPosV + ubSizeV) > CP_DATA_SIZE)
      {
         tvStatusT = eCP_ERR_PARAM;
      }
      else
      {
         //---------------------------------------------------
         // copy data from simulated CAN buffer
         //
         for (ubCntT = ubStartPosV; ubCntT < ubSizeV; ubCntT++)
         {
            CpMsgSetData(&(atsCanMsgS[ptsPortV->ubPhyIf-1][ubBufferIdxV]), ubCntT, *pubSrcDataV);
            pubSrcDataV++;
         }
      }
   }


   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSetDlc()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSetDlc(CpPort_ts *ptsPortV,
                               uint8_t ubBufferIdxV,
                               uint8_t ubDlcV)
{
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      //--------------------------------------------------------
      // write DLC to simulated CAN buffer
      //
      atsCanMsgS[ptsPortV->ubPhyIf-1][ubBufferIdxV].ubMsgDLC = ubDlcV;
   }

   return (tvStatusT);
}



//----------------------------------------------------------------------------//
// CpCoreCanMode()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanMode(CpPort_ts *ptsPortV, uint8_t ubModeV)
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
      {
         tvStatusT = eCP_ERR_NONE;

         //-----------------------------------------------------
         // switch CAN controller into mode "ubModeV"
         //
         switch (ubModeV)
         {
            //---------------------------------------------
            // Stop the CAN controller (passive on the bus)
            //
            case eCP_MODE_STOP:
               McUsartSetDir(CpPortToUsart(ptsPortV), eUSART_DIR_NONE);
               ubCanModeS = ubModeV;
               break;

            //---------------------------------------------
            // Start the CAN controller (active on the bus)
            //
            case eCP_MODE_START:
               McUsartSetRcvBufferSize(CpPortToUsart(ptsPortV), sizeof(CpUsartFrame_ts), &ubUsartRcvBufferS[0]);
               McUsartSetDir(CpPortToUsart(ptsPortV), eUSART_DIR_RXTX);
               ubCanModeS = ubModeV;
               break;

            //--------------------------------------------------------
            // Start the CAN controller (Listen-Only)
            //
            case eCP_MODE_LISTEN_ONLY:
               McUsartSetRcvBufferSize(CpPortToUsart(ptsPortV), sizeof(CpUsartFrame_ts), &ubUsartRcvBufferS[0]);
               McUsartSetDir(CpPortToUsart(ptsPortV), eUSART_DIR_RX);
               ubCanModeS = ubModeV;
               break;

            //--------------------------------------------------------
            // Operate in self-test mode
            //
            case eCP_MODE_SELF_TEST:
               ubCanModeS = ubModeV;
               break;

            //--------------------------------------------------------
            // Other modes are not supported
            //
            default:
               tvStatusT = eCP_ERR_NOT_SUPPORTED;
               break;
         }
      }
   }

   return (tvStatusT);
}



//----------------------------------------------------------------------------//
// CpCoreCanState()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanState(CpPort_ts *ptsPortV, CpState_ts *ptsStateV)
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
      {
         tvStatusT = eCP_ERR_NONE;

         //-----------------------------------------------------
         // get current error counter
         //
         ptsStateV->ubCanTrmErrCnt = 0;
         ptsStateV->ubCanRcvErrCnt = 0;

      }
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreDriverInit()                                                         //
// init CAN controller                                                        //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverInit(uint8_t ubPhyIfV, CpPort_ts *ptsPortV,
                             uint8_t CPP_PARM_UNUSED(ubConfigV))
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;
   uint8_t ubBufferIdxT;

   //----------------------------------------------------------------
   // test physical CAN channel
   //
   if (ubPhyIfV <= CP_CHANNEL_MAX)
   {

      //--------------------------------------------------------
      // test CAN port
      //
      if (ptsPortV != (CpPort_ts *) 0L)
      {
         if (ptsPortV->ubDrvInfo == eDRV_INFO_OFF)
         {
            ptsPortV->ubPhyIf   = ubPhyIfV;
            ptsPortV->ubDrvInfo = eDRV_INFO_INIT;

            // store port information for internal usage
            aptsPortS[ptsPortV->ubPhyIf-1] = ptsPortV;

            // reset error handler
            apfnRcvHandlerS[ptsPortV->ubPhyIf-1] = CPP_NULL;
            apfnTrmHandlerS[ptsPortV->ubPhyIf-1] = CPP_NULL;
            apfnErrHandlerS[ptsPortV->ubPhyIf-1] = CPP_NULL;

            // reset all CAN buffers
            for (ubBufferIdxT = 0; ubBufferIdxT < CP_BUFFER_MAX; ubBufferIdxT++)
            {
               memset(&atsCanMsgS[ptsPortV->ubPhyIf-1][ubBufferIdxT], 0x00,
                      sizeof(CpCanMsg_ts));
               aptsFifoS[ptsPortV->ubPhyIf-1][ubBufferIdxT] = CPP_NULL;
            }

            // reset receive and transmit buffers for USART frames
            memset(&atsUsartRcvFrameS[CpPortToUsart(ptsPortV)], 0x00,
                   sizeof(CpUsartFrame_ts));
            memset(&atsUsartTrmFrameS[CpPortToUsart(ptsPortV)], 0x00,
                   sizeof(CpUsartFrame_ts));

            aubUsartStateS[CpPortToUsart(ptsPortV)] = CP_USART_STATE_IDLE;

            tvStatusT = eCP_ERR_NONE;
         }
         else
         {
            //---------------------------------------------
            // already initialised
            //
            tvStatusT = eCP_ERR_INIT_FAIL;
         }
      }
      else
      {
         //-----------------------------------------------------
         // parameter ptsPortV is not correct
         //
         tvStatusT = eCP_ERR_PARAM;
      }
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreDriverRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverRelease(CpPort_ts *ptsPortV)
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
      {
         tvStatusT = CpCoreCanMode(ptsPortV, eCP_MODE_STOP);
         ptsPortV->ubDrvInfo = eDRV_INFO_OFF;
      }
      else
      {
         tvStatusT = eCP_ERR_INIT_MISSING;
      }
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreFifoConfig()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoConfig(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV,
                             CpFifo_ts *ptsFifoV)
{
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      if (ptsFifoV != (CpFifo_ts *) 0)
      {
         aptsFifoS[ptsPortV->ubPhyIf-1][ubBufferIdxV] = ptsFifoV;
      }
      else
      {
         tvStatusT = eCP_ERR_FIFO_PARAM;
      }
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreFifoRead()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoRead(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV,
                           CpCanMsg_ts *ptsCanMsgV,
                           uint32_t *pulBufferSizeV)
{
   CpFifo_ts   *ptsFifoT;
   CpCanMsg_ts *ptsFifoMsgT;
   CpStatus_tv  tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      if (aptsFifoS[ptsPortV->ubPhyIf-1][ubBufferIdxV] == CPP_NULL)
      {
         tvStatusT = eCP_ERR_INIT_MISSING;
      }

      else if ((pulBufferSizeV == (uint32_t *) 0L) ||
               (ptsCanMsgV == (CpCanMsg_ts *) 0L))
      {
         tvStatusT = eCP_ERR_PARAM;
      }

      else
      {
         ptsFifoT = aptsFifoS[ptsPortV->ubPhyIf-1][ubBufferIdxV];

         if (CpFifoIsEmpty(ptsFifoT) == 1)
         {
            // FIFO is empty, no data has been copied
            *pulBufferSizeV = 0;
            tvStatusT = eCP_ERR_FIFO_EMPTY;
         }
         else
         {
            ptsFifoMsgT = CpFifoDataOutPtr(ptsFifoT);
            memcpy(ptsCanMsgV, ptsFifoMsgT, sizeof(CpCanMsg_ts));
            CpFifoIncOut(ptsFifoT);
            *pulBufferSizeV = 1;
            tvStatusT = eCP_ERR_NONE;
         }
      }
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreFifoRelease()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoRelease(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV)
{
   CpStatus_tv tvStatusT;

   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      aptsFifoS[ptsPortV->ubPhyIf-1][ubBufferIdxV] = CPP_NULL;
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreFifoWrite()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoWrite(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV,
                            CpCanMsg_ts *ptsCanMsgV,
                            uint32_t *pulBufferSizeV)
{
   CpFifo_ts   *ptsFifoT;
   CpCanMsg_ts *ptsFifoMsgT;
   CpStatus_tv  tvStatusT;


   //----------------------------------------------------------------
   // test parameter ptsPortV and ubBufferIdxV
   //
   tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
   if (tvStatusT == eCP_ERR_NONE)
   {
      if (aptsFifoS[ptsPortV->ubPhyIf-1][ubBufferIdxV] == CPP_NULL)
      {
         tvStatusT = eCP_ERR_INIT_MISSING;
      }

      else if ((pulBufferSizeV == (uint32_t *) 0L) ||
               (ptsCanMsgV == (CpCanMsg_ts *) 0L))
      {
         tvStatusT = eCP_ERR_PARAM;
      }

      else
      {
         ptsFifoT = aptsFifoS[ptsPortV->ubPhyIf-1][ubBufferIdxV];

         //------------------------------------------------
         // check if buffer is busy
         //
         if ((atsCanMsgS[ptsPortV->ubPhyIf-1][ubBufferIdxV].ulMsgUser & CP_BUFFER_PND) !=
               0)
         {
            if (CpFifoIsFull(ptsFifoT) == 1)
            {
               // FIFO is empty, no data has been copied
               *pulBufferSizeV = 0;
               tvStatusT = eCP_ERR_FIFO_FULL;
            }
            else
            {
               ptsFifoMsgT = CpFifoDataInPtr(ptsFifoT);
               memcpy(ptsFifoMsgT, ptsCanMsgV, sizeof(CpCanMsg_ts));
               CpFifoIncIn(ptsFifoT);
               *pulBufferSizeV = 1;
               tvStatusT = eCP_ERR_NONE;
            }
         }
         else
         {
            memcpy(&atsCanMsgS[ptsPortV->ubPhyIf-1][ubBufferIdxV], ptsCanMsgV,
                   sizeof(CpCanMsg_ts));
            tvStatusT = CpCoreBufferSend(ptsPortV, ubBufferIdxV);
         }
      }
   }

   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreHDI()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreHDI(CpPort_ts *ptsPortV, CpHdi_ts *ptsHdiV)
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
      {
         tvStatusT = eCP_ERR_NONE;

         if (ptsHdiV != (CpHdi_ts *) 0)
         {
            //--------------------------------------------------
            // get version number
            //
            ptsHdiV->ubVersionMajor = CP_VERSION_MAJOR;
            ptsHdiV->ubVersionMinor = CP_VERSION_MINOR;

            ptsHdiV->ubCanFeatures    = 0;
            ptsHdiV->ubDriverFeatures = 0;

            ptsHdiV->ubBufferMax = CP_BUFFER_MAX;

         }
         else
         {
            tvStatusT = eCP_ERR_PARAM;
         }
      }
   }


   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreIntFunctions()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreIntFunctions(CpPort_ts *ptsPortV,
                               CpRcvHandler_Fn pfnRcvHandlerV,
                               CpTrmHandler_Fn pfnTrmHandlerV,
                               CpErrHandler_Fn pfnErrHandlerV)
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
      {
         tvStatusT = eCP_ERR_NONE;

         //-----------------------------------------------------
         // store the new callbacks
         //
         apfnRcvHandlerS[ptsPortV->ubPhyIf-1] = pfnRcvHandlerV;
         apfnTrmHandlerS[ptsPortV->ubPhyIf-1] = pfnTrmHandlerV;
         apfnErrHandlerS[ptsPortV->ubPhyIf-1] = pfnErrHandlerV;

         if (CpPortToUsart(ptsPortV) == eUSART_PORT_1)
         {
            if (pfnRcvHandlerV != CPP_NULL)
            {
               McUsartSetRcvHandler(CpPortToUsart(ptsPortV), CpUsart1RcvHandler);
            }

            if (pfnTrmHandlerV != CPP_NULL)
            {
               McUsartSetTrmHandler(CpPortToUsart(ptsPortV), CpUsart1TrmHandler);
            }
         }

         #if MC_USART_PORT_MAX > 1
         if (CpPortToUsart(ptsPortV) == eUSART_PORT_2)
         {
            if (pfnRcvHandlerV != CPP_NULL)
            {
               McUsartSetRcvHandler(CpPortToUsart(ptsPortV), CpUsart2RcvHandler);
            }

            if (pfnTrmHandlerV != CPP_NULL)
            {
               McUsartSetTrmHandler(CpPortToUsart(ptsPortV), CpUsart2TrmHandler);
            }
         }
         #endif

      }
   }



   return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreStatistic()                                                          //
// return statistical information                                             //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreStatistic(CpPort_ts *ptsPortV,
                            CpStatistic_ts *ptsStatsV)
{
   CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

   //----------------------------------------------------------------
   // test CAN port
   //
   if (ptsPortV != (CpPort_ts *) 0L)
   {
      if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
      {
         tvStatusT = eCP_ERR_NONE;

         ptsStatsV->ulErrMsgCount = 0;
         ptsStatsV->ulRcvMsgCount = 0;
         ptsStatsV->ulTrmMsgCount = 0;

      }

   }


   return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpUsart1RcvHandler()                                                       //
// is called when new have been fully received into atsUsartRcvFrameS         //
//----------------------------------------------------------------------------//
void CpUsart1RcvHandler(uint8_t *pubDataV, uint32_t ulSizeV)
{
   static uint8_t    ubBufferIdxT = 0;
   CpCanMsg_ts      *ptsBufferMsgT;
   CpFifo_ts        *ptsFifoT;
   CpCanMsg_ts      *ptsFifoMsgT;

   //----------------------------------------------------------------
   // we should never run in here!
   //
   if (ulSizeV != sizeof(CpUsartFrame_ts))
   {
      if (apfnErrHandlerS[eCP_CHANNEL_1-1] != CPP_NULL)
      {
         atsCpStateS[eCP_CHANNEL_1-1].ubCanErrType = 0xFF;
         (* apfnErrHandlerS[eCP_CHANNEL_1-1])(&atsCpStateS[eCP_CHANNEL_1-1]);
      }
   }

   //----------------------------------------------------------------
   // get corresponding buffer
   //
   ptsBufferMsgT = CpCanMsgFromBuffer((CpUsartFrame_ts *) pubDataV,
                                      eCP_BUFFER_DIR_RCV,
                                      &ubBufferIdxT);

   //----------------------------------------------------------------
   // process buffer at a hit
   //
   if (ptsBufferMsgT != CPP_NULL)
   {
      CopyUsartFrameToCanMsg(ptsBufferMsgT, (CpUsartFrame_ts *) pubDataV);

      #if CP_STATISTIC > 0
      atsCpStatisticS[eCP_CHANNEL_1-1].ulRcvMsgCount++;
      #endif

      //----------------------------------------------------------------
      // test for FIFO
      //
      if (aptsFifoS[eCP_CHANNEL_1-1][ubBufferIdxT] == CPP_NULL)
      {
         //--------------------------------------------------------
         // test for receive callback handler and pass data to it
         //
         if (apfnRcvHandlerS[eCP_CHANNEL_1-1] != CPP_NULL)
         {
            (* apfnRcvHandlerS[eCP_CHANNEL_1-1])(ptsBufferMsgT, ubBufferIdxT);
         }
      }
      else
      {
         //--------------------------------------------------------
         // put new message into receive FIFO
         //
         ptsFifoT = aptsFifoS[eCP_CHANNEL_1-1][ubBufferIdxT];
         if (CpFifoIsEmpty(ptsFifoT) == 0)
         {
            ptsFifoMsgT = CpFifoDataInPtr(ptsFifoT);
            memcpy(ptsFifoMsgT, ptsBufferMsgT, sizeof(CpCanMsg_ts));
            CpFifoIncIn(ptsFifoT);
         }
      }

   }
}

//----------------------------------------------------------------------------//
// CpUsart1TrmHandler()                                                       //
// is called when all data from atsUsartTrmFrameS have been send              //
//----------------------------------------------------------------------------//
void CpUsart1TrmHandler(uint8_t *pubDataV, uint32_t ulSizeV)
{
   uint8_t          ubBufferIdxT  = 0;
   CpCanMsg_ts     *ptsBufferMsgT;
   CpFifo_ts       *ptsFifoT;
   CpCanMsg_ts     *ptsFifoMsgT;

   //----------------------------------------------------------------
   // we should never run in here!
   //
   if (ulSizeV != sizeof(CpUsartFrame_ts))
   {
      if (apfnErrHandlerS[eCP_CHANNEL_1-1] != CPP_NULL)
      {
         atsCpStateS[eCP_CHANNEL_1-1].ubCanErrType = 0xFF;
         (* apfnErrHandlerS[eCP_CHANNEL_1-1])(&atsCpStateS[eCP_CHANNEL_1-1]);
      }
   }

   // clear BUSY flag, so next transmission can be done
   aubUsartStateS[eCP_CHANNEL_1-1] &= ~CP_USART_STATE_IDLE;

   //----------------------------------------------------------------
   // get corresponding buffer
   //
   ptsBufferMsgT = CpCanMsgFromBuffer((CpUsartFrame_ts *) pubDataV,
                                      eCP_BUFFER_DIR_TRM,
                                      &ubBufferIdxT);

   //----------------------------------------------------------------
   // process buffer at a hit
   //
   if (ptsBufferMsgT != CPP_NULL)
   {
      #if CP_STATISTIC > 0
      atsCpStatisticS[eCP_CHANNEL_1-1].ulTrmMsgCount++;
      #endif

      //--------------------------------------------------------
      // test for transmit callback handler
      //
      if (aptsFifoS[eCP_CHANNEL_1-1][ubBufferIdxT] == CPP_NULL)
      {
         if (apfnTrmHandlerS[eCP_CHANNEL_1-1] != CPP_NULL)
         {
            (* apfnTrmHandlerS[eCP_CHANNEL_1-1])(ptsBufferMsgT, ubBufferIdxT);
         }
      }
      else
      {
         ptsFifoT = aptsFifoS[eCP_CHANNEL_1-1][ubBufferIdxT];
         if (CpFifoIsEmpty(ptsFifoT) == 0)
         {
            ptsFifoMsgT = CpFifoDataOutPtr(ptsFifoT);
            memcpy(ptsBufferMsgT, ptsFifoMsgT, sizeof(CpCanMsg_ts));
            CpFifoIncOut(ptsFifoT);
            ptsFifoMsgT->ulMsgUser |= CP_BUFFER_PND;
         }
      }
   }

   //----------------------------------------------------------------
   // run through buffer list and test for open Tx requests
   //
   for (ubBufferIdxT = 0; ubBufferIdxT < CP_BUFFER_MAX; ubBufferIdxT++)
   {
      ptsBufferMsgT = &(atsCanMsgS[eCP_CHANNEL_1-1][ubBufferIdxT]);
      if (((ptsBufferMsgT->ulMsgUser) & CP_BUFFER_PND))
      {
         ptsBufferMsgT->ulMsgUser &= ~CP_BUFFER_PND;
         CpCoreBufferSend(aptsPortS[0], ubBufferIdxT);
      }
   }
}

//----------------------------------------------------------------------------//
// CpUsart2RcvHandler()                                                       //
// is called when new have been fully received into atsUsartRcvFrameS         //
//----------------------------------------------------------------------------//
#if MC_USART_PORT_MAX > 1
void CpUsart2RcvHandler(uint8_t *pubDataV, uint32_t ulSizeV)
{
   uint8_t           ubBufferIdxT  = 0;
   CpCanMsg_ts      *ptsBufferMsgT;
   CpFifo_ts        *ptsFifoT;
   CpCanMsg_ts      *ptsFifoMsgT;

   //----------------------------------------------------------------
   // we should never run in here!
   //
   if (ulSizeV != sizeof(CpUsartFrame_ts))
   {
      if (apfnErrHandlerS[eCP_CHANNEL_2-1] != CPP_NULL)
      {
         atsCpStateS[eCP_CHANNEL_2-1].ubCanErrType = 0xFF;
         (* apfnErrHandlerS[eCP_CHANNEL_2-1])(&atsCpStateS[eCP_CHANNEL_2-1]);
      }
   }

   //----------------------------------------------------------------
   // get corresponding buffer
   //
   ptsBufferMsgT = CpCanMsgFromBuffer((CpUsartFrame_ts *) pubDataV,
                                      eCP_BUFFER_DIR_RCV,
                                      &ubBufferIdxT);

   //----------------------------------------------------------------
   // process buffer at a hit
   //
   if (ptsBufferMsgT != CPP_NULL)
   {
      CopyUsartFrameToCanMsg(ptsBufferMsgT, (CpUsartFrame_ts *) pubDataV);

      #if CP_STATISTIC > 0
      atsCpStatisticS[eCP_CHANNEL_2-1].ulRcvMsgCount++;
      #endif

      //----------------------------------------------------------------
      // test for FIFO
      //
      if (aptsFifoS[eCP_CHANNEL_2-1][ubBufferIdxT] == CPP_NULL)
      {
         //--------------------------------------------------------
         // test for receive callback handler and pass data to it
         //
         if (apfnRcvHandlerS[eCP_CHANNEL_2-1] != CPP_NULL)
         {
            (* apfnRcvHandlerS[eCP_CHANNEL_2-1])(ptsBufferMsgT, ubBufferIdxT);
         }
      }
      else
      {
         //--------------------------------------------------------
         // put new message into receive FIFO
         //
         ptsFifoT = aptsFifoS[eCP_CHANNEL_2-1][ubBufferIdxT];
         if (CpFifoIsEmpty(ptsFifoT) == 0)
         {
            ptsFifoMsgT = CpFifoDataInPtr(ptsFifoT);
            memcpy(ptsFifoMsgT, ptsBufferMsgT, sizeof(CpCanMsg_ts));
            CpFifoIncIn(ptsFifoT);
         }
      }
   }
}
#endif

//----------------------------------------------------------------------------//
// CpUsart2TrmHandler()                                                       //
// is called when all data from atsUsartTrmFrameS have been send              //
//----------------------------------------------------------------------------//
#if MC_USART_PORT_MAX > 1
void CpUsart2TrmHandler(uint8_t *pubDataV, uint32_t ulSizeV)
{
   uint8_t          ubBufferIdxT  = 0;
   CpCanMsg_ts     *ptsBufferMsgT;
   CpFifo_ts       *ptsFifoT;
   CpCanMsg_ts     *ptsFifoMsgT;

   //----------------------------------------------------------------
   // we should never run in here!
   //
   if (ulSizeV != sizeof(CpUsartFrame_ts))
   {
      if (apfnErrHandlerS[eCP_CHANNEL_2-1] != CPP_NULL)
      {
         atsCpStateS[eCP_CHANNEL_2-1].ubCanErrType = 0xFF;
         (* apfnErrHandlerS[eCP_CHANNEL_2-1])(&atsCpStateS[eCP_CHANNEL_2-1]);
      }
   }

   // clear BUSY flag, so next transmission can be done
   aubUsartStateS[eCP_CHANNEL_2-1] &= ~CP_USART_STATE_IDLE;

   //----------------------------------------------------------------
   // get corresponding buffer
   //
   ptsBufferMsgT = CpCanMsgFromBuffer((CpUsartFrame_ts *) pubDataV,
                                      eCP_BUFFER_DIR_TRM,
                                      &ubBufferIdxT);

   //----------------------------------------------------------------
   // process buffer at a hit
   //
   if (ptsBufferMsgT != CPP_NULL)
   {
      #if CP_STATISTIC > 0
      atsCpStatisticS[eCP_CHANNEL_2-1].ulTrmMsgCount++;
      #endif

      //--------------------------------------------------------
      // test for transmit callback handler
      //
      if (aptsFifoS[eCP_CHANNEL_2-1][ubBufferIdxT] == CPP_NULL)
      {
         if (apfnTrmHandlerS[eCP_CHANNEL_2-1] != CPP_NULL)
         {
            (* apfnTrmHandlerS[eCP_CHANNEL_2-1])(ptsBufferMsgT, ubBufferIdxT);
         }
      }
      else
      {
         ptsFifoT = aptsFifoS[eCP_CHANNEL_2-1][ubBufferIdxT];
         if (CpFifoIsEmpty(ptsFifoT) == 0)
         {
            ptsFifoMsgT = CpFifoDataOutPtr(ptsFifoT);
            memcpy(ptsBufferMsgT, ptsFifoMsgT, sizeof(CpCanMsg_ts));
            CpFifoIncOut(ptsFifoT);
            ptsFifoMsgT->ulMsgUser |= CP_BUFFER_PND;
         }
      }
   }

   //----------------------------------------------------------------
   // run through buffer list and test for open Tx requests
   //
   for (ubBufferIdxT = 0; ubBufferIdxT < CP_BUFFER_MAX; ubBufferIdxT++)
   {
      ptsBufferMsgT = &(atsCanMsgS[eCP_CHANNEL_2-1][ubBufferIdxT]);
      if (((ptsBufferMsgT->ulMsgUser) & CP_BUFFER_PND))
      {
         ptsBufferMsgT->ulMsgUser &= ~CP_BUFFER_PND;
         CpCoreBufferSend(aptsPortS[0], ubBufferIdxT);
      }
   }
}
#endif

//uint32_t CpUsartFrameSize()
//{
//   return (uint32_t)sizeof(CpUsartFrame_ts);
//}
#endif // MC_USART_PORT_MAX > 0
