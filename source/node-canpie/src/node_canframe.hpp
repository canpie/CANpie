//====================================================================================================================//
// File:          cansocket_nodejs.hpp                                                                                //
// Description:   Node.js binding for QCanSocket class                                                                //
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

// #define NAPI_VERSION    3

/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


#include <napi.h>

#include "qcan_frame.hpp"

class CanFrame : public Napi::ObjectWrap<CanFrame>
{
 public:

   //Constructor to initialise
   CanFrame(const Napi::CallbackInfo & clNapiInfoR);
   ~CanFrame();

   //Init function for setting the export key to JS
   static Napi::Object  init(Napi::Env clNapiEnvV, Napi::Object clNapiExportsV);

   QCanFrame            clCanFrameM;

 private:

   Napi::Value          data(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          dataSize(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          dataUInt16(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          dataUInt32(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          dlc(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          errorStateIndicator(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          frameFormat(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          identifier(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          setBitrateSwitch(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          setData(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          setDataSize(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          setDataUInt16(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          setDataUInt32(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          setDlc(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          setFrameFormat(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          setIdentifier(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          toString(const Napi::CallbackInfo& clNapiInfoR);

   static Napi::FunctionReference   constructor; //reference to store the class definition that needs to be exported to JS
};
