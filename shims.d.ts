// Auto-generated. Do not edit.



    //% color=#FF9900 weight=95 icon="\uf1b0"
declare namespace ScratchMore {

    /**
     * Starts a Scratch extension service.
     * The handler can call ``setscratchMoreSlot`` to send any data to Scratch.
     */
    //% blockId=scratchmore_startScratchMoreService block="Scratch More service"
    //% shim=ScratchMore::startScratchMoreService shim=ScratchMore::startScratchMoreService
    function startScratchMoreService(handler: () => void): void;

    /**
     * Sets the current temperature value on the external temperature sensor
     */
    //% blockId=scratchmore_setScratchMoreSlot block="Scratch More at %slot put %value"
    //% shim=ScratchMore::setScratchMoreSlot shim=ScratchMore::setScratchMoreSlot
    function setScratchMoreSlot(slot: Slot, value: int32): void;
}

// Auto-generated. Do not edit. Really.
