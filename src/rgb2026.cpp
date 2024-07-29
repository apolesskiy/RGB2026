#include "rgb2026.h"
#include "debug.h"

#define RGB2026_D1 0
#define RGB2026_D2 1
#define RGB2026_D3 2
#define RGB2026_D4 3

namespace rgb2026
{

  static const uint8_t kDefaultRegisters[10] = {
    // [7]   Factory test - must be 0
    // [6:5] Ramp time scaling 
    // [4:3] Power control
    // [2:0] Reset control
    0b00011000,
    // [7]   Ramp shape - 0=s-curve, 1=linear
    // [6:0] Flash period
    0b00000000,
    // Flash timer 1
    0b10000000,
    // Flash timer 2
    0b10000000,
    // [7:6] D4 channel setting (unused on 2026)
    // [5:4] D3 channel setting
    // [3:2] D2 channel setting
    // [1:0] D1 channel setting
    0b00000000,
    // [7:4] Tfall
    // [3:0] Trise
    0b00000000,
    // D1 max brightness
    0b00000000,
    // D2 max brightness
    0b00000000,
    // D3 max brightness
    0b00000000,
    // D4 max brightness (unused on 2026)
    0b00000000
  };

  // Convert a flash period in milliseconds to the closest binary
  // representation supported by the chip. The chip has 127 steps of 128ms.
  // However, step 0 is 128ms and step 1 is 384ms, for a gap of 256ms.
  // The 128th step indicates a non-repeating signal.
  // Range supported is [128-16384]ms. Values outside of this range will
  // be clamped.
  // Ref Datasheet p.14
  uint8_t convertFlashPeriod(uint16_t flashPeriodMs)
  {
    if (flashPeriodMs > 16384)
    {
      return 126U;
    }
    if (flashPeriodMs < 256)
    {
      return 0U;
    }
    if (flashPeriodMs < 448)
    {
      return 1U;
    }
    uint16_t adj = flashPeriodMs - 384U;
    uint8_t rem = (uint8_t)adj & 0x7F; // adj % 128
    adj = adj >> 7;           // adj / 128
    if (rem & 0x40)
    { // 128 > rem > 63
      return adj + 2;
    }
    return adj + 1;
  }

  // Convert ramp times in milliseconds to the closest binary
  // representation supported by the chip.
  // The chip supports ramp times [16-7680]ms using ramp scaling.
  // However, ramp scaling cannot be set independently for ramp up/down.
  // This function will find the Trise, Tfall, and scale settings that
  // best approximate the desired input.
  // The output is the contents of Reg5 in the low 8 bits, and the ramp scale
  // setting (Reg0[6:5] in bits 10:9.
  // Ref Datasheet p.15
  uint16_t convertRampTime(uint16_t up, uint16_t down)
  {
    // If ramps are zero, set to default.
    if (!(up | down))
    {
      return 0;
    }

    uint16_t out = 0U;
    uint16_t interval = 128U;
    if (up > 7680)
    {
      up = 7680;
    }
    if (down > 7680)
    {
      down = 7680;
    }
    // 4x slower
    if (up > 3840 || down > 3840)
    {
      interval = 512U;
      out = 0b10;
      // 2x slower
    }
    else if (up > 1920 || down > 1920)
    {
      interval = 256U;
      out = 0b01;
    }
    // 8x faster. Allow "rounding down" to 240.
    // Rounding up to 256 moves into the 1x band.
    if (up < 248 && down < 248)
    {
      interval = 16U;
      out = 0b11;
    }
    if (up < interval)
    {
      up = interval;
    }
    if (down < interval)
    {
      down = interval;
    }
    // Integer division. Slow but ok, this should not be running every frame.
    uint8_t tfall = down / interval;
    if (down % interval >= (interval >> 1))
    {
      tfall++;
    }
    // push Tfall.
    out = out << 4;
    out += tfall;

    uint8_t trise = up / interval;
    if (up % interval > (interval >> 1))
    {
      trise++;
    }
    out = out << 4;
    return out + trise;
  }

