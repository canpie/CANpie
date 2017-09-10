//============================================================================//
// File:          qcan_plugin.hpp                                             //
// Description:   Virtual CAN plugin class                                    //
//                                                                            //
// Copyright 2017 MicroControl GmbH & Co. KG                                  //
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
// Licensed under the Apache License, Version 2.0 (the "License");            //
// you may not use this file except in compliance with the License.           //
// You may obtain a copy of the License at                                    //
//                                                                            //
//    http://www.apache.org/licenses/LICENSE-2.0                              //
//                                                                            //
// Unless required by applicable law or agreed to in writing, software        //
// distributed under the License is distributed on an "AS IS" BASIS,          //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   //
// See the License for the specific language governing permissions and        //
// limitations under the License.                                             //
//============================================================================//




//-----------------------------------------------------------------------------
/*!
** \file    qcan_plugin.hpp
** \brief   CAN plugin class
**
*/

#ifndef QCAN_PLUGIN_HPP_
#define QCAN_PLUGIN_HPP_

//#include <stdint.h>
//#include "qcan_defs.hpp"
#include "qcan_interface.hpp"




//-----------------------------------------------------------------------------
/*!
** \class   QCanPlugin
** \brief   CAN plugin
** 
*/
class QCanPlugin : public QObject
{

public:
   virtual QIcon           icon(void) = 0;
   /*!
    * \brief interfaceCount should return number
    *        interfaces that are actually available.
    * \return Total count of interfaces
    */
   virtual uint8_t         interfaceCount(void) = 0;
   virtual QCanInterface * getInterface(uint8_t ubInterfaceV = 0) = 0;
   virtual QString         name(void) = 0;

private:

};

#define QCanPlugin_iid "net.microcontrol.Qt.qcan.QCanPlugin"
Q_DECLARE_INTERFACE(QCanPlugin, QCanPlugin_iid)


#endif /*QCAN_PLUGIN_HPP_*/
