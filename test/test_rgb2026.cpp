#include "gtest/gtest.h"
#include <iostream>
#include "rgb2026.h"


struct I2CCaptureWrapper : public rgb2026::I2CWrapper {
  uint8_t m_register[10];

  void setRegisters(uint8_t val) {
    for(uint8_t i = 0; i < 10; i++) {
        m_register[i] = val;
    }
  }

  bool begin() {
    return true;
  }

  bool regWrite(uint8_t index, uint8_t data) {
    m_register[index] = data;
    return true;
  }
};

TEST(RGB, Uninited) {
  I2CCaptureWrapper cw;
  rgb2026::RGB r(&cw);
  EXPECT_FALSE(r.setRamp(rgb2026::eRamp::LINEAR));
  EXPECT_FALSE(r.setColor(0, 0, 0, 0));
  EXPECT_FALSE(r.on());
  EXPECT_FALSE(r.blinkOn(1000));
  EXPECT_FALSE(r.off());
}

TEST(RGB, BeginWritesDefaults) {
  I2CCaptureWrapper cw;
  cw.setRegisters(0xEE);
  rgb2026::RGB r(&cw);
  EXPECT_TRUE(r.begin());
  EXPECT_EQ(0b00011000,cw.m_register[0]);
  EXPECT_EQ(0b00000000,cw.m_register[1]);
  EXPECT_EQ(0b10000000,cw.m_register[2]);
  EXPECT_EQ(0b10000000,cw.m_register[3]);
  EXPECT_EQ(0b00000000,cw.m_register[4]);
  EXPECT_EQ(0b00000000,cw.m_register[5]);
  EXPECT_EQ(0b00000000,cw.m_register[6]);
  EXPECT_EQ(0b00000000,cw.m_register[7]);
  EXPECT_EQ(0b00000000,cw.m_register[8]);
  EXPECT_EQ(0b00000000,cw.m_register[9]);
}

TEST(RGB, SetRamp) {
  I2CCaptureWrapper cw;
  cw.setRegisters(0xEE);
  rgb2026::RGB r(&cw);
  ASSERT_TRUE(r.begin());
  EXPECT_TRUE(r.setRamp(rgb2026::eRamp::LINEAR));
  EXPECT_EQ(0x80, cw.m_register[1]);
  EXPECT_TRUE(r.setRamp(rgb2026::eRamp::S_CURVE));
  EXPECT_EQ(0x00, cw.m_register[1]);
}

TEST(RGB, SetColor) {
  I2CCaptureWrapper cw;
  cw.setRegisters(0xEE);
  rgb2026::RGB r(&cw);
  ASSERT_TRUE(r.begin());
  EXPECT_TRUE(r.setColor(64, 128, 192, 255));
  EXPECT_EQ(48U,  cw.m_register[6]);
  EXPECT_EQ(96U,  cw.m_register[7]);
  EXPECT_EQ(144U, cw.m_register[8]);
  EXPECT_EQ(192U, cw.m_register[9]);
}

TEST(RGB, SetColorHex) {
  I2CCaptureWrapper cw;
  cw.setRegisters(0xEE);
  rgb2026::RGB r(&cw);
  ASSERT_TRUE(r.begin());
  EXPECT_TRUE(r.setColor(0xFF4080C0));
  EXPECT_EQ(48U,  cw.m_register[6]);
  EXPECT_EQ(96U,  cw.m_register[7]);
  EXPECT_EQ(144U, cw.m_register[8]);
  EXPECT_EQ(192U, cw.m_register[9]);
}

TEST(RGB, SetColorHexRGB) {
  I2CCaptureWrapper cw;
  cw.setRegisters(0xEE);
  rgb2026::RGB r(&cw);
  ASSERT_TRUE(r.begin());
  EXPECT_TRUE(r.setColor(0x4080C0));
  EXPECT_EQ(48U,  cw.m_register[6]);
  EXPECT_EQ(96U,  cw.m_register[7]);
  EXPECT_EQ(144U, cw.m_register[8]);
  EXPECT_EQ(0U,   cw.m_register[9]);
}

TEST(RGB, on) {
  I2CCaptureWrapper cw;
  cw.setRegisters(0xEE);
  rgb2026::RGB r(&cw);
  ASSERT_TRUE(r.begin());
  EXPECT_TRUE(r.on());
  EXPECT_EQ(0x55,  cw.m_register[4]);
}

TEST(RGB, off) {
  I2CCaptureWrapper cw;
  cw.setRegisters(0xEE);
  rgb2026::RGB r(&cw);
  ASSERT_TRUE(r.begin());
  EXPECT_TRUE(r.off());
  EXPECT_EQ(0U,  cw.m_register[4]);
}

TEST(RGB, blinkOn) {
  I2CCaptureWrapper cw;
  cw.setRegisters(0xEE);
  rgb2026::RGB r(&cw);
  ASSERT_TRUE(r.begin());
  // Set ramp to make sure blink does not clobber the ramp bit.
  ASSERT_TRUE(r.setRamp(rgb2026::eRamp::LINEAR));
  EXPECT_TRUE(r.blinkOn(1024, 64, 16, 128));
  EXPECT_EQ(0x78,  cw.m_register[0]);
  EXPECT_EQ(0x86,  cw.m_register[1]);
  EXPECT_EQ(0x40,  cw.m_register[2]);
  EXPECT_EQ(0xAA, cw.m_register[4]);
  EXPECT_EQ(0x81, cw.m_register[5]);
}

#ifdef TEST
#ifdef ARDUINO
#ifdef ESP32 // AVR can't use GoogleTest
#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
  ::testing::InitGoogleTest();
}

void loop()
{
  if (RUN_ALL_TESTS()) {}
}
#endif // ESP32
#else // ARDUINO
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  if (RUN_ALL_TESTS()){}
  return 0;
}
#endif // ARDUINO
#endif // TEST