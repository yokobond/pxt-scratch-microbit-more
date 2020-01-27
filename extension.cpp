#include "pxt.h"
#include "MbitMoreService.h"

#define NOTIFY_PERIOD 10

enum Slot {
    //% block="slot0"
    SLOT0 = 0,
    //% block="slot1"
    SLOT1 = 1,
    //% block="slot2"
    SLOT2 = 2,
    //% block="slot3"
    SLOT3 = 3,
};

//% color=#FF9900 weight=95 icon="\uf1b0"
namespace MbitMore {
    MbitMoreService* _pService = NULL;
    Action _handler;

    void notifyScratch() {
        while (NULL != _pService) {
            // run actions in the loop
            pxt::runAction0(_handler);
            // notyfy data to Scratch
            _pService->notify();
            // wait period
            fiber_sleep(NOTIFY_PERIOD);
        }
    }

    /**
    * Starts a Scratch extension service.
    * The handler can call ``setMbitMoreSlot`` to send any data to Scratch.
    */
    //%
    void startMbitMoreService(Action handler) {
        if (NULL != _pService) return;

        _pService = new MbitMoreService(uBit);
        _handler = handler;
        pxt::incr(_handler);
        create_fiber(notifyScratch);
    }

    /**
    * Set slot value.
    */
    //%
    void setMbitMoreSlot(Slot slot, int value) {
        if (NULL == _pService) return;

        _pService->setSlot((int)slot, value);
    }

    /**
     * Get slot value. 
     */
    //%
    int getMbitMoreSlot(Slot slot) {
        if (NULL == _pService) return 0;

        return _pService->getSlot((int)slot);
    }    
}
