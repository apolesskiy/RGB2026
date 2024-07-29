#pragma once
#include "rgb2026.h"

// This is a very simple reference implementation of I2CWrapper backed by the
// Adafruit_BusIO library. Define the RGB2026_I2C_BUSIO flag and add
// adafruit/Adafruit_BusIO (https://github.com/adafruit/Adafruit_BusIO) to your
// project to use.

// This is an HPP so we can pick up the #define below if it's defined in
// e.g. a .ino instead of at the project level.

#ifdef RGB2026_I2C_BUSIO
#include "Adafruit_I2CDevice.h"
#include "Adafruit_I2CRegister.h"

namespace rgb2026
{

  class KTD2026BusIO : public I2CWrapper
  {

  private:
    Adafruit_I2CDevice m_dev;
    // Unrolled bc there is no good way to init these.
    Adafruit_BusIO_Register m_reg0;
    Adafruit_BusIO_Register m_reg1;
    Adafruit_BusIO_Register m_reg2;
    Adafruit_BusIO_Register m_reg3;
    Adafruit_BusIO_Register m_reg4;
    Adafruit_BusIO_Register m_reg5;
    Adafruit_BusIO_Register m_reg6;
    Adafruit_BusIO_Register m_reg7;
    Adafruit_BusIO_Register m_reg8;
    Adafruit_BusIO_Register m_reg9;
    Adafruit_BusIO_Register *m_registers[10];

  public:
    KTD2026BusIO(uint8_t addr)
        : m_dev(addr)
        , m_reg0(&m_dev, 0, 1, LSBFIRST)
        , m_reg1(&m_dev, 1, 1, LSBFIRST)
        , m_reg2(&m_dev, 2, 1, LSBFIRST)
        , m_reg3(&m_dev, 3, 1, LSBFIRST)
        , m_reg4(&m_dev, 4, 1, LSBFIRST)
        , m_reg5(&m_dev, 5, 1, LSBFIRST)
        , m_reg6(&m_dev, 6, 1, LSBFIRST)
        , m_reg7(&m_dev, 7, 1, LSBFIRST)
        , m_reg8(&m_dev, 8, 1, LSBFIRST)
        , m_reg9(&m_dev, 9, 1, LSBFIRST)
        , m_registers{&m_reg0, &m_reg1, &m_reg2, &m_reg3, &m_reg4, &m_reg5, &m_reg6, &m_reg7, &m_reg8, &m_reg9}
    {}

    bool begin()
    {
      if (!m_dev.begin()) {
        return false;
      }
      return true;
    }

    bool regWrite(uint8_t index, uint8_t data)
    {
      if (index > 9) {
        return false;
      }
      return m_registers[index]->write(data, 1);
    }
  };
} // namespace rgb2026

#endif // RGB2026_I2C_BUSIO