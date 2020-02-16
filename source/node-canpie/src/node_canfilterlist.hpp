//====================================================================================================================//
// File:          node_canfilterlist.hpp                                                                              //
// Description:   Node.js binding for QCanFilterList class                                                            //
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

//#define NAPI_VERSION    3

/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


#include <napi.h>

#include "qcan_filter_list.hpp"

class CanFilterList : public Napi::ObjectWrap<CanFilterList>
{
 public:

   //Constructor to initialise
   CanFilterList(const Napi::CallbackInfo & clNapiInfoR);
   ~CanFilterList();

   //Init function for setting the export key to JS
   static Napi::Object  init(Napi::Env clNapiEnvV, Napi::Object clNapiExportsV);

private:
   Napi::Value          appendFilter(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          clear(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          isEmpty(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          removeFilter(const Napi::CallbackInfo& clNapiInfoR);

   //---------------------------------------------------------------------------------------------------
   // internal instance of QCanFilterList class used to perform actual operations
   //
   QCanFilterList                   clCanFilterListP;

   //---------------------------------------------------------------------------------------------------
   // reference to store the class definition that needs to be exported to JS
   //
   static Napi::FunctionReference   clConstructorP;

};
