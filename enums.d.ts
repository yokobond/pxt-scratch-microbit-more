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
     * Protocol of microbit more.
     */

    declare const enum ScratchBLECommand
    {
    CMD_PIN_CONFIG = 0x80,
    CMD_DISPLAY_TEXT = 0x81,
    CMD_DISPLAY_LED = 0x82,
    CMD_PIN_INPUT = 0x90,
    CMD_PIN_OUTPUT = 0x91,
    CMD_PIN_PWM = 0x92,
    CMD_PIN_SERVO = 0x93,
    CMD_SHARED_DATA_SET = 0x94,
    CMD_PROTOCOL_SET = 0xA0,
    }


    declare const enum MBitMoreDataFormat
    {
    MIX_01 = 0x01,
    MIX_02 = 0x02,
    MIX_03 = 0x03,
    SHARED_DATA = 0x11,
    }

// Auto-generated. Do not edit. Really.
