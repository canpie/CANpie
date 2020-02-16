var cp = require("./canpie")



const message1 = Object.create(cp.message);
const message2 = Object.create(cp.message);

console.log("Set valid Message ID: " + message1.setDlc(7));
console.log("Set invalid Message ID: " + message1.setDlc(9999));
console.log("Setup data: "+message1.setData(10, 18));

message1.id = 22;   // possible way to set an non checked value 

// some exmaples to set checke values 
console.log("Setup FD frame: "+message2.setType(cp.CP_FORMAT_FD_EXT));
console.log("Setup data: "+message2.setData(10, 18));
console.log("Setup Id: "+message2.setId(180));
console.log("Setup Dlc: "+message2.setDlc(8));
console.log("Setup Dlc: "+message2.setData(63, 54));

// print message 
console.log("Conten of message1: Type " + message1.type + ", ID " + message1.id + ", DLC " + message1.dlc + ", Data " + message1.data)
console.log("Conten of message2: Type " + message2.type + ", ID " + message2.id + ", DLC " + message2.dlc + ", Data " + message2.data)

// Get the keys of the object
const keys = Object.keys(message1);
console.log("message 1:" + keys);

// Iterate through the keys
Object.keys(message2).forEach(key => {
    let value = message2[key];
     console.log(`${key}: ${value}`);
});

// Get the length of the keys
const length = Object.keys(message2).length;
console.log(length);


// Get all values of the object
const values = Object.values(message1);

console.log(values);


var myNewMessage = cp.read();

// Get the object key/value pairs
const entries = Object.entries(myNewMessage);
console.log(entries);

cp.write(myNewMessage);


readAsynchron(function(nonBlkMsg){
        // Get the object key/value pairs
        const entries2 = Object.entries(nonBlkMsg);
        console.log(entries2);
    })


