//====================================================================================================================//
// File:          node_cansocket.cpp                                                                                  //
// Description:   Node.js binding for CAN socket                                                                      //
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

#include <QtCore/QCoreApplication>

#include <uv.h>                        // header for libuv


#include "node_canframe.hpp"
#include "node_cansocket.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Static variables                                                                                                   **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

Napi::FunctionReference    CanSocket::clConstructorP;

static QCoreApplication *  pclApplicationS = 0L;
static uv_timer_t          tsEventTimerS;


/*--------------------------------------------------------------------------------------------------------------------*\
** Node.js binding                                                                                                    **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

void EventTimer(uv_timer_t *handle)
{
   if (pclApplicationS != 0L)
   {
      pclApplicationS->processEvents();
   }
}


//--------------------------------------------------------------------------------------------------------------------//
// CanSocket::CanSocket()                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CanSocket::CanSocket(const Napi::CallbackInfo& clNapiInfoR) : Napi::ObjectWrap<CanSocket>(clNapiInfoR)
{
   Napi::Env            clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope    clScopeT(clNapiEnvT);
   int32_t              slParamCountT;

   //---------------------------------------------------------------------------------------------------
   // check number of parameter
   //
   slParamCountT = clNapiInfoR.Length();
   if (slParamCountT != 0)
   {
      Napi::TypeError::New(clNapiEnvT, "Constructor has no parameter.").ThrowAsJavaScriptException();
   }

   //---------------------------------------------------------------------------------------------------
   // create a socket
   //
   pclCanSocketP = new QCanSocket();


   //---------------------------------------------------------------------------------------------------
   // connect signal for message reception to the event handler slot
   //
   if (pclCanSocketP != Q_NULLPTR)
   {
      connect(pclCanSocketP, &QCanSocket::disconnected,   this, &CanSocket::onDisconnectEventHandler);
      connect(pclCanSocketP, &QCanSocket::framesReceived, this, &CanSocket::onMessageEventHandler);
   }

}


//--------------------------------------------------------------------------------------------------------------------//
// CanSocket::~CanSocket()                                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CanSocket::~CanSocket()
{

}


//--------------------------------------------------------------------------------------------------------------------//
// CanSocket::attachEmitter()                                                                                         //
// connect to socket                                                                                                  //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanSocket::attachEmitter(const Napi::CallbackInfo& clNapiInfoR)
{
   Napi::Env          clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope  clScopeT(clNapiEnvT);
   Napi::Function     clEmitT;

   int slLengthT = clNapiInfoR.Length();

   if (slLengthT != 1)
   {
     Napi::TypeError::New(clNapiEnvT, "Method requires ome parameter.").ThrowAsJavaScriptException();
   }

   printf("Save callback function .. \n");
   clEmitT = clNapiInfoR[0].As<Napi::Function>();
   clEmitT.Call({Napi::String::New(clNapiEnvT, "disconnect")});
   clEmitT.Call({Napi::String::New(clNapiEnvT, "frames"), Napi::String::New(clNapiEnvT, "data ...")});

   // Napi::Function otto =  Napi::Function::New(clNapiEnvT, clEmitT);
   //clEmitFunctionP =

   // clEmitFunctionP = clEmitT;
   //clEmitFunctionP = clNapiInfoR[0].As<Napi::Function>();
   //clEmitCmdDisconnectP = Napi::String::New	(	clNapiEnvT, "disconnect");
   //clEmitCallbackP = clNapiInfoR.This();

   return Napi::Boolean::New(clNapiEnvT, true);
}

//--------------------------------------------------------------------------------------------------------------------//
// CanSocket::connectNetwork()                                                                                        //
// connect to socket                                                                                                  //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanSocket::connectNetwork(const Napi::CallbackInfo& clNapiInfoR)
{
  Napi::Env          clNapiEnvT = clNapiInfoR.Env();
  Napi::HandleScope  clScopeT(clNapiEnvT);

  int slLengthT = clNapiInfoR.Length();

  if (slLengthT != 2)
  {
     Napi::TypeError::New(clNapiEnvT, "Method requires two parameter.").ThrowAsJavaScriptException();
  }

  Napi::Number clNapiNumChannelT = clNapiInfoR[0].As<Napi::Number>();
  Napi::Number clNapiNumWaitT    = clNapiInfoR[1].As<Napi::Number>();

  int32_t slChannelT = clNapiNumChannelT.Int32Value();
  int32_t slWaitT    = clNapiNumWaitT.Int32Value();

  bool btResultT = pclCanSocketP->connectNetwork((CAN_Channel_e) slChannelT, slWaitT);

  return Napi::Boolean::New(clNapiEnvT, btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CanSocket::disconnectNetwork()                                                                                     //
// disconnect from socket                                                                                             //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanSocket::disconnectNetwork(const Napi::CallbackInfo& clNapiInfoR)
{
   Napi::Env          clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope  clScopeT(clNapiEnvT);

   pclCanSocketP->disconnectNetwork();

   return Napi::Boolean::New(clNapiEnvT, true);
}


//--------------------------------------------------------------------------------------------------------------------//
// CanSocket::framesAvailable()                                                                                       //
// check how many frames are available in the queue                                                                   //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanSocket::framesAvailable(const Napi::CallbackInfo& clNapiInfoR)
{
   Napi::Env          clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope  clScopeT(clNapiEnvT);

   int32_t slFrameNumT = pclCanSocketP->framesAvailable();


   return Napi::Number::New(clNapiEnvT, slFrameNumT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CanSocket::onDisconnectEventHandler()                                                                              //
// run the Qt event handler                                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
void CanSocket::onDisconnectEventHandler(void)
{
   // printf("CanSocket::onDisconnectEventHandler()  \n");
   //clEmitFunctionP.Call(clEmitCallbackP, clEmitCmdDisconnectP);
}


//--------------------------------------------------------------------------------------------------------------------//
// CanSocket::onMessageEventHandler()                                                                                 //
// run the Qt event handler                                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
void CanSocket::onMessageEventHandler(uint32_t ulFrameCntV)
{
   // printf("CanSocket::onMessageEventHandler() - %d messages .. \n", ulFrameCntV);

   /*
   Napi::Env          clNapiEnvT = Env().Null();
   Napi::HandleScope  clScopeT(clNapiEnvT);

   clEmitFunctionP.Call( {clNapiEnvT, Napi::Number::New(Env(), ulFrameCntV)});
   */
   //Napi::Env          clNapiEnvT = Napi::Env::Global();
   //clEmitFunctionP.Call({Napi::String::New(clNapiEnvT, "disconnect")});

}


