//====================================================================================================================//
// File:          qcan_interface.hpp                                                                                  //
// Description:   QCAN classes - CAN interface class                                                                  //
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


#ifndef QCAN_INTERFACE_HPP_
#define QCAN_INTERFACE_HPP_

/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


#include <stdint.h>
#include "qcan_defs.hpp"
#include "qcan_frame.hpp"


//----------------------------------------------------------------------------------------------------------------
/*!
** \class   QCanInterface
** \brief   CAN interface
** 
** The QCanInterface class describes one physical CAN interface. The CAN interface is typically implemented
** inside a CAN plug-in (see QCanPlugin). After connection to the CAN interface (see connect()) the use can
** read and write CAN frames via this hardware.
** <p>
** If the QCanInterface class is used for console applications, (i.e. no Qt GUI support), the definition
** QCAN_NO_QT_GUI must be set.
*/
class QCanInterface : public QObject
{

public:

    typedef struct QCanStatistic_s {
       uint32_t   ulRcvCount;
       uint32_t   ulTrmCount;
       uint32_t   ulErrCount;
    } QCanStatistic_ts;



   /*!
   ** \enum    InterfaceError_e
   **
   ** This enumeration describes possible error conditions.
   */
   enum InterfaceError_e {
      eERROR_CHANNEL = -4,

      /*! Access to library / plug-in failed          */
      eERROR_LIBRARY = -3,

      /*! CAN interface error                         */
      eERROR_DEVICE   = -2,

      /*! An unknown error occurred.                  */
      eERROR_UNKNOWN = -1,

      /*! No error occurred.                          */
      eERROR_NONE,



      /*! Bit-rate not supported                      */
      eERROR_BITRATE,

      eERROR_MODE,

      /*! Interface is already used, and can't
       *  be occupied                                 */
      eERROR_USED,

      /*! No message in receive FIFO available        */
      eERROR_FIFO_RCV_EMPTY,

      /*! Transmit FIFO is full                       */
      eERROR_FIFO_TRM_FULL
   };

   /*!
   ** \enum    ConnectionState_e
   **
   ** This enumeration describes the state of a CAN interface connection.
   */
   enum ConnectionState_e {

      /*! CAN interface is not connected.                      */
      UnconnectedState = 0,

      /*! Driver is attempting to connect the CAN interface.   */
      ConnectingState,

      /*! CAN interface is connected.                          */
      ConnectedState,

      /*! CAN interface is connected and will be closed.       */
      ClosingState,

      /*! CAN interface has a failure.                         */
      FailureState
   };
   
   QCanInterface()   {  }
   QCanInterface(const QCanInterface&) = delete;               // no copy constructor
   QCanInterface& operator=(const QCanInterface&) = delete;    // no assignment operator
   QCanInterface(QCanInterface&&) = delete;                    // no move constructor
   QCanInterface& operator=(QCanInterface&&) = delete;         // no move operator

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  true if the interface settings have been changed
   **
   ** This method is used to initiate a device specific configuration which is out of scope of this
   ** interface class. A return value of \a true means the interface settings have been changed.
   */
   inline virtual bool        configureDevice(void)   { return (false); }


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Status code defined by InterfaceError_e
   ** \see        disconnect()
   **
   ** Connect to the physical CAN interface.
   */
   virtual InterfaceError_e   connect(void) = 0;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     \c true if connection is possible
   ** \see connected()
   **
   ** The function returns \c true if the interface is connected.
   */
   virtual ConnectionState_e  connectionState(void) = 0;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Status code defined by InterfaceError_e
   ** \see        connect()
   **
   ** Disconnect from the physical CAN interface.
   */
   virtual InterfaceError_e   disconnect(void) = 0;


   virtual void               disableFeatures(uint32_t ulFeatureMaskV) = 0;

