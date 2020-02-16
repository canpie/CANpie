/*--------------------------------------------------------------------------------------------------------------------*\
** define required classes                                                                                            **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 
const CANpie = require('./canpie');

//--------------------------------------------------------------------------------------------------------------------//
// senCanMessage()                                                                                                    //
// strait forward example for sending a CAN message                                                                   //
//--------------------------------------------------------------------------------------------------------------------// 
function sendCanMessage() {
    const cpSocket = new CANpie.Socket();
    const cpFrame  = new CANpie.Frame();

    const buffer   = new ArrayBuffer(8);
    const canData  = new Uint8Array(buffer);

    //--------------------------------------------------------------------------------------------------- 
    // connect to CANpie Server on CAN channel 1 with timeout delay of 100ms
    // 
    if (cpSocket.connect(CANpie.CAN_CHANNEL_1, 100) === true)
    {
        console.log("Send a CBFF CAN message with Identifier 127h, DLC 5 and data [11 22 03 04 05] ...");

        // prepare data
        canData.set([0x11,0x22,3,4,5,6,7,8],0);

        // setup CAN message 
        cpFrame.setFrameFormat(CANpie.MSG_FORMAT_CBFF);
        cpFrame.setIdentifier(0x127);
        cpFrame.setDlc(5);
        cpFrame.setData(canData);
        
        // trigger transmission
        cpSocket.write(cpFrame);
        
        // release connection to CANpie Server
        cpSocket.disconnect();

    } else {
        console.log("The connection to the CANpie server could not be established.");
    }
}

//---------------------------------------------------------------------------------------------------------------------- 
// Call the example function to send a CAN message.
//
setInterval(() => {
    sendCanMessage();
}, 1000);