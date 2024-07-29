#pragma once
#include <stdint.h>

// KTD20206/2027 LED Driver.
// Datasheet: https://www.kinet-ic.com/uploads/KTD2026-7-04h.pdf


// Configurable params:
// Define RGB2026_R, _G, _B, _K before including this header to configure
// the channel->color mapping to match your circuit.
#ifndef RGB2026_R
#define RGB2026_R RGB2026_D1
#endif
#ifndef RGB2026_G
#define RGB2026_G RGB2026_D2
#endif
#ifndef RGB2026_B
#define RGB2026_B RGB2026_D3
#endif
#ifndef RGB2026_K
#define RGB2026_K RGB2026_D4
#endif


namespace rgb2026 {
  // Ref Datasheet p. 17.
  // I2C address for KTD2026/KTD2027.
  static const uint8_t kI2CAddr = 0x30;
  // I2C address for KTD2026B/KTD2027B.
  static const uint8_t kI2CAddrB = 0x31;
  // I2C address for KTD2026C/KTD2027C.
  static const uint8_t kI2CAddrC = 0x32;

  enum eRamp {
    S_CURVE = 0,
    LINEAR
  };

  typedef uint8_t channel_t;
  static const channel_t kChOff  = 0x0;
  static const channel_t kChOn   = 0x1;
  static const channel_t kChPWM1 = 0x2;
  static const channel_t kChPWM2 = 0x3;

  // Convert a flash period in milliseconds to the closest binary
  // representation supported by the chip. The chip has 127 steps of 128ms.
  // The 128th step indicates a non-repeating signal.
  // Range supported is [128-16384]ms. Values outside of this range will
  // be clamped.
  // Ref Datasheet p.14
  uint8_t convertFlashPeriod(uint16_t flashPeriodMs);

  // Convert ramp times in milliseconds to the closest binary
  // representation supported by the chip.
  // The chip supports ramp times [16-7680]ms using ramp scaling.
  // However, ramp scaling cannot be set independently for ramp up/down. 
  // This function will find the Trise, Tfall, and scale settings that
  // best approximate the desired input. 
  // The output is the contents of Reg5 in the low 8 bits, and the ramp scale
  // setting (Reg0[6:5] in bits 10:9.
  // Ref Datasheet p.15
  uint16_t convertRampTime(uint16_t up, uint16_t down);

  // The current register on the chip maxes out at 192, and ignores the range
  // 193-255. So multiply the input color by 0.75.
  uint8_t convertColor(uint8_t color);

  // This class is a simple wrapper interface for I2C libraries.
  // An implementation based on Adafruit I2C is provided. If you are using a
  // different I2C abstraction, implement this interface.
  class I2CWrapper {
  public:
    virtual bool begin() = 0;

    virtual bool regWrite(uint8_t index, uint8_t data) = 0;
  };

  class RGB {
  public:

    // Track an LED controller using the provided I2C address and protocol
    // implementation. I2C addresses for the KTD2026/2027 line are provided
    // in kI2CAddrA/B/C constants.
    RGB(I2CWrapper* i2c);

    bool begin();

    // Set the ramp up/down shape.
    bool setRamp(eRamp ramp);

    // Set the LED color.
    bool setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t k=0);

    // Set the LED color with a hex code, in KRGB order.
    // Note the K channel is in the high byte, to allow setting
    // RGB colors with a standard 24 bit hex code.
    bool setColor(uint32_t hexColor);

    // Turn the LED on.
    bool on();

    // Turn the LED off.
    bool off();

    // Blink the LED with the given settings.
    // periodMs - full cycle period. Valid range [128-16384]ms
    // dutyCycle - Amount of time LED should be on during the period, 0-255.
    // rampUpMs - Time for the LED to ramp to full brightness. Counts as part
    //            of duty cycle. Valid range [16-7680]ms.
    // rampDownMS - Time for the LED to dim to off. Does not count as part of
    //              duty cycle. Valid range [16-7680]ms.
    bool blinkOn(uint16_t periodMs, uint8_t dutyCycle = 255U, uint16_t rampUpMs = 0U, uint16_t rampDownMs = 0U);

  private:
    bool writeReg(uint8_t index, uint8_t data);

    I2CWrapper* m_i2c;
    // Write thru cache of registers so we don't need to read from device.
    uint8_t m_regCache[10];
    bool m_inited;
  };

} // namespace rgb2026
