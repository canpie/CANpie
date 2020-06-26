//====================================================================================================================//
// File:          qcan_server_logger.hpp                                                                              //
// Description:   QCAN server - logging class                                                                         //
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


#ifndef QCAN_SERVER_LOGGER_HPP_
#define QCAN_SERVER_LOGGER_HPP_


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QObject>

#include "qcan_defs.hpp"
#include "qcan_namespace.hpp"
#include "qcan_server.hpp"


using namespace QCan;

//----------------------------------------------------------------------------------------------------------------
/*!
** \class QCanServerLogger
**
** This class add a logging capability for a QCanServer.
** <p>
** \todo - Description of file names, default log level
**
*/
class QCanServerLogger : public QObject
{
   Q_OBJECT

public:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  pclServerV      - Pointer to QCanServer class
   **
   ** Create new QCanServerLogger object. The parameter \a pclServerV defines the QCanServer which
   ** shall be attached for logging.
   */
   QCanServerLogger(QCanServer * pclServerV = Q_NULLPTR);

   ~QCanServerLogger();

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  pclServerV      - Pointer to QCanServer class
   **
   ** The function attaches a QCanServer instance for logging.
   */
   void           attachServer(QCanServer * pclServerV);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelV      - CAN channel
   ** \return     Current log level
   ** \see        setLogLevel()
   **
   ** The function changes the default file path and file name for the log attached to the CAN
   ** channel defined by the parameter \a teChannelV.
   */
   LogLevel_e     logLevel(const CAN_Channel_e teChannelV);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return     Generated log message
   ** \see        appendMessage()
   **
   ** The function returns the log message generated by appendMessage(), including the time-stamp.
   */
   inline QString message(void) const { return clLogMessageP; };


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelV      - CAN channel
   ** \param[in]  clFileNameV     - Log file name
   ** \return     \c true if file could be opened, otherwise \c false
   **
   ** The function changes the default file path and file name for the log attached to the CAN
   ** channel defined by the parameter \a teChannelV.
   */
   bool           setFileName(const CAN_Channel_e teChannelV, QString clFileNameV);


   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelV      - CAN channel
   ** \param[in]  teLogLevelV     - Log level
   ** \see        logLevel()
   **
   ** The function changes the default log level for the CAN channel defined by the
   ** parameter \a teChannelV. The new log level will be appended to the log file.
   */
   void           setLogLevel(const CAN_Channel_e teChannelV, LogLevel_e teLogLevelV);


public slots:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  teChannelV      - CAN channel
   ** \param[in]  clLogMessageV   - Log message
   ** \param[in]  teLogLevelV     - Log level
   **
   ** The function function appends a log message to the log file. The message will be prepended
   ** with a time-stamp. The parameter \c teLogLevelV defines the log level of the message.
   ** Only messages with a log level smaller or equal to the actual defined log level will be appended
   ** (see setLogLevel()).
   */
   virtual void   appendMessage(const CAN_Channel_e teChannelV, const QString & clLogMessageV,
                               LogLevel_e teLogLevelV = eLOG_LEVEL_INFO);


private:

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \param[in]  pclSenderV      - Pointer to class derived from QObject
   **
   ** The function registers a class for logging: the signal \c addLogMessage() is connect to the slot
   ** appendMessage() of this class.
   **
   */
   void           addLoggingSource(QObject * pclSenderV);


   QDateTime      clTimeP;
   QString        clLogMessageP;
   QFile *        apclLogFileP[QCAN_NETWORK_MAX];
   LogLevel_e     ateLogLevelP[QCAN_NETWORK_MAX];
};

#endif // QCAN_SERVER_LOGGER_HPP_
