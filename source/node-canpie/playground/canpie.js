//------------------------------------------------------------------------------------------------------
// Define a CAN message class 
//
const  FORMAT_CAN_STD = 0
const  FORMAT_CAN_EXT = 1
const  FORMAT_FD_STD  = 2
const  FORMAT_FD_EXT  = 3



//--------------------------------------------------------------------------------------------------------------------//
// message                                                                                                            //
// general CANpie message object for multiple usage                                                                   //
//--------------------------------------------------------------------------------------------------------------------// 
const message = {
    //--------------------------------------------------------------------------------------------------- 
    // set default values for a standart message
    //
    type: FORMAT_CAN_STD,
    id: 0,
    dlc: 0,
    data: new Uint8ClampedArray(8),

    //--------------------------------------------------------------------------------------------------- 
    // implement methodes which validates the set parameter for Message Type
    //
    setType(type) {
        if ((Number.isNaN(type) === true) || (type < FORMAT_CAN_STD) || (type > FORMAT_FD_EXT)) {
            // not a valid number return false here
            return false;
        }

        if ((type === FORMAT_CAN_STD) || (type === FORMAT_CAN_EXT)) {
            this.data = new Uint8ClampedArray(8)
        } else {
            this.data = new Uint8ClampedArray(64)
        }

        // take new number 
        this.type = type;
        return true;
    },

    //--------------------------------------------------------------------------------------------------- 
    // implement methodes which validates the set parameter for ID
    //
    setId(id) {
        if ((this.type === FORMAT_CAN_STD) || (this.type === FORMAT_FD_STD)) {
            if ((Number.isNaN(id) === true) || (id < 0) || (id > 0x7FF)) {
                // not a valid number return false here
                return false;
            }
            // take new number 
            this.id = id;
            return true;
        } else if ((this.type === FORMAT_CAN_EXT) || (this.type === FORMAT_FD_EXT)) {
            if ((Number.isNaN(id) === true) || (id < 0) || (id > 0x1FFFFFFF)) {
                // not a valid number return false here
                return false;
            }
            // take new number 
            this.id = id;
            return true;
        }
        return false;
    },

    //--------------------------------------------------------------------------------------------------- 
    // implement methodes which validates the set parameter for Message Type
    //
    setDlc(dlc) {
        if ((this.type === FORMAT_CAN_STD) || (this.type === FORMAT_CAN_EXT)) {
            if ((Number.isNaN(dlc) === true) || (dlc < 0) || (dlc > 8)) {
                // not a valid number return false here
                return false;
            }
            // take new number 
            this.dlc = dlc;
            return true;
        } else if ((this.type === FORMAT_FD_STD) || (this.type === FORMAT_FD_EXT)) {
            if ((Number.isNaN(dlc) === true) || (dlc < 0) || (dlc > 64)) {
                // not a valid number return false here
                return false;
            }
            // take new number 
            // \todo normalize to 0..8 and 12,16,20,24,32,48,64
            this.dlc = dlc;
            return true;
        }
        return false;
    },

    setData(byte, data) {
        if ((this.type === FORMAT_CAN_STD) || (this.type === FORMAT_CAN_EXT)) {
            if ((Number.isNaN(byte) === true) || (byte < 0) || (byte >= 8)) {
                // not a valid number return false here
                return false;
            }
        } else if ((this.type === FORMAT_FD_STD) || (this.type === FORMAT_FD_EXT)) {
            if ((Number.isNaN(byte) === true) || (byte < 0) || (byte >= 64)) {
                // not a valid number return false here
                return false;
            }
        }

        if ((Number.isNaN(data) === true) || (data < 0) || (data > 255)) {
            return false;
        }
        console.log("set new data at index " + byte + " to " + data);
        this.data[byte] = data;
        return true;
    }
};

function write (message) {
    console.log("Write CANpie Message")
}

function read () {
    var msg = Object.create(this.message);
    msg.id = 17;
    msg.dlc = 8;
    msg.data = [1,2,3,4,5,6,7,8];
    console.log("Read CANpie Message")
    return msg;
}

// perfom an non blocking read on a can message
var msgGlobal = Object.create(message);
readAsynchron = function(callback){
    process.nextTick(function(){
        msgGlobal.id = 22;
        msgGlobal.dlc = 2;
        msgGlobal.data = [0,1,0,1,0,1,0,1];
        console.log("Read CANpie Message")
        callback(msgGlobal);
    },0);
}


exports.CP_FORMAT_CAN_STD = FORMAT_CAN_STD;
exports.CP_FORMAT_CAN_EXT = FORMAT_CAN_EXT;
exports.CP_FORMAT_FD_STD  = FORMAT_FD_STD;
exports.CP_FORMAT_FD_EXT  = FORMAT_FD_EXT;

exports.message  = message;
exports.read  = read;
exports.write  = write;
exports.readAsynchron = readAsynchron;