   virtual void               enableFeatures(uint32_t ulFeatureMaskV) = 0;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Icon
   **
   ** The function returns an icon for the physical CAN interface. In case an application is compiled
   ** without Qt GUI support, the definition QCAN_NO_QT_GUI must be set.
   */
   #ifndef QCAN_NO_QT_GUI
   virtual QIcon icon(void) = 0;
   #endif


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Name
   **
   ** The function returns a name for the physical CAN interface.
   */
   virtual QString name(void) = 0;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[out] clFrameR CAN frame
   ** \return     Status code defined by InterfaceError_e
   ** \see        write()
   **
   ** The functions reads a CAN message (data or error frame) from the CAN interface. The data is
   ** copied to the QByteArray \a clDataR. If no message is available, the function will return the
   ** value eERROR_FIFO_RCV_EMPTY. On success the function returns eERROR_NONE.
   **
   */
   virtual InterfaceError_e   read(QCanFrame &clFrameR) = 0;

   
   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Status code defined by InterfaceError_e
   ** \see        reset()
   **
   ** The functions resets a CAN a physical CAN interface. All statistic information will be reset also.
   ** On success the function returns eERROR_NONE.
   */
   virtual InterfaceError_e   reset(void) = 0;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  slNomBitRateV  Nominal Bit-rate value
   ** \param[in]  slDatBitRateV  Data Bit-rate value
   ** \return     Status code defined by InterfaceError_e
   **
   **	This function sets the bit-rate of the CAN interface. If the physical CAN interface does not
   **	support CAN FD, the parameter \c slDatBitRateV is not evaluated.
   */
   virtual InterfaceError_e   setBitrate(int32_t slNomBitRateV, int32_t slDatBitRateV = QCan::eCAN_BITRATE_NONE) = 0;


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teModeV CAN mode 
   ** \return     Status code defined by InterfaceError_e
   **
   ** Set mode of the physical CAN interface.
   */
   virtual InterfaceError_e   setMode(const QCan::CAN_Mode_e teModeV) = 0;

   virtual QCan::CAN_State_e  state(void) = 0;

   virtual InterfaceError_e   statistic(QCanStatistic_ts &clStatisticR) = 0;

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Bit-mask defined by \ref QCAN_IF
   ** \sa         disableFeatures() , enableFeatures()
   **
   ** The functions returns information about the supported features of a CAN interface. Possible
   ** features are CAN FD support, Listen-Only support or detection of Error Frames. The return value
   ** is a bit-mask using values defined inside the header file qcan_defs.hpp.
   **
   */
   virtual uint32_t           supportedFeatures(void) = 0;

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Version of the CAN interface
   **
   ** The function returns version information of the physical CAN interface.
   **
   */
   virtual QString            version(void) { return QString("No version information available"); }


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clFrameR CAN frame
   ** \return     Status code defined by InterfaceError_e
   ** \see        read()
   **
   ** The functions writes a CAN message (data frame) to the CAN interface. The CAN frame data is
   ** defined by the parameter \a clFrameR. On success the function returns eERROR_NONE.
   **
   */
   virtual InterfaceError_e   write(const QCanFrame &clFrameR) = 0;


Q_SIGNALS:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  clMessageR  - Logging message
   ** \param[in]  teLogLevelR - Logging level
   **
   ** This signal is emitted by the CAN interface to inform the application about status changes or
   ** error conditions.
   **
   */
   void addLogMessage(const QString & clMessageR, const QCan::LogLevel_e & teLogLevelR = QCan::eLOG_LEVEL_WARN);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teConnectionStateR - CAN interface connection state
   **
   ** This signal is emitted once when the connection state of the interface changes.
   */
   void connectionChanged(const QCanInterface::ConnectionState_e & teConnectionStateR);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** This signal is emitted once every time new data is available for reading from the device's current
   ** read channel. It will only be emitted again once new data is available.
   */
   void readyRead(void);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teCanStateR - CAN state
   **
   ** This signal is emitted by the CAN interface to inform the application about state changes of
   ** the CAN controller.
   **
   */
   void stateChanged(const QCan::CAN_State_e & teCanStateR);

private:


};

#endif /*QCAN_INTERFACE_HPP_*/
