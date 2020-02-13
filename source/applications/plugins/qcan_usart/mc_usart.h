//============================================================================//
// File:          mc_usart.h                                                  //
// Description:   Universal synchronous/Asynchronous Receiver Transmitter     //
//                driver definitions and structures                           //
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


#ifndef  MC_USART_H_
#define  MC_USART_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include "mc_compiler.h"


//-----------------------------------------------------------------------------
/*!
** \file    mc_usart.h
** \brief   MCL - USART support functions
**
** This header file includes macros and definitions to access the
** serial interface (USART) of a microcontroller.
**
*/


//-------------------------------------------------------------------//
// take precautions if compiled with C++ compiler                    //
#ifdef __cplusplus                                                   //
extern "C" {                                                         //
#endif                                                               //
//-------------------------------------------------------------------//

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
/*!
** \enum USART_DIR_e
*/
enum USART_DIR_e
{
   eUSART_DIR_NONE = 0,
   eUSART_DIR_RX,
   eUSART_DIR_TX,
   eUSART_DIR_RXTX,
   /*!
    * \enum eUSART_DIR_RS485
    * In that direction mode the USART driver will handle the
    * enabling / disabling of driver itself.
    * Delay times before and after transmission will be also
    * handle by this implementation.
   */
   eUSART_DIR_RS485
};

//------------------------------------------------------------------------------
/*!
** \enum USART_ERROR_e
*/
enum USART_ERROR_e
{
   eUSART_ERROR_OK = 0,//!< eUSART_ERROR_OK

   /*! port number is out of range                             */
   eUSART_ERROR_PORT,  //!< eUSART_ERROR_PORT

   /*! baud rate is out of range                               */
   eUSART_ERROR_BAUD,  //!< eUSART_ERROR_BAUD

   /*! mode is out of range                                    */
   eUSART_ERROR_MODE,  //!< eUSART_ERROR_MODE

   /*! USART port is busy                                       */
   eUSART_ERROR_BUSY,  //!< eUSART_ERROR_BUSY

   /*! the requested size can not be handled                   */
   eUSART_ERROR_SIZE   //!< eUSART_ERROR_SIZE

};

//------------------------------------------------------------------------------
/*!
** \enum USART_PORT_e
*/
enum USART_PORT_e
{
   eUSART_PORT_1 = 0,
   eUSART_PORT_2,
   eUSART_PORT_3,
   eUSART_PORT_4,
   eUSART_PORT_5,
   eUSART_PORT_6
};

//------------------------------------------------------------------------------
/*!
** \enum USART_MODE_e
*/
enum USART_MODE_e
{
   eUSART_MODE_7N1 = 1,
   eUSART_MODE_7E1,
   eUSART_MODE_7O1,
   eUSART_MODE_8N1,
   eUSART_MODE_8E1,
   eUSART_MODE_8O1
};


/*!
** \typedef    UsartHandler_fn
** \brief      USART callback function
**
*/
typedef void (*UsartHandler_fn)(uint8_t *pubDataV, uint32_t ulSizeV);


/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/
//------------------------------------------------------------------------------
/*!
** \param   ubPortV - Serial USART port number
** \return  Direction of actual pending transmission defined by #USART_DIR_e
*/
uint8_t McUsartGetDir(uint8_t ubPortV);

//------------------------------------------------------------------------------
/*!
** \brief   Get receive status
** \param   ubPortV - Serial USART port number
** \return  Error code or number of bytes in receive buffer
**
** \sa      McUsartGetTrmStatus()
**
** This function returns the receive status of the serial USART port
** with the index \a ubPortV. The number is taken from the enumeration
** #USART_PORT_e.
** <p>
** A return value of 0 (i.e. #eUSART_ERROR_OK) denotes that there is no
** reception in progress and no data is inside the receive buffer.
** A negative return value denotes an error. The error code is taken
** from the enumeration USART_ERROR_e.
** <p>
** A value greater than 0 specifies the number of bytes which are inside
** the receive buffer.
**
*/
int32_t McUsartGetRcvStatus(uint8_t ubPortV);

//------------------------------------------------------------------------------
/*!
**
** \param   ubPortV
** \return  Error code or timeout value
**
** A value greater than 0 specifies timeout value given in microsecond.
*/
int32_t McUsartGetTimeout(uint8_t ubPortV);

//------------------------------------------------------------------------------
/*!
** \brief   Get transmit status
** \param   ubPortV - Serial USART port number
** \return  Error code or number of bytes in transmit buffer
**
** \sa      McUsartGetRcvStatus()
**
** This function returns the transmit status of the serial USART port
** with the index \a ubPortV. The number is taken from the enumeration
** #USART_PORT_e.
** <p>
** A return value of 0 (i.e. #eUSART_ERROR_OK) denotes that there is no
** transmission in progress and a new transmission can be started by
** calling McUsartWrite(). A negative return value denotes an error.
** The error code is taken from the enumeration USART_ERROR_e. A value
** greater than 0 specifies the number of bytes which are still in the
** transmit buffer. In this case you can not call McUsartWrite(), as it
** will return with an error code of #eUSART_ERROR_BUSY.
**
*/
int32_t McUsartGetTrmStatus(uint8_t ubPortV);

//------------------------------------------------------------------------------
/*!
** \brief   Initialise Serial Interface
** \param   ubPortV - Serial USART port number
** \param   ulBaudV - Baud rate value
** \param   ubModeV - Mode of USART port
** \return  eUSART_ERROR_OK or an error value from #USART_ERROR_e
**
** \sa      McUsartRelease()
**
** This function initialises the serial USART port with the index
** \a ubPortV. The number is taken from the enumeration #USART_PORT_e.
** The initial baudrate is set via \a ulBaudV. The number of
** data bits, stop bits and the parity is configured via the
** parameter \a ubModeV.
**
** \attention After initialisation USART reception and sending are disabled.
**            Call #McUsartSetDir() to setup direction of transmission.
*/
int32_t McUsartInit(uint8_t ubPortV, uint32_t ulBaudV, uint8_t ubModeV);

//------------------------------------------------------------------------------
/*!
** \brief   Release Serial Interface
** \param   ubPortV - Serial USART port number
**
** \see  McUsartInit()
**
** This function releases the serial USART port with index
** \a ubPortV.
*/
int32_t McUsartRelease(uint8_t ubPortV);

//------------------------------------------------------------------------------
/*!
** \brief   Send one byte
** \param   ubPortV - Serial USART port number
** \param   ubDataV - Data to send
**
** This function sends one byte of data (\a ubDataV) to the serial USART
** port with index \a ubPortV. The data is not buffered in the driver,
** so subsequent calls are likely to fail, depending on the serial USART
** baudrate. The function directly accesses the hardware.
*/
int32_t McUsartSendByte(uint8_t ubPortV, uint8_t ubDataV);

//------------------------------------------------------------------------------
/*!
** \brief   Set transmission direction of USART
** \param   ubPortV - ubPortV - USART port number
** \param   ubDirV - Supported direction defined by #USART_DIR_e
** \return  eUSART_ERROR_OK or an error value from #USART_ERROR_e
*/
int32_t McUsartSetDir(uint8_t ubPortV, uint8_t ubDirV);

//------------------------------------------------------------------------------
/*!
** \brief   Setup buffer size for reception
** \param   ubPortV - ubPortV - USART port number
** \param   ulSizeV - Size given in byte
** \return  eUSART_ERROR_OK or an error value from #USART_ERROR_e
**
** After reception of given number of bytes the receive callback handler
** that was defined by #McUsartSetRcvHandler() will be called.
*/
int32_t McUsartSetRcvBufferSize(uint8_t ubPortV, uint32_t ulSizeV);

//------------------------------------------------------------------------------
/*!
** \brief   Define user callback handler, that is called at reception
** \param   ubPortV - USART port number
** \param   pfnHandler - pointer user callback function handler
** \return  eUSART_ERROR_OK or an error value from #USART_ERROR_e
**
** \attention  User callback function handler will be only called if
**             one of following condition was met:
**             - receive buffer size is full, set by #McUsartSetRcvBufferSize()
**             - receive timeout occurs, set by #McUsartSetTimeout()
*/
int32_t McUsartSetRcvHandler(uint8_t ubPortV, UsartHandler_fn pfnHandler);

//------------------------------------------------------------------------------
/*!
** \brief   Set Timeout for end of reception
** \param   ubPortV - Serial USART port number
** \param   ulTimeV - time value given in microseconds [µs]
** \return  eUSART_ERROR_OK or an error value from #USART_ERROR_e
**
** After reception of a data byte timeout timer will be restarted.
** If no further data bytes will be received than the timer runs into timeout
** and triggers function handler set by #McUsartSetRcvHandler().
**
** \attention ulTimeV value of 0 disables timeout monitoring!
*/
int32_t McUsartSetTimeout(uint8_t ubPortV, uint32_t ulTimeV);

//------------------------------------------------------------------------------
/*!
** \brief   Define user callback handler, that is called at reception
** \param   ubPortV - USART port number
** \param   pfnHandler - pointer user callback function handler
** \return  eUSART_ERROR_OK or an error value from #USART_ERROR_e
**
*/
int32_t McUsartSetTrmHandler(uint8_t ubPortV, UsartHandler_fn pfnHandler);

//------------------------------------------------------------------------------
/*!
** \brief   Write data on Serial Interface
** \param   ubPortV - Serial USART port number
** \param   pubBufferV - Pointer to data buffer
** \param   uwSizeV - Number of bytes to write
**
** This function writes data to the serial USART port with index
** \a ubPortV. The start address of the data to write is given
** by the parameter \a pubBufferV, whereas \a ulSizeV denotes
** the number of bytes to write.
**
** \attention This function is not blocking!
**            Setup a transmit callback handler to trigger transmission finish.
**
*/
int32_t McUsartWrite(uint8_t ubPortV, uint8_t *pubBufferV, uint32_t ulSizeV);


//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//


#endif   // #define  MC_USART_H_