//--------------------------------------------------------------------------------------------------------------------//
// CanSocket::processEvents()                                                                                         //
// process events like message reception or socket disconnect                                                         //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanSocket::processEvents(const Napi::CallbackInfo& clNapiInfoR)
{
   Napi::Env          clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope  clScopeT(clNapiEnvT);
   

   //---------------------------------------------------------------------------------------------------
   // the function gets one parameter, which is the emitter function callback
   //
   int slLengthT = clNapiInfoR.Length();

   if (slLengthT != 1)
   {
     Napi::TypeError::New(clNapiEnvT, "Method requires ome parameter.").ThrowAsJavaScriptException();
   }

   //---------------------------------------------------------------------------------------------------
   // first parameter must be a function
   //
   Napi::Function clNapiFunctionT = clNapiInfoR[0].As<Napi::Function>();
   if (clNapiFunctionT.IsFunction() == false)
   {
      Napi::TypeError::New(clNapiEnvT, "process() requires a function as parameter.").ThrowAsJavaScriptException();
   }


   //---------------------------------------------------------------------------------------------------
   // check for disconnected state
   //
   if (pclCanSocketP->isConnected() == false)
   {
      clNapiFunctionT.Call({Napi::String::New(clNapiEnvT, "disconnect")});
      return Napi::Boolean::New(clNapiEnvT, false);
   }

   //---------------------------------------------------------------------------------------------------
   // check for frames available
   //
   uint32_t ulFramesT = pclCanSocketP->framesAvailable();
   if (ulFramesT > 0)
   {
      clNapiFunctionT.Call({Napi::String::New(clNapiEnvT, "frames"), Napi::Number::New(clNapiEnvT, ulFramesT)});
   }

   return Napi::Boolean::New(clNapiEnvT, true);
}


