/**
  * Class definition for the Scratch MicroBit More Service.
  * Provides a BLE service to remotely controll Micro:bit from Scratch3.
  */
#include "MbitMoreService.h"

int gpio[] = {0, 1, 2, 8, 13, 14, 15, 16};
int analogIn[] = {0, 1, 2};

/**
  * Constructor.
  * Create a representation of the MbitMoreService
  * @param _uBit The instance of a MicroBit runtime.
  */
MbitMoreService::MbitMoreService(MicroBit &_uBit)
    : uBit(_uBit)
{
  // Calibrate the compass before start bluetooth service.
  if (!uBit.compass.isCalibrated())
  {
    uBit.compass.calibrate();
  }

  // Initialize pin configuration.
  for (size_t i = 0; i < sizeof(gpio) / sizeof(gpio[0]); i++)
  {
    setPullMode(gpio[i], PinMode::PullUp);
  }

  // Initialize microbit more protocol.
  mbitMoreProtocol = 0;

  // Create the data structures that represent each of our characteristics in Soft Device.
  GattCharacteristic txCharacteristic(
      MBIT_MORE_BASIC_TX,
      (uint8_t *)&txData,
      0,
      sizeof(txData),
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

  GattCharacteristic rxCharacteristic(
      MBIT_MORE_BASIC_RX,
      (uint8_t *)&rxBuffer,
      0,
      sizeof(rxBuffer),
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE);

  // Set default security requirements
  txCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  rxCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

  GattCharacteristic *characteristics[] = {&txCharacteristic, &rxCharacteristic};
  GattService service(
      MBIT_MORE_BASIC_SERVICE, characteristics,
      sizeof(characteristics) / sizeof(GattCharacteristic *));

  uBit.ble->addService(service);

  txCharacteristicHandle = txCharacteristic.getValueHandle();
  rxCharacteristicHandle = rxCharacteristic.getValueHandle();

  // Write initial value.
  txBuffer01[DATA_FORMAT_INDEX] = MBitMoreDataFormat::MIX_01;
  txBuffer02[DATA_FORMAT_INDEX] = MBitMoreDataFormat::MIX_02;
  txBuffer03[DATA_FORMAT_INDEX] = MBitMoreDataFormat::MIX_03;

  uBit.ble->gattServer().write(
      txCharacteristicHandle,
      (uint8_t *)&txData,
      sizeof(txData));

  // Advertise this service.
  const uint16_t uuid16_list[] = {MBIT_MORE_BASIC_SERVICE};
  uBit.ble->gap().accumulateAdvertisingPayload(GapAdvertisingData::INCOMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));

  // Mbit More Service
  // Create the data structures that represent each of our characteristics in Soft Device.

  configChar = new GattCharacteristic(
      MBIT_MORE_CONFIG,
      (uint8_t *)&configBuffer,
      0,
      sizeof(configBuffer),
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE);
  ioChar = new GattCharacteristic(
      MBIT_MORE_IO,
      (uint8_t *)&ioBuffer,
      0,
      sizeof(ioBuffer),
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);
  analogInChar = new GattCharacteristic(
      MBIT_MORE_ANALOG_IN,
      (uint8_t *)&analogInBuffer,
      0,
      sizeof(analogInBuffer),
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);
  analogInChar->setReadAuthorizationCallback(this, &MbitMoreService::onReadAnalogIn);
  sensorsChar = new GattCharacteristic(
      MBIT_MORE_SENSORS,
      (uint8_t *)&sensorsBuffer,
      0,
      sizeof(sensorsBuffer),
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);
  sharedDataChar = new GattCharacteristic(
      MBIT_MORE_SHARED_DATA,
      (uint8_t *)&sharedBuffer,
      0,
      sizeof(sharedBuffer),
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE);

  // Set default security requirements
  analogInChar->requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  configChar->requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  ioChar->requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  sensorsChar->requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  sharedDataChar->requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

  GattCharacteristic *mbitMoreChars[] = {
      configChar,
      ioChar,
      analogInChar,
      sensorsChar,
      sharedDataChar};
  GattService mbitMoreService(
      MBIT_MORE_SERVICE, mbitMoreChars,
      sizeof(mbitMoreChars) / sizeof(GattCharacteristic *));

  uBit.ble->addService(mbitMoreService);

  analogInCharHandle = analogInChar->getValueHandle();
  configCharHandle = configChar->getValueHandle();
  ioCharHandle = ioChar->getValueHandle();
  sensorsCharHandle = sensorsChar->getValueHandle();
  sharedDataCharHandle = sharedDataChar->getValueHandle();

  // Initialize buffers.
  sharedBuffer[DATA_FORMAT_INDEX] = MBitMoreDataFormat::SHARED_DATA;

  // Advertise this service.
  const uint8_t *mbitMoreServices[] = {MBIT_MORE_SERVICE};
  uBit.ble->gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS, (uint8_t *)mbitMoreServices, sizeof(mbitMoreServices));

  // Setup callbacks for events.
  uBit.ble->onDataWritten(this, &MbitMoreService::onDataWritten);

  uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, this, &MbitMoreService::onBLEConnected, MESSAGE_BUS_LISTENER_IMMEDIATE);

  uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, this, &MbitMoreService::onButtonChanged, MESSAGE_BUS_LISTENER_IMMEDIATE);
  uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_EVT_ANY, this, &MbitMoreService::onButtonChanged, MESSAGE_BUS_LISTENER_IMMEDIATE);
  uBit.messageBus.listen(MICROBIT_ID_GESTURE, MICROBIT_EVT_ANY, this, &MbitMoreService::onGestureChanged, MESSAGE_BUS_LISTENER_IMMEDIATE);
}

