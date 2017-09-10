//============================================================================//
// File:          qcan_network.hpp                                            //
// Description:   QCAN classes - CAN network                                  //
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


#ifndef QCAN_SERVER_HPP_
#define QCAN_SERVER_HPP_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include <QObject>

#include "qcan_network.hpp"


//-----------------------------------------------------------------------------
/*!
** \class QCanServer
** \brief CAN server
**
** This class represents a CAN server, which incorporates up to
** QCAN_NETWORK_MAX number of CAN networks (QCanNetwork).
**
*/
class QCanServer : public QObject
{
    Q_OBJECT

public:
    QCanServer( QObject * pclParentV = Q_NULLPTR,
                uint16_t  uwPortStartV = QCAN_TCP_DEFAULT_PORT,
                uint8_t   ubNetworkNumV = QCAN_NETWORK_MAX);

    ~QCanServer();


    uint32_t      dispatcherTime(void)    { return (ulDispatchTimeP);   };

    QCanNetwork * network(uint8_t ubNetworkIdxV);

    uint8_t       maximumNetwork(void) const;

    void          setDispatcherTime(uint32_t ulTimeV);

    QHostAddress  serverAddress(void)     { return (clServerAddressP);  };

    void          setServerAddress(QHostAddress clHostAddressV);

private:

    QVector<QCanNetwork *> *  pclListNetsP;
    QHostAddress              clServerAddressP;
    uint32_t                  ulDispatchTimeP;
};

#endif // QCAN_SERVER_HPP_
