//============================================================================//
// File:          qcan_usart.hpp                                              //
// Description:   QCAN USART library API                                      //
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
// ("LGPL") version 3 as distributed in the 'COPYING' file.                   //
//                                                                            //
//============================================================================//

#ifndef QCAN_USART_H_
#define QCAN_USART_H_

#include <QtCore/QObject>


#include <QtCore/QLibrary>
#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "cp_core.h"
#include "cp_fifo.h"
#include "cp_msg.h"

#include "mc_usart.h"


//----------------------------------------------------------------------------//
// QCanPeakUsb                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
class QCanUsart : public QObject
{
    Q_OBJECT

public:
   typedef struct QCanUsartConfig_s{
      QString clName;
      qint32  slBaud;
      quint8  ubDirection;  // USART_DIR_e from mc_usart.h
      quint8  ubMode;       // USART_MODE_e from mc_usart.h
   } QCanUsartConfig_ts;



private:
   QCanUsart();
   QCanUsart(const QCanUsart &);

   QSerialPort    *pclSerialPortP;

   bool btLibFuncLoadP;

   QCanUsartConfig_ts tsConfigP;
   UsartHandler_fn    pfnUsartRcvHandlerP;
   UsartHandler_fn    pfnUsartTrmHandlerP;
   quint32            ulUsartRcvBufferSizeP;
   quint32            ulUsartTrmBufferSizeP;
   quint8             ubUsartDirectionP;

private slots:
   void onError(QSerialPort::SerialPortError error);
   void onReadyRead(void);
   void onBytesWritten(qint64 sqByteCountV);

public:
   ~QCanUsart();

   /*!
    * \brief currentConfig
    * \return current configuration of device
    */
   QCanUsartConfig_ts & currentConfig();
   /*!
    * \brief setConfig
    * \return process state of new configuration
    */
   bool setConfig(QCanUsartConfig_ts &);

   /*!
    * \brief Provide name of Serial Interface for initial
    *        initialisation
    * \attention This Function have to be called before
    */
   void setDeviceName(QString);

   /*!
    * \brief isAvailable
    * \return true if interface acces is available and false if not
    */
   bool isAvailable (void);

   /*!
    * \brief formatedError
    * \param tvErrorV - Error code provided by this module.
    * \return Formated error string that is coded as 'tvErrorV'.
    */
   QString formatedError(CpStatus_tv tvErrorV);


   /*!
    * \brief Allow only one instance of PCAN Basic
    */
   static QCanUsart & getInstance()
   {
      static QCanUsart clInstanceS;
      return clInstanceS;
   }

   //----------------------------------------------------------------
   // this functions will be called from cp_usart module
   // -> Realize the transition from C to C++
   qint32 DevInit(qint32 slBaudV, quint8 ubModeV);
   qint32 DevRelease(void);
   qint32 DevSetDir(quint8 ubDirV);
   qint32 DevSetRcvBufferSize(quint32 ulSizeV);
   qint32 DevSetRcvHandler(UsartHandler_fn pfnHandler);
   qint32 DevSetTrmHandler(UsartHandler_fn pfnHandler);
   qint32 DevWrite(quint8 *pubDataV, quint32 ulSizeV);

   //----------------------------------------------------------------
   // this function will be called from plugin module and
   // and provide access to the C implementation
   // -> Realize the transition from C++ to C
   //
   CpStatus_tv CpUsartBitrate(CpPort_ts *ptsPortV, int32_t slNomBitRateV,
                              int32_t slDatBitRateV)
   {
      return CpCoreBitrate(ptsPortV, slNomBitRateV, slDatBitRateV);
   }

   CpStatus_tv CpUsartBufferConfig(CpPort_ts *ptsPortV, uint8_t   ubBufferIdxV,
                                   uint32_t  ulIdentifierV, uint32_t  ulAcceptMaskV,
                                   uint8_t   ubFormatV, uint8_t   ubDirectionV)
   {
      return CpCoreBufferConfig(ptsPortV, ubBufferIdxV, ulIdentifierV,
                                ulAcceptMaskV, ubFormatV, ubDirectionV);
   }

   CpStatus_tv CpUsartBufferEnable(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV,
                                   uint8_t ubEnableV)
   {
      return CpCoreBufferEnable(ptsPortV, ubBufferIdxV, ubEnableV);
   }

   CpStatus_tv CpUsartBufferGetData(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV,
                                    uint8_t   *pubDestDataV, uint8_t ubStartPosV,
                                    uint8_t   ubSizeV)
   {
      return CpCoreBufferGetData(ptsPortV, ubBufferIdxV, pubDestDataV, ubStartPosV, ubSizeV);
   }

