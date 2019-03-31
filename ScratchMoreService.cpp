/*
The MIT License (MIT)

Copyright (c) 2019 Koji Yokokawa.

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

/**
  * Class definition for the Scratch MicroBit More Service.
  * Provides a BLE service to remotely controll Micro:bit from Scratch3.
  */
#include "ScratchMoreService.h"

int digitalIn[] = {0, 1, 2, 8, 13, 14, 15, 16};
int analogIn[] = {0, 1, 2};

/**
  * Constructor.
  * Create a representation of the ScratchMoreService
  * @param _uBit The instance of a MicroBit runtime.
  */
ScratchMoreService::ScratchMoreService(MicroBit &_uBit)
    : uBit(_uBit)
{
  // Initialize pin configuration.
  for (size_t i = 0; i < sizeof(digitalIn) / sizeof(digitalIn[0]); i++)
  {
    setInputMode(digitalIn[i]);
  }
  for (size_t i = 0; i < sizeof(analogIn) / sizeof(analogIn[0]); i++)
  {
    setInputMode(analogIn[i]);
  }

  // Create the data structures that represent each of our characteristics in Soft Device.
  GattCharacteristic txCharacteristic(
      ScratchMoreServiceTxUUID,
      (uint8_t *)&txData,
      0,
      sizeof(txData),
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

  GattCharacteristic rxCharacteristic(
      ScratchMoreServiceRxUUID,
      (uint8_t *)&rxBuffer,
      0,
      sizeof(rxBuffer),
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE);

  // Set default security requirements
  txCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  rxCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

  GattCharacteristic *characteristics[] = {&txCharacteristic, &rxCharacteristic};
  GattService service(
      ScratchMoreServiceUUID, characteristics,
      sizeof(characteristics) / sizeof(GattCharacteristic *));

  uBit.ble->addService(service);

  txCharacteristicHandle = txCharacteristic.getValueHandle();
  rxCharacteristicHandle = rxCharacteristic.getValueHandle();

  // Write initial value.
  uBit.ble->gattServer().write(
      txCharacteristicHandle,
      (uint8_t *)&txData,
      sizeof(txData));

  // Advertise this service.
  const uint16_t uuid16_list[] = {ScratchMoreServiceUUID};
  uBit.ble->accumulateAdvertisingPayload(GapAdvertisingData::INCOMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));

  // Setup callbacks for button events.
  if (EventModel::defaultEventBus)
  {
    EventModel::defaultEventBus->listen(SCRATCH_MORE_ID, SCRATCH_MORE_EVT_NOTIFY, this, &ScratchMoreService::notify, MESSAGE_BUS_LISTENER_IMMEDIATE);
    EventModel::defaultEventBus->listen(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, this, &ScratchMoreService::onButtonChanged, MESSAGE_BUS_LISTENER_IMMEDIATE);
    EventModel::defaultEventBus->listen(MICROBIT_ID_BUTTON_B, MICROBIT_EVT_ANY, this, &ScratchMoreService::onButtonChanged, MESSAGE_BUS_LISTENER_IMMEDIATE);
    EventModel::defaultEventBus->listen(MICROBIT_ID_ACCELEROMETER, MICROBIT_ACCELEROMETER_EVT_DATA_UPDATE, this, &ScratchMoreService::onAccelerometerChanged, MESSAGE_BUS_LISTENER_IMMEDIATE);
  }

  uBit.ble->onDataWritten(this, &ScratchMoreService::onDataWritten);
}

/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void ScratchMoreService::onDataWritten(const GattWriteCallbackParams *params)
{
  uint8_t *data = (uint8_t *)params->data;

  if (params->handle == rxCharacteristicHandle && params->len > 0)
  {
    if (data[0] == ScratchBLECommand::CMD_DISPLAY_TEXT)
    {
      ManagedString text;
      for (int i = 1; i < params->len; i++)
      {
        text = text + ManagedString(char(data[i]));
      }
      uBit.display.scroll(text, 120); // Interval is corresponding with the Scratch extension.
    }
    else if (data[0] == ScratchBLECommand::CMD_DISPLAY_LED)
    {
      uBit.display.stopAnimation();
      for (int y = 1; y < params->len; y++)
      {
        for (int x = 0; x < 5; x++)
        {
          uBit.display.image.setPixelValue(x, y - 1, (data[y] & (0x01 << x)) ? 255 : 0);
        }
      }
    }
  }
}

