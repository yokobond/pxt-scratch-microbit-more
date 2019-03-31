#include "pxt.h"
#include "ScratchMoreService.h"

#define NOTIFY_PERIOD 100

namespace bluetooth {
    ScratchMoreService* _pService = NULL;
    Action _handler;

    void notifyScratch() {
        while (NULL != _pService) {
            // run action that updates Scratch data
            pxt::runAction0(_handler);
            // raise event to trigger notification
            MicroBitEvent ev(SCRATCH_MORE_ID, SCRATCH_MORE_EVT_NOTIFY);
            // wait period
            fiber_sleep(NOTIFY_PERIOD);
        }
    }

    /**
    * Starts a Scratch extension service. The handler must call ``setSensorTemperature``
    * to update the data sent to Scratch.
    */
    //% blockId=bluetooth_startScratchMoreService block="Scratch More service"
    void startScratchMoreService(Action handler) {
        if (NULL != _pService) return;

        _pService = new ScratchMoreService(uBit);
        _handler = handler;
        pxt::incr(_handler);
        create_fiber(notifyScratch);
    }

    /**
    * Sets the current temperature value on the external temperature sensor
    */
    //% blockId=bluetooth_setScratchMoreSlot block="Scratch More at Slot %slot put %value"
    void setScratchMoreSlot(int slot, int value) {
        if (NULL == _pService) return;

        _pService->setSlot(slot, value);
    }
}
