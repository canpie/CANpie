const CANpie = require('../index.js');

let CanMsg100 = new CANpie.Frame(CANpie.messageFormat.CBFF, 100, 5);
let CanData01 = new Uint8ClampedArray(8);
CanData01[0] = 0x88;
CanData01[1] = 0x99;
CanData01[2] = 0xAA;
CanData01[3] = 0xBB;
console.log("CAN data: " + CanData01.toString());
CanMsg100.setData(CanData01);

let CanData02 = CanMsg100.data();
console.log("CAN data: " + CanData02.toString());

console.log("CAN message: " + CanMsg100.toString());
console.log("CAN ID     : " + CanMsg100.identifier().toString(16));

let CanMsg200 = new CANpie.Frame(CANpie.messageFormat.CEFF);
console.log("CAN message: " + CanMsg200.toString());
console.log("CAN ID     : " + CanMsg200.identifier().toString(16));

let Socket01 = new CANpie.Socket();

Socket01.connect(CANpie.channel.CAN1, 500);
Socket01.write(CanMsg100);
Socket01.disconnect();
