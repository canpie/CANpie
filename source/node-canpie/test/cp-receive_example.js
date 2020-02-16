/*--------------------------------------------------------------------------------------------------------------------*\
** define required classes                                                                                            **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 
const CANpie = require('./canpie');

//--------------------------------------------------------------------------------------------------------------------//
// receiveCanMessage()                                                                                                //
// strait forward example for reception of a CAN message                                                              //
//--------------------------------------------------------------------------------------------------------------------// 
function receiveCanMessage() {
    const cpSocket = new CANpie.Socket();
    const cpFrame  = new CANpie.Frame();

    const buffer   = new ArrayBuffer(8);
    const canData  = new Uint8Array(buffer);

    //--------------------------------------------------------------------------------------------------- 
    // connect to CANpie Server on CAN channel 1 with timeout delay of 100ms
    // 
    if (cpSocket.connect(CANpie.CAN_CHANNEL_1, 100) === true)
    {
        console.log("Read a CBFF CAN message ...");

 
        // setup CAN message 
        cpFrame.setFrameFormat(CANpie.MSG_FORMAT_CBFF);

        // trigger reading of a CAN message
        setInterval(() => {
            //!\todo Not sure how to use the read function ¯\_(ツ)_/¯
            if (cpSocket.read(cpFrame) === true)
            {
                console.log("read returns true");
            } 
            
        }, 1000);
        
        // release connection to CANpie Server
        cpSocket.disconnect();

    } else {
        console.log("The connection to the CANpie server could not be established.");
    }
}

//---------------------------------------------------------------------------------------------------------------------- 
// Call the example function to receive a CAN message.
//
receiveCanMessage();
