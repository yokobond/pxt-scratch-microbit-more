// Auto-generated. Do not edit.
declare namespace bluetooth {

    /**
     * Starts a Scratch extension service. The handler must call ``setSensorTemperature``
     * to update the data sent to Scratch.
     */
    //% blockId=bluetooth_startScratchMoreService block="Scratch More service" shim=bluetooth::startScratchMoreService
    function startScratchMoreService(handler: () => void): void;

    /**
     * Sets the current temperature value on the external temperature sensor
     */
    //% blockId=bluetooth_setScratchMoreSlot block="Scratch More at Slot %slot put %value" shim=bluetooth::setScratchMoreSlot
    function setScratchMoreSlot(slot: int32, value: int32): void;
}

// Auto-generated. Do not edit. Really.
