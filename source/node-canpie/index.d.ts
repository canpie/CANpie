
/*--------------------------------------------------------------------------------------------------------------------*\
** TypeScript Declaration file for canpie Node.js package                                                             **
** http://www.typescriptlang.org/docs/handbook/declaration-files/introduction.html                                    **
\*--------------------------------------------------------------------------------------------------------------------*/ 

//---------------------------------------------------------------------------------------------------------------------- 
// some globale declarations of constant declared values
//

/** CAN channel number of the CANpie Server */
declare const channel = {
    /** Channel number 1 */
    CAN1: 1,
    /** Channel number 2 */
    CAN2: 2,
    /** Channel number 3 */
    CAN3: 3,
    /** Channel number 4 */
    CAN4: 4,
    /** Channel number 5 */
    CAN5: 5,
    /** Channel number 6 */
    CAN6: 6,
    /** Channel number 7 */
    CAN7: 7,
    /** Channel number 8 */
    CAN8: 8
}

/** CAN message frame formats */
declare const messageFormat = {
    /** Classical base frame format */
    CBFF: 0,
    /** Classical extended frame format */
    CEFF: 1,
    /** FD base frame format */
    FBFF: 2,
    /** FD extended frame format */
    FEFF: 3
}

//---------------------------------------------------------------------------------------------------------------------- 
// Describe API of used classes and their methods
//

/**
 * Filter for a CAN receive message
 */
export class Filter {
    /**  
     * Set CAN acceptance mask for frame reception
     * @param frameType type of a frame, that is described by #messageFormat
     * @param identifierLow low identifier value 
     * @param identifierLow high identifier value
     * @returns a number
     */
    static acceptFrame(frameType: number, identifierLow: number, identifierHigh: number) : number;
    /**  
     * This function do something
     * @param payload uint8 array
     * @returns a boolean value
     */
    static rejectFrame(payload: Uint8Array) : boolean;
}

/**
 * Socket of CANpie 
 */
export class Socket {
    /**  
     * Perofrm connection to a CANpie Server
     * 
     * @param channel CAN channel number
     * @param timeout timeout value given in milliseconds for socket connection
     * @returns true on successful connection or false on error
     */
    static connect(channel: number, timeout: number) : boolean;
}