   CpStatus_tv CpUsartBufferGetDlc(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV, uint8_t *pubDlcV)
   {
      return CpCoreBufferGetDlc(ptsPortV, ubBufferIdxV, pubDlcV);
   }

   CpStatus_tv CpUsartBufferRelease(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV)
   {
      return CpCoreBufferRelease(ptsPortV, ubBufferIdxV);
   }

   CpStatus_tv CpUsartBufferSend(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV)
   {
      return CpCoreBufferSend(ptsPortV, ubBufferIdxV);
   }

   CpStatus_tv CpUsartBufferSetData(CpPort_ts *ptsPortV,
                                    uint8_t ubBufferIdxV,
                                    uint8_t *pubSrcDataV,
                                    uint8_t ubStartPosV,
                                    uint8_t   ubSizeV)
   {
      return CpCoreBufferSetData(ptsPortV, ubBufferIdxV, pubSrcDataV, ubStartPosV, ubSizeV);
   }

   CpStatus_tv CpUsartBufferSetDlc(CpPort_ts *ptsPortV,
                                   uint8_t ubBufferIdxV,
                                   uint8_t ubDlcV)
   {
      return CpCoreBufferSetDlc(ptsPortV, ubBufferIdxV, ubDlcV);
   }

   CpStatus_tv CpUsartCanMode(CpPort_ts *ptsPortV, uint8_t ubModeV)
   {
      return CpCoreCanMode(ptsPortV, ubModeV);
   }

   CpStatus_tv CpUsartCanState(CpPort_ts *ptsPortV, CpState_ts *ptsStateV)
   {
      return CpCoreCanState(ptsPortV, ptsStateV);
   }

   CpStatus_tv CpUsartDriverInit(uint8_t ubPhyIfV, CpPort_ts *ptsPortV,
                                uint8_t ubConfigV)
   {
      return CpCoreDriverInit(ubPhyIfV, ptsPortV, ubConfigV);
   }

   CpStatus_tv CpUsartDriverRelease(CpPort_ts *ptsPortV)
   {
      return CpCoreDriverRelease(ptsPortV);
   }

   CpStatus_tv CpUsartFifoConfig(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV,
                                CpFifo_ts *ptsFifoV)
   {
      return CpCoreFifoConfig(ptsPortV,ubBufferIdxV,ptsFifoV);
   }


   void        CpUsartFifoEvent(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV)
   {
      CpCoreFifoEvent(ptsPortV, ubBufferIdxV);
   }

   CpStatus_tv CpUsartFifoRead(CpPort_ts *ptsPortV,
                               uint8_t ubBufferIdxV,
                               CpCanMsg_ts *ptsCanMsgV,
                               uint32_t *pulMsgCntV)
   {
      return CpCoreFifoRead(ptsPortV, ubBufferIdxV, ptsCanMsgV, pulMsgCntV);
   }

   CpStatus_tv CpUsartFifoRelease(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV)
   {
      return CpCoreFifoRelease(ptsPortV, ubBufferIdxV);
   }

   CpStatus_tv CpUsartFifoWrite(CpPort_ts *ptsPortV,
                                uint8_t ubBufferIdxV,
                                CpCanMsg_ts *ptsCanMsgV,
                                uint32_t *pulMsgCntV)
   {
      return CpCoreFifoWrite(ptsPortV, ubBufferIdxV, ptsCanMsgV, pulMsgCntV);
   }

   CpStatus_tv CpUsartHDI(CpPort_ts *ptsPortV, CpHdi_ts *ptsHdiV)
   {
      return CpCoreHDI(ptsPortV, ptsHdiV);
   }

   CpStatus_tv CpUsartIntFunctions(CpPort_ts *ptsPortV,
                                   CpRcvHandler_Fn pfnRcvHandlerV,
                                   CpTrmHandler_Fn pfnTrmHandlerV,
                                   CpErrHandler_Fn pfnErrHandlerV)
   {
      return CpCoreIntFunctions(ptsPortV, pfnRcvHandlerV, pfnTrmHandlerV, pfnErrHandlerV);
   }

   CpStatus_tv CpUsartStatistic(CpPort_ts *ptsPortV, CpStatistic_ts *ptsStatsV)
   {
       return CpCoreStatistic(ptsPortV, ptsStatsV);
   }
};

#endif /*QCAN_USART_H_*/