/**
  * Callback. Invoked when AnalogIn is read via BLE.
  */
void MbitMoreService::onReadAnalogIn(GattReadAuthCallbackParams *authParams)
{
  updateAnalogValues();
  writeAnalogIn();
}

/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MbitMoreService::onDataWritten(const GattWriteCallbackParams *params)
{
  uint8_t *data = (uint8_t *)params->data;

  if (params->handle == rxCharacteristicHandle && params->len > 0)
  {
    if (data[0] == ScratchBLECommand::CMD_DISPLAY_TEXT)
    {
      char text[params->len];
      memcpy(text, &(data[1]), (params->len) - 1);
      text[(params->len) - 1] = '\0';
      ManagedString mstr(text);
      uBit.display.scroll(mstr, 120); // Interval is corresponding with the Scratch extension.
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
    else if (data[0] == ScratchBLECommand::CMD_PIN_PULL_UP)
    {
      setPullMode(data[1], PinMode::PullUp);
    }
    else if (data[0] == ScratchBLECommand::CMD_PIN_PULL_DOWN)
    {
      setPullMode(data[1], PinMode::PullDown);
    }
    else if (data[0] == ScratchBLECommand::CMD_PIN_TOUCH)
    {
      setPinModeTouch(data[1]);
    }
    else if (data[0] == ScratchBLECommand::CMD_PIN_OUTPUT)
    {
      setDigitalValue(data[1], data[2]);
    }
    else if (data[0] == ScratchBLECommand::CMD_PIN_PWM)
    {
      // value is read as uint16_t little-endian.
      int value;
      memcpy(&value, &(data[2]), 2);
      setAnalogValue(data[1], value);
    }
    else if (data[0] == ScratchBLECommand::CMD_PIN_SERVO)
    {
      // angle is read as uint16_t little-endian.
      uint16_t angle;
      memcpy(&angle, &(data[2]), 2);
      // range is read as uint16_t little-endian.
      uint16_t range;
      memcpy(&range, &(data[4]), 2);
      // center is read as uint16_t little-endian.
      uint16_t center;
      memcpy(&center, &(data[6]), 2);
      setServoValue((int)data[1], (int)angle, (int)range, (int)center);
    }
    else if (data[0] == ScratchBLECommand::CMD_SHARED_DATA_SET)
    {
      // value is read as int16_t little-endian.
      int16_t value;
      memcpy(&value, &(data[2]), 2);
      sharedData[data[1]] = value;
    }
    else if (data[0] == ScratchBLECommand::CMD_PROTOCOL_SET)
    {
      mbitMoreProtocol = data[1];
    }
  }
  if (params->handle == configCharHandle && params->len > 0)
  {
    // Shold be implemented later.
  }
}

/**
  * Button update callback
  */
void MbitMoreService::onButtonChanged(MicroBitEvent e)
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

void MbitMoreService::onGestureChanged(MicroBitEvent e)
{
  if (e.value == MICROBIT_ACCELEROMETER_EVT_SHAKE)
  {
    gesture = gesture | 1;
  }
  if (e.value == MICROBIT_ACCELEROMETER_EVT_FREEFALL)
  {
    gesture = gesture | 1 << 1;
  }
}

/**
 * Normalize angle in upside down.
 */
int MbitMoreService::normalizeCompassHeading(int heading)
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
int MbitMoreService::convertToTilt(float radians)
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

void MbitMoreService::updateGesture()
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

void MbitMoreService::resetGesture()
{
  gesture = 0;
}

void MbitMoreService::updateDigitalValues()
{
  digitalValues = 0;
  for (size_t i = 0; i < sizeof(gpio) / sizeof(gpio[0]); i++)
  {
    if (uBit.io.pin[gpio[i]].isInput())
    {
      digitalValues =
          digitalValues | (((uBit.io.pin[gpio[i]].getDigitalValue(pullMode[gpio[i]]) == 1 ? 0 : 1)) << gpio[i]);
    }
  }
}

void MbitMoreService::updateAnalogValues()
{
  for (size_t i = 0; i < sizeof(analogIn) / sizeof(analogIn[0]); i++)
  {
    if (uBit.io.pin[analogIn[i]].isInput())
    {
      uBit.io.pin[analogIn[i]].setPull(PinMode::PullNone);
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

void MbitMoreService::updateLightSensor()
{
  lightLevel = uBit.display.readLightLevel();
}

void MbitMoreService::updateAccelerometer()
{
  acceleration[0] = -uBit.accelerometer.getX(); // Face side is positive in Z-axis.
  acceleration[1] = uBit.accelerometer.getY();
  acceleration[2] = -uBit.accelerometer.getZ(); // Face side is positive in Z-axis.
  rotation[0] = uBit.accelerometer.getPitchRadians();
  rotation[1] = uBit.accelerometer.getRollRadians();
}

void MbitMoreService::updateMagnetometer()
{
  compassHeading = uBit.compass.heading();
  magneticForce[0] = uBit.compass.getX();
  magneticForce[1] = uBit.compass.getY();
  magneticForce[2] = uBit.compass.getZ();
}

void MbitMoreService::setPullMode(int pinIndex, PinMode pull)
{
  uBit.io.pin[pinIndex].getDigitalValue(pull);
  pullMode[pinIndex] = pull;
}

void MbitMoreService::setDigitalValue(int pinIndex, int value)
{
  uBit.io.pin[pinIndex].setDigitalValue(value);
}

void MbitMoreService::setAnalogValue(int pinIndex, int value)
{
  uBit.io.pin[pinIndex].setAnalogValue(value);
}

void MbitMoreService::setServoValue(int pinIndex, int angle, int range, int center)
{
  uBit.io.pin[pinIndex].setServoValue(angle, range, center);
}

void MbitMoreService::setPinModeTouch(int pinIndex)
{
  uBit.io.pin[pinIndex].isTouched(); // Configure to touch mode then the return value is not used.
}

void MbitMoreService::composeDefaultData(uint8_t *buff)
{
  // Tilt value is sent as int16_t big-endian.
  int16_t tiltX = (int16_t)convertToTilt(rotation[1]);
  buff[0] = (tiltX >> 8) & 0xFF;
  buff[1] = tiltX & 0xFF;
  int16_t tiltY = (int16_t)convertToTilt(rotation[0]);
  buff[2] = (tiltY >> 8) & 0xFF;
  buff[3] = tiltY & 0xFF;
  buff[4] = (uint8_t)buttonAState;
  buff[5] = (uint8_t)buttonBState;
  buff[6] = (uint8_t)((digitalValues >> 0) & 1);
  buff[7] = (uint8_t)((digitalValues >> 1) & 1);
  buff[8] = (uint8_t)((digitalValues >> 2) & 1);
  buff[9] = (uint8_t)gesture;
}

void MbitMoreService::composeTxBuffer01()
{
  composeDefaultData(txBuffer01);

  updateAnalogValues();
  // analog value (0 to 1024) is sent as uint16_t little-endian.
  memcpy(&(txBuffer01[10]), &(analogValues[0]), 2);
  memcpy(&(txBuffer01[12]), &(analogValues[1]), 2);
  memcpy(&(txBuffer01[14]), &(analogValues[2]), 2);

  updateMagnetometer();
  // compassHeading angle (0 - 359) is sent as uint16_t little-endian.
  uint16_t heading = (uint16_t)normalizeCompassHeading(compassHeading);
  memcpy(&(txBuffer01[16]), &heading, 2);

  updateLightSensor();
  // level of light amount (0-255) is sent as uint8_t.
  txBuffer01[18] = (uint8_t)lightLevel;
}

void MbitMoreService::composeTxBuffer02()
{
  composeDefaultData(txBuffer02);

  txBuffer02[18] = 0;
  for (size_t i = 0; i < 8; i++)
  {
    txBuffer02[18] = txBuffer02[18] | (uint8_t)(((digitalValues >> gpio[i]) & 1) << i);
  }
}

void MbitMoreService::composeTxBuffer03()
{
  composeDefaultData(txBuffer03);

  updateMagnetometer();
  // Magnetic field strength [micro teslas] is sent as uint16_t little-endian in 03:10.
  uint16_t magStrength = (uint16_t)(uBit.compass.getFieldStrength() / 1000);
  memcpy(&(txBuffer03[10]), &magStrength, 2);

  int16_t data;
  // Acceleration X [milli-g] is sent as int16_t little-endian in 03:12.
  data = (int16_t)(acceleration[0]);
  memcpy(&(txBuffer03[12]), &data, 2);
  // Acceleration Y [milli-g] is sent as int16_t little-endian in 03:14.
  data = (int16_t)(acceleration[1]);
  memcpy(&(txBuffer03[14]), &data, 2);
  // Acceleration Z [milli-g] is sent as int16_t little-endian in 03:16.
  data = (int16_t)(acceleration[2]);
  memcpy(&(txBuffer03[16]), &data, 2);
}

/**
  * Write analog input values on BLE.
  */
void MbitMoreService::writeAnalogIn()
{
  // analog value (0 to 1023) is sent as uint16_t little-endian.
  memcpy(&(analogInBuffer[0]), &(analogValues[0]), 2);
  memcpy(&(analogInBuffer[2]), &(analogValues[1]), 2);
  memcpy(&(analogInBuffer[4]), &(analogValues[2]), 2);
  uBit.ble->gattServer().write(
      analogInCharHandle,
      (uint8_t *)&analogInBuffer,
      sizeof(analogInBuffer) / sizeof(analogInBuffer[0]));
}

/**
  * Notify shared data to Scratch3
  */
void MbitMoreService::notifySharedData()
{
  for (size_t i = 0; i < sizeof(sharedData) / sizeof(sharedData[0]); i++)
  {
    memcpy(&(sharedBuffer[(i * 2)]), &sharedData[i], 2);
  }
  uBit.ble->gattServer().notify(
      sharedDataCharHandle,
      (uint8_t *)&sharedBuffer,
      sizeof(sharedBuffer) / sizeof(sharedBuffer[0]));
}

/**
 * Notify default micro:bit data to Scratch.
 */
void MbitMoreService::notifyDefaultData()
{
  composeDefaultData(txData);
  uBit.ble->gattServer().notify(
      txCharacteristicHandle,
      (uint8_t *)&txData,
      sizeof(txData) / sizeof(txData[0]));
}

/**
  * Notify data to Scratch3
  */
void MbitMoreService::notify()
{
  if (uBit.ble->gap().getState().connected)
  {
    updateGesture();
    if (mbitMoreProtocol == 0)
    {
      updateDigitalValues();
      updateAccelerometer();
      switch (txDataFormat)
      {
      case 1:
        composeTxBuffer01();
        uBit.ble->gattServer().notify(
            txCharacteristicHandle,
            (uint8_t *)&txBuffer01,
            sizeof(txBuffer01) / sizeof(txBuffer01[0]));
        break;

      case 2:
        composeTxBuffer02();
        uBit.ble->gattServer().notify(
            txCharacteristicHandle,
            (uint8_t *)&txBuffer02,
            sizeof(txBuffer02) / sizeof(txBuffer02[0]));
        break;

      case 3:
        composeTxBuffer03();
        uBit.ble->gattServer().notify(
            txCharacteristicHandle,
            (uint8_t *)&txBuffer03,
            sizeof(txBuffer03) / sizeof(txBuffer03[0]));
        break;

      default:
        break;
      }
      txDataFormat++;
      if (txDataFormat > 3)
        txDataFormat = 1;
    }
    else
    {
      notifyDefaultData();
    }
    resetGesture();
  }
  else
  {
    txDataFormat = 1;
    mbitMoreProtocol = 0;
    displayFriendlyName();
  }
}

/**
 * Set value to shared data.
 * shared data (0, 1, 2, 3)
 */
void MbitMoreService::setSharedData(int index, int value)
{
  // value (-32768 to 32767) is sent as int16_t little-endian.
  int16_t data = (int16_t)value;
  memcpy(&(txBuffer02[10 + (index * 2)]), &data, 2);
  sharedData[index] = data;
  notifySharedData();
}

/**
 * Get value of a shared data.
 * shared data (0, 1, 2, 3)
 */
int MbitMoreService::getSharedData(int index)
{
  return (int)(sharedData[index]);
}

void MbitMoreService::onBLEConnected(MicroBitEvent _e)
{
  uBit.display.stopAnimation(); // To stop display friendly name.
}

/**
 * Update sensors.
 */
void MbitMoreService::update()
{
  updateDigitalValues();
  writeIo();
  updateLightSensor();
  updateAccelerometer();
  updateMagnetometer();
  writeSensors();
}

/**
  * Write IO characteristics.
  */
void MbitMoreService::writeIo()
{
  memcpy(&(ioBuffer[0]), &digitalValues, 4);

  uBit.ble->gattServer().write(
      ioCharHandle,
      (uint8_t *)&ioBuffer,
      sizeof(ioBuffer) / sizeof(ioBuffer[0]));
}

/**
  * Write shared data characteristics.
  */
void MbitMoreService::writeSharedData()
{
  for (size_t i = 0; i < sizeof(sharedData) / sizeof(sharedData[0]); i++)
  {
    memcpy(&(sharedBuffer[(i * 2)]), &sharedData[i], 2);
  }

  uBit.ble->gattServer().write(
      sharedDataCharHandle,
      (uint8_t *)&sharedBuffer,
      sizeof(sharedBuffer) / sizeof(sharedBuffer[0]));
}

/**
  * Write data of all sensors to the characteristic.
  */
void MbitMoreService::writeSensors()
{
  // Accelerometer
  int16_t acc;
  // Acceleration X [milli-g] is sent as int16_t little-endian.
  acc = (int16_t)acceleration[0];
  memcpy(&(sensorsBuffer[0]), &acc, 2);
  // Acceleration Y [milli-g] is sent as int16_t little-endian.
  acc = (int16_t)acceleration[1];
  memcpy(&(sensorsBuffer[2]), &acc, 2);
  // Acceleration Z [milli-g] is sent as int16_t little-endian.
  acc = (int16_t)acceleration[2];
  memcpy(&(sensorsBuffer[4]), &acc, 2);

  int16_t rot;
  // Pitch (radians / 1000) is sent as int16_t little-endian [6..7].
  rot = (int16_t)(rotation[0] * 1000);
  memcpy(&(sensorsBuffer[6]), &rot, 2);
  // Roll (radians / 1000) is sent as int16_t little-endian [8..9].
  rot = (int16_t)(rotation[1] * 1000);
  memcpy(&(sensorsBuffer[8]), &rot, 2);

  // Magnetometer
  uint16_t heading = (uint16_t)normalizeCompassHeading(compassHeading);
  memcpy(&(sensorsBuffer[10]), &heading, 2);

  int16_t force;
  // Magnetic force X (micro-teslas) is sent as uint16_t little-endian [2..3].
  force = (int16_t)(magneticForce[0] / 1000);
  memcpy(&(sensorsBuffer[12]), &force, 2);
  // Magnetic force Y (micro-teslas) is sent as uint16_t little-endian [4..5].
  force = (int16_t)(magneticForce[1] / 1000);
  memcpy(&(sensorsBuffer[14]), &force, 2);
  // Magnetic force Z (micro-teslas) is sent as uint16_t little-endian [6..7].
  force = (int16_t)(magneticForce[2] / 1000);
  memcpy(&(sensorsBuffer[16]), &force, 2);

  // Light sensor
  sensorsBuffer[18] = (uint8_t)lightLevel;

  uBit.ble->gattServer().write(
      sensorsCharHandle,
      (uint8_t *)&sensorsBuffer,
      sizeof(sensorsBuffer) / sizeof(sensorsBuffer[0]));
}

void MbitMoreService::displayFriendlyName()
{
  ManagedString suffix(" MORE! ");
  uBit.display.scrollAsync(uBit.getName() + suffix, 120);
}

const uint16_t MBIT_MORE_BASIC_SERVICE = 0xf005;
const uint8_t MBIT_MORE_BASIC_TX[] = {0x52, 0x61, 0xda, 0x01, 0xfa, 0x7e, 0x42, 0xab, 0x85, 0x0b, 0x7c, 0x80, 0x22, 0x00, 0x97, 0xcc};
const uint8_t MBIT_MORE_BASIC_RX[] = {0x52, 0x61, 0xda, 0x02, 0xfa, 0x7e, 0x42, 0xab, 0x85, 0x0b, 0x7c, 0x80, 0x22, 0x00, 0x97, 0xcc};

const uint8_t MBIT_MORE_SERVICE[] = {0xa6, 0x2d, 0x57, 0x4e, 0x1b, 0x34, 0x40, 0x92, 0x8d, 0xee, 0x41, 0x51, 0xf6, 0x3b, 0x28, 0x65};
const uint8_t MBIT_MORE_CONFIG[] = {0xa6, 0x2d, 0x00, 0x01, 0x1b, 0x34, 0x40, 0x92, 0x8d, 0xee, 0x41, 0x51, 0xf6, 0x3b, 0x28, 0x65};
const uint8_t MBIT_MORE_IO[] = {0xa6, 0x2d, 0x00, 0x02, 0x1b, 0x34, 0x40, 0x92, 0x8d, 0xee, 0x41, 0x51, 0xf6, 0x3b, 0x28, 0x65};
const uint8_t MBIT_MORE_ANALOG_IN[] = {0xa6, 0x2d, 0x00, 0x03, 0x1b, 0x34, 0x40, 0x92, 0x8d, 0xee, 0x41, 0x51, 0xf6, 0x3b, 0x28, 0x65};
const uint8_t MBIT_MORE_SENSORS[] = {0xa6, 0x2d, 0x00, 0x04, 0x1b, 0x34, 0x40, 0x92, 0x8d, 0xee, 0x41, 0x51, 0xf6, 0x3b, 0x28, 0x65};
const uint8_t MBIT_MORE_SHARED_DATA[] = {0xa6, 0x2d, 0x00, 0x10, 0x1b, 0x34, 0x40, 0x92, 0x8d, 0xee, 0x41, 0x51, 0xf6, 0x3b, 0x28, 0x65};
