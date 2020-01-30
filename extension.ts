
namespace MbitMore {
    let sharedData: number[] = [0, 0, 0, 0];

    /**
    * Starts a Scratch extension service.
    * The handler can call ``setscratchMoreSharedData`` to send any data to Scratch.
    */
    //% blockId=scratchmore_startMbitMoreService block="Scratch More service"
    //% shim=MbitMore::startMbitMoreService
    export function startService(handler: Action):void {
        console.log("Scratch More started");
    }

    /**
     * Set value of the shared data.
     * @param index - Index of shared data.
     * @param value - New value of shared data.
     */
    //% blockId=scratchmore_setMbitMoreSharedData block="set shared %index to %value"
    //% shim=MbitMore::setMbitMoreSharedData
    export function setSharedData(index: SharedDataIndex, value: number):void {
        sharedData[index] = value;
    }

    /**
     * Get value of the shared data.
     */
    //% blockId=scratchmore_getMbitMoreSharedData block="value of shared %index"
    //% shim=MbitMore::getMbitMoreSharedData
    export function getSharedData(index: SharedDataIndex):number {
        return sharedData[index];
    }
}