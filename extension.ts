
namespace MbitMore {
    let slots: number[] = [0, 0, 0, 0];

    /**
    * Starts a Scratch extension service.
    * The handler can call ``setscratchMoreSlot`` to send any data to Scratch.
    */
    //% blockId=scratchmore_startMbitMoreService block="Scratch More service"
    //% shim=MbitMore::startMbitMoreService
    export function startService(handler: Action):void {
        console.log("Scratch More started");
    }

    //% blockId=scratchmore_setMbitMoreSlot block="Scratch More at %slot put %value"
    //% shim=MbitMore::setMbitMoreSlot
    export function setSlot(slotIndex: Slot, slotValue: number):void {
        slots[slotIndex] = slotValue;
    }

    /**
     * Get slot value. 
     */
    //% blockId=scratchmore_getMbitMoreSlot block="Scratch More at %slot"
    //% shim=MbitMore::getMbitMoreSlot
    export function getSlot(slotIndex: Slot):number {
        return slots[slotIndex];
    }
}