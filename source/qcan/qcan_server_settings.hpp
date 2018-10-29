//====================================================================================================================//
// File:          qcan_server_settings.hpp                                                                            //
// Description:   QCAN classes - CAN server settings                                                                  //
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


#ifndef QCAN_SERVER_SETTINGS_HPP_
#define QCAN_SERVER_SETTINGS_HPP_


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QSharedMemory>

#include "qcan_namespace.hpp"

using namespace QCan;

//----------------------------------------------------------------------------------------------------------------
/*!
** \class QCanServerSettings
**
** The QCanServerSettings provides the functionality to test the current state of a local QCanServer. For
** access to parameters of a single CAN network please refer to QCanNetworkSettings.
** <p>
** The following code shows an example how to test for a local QCanServer:
** \code
** QCanServerSettings   clServerSettingsT;
** if (clServerSettingsT.state() < QCanServerSettings::eSTATE_ACTIVE)
** {
**    fprintf(stdout, "CANpie FD server %s \n", qPrintable(clServerSettingsT.stateString()));
**    exit(0);
** }
**
** \endcode
*/
class QCanServerSettings
{

public:
   //---------------------------------------------------------------------------------------------------
   /*!
   ** Construct a QCanServerSettings object
   */
   QCanServerSettings();

   ~QCanServerSettings();

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \enum    State_e
   **
   ** This enumeration describes possible states of the server.
   */
   enum State_e {
      /*! Server is crashed                           */
      eSTATE_CRASHED = -2,

      /*! Server is in unknown state.                 */
      eSTATE_UNKNOWN = -1,

      /*! Server is not running.                      */
      eSTATE_INACTIVE = 0,

      /*! Server is active                            */
      eSTATE_ACTIVE   = 1
   };


   //---------------------------------------------------------------------------------------------------
   /*!
   **
   ** \return  Number of CAN networks
   **
   ** Return the number of networks (i.e. QCanNetwork) managed by the QCanServer class.
   */
   int32_t        networkCount(void);

   bool           startServer(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  State of local QCanServer
   ** \see     stateString()
   **
   ** Return the state of a local QCanServer, possible values are listed by QCanServerSettings::State_e.
   */
   State_e        state(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  State of local QCanServer
   ** \see     state()
   **
   ** Return the state of a local QCanServer as a QString object.
   */
   QString        stateString(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Build version
   **
   ** Return the build version of the active QCanServer.
   */
   int32_t        versionBuild(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Major version
   **
   ** Return the major version of the active QCanServer.
   */
   int32_t        versionMajor(void);

   //---------------------------------------------------------------------------------------------------
   /*!
   ** \return  Minor version
   **
   ** Return the minor version of the active QCanServer.
   */
   int32_t        versionMinor(void);

private:
   QSharedMemory *   pclSettingsP;
};

#endif // QCAN_SERVER_SETTINGS_HPP_
