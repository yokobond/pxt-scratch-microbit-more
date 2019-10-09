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
     * Set slot value.
     */
    //% blockId=scratchmore_setScratchMoreSlot block="Scratch More at %slot put %value"
    //% shim=ScratchMore::setScratchMoreSlot shim=ScratchMore::setScratchMoreSlot
    function setScratchMoreSlot(slot: Slot, value: int32): void;

    /**
     * Get slot value. 
     */
    //% blockId=scratchmore_getScratchMoreSlot block="Scratch More at %slot"
    //% shim=ScratchMore::getScratchMoreSlot shim=ScratchMore::getScratchMoreSlot
    function getScratchMoreSlot(slot: Slot): int32;
}

// Auto-generated. Do not edit. Really.
