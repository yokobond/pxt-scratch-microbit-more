/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef SCRATCH_MORE_SERVICE_H
#define SCRATCH_MORE_SERVICE_H

#include "MicroBit.h"

#define SCRATCH_MORE_ID 2000

#define SCRATCH_MORE_EVT_NOTIFY 1

// UUIDs for our service and characteristics
extern const uint16_t ScratchMoreServiceUUID;
extern const uint8_t ScratchMoreServiceTxUUID[];
extern const uint8_t ScratchMoreServiceRxUUID[];

/**
  * Class definition for a MicroBitMore Service.
  * Provides a BLE service to remotely read the state of sensors from Scratch3.
  */
class ScratchMoreService
{
public:
  /**
    * Constructor.
    * Create a representation of the ScratchMoreService
    * @param _uBit The instance of a MicroBit runtime.
    */
  ScratchMoreService(MicroBit &_uBit);

  /**
    * Notify data to Scratch3.
    */
  void notify(MicroBitEvent);

  /**
   * Set value to Slots.
   */
  void setSlot(int slot, int value);

  /**
    * Callback. Invoked when any of our attributes are written via BLE.
    */
  void onDataWritten(const GattWriteCallbackParams *params);

  void updateDigitalValues();
  void updateAnalogValues();

private:
  // Sending data to Scratch3.
  uint8_t txData[20];

  /**
   * Sending data buffer A.
   */
  uint8_t txBufferA[20];

  /**
   * Sending data buffer B.
   */
  uint8_t txBufferB[20];

  // Recieving buffer from Scratch3.
  uint8_t rxBuffer[20];

  /**
   * Button state.
   */
  int buttonAState;
  int buttonBState;

  /**
   * Hold gesture state until next nofification.
   */
  int gesture;

  /**
   * Save the last accelerometer values to conpaire current for detecting moving.
   */
  int lastAcc[3];

  /**
   * Heading angle of compass.
   */
  int compassHeading;

  uint32_t digitalValues;

  uint16_t analogValues[6];

  void setInputMode(int pinIndex);
  void setDigitalValue(int pinIndex, int value);
  void setServoValue(int pinIndex, int value);
  void setAnalogValue(int pinIndex, int value);

  void onButtonChanged(MicroBitEvent);
  void onAccelerometerChanged(MicroBitEvent);

  void updateGesture(void);
  void resetGesture(void);

  int normalizeCompassHeading(int heading);
  int convertToTilt(float radians);

  void composeDefaultData(uint8_t *buff);
  void composeTxBufferA(void);
  void composeTxBufferB(void);

  // microbit runtime instance
  MicroBit &uBit;

  // Handles to access each characteristic when they are held by Soft Device.
  GattAttribute::Handle_t txCharacteristicHandle;
  GattAttribute::Handle_t rxCharacteristicHandle;

  enum ScratchBLECommand
  {
    CMD_PIN_CONFIG = 0x80,
    CMD_DISPLAY_TEXT = 0x81,
    CMD_DISPLAY_LED = 0x82
  };
};

#endif