/**
  * Button update callback
  */
void ScratchMoreService::onButtonChanged(MicroBitEvent e)
{
  int state;
  if (e.value == MICROBIT_BUTTON_EVT_UP)
  {
    state = 0;
  }
  if (e.value == MICROBIT_BUTTON_EVT_DOWN)
  {
    state = 1;
  }
  if (e.value == MICROBIT_BUTTON_EVT_HOLD)
  {
    state = 5;
  }
  // if (e.value == MICROBIT_BUTTON_EVT_CLICK)
  // {
  //   state = 3;
  // }
  // if (e.value == MICROBIT_BUTTON_EVT_LONG_CLICK)
  // {
  //   state = 4;
  // }
  // if (e.value == MICROBIT_BUTTON_EVT_DOUBLE_CLICK)
  // {
  //   state = 6;
  // }
  if (e.source == MICROBIT_ID_BUTTON_A)
  {
    buttonAState = state;
  }
  if (e.source == MICROBIT_ID_BUTTON_B)
  {
    buttonBState = state;
  }
}

void ScratchMoreService::onAccelerometerChanged(MicroBitEvent)
{
  if (uBit.accelerometer.getGesture() == MICROBIT_ACCELEROMETER_EVT_SHAKE)
  {
    gesture = gesture | 1;
  }
  if (uBit.accelerometer.getGesture() == MICROBIT_ACCELEROMETER_EVT_FREEFALL)
  {
    gesture = gesture | 1 << 1;
  }
}

/**
 * Normalize angle in upside down.
 */
int ScratchMoreService::normalizeCompassHeading(int heading)
{
  if (uBit.accelerometer.getZ() > 0)
  {
    if (heading <= 180)
    {
      heading = 180 - heading;
    }
    else
    {
      heading = 360 - (heading - 180);
    }
  }
  return heading;
}

/**
 * Convert roll/pitch radians to Scratch extension value (-1000 to 1000).
 */
int ScratchMoreService::convertToTilt(float radians)
{
  float degrees = (360.0f * radians) / (2.0f * PI);
  float tilt = degrees * 1.0f / 90.0f;
  if (degrees > 0)
  {
    if (tilt > 1.0f)
      tilt = 2.0f - tilt;
  }
  else
  {
    if (tilt < -1.0f)
      tilt = -2.0f - tilt;
  }
  return (int)(tilt * 1000.0f);
}

void ScratchMoreService::updateGesture()
{
  int old[] = {lastAcc[0], lastAcc[1], lastAcc[2]};
  lastAcc[0] = uBit.accelerometer.getX();
  lastAcc[1] = uBit.accelerometer.getY();
  lastAcc[2] = uBit.accelerometer.getZ();
  int threshold = 100;
  if ((abs(lastAcc[0] - old[0]) > threshold) || (abs(lastAcc[1] - old[1]) > threshold) || (abs(lastAcc[2] - old[2]) > threshold))
  {
    // Moved
    gesture = gesture | (1 << 2);
  }
}

void ScratchMoreService::resetGesture()
{
  gesture = 0;
}

void ScratchMoreService::updateDigitalValues()
{
  digitalValues = 0;
  for (size_t i = 0; i < sizeof(digitalIn) / sizeof(digitalIn[0]); i++)
  {
    if (uBit.io.pin[digitalIn[i]].isInput())
    {
      digitalValues =
          digitalValues | (((uBit.io.pin[digitalIn[i]].getDigitalValue(PullUp) == 1 ? 0 : 1)) << digitalIn[i]);
    }
  }
}

void ScratchMoreService::updateAnalogValues()
{
  for (size_t i = 0; i < sizeof(analogIn) / sizeof(analogIn[0]); i++)
  {
    if (uBit.io.pin[analogIn[i]].isInput())
    {
      analogValues[i] = (uint16_t)uBit.io.pin[analogIn[i]].getAnalogValue();
    }
  }

  //// It will cause flickering LED.
  // uBit.display.disable();
  // analogValues[3] = (uint16_t)uBit.io.P3.getAnalogValue();
  // analogValues[4] = (uint16_t)uBit.io.P4.getAnalogValue();
  // analogValues[5] = (uint16_t)uBit.io.P10.getAnalogValue();
  // uBit.display.enable();
}

