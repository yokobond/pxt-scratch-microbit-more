// Auto-generated. Do not edit.



    //% color=#FF9900 weight=95 icon="\uf1b0"
declare namespace MbitMore {

    /**
     * Starts a Scratch extension service.
     * The handler can call ``setMbitMoreSlot`` to send any data to Scratch.
     */
    //% shim=MbitMore::startMbitMoreService
    function startMbitMoreService(handler: () => void): void;

    /**
     * Set slot value.
     */
    //% shim=MbitMore::setMbitMoreSlot
    function setMbitMoreSlot(slot: Slot, value: int32): void;

    /**
     * Get slot value. 
     */
    //% shim=MbitMore::getMbitMoreSlot
    function getMbitMoreSlot(slot: Slot): int32;
}

// Auto-generated. Do not edit. Really.