//--------------------------------------------------------------------------------------------------------------------//
// CanSocket::read()                                                                                                  //
// read message from socket                                                                                           //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanSocket::read(const Napi::CallbackInfo& clNapiInfoR)
{
   Napi::Env          clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope  clScopeT(clNapiEnvT);
   bool               btResultT = false;

   //---------------------------------------------------------------------------------------------------
   // check number of parameter
   //
   int slLengthT = clNapiInfoR.Length();
   if (slLengthT != 1)
   {
      Napi::TypeError::New(clNapiEnvT, "read() requires a CanFrame object as parameter.").ThrowAsJavaScriptException();
   }

   //---------------------------------------------------------------------------------------------------
   // first parameter must be an object
   //
   Napi::Object clNapiObject = clNapiInfoR[0].As<Napi::Object>();
   if (clNapiObject.IsObject() == false)
   {
      Napi::TypeError::New(clNapiEnvT, "read() requires a CanFrame object as parameter.").ThrowAsJavaScriptException();
   }

   CanFrame * pclCanFrameT = Napi::ObjectWrap<CanFrame>::Unwrap(clNapiObject);
   if (pclCanFrameT != 0L)
   {
      //-------------------------------------------------------------------------------------------
      // read CAN frame from socket
      //
      btResultT = pclCanSocketP->read(pclCanFrameT->clCanFrameM);
   }

   return Napi::Boolean::New(clNapiEnvT, btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CanSocket::write()                                                                                                 //
// write a CAN message                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Value CanSocket::write(const Napi::CallbackInfo& clNapiInfoR)
{
   Napi::Env          clNapiEnvT = clNapiInfoR.Env();
   Napi::HandleScope  clNapiScopeT(clNapiEnvT);

   int slLengthT = clNapiInfoR.Length();

   if (slLengthT != 1)
   {
      Napi::TypeError::New(clNapiEnvT, "write() requires a CanFrame object as parameter.").ThrowAsJavaScriptException();
   }

   Napi::Object clNapiObject = clNapiInfoR[0].As<Napi::Object>();
   bool btResultT = false;
   if (clNapiObject.IsObject())
   {
      CanFrame * pclCanFrameT = Napi::ObjectWrap<CanFrame>::Unwrap(clNapiObject);
      if (pclCanFrameT != 0L)
      {
         btResultT = pclCanSocketP->write(pclCanFrameT->clCanFrameM);
      }
   }

   return Napi::Boolean::New(clNapiEnvT, btResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// CanSocket::cleanup()                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void CanSocket::cleanup(void * pvdArgumentV)
{
   if (pclApplicationS != 0L)
   {
      printf("Quit application loop :-)) \n");
      pclApplicationS->quit();
   }

   printf("Good by, you litte bastard :-)) \n");
}


//--------------------------------------------------------------------------------------------------------------------//
// CanSocket::init()                                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
Napi::Object CanSocket::init(Napi::Env clNapiEnvV, Napi::Object clNapiExportsV)
{
   Napi::HandleScope  clNapiScopeT(clNapiEnvV);
   Napi::Function     clNapiFuntionT;


   //---------------------------------------------------------------------------------------------------
   // Create one instance of QCoreApplication:
   // Check if an instance of QCoreApplication is active. In case there is no QCoreApplication there
   // is no QEventLoop and we need a thread that polls data from the CANpie server.
   //
   if (pclApplicationS == 0L)
   {
      if (QCoreApplication::instance() == 0L)
      {
         int32_t slAppArgcT = 0;
         char ** szAppArgvT = NULL;
         pclApplicationS = new QCoreApplication(slAppArgcT, szAppArgvT);
      }
      else
      {
         pclApplicationS = QCoreApplication::instance();
      }
   }


   //---------------------------------------------------------------------------------------------------
   // Create a hook which is called when the module quits
   //
   //Napi::Status clHookStatusT
   napi_add_env_cleanup_hook(clNapiEnvV, &CanSocket::cleanup, 0);


   //---------------------------------------------------------------------------------------------------
   // setup a timer that calls the Qt event handler
   //
   uv_timer_init(uv_default_loop(), &tsEventTimerS);
   uv_timer_start(&tsEventTimerS, EventTimer, 5, 5);

   //---------------------------------------------------------------------------------------------------

   clNapiFuntionT = DefineClass(clNapiEnvV, "CanSocket",
                              {
                                  InstanceMethod("connect"   , &CanSocket::connectNetwork),
                                  InstanceMethod("disconnect", &CanSocket::disconnectNetwork),
                                  InstanceMethod("process"   , &CanSocket::processEvents),
                                  InstanceMethod("read"      , &CanSocket::read),
                                  InstanceMethod("write"     , &CanSocket::write)
                              }    );

   clConstructorP = Napi::Persistent(clNapiFuntionT);
   clConstructorP.SuppressDestruct();


   clNapiExportsV.Set("CanSocket", clNapiFuntionT);

   return clNapiExportsV;
}
