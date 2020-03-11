#include "pxt.h"
#include "MbitMoreService.h"

#define NOTIFY_PERIOD 100

enum SharedDataIndex {
    //% block="data0"
    DATA0 = 0,
    //% block="data1"
    DATA1 = 1,
    //% block="data2"
    DATA2 = 2,
    //% block="data3"
    DATA3 = 3,
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
            fiber_sleep(NOTIFY_PERIOD);
        }
    }

    /**
    * Starts a Scratch extension service.
    * The handler can call ``setMbitMoreSharedData`` to send any data to Scratch.
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
    * Set shared data value.
    */
    //%
    void setMbitMoreSharedData(SharedDataIndex index, int value) {
        if (NULL == _pService) return;

        _pService->setSharedData((int)index, value);
    }

    /**
     * Get shared data value. 
     */
    //%
    int getMbitMoreSharedData(SharedDataIndex index) {
        if (NULL == _pService) return 0;

        return _pService->getSharedData((int)index);
    }    
}
