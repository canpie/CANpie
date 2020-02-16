'use strict'

const CANpie = require('../index.js');

const EventEmitter = require('events').EventEmitter;
const emitter = new EventEmitter()




let Socket01 = new CANpie.Socket();


//Socket01.attachEmitter(emitter.emit.bind(emitter));


let socketConnected = Socket01.connect(CANpie.channel.CAN1, 500);
if (socketConnected == true)
{
   console.log("Connected to CANpie server .. ");
}

emitter.on('disconnect', () => {
    console.log('### DISCONNECT ...:')
})

emitter.on('frames', (number) => {
    let cpFrame  = new CANpie.Frame();
    while(Socket01.read(cpFrame) === true)
    {
        console.log(cpFrame.toString());
    }
})

process.on('SIGINT', function() {
    console.log("Caught interrupt signal");
    Socket01.disconnect();
    clearInterval(timeout);
    process.exit();
});


function sleep(millis)
{
    return new Promise(resolve => setTimeout(resolve, millis));
}

function processMessages()
{
   Socket01.process(emitter.emit.bind(emitter))
}

let timeout = setInterval(processMessages, 10);

async function main() {
    console.log("Let it go .. \n");
    await sleep(20000);
    console.log("done.\n");
    clearInterval(timeout);
}

// main();
