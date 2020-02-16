//====================================================================================================================//
// File:          node_canframe.cpp                                                                                   //
// Description:   Node.js binding for QCanFrame class                                                                 //
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



/*--------------------------------------------------------------------------------------------------------------------*\
** Static variables                                                                                                   **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

Napi::FunctionReference CanFilter::clConstructorP;

/*--------------------------------------------------------------------------------------------------------------------*\
** Node.js binding                                                                                                    **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/






//--------------------------------------------------------------------------------------------------------------------//
// CanFilter::CanFilter()                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CanFilter::CanFilter(const Napi::CallbackInfo& clNapiInfoR) : Napi::ObjectWrap<CanFilter>(clNapiInfoR)
{
   Napi::Env            clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope    clScopeT(clNapiEnvT);
   int32_t              slParamCountT;

   //---------------------------------------------------------------------------------------------------
   // Extract number of parameters supplied
   //
   slParamCountT = clNapiInfoR.Length();


}

CanFilter::~CanFilter()
{

}


//--------------------------------------------------------------------------------------------------------------------//
// CanFilter::acceptFrame()                                                                                           //
// return identifier value                                                                                            //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFilter::acceptFrame(const Napi::CallbackInfo& clNapiInfoR)
{
   Napi::Env          clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope  clScopeT(clNapiEnvT);
   int32_t            slParamCountT;
   uint32_t           ulFrameTypeT = 0;
   uint32_t           ulIdentifierLowT = 0;
   uint32_t           ulIdentifierHighT = 0;


   //---------------------------------------------------------------------------------------------------
   // check number of parameter
   //
   slParamCountT = clNapiInfoR.Length();
   if (slParamCountT != 3)
   {
      Napi::TypeError::New(clNapiEnvT, "acceptFrame() requires three parameter.").ThrowAsJavaScriptException();
   }

   //---------------------------------------------------------------------------------------------------
   // first parameter must be a number, frame type
   //
   if (clNapiInfoR[0].IsNumber())
   {
      Napi::Number clNapiFrameTypeT = clNapiInfoR[0].As<Napi::Number>();
      ulFrameTypeT = clNapiFrameTypeT.Uint32Value();
   }
   else
   {
      Napi::TypeError::New(clNapiEnvT, "acceptFrame() requires a number as parameter 1.").ThrowAsJavaScriptException();
   }

   //---------------------------------------------------------------------------------------------------
   // second parameter must be a number, identifier low
   //
   if (clNapiInfoR[1].IsNumber())
   {
      Napi::Number clNapiIdentifierLowT = clNapiInfoR[1].As<Napi::Number>();
      ulIdentifierLowT = clNapiIdentifierLowT.Uint32Value();
   }
   else
   {
      Napi::TypeError::New(clNapiEnvT, "acceptFrame() requires a number as parameter 2.").ThrowAsJavaScriptException();
   }

   //---------------------------------------------------------------------------------------------------
   // third parameter must be a number, identifier high
   //
   if (clNapiInfoR[2].IsNumber())
   {
      Napi::Number clNapiIdentifierHighT = clNapiInfoR[2].As<Napi::Number>();
      ulIdentifierHighT = clNapiIdentifierHighT.Uint32Value();
   }
   else
   {
      Napi::TypeError::New(clNapiEnvT, "acceptFrame() requires a number as parameter 3.").ThrowAsJavaScriptException();
   }

   return Napi::Number::From(clNapiEnvT, clCanFilterP.acceptFrame( (QCanFrame::FrameFormat_e) ulFrameTypeT,
                                                                   ulIdentifierLowT, ulIdentifierHighT)    );
}


//--------------------------------------------------------------------------------------------------------------------//
// CanFilter::rejectFrame()                                                                                           //
// set payload for this object                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFilter::rejectFrame(const Napi::CallbackInfo& clNapiInfoR)
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
Napi::Object CanFilter::init(Napi::Env clNapiEnvV, Napi::Object clNapiExportsV)
{
  Napi::HandleScope  clNapiScopeT(clNapiEnvV);

  Napi::Function     clNapiFuntionT;


  clNapiFuntionT = DefineClass(clNapiEnvV, "CanFilter",
                               { InstanceMethod("acceptFrame"     , &CanFilter::acceptFrame),
                                 InstanceMethod("rejectFrame"     , &CanFilter::rejectFrame) }
                               );

  clConstructorP = Napi::Persistent(clNapiFuntionT);
  clConstructorP.SuppressDestruct();


  clNapiExportsV.Set("CanFilter", clNapiFuntionT);

  return clNapiExportsV;
}
