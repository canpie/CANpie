//====================================================================================================================//
// File:          qcan_interface_template.cpp                                                                         //
// Description:   Template for QCan Interface                                                                         //
//                                                                                                                    //
// Copyright (C) MicroControl GmbH & Co. KG                                                                           //
// 53844 Troisdorf - Germany                                                                                          //
// www.microcontrol.net                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the   //
// following conditions are met:                                                                                      //
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions, the following   //
//    disclaimer and the referenced file 'LICENSE'.                                                                   //
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       //
//    following disclaimer in the documentation and/or other materials provided with the distribution.                //
// 3. Neither the name of MicroControl nor the names of its contributors may be used to endorse or promote products   //
//    derived from this software without specific prior written permission.                                           //
//                                                                                                                    //
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance     //
// with the License. You may obtain a copy of the License at                                                          //
//                                                                                                                    //
//    http://www.apache.org/licenses/LICENSE-2.0                                                                      //
//                                                                                                                    //
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed   //
// on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for  //
// the specific language governing permissions and limitations under the License.                                     //
//                                                                                                                    //
//====================================================================================================================//


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include "qcan_interface_template.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

enum SimulationCommand_e {
   eSIMULATION_COMMAND_NONE = 0,
   eSIMULATION_COMMAND_REPLY_FRAME,
   eSIMULATION_COMMAND_SEND_BURST,
   eSIMULATION_COMMAND_ERROR_FRAME_RCV,
   eSIMULATION_COMMAND_ERROR_FRAME_TRM,
   eSIMULATION_COMMAND_ERROR_PLUGIN_CHANNEL,
   eSIMULATION_COMMAND_ERROR_PLUGIN_LIBRARY,
   eSIMULATION_COMMAND_ERROR_PLUGIN_DEVICE,
   eSIMULATION_COMMAND_ERROR_PLUGIN_UNKNOWN,
   eSIMULATION_COMMAND_MAX
};