void ScratchMoreService::setInputMode(int pinIndex)
{
  uBit.io.pin[pinIndex].getAnalogValue(); // Configure the pin as input, but the value is not used.
}

void ScratchMoreService::setDigitalValue(int pinIndex, int value)
{
  uBit.io.pin[pinIndex].setDigitalValue(value);
}

void ScratchMoreService::setServoValue(int pinIndex, int value)
{
  uBit.io.pin[pinIndex].setServoValue(value);
}

void ScratchMoreService::setAnalogValue(int pinIndex, int value)
{
  uBit.io.pin[pinIndex].setAnalogValue(value);
}

void ScratchMoreService::composeDefaultData(uint8_t *buff)
{
  // Tilt value is sent as int16_t big-endian.
  int16_t tiltX = (int16_t)convertToTilt(uBit.accelerometer.getRollRadians());
  buff[0] = (tiltX >> 8) & 0xFF;
  buff[1] = tiltX & 0xFF;
  int16_t tiltY = (int16_t)convertToTilt(uBit.accelerometer.getPitchRadians());
  buff[2] = (tiltY >> 8) & 0xFF;
  buff[3] = tiltY & 0xFF;
  buff[4] = (uint8_t)buttonAState;
  buff[5] = (uint8_t)buttonBState;
  buff[6] = (uint8_t)((digitalValues >> 0) & 1);
  buff[7] = (uint8_t)((digitalValues >> 1) & 1);
  buff[8] = (uint8_t)((digitalValues >> 2) & 1);
  buff[9] = (uint8_t)gesture;
}

void ScratchMoreService::composeTxBufferA()
{
  composeDefaultData(txBufferA);

  // analog value (0 to 1024) is sent as uint16_t little-endian.
  memcpy(&(txBufferA[10]), &(analogValues[0]), 2);
  memcpy(&(txBufferA[12]), &(analogValues[1]), 2);
  memcpy(&(txBufferA[14]), &(analogValues[2]), 2);

  // compassHeading angle (0 - 359) is sent as uint16_t little-endian.
  uint16_t heading = (uint16_t)normalizeCompassHeading(uBit.compass.heading());
  memcpy(&(txBufferA[16]), &heading, 2);

  // level of light amount (0-255) is sent as uint8_t.
  txBufferA[18] = (uint8_t)uBit.display.readLightLevel();

  // More extension format.
  txBufferA[19] = 0x01;
}

void ScratchMoreService::composeTxBufferB()
{
  composeDefaultData(txBufferB);

  // More extension format.
  txBufferB[19] = 0x02;
}

/**
  * Notify data to Scratch3
  */
void ScratchMoreService::notify(MicroBitEvent)
{
  if (uBit.ble->getGapState().connected)
  {
    updateDigitalValues();
    updateAnalogValues();
    updateGesture();
    composeTxBufferA();
    uBit.ble->gattServer().notify(
        txCharacteristicHandle,
        (uint8_t *)&txBufferA,
        sizeof(txBufferA) / sizeof(txBufferA[0]));
    composeTxBufferB();
    uBit.ble->gattServer().notify(
        txCharacteristicHandle,
        (uint8_t *)&txBufferB,
        sizeof(txBufferB) / sizeof(txBufferB[0]));
    resetGesture();
  }
}

/**
 * Set value to Slots.
 */
void ScratchMoreService::setSlot(int slot, int value)
{
  // value (-32768 to 32767) is sent as int16_t little-endian.
  int16_t slotData = (int16_t)value;
  memcpy(&(txBufferB[10 + slot]), &slotData, 2);
}

const uint16_t ScratchMoreServiceUUID = 0xf005;

const uint8_t ScratchMoreServiceTxUUID[] = {
    0x52, 0x61, 0xda, 0x01, 0xfa, 0x7e, 0x42, 0xab, 0x85, 0x0b, 0x7c, 0x80, 0x22, 0x00, 0x97, 0xcc};

const uint8_t ScratchMoreServiceRxUUID[] = {
    0x52, 0x61, 0xda, 0x02, 0xfa, 0x7e, 0x42, 0xab, 0x85, 0x0b, 0x7c, 0x80, 0x22, 0x00, 0x97, 0xcc};
