// Auto-generated. Do not edit.
declare namespace ScratchMore {

    /**
     * Starts a Scratch extension service.
     * The handler can call ``setscratchMoreSlot`` to send any data to Scratch.
     */
    //% shim=ScratchMore::startScratchMoreService
    function startScratchMoreService(handler: () => void): void;

    /**
     * Sets the current temperature value on the external temperature sensor
     */
    //% shim=ScratchMore::setScratchMoreSlot
    function setScratchMoreSlot(slot: Slot, value: int32): void;
}

// Auto-generated. Do not edit. Really.
