
namespace ScratchMore {
    let slots: number[] = [0, 0, 0, 0];

    /**
    * Starts a Scratch extension service.
    * The handler can call ``setscratchMoreSlot`` to send any data to Scratch.
    */
    //% blockId=scratchmore_startScratchMoreService block="Scratch More service"
    //% shim=ScratchMore::startScratchMoreService
    export function startService(handler: Action):void {
        console.log("Scratch More started");
    }

    //% blockId=scratchmore_setScratchMoreSlot block="Scratch More at %slot put %value"
    //% shim=ScratchMore::setScratchMoreSlot
    export function setSlot(slotIndex: Slot, slotValue: number):void {
        slots[slotIndex] = slotValue;
    }

    /**
     * Get slot value. 
     */
    //% blockId=scratchmore_getScratchMoreSlot block="Scratch More at %slot"
    //% shim=ScratchMore::getScratchMoreSlot
    export function getSlot(slotIndex: Slot):number {
        return slots[slotIndex];
    }
}