  // The current register on the chip maxes out at 192, and ignores the range
  // 193-255. So multiply the input color by 0.75.
  uint8_t convertColor(uint8_t color)
  {
    if (color == 255) {
      return 192;
    }
    uint16_t actualColor = color * 3;
    return (uint8_t)(actualColor >> 2); // actualColor / 4
  }

  // Track an LED controller using the provided I2C address and protocol
  // implementation. I2C addresses for the KTD2026/2027 line are provided
  // in kI2CAddrA/B/C constants.
  RGB::RGB(I2CWrapper *i2c)
      : m_i2c(i2c)
      , m_inited(false)
  {
  }

  bool RGB::writeReg(uint8_t index, uint8_t data) 
  {
    if (m_i2c->regWrite(index, data)) {
      //if success, cache
      m_regCache[index] = data;
      return true;
    }
    return false;
  }

  bool RGB::begin()
  {
    if (!m_i2c->begin()) {
      return false;
    }

    // Establish ground state.
    bool out = true;
    for( uint8_t i = 0; i < 10; i++) {
      out = out && writeReg(i, kDefaultRegisters[i]);
    }
    m_inited = out;
    DEBUGPRINT("RGB2026: Init: ");
    DEBUGPRINTLN(out);
    return out;
  }

  // Set the ramp up/down shape.
  bool RGB::setRamp(eRamp ramp)
  {
    if (!m_inited)
    {
      return false;
    }
    uint8_t reg1 = (m_regCache[1] & 0x7F) | (((uint8_t)(ramp == eRamp::LINEAR)) << 7);
    return writeReg(1, reg1);
  }

  // Set the LED color.
  bool RGB::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t k)
  {
    if (!m_inited)
    {
      return false;
    }
    bool out = true;
    out &= writeReg(RGB2026_R + 6, convertColor(r));
    out &= writeReg(RGB2026_G + 6, convertColor(g));
    out &= writeReg(RGB2026_B + 6, convertColor(b));
    out &= writeReg(RGB2026_K + 6, convertColor(k));
    return out;
  }

  bool RGB::setColor(uint32_t hexColor) {
    return setColor(
      (uint8_t)((hexColor & 0x00FF0000) >> 16),
      (uint8_t)((hexColor & 0x0000FF00) >> 8),
      (uint8_t)(hexColor & 0x000000FF),
      (uint8_t)((hexColor & 0xFF000000) >> 24)
      );
  }

  // Turn the LED on.
  bool RGB::on()
  {
    if (!m_inited)
    {
      return false;
    }
    return writeReg(4, 0x55);
  }

  // Turn the LED off.
  bool RGB::off()
  {
    if (!m_inited)
    {
      return false;
    }
    return writeReg(4, 0x00);
  }

  // Blink the LED with the given settings.
  // periodMs - full cycle period. Valid range [128-16384]ms
  // dutyCycle - Amount of time LED should be on during the period, 0-255.
  // rampUpMs - Time for the LED to ramp to full brightness. Counts as part of duty cycle. Valid range [16-7680]ms.
  // rampDownMS = Time for the LED to dim to off. Does not count as part of duty cycle. Valid range [16-7680]ms.
  bool RGB::blinkOn(uint16_t periodMs, uint8_t dutyCycle, uint16_t rampUpMs, uint16_t rampDownMs)
  {
    if (!m_inited)
    {
      return false;
    }

    // Configure flash cycle.
    uint8_t fp = (m_regCache[1] & 0x80) | convertFlashPeriod(periodMs);
    uint16_t rampCombined = convertRampTime(rampUpMs, rampDownMs);
    // Set ramp scaling from the lower 2 bits of the high byte.
    uint8_t reg0 = (m_regCache[0] & 0x9F) | ((rampCombined >> 8) << 5);
    bool out = true;
    out &= writeReg(0, reg0);
    out &= writeReg(1, fp);
    out &= writeReg(2, dutyCycle);
    out &= writeReg(5, (uint8_t)rampCombined);

    // Switch LED to timer.
    out &= writeReg(4, 0xAA);

    return out;
  }
} // namespace rgb2026