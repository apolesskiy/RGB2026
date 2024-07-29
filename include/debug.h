#pragma once
// Set the DEBUG build flag to access debug info.

#ifdef DEBUG
#ifdef ARDUINO
#include <Arduino.h>
#define DEBUGPRINT(x) Serial.print(x);
#define DEBUGPRINTHEX(x) Serial.print(x, HEX);
#define DEBUGPRINTLN(x) Serial.println(x);
#else
#include <iostream>
#define DEBUGPRINT(x) std::cout << x;
#define DEBUGPRINTHEX(x) std::cout << std::hex << x << std::dec;
#define DEBUGPRINTLN(x) std::cout << x << std::endl;
#endif
#else  // DEBUG
#define DEBUGPRINT(x)
#define DEBUGPRINTHEX(x)
#define DEBUGPRINTLN(x)
#endif  // DEBUG
