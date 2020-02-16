//------------------------------------------------------------------------------------------------------
//
// canpie.js


//------------------------------------------------------------------------------------------------------
// Use Strict Mode for JavaScript
//
'use strict';

const CANpie = require('./build/Release/canpie.node');

//------------------------------------------------------------------------------------------------------
// Define CAN channel
//
const channel = {
	CAN1: 1,
	CAN2: 2,
	CAN3: 3,
	CAN4: 4,
	CAN5: 5,
	CAN6: 6,
	CAN7: 7,
	CAN8: 8
}

//------------------------------------------------------------------------------------------------------
// Define CAN message format
//
const messageFormat = {
	CBFF: 0,
	CEFF: 1,
	FBFF: 2,
	FEFF: 3
}

module.exports = {
	Filter          : CANpie.CanFilter,
	FilterList      : CANpie.CanFilterList,
	Frame           : CANpie.CanFrame,
	Socket          : CANpie.CanSocket,
	channel 		: channel,
	messageFormat	: messageFormat
}
