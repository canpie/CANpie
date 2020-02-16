//====================================================================================================================//
// File:          node_cansocket.hpp                                                                                //
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

#include "qcan_socket.hpp"
#include <QtCore/QObject>


class CanSocket : public QObject, public Napi::ObjectWrap<CanSocket>
{
   Q_OBJECT

public:

   //Constructor to initialise
   CanSocket(const Napi::CallbackInfo & clNapiInfoR);
   ~CanSocket();

   //Init function for setting the export key to JS
   static Napi::Object  init(Napi::Env clNapiEnvV, Napi::Object clNapiExportsV);
   static void          cleanup(void * pvdArgumentV);

private slots:
   void                 onDisconnectEventHandler(void);
   void                 onMessageEventHandler(uint32_t ulFrameCntV);
private:

   Napi::Value          attachEmitter(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          connectNetwork(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          disconnectNetwork(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          framesAvailable(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          processEvents(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          read(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          setHostAddress(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          state(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Value          write(const Napi::CallbackInfo& clNapiInfoR);

   Napi::Function                   clEmitFunctionP;

   //---------------------------------------------------------------------------------------------------
   // reference to store the class definition that needs to be exported to JS
   //
   static Napi::FunctionReference   clConstructorP;

   //---------------------------------------------------------------------------------------------------
   // internal instance of QCanSocket class used to perform actual operations
   //
   QCanSocket *                     pclCanSocketP;

};