/*--------------------------------------------------------------------------------------------------------------------*\
** Class methods                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceTemplate()                                                                                            //
// constructor                                                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterfaceTemplate::QCanInterfaceTemplate(uint16_t uwChannelV)
{

   //---------------------------------------------------------------------------------------------------
   // setup Channel of this interface
   //
   uwChannelP = uwChannelV;
   teCanModeP = eCAN_MODE_STOP;

   //---------------------------------------------------------------------------------------------------
   // The interface is not yet connected
   //
   teConnectedP = UnconnectedState;

   //---------------------------------------------------------------------------------------------------
   // In this example all features are supported by the interface, the actual settings
   // are copied here
   //
   ulFeaturesP  = this->supportedFeatures();

   //---------------------------------------------------------------------------------------------------
   // message buffer for CAN message is empty
   //
   btHasReceivedFrameP = false;


   //---------------------------------------------------------------------------------------------------
   // clear simulation / testing
   //
   ubCommandP    = eSIMULATION_COMMAND_NONE;
   ubCountP      = 0;
   teErrorStateP = eCAN_STATE_STOPPED;

   clErrFrameP.setFrameType(QCanFrame::eFRAME_TYPE_ERROR);
   clErrFrameP.setErrorCounterReceive(0);
   clErrFrameP.setErrorCounterTransmit(0);
   clErrFrameP.setErrorState(eCAN_STATE_STOPPED);
}


//--------------------------------------------------------------------------------------------------------------------//
// ~QCanInterfaceTemplate()                                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterfaceTemplate::~QCanInterfaceTemplate()
{

}


//--------------------------------------------------------------------------------------------------------------------//
// connect()                                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceTemplate::connect(void)
{
   InterfaceError_e teReturnT = eERROR_USED;

   //---------------------------------------------------------------------------------------------------
   // Try to connect the CAN interface
   //
   if (teConnectedP == UnconnectedState)
   {
      //---------------------------------------------------------------------------------------------------
      // tell some details about this CAN interface to the logger
      //
      emit addLogMessage(version(), eLOG_LEVEL_INFO);

      clErrFrameP.setErrorState(eCAN_STATE_BUS_ACTIVE);
      teErrorStateP = eCAN_STATE_BUS_ACTIVE;
      teConnectedP = ConnectedState;
      emit connectionChanged(ConnectedState);
      teReturnT = eERROR_NONE;


      //---------------------------------------------------------------------------------------------------
      // configure the refresh timer which updates all statistic information and sends some signals
      //
      QTimer::singleShot(10, this, SLOT(onTimerEvent()));
   }

   return teReturnT;
}


//--------------------------------------------------------------------------------------------------------------------//
// connectionState()                                                                                                  //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::ConnectionState_e QCanInterfaceTemplate::connectionState(void)
{
   return teConnectedP;
}


//--------------------------------------------------------------------------------------------------------------------//
// disconnect()                                                                                                       //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceTemplate::disconnect()
{
   InterfaceError_e teReturnT = eERROR_DEVICE;

   //---------------------------------------------------------------------------------------------------
   // Try to disconnect the CAN interface
   //
   if (teConnectedP == ConnectedState)
   {
      //---------------------------------------------------------------------------------------------------
      // tell some details about this CAN interface to the logger
      //
      emit addLogMessage(version(), eLOG_LEVEL_INFO);

      clErrFrameP.setErrorState(eCAN_STATE_STOPPED);

      teErrorStateP = eCAN_STATE_STOPPED;
      teConnectedP  = UnconnectedState;
      emit connectionChanged(UnconnectedState);
      teReturnT     = eERROR_NONE;
   }

   return teReturnT;
}

//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceTemplate::disableFeatures()                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void  QCanInterfaceTemplate::disableFeatures(uint32_t ulFeatureMaskV)
{
   //------------------------------------------------------------------------------------
   // mask features supported by QCAN_IF_SUPPORT_xxx
   //
   ulFeatureMaskV = ulFeatureMaskV & QCAN_IF_SUPPORT_MASK;

   //------------------------------------------------------------------------------------
   // disable features
   //
   ulFeaturesP    = ulFeaturesP & (~ulFeatureMaskV);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceTemplate::enableFeatures()                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void  QCanInterfaceTemplate::enableFeatures(uint32_t ulFeatureMaskV)
{
   //------------------------------------------------------------------------------------
   // mask features supported by QCAN_IF_SUPPORT_xxx and the interface
   //
   ulFeatureMaskV = ulFeatureMaskV & QCAN_IF_SUPPORT_MASK;
   ulFeatureMaskV = ulFeatureMaskV & this->supportedFeatures();

   //------------------------------------------------------------------------------------
   // enable features
   //
   ulFeaturesP    = ulFeaturesP | ulFeatureMaskV;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceTemplate::icon()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QIcon QCanInterfaceTemplate::icon(void)
{

   return QIcon(":/images/mc_can_plugin_256.png");
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceTemplate::name()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanInterfaceTemplate::name()
{
   return QString("CAN Plug-In Channel 1");
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceTemplate::onTimerEven()                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void QCanInterfaceTemplate::onTimerEvent(void)
{
   if (connectionState() == ConnectedState)
   {
      if (btHasReceivedFrameP == true)
      {
         btHasReceivedFrameP = false;
         emit readyRead();
      }

      if (teErrorStateP != clErrFrameP.errorState())
      {
         teErrorStateP = clErrFrameP.errorState();
         emit stateChanged(teErrorStateP);
      }
      QTimer::singleShot(50, this, SLOT(onTimerEvent()));

   }
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceTemplate::read()                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfaceTemplate::read(QCanFrame &clFrameR)
{
   InterfaceError_e  clRetValueT = eERROR_FIFO_RCV_EMPTY;

   //---------------------------------------------------------------------------------------------------
   // Reading is only possible in connected() state
   //
   if (connectionState() == ConnectedState)
   {
      //-------------------------------------------------------------------------------------------
      // Test for a possible command and execute the simulation
      //
      switch (ubCommandP)
      {
         //-----------------------------------------------------------------------------------
         // Receive FIFO is empty
         //
         case eSIMULATION_COMMAND_NONE:
            clRetValueT = eERROR_FIFO_RCV_EMPTY;
            break;

         //-----------------------------------------------------------------------------------
         // Reply the request message which is modified inside the write() method.
         //
         case eSIMULATION_COMMAND_REPLY_FRAME:
            clFrameR    = clRcvFrameP;
            clRetValueT = eERROR_NONE;
            ubCommandP  = eSIMULATION_COMMAND_NONE;      // set next action
            break;

         //-----------------------------------------------------------------------------------
         // Send a burst of messages
         //
         case eSIMULATION_COMMAND_SEND_BURST:
            if (ubCountP > 0)
            {
               ubCountP--;
               clRetValueT = eERROR_NONE;
            }
            else
            {
               clRetValueT = eERROR_FIFO_RCV_EMPTY;
               ubCommandP  = eSIMULATION_COMMAND_NONE;   // set next action
            }
            break;


         //-----------------------------------------------------------------------------------
         // Simulate receive error counter
         //
         case eSIMULATION_COMMAND_ERROR_FRAME_RCV:
            if (ubCountP > 0)
            {
               ubCountP--;
               uint8_t ubRcvErrorCounterT = clErrFrameP.errorCounterReceive();
               ubRcvErrorCounterT += 8;
               if (ubRcvErrorCounterT < clErrFrameP.errorCounterReceive())
               {
                  ubRcvErrorCounterT = 255;     // handle possible overflow
               }

               clErrFrameP.setErrorCounterReceive( ubRcvErrorCounterT );
               clFrameR    = clErrFrameP;
               clRetValueT = eERROR_NONE;
            }
            else
            {
               clRetValueT = eERROR_FIFO_RCV_EMPTY;
               ubCommandP  = eSIMULATION_COMMAND_NONE;   // set next action
            }
            break;

         //-----------------------------------------------------------------------------------
         // Simulate transmit error counter
         //
         case eSIMULATION_COMMAND_ERROR_FRAME_TRM:
            if (ubCountP > 0)
            {
               ubCountP--;
               uint8_t ubTrmErrorCounterT = clErrFrameP.errorCounterTransmit();
               ubTrmErrorCounterT += 8;
               if (ubTrmErrorCounterT < clErrFrameP.errorCounterTransmit())
               {
                  ubTrmErrorCounterT = 255;     // handle possible overflow
               }

               clErrFrameP.setErrorCounterTransmit( ubTrmErrorCounterT );
               clFrameR    = clErrFrameP;
               clRetValueT = eERROR_NONE;
            }
            else
            {
               clRetValueT = eERROR_FIFO_RCV_EMPTY;
               ubCommandP  = eSIMULATION_COMMAND_NONE;   // set next action
            }
            break;

         case eSIMULATION_COMMAND_ERROR_PLUGIN_CHANNEL:
            emit connectionChanged(QCanInterface::FailureState);
            ubCommandP  = eSIMULATION_COMMAND_NONE;      // set next action
            break;

         case eSIMULATION_COMMAND_ERROR_PLUGIN_LIBRARY:

            clRetValueT = eERROR_LIBRARY;
            ubCommandP  = eSIMULATION_COMMAND_NONE;      // set next action
            break;

         case eSIMULATION_COMMAND_ERROR_PLUGIN_DEVICE:
            clRetValueT = eERROR_DEVICE;
            ubCommandP  = eSIMULATION_COMMAND_NONE;      // set next action
            break;

         case eSIMULATION_COMMAND_ERROR_PLUGIN_UNKNOWN:

            ubCommandP  = eSIMULATION_COMMAND_NONE;      // set next action
            break;

         default:
            clRetValueT = eERROR_FIFO_RCV_EMPTY;
            ubCommandP  = eSIMULATION_COMMAND_NONE;      // set next action
            break;
      }
   }
   else
   {
      clRetValueT = eERROR_DEVICE;
   }

   return (clRetValueT);
}




//--------------------------------------------------------------------------------------------------------------------//
// reset()                                                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e  QCanInterfaceTemplate::reset()
{
   QString           clLogMessageT;
   InterfaceError_e  clRetValueT = eERROR_NONE;

   //---------------------------------------------------------------------------------------------------
   // message buffer for CAN message is empty
   //
   btHasReceivedFrameP = false;

   //---------------------------------------------------------------------------------------------------
   // clear simulation / testing
   //
   ubCommandP = eSIMULATION_COMMAND_NONE;
   ubCountP   = 0;

   clErrFrameP.setFrameType(QCanFrame::eFRAME_TYPE_ERROR);
   clErrFrameP.setErrorCounterReceive(0);
   clErrFrameP.setErrorCounterTransmit(0);
   clErrFrameP.setErrorState(eCAN_STATE_BUS_ACTIVE);

   clLogMessageT = "Reset CAN interface .... : done";
   emit addLogMessage(clLogMessageT, eLOG_LEVEL_INFO);

   return (clRetValueT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceTemplate::setBitrate()                                                                                //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceTemplate::setBitrate( int32_t slNomBitRateV, int32_t slDatBitRateV)
{
   QString           clLogMessageT;
   InterfaceError_e  clRetValueT = eERROR_NONE;


   if (slDatBitRateV == eCAN_BITRATE_NONE)
   {
      //---------------------------------------------------------------
      // Classical CAN or no bit-rate switching
      //

   }
   else
   {
      if (slNomBitRateV > 1000000)
      {
         clLogMessageT = "Nominal bit-rate out of range";
         clRetValueT   = eERROR_BITRATE;
      }
      else if (slDatBitRateV < slNomBitRateV)
      {
         clLogMessageT = "Data bit-rate must be higher or equal to nominal bit-rate";
         clRetValueT   = eERROR_BITRATE;
      }
      else if ((ulFeaturesP & QCAN_IF_SUPPORT_CAN_FD) == 0)
      {

      }
      else
      {

      }

   }

   emit addLogMessage(clLogMessageT, eLOG_LEVEL_INFO);

   return eERROR_NONE;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceTemplate::setMode                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceTemplate::setMode(const CAN_Mode_e teModeV)
{

   //----------------------------------------------------------------
   // select mode
   //
   switch (teModeV)
   {
      case eCAN_MODE_START :

         //---------------------------------------------------
         // reset statistic values
         //
         clStatisticP.ulErrCount = 0;
         clStatisticP.ulRcvCount = 0;
         clStatisticP.ulTrmCount = 0;


         teCanModeP = eCAN_MODE_START;
         break;

      case eCAN_MODE_STOP :
         teCanModeP = eCAN_MODE_STOP;
         break;

      default :
         return eERROR_MODE;
         break;
   }

   return eERROR_NONE;
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceTemplate::state()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
CAN_State_e QCanInterfaceTemplate::state(void)
{
   return (teErrorStateP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceTemplate::statistic()                                                                                 //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e	QCanInterfaceTemplate::statistic(QCanStatistic_ts &clStatisticR)
{
   //! \todo

   clStatisticR.ulErrCount = 0;

   return(eERROR_NONE);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceTemplate::supportedFeatures()                                                                         //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
uint32_t QCanInterfaceTemplate::supportedFeatures()
{
   uint32_t ulFeaturesT = 0;

   //---------------------------------------------------------------------------------------------------
   // Within this method we test what features are supported by the CAN interface, possible values
   // are defined inside the file qcan_defs.hpp
   //
   ulFeaturesT += QCAN_IF_SUPPORT_ERROR_FRAMES;
   ulFeaturesT += QCAN_IF_SUPPORT_LISTEN_ONLY;
   ulFeaturesT += QCAN_IF_SUPPORT_CAN_FD;

   return (ulFeaturesT);
}



//--------------------------------------------------------------------------------------------------------------------//
// version()                                                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanInterfaceTemplate::version(void)
{
   QString clVersionT;

   clVersionT  = QString("%1.%2.").arg(VERSION_MAJOR).arg(VERSION_MINOR, 2, 10, QLatin1Char('0'));
   clVersionT += QString("%1").arg(VERSION_BUILD, 2, 10, QLatin1Char('0'));

   return (clVersionT);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanInterfaceTemplate::write()                                                                                     //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanInterface::InterfaceError_e QCanInterfaceTemplate::write(const QCanFrame &clFrameR)
{

   //---------------------------------------------------------------------------------------------------
   // handle CAN messages and simulate a response / reaction here
   //
   if (clFrameR.identifier() != 0)
   {
      clRcvFrameP = clFrameR;

      //-------------------------------------------------------------------------------------------
      // the identifier for reception is incremented, data is inverted
      //
      clRcvFrameP.setIdentifier(clRcvFrameP.identifier() + 1);
      for (uint8_t ubDataCountT = 0; ubDataCountT < clRcvFrameP.dataSize(); ubDataCountT++)
      {
         clRcvFrameP.setData(ubDataCountT, ~(clRcvFrameP.data(ubDataCountT)));
      }

      ubCommandP = eSIMULATION_COMMAND_REPLY_FRAME;
      btHasReceivedFrameP = true;
   }

   else
   {
      //-------------------------------------------------------------------------------------------
      // An identifier value of 0 is treated as "command", which controls special functions.
      // Byte 0 : command
      // Byte 1 : counter
      //
      if (clFrameR.dataSize() > 1)
      {
         ubCommandP = clFrameR.data(0);
         ubCountP   = clFrameR.data(1);
         btHasReceivedFrameP = true;
      }
      else
      {
         btHasReceivedFrameP = false;
      }
   }

   return eERROR_NONE;
}

