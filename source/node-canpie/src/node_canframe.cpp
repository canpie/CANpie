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

#include "node_canframe.hpp"



/*--------------------------------------------------------------------------------------------------------------------*\
** Static variables                                                                                                   **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

Napi::FunctionReference CanFrame::constructor;

/*--------------------------------------------------------------------------------------------------------------------*\
** Node.js binding                                                                                                    **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/






//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::CanFrame()                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CanFrame::CanFrame(const Napi::CallbackInfo& clNapiInfoR) : Napi::ObjectWrap<CanFrame>(clNapiInfoR)
{
   Napi::Env            clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope    clScopeT(clNapiEnvT);
   int32_t              slParamCountT;

   //---------------------------------------------------------------------------------------------------
   // Extract number of parameters supplied
   //
   slParamCountT = clNapiInfoR.Length();


   //---------------------------------------------------------------------------------------------------
   // first parameter: frame type
   //
   if (slParamCountT > 0)
   {
      Napi::Number clNapiFrameFormatT = clNapiInfoR[0].As<Napi::Number>();
      uint32_t ulFrameFormatT = clNapiFrameFormatT.Uint32Value();
      if (ulFrameFormatT <= QCanFrame::eFORMAT_FD_EXT)
      {
         clCanFrameM.setFrameFormat((QCanFrame::FrameFormat_e) ulFrameFormatT);
      }
   }

   //---------------------------------------------------------------------------------------------------
   // second parameter: identifier
   //
   if (slParamCountT > 1)
   {
      Napi::Number clNapiIdentifierT = clNapiInfoR[1].As<Napi::Number>();
      uint32_t ulIdentifierT = clNapiIdentifierT.Uint32Value();
      clCanFrameM.setIdentifier(ulIdentifierT);
   }

   //---------------------------------------------------------------------------------------------------
   // third parameter: DLC
   //
   if (slParamCountT > 2)
   {
      Napi::Number clNapiDlcT = clNapiInfoR[2].As<Napi::Number>();
      uint32_t ulDlcT = clNapiDlcT.Uint32Value();
      clCanFrameM.setDlc((uint8_t) ulDlcT);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::frameFormat()                                                                                            //
// return frame format value                                                                                          //
//--------------------------------------------------------------------------------------------------------------------//
CanFrame::~CanFrame()
{
   printf("CanFrame destructor \n");
}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::data()                                                                                                   //
// return data                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::data(const Napi::CallbackInfo& clNapiInfoR)
{
   Napi::Env          clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope  clScopeT(clNapiEnvT);
   Napi::Uint8Array   clNapiArrayT;

   //---------------------------------------------------------------------------------------------------
   // convert the data to a Uint8ClampedArray
   //
   size_t ulArraySizeT = clCanFrameM.dataSize();
   clNapiArrayT = Napi::Uint8Array::New(clNapiEnvT, ulArraySizeT, napi_uint8_clamped_array);

   uint8_t * pubDataT = clNapiArrayT.Data();
   for (uint8_t ubDataCntT = 0; ubDataCntT < (uint8_t ) ulArraySizeT; ubDataCntT++)
   {
      *pubDataT = clCanFrameM.data(ubDataCntT);
      pubDataT++;
   }

   return clNapiArrayT;
}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::dataSize()                                                                                               //
// return data size of payload                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::dataSize(const Napi::CallbackInfo& clNapiInfoR)
{
   Napi::Env          clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope  clScopeT(clNapiEnvT);

   //---------------------------------------------------------------------------------------------------
   // convert the data size value to an int32_t data type here
   //
   return Napi::Number::From(clNapiEnvT, (int32_t) clCanFrameM.dataSize());

}

//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::dataUInt16()                                                                                             //
// return uint16_t value from specific position inside payload                                                        //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::dataUInt16(const Napi::CallbackInfo& clNapiInfoR)
{
  Napi::Env          clNapiEnvT = clNapiInfoR.Env();
  Napi::HandleScope  clScopeT(clNapiEnvT);
  int32_t            ulParamCountT;
  uint32_t           ulPositionT = 0;
  bool               btMsbFirstT = false;

  //---------------------------------------------------------------------------------------------------
  // check number of parameter
  //
  ulParamCountT = clNapiInfoR.Length();
  if (ulParamCountT < 1)
  {
     Napi::Error::New(clNapiEnvT, "dataUInt16() requires at least one parameter.").ThrowAsJavaScriptException();
  }

  //---------------------------------------------------------------------------------------------------
  // first parameter: start position
  //
  if (ulParamCountT > 0)
  {
     Napi::Number clNapiPositionT = clNapiInfoR[0].As<Napi::Number>();
     ulPositionT = clNapiPositionT.Uint32Value();
  }

  //---------------------------------------------------------------------------------------------------
  // second parameter: MSB first (optional)
  //
  if (ulParamCountT > 1)
  {
     Napi::Boolean clNapiMsbFirstT = clNapiInfoR[1].As<Napi::Boolean>();
     btMsbFirstT = clNapiMsbFirstT.Value();
  }

  //---------------------------------------------------------------------------------------------------
  // convert the result to an int32_t data type here
  //
  return Napi::Number::From(clNapiEnvT, (int32_t) clCanFrameM.dataUInt16(ulPositionT, btMsbFirstT));

}

//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::dataUInt32()                                                                                             //
// return uint32_t value from specific position inside payload                                                        //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::dataUInt32(const Napi::CallbackInfo& clNapiInfoR)
{
  Napi::Env          clNapiEnvT = clNapiInfoR.Env();
  Napi::HandleScope  clScopeT(clNapiEnvT);
  int32_t            ulParamCountT;
  uint32_t           ulPositionT = 0;
  bool               btMsbFirstT = false;


  //---------------------------------------------------------------------------------------------------
  // check number of parameter
  //
  ulParamCountT = clNapiInfoR.Length();
  if (ulParamCountT < 1)
  {
     Napi::Error::New(clNapiEnvT, "dataUInt32() requires at least one parameter.").ThrowAsJavaScriptException();
  }

  //---------------------------------------------------------------------------------------------------
  // first parameter: start position
  //
  if (ulParamCountT > 0)
  {
     Napi::Number clNapiPositionT = clNapiInfoR[0].As<Napi::Number>();
     ulPositionT = clNapiPositionT.Uint32Value();
  }

  //---------------------------------------------------------------------------------------------------
  // second parameter: MSB first (optional)
  //
  if (ulParamCountT > 1)
  {
     Napi::Boolean clNapiMsbFirstT = clNapiInfoR[1].As<Napi::Boolean>();
     btMsbFirstT = clNapiMsbFirstT.Value();
  }

  //---------------------------------------------------------------------------------------------------
  // convert the result to an int32_t data type here
  //
  return Napi::Number::From(clNapiEnvT, (int32_t) clCanFrameM.dataUInt32(ulPositionT, btMsbFirstT));

}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::dlc()                                                                                                    //
// return DLC value                                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::dlc(const Napi::CallbackInfo& clNapiInfoR)
{
   Napi::Env          clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope  clScopeT(clNapiEnvT);

   //---------------------------------------------------------------------------------------------------
   // convert the DLC value to an int32_t data type here
   //
   return Napi::Number::From(clNapiEnvT, (int32_t) clCanFrameM.dlc());
}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::errorStateIndicator()                                                                                    //
// return value of error state indicator                                                                              //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::errorStateIndicator(const Napi::CallbackInfo& clNapiInfoR)
{
  Napi::Env          clNapiEnvT = clNapiInfoR.Env();
  Napi::HandleScope  clScopeT(clNapiEnvT);

  //---------------------------------------------------------------------------------------------------
  // convert the ESI value to a bool data type here
  //
  return Napi::Boolean::From(clNapiEnvT, clCanFrameM.errorStateIndicator());
}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::frameFormat()                                                                                            //
// return frame format value                                                                                          //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::frameFormat(const Napi::CallbackInfo& clNapiInfoR)
{
   Napi::Env          clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope  clScopeT(clNapiEnvT);

   //---------------------------------------------------------------------------------------------------
   // convert the enum value of FrameFormat_e to an int32_t data type here
   //
   return Napi::Number::From(clNapiEnvT, (int32_t) clCanFrameM.frameFormat());
}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::identifier()                                                                                             //
// return identifier value                                                                                            //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::identifier(const Napi::CallbackInfo& clNapiInfoR)
{
  Napi::Env          clNapiEnvT = clNapiInfoR.Env();
  Napi::HandleScope  clScopeT(clNapiEnvT);


  return Napi::Number::From(clNapiEnvT, clCanFrameM.identifier());
}

Napi::Value CanFrame::setBitrateSwitch(const Napi::CallbackInfo& clNapiInfoR)
{
   Napi::Env          clNapiEnvT = clNapiInfoR.Env();
   return Napi::Number::From(clNapiEnvT, 0);
}

//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::setData()                                                                                                //
// set payload for this object                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::setData(const Napi::CallbackInfo& clNapiInfoR)
{
  Napi::Env          clNapiEnvT = clNapiInfoR.Env();
  Napi::HandleScope  clScopeT(clNapiEnvT);
  int32_t            ulParamCountT;


  //---------------------------------------------------------------------------------------------------
  // check number of parameter
  //
  ulParamCountT = clNapiInfoR.Length();
  if (ulParamCountT != 1)
  {
     Napi::Error::New(clNapiEnvT, "setData() requires one parameter.").ThrowAsJavaScriptException();
  }

  //---------------------------------------------------------------------------------------------------
  // first parameter must be an object
  //
  if (clNapiInfoR[0].IsTypedArray())
  {
     Napi::TypedArrayOf<uint8_t> clNapiArrayT = clNapiInfoR[0].As<Napi::TypedArrayOf<uint8_t>>();
     uint32_t ulArraySizeT = clNapiArrayT.ByteLength();
     if (ulArraySizeT > 64)
     {
        ulArraySizeT = 64;
     }

     uint8_t * pubDataT = clNapiArrayT.Data();
     for (uint8_t ubDataCntT = 0; ubDataCntT < (uint8_t ) ulArraySizeT; ubDataCntT++)
     {
        clCanFrameM.setData(ubDataCntT, *pubDataT);
        pubDataT++;
     }
  }
  else
  {
     Napi::TypeError::New(clNapiEnvT, "setData() requires a Uint8ClampedArray as parameter.").ThrowAsJavaScriptException();
  }

  return Napi::Boolean::New(clNapiEnvT, true);
}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::setDataSize()                                                                                            //
// set data size                                                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::setDataSize(const Napi::CallbackInfo& clNapiInfoR)
{
  Napi::Env          clNapiEnvT = clNapiInfoR.Env();
  Napi::HandleScope  clScopeT(clNapiEnvT);
  int32_t            ulParamCountT;


  //---------------------------------------------------------------------------------------------------
  // check number of parameter
  //
  ulParamCountT = clNapiInfoR.Length();
  if (ulParamCountT != 1)
  {
     Napi::Error::New(clNapiEnvT, "setDataSize() requires one parameter.").ThrowAsJavaScriptException();
  }

  //---------------------------------------------------------------------------------------------------
  // first parameter must be a number
  //
  if (clNapiInfoR[0].IsNumber())
  {
     Napi::Number clNapiDataSizeT = clNapiInfoR[0].As<Napi::Number>();
     uint32_t ulDataSizeT = clNapiDataSizeT.Uint32Value();
     clCanFrameM.setDataSize((uint8_t) ulDataSizeT);
  }
  else
  {
     Napi::TypeError::New(clNapiEnvT, "setDataSize() requires a Number as parameter.").ThrowAsJavaScriptException();
  }

  return Napi::Boolean::New(clNapiEnvT, true);
}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::setDataUInt16()                                                                                          //
// return frame format value                                                                                          //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::setDataUInt16(const Napi::CallbackInfo& clNapiInfoR)
{
   Napi::Env          clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope  clScopeT(clNapiEnvT);
   int32_t            ulParamCountT;
   uint32_t           ulPositionT = 0;
   uint32_t           ulValueT = 0;
   bool               btMsbFirstT = false;


  //---------------------------------------------------------------------------------------------------
  // check number of parameter
  //
  ulParamCountT = clNapiInfoR.Length();
  if (ulParamCountT < 2)
  {
     Napi::Error::New(clNapiEnvT, "setDataUInt16() requires at least two parameter.").ThrowAsJavaScriptException();
  }

  //---------------------------------------------------------------------------------------------------
  // first parameter: start position
  //
  if (ulParamCountT > 0)
  {
     if (clNapiInfoR[0].IsNumber())
     {
        Napi::Number clNapiPositionT = clNapiInfoR[0].As<Napi::Number>();
        ulPositionT = clNapiPositionT.Uint32Value();
     }
     else
     {
        Napi::TypeError::New(clNapiEnvT, "setDataUInt16() requires a Number as parameter.").ThrowAsJavaScriptException();
     }
  }

  //---------------------------------------------------------------------------------------------------
  // second parameter: value
  //
  if (ulParamCountT > 1)
  {
     if (clNapiInfoR[1].IsNumber())
     {
        Napi::Number clNapiValueT = clNapiInfoR[0].As<Napi::Number>();
        ulValueT = clNapiValueT.Uint32Value();
     }
     else
     {
        Napi::TypeError::New(clNapiEnvT, "setDataUInt16() requires a Number as parameter.").ThrowAsJavaScriptException();
     }
  }

  //---------------------------------------------------------------------------------------------------
  // third parameter: MSB first (optional)
  //
  if (ulParamCountT > 2)
  {
     Napi::Boolean clNapiMsbFirstT = clNapiInfoR[2].As<Napi::Boolean>();
     btMsbFirstT = clNapiMsbFirstT.Value();
  }

   clCanFrameM.setDataUInt16((uint8_t) ulPositionT, (uint16_t) ulValueT, btMsbFirstT);

   return Napi::Boolean::New(clNapiEnvT, true);
}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::setDataUInt32()                                                                                          //
// return frame format value                                                                                          //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::setDataUInt32(const Napi::CallbackInfo& clNapiInfoR)
{
  Napi::Env          clNapiEnvT = clNapiInfoR.Env();
  Napi::HandleScope  clScopeT(clNapiEnvT);
  int32_t            ulParamCountT;
  uint32_t           ulPositionT = 0;
  uint32_t           ulValueT = 0;
  bool               btMsbFirstT = false;


  //---------------------------------------------------------------------------------------------------
  // check number of parameter
  //
  ulParamCountT = clNapiInfoR.Length();
  if (ulParamCountT < 2)
  {
     Napi::Error::New(clNapiEnvT, "setDataUInt32() requires at least two parameter.").ThrowAsJavaScriptException();
  }

  //---------------------------------------------------------------------------------------------------
  // first parameter: start position
  //
  if (ulParamCountT > 0)
  {
     if (clNapiInfoR[0].IsNumber())
     {
        Napi::Number clNapiPositionT = clNapiInfoR[0].As<Napi::Number>();
        ulPositionT = clNapiPositionT.Uint32Value();
     }
     else
     {
        Napi::TypeError::New(clNapiEnvT, "setDataUInt32() requires a Number as parameter.").ThrowAsJavaScriptException();
     }
  }

  //---------------------------------------------------------------------------------------------------
  // second parameter: value
  //
  if (ulParamCountT > 1)
  {
     if (clNapiInfoR[1].IsNumber())
     {
        Napi::Number clNapiValueT = clNapiInfoR[0].As<Napi::Number>();
        ulValueT = clNapiValueT.Uint32Value();
     }
     else
     {
        Napi::TypeError::New(clNapiEnvT, "setDataUInt32() requires a Number as parameter.").ThrowAsJavaScriptException();
     }
  }

  //---------------------------------------------------------------------------------------------------
  // third parameter: MSB first (optional)
  //
  if (ulParamCountT > 2)
  {
     Napi::Boolean clNapiMsbFirstT = clNapiInfoR[2].As<Napi::Boolean>();
     btMsbFirstT = clNapiMsbFirstT.Value();
  }

   clCanFrameM.setDataUInt32((uint8_t) ulPositionT, (uint16_t) ulValueT, btMsbFirstT);

   return Napi::Boolean::New(clNapiEnvT, true);

}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::setDlc()                                                                                                 //
// set DLC value                                                                                                      //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::setDlc(const Napi::CallbackInfo& clNapiInfoR)
{
   Napi::Env          clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope  clScopeT(clNapiEnvT);
   int32_t            ulParamCountT;


   //---------------------------------------------------------------------------------------------------
   // check number of parameter
   //
   ulParamCountT = clNapiInfoR.Length();
   if (ulParamCountT != 1)
   {
      Napi::Error::New(clNapiEnvT, "setDlc() requires one parameter.").ThrowAsJavaScriptException();
   }

   //---------------------------------------------------------------------------------------------------
   // first parameter must be a number
   //
   if (clNapiInfoR[0].IsNumber())
   {
      Napi::Number clNapiDlcT = clNapiInfoR[0].As<Napi::Number>();
      uint32_t ulDlcT = clNapiDlcT.Uint32Value();
      clCanFrameM.setDlc((uint8_t) ulDlcT);
   }
   else
   {
      Napi::TypeError::New(clNapiEnvT, "setDlc() requires a Number as parameter.").ThrowAsJavaScriptException();
   }

   return Napi::Boolean::New(clNapiEnvT, true);

}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::setFrameFormat()                                                                                         //
// set frame format                                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::setFrameFormat(const Napi::CallbackInfo& clNapiInfoR)
{
  Napi::Env          clNapiEnvT = clNapiInfoR.Env();
  Napi::HandleScope  clScopeT(clNapiEnvT);
  int32_t            ulParamCountT;


  //---------------------------------------------------------------------------------------------------
  // check number of parameter
  //
  ulParamCountT = clNapiInfoR.Length();
  if (ulParamCountT != 1)
  {
     Napi::Error::New(clNapiEnvT, "setFrameFormat() requires one parameter.").ThrowAsJavaScriptException();
  }

  //---------------------------------------------------------------------------------------------------
  // first parameter must be a number
  //
  if (clNapiInfoR[0].IsNumber())
  {
     Napi::Number clNapiFrameFormatT = clNapiInfoR[0].As<Napi::Number>();
     uint32_t ulFormatT = clNapiFrameFormatT.Uint32Value();
     clCanFrameM.setFrameFormat((QCanFrame::FrameFormat_e) ulFormatT);
  }
  else
  {
     Napi::TypeError::New(clNapiEnvT, "setFrameFormat() requires a Number as parameter.").ThrowAsJavaScriptException();
  }

  return Napi::Boolean::New(clNapiEnvT, true);
}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::setIdentifier()                                                                                          //
// set identifier value                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::setIdentifier(const Napi::CallbackInfo& clNapiInfoR)
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
      Napi::Error::New(clNapiEnvT, "setIdentifier() requires one parameter.").ThrowAsJavaScriptException();
   }

   //---------------------------------------------------------------------------------------------------
   // first parameter must be a number
   //
   if (clNapiInfoR[0].IsNumber())
   {
      Napi::Number clNapiIdentifierT = clNapiInfoR[0].As<Napi::Number>();
      uint32_t ulIdentifierT = clNapiIdentifierT.Uint32Value();
      clCanFrameM.setIdentifier(ulIdentifierT);
   }
   else
   {
      Napi::TypeError::New(clNapiEnvT, "setIdentifier() requires a Number as parameter.").ThrowAsJavaScriptException();
   }

   return Napi::Boolean::New(clNapiEnvT, true);

}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::toString()                                                                                               //
// convert to string object                                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanFrame::toString(const Napi::CallbackInfo& clNapiInfoR)
{
  Napi::Env          clNapiEnvT = clNapiInfoR.Env();
  Napi::HandleScope  clScopeT(clNapiEnvT);

  QString clStringT = clCanFrameM.toString();

  return Napi::String::New(clNapiEnvT, clStringT.toUtf8());
}


//--------------------------------------------------------------------------------------------------------------------//
// CanFrame::init()                                                                                                   //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Object CanFrame::init(Napi::Env clNapiEnvV, Napi::Object clNapiExportsV)
{
   Napi::HandleScope  clNapiScopeT(clNapiEnvV);

   Napi::Function     clNapiFuntionT;

   clNapiFuntionT = DefineClass(clNapiEnvV, "CanFrame",
                               { InstanceMethod("data"                , &CanFrame::data),
                                 InstanceMethod("dataSize"            , &CanFrame::dataSize),
                                 InstanceMethod("dataUInt16"          , &CanFrame::dataUInt16),
                                 InstanceMethod("dataUInt32"          , &CanFrame::dataUInt32),
                                 InstanceMethod("dlc"                 , &CanFrame::dlc),
                                 InstanceMethod("errorStateIndicator" , &CanFrame::errorStateIndicator),
                                 InstanceMethod("frameFormat"         , &CanFrame::frameFormat),
                                 InstanceMethod("identifier"          , &CanFrame::identifier),
                                 InstanceMethod("setBitrateSwitch"    , &CanFrame::setBitrateSwitch),
                                 InstanceMethod("setData"             , &CanFrame::setData),
                                 InstanceMethod("setDataSize"         , &CanFrame::setDataSize),
                                 InstanceMethod("setDataUInt16"       , &CanFrame::setDataUInt16),
                                 InstanceMethod("setDataUInt32"       , &CanFrame::setDataUInt32),
                                 InstanceMethod("setDlc"              , &CanFrame::setDlc),
                                 InstanceMethod("setFrameFormat"      , &CanFrame::setFrameFormat),
                                 InstanceMethod("setIdentifier"       , &CanFrame::setIdentifier),
                                 InstanceMethod("toString"            , &CanFrame::toString)     }
                               );

   constructor = Napi::Persistent(clNapiFuntionT);
   constructor.SuppressDestruct();


   clNapiExportsV.Set("CanFrame", clNapiFuntionT);

   return clNapiExportsV;
}
