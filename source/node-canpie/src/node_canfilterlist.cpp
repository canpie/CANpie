//====================================================================================================================//
// File:          node_canfilterlist.cpp                                                                              //
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




/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include "node_canfilterlist.hpp"



/*--------------------------------------------------------------------------------------------------------------------*\
** Static variables                                                                                                   **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

Napi::FunctionReference CanFilterList::clConstructorP;

/*--------------------------------------------------------------------------------------------------------------------*\
** Node.js binding                                                                                                    **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/






//--------------------------------------------------------------------------------------------------------------------//
// CanFilterList::CanFilterList()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CanFilterList::CanFilterList(const Napi::CallbackInfo& clNapiInfoR) : Napi::ObjectWrap<CanFilterList>(clNapiInfoR)
{
   Napi::Env            clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope    clScopeT(clNapiEnvT);
   int32_t              slParamCountT;

   //---------------------------------------------------------------------------------------------------
   // Extract number of parameters supplied
   //
   slParamCountT = clNapiInfoR.Length();


}

CanFilterList::~CanFilterList()
{

}


//--------------------------------------------------------------------------------------------------------------------//
// CanFilterList::identifier()                                                                                             //
// return identifier value                                                                                            //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFilterList::appendFilter(const Napi::CallbackInfo& clNapiInfoR)
{
  Napi::Env          clNapiEnvT = clNapiInfoR.Env();
  Napi::HandleScope  clScopeT(clNapiEnvT);


  //return Napi::Number::From(clNapiEnvT, clCanFilterM.identifier());
  return Napi::Boolean::New(clNapiEnvT, true);

}


//--------------------------------------------------------------------------------------------------------------------//
// CanFilter::setData()                                                                                                //
// set payload for this object                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFilterList::clear(const Napi::CallbackInfo& clNapiInfoR)
{
  Napi::Env          clNapiEnvT = clNapiInfoR.Env();
  Napi::HandleScope  clScopeT(clNapiEnvT);
  int32_t            slParamCountT;


  //---------------------------------------------------------------------------------------------------
  // check number of parameter
  //
  slParamCountT = clNapiInfoR.Length();
  if (slParamCountT != 1)
  {
     Napi::TypeError::New(clNapiEnvT, "setData() requires a Uint8ClampedArray as parameter.").ThrowAsJavaScriptException();
  }

  return Napi::Boolean::New(clNapiEnvT, true);
}


//--------------------------------------------------------------------------------------------------------------------//
// CanFilter::init()                                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Object CanFilterList::init(Napi::Env clNapiEnvV, Napi::Object clNapiExportsV)
{
  Napi::HandleScope  clNapiScopeT(clNapiEnvV);

  Napi::Function     clNapiFuntionT;


  clNapiFuntionT = DefineClass(clNapiEnvV, "CanFilterList",
                               { InstanceMethod("appendFilter"    , &CanFilterList::appendFilter),
                                 InstanceMethod("clear"           , &CanFilterList::clear) }
                               );

  clConstructorP = Napi::Persistent(clNapiFuntionT);
  clConstructorP.SuppressDestruct();


  clNapiExportsV.Set("CanFilterList", clNapiFuntionT);

  return clNapiExportsV;
}
