#ifndef SCRATCH_MORE_SERVICE_H
#define SCRATCH_MORE_SERVICE_H

#include "MicroBit.h"

#define SCRATCH_MORE_ID 2000

#define SCRATCH_MORE_EVT_NOTIFY 1

/**
 * Position of data format in a value holder.
 */
#define DATA_FORMAT_INDEX 19

// UUIDs for our service and characteristics
extern const uint16_t MBIT_MORE_BASIC_SERVICE;
extern const uint8_t MBIT_MORE_BASIC_TX[];
extern const uint8_t MBIT_MORE_BASIC_RX[];
extern const uint8_t MBIT_MORE_SERVICE[];
extern const uint8_t MBIT_MORE_EVENT[];
extern const uint8_t MBIT_MORE_IO[];
extern const uint8_t MBIT_MORE_SENSORS[];
extern const uint8_t MBIT_MORE_SHARED_DATA[];
extern const uint8_t MBIT_MORE_ANALOG_IN[];

/**
  * Class definition for a MicroBitMore Service.
  * Provides a BLE service to remotely read the state of sensors from Scratch3.
  */
class MbitMoreService
{
public:
  /**
    * Constructor.
    * Create a representation of the MbitMoreService
    * @param _uBit The instance of a MicroBit runtime.
    */
  MbitMoreService(MicroBit &_uBit);

  void initConfiguration();

  /**
    * Notify data to Scratch3.
    */
  void notify();
  void notifyDefaultData();
  void notifySharedData();

  /**
   * Set value to Slots.
   */
  void setSharedData(int index, int value);

  /**
   * Get value to Slots.
   */
  int getSharedData(int index);

  /**
   * Callback. Invoked when AnalogIn is read via BLE.
   */
  void onReadAnalogIn(GattReadAuthCallbackParams *authParams);

  /**
   * Callback. Invoked when any of our attributes are written via BLE.
   */
  void onDataWritten(const GattWriteCallbackParams *params);

  /**
   * Invocked when the bluetooth connected.
   */
  void onBLEConnected(MicroBitEvent e);

  /**
   * Invocked when the bluetooth disconnected.
   */
  void onBLEDisconnected(MicroBitEvent e);

  /**
   * Callback. Invoked when a pin event sent.
   */
  void onPinEvent(MicroBitEvent evt);

  void update();

  void updateDigitalValues();
  void updatePowerVoltage();
  void updateAnalogValues();
  void updateLightSensor();
  void updateAccelerometer();
  void updateMagnetometer();

  void writeIo();
  void writeAnalogIn();
  void writeSensors();
  void writeSharedData();

private:

  // Sending data to Scratch3.
  uint8_t txData[20];

  // Recieving buffer from Scratch3.
  uint8_t rxBuffer[10];

  // Config buffer set by Scratch.
  uint8_t eventBuffer[20];

  // Sending data of IO to Scratch.
  uint8_t ioBuffer[4];

  // Sending data of analog input to Scratch.
  uint8_t analogInBuffer[20];

  // Sending data of all sensors to Scratch.
  uint8_t sensorsBuffer[20];

  // Shared data with Scratch.
  uint8_t sharedBuffer[20];

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

  /**
   * Light level value from 0 to 255.
   */
  int lightLevel;

  int lightSensingDuration;

  /**
   * Acceleration value [x, y, z] in milli-g.
   */
  int acceleration[6];

  /**
   * Rotation value [pitch, roll] in radians.
   */
  float rotation[2];

  /**
   * Magnetic force [x, y, z] in 1000 * micro-teslas.
   */
  int magneticForce[3];

  /**
   * Shared data
   */
  int16_t sharedData[4];

  /**
   * Protocol of microbit more.
   */
  int mbitMoreProtocol;

  /**
   * Current mode of all pins.
   */
  PinMode pullMode[21];
  
  /**
   * Voltage of the power supply in [mV]
   */
  int powerVoltage;


  void listenPinEventOn(int pinIndex, int eventType);
  void setPullMode(int pinIndex, PinMode pull);
  void setDigitalValue(int pinIndex, int value);
  void setAnalogValue(int pinIndex, int value);
  void setServoValue(int pinIndex, int angle, int range, int center);
  void setPinModeTouch(int pinIndex);
  void setLightSensingDuration(int duration);

  void onButtonChanged(MicroBitEvent);
  void onGestureChanged(MicroBitEvent);

  void updateGesture(void);
  void resetGesture(void);

  int normalizeCompassHeading(int heading);
  int convertToTilt(float radians);

  void composeDefaultData(uint8_t *buff);
  void composeTxBuffer01(void);
  void composeTxBuffer02(void);
  void composeTxBuffer03(void);

  void displayFriendlyName();

  // microbit runtime instance
  MicroBit &uBit;

  // Handles to access each characteristic when they are held by Soft Device.
  GattAttribute::Handle_t txCharacteristicHandle;
  GattAttribute::Handle_t rxCharacteristicHandle;

  GattCharacteristic *eventChar;
  GattCharacteristic *ioChar;
  GattCharacteristic *analogInChar;
  GattCharacteristic *sensorsChar;
  GattCharacteristic *sharedDataChar;

  GattAttribute::Handle_t eventCharHandle;
  GattAttribute::Handle_t ioCharHandle;
  GattAttribute::Handle_t sensorsCharHandle;
  GattAttribute::Handle_t sharedDataCharHandle;

  enum ScratchBLECommand
  {
    CMD_PIN_CONFIG = 0x80,
    CMD_DISPLAY_TEXT = 0x81,
    CMD_DISPLAY_LED = 0x82,
    CMD_PROTOCOL = 0x90,
    CMD_PIN = 0x91,
    CMD_SHARED_DATA = 0x92,
    CMD_LIGHT_SENSING = 0x93
  };

  enum MBitMorePinCommand
  {
    SET_OUTPUT = 0x01,
    SET_PWM = 0x02,
    SET_SERVO = 0x03,
    SET_PULL = 0x04,
    SET_EVENT = 0x05,
    SET_TOUCH = 0x06,
  };

  enum MBitMorePinMode
  {
    PullNone = 0,
    PullUp = 1,
    PullDown = 2,
  };

  enum MBitMoreDataFormat
  {
    MIX_01 = 0x01,
    MIX_02 = 0x02,
    MIX_03 = 0x03,
    SHARED_DATA = 0x11,
    EVENT = 0x12,
  };

  enum MBitMorePinEventType
  {
    NONE = 0,
    ON_EDGE = 1,
    ON_PULSE = 2,
    ON_TOUCH = 3
  };
};

#endif
