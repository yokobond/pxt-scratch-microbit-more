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
   * Get value to Slots.
   */
  int getSlot(int slot);

  /**
    * Callback. Invoked when any of our attributes are written via BLE.
    */
  void onDataWritten(const GattWriteCallbackParams *params);

  /**
   * Invocked when the bluetooth connected.
   */
  void onBLEConnected(MicroBitEvent e);

  void updateDigitalValues();
  void updateAnalogValues();

private:
  // Sending data to Scratch3.
  uint8_t txData[20];

  /**
   * Sending data buffer 01.
   */
  uint8_t txBuffer01[20];

  /**
   * Sending data buffer 02.
   */
  uint8_t txBuffer02[20];

  /**
   * Sending data buffer 03.
   */
  uint8_t txBuffer03[20];

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

  /**
   * Slots
   */
  int16_t slots[4];

  void setInputMode(int pinIndex);
  void setDigitalValue(int pinIndex, int value);
  void setAnalogValue(int pinIndex, int value);
  void setServoValue(int pinIndex, int angle, int range, int center);

  void onButtonChanged(MicroBitEvent);
  void onAccelerometerChanged(MicroBitEvent);

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

  enum ScratchBLECommand
  {
    CMD_PIN_CONFIG = 0x80,
    CMD_DISPLAY_TEXT = 0x81,
    CMD_DISPLAY_LED = 0x82,
    CMD_PIN_INPUT = 0x90,
    CMD_PIN_OUTPUT = 0x91,
    CMD_PIN_PWM = 0x92,
    CMD_PIN_SERVO = 0x93,
    CMD_SLOT_VALUE = 0xA0
  };
};

#endif
