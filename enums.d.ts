// Auto-generated. Do not edit.


    declare const enum SharedDataIndex {
    //% block="data0"
    DATA0 = 0,
    //% block="data1"
    DATA1 = 1,
    //% block="data2"
    DATA2 = 2,
    //% block="data3"
    DATA3 = 3,
    }
declare namespace MbitMore {
}


    /**
     * Make it listen events of the event type on the pin.
     * Remove listener if the event type is MICROBIT_PIN_EVENT_NONE.
     */

    declare const enum ScratchBLECommand
    {
    CMD_PIN_CONFIG = 0x80,
    CMD_DISPLAY_TEXT = 0x81,
    CMD_DISPLAY_LED = 0x82,
    CMD_PROTOCOL = 0x90,
    CMD_PIN = 0x91,
    CMD_SHARED_DATA = 0x92,
    }


    declare const enum MBitMorePinCommand
    {
    SET_OUTPUT = 0x01,
    SET_PWM = 0x02,
    SET_SERVO = 0x03,
    SET_PULL = 0x04,
    SET_EVENT = 0x05,
    SET_TOUCH = 0x06,
    }


    declare const enum MBitMorePinMode
    {
    PullNone = 0,
    PullUp = 1,
    PullDown = 2,
    }


    declare const enum MBitMoreDataFormat
    {
    MIX_01 = 0x01,
    MIX_02 = 0x02,
    MIX_03 = 0x03,
    SHARED_DATA = 0x11,
    EVENT = 0x12,
    }

// Auto-generated. Do not edit. Really.
