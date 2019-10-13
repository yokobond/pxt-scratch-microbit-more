// Auto-generated. Do not edit.



    //% color=#FF9900 weight=95 icon="\uf1b0"
declare namespace ScratchMore {

    /**
     * Starts a Scratch extension service.
     * The handler can call ``setscratchMoreSlot`` to send any data to Scratch.
     */
    //% shim=ScratchMore::startScratchMoreService
    function startScratchMoreService(handler: () => void): void;

    /**
     * Set slot value.
     */
    //% shim=ScratchMore::setScratchMoreSlot
    function setScratchMoreSlot(slot: Slot, value: int32): void;

    /**
     * Get slot value. 
     */
    //% shim=ScratchMore::getScratchMoreSlot
    function getScratchMoreSlot(slot: Slot): int32;
}

// Auto-generated. Do not edit. Really.
