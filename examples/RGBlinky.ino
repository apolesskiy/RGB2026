#include <Arduino.h>

// Define which color LEDs are connected to which channels in the circuit.
// KTD2026 has three channels, KTD2027 has four.
#define RGB2026_R RGB2026_D1
#define RGB2026_G RGB2026_D2
#define RGB2026_B RGB2026_D3
#define RGB2026_K RGB2026_D4

// Use the BusIO implementation.
#define RGB2026_I2C_BUSIO
#include "rgb2026.h"
#include "i2cwrapper_busio.hpp"

// Set up I2C communication to address 0x30 (KTD2026/KTD2027).
rgb2026::KTD2026BusIO ktd2026(rgb2026::kI2CAddr);
rgb2026::RGB rgb(&ktd2026);

void setup () {
  Serial.begin(9600);

  if (!rgb.begin()) {
    Serial.println("Failed to init RGB!");
    while(1);
  }
}

void loop() {
  Serial.println("Start test");
  Serial.print("set ramp: ");
  Serial.println(rgb.setRamp(rgb2026::eRamp::LINEAR));

  // Test full on.
  Serial.print("all on - set color: ");
  Serial.println(rgb.setColor(255, 255, 255, 255));
  Serial.print("all on: ");
  Serial.println(rgb.on());
  delay(2000);
  Serial.print("all off: ");
  Serial.println( rgb.off());
  delay(500);

  // Test D1.
  Serial.print("red: ");
  Serial.println(rgb.setColor(255, 0, 0, 0));
  Serial.println(rgb.blinkOn(1000, 128, 250, 500));
  delay(2500);

  // Test D2.
  Serial.print("green: ");
  Serial.println(rgb.setColor(0, 255, 0, 0));
  Serial.println(rgb.blinkOn(500, 192, 128, 10));
  delay(2500);

  // Test D3.
  Serial.print("blue: ");
  Serial.println(rgb.setColor(0, 0, 255, 0));
  Serial.println(rgb.blinkOn(1000, 128, 500, 250));
  delay(2500);

  // Test D4.
  Serial.print("white: ");
  Serial.println(rgb.setColor(0, 0, 0, 255));
  Serial.println(rgb.blinkOn(500, 128, 250, 250));
  delay(2500);
}