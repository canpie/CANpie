//====================================================================================================================//
// File:          node_init.cpp                                                                                       //
// Description:   Node.js binding for CANopen master library                                                          //
//                                                                                                                    //
// Copyright (C) MicroControl GmbH & Co. KG                                                                           //
// 53844 Troisdorf - Germany                                                                                          //
// www.microcontrol.net                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
// The copyright to the computer program(s) herein is the property of MicroControl GmbH & Co. KG, Germany. The        //
// program(s) may be used and/or copied only with the written permission of MicroControl GmbH & Co. KG or in          //
// accordance with the terms and conditions stipulated in the agreement/contract under which the program(s) have      //
// been supplied.                                                                                                     //
//                                                                                                                    //
//====================================================================================================================//



/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include "node_canfilter.hpp"
#include "node_canfilterlist.hpp"
#include "node_canframe.hpp"
#include "node_cansocket.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Static variables                                                                                                   **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*\
** Node.js binding                                                                                                    **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// InitAll()                                                                                                          //
// N-API initialisation                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
   exports = CanFilter::init(env, exports);
   exports = CanFilterList::init(env, exports);
   exports = CanFrame::init(env, exports);
   return CanSocket::init(env, exports);
 }


NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)
