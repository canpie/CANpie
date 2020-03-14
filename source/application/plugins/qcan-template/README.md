# Template plugin

This plugin is an example for a CAN interface plugin. It supports one simulated CAN interface.


## CAN simulation

The plugin responds in two different ways on reception of CAN data frames, depending on the identifier
value.

A CAN data frame 

* indetifier > 0: response = id + 1, data inverted

* identifer 0
  byte 0 = command
  byte 1 = counter

   eSIMULATION_COMMAND_NONE = 0       -> OK
   eSIMULATION_COMMAND_REPLY_FRAME    -> gibt letzte Botschaft zurück ?!
   eSIMULATION_COMMAND_SEND_BURST     -> nicht implementiert
   eSIMULATION_COMMAND_ERROR_FRAME_RCV -> recieve error counter increase,
   eSIMULATION_COMMAND_ERROR_FRAME_TRM  -> transmit error counter increase,
   eSIMULATION_COMMAND_ERROR_PLUGIN_CHANNEL -> remove CAN interface
   eSIMULATION_COMMAND_ERROR_PLUGIN_LIBRARY,
   eSIMULATION_COMMAND_ERROR_PLUGIN_DEVICE